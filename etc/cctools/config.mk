# Generated at Mon Oct  2 13:52:48 EDT 2023 by jzhou24@cclws22.cse.nd.edu

CCTOOLS_HOME=/afs/crc.nd.edu/user/j/jzhou24/cctools
CCTOOLS_INSTALL_DIR=/afs/crc.nd.edu/user/j/jzhou24/cctools

CCTOOLS_OPSYS=LINUX

CCTOOLS_PACKAGES= dttools batch_job grow makeflow work_queue ftp_lite taskvine parrot resource_monitor chirp deltadb doc poncho

CCTOOLS_CC=@echo COMPILE $@;/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/x86_64-conda-linux-gnu-cc

CCTOOLS_BASE_CCFLAGS=-march=nocona -mtune=haswell -ftree-vectorize -fPIC -fstack-protector-strong -fno-plt -O2 -ffunction-sections -pipe -isystem /afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/include -D__EXTENSIONS__ -D_LARGEFILE64_SOURCE -D__LARGE64_FILES -Wall -Wextra -Wno-unused-result -fPIC -Wno-unused-parameter -Wno-unknown-pragmas -Wno-deprecated-declarations -Wno-unused-const-variable -DHAS_LIBREADLINE -DHAS_TLS_method -DHAS_OPENSSL -DHAS_PPOLL -DHAVE_GMTIME_R -DHAVE_FDATASYNC -DHAS_ISNAN -DHAVE_ISNAN -DSQLITE_HAVE_ISNAN -DHAVE_LOCALTIME_R -DHAS_OPENAT -DHAS_PREAD -DUSE_PREAD -DUSE_PREAD64 -DHAS_PWRITE -DUSE_PWRITE -DUSE_PWRITE64 -DHAVE_STRCHRNUL -DHAS_STRSIGNAL -DHAS_USLEEP -DHAVE_USLEEP -DHAS_UTIME -DHAVE_UTIME -DHAS_UTIMENSAT -DHAS_SYS_XATTR_H -DHAS_IFADDRS -DHAS_INTTYPES_H -DHAVE_INTTYPES_H -DHAS_STDINT_H -DHAVE_STDINT_H -DHAS_SYS_STATFS_H -DHAS_SYS_STATVFS_H -DHAS_SYS_VFS_H -DBUILD_DATE='"2023-10-02 13:52:47 -0400"' -DBUILD_HOST='"cclws22.cse.nd.edu"' -DBUILD_USER='"jzhou24"' -DCCTOOLS_COMMIT='""' -DCCTOOLS_CONFIGURE_ARGUMENTS='""' -DCCTOOLS_CPU_X86_64 -DCCTOOLS_CVMFS_BUILD_FLAGS='" "' -DCCTOOLS_OPSYS_LINUX -DCCTOOLS_RELEASE_DATE='"2023-10-02 13:52:47 -0400"' -DCCTOOLS_SOURCE='"DEVELOPMENT"' -DCCTOOLS_SYSTEM_INFORMATION='"Linux cclws22.cse.nd.edu 4.18.0-477.21.1.el8_8.x86_64 \#1 SMP Thu Jul 20 08:38:27 EDT 2023 x86_64 x86_64 x86_64 GNU/Linux"' -DCCTOOLS_VERSION='"8.0.0 DEVELOPMENT"' -DCCTOOLS_VERSION_MAJOR=8 -DCCTOOLS_VERSION_MICRO=0 -DCCTOOLS_VERSION_MINOR=0 -DINSTALL_PATH='"/afs/crc.nd.edu/user/j/jzhou24/cctools"' -D_GNU_SOURCE -D_REENTRANT -g

CCTOOLS_INTERNAL_CCFLAGS= -DCCTOOLS_WITH_CHIRP -I /afs/crc.nd.edu/user/j/jzhou24/cctools/dttools/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/batch_job/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/grow/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/makeflow/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/work_queue/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/ftp_lite/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/taskvine/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/parrot/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/resource_monitor/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/chirp/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/deltadb/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/poncho/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/taskvine/src/manager ${CCTOOLS_BASE_CCFLAGS} -std=c99

CCTOOLS_CCFLAGS=-I${CCTOOLS_INSTALL_DIR}/include/cctools ${CCTOOLS_BASE_CCFLAGS} -std=c99

