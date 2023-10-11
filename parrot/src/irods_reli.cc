/*
Copyright (C) 2003-2004 Douglas Thain and the University of Wisconsin
Copyright (C) 2022 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file COPYING for details.
*/

#ifdef HAS_IRODS

#ifdef IRODS_USES_HPP
#include "rodsClient.hpp"
#include "rodsPath.hpp"
#include "miscUtil.hpp"
#ifdef IRODS_USES_ERROR_HPP
#include "rodsErrorTable.h"
#endif
#ifdef IRODS_USES_PLUGINS
#include "irods_plugin_home_directory.hpp"
#endif
#else
#include "rodsClient.h"
#include "rodsPath.h"
#include "miscUtil.h"
#endif

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "irods_reli.h"

#include "hash_table.h"
#include "debug.h"
#include "path.h"
#include "sleeptools.h"
#include "stringtools.h"
}

#include <string>

extern int pfs_irods_debug_level;

static struct hash_table *connect_cache = 0;
static rodsEnv irods_env;
static int got_irods_env = 0;
static int got_irods_passwd_file = 0;
static int irods_serial = 0;

/* iRODS considers paths ending in a forward slash to be invalid and throws an error. */
#define CLEAN(path) \
char clean_##path[PATH_MAX];\
snprintf(clean_##path, PATH_MAX, "%s", path);\
path_remove_trailing_slashes(clean_##path);\
path = clean_##path;

struct irods_file {
	char *host;
	char *path;
	int fd;
	int flags;
	INT64_T offset;
	int serial;
};

struct irods_server {
	rcComm_t *conn;
	time_t lastused;
	int serial;
};

#define DISCONNECT_TIMEOUT 300
#define STOPTIME_DEFAULT (time(NULL)+150)
#define MIN_DELAY 1
#define MAX_DELAY 60

/*
Convert an irods error into a Unix errno.
Unfortunately, the routine getUnixErrno doesn't
interpret error codes that were not generated by Unix in the first place,
so if it is out of range, default to EIO "I/O Error".
*/

static int irods_reli_errno( int e )
{
	int unix_errno;

	if(e==CAT_NO_ROWS_FOUND) {
		unix_errno = ENOENT;
	} else {
		unix_errno = getErrno(e);
		if(unix_errno>32) {
			debug(D_NOTICE,"irods returned unexpected error code %d",e);
			unix_errno = EIO;
		}
	}

	return unix_errno;
}

static void invalidate_host (const char *hostport, struct irods_server *server)
{
	if (server) {
		debug(D_IRODS, "discarding stale connection to %s", hostport);
		rcDisconnect(server->conn);
		free(server);
		if (connect_cache)
			hash_table_remove(connect_cache, hostport);
	}
}

static struct irods_server * connect_to_host( const char *hostport )
{
	struct irods_server *server;
	rcComm_t *conn;
	rErrMsg_t errmsg;
	char host[PFS_PATH_MAX];
	int port;

	if(!hostport[0]) {
		errno = EACCES;
		return 0;
	}

	if(!got_irods_passwd_file) {

		if(!getenv("HOME")) {
			debug(D_NOTICE, "Environment variable HOME is not set.");
			debug(D_NOTICE, "Need $HOME/.irods/.irodsA to access iRODS resources.");
			return 0;
		}

		std::string passwd_file(getenv("HOME"));
		passwd_file += "/.irods/.irodsA";
		if(access(passwd_file.c_str(),R_OK)!=0) {
			debug(D_NOTICE,"Could not find password file %s", passwd_file.c_str());
			debug(D_NOTICE,"Did you run iinit to access your iRODS resources?");
			return 0;
		}

		got_irods_passwd_file = 1;
	}

	if(!got_irods_env) {
		rodsLogLevel(pfs_irods_debug_level);
		if(getRodsEnv(&irods_env)<0) {
			debug(D_IRODS,"couldn't load irods environment!");
			return 0;
		}
		got_irods_env = 1;
	}


#ifdef IRODS_USES_PLUGINS
	static int did_plugin_warning = 0;

	if(!did_plugin_warning) {
		std::string plugin_home(irods::PLUGIN_HOME);
		if(strlen(irods_env.irodsPluginHome) > 0)
			plugin_home = irods_env.irodsPluginHome;

		std::string plugin_path = plugin_home + "network/libtcp.so";
		if(access(plugin_path.c_str(),R_OK)!=0) {
			debug(D_NOTICE,"warning: irods expects plugins installed in %s", plugin_home.c_str());
			debug(D_NOTICE,"warning: could not find %s",plugin_path.c_str());
			debug(D_NOTICE,"warning: please update irods_plugin_home in your ~/.irods/irods_environment.json configuration file");
		}
		did_plugin_warning = 1;
	}

#endif

	if(!connect_cache) connect_cache = hash_table_create(0,0);

	time_t current = time(0);

	server = (struct irods_server*) hash_table_lookup(connect_cache,hostport);
	if(server) {
		if((current-server->lastused)>DISCONNECT_TIMEOUT) {
			invalidate_host(hostport, server);
		} else {
			server->lastused = current;
			return server;
		}
	}

	debug(D_IRODS,"connecting to %s",hostport);

	sscanf(hostport,"%[^:]:%d",host,&port);

	conn = rcConnect(host,port,irods_env.rodsUserName,irods_env.rodsZone,1,&errmsg);
	if(!conn) {
		/* iRODS returns EINPROGRESS when the server closes the connection during connect. */
		if(errno==EINPROGRESS) {
			errno = ECONNREFUSED;
		}
		debug(D_IRODS,"couldn't connect: %s\n",strerror(errno));
		return 0;
	}

	debug(D_IRODS,"logging in...");

	if(clientLogin(conn)!=0) {
		debug(D_IRODS,"unable to login as %s",irods_env.rodsUserName);
		rcDisconnect(conn);
		return 0;
	}

	server = (struct irods_server *) malloc(sizeof(*server));
	server->conn = conn;
	server->lastused = current;
	server->serial = irods_serial++;

	hash_table_insert(connect_cache,hostport,server);

	return server;
}

#define RETRY_PATH_START \
	{\
		int delay = 0;\
		time_t nexttry;\
		time_t current;\
		time_t stoptime = STOPTIME_DEFAULT;\
		while(1) {\
			struct irods_server *server = connect_to_host(host);\
			if(server) {

#define RETRY_PATH_END(onerror,dircheck) \
				if(result>=0)\
					break;\
				else {\
					errno = irods_reli_errno(result);\
					if (errno == ECONNRESET || errno == EINPROGRESS || errno == EPIPE) {\
						/* iRODS returns EINPROGRESS when the server closes the connection during connect. */\
						invalidate_host(host, server);\
					} else if (errno == EAGAIN) {\
						; /* fall through */\
					} else if(dircheck) {\
						if (result==CAT_NO_ROWS_FOUND) {\
							break; /* empty dir */\
						} else {\
							errno = ENOTDIR;\
							return onerror;\
						}\
					} else {\
						return onerror;\
					}\
				}\
			} else {\
				if(errno==ENOENT) return onerror;\
				if(errno==EPERM) return onerror;\
				if(errno==EACCES) return onerror;\
			}\
			if(time(0)>=stoptime) {\
				errno = ECONNRESET;\
				return onerror;\
			}\
			if(delay>=2) debug(D_NOTICE,"couldn't connect to %s: still trying...\n",host);\
			debug(D_IRODS,"couldn't talk to %s: %s\n",host,strerror(errno));\
			current = time(0);\
			nexttry = MIN(stoptime,current+delay);\
			debug(D_IRODS,"try again in %d seconds\n",(int)(nexttry-current));\
			sleep_until(nexttry);\
			if(delay==0) {\
				delay = 1;\
			} else {\
				delay = MIN(delay*2,MAX_DELAY);\
			}\
		}\
	}

int irods_reli_stat ( const char *host, const char *path, struct pfs_stat *info )
{
	int result;
	dataObjInp_t request;
	rodsObjStat_t *response = 0;
	CLEAN(path)

	/* Special case: /irods looks like an empty dir */
	if(!host[0]) {
		memset(info,0,sizeof(*info));
		info->st_ino = 2;
		info->st_dev = -1;
		info->st_mode = S_IFDIR | 0755;
		info->st_size = 4096;
		info->st_blocks = 8;
		info->st_nlink = 1;
		return 0;
	}

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.objPath,path);

		debug(D_IRODS,"rcObjStat %s %s",host,path);
		result = rcObjStat (server->conn,&request,&response);
		debug(D_IRODS, "= %d", result);
	RETRY_PATH_END(-1,0)

	memset(info,0,sizeof(*info));

	info->st_ino = hash_string(path);
	info->st_dev = -1;

	if (response->objType == COLL_OBJ_T) {
		info->st_mode = S_IFDIR | 0755;
		info->st_size = 4096;
		info->st_blocks = 8;
		info->st_ctime = atoi (response->createTime);
		info->st_mtime = atoi (response->modifyTime);
		info->st_nlink = 1;
	} else {
		info->st_mode = S_IFREG | 0755;
		info->st_size = response->objSize;
		info->st_blocks = info->st_size/512+1;
		info->st_nlink = 1;
	}

	info->st_blksize = 4096;
	info->st_ctime = atoi(response->createTime);
	info->st_mtime = atoi(response->modifyTime);
	info->st_atime = atoi(response->modifyTime);

	free(response);

	return 0;
}

int irods_reli_getdir( const char *host, const char *path, void (*callback) ( const char *path, void *arg ), void *arg )
{
	int i;
	int result;
	int keepgoing;
	struct irods_server *server_query = NULL;
	CLEAN(path)

	queryHandle_t query_handle;
	genQueryInp_t query_in;
	genQueryOut_t *query_out = 0;
	sqlResult_t *value;

	/* Special case: /irods looks like an empty dir */
	if(!host[0]) {
		callback(".",arg);
		callback("..",arg);
		return 0;
	}

	RETRY_PATH_START
		/* First we set up a query for file names */
		memset(&query_in,0,sizeof(query_in));
		query_out = 0;

		rclInitQueryHandle(&query_handle,server->conn);

		debug(D_IRODS,"queryDataObjInColl %s %s",host,path);
		result = queryDataObjInColl(&query_handle,(char*)path,0,&query_in,&query_out,0);
		debug(D_IRODS, "= %d", result);
		server_query = server;
	RETRY_PATH_END(-1,1)

	callback(".",arg);
	callback("..",arg);

	while(result>=0) {
		value = getSqlResultByInx(query_out,COL_DATA_NAME);
		if(!value) break;

		char *subname = value->value;

		for(i=0;i<query_out->rowCnt;i++) {
			callback(subname,arg);
			subname += value->len;
		}

		keepgoing = query_out->continueInx;
		freeGenQueryOut(&query_out);

		if(keepgoing>0) {
			query_in.continueInx = keepgoing;
			debug(D_IRODS,"rcGenQuery %s",host);
			result = rcGenQuery(server_query->conn,&query_in,&query_out);
			debug(D_IRODS,"= %d",result);
		} else {
			break;
		}
	}

	/* Now we set up a SECOND query for directory names */

	RETRY_PATH_START
		memset(&query_in,0,sizeof(query_in));
		query_out = 0;

		rclInitQueryHandle(&query_handle,server->conn);

		debug(D_IRODS,"queryCollInColl %s %s",host,path);
		result = queryCollInColl(&query_handle,(char*)path,0,&query_in,&query_out);
		debug(D_IRODS, "= %d", result);
		server_query = server;
	RETRY_PATH_END(-1,1)

	while(result>=0) {
		value = getSqlResultByInx(query_out,COL_COLL_NAME);
		if(!value) break;

		char *subname = value->value;

		for(i=0;i<query_out->rowCnt;i++) {
			callback(subname,arg);
			subname += value->len;
		}

		keepgoing = query_out->continueInx;
		freeGenQueryOut(&query_out);

		if(keepgoing>0) {
			query_in.continueInx = keepgoing;
			debug(D_IRODS,"rcGenQuery %s",host);
			result = rcGenQuery(server_query->conn,&query_in,&query_out);
			debug(D_IRODS,"= %d",result);
		} else {
			break;
		}
	}

	return 0;
}

int irods_reli_statfs   ( const char *host, const char *path, struct pfs_statfs *info )
{
	memset(info,0,sizeof(*info));

	info->f_blocks = 10000000;
	info->f_bavail = 10000000;
	info->f_bsize  = 4096;
	info->f_bfree  = 10000000;
	info->f_files  = 10000000;
	info->f_ffree  = 10000000;

	return 0;
}

int irods_reli_mkdir ( const char *host, const char *path )
{
	collInp_t request;
	int result;
	CLEAN(path)

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.collName,path);

		debug(D_IRODS,"rcCollCreate %s %s",host,path);
		result = rcCollCreate(server->conn,&request);
		debug(D_IRODS, "= %d", result);
	RETRY_PATH_END(-1,0)

	return result;
}

