/*
Path manipulation

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

/* 
   To create implementation (the function definitions) add
      #define HLH_PATH_IMPLEMENTATION
   before including this file in *one* C file (translation unit)
*/

/*
   malloc(), realloc() and free() can be overwritten by 
   defining the following macros:

   HLH_MALLOC
   HLH_FREE
   HLH_REALLOC
*/

#ifndef _HLH_PATH_H_

#define _HLH_PATH_H_

int HLH_path_pop_ext(const char *path, char *out, char *ext);

#endif

#ifdef HLH_PATH_IMPLEMENTATION
#ifndef HLH_PATH_IMPLEMENTATION_ONCE
#define HLH_PATH_IMPLEMENTATION_ONCE

#ifndef HLH_PATH_PATH_MAX
#define HLH_PATH_PATH_MAX 512
#endif

#ifndef HLH_PATH_EXT_MAX
#define HLH_PATH_EXT_MAX 32
#endif

#ifndef HLH_MALLOC
#define HLH_MALLOC malloc
#endif

#ifndef HLH_FREE
#define HLH_FREE free
#endif

#ifndef HLH_REALLOC
#define HLH_REALLOC realloc
#endif

int HLH_path_pop_ext(const char *path, char *out, char *ext)
{
   if(path==NULL)
      return 0;

   if(out!=NULL)
      out[0] = '\0';
   if(ext!=NULL)
      ext[0] = '\0';

   char *last_dot = strrchr(path,'.');

   //No dot, or string is '.' or '..' --> no extension
   if(last_dot==NULL||!strcmp(path,".")||!strcmp(path,".."))
   {
      if(out==NULL)
         return 0;

      strncpy(out,path,HLH_PATH_PATH_MAX-1);
      out[HLH_PATH_PATH_MAX-1] = '\0';
      return strlen(out);
   }

   //slash after dot --> no extension
   if(last_dot[1]=='/'||last_dot[1]=='\\')
   {
      if(out==NULL)
         return 0;

      strncpy(out,path,HLH_PATH_PATH_MAX-1);
      out[HLH_PATH_PATH_MAX-1] = '\0';
      return strlen(out);
   }

   if(ext!=NULL)
   {
      strncpy(ext,last_dot+1,HLH_PATH_EXT_MAX-1);
      ext[HLH_PATH_EXT_MAX-1] = '\0';
   }

   if(out==NULL)
      return 0;
   intptr_t len_copy = (intptr_t)(last_dot-path);
#define HLH_path_min(a,b) ((a)<(b)?(a):(b))
   strncpy(out,path,HLH_path_min(len_copy,HLH_PATH_PATH_MAX-1));
   out[HLH_path_min(len_copy,HLH_PATH_PATH_MAX-1)] = '\0';
#undef HLH_path_min
   return strlen(out);
}

#undef HLH_MALLOC
#undef HLH_FREE
#undef HLH_REALLOC

#endif
#endif
