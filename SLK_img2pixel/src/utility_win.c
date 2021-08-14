/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//wchar_t my ass
//whats utf8? - microshaft
//windows is the only reason this file exists

//External includes
#include <stdint.h>
#include <SLK/SLK.h>
#define FOPEN_UTF8_IMPLEMENTATION
#include "../../external/fopen_utf8.h"
#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"
#define CUTE_FILES_IMPLEMENTATION
#include "../../external/cute_files.h"
#include "../../external/gifdec.h"
#include "../../external/gifenc.h"
#include "../../external/tinyfiledialogs.h"
//-------------------------------------

//Internal includes
#include "utility.h"
#include "image2pixel.h"
//-------------------------------------

//#defines
#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static wchar_t input_dir[512];
static wchar_t output_dir[512];
static wchar_t input_gif[512];
static wchar_t output_gif[512];

int upscale = 1;
//-------------------------------------

//Function prototypes
static uint8_t find_palette(SLK_Color in, SLK_Palette *pal);
static void image_save_w(const wchar_t *path, SLK_RGB_sprite *img, SLK_Palette *pal);
static SLK_Palette *palette_png(FILE *f);
static SLK_Palette *palette_gpl(FILE *f);
static SLK_Palette *palette_hex(FILE *f);
static int chartoi(char in);
//-------------------------------------

//Function implementations

SLK_RGB_sprite *image_select()
{
   const wchar_t *filter_patterns[2] = {L"*.png"};
   const wchar_t *file_path = tinyfd_openFileDialogW(L"Select a file",L"",0,filter_patterns,NULL,0);
   if(file_path==NULL)
      return SLK_rgb_sprite_create(1,1);

   char buffer[512];
   stbi_convert_wchar_to_utf8(buffer,512,file_path);

   return image_load(buffer);
}

void image_write(SLK_RGB_sprite *img, SLK_Palette *pal)
{
   const wchar_t *filter_patterns[2] = {L"*.png",L"*.slk"};
   const wchar_t *file_path = tinyfd_saveFileDialogW(L"Save image",L"",2,filter_patterns,NULL);
   image_save_w(file_path,img,pal);
}

FILE *json_select()
{
   const wchar_t *filter_patterns[2] = {L"*.json"};
   const wchar_t *file_path = tinyfd_openFileDialogW(L"Select a preset",L"",1,filter_patterns,NULL,0);
   if(file_path==NULL)
      return NULL;

   char buffer[512];
   stbi_convert_wchar_to_utf8(buffer,512,file_path);
   if(buffer[0]!='\0')
      return fopen_utf8(buffer,"r");

   return NULL;
}

FILE *json_write()
{
   const wchar_t *filter_patterns[2] = {L"*.json"};
   const wchar_t *file_path = tinyfd_saveFileDialogW(L"Save preset",L"",1,filter_patterns,NULL);
   if(file_path==NULL)
      return NULL;

   char buffer[512];
   stbi_convert_wchar_to_utf8(buffer,512,file_path);
   if(buffer[0]!='\0')
      return fopen_utf8(buffer,"w");

   return NULL;
}

SLK_Palette *palette_select()
{
   const wchar_t *filter_patterns[] = {L"*.pal",L"*.png",L"*.gpl",L"*.hex"};
   const wchar_t *file_path = tinyfd_openFileDialogW(L"Load a palette",L"",4,filter_patterns,NULL,0);
   if(file_path==NULL)
      return NULL;

   char buffer[512];
   stbi_convert_wchar_to_utf8(buffer,512,file_path);
   if(buffer[0]!='\0')
   {
      SLK_Palette *p = NULL;
      FILE *f = fopen_utf8(buffer,"rb");
      if(f)
      {
         cf_file_t file; //Not ment to be used this way, but since it's possible, who cares
         strcpy(file.name,buffer);
         cf_get_ext(&file);

         if(strcmp(file.ext,".pal")==0)
            p = SLK_palette_load_file(f);
         else if(strcmp(file.ext,".png")==0)
            p = palette_png(f);
         else if(strcmp(file.ext,".gpl")==0)
            p = palette_gpl(f);
         else if(strcmp(file.ext,".hex")==0)
            p = palette_hex(f);
         
         fclose(f);
         return p;
      }
   }
   return NULL;
}

SLK_Palette *palette_load(const char *path)
{
   if(path!=NULL)
   {
      SLK_Palette *p = NULL;
      FILE *f = fopen(path,"rb");
      if(f)
      {
         cf_file_t file; //Not ment to be used this way, but since it's possible, who cares
         strcpy(file.name,path);
         cf_get_ext(&file);

         if(strcmp(file.ext,".pal")==0)
            p = SLK_palette_load_file(f);
         else if(strcmp(file.ext,".png")==0)
            p = palette_png(f);
         else if(strcmp(file.ext,".gpl")==0)
            p = palette_gpl(f);
         else if(strcmp(file.ext,".hex")==0)
            p = palette_hex(f);
         
         fclose(f);
         return p;
      }
   }
   return NULL;
}