int irods_reli_unlink( const char *host, const char *path )
{
	{
		/* Note that an irods unlink will fail silently if you attempt to
		 * remove a directory. So, first we must examine the type and fail
		 * explicitly if it is a dir.
		 */
		struct pfs_stat info;
		int result = irods_reli_stat(host,path,&info);
		if(result<0)
			return -1;
		else if(S_ISDIR(info.st_mode))
			return errno = EISDIR, -1;
	}

	dataObjInp_t request;
	int result;
	CLEAN(path)

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.objPath,path);

		addKeyVal (&request.condInput, (char*)FORCE_FLAG_KW, (char*)"");

		debug(D_IRODS,"rcUnlink %s %s",host,path);
		result = rcDataObjUnlink(server->conn,&request);
		debug(D_IRODS, "= %d", result);

		clearKeyVal (&request.condInput);
	RETRY_PATH_END(-1,0)

	return result;
}

int irods_reli_rmdir ( const char *host, const char *path )
{
	{
		/* Note that an irods rmdir will fail silently if you attempt to remove
		 * a file. So, first we must examine the type and fail explicitly if it
		 * is a dir.
		 */
		struct pfs_stat info;
		int result = irods_reli_stat(host,path,&info);
		if(result<0)
			return -1;
		else if(!S_ISDIR(info.st_mode))
			return errno = ENOTDIR, -1;
	}

	collInp_t request;
	int result;
	CLEAN(path)

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.collName,path);
		addKeyVal (&request.condInput, (char*)FORCE_FLAG_KW, (char*)"");

		debug(D_IRODS,"rcRmColl %s %s",host,path);
		result = rcRmColl(server->conn,&request,0);
		debug(D_IRODS, "= %d", result);

		clearKeyVal (&request.condInput);
	RETRY_PATH_END(-1,0)

	return result;
}

