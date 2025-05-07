/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//wchar_t my ass
//whats utf8? - microshaft
//windows is the only reason this file exists

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>

#include "HLH_gui.h"
#include "HLH_json.h"

#include "../external/fopen_utf8.h"
#include "../external/nfd.h"
//-------------------------------------

//Internal includes
#include "../img2pixel.h"
#include "../util.h"
//-------------------------------------

//#defines
#define PATH_MAX 1024
#define PATH_EXT 32
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static char path_image_load[512] = {0};
static char path_palette_load[512] = {0};
static char path_preset_load[512] = {0};
static char path_image_save[512] = {0};
static char path_palette_save[512] = {0};
static char path_preset_save[512] = {0};
static char path_dir_input[512] = {0};
static char path_dir_output[512] = {0};

static char path_image[512] = {0};
static char path_palette[512] = {0};
static char path_preset[512] = {0};

static int gui_scale = 1;
//-------------------------------------

//Function prototypes
static int slk_path_pop_ext(const char *path, char *out, char *ext);
static int path_pop(const char* path, char* out, char* pop);
static int path_is_slash(char c);
//-------------------------------------

//Function implementations

FILE *image_load_select()
{
   NFD_Init();
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_OpenDialogU8(&file_path,NULL,0,path_image_load);

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return NULL;
   }

   FILE *f = NULL;
   if(file_path!=NULL)
   {
      path_pop(file_path,path_image_load,NULL);
      path_image_load[511] = '\0';
      f = fopen_utf8(file_path,"rb");
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return f;
}

FILE *palette_load_select(char ext[512])
{
   NFD_Init();

   nfdu8filteritem_t filter_item[4] = {{"pal", "pal"},{"PNG","png"},{"gpl","gpl"},{"hex","hex"}};
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_OpenDialogU8(&file_path,filter_item,4,path_palette_load);

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return NULL;
   }

   FILE *f = NULL;

   memset(path_palette,0,sizeof(path_palette));
   if(file_path!=NULL)
   {
      //strncpy(path_palette_load,file_path,511);
      path_pop(file_path,path_palette_load,NULL);
      path_palette_load[511] = '\0';
      slk_path_pop_ext(file_path,NULL,ext);
      f = fopen_utf8(file_path,"r");
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return f;
}

//const char *preset_load_select()
FILE *preset_load_select()
{
   NFD_Init();

   nfdu8filteritem_t filter_item[1] = {{"JSON", "json"}};
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_OpenDialogU8(&file_path,filter_item,1,path_preset_load);
   //const char *filter_patterns[2] = {"*.json"};
   //const char *file_path = tinyfd_openFileDialog("Select a preset",path_preset_load,1,filter_patterns,NULL,0);

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return NULL;
   }

   FILE *f = NULL;
   memset(path_preset,0,sizeof(path_preset));
   if(file_path!=NULL)
   {
      //strncpy(path_preset_load,file_path,511);
      path_pop(file_path,path_preset_load,NULL);
      path_preset_load[511] = '\0';
      f = fopen_utf8(file_path,"r");
      //strncpy(path_preset,file_path,511);
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return f;
}

//const char *image_save_select()
//FILE *image_save_select(char ext[512])
void image_save_select(char path[1024], char ext[512])
{
   NFD_Init();
   
   nfdu8filteritem_t filter_item[] = {{"PNG", "png"}, {"BMP", "bmp"},{"TGA","tga"},{"PCX","pcx"}};
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_SaveDialogU8(&file_path,filter_item,4,path_image_save,"untitled.png");
   //const char *filter_patterns[3] = {"*.png","*.bmp","*.tga"};
   //const char *file_path = tinyfd_saveFileDialog("Save image",path_image_save,3,filter_patterns,NULL);

   path[0] = '\0';
   ext[0] = '\0';
   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      //return NULL;
   }

   FILE *f = NULL;
   memset(path_image,0,sizeof(path_image));
   if(file_path!=NULL)
   {
      //strncpy(path_image_save,file_path,511);
      path_pop(file_path,path_image_save,NULL);
      path_image_save[511] = '\0';
      strncpy(path,file_path,1023);
      path[1023] = '\0';
      //f = fopen_utf8(file_path,"wb");
      //strncpy(path_image,file_path,511);
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   //return f;
}

