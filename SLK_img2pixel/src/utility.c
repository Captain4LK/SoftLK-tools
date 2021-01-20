/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
#include <SLK/SLK.h>
#define CUTE_FILES_IMPLEMENTATION
#include "../../external/cute_files.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"
//-------------------------------------

//Internal includes
#include "utility.h"
#include "process.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static uint8_t find_palette(SLK_Color in, SLK_Palette *pal);
static char input_dir[256];
static char output_dir[256];
//-------------------------------------

//Function implementations

void image_save(const char *path, SLK_RGB_sprite *img, SLK_Palette *pal)
{
   if(img==NULL||path==NULL)
      return;

   cf_file_t file; //Not ment to be used this way, but since it's possible, who cares
   strcpy(file.name,path);

   //slk file
   if(strcmp(cf_get_ext(&file),".slk")==0)
   {
      SLK_Pal_sprite *p = SLK_pal_sprite_create(img->width,img->height);
      for(int i = 0;i<p->width*p->height;i++)
      {
         p->data[i].mask = img->data[i].a==0?255:0;
         p->data[i].index = find_palette(img->data[i],pal);
      }
      SLK_pal_sprite_save(path,p,0);
      SLK_pal_sprite_destroy(p);

      return;
   }

   //anything else --> png
   SLK_rgb_sprite_save(path,img);
}

SLK_RGB_sprite *image_load(const char *path)
{
   unsigned char *data = NULL;
   int width = 1;
   int height = 1;
   SLK_RGB_sprite *out;

   data = stbi_load(path,&width,&height,NULL,4);
   if(data==NULL)
   {
      printf("Failed to load %s\n",path);
      return SLK_rgb_sprite_create(1,1);
   }

   out = SLK_rgb_sprite_create(width,height);
   memcpy(out->data,data,width*height*sizeof(*out->data));

   stbi_image_free(data);

   return out;
}

static uint8_t find_palette(SLK_Color in, SLK_Palette *pal)
{
   for(int i = 0;i<pal->used;i++)
      if(pal->colors[i].r==in.r&&pal->colors[i].g==in.g&&pal->colors[i].b==in.b)
         return i;

   return 0;
}

void dir_input_select(const char *path)
{
   if(path==NULL)
   {
      input_dir[0] = '\0';
      return;
   }
   strcpy(input_dir,path);
}

void dir_output_select(const char *path, int dither_mode, int sample_mode, int width, int height, SLK_Palette *pal)
{
   if(path==NULL)
   {
      output_dir[0] = '\0';
      return;
   }
   strcpy(output_dir,path);

   if(output_dir[0]!='\0'&&input_dir[0]!='\0') //Process directory
   {
      cf_dir_t dir;
      cf_dir_open(&dir,input_dir);
      SLK_RGB_sprite *out = SLK_rgb_sprite_create(width,height);
      while (dir.has_next)
      {
         cf_file_t file;
         cf_read_file(&dir, &file);
         if(strcmp(file.name,".")&&strcmp(file.name,".."))
         {
            char tmp[516];
            sprintf(tmp,"%s/%s",input_dir,file.name);
            SLK_RGB_sprite *in = image_load(tmp);
            process_image(in,out,pal,sample_mode,dither_mode);
            sprintf(tmp,"%s/%s.png",output_dir,file.name);
            image_save(tmp,out,pal);
            SLK_rgb_sprite_destroy(in);
         }
         cf_dir_next(&dir);
      }
      cf_dir_close(&dir);
      SLK_rgb_sprite_destroy(out);
   }
}
//-------------------------------------