int irods_reli_rename   ( const char *host, const char *path, const char *newpath )
{
	/* rcDataObjRename does not have the Unix semantics of atomically
	   destroying the target file.  So, we must do it ourselves.
	*/

	irods_reli_unlink(host,newpath);

	dataObjCopyInp_t request;
	int result;
	CLEAN(path)
	CLEAN(newpath)

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.srcDataObjInp.objPath,path);
		strcpy(request.destDataObjInp.objPath,newpath);
		request.srcDataObjInp.oprType  = RENAME_DATA_OBJ;
		request.destDataObjInp.oprType = RENAME_DATA_OBJ;

		debug(D_IRODS,"rcDataObjRename %s %s %s",host,path,newpath);
		result = rcDataObjRename(server->conn,&request);
		debug(D_IRODS, "= %d", result);
	RETRY_PATH_END(-1,0)

	return result;
}

int irods_reli_truncate ( const char *host, const char *path, INT64_T length )
{
	dataObjInp_t request;
	int result;
	CLEAN(path)

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.objPath,path);
		request.dataSize = length;

		debug(D_IRODS,"rcDataObjTruncate %s %s",host,path);
		result = rcDataObjTruncate(server->conn,&request);
		debug(D_IRODS, "= %d", result);
	RETRY_PATH_END(-1,0)

	return 0;
}

