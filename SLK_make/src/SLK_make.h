/*
SLK_make - a build-system

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _SLK_MAKE_H_

#define _SLK_MAKE_H_

enum
{
   BUILD_DEBUG = 1 << 0,
   BUILD_OPT = 1 << 1,
   BUILD_PROF = 1 << 2,
   BUILD_CLEAN = 1 << 3,
   BUILD_BACKUP = 1 << 4,
   BUILD_LAST = BUILD_BACKUP
};


typedef enum
{
   OS_WIN32,
   OS_LINUX
} os_type;

enum
{
   WIN32_CONSOLE_APP,
   WIN32_WINDOWED_APP
};


// global options
typedef struct
{
   int show_deps;
   int build_flags;
   int continue_on_error;
   int no_libs;
   int verbose, quiet;
   int unix_libs_are_shared;
   int no_compile;
   int no_tmp;
   int no_syms;
   int show_includes;

   char *project_file;
   char *target_name;
   char *tmp_dir;
   list *targets_to_build;
   list *targets_built;
   char slash_char;
   os_type os;

} mk_options_struct;

list mk_global_defines;

// options dependant on each target
typedef struct
{
   char *def_file;

   char *target;
   char *target_type;
   char *outdir;

   list *dlls, *libs, *src, *inc, *defines;


   int app_type;
   char *cc_flags, *link_flags;
} mk_target_struct;

int mk_is_unix(mk_options_struct mk);
void mk_get(mk_options_struct * mk, int argc, char **argv);
void mk_reset(mk_target_struct * mk);
char *get_abs_path(char *filename, int force);

extern mk_options_struct mk_options;

#endif