CCTOOLS_CXX=@echo COMPILE $@;/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/x86_64-conda-linux-gnu-c++

CCTOOLS_BASE_CXXFLAGS=${CCTOOLS_BASE_CCFLAGS}

CCTOOLS_INTERNAL_CXXFLAGS= -DCCTOOLS_WITH_CHIRP -I /afs/crc.nd.edu/user/j/jzhou24/cctools/dttools/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/batch_job/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/grow/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/makeflow/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/work_queue/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/ftp_lite/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/taskvine/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/parrot/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/resource_monitor/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/chirp/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/deltadb/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/poncho/src -I /afs/crc.nd.edu/user/j/jzhou24/cctools/taskvine/src/manager ${CCTOOLS_BASE_CCFLAGS}

CCTOOLS_CXXFLAGS=-I${CCTOOLS_INSTALL_DIR}/include/cctools ${CCTOOLS_BASE_CCFLAGS}

CCTOOLS_LD = @echo LINK $@;/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/x86_64-conda-linux-gnu-cc

CCTOOLS_BASE_LDFLAGS = -Wl,-O2 -Wl,--sort-common -Wl,--as-needed -Wl,-z,relro -Wl,-z,now -Wl,--disable-new-dtags -Wl,--gc-sections -Wl,--allow-shlib-undefined -Wl,-rpath,/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -Wl,-rpath-link,/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -Xlinker -Bstatic -static-libgcc -Xlinker -Bdynamic -Xlinker --as-needed -Wl,--no-warn-search-mismatch -g

CCTOOLS_INTERNAL_LDFLAGS = $(CCTOOLS_BASE_LDFLAGS) 

CCTOOLS_EXTERNAL_LINKAGE = $(CCTOOLS_OPENSSL_STATIC_LDFLAGS) -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib  -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -lresolv -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -lnsl -lrt -ldl -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -lz -lstdc++ -lpthread -lz -lc -lm
CCTOOLS_EXTERNAL_LINKAGE_NO_OPENSSL = -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib  -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -lresolv -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -lnsl -lrt -ldl -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -lz -lstdc++ -lpthread -lz -lc -lm

CCTOOLS_STATIC_LINKAGE = 

CCTOOLS_LDFLAGS = -L$(CCTOOLS_INSTALL_DIR)/lib $(CCTOOLS_BASE_LDFLAGS)

CCTOOLS_STATIC=

CCTOOLS_DYNAMIC_SUFFIX=so
CCTOOLS_DYNAMIC_FLAG=-shared

CC=/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/x86_64-conda-linux-gnu-cc
CCFLAGS=$(CCTOOLS_CCFLAGS)
LD=/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/x86_64-conda-linux-gnu-cc
LDFLAGS=$(CCTOOLS_LDFLAGS)
CXX=/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/x86_64-conda-linux-gnu-c++
CXXFLAGS=$(CCTOOLS_CXXFLAGS)

CCTOOLS_AR=ar

CCTOOLS_CHIRP=chirp

CCTOOLS_READLINE_AVAILABLE=yes
CCTOOLS_READLINE_LDFLAGS=-lreadline -Xlinker --no-as-needed -lncurses -lhistory -Xlinker --as-needed

CCTOOLS_SWIG_AVAILABLE=yes
CCTOOLS_SWIG=/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/swig
CCTOOLS_SWIG_TASKVINE_BINDINGS=python3 
CCTOOLS_SWIG_WORKQUEUE_BINDINGS=python3 
CCTOOLS_SWIG_CHIRP_BINDINGS=python3 
CCTOOLS_SWIG_RMONITOR_BINDINGS=python3 

CCTOOLS_PERL_AVAILABLE=no
CCTOOLS_PERL=
CCTOOLS_PERL_CCFLAGS=
CCTOOLS_PERL_LDFLAGS=
CCTOOLS_PERL_VERSION=
CCTOOLS_PERL_PATH=$(CCTOOLS_INSTALL_DIR)/lib//$(CCTOOLS_PERL_VERSION)

CCTOOLS_PYTHON_TEST_EXEC=/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/python3
CCTOOLS_PYTHON_TEST_DIR=python3