void palette_write(SLK_Palette *pal)
{
   const wchar_t *filter_patterns[2] = {L"*.pal"};
   const wchar_t *file_path = tinyfd_saveFileDialogW(L"Save palette",L"",1,filter_patterns,NULL);
   if(file_path==NULL)
      return;

   char buffer[512];
   stbi_convert_wchar_to_utf8(buffer,512,file_path);
   if(buffer[0]!='\0')
   {
      FILE *f = fopen_utf8(buffer,"w");
      if(f)
      {
         SLK_palette_save_file(f,pal);
         fclose(f);
      }
   }
}

void image_save_w(const wchar_t *path, SLK_RGB_sprite *img, SLK_Palette *pal)
{
   if(img==NULL||path==NULL)
      return;

   char buffer[512];
   stbi_convert_wchar_to_utf8(buffer,512,path);
   cf_file_t file; //Not ment to be used this way, but since it's possible, who cares
   strcpy(file.name,buffer);

   //slk file
   if(strcmp(cf_get_ext(&file),".slk")==0)
   {
      SLK_Pal_sprite *p = SLK_pal_sprite_create(img->width,img->height);
      for(int i = 0;i<p->width*p->height;i++)
      {
         p->data[i] = find_palette(img->data[i],pal);
         if(!img->data[i].a)
            p->data[i] = 0;
      }
      FILE *f = fopen_utf8(buffer,"wb");
      SLK_pal_sprite_save_file(f,p,0);
      fclose(f);
      SLK_pal_sprite_destroy(p);

      return;
   }

   //anything else --> png
   SLK_RGB_sprite *tmp_up = SLK_rgb_sprite_create(upscale*img->width,upscale*img->height);
   for(int y = 0;y<img->height;y++)
      for(int x = 0;x<img->width;x++)
         for(int y_ = 0;y_<upscale;y_++)
            for(int x_ = 0;x_<upscale;x_++)
               SLK_rgb_sprite_set_pixel(tmp_up,x*upscale+x_,y*upscale+y_,SLK_rgb_sprite_get_pixel(img,x,y));
   FILE *f = fopen_utf8(buffer,"wb");
   SLK_rgb_sprite_save_file(f,tmp_up);
   fclose(f);
   SLK_rgb_sprite_destroy(tmp_up);
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
         p->data[i] = find_palette(img->data[i],pal);
         if(!img->data[i].a)
            p->data[i] = 0;
      }

      //Save image as smallest type
      FILE *in = fopen(path,"wb");
      SLK_pal_sprite_save_file(in,p,0);
      fclose(in); 
      SLK_pal_sprite_destroy(p);

      return;
   }

   //anything else --> png
   SLK_RGB_sprite *tmp_up = SLK_rgb_sprite_create(upscale*img->width,upscale*img->height);
   for(int y = 0;y<img->height;y++)
      for(int x = 0;x<img->width;x++)
         for(int y_ = 0;y_<upscale;y_++)
            for(int x_ = 0;x_<upscale;x_++)
               SLK_rgb_sprite_set_pixel(tmp_up,x*upscale+x_,y*upscale+y_,SLK_rgb_sprite_get_pixel(img,x,y));
   SLK_rgb_sprite_save(path,tmp_up);
   SLK_rgb_sprite_destroy(tmp_up);
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
   const wchar_t *path = tinyfd_selectFolderDialogW(L"Select input directory",NULL);
   if(path==NULL)
   {
      input_dir[0] = '\0';
      return;
   }
   wcscpy(input_dir,path);
}

void dir_output_select(int dither_mode, int sample_mode, int distance_mode, int scale_mode, int width, int height, SLK_Palette *pal)
{
   const wchar_t *path = tinyfd_selectFolderDialogW(L"Select output directory",NULL);
   if(path==NULL)
   {
      output_dir[0] = '\0';
      return;
   }
   wcscpy(output_dir,path);

   if(output_dir[0]!='\0'&&input_dir[0]!='\0') //Process directory
   {
      char buffer[512];
      stbi_convert_wchar_to_utf8(buffer,512,input_dir);
      cf_dir_t dir;
      cf_dir_open(&dir,buffer);
      SLK_RGB_sprite *out = NULL;
      if(scale_mode==0)
         out = SLK_rgb_sprite_create(width,height);

      while (dir.has_next)
      {
         cf_file_t file;
         cf_read_file(&dir, &file);
         if(strcmp(file.name,".")&&strcmp(file.name,".."))
         {
            char tmp[1028];
            sprintf(tmp,"%s/%s",buffer,file.name);
            SLK_RGB_sprite *in = image_load(tmp);
            img2pixel_lowpass_image(in,in);
            img2pixel_sharpen_image(in,in);
            if(in!=NULL)
            {
               if(scale_mode==1)
                  out = SLK_rgb_sprite_create(in->width/width,in->height/height);
               img2pixel_process_image(in,out);
               char buffer[512];
               stbi_convert_wchar_to_utf8(buffer,512,output_dir);
               sprintf(tmp,"%s/%s.png",buffer,file.name);
               wchar_t *wpath = (wchar_t *) utf8_to_utf16((const uint8_t *) tmp, NULL);
               image_save_w(wpath,out,pal);
               free(wpath);
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
   const wchar_t *filter_patterns[2] = {L"*.gif"};
   const wchar_t *path = tinyfd_openFileDialogW(L"Select a gif file",L"",1,filter_patterns,NULL,0);
   if(path==NULL)
   {
      input_gif[0] = '\0';
      return;
   }
   wcscpy(input_gif,path);
}

void gif_output_select(int dither_mode, int sample_mode, int distance_mode, int scale_mode, int width, int height, SLK_Palette *pal)
{
   const wchar_t *filter_patterns[2] = {L"*.gif"};
   const wchar_t *path = tinyfd_saveFileDialogW(L"Save gif",L"",1,filter_patterns,NULL);
   if(path==NULL)
   {
      output_gif[0] = '\0';
      return;
   }
   wcscpy(output_gif,path);

   if(output_gif[0]!='\0'&&input_gif[0]!='\0') //Process directory
   {
      char buffer[512];
      stbi_convert_wchar_to_utf8(buffer,512,input_gif);
      gd_GIF *gif = gd_open_gif(buffer);
      if(!gif)
         return;
      uint8_t gif_palette[256*3] = {0};
      for(int i = 0;i<256;i++)
      {
         gif_palette[i*3] = pal->colors[i].r;
         gif_palette[i*3+1] = pal->colors[i].g;
         gif_palette[i*3+2] = pal->colors[i].b;
      }
      char buffer_out[512];
      stbi_convert_wchar_to_utf8(buffer_out,512,output_gif);
      ge_GIF *gif_out;
      if(scale_mode==0)
         gif_out = ge_new_gif(buffer_out,width,height,gif_palette,8,gif->loop_count);
      else
         gif_out = ge_new_gif(buffer_out,gif->width/width,gif->height/height,gif_palette,8,gif->loop_count);
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
         img2pixel_lowpass_image(in,in);
         img2pixel_sharpen_image(in,in);
         img2pixel_process_image(in,out);
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

static SLK_Palette *palette_png(FILE *f)
{
   SLK_RGB_sprite *s = SLK_rgb_sprite_load_file(f);
   SLK_Palette *p = malloc(sizeof(*p));
   if(!p)
      return NULL;
   memset(p,0,sizeof(*p));
   p->used = MIN(256,s->width*s->height);
   for(int i = 0;i<p->used;i++)
      p->colors[i] = s->data[i];
   SLK_rgb_sprite_destroy(s);

   return p;
}

static SLK_Palette *palette_gpl(FILE *f)
{
   SLK_Palette *p = malloc(sizeof(*p));
   if(!p)
      return NULL;
   memset(p,0,sizeof(*p));
   char buffer[512];
   int c = 0;
   int r,g,b;

   while(fgets(buffer,512,f))
   {
      if(buffer[0]=='#')
         continue;
      if(sscanf(buffer,"%d %d %d",&r,&g,&b)==3)
      {
         p->colors[c].r = r;
         p->colors[c].g = g;
         p->colors[c].b = b;
         p->colors[c].a = 255;
         c++;
      }
   }
   p->used = c;

   return p;
}

static SLK_Palette *palette_hex(FILE *f)
{

   SLK_Palette *p = malloc(sizeof(*p));
   if(!p)
      return NULL;
   memset(p,0,sizeof(*p));
   char buffer[512];
   int c = 0;

   while(fgets(buffer,512,f))
   {
      p->colors[c].r = chartoi(buffer[0])*16+chartoi(buffer[1]);
      p->colors[c].g = chartoi(buffer[2])*16+chartoi(buffer[3]);
      p->colors[c].b = chartoi(buffer[4])*16+chartoi(buffer[5]);
      p->colors[c].a = 255;
      c++;
   }
   p->used= c;

   return p;
}

//Helper function for palette_hex
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