FILE *palette_save_select(char ext[512])
{
   //const char *filter_patterns[] = {"*.pal","*.hex","*.gpl"};
   //const char *file_path = tinyfd_saveFileDialog("Save palette",path_palette_save,3,filter_patterns,NULL);
   NFD_Init();

   nfdu8filteritem_t filter_item[] = {{"pal", "pal"},{"hex","hex"},{"gpl","gpl"}};
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_SaveDialogU8(&file_path,filter_item,3,path_palette_save,"untitled.pal");

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return NULL;
   }

   FILE *f = NULL;
   memset(path_palette,0,sizeof(path_palette));
   if(file_path!=NULL)
   {
      //strncpy(path_palette_save,file_path,511);
      path_pop(file_path,path_palette_save,NULL);
      path_palette_save[511] = '\0';
      f = fopen_utf8(file_path,"w");
      //strncpy(path_palette,file_path,511);
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return f;
}

FILE *preset_save_select()
{
   //const char *filter_patterns[2] = {"*.json"};
   //const char *file_path = tinyfd_saveFileDialog("Save preset",path_preset_save,1,filter_patterns,NULL);
   NFD_Init();

   nfdu8filteritem_t filter_item[1] = {{"JSON", "json"}};
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_SaveDialogU8(&file_path,filter_item,1,path_preset_save,"untitled.json");

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return NULL;
   }

   FILE *f = NULL;
   memset(path_preset,0,sizeof(path_preset));
   if(file_path!=NULL)
   {
      //strncpy(path_preset_save,file_path,511);
      path_pop(file_path,path_preset_save,NULL);
      path_preset_save[511] = '\0';
      f = fopen_utf8(file_path,"w");
      //strncpy(path_preset,file_path,511);
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return f;

   //return path_preset;
}

void dir_input_select(char path[512])
{
   NFD_Init();

   nfdu8char_t *path_dir = NULL;
   nfdresult_t result = NFD_PickFolderU8(&path_dir,path_dir_input);

   puts(path_dir);
   if(result!=NFD_OKAY)
   {
      path[0] = '\0';
   }
   else
   {
      strncpy(path,path_dir,512);
      path[511] = '\0';
      strncpy(path_dir_input,path_dir,512);
      path_dir_input[511] = '\0';
      //strncpy(path_dir_input,path,512);
      //convert_utf8_to_wchar(input_dir,512,path);
      NFD_FreePathU8(path_dir);
   }

   NFD_Quit();
}

void dir_output_select(char path[512])
{
   NFD_Init();

   nfdu8char_t *path_dir = NULL;
   nfdresult_t result = NFD_PickFolderU8(&path_dir,path_dir_input);

   if(result!=NFD_OKAY)
   {
      path[0] = '\0';
   }
   else
   {
      strncpy(path,path_dir,512);
      path[511] = '\0';
      strncpy(path_dir_output,path_dir,512);
      path_dir_output[511] = '\0';
      //strncpy(path_dir_input,path,512);
      //convert_utf8_to_wchar(input_dir,512,path);
      NFD_FreePathU8(path_dir);
   }

   NFD_Quit();
}

void settings_load(const char *path)
{
   FILE *f = fopen(path,"r");
   if(f==NULL)
      return;

   HLH_json5_root *root = HLH_json_parse_file_stream(f);

   strncpy(path_image_load,HLH_json_get_object_string(&root->root,"path_image_load",""),511);
   strncpy(path_palette_load,HLH_json_get_object_string(&root->root,"path_palette_load",""),511);
   strncpy(path_preset_load,HLH_json_get_object_string(&root->root,"path_preset_load",""),511);
   strncpy(path_image_save,HLH_json_get_object_string(&root->root,"path_image_save",""),511);
   strncpy(path_palette_save,HLH_json_get_object_string(&root->root,"path_palette_save",""),511);
   strncpy(path_preset_save,HLH_json_get_object_string(&root->root,"path_preset_save",""),511);
   strncpy(path_dir_input,HLH_json_get_object_string(&root->root,"path_dir_input",""),511);
   strncpy(path_dir_output,HLH_json_get_object_string(&root->root,"path_dir_output",""),511);
   gui_scale = HLH_json_get_object_integer(&root->root,"gui_scale",1);
   path_image_load[511] = '\0';
   path_palette_load[511] = '\0';
   path_preset_load[511] = '\0';
   path_image_save[511] = '\0';
   path_palette_save[511] = '\0';
   path_preset_save[511] = '\0';
   path_dir_input[511] = '\0';
   path_dir_output[511] = '\0';
   HLH_gui_set_scale(gui_scale);

   HLH_json_free(root);
   fclose(f);
}

void settings_save()
{
   FILE *f = fopen("settings.json","w");
   if(f==NULL)
      return;

   HLH_json5_root *root = HLH_json_create_root();
   HLH_json_object_add_string(&root->root,"path_image_load",path_image_load);
   HLH_json_object_add_string(&root->root,"path_palette_load",path_palette_load);
   HLH_json_object_add_string(&root->root,"path_preset_load",path_preset_load);
   HLH_json_object_add_string(&root->root,"path_image_save",path_image_save);
   HLH_json_object_add_string(&root->root,"path_palette_save",path_palette_save);
   HLH_json_object_add_string(&root->root,"path_preset_save",path_preset_save);
   HLH_json_object_add_string(&root->root,"path_dir_input",path_dir_input);
   HLH_json_object_add_string(&root->root,"path_dir_output",path_dir_output);
   HLH_json_object_add_integer(&root->root,"gui_scale",gui_scale);

   HLH_json_write_file(f,&root->root);
   HLH_json_free(root);

   fclose(f);
}

static int slk_path_pop_ext(const char *path, char *out, char *ext)
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
      return strlen(out);
   }

   //slash after dot --> no extension
   if(last_dot[1]=='/'||last_dot[1]=='\\')
   {
      if(out==NULL)
         return 0;

      strncpy(out,path,PATH_MAX-1);
      out[PATH_MAX-1] = '\0';
      return strlen(out);
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
   return strlen(out);
}

//pak_path_pop, pak_path_is_slash
//by RandyGaul (https://github.com/RandyGaul), cute_headers (https://github.com/RandyGaul/cute_headers/blob/master/cute_path.h)
//Original license info:
/*
   ------------------------------------------------------------------------------
   This software is available under 2 licenses - you may choose the one you like.
   ------------------------------------------------------------------------------
   ALTERNATIVE A - zlib license
   Copyright (c) 2017 Randy Gaul http://www.randygaul.net
   This software is provided 'as-is', without any express or implied warranty.
   In no event will the authors be held liable for any damages arising from
   the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:
     1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
     2. Altered source versions must be plainly marked as such, and must not
        be misrepresented as being the original software.
     3. This notice may not be removed or altered from any source distribution.
   ------------------------------------------------------------------------------
   ALTERNATIVE B - Public Domain (www.unlicense.org)
   This is free and unencumbered software released into the public domain.
   Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
   software, either in source code form or as a compiled binary, for any purpose,
   commercial or non-commercial, and by any means.
   In jurisdictions that recognize copyright laws, the author or authors of this
   software dedicate any and all copyright interest in the software to the public
   domain. We make this dedication for the benefit of the public at large and to
   the detriment of our heirs and successors. We intend this dedication to be an
   overt act of relinquishment in perpetuity of all present and future rights to
   this software under copyright law.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   ------------------------------------------------------------------------------
*/

static int path_pop(const char* path, char* out, char* pop)
{
   const char *original = path;
   int total_len = 0;
   int len = 0;
   int pop_len = 0; //length of substring to be popped

   while(*path)
   {
      ++total_len;
      ++path;
   }

   //ignore trailing slash from input path
   if(path_is_slash(*(path - 1)))
   {
      --path;
      total_len -= 1;
   }

   while(!path_is_slash(*--path)&&pop_len!=total_len)
      ++pop_len;
   len = total_len - pop_len; //ength to copy

   //don't ignore trailing slash if it is the first character
   if(len>1)
      len--;

   if(len>0)
   {
      if(out)
      {
         strncpy(out, original, len);
         out[len] = 0;
      }

      if(pop)
      {
         strncpy(pop, path + 1, pop_len);
         pop[pop_len] = 0;
      }

      return len;
   }
   else
   {
      if(out)
      {
         out[0] = '.';
         out[1] = 0;
      }
      if(pop)
         *pop = 0;

      return 1;
   }
}

static int path_is_slash(char c)
{
   return (c=='/') | (c=='\\');
}
//cute_path END
//-------------------------------------
