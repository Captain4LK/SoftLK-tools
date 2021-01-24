/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
#include <SLK/SLK.h>
#define FOPEN_UTF8_IMPLEMENTATION
#include "../../external/fopen_utf8.h"
#define CUTE_FILES_IMPLEMENTATION
#include "../../external/cute_files.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"
#include "../../external/gifdec.h"
#include "../../external/gifenc.h"
#include "../../external/tinyfiledialogs.h"
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
static char input_dir[256];
static char output_dir[256];
static char input_gif[256];
static char output_gif[256];
//-------------------------------------

//Function prototypes
static uint8_t find_palette(SLK_Color in, SLK_Palette *pal);
//-------------------------------------

//Function implementations

SLK_RGB_sprite *image_select()
{
   const char *filter_patterns[2] = {"*.png"};
   const char *file_path = tinyfd_openFileDialog("Select a file","",0,filter_patterns,NULL,0);
   return image_load(file_path);
}

void image_write(SLK_RGB_sprite *img, SLK_Palette *pal)
{
   const char *filter_patterns[2] = {"*.png","*.slk"};
   const char *file_path = tinyfd_saveFileDialog("Save image","",2,filter_patterns,NULL);
   image_save(file_path,img,pal);
}

FILE *json_select()
{
   const char *filter_patterns[2] = {"*.json"};
   const char *file_path = tinyfd_openFileDialog("Select a preset","",1,filter_patterns,NULL,0);
   if(file_path!=NULL)
      return fopen(file_path,"r");
   return NULL;
}

FILE *json_write()
{
   const char *filter_patterns[2] = {"*.json"};
   const char *file_path = tinyfd_saveFileDialog("Save preset","",1,filter_patterns,NULL);
   if(file_path!=NULL)
      return fopen(file_path,"w");
   return NULL;
}

SLK_Palette *palette_select()
{
   const char *filter_patterns[2] = {"*.pal"};
   const char *file_path = tinyfd_openFileDialog("Load a palette","",1,filter_patterns,NULL,0);
   if(file_path!=NULL)
      return SLK_palette_load(file_path);
   return NULL;
}

void palette_write(SLK_Palette *pal)
{
   const char *filter_patterns[2] = {"*.pal"};
   const char *file_path = tinyfd_saveFileDialog("Save palette","",1,filter_patterns,NULL);
   if(file_path!=NULL)
      SLK_palette_save(file_path,pal);
}

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

void image_save_w(const wchar_t *path, SLK_RGB_sprite *img, SLK_Palette *pal)
{

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

void dir_input_select()
{
   const char *path = tinyfd_selectFolderDialog("Select input directory",NULL);
   if(path==NULL)
   {
      input_dir[0] = '\0';
      return;
   }
   strcpy(input_dir,path);
}

void dir_output_select(int dither_mode, int sample_mode, int scale_mode, int width, int height, SLK_Palette *pal)
{
   const char *path = tinyfd_selectFolderDialog("Select output directory",NULL);
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
      SLK_RGB_sprite *out = NULL;
      if(scale_mode==0)
         out = SLK_rgb_sprite_create(width,height);

      while (dir.has_next)
      {
         cf_file_t file;
         cf_read_file(&dir, &file);
         if(strcmp(file.name,".")&&strcmp(file.name,".."))
         {
            char tmp[516];
            sprintf(tmp,"%s/%s",input_dir,file.name);
            SLK_RGB_sprite *in = image_load(tmp);
            sharpen_image(in,in);
            if(in!=NULL)
            {
               if(scale_mode==1)
                  out = SLK_rgb_sprite_create(in->width/width,in->height/height);
               process_image(in,out,pal,sample_mode,dither_mode);
               sprintf(tmp,"%s/%s.png",output_dir,file.name);
               image_save(tmp,out,pal);
               if(scale_mode==1)
                  SLK_rgb_sprite_destroy(out);
               SLK_rgb_sprite_destroy(in);
            }
         }
         cf_dir_next(&dir);
      }
      cf_dir_close(&dir);
      if(scale_mode==0)
         SLK_rgb_sprite_destroy(out);
   }
}

void gif_input_select()
{
   const char *filter_patterns[2] = {"*.gif"};
   const char *path = tinyfd_openFileDialog("Select a gif file","",1,filter_patterns,NULL,0);
   if(path==NULL)
   {
      input_gif[0] = '\0';
      return;
   }
   strcpy(input_gif,path);
}

void gif_output_select(int dither_mode, int sample_mode, int scale_mode, int width, int height, SLK_Palette *pal)
{
   const char *filter_patterns[2] = {"*.gif"};
   const char *path = tinyfd_saveFileDialog("Save gif","",1,filter_patterns,NULL);
   if(path==NULL)
   {
      output_gif[0] = '\0';
      return;
   }
   strcpy(output_gif,path);

   if(output_gif[0]!='\0'&&input_gif[0]!='\0') //Process directory
   {
      gd_GIF *gif = gd_open_gif(input_gif);
      if(!gif)
         return;
      uint8_t gif_palette[256*3] = {0};
      for(int i = 0;i<256;i++)
      {
         gif_palette[i*3] = pal->colors[i].r;
         gif_palette[i*3+1] = pal->colors[i].g;
         gif_palette[i*3+2] = pal->colors[i].b;
      }
      ge_GIF *gif_out;
      if(scale_mode==0)
         gif_out = ge_new_gif(output_gif,width,height,gif_palette,8,gif->loop_count);
      else
         gif_out = ge_new_gif(output_gif,gif->width/width,gif->height/height,gif_palette,8,gif->loop_count);
      uint8_t *frame = malloc(gif->width*gif->height*3);
      SLK_RGB_sprite *out = NULL;
      if(scale_mode==0)
         out = SLK_rgb_sprite_create(width,height);
      else
         out = SLK_rgb_sprite_create(gif->width/width,gif->height/height);
      SLK_RGB_sprite *in = SLK_rgb_sprite_create(gif->width,gif->height);

      while(gd_get_frame(gif))
      {
         gd_render_frame(gif,frame);
         for(int i = 0;i<gif->width*gif->height;i++)
         {
            in->data[i].r = frame[i*3];
            in->data[i].g = frame[i*3+1];
            in->data[i].b = frame[i*3+2];
            in->data[i].a = 255;
         }
         sharpen_image(in,in);
         process_image(in,out,pal,sample_mode,dither_mode);
         for(int i = 0;i<out->width*out->height;i++)
            gif_out->frame[i] = find_palette(out->data[i],pal);
         ge_add_frame(gif_out,gif->gce.delay);
      }

      free(frame);
      gd_close_gif(gif);
      ge_close_gif(gif_out);
      SLK_rgb_sprite_destroy(out);
      SLK_rgb_sprite_destroy(in);
   }
}
//-------------------------------------
