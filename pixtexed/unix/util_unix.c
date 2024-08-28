/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../external/tinyfiledialogs.h"
//-------------------------------------

//Internal includes
//-------------------------------------

//#defines
#define PATH_MAX 1024
#define PATH_EXT 32
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static char path_image_load[PATH_MAX] = {0};
static char path_image_save[PATH_MAX] = {0};
//-------------------------------------

//Function prototypes
static int path_pop_ext(const char *path, char *out, char *ext);
//-------------------------------------

//Function implementations

void image_load_select(char path[1024], char ext[32])
{
   const char *filter_patterns[2] = {"*.png"};
   const char *file_path = tinyfd_openFileDialog("Select a file",path_image_load,0,filter_patterns,NULL,0);

   if(file_path!=NULL)
   {
      strncpy(path_image_load,file_path,PATH_MAX-1);
      strncpy(path,file_path,1023);
      path_image_load[PATH_MAX-1] = '\0';
      path[1023] = '\0';
      path_pop_ext(file_path,NULL,ext);
   }
}

void image_save_select(char path[1024], char ext[32])
{
   const char *filter_patterns[4] = {"*.pcx"};
   const char *file_path = tinyfd_saveFileDialog("Save image",path_image_save,1,filter_patterns,NULL);

   //memset(path_image,0,sizeof(path_image));
   if(file_path!=NULL)
   {
      strncpy(path_image_save,file_path,PATH_MAX-1);
      strncpy(path,file_path,1023);
      path_image_save[PATH_MAX-1] = '\0';
      path[1023] = '\0';
      path_pop_ext(file_path, NULL, ext);
   }
}

static int path_pop_ext(const char *path, char *out, char *ext)
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

      strncpy(out,path,PATH_MAX-1);
      out[PATH_MAX-1] = '\0';
      return (int)strlen(out);
   }

   //slash after dot --> no extension
   if(last_dot[1]=='/'||last_dot[1]=='\\')
   {
      if(out==NULL)
         return 0;

      strncpy(out,path,PATH_MAX-1);
      out[PATH_MAX-1] = '\0';
      return (int)strlen(out);
   }

   if(ext!=NULL)
   {
      strncpy(ext,last_dot+1,PATH_EXT-1);
      ext[PATH_EXT-1] = '\0';
   }

   if(out==NULL)
      return 0;
   intptr_t len_copy = (intptr_t)(last_dot-path);
#define min(a,b) ((a)<(b)?(a):(b))
   strncpy(out,path,min(len_copy,PATH_MAX-1));
   out[min(len_copy,PATH_MAX-1)] = '\0';
#undef min
   return (int)strlen(out);
}
//-------------------------------------