int irods_reli_getfile ( const char *host, const char *path, const char *local_path )
{
	dataObjInp_t request;
	int result;
	CLEAN(path)

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.objPath,path);

		request.numThreads = 0; // server chooses threads
		request.openFlags = O_RDONLY;

		addKeyVal (&request.condInput, (char*)FORCE_FLAG_KW, (char*)"");

		debug(D_IRODS,"rcDataObjGet %s %s %s",host,path,local_path);
		result = rcDataObjGet(server->conn,&request,(char*) local_path);
		debug(D_IRODS, "= %d", result);

		clearKeyVal(&request.condInput);
	RETRY_PATH_END(-1,0)

	return 0;
}

int irods_reli_putfile ( const char *host, const char *path, const char *local_path )
{
	dataObjInp_t request;
	int result;
	CLEAN(path)

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.objPath,path);

		request.numThreads = 0; // server chooses threads
		request.openFlags = O_CREAT|O_WRONLY;

		addKeyVal (&request.condInput, (char*)FORCE_FLAG_KW, (char*)"");
		addKeyVal (&request.condInput, (char*)ALL_KW, (char*)"");

		debug(D_IRODS,"rcDataObjPut %s %s %s",host,path,local_path);
		result = rcDataObjPut(server->conn,&request,(char*)local_path);
		debug(D_IRODS, "= %d", result);

		clearKeyVal (&request.condInput);
	RETRY_PATH_END(-1,0)

	return 0;
}