CCTOOLS_PYTHON2_AVAILABLE=no
CCTOOLS_PYTHON2=
CCTOOLS_PYTHON2_CCFLAGS=
CCTOOLS_PYTHON2_LDFLAGS=
CCTOOLS_PYTHON2_VERSION=
CCTOOLS_PYTHON2_PATH=$(CCTOOLS_INSTALL_DIR)/lib/python$(CCTOOLS_PYTHON2_VERSION)/site-packages

CCTOOLS_PYTHON3_AVAILABLE=yes
CCTOOLS_PYTHON3=/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/python3
CCTOOLS_PYTHON3_CCFLAGS=-DNDEBUG -I/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/include/python3.11 -IObjects -IInclude -IPython -I. -I/home/conda/feedstock_root/build_artifacts/python-split_1686419548788/work/Include -DNDEBUG -D_FORTIFY_SOURCE=2 -O2 -isystem /afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/include -I/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/include -DNDEBUG -D_FORTIFY_SOURCE=2 -O2 -isystem /afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/include -I/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/include
CCTOOLS_PYTHON3_LDFLAGS=-Wl,--allow-shlib-undefined -Wl,-rpath,/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -Wl,-rpath-link,/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -Wl,--allow-shlib-undefined -Wl,-rpath,/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -Wl,-rpath-link,/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib
CCTOOLS_PYTHON3_VERSION=3.11
CCTOOLS_PYTHON3_PATH=$(CCTOOLS_INSTALL_DIR)/lib/python$(CCTOOLS_PYTHON3_VERSION)/site-packages

CCTOOLS_PYDOC=/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/bin/pydoc

CCTOOLS_SGE_PARAMETERS=

CCTOOLS_DOCTARGETS=manpages htmlpages mdpages 

CCTOOLS_M4_ARGS=-DCCTOOLS_VERSION="8.0.0 DEVELOPMENT" -DCCTOOLS_RELEASE_DATE=""

CCTOOLS_BUILD_LIB64PARROT_HELPER=yes
CCTOOLS_BUILD_LIB32PARROT_HELPER=no

CCTOOLS_VERSION=8.0.0 DEVELOPMENT
CCTOOLS_RELEASEDATE=

CCTOOLS_IRODS_AVAILABLE=no
CCTOOLS_IRODS_LDFLAGS=
CCTOOLS_IRODS_CCFLAGS=

CCTOOLS_MYSQL_AVAILABLE=no
CCTOOLS_MYSQL_LDFLAGS=
CCTOOLS_MYSQL_CCFLAGS=

CCTOOLS_XROOTD_AVAILABLE=no
CCTOOLS_XROOTD_LDFLAGS=
CCTOOLS_XROOTD_CCFLAGS=

CCTOOLS_CVMFS_AVAILABLE=no
CCTOOLS_CVMFS_LDFLAGS=
CCTOOLS_CVMFS_CCFLAGS=

CCTOOLS_OPENSSL_AVAILABLE=yes
CCTOOLS_OPENSSL_LDFLAGS=-L/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib -lssl -lcrypto
CCTOOLS_OPENSSL_CCFLAGS=-I/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/include

CCTOOLS_OPENSSL_STATIC_AVAILABLE=yes
CCTOOLS_OPENSSL_STATIC_LDFLAGS=/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib/libssl.a /afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/lib/libcrypto.a
CCTOOLS_OPENSSL_STATIC_CCFLAGS=-I/afs/crc.nd.edu/user/j/jzhou24/miniconda3/envs/cctools-dev/include

CCTOOLS_EXT2FS_AVAILABLE=no
CCTOOLS_EXT2FS_LDFLAGS=
CCTOOLS_EXT2FS_CCFLAGS=

CCTOOLS_FUSE_AVAILABLE=no
CCTOOLS_FUSE_LDFLAGS=
CCTOOLS_FUSE_CCFLAGS=

CCTOOLS_MPI_AVAILABLE=no
CCTOOLS_MPI_LDFLAGS=
CCTOOLS_MPI_CCFLAGS=

CCTOOLS_CURL_AVAILABLE=no
CCTOOLS_CURL_LDFLAGS=
CCTOOLS_CURL_CCFLAGS=

CCTOOLS_GLOBUS_AVAILABLE=no
CCTOOLS_GLOBUS_LDFLAGS=
CCTOOLS_GLOBUS_CCFLAGS=

export CCTOOLS_TEST_CCFLAGS=
