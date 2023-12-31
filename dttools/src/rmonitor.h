/*
Copyright (C) 2022 The University of Notre Dame This software is
distributed under the GNU General Public License.  See the file
COPYING for details.
*/

#ifndef __RMONITOR_H
#define __RMONITOR_H

#include "rmsummary.h"
#include "rmonitor_types.h"

#define RESOURCE_MONITOR_ENV_VAR "CCTOOLS_RESOURCE_MONITOR"


/** Wraps a command line with the resource monitor.
The command line is rewritten to be run inside the monitor with
the corresponding log file options.
@param monitor_path Path to the resource monitor. If NULL, it is located automatically.
@param template The filename template for all the log files.
@param limits resource limits value. If a resource specifies -1, no enforcement is done for that resource. NULL if no limits are specified.
@param summary Generate <template>.summary if not 0.
@param time_series Generate <template>.series if not 0.
@param opened_files Generate <template>.files if not 0.
@return A wrapper command line for string_wrap_command to wrap original command line with the resource monitor.
*/

char *resource_monitor_write_command(const char *monitor_path, const char *template_filename, const struct rmsummary *limits, const char *extra_monitor_options, int debug_output, int time_series, int inotify_stats, const char *measure_dir);

/** Looks for a resource monitor executable, and makes a copy in
current working directory.
The resource monitor executable is searched, in order, in the following locations: the path given as an argument, the value of the environment variable RESOURCE_MONITOR_ENV_VAR, the current working directory, the cctools installation directory. The copy is deleted when the current process exits.
@param path_from_cmdline The first path to look for the resource monitor executable.
@return The name of the monitor executable in the current working directory.
*/

char *resource_monitor_copy_to_wd(const char *path_from_cmdline);

/**  Reads a single resources file from filename **/
struct rmsummary *resident_monitor_parse_resources_file(const char *filename);

/** Looks for the path of the resource_monitor executable.
@param path_from_cmdline The first path to look for the resource monitor executable.
@return The path of the monitor executable.
*/
char *resource_monitor_locate(const char *path_from_cmdline);

#endif