static unsigned char hex_to_nybble( char h )
{
	switch(h) {
		case '0':	return 0;
		case '1':	return 1;
		case '2':	return 2;
		case '3':	return 3;
		case '4':	return 4;
		case '5':	return 5;
		case '6':	return 6;
		case '7':	return 7;
		case '8':	return 8;
		case '9':	return 9;
		case 'a':	return 10;
		case 'b':	return 11;
		case 'c':	return 12;
		case 'd':	return 13;
		case 'e':	return 14;
		case 'f':	return 15;
		default:	return 0;
	}
}

static unsigned char hex_to_byte( char hex[2] )
{
	return (hex_to_nybble(hex[0])<<4) | hex_to_nybble(hex[1]);

}

int irods_reli_md5( const char *host, const char *path, char *digest )
{
	dataObjInp_t request;
	int result;
	char *str = 0;
	CLEAN(path)

	RETRY_PATH_START
		memset(&request,0,sizeof(request));
		strcpy(request.objPath,path);

		debug(D_IRODS,"rcDataObjChksum %s %s",host,path);
		result = rcDataObjChksum(server->conn,&request,&str);
		debug(D_IRODS, "= %d", result);
	RETRY_PATH_END(-1,0)

	/* rcDataObjChecksum returns the checksum as ASCII */
	/* but Parrot is expecting 16 bytes of binary data */
	int i;

	for(i=0;i<16;i++) {
		digest[i] = hex_to_byte(&str[2*i]);
	}

	debug(D_IRODS,"%s",str);
	free(str);
	return 0;
}

struct irods_file *irods_reli_open ( const char *host, const char *path, int flags, int mode )
{
	struct irods_server *server_file = NULL;
	int result;
	CLEAN(path)

	RETRY_PATH_START
		dataObjInp_t request;
		memset(&request,0,sizeof(request));
		strcpy(request.objPath,path);
		request.openFlags = flags;

		debug(D_IRODS,"rcDataObjOpen %s %s",host,path);
		result = rcDataObjOpen (server->conn,&request);
		debug(D_IRODS,"= %d",result);

		if(result<0 && flags&O_CREAT) {
			memset(&request,0,sizeof(request));
			strcpy(request.objPath,path);
			request.createMode = mode;
			request.openFlags = flags;
			request.dataSize = -1;

			addKeyVal (&request.condInput, (char*)DATA_TYPE_KW, (char*)"generic");

			if(irods_env.rodsDefResource[0]) {
				addKeyVal (&request.condInput, (char*)RESC_NAME_KW, irods_env.rodsDefResource);
			}

			debug(D_IRODS,"rcDataObjCreate %s %s",host,path);
			result = rcDataObjCreate (server->conn,&request);
			debug(D_IRODS,"= %d",result);
		}
		server_file = server;

		/* An attempt to open a directory returns CAT_NO_ROWS_FOUND. For Unix
		 * compatibility, we must check for a directory by that name, and return
		 * EISDIR if it exists.
		 */
		if(result==CAT_NO_ROWS_FOUND) {
			struct pfs_stat info;
			if(irods_reli_stat(host,path,&info)>=0) {
				if(S_ISDIR(info.st_mode)) {
					errno = EISDIR;
					return 0;
				}
			}
		}
	RETRY_PATH_END(NULL,0)

	if(flags&O_TRUNC) {
		irods_reli_truncate(host,path,0);
	}

	struct irods_file *file = (struct irods_file *) malloc(sizeof(*file));
	file->host = strdup(host);
	file->path = strdup(path);
	file->fd = result;
	file->offset = 0;
	file->serial = server_file->serial;
	file->flags = flags;

	return file;
}

