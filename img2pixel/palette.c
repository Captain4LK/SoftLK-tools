/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "HLH_gui.h"
#include "HLH.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
//-------------------------------------

//#defines
#define PATH_MAX 1024
#define PATH_EXT 32
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static int slk_path_pop_ext(const char *path, char *out, char *ext);
static int chartoi(char in);
//-------------------------------------

//Function implementations

void SLK_palette_load(FILE *f, uint32_t *colors, int *color_count, const char *ext)
{
   if(f==NULL)
      return;

   //char ext[PATH_EXT];
   //slk_path_pop_ext(path,NULL,ext);

   if(strcmp(ext,"pal")==0)
   {
      //FILE *f = fopen(path,"r");
      //if(f==NULL)
         //return;

      int color = 0;
      char buffer[512];
      for(int i = 0;fgets(buffer,512,f)&&color<256;i++)
      {
         if(i<=2||buffer[0]=='\0')
            continue;

         uint32_t r,g,b;
         uint32_t a = 255;
         if(sscanf(buffer,"%u %u %u",&r,&g,&b)==3)
         {
            colors[color] = r|(g<<8)|(b<<16)|(a<<24);
            color++;
         }
      }

      *color_count = color;
      //fclose(f);
   }
   else if(strcmp(ext,"hex")==0)
   {
      //FILE *f = fopen(path,"r");
      //if(f==NULL)
         //return;

      char buffer[512];
      int color = 0;
      while(fgets(buffer,512,f)&&color<256)
      {
         uint32_t r = chartoi(buffer[0])*16+chartoi(buffer[1]);
         uint32_t g = chartoi(buffer[2])*16+chartoi(buffer[3]);
         uint32_t b = chartoi(buffer[4])*16+chartoi(buffer[5]);
         uint32_t a = 255;
         colors[color] = r|(g<<8)|(b<<16)|(a<<24);
         color++;
      }

      *color_count = color;
      //fclose(f);
   }
   else if(strcmp(ext,"gpl")==0)
   {
      //FILE *f = fopen(path,"r");
      //if(f==NULL)
         //return;

      char buffer[512];
      int color = 0;
      while(fgets(buffer,512,f)&&color<256)
      {
         if(buffer[0]=='#')
            continue;

         uint32_t r,g,b;
         uint32_t a = 255;
         if(sscanf(buffer,"%u %u %u",&r,&g,&b)==3)
         {
            colors[color] = r|(g<<8)|(b<<16)|(a<<24);
            color++;
         }
      }

      *color_count = color;
      //fclose(f);
   }
   else
   {
      //png or unknown: asume image
      int width,height;
      uint32_t *data = HLH_gui_image_load(f,&width,&height);
      *color_count = HLH_min(256,width*height);
      for(int i = 0;i<*color_count;i++)
         colors[i] = data[i];
      HLH_gui_image_free(data);
   }
}

void SLK_palette_save(FILE *f, uint32_t *colors, int color_count, const char *ext)
{
   if(f==NULL)
      return;

   //FILE *f = fopen(path,"w");
   //if(f==NULL)
      //return;

   //char ext[PATH_EXT];
   //slk_path_pop_ext(path,NULL,ext);

   if(strcmp(ext,"gpl")==0)
   {
      fprintf(f,"GIMP Palette\n");
      fprintf(f,"#Colors: %d\n",color_count);
      for(int i = 0;i<color_count;i++)
      {
         uint32_t r = SLK_color32_r(colors[i]);
         uint32_t g = SLK_color32_g(colors[i]);
         uint32_t b = SLK_color32_b(colors[i]);
         fprintf(f,"%d %d %d %x%x%x%x%x%x\n",r,g,b,(r>>4)&15,r&15,(g>>4)&15,g&15,(b>>4)&15,b&15);
      }
   }
   else if(strcmp(ext,"hex")==0)
   {
      for(int i = 0;i<color_count;i++)
      {
         uint32_t r = SLK_color32_r(colors[i]);
         uint32_t g = SLK_color32_g(colors[i]);
         uint32_t b = SLK_color32_b(colors[i]);
         fprintf(f,"%x%x%x%x%x%x\n",(r>>4)&15,r&15,(g>>4)&15,g&15,(b>>4)&15,b&15);
      }
   }
   //Asume pal
   else
   {
      fprintf(f,"JASC-PAL\n0100\n%d\n",color_count);
      for(int i = 0;i<color_count;i++)
      {
         uint32_t r = SLK_color32_r(colors[i]);
         uint32_t g = SLK_color32_g(colors[i]);
         uint32_t b = SLK_color32_b(colors[i]);
         fprintf(f,"%d %d %d\n",r,g,b);
      }
   }

   //fclose(f);
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

static int chartoi(char in)
{
   if(in>='0'&&in<='9')
      return in-'0';
   if(in>='a'&&in<='f')
      return in-'a'+10;
   if(in>='A'&&in<='F')
      return in-'A'+10;
   return 0;
}
//-------------------------------------