#define RETRY_FILE_START \
	{\
		int delay = 0;\
		time_t nexttry;\
		time_t current;\
		time_t stoptime = STOPTIME_DEFAULT;\
		const char *host = file->host;\
		while(1) {\
			struct irods_server *server = connect_to_host(host);\
			if(server) {\
				if (connect_to_file(server, file) == -1) {\
					/* fall through to error catching */\
				} else {

#define RETRY_FILE_END \
				}\
				RETRY_PATH_END(-1,0)

static int connect_to_file( struct irods_server *server, struct irods_file *file )
{
	dataObjInp_t request;
	int result;

	if(server->serial == file->serial) return 0;

	memset(&request,0,sizeof(request));
	strcpy(request.objPath,file->path);
	request.openFlags = file->flags & ~(O_CREAT|O_TRUNC|O_EXCL);

	debug(D_IRODS,"rcDataObjOpen %s %s",file->host,file->path);
	result = rcDataObjOpen(server->conn,&request);
	debug(D_IRODS,"= %d",result);

	if (result >= 0) {
		file->fd = result;
		file->offset = 0;
		file->serial = server->serial;
		return 0;
	} else {
		return result;
	}
}

static int irods_reli_lseek_if_needed( struct irods_file *file, INT64_T offset )
{
	openedDataObjInp_t request;
	fileLseekOut_t *response=0;
	int result;

	RETRY_FILE_START
		if(file->offset==offset) return 0;
		memset(&request,0,sizeof(request));
		request.l1descInx = file->fd;
		request.offset  = offset;
		request.whence  = SEEK_SET;

		debug(D_IRODS,"rcDataObjLseek %s %d %lld",file->host,file->fd,(long long)offset);
		result = rcDataObjLseek(server->conn,&request,&response);
		debug(D_IRODS,"= %d",result);
	RETRY_FILE_END

	free(response);
	file->offset = offset;
	return 0;
}

int irods_reli_pread ( struct irods_file *file, char *data, int length, INT64_T offset )
{
	int result;
	openedDataObjInp_t request;
	bytesBuf_t       response;


	RETRY_FILE_START
		if(irods_reli_lseek_if_needed(file,offset) == -1) return -1;

		memset(&request,0,sizeof(request));
		request.l1descInx = file->fd;
		request.len = length;

		response.buf = data;
		response.len = length;

		debug(D_IRODS,"rcDataObjRead %s %d %d",file->host,file->fd,length);
		result = rcDataObjRead(server->conn,&request,&response);
		debug(D_IRODS,"= %d",result);
	RETRY_FILE_END

	/*
	The iRODS API semantics have changed!
	In 3.x, rcDataObjRead would use the pointer supplied.
	In 4.x, rcDataObjRead allocates a buffer that must be freed.
	*/

	if(result>0 && data!=response.buf) {
		memcpy(data,response.buf,response.len);
	}

	if(result<0) {
		errno = irods_reli_errno(result);
		return -1;
	}

	file->offset += result;

	return result;
}

int irods_reli_pwrite    ( struct irods_file *file, const char *data, int length, INT64_T offset )
{
	int result;
	openedDataObjInp_t request;
	bytesBuf_t        response;


	RETRY_FILE_START
		if(irods_reli_lseek_if_needed(file,offset)<0) return -1;

		memset(&request,0,sizeof(request));
		request.l1descInx = file->fd;
		request.len = length;

		response.buf = (char*)data;
		response.len = length;

		debug(D_IRODS,"rcDataObjWrite %s %d %d",file->host,file->fd,length);
		result = rcDataObjWrite(server->conn,&request,&response);
		debug(D_IRODS,"= %d",result);
	RETRY_FILE_END

	file->offset += result;

	return result;
}

int irods_reli_close    ( struct irods_file *file )
{
	int result;
	openedDataObjInp_t request;

	RETRY_FILE_START
		memset(&request,0,sizeof(request));
		request.l1descInx = file->fd;

		debug(D_IRODS,"rcDataObjClose %s %d",file->host,file->fd);
		result = rcDataObjClose(server->conn,&request);
		debug(D_IRODS,"= %d",result);
	RETRY_FILE_END

	free(file->host);
	free(file->path);
	free(file);

	return 0;
}

#endif

/* vim: set noexpandtab tabstop=8: */