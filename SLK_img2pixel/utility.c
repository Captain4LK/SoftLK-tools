/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
#include "../headers/libSLK.h"
#define CUTE_FILES_IMPLEMENTATION
#include "../external/cute_files.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#include "../external/tinyfiledialogs.h"
#include "../external/HLH_json.h"
#define CUTE_PATH_IMPLEMENTATION
#include "../external/cute_path.h"

#define HLH_STREAM_IMPLEMENTATION
#include "../external/HLH_stream.h"

#define HLH_QOI_IMPLEMENTATION
#include "../external/HLH_qoi.h"
//-------------------------------------

//Internal includes
#include "image2pixel.h"
#include "utility.h"
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
static char input_dir[256];
static char output_dir[256];
static char path_image_load[512] = {0};
static char path_image_save[512] = {0};
static char path_preset_load[512] = {0};
static char path_preset_save[512] = {0};
static char path_dir_input[512] = {0};
static char path_dir_output[512] = {0};
static char path_palette_load[512] = {0};
static char path_palette_save[512] = {0};

int win_preview_width = 260;
int win_preview_height = 286;
int gui_scale = 1;
//-------------------------------------

//Function prototypes
static uint8_t find_palette(SLK_Color in, SLK_Palette *pal);
static SLK_Palette *palette_png(FILE *f);
static SLK_Palette *palette_gpl(FILE *f);
static SLK_Palette *palette_hex(FILE *f);
static int chartoi(char in);
//-------------------------------------

//Function implementations

SLK_RGB_sprite *image_select()
{
   const char *filter_patterns[2] = {"*.png"};
   const char *file_path = tinyfd_openFileDialog("Select a file",path_image_load,0,filter_patterns,NULL,0);

   if(file_path!=NULL)
      strncpy(path_image_load,file_path,512);

   return image_load(file_path);
}

void image_write(I2P_state *s, SLK_RGB_sprite *img, SLK_Palette *pal)
{
   const char *filter_patterns[2] = {"*.png","*.qoi"};
   const char *file_path = tinyfd_saveFileDialog("Save image",path_image_save,2,filter_patterns,NULL);

   if(file_path!=NULL)
      strncpy(path_image_save,file_path,512);

   image_save(s,file_path,img,pal);
}

FILE *json_select()
{
   const char *filter_patterns[2] = {"*.json"};
   const char *file_path = tinyfd_openFileDialog("Select a preset",path_preset_load,1,filter_patterns,NULL,0);

   if(file_path!=NULL)
   {
      strncpy(path_preset_load,file_path,512);
      return fopen(file_path,"r");
   }

   return NULL;
}

FILE *json_write()
{
   const char *filter_patterns[2] = {"*.json"};
   const char *file_path = tinyfd_saveFileDialog("Save preset",path_preset_save,1,filter_patterns,NULL);

   if(file_path!=NULL)
   {
      strncpy(path_preset_save,file_path,512);
      return fopen(file_path,"w");
   }

   return NULL;
}

SLK_Palette *palette_select()
{
   const char *filter_patterns[] = {"*.pal","*.png","*.gpl","*.hex"};
   const char *file_path = tinyfd_openFileDialog("Load a palette",path_palette_load,4,filter_patterns,NULL,0);
   if(file_path!=NULL)
   {
      strncpy(path_palette_load,file_path,512);

      SLK_Palette *p = NULL;
      FILE *f = fopen(file_path,"rb");
      if(f)
      {
         cf_file_t file; //Not ment to be used this way, but since it's possible, who cares
         strcpy(file.name,file_path);
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
   const char *filter_patterns[2] = {"*.pal"};
   const char *file_path = tinyfd_saveFileDialog("Save palette",path_palette_save,1,filter_patterns,NULL);

   if(file_path!=NULL)
   {
      strncpy(path_palette_save,file_path,512);
      SLK_palette_save(file_path,pal);
   }
}

void image_save(I2P_state *s, const char *path, SLK_RGB_sprite *img, SLK_Palette *pal)
{
   if(img==NULL||path==NULL)
      return;

   cf_file_t file; //Not ment to be used this way, but since it's possible, who cares
   strcpy(file.name,path);

   //Convert to rgb and upscale
   SLK_RGB_sprite *tmp_up = SLK_rgb_sprite_create(s->upscale*img->width,s->upscale*img->height);
   for(int y = 0;y<img->height;y++)
      for(int x = 0;x<img->width;x++)
         for(int y_ = 0;y_<s->upscale;y_++)
            for(int x_ = 0;x_<s->upscale;x_++)
               SLK_rgb_sprite_set_pixel(tmp_up,x*s->upscale+x_,y*s->upscale+y_,SLK_rgb_sprite_get_pixel(img,x,y));

   //qoi file
   if(strcmp(cf_get_ext(&file),".qoi")==0)
   {
      HLH_qoi_image img_qoi = {0};
      img_qoi.width = tmp_up->width;
      img_qoi.height = tmp_up->height;
      img_qoi.channels = 4;
      img_qoi.color_space = 0;
      img_qoi.data = (HLH_qoi_color *)tmp_up->data;

      FILE *f = fopen(path,"wb");
      if(f==NULL)
         return;
      HLH_rw rw;
      HLH_rw_init_file(&rw,f);
      HLH_qoi_encode(&rw,&img_qoi);
      HLH_rw_close(&rw);
      fclose(f);

      return;
   }

   //png file
   SLK_rgb_sprite_save(path,tmp_up);
   SLK_rgb_sprite_destroy(tmp_up);
}

SLK_RGB_sprite *image_load(const char *path)
{
   unsigned char *data = NULL;
   int width = 1;
   int height = 1;
   SLK_RGB_sprite *out;

   if(path==NULL)
      goto err;

   cf_file_t file; //Not ment to be used this way, but since it's possible, who cares
   strcpy(file.name,path);
   cf_get_ext(&file);

   if(strcmp(file.ext,".qoi")==0)
   {
      FILE *f = fopen(path,"rb");
      if(f==NULL)
         goto err;

      HLH_rw rw; 
      HLH_rw_init_file(&rw,f);
      HLH_qoi_image *img = HLH_qoi_decode(&rw);
      HLH_rw_close(&rw);
      fclose(f);

      if(img==NULL)
         goto err;

      out = SLK_rgb_sprite_create(img->width,img->height);
      memcpy(out->data,img->data,img->width*img->height*sizeof(*out->data));
      return out;
   }

   data = stbi_load(path,&width,&height,NULL,4);
   if(data==NULL)
      goto err;

   out = SLK_rgb_sprite_create(width,height);
   memcpy(out->data,data,width*height*sizeof(*out->data));

   stbi_image_free(data);

   return out;

err:
   printf("Failed to load %s\n",path);
   return SLK_rgb_sprite_create(1,1);
}

static uint8_t find_palette(SLK_Color in, SLK_Palette *pal)
{
   for(int i = 0;i<pal->used;i++)
      if(pal->colors[i].rgb.r==in.rgb.r&&pal->colors[i].rgb.g==in.rgb.g&&pal->colors[i].rgb.b==in.rgb.b)
         return i;

   return 0;
}

void dir_input_select()
{
   const char *path = tinyfd_selectFolderDialog("Select input directory",path_dir_input);
   if(path==NULL)
   {
      input_dir[0] = '\0';
      return;
   }

   strncpy(path_dir_input,path,512);
   strcpy(input_dir,path);
}

void dir_output_select(I2P_state *s, int dither_mode, int sample_mode, int distance_mode, int scale_mode, int width, int height, SLK_Palette *pal)
{
   const char *path = tinyfd_selectFolderDialog("Select output directory",path_dir_output);

   if(path==NULL)
   {
      output_dir[0] = '\0';
      return;
   }

   strncpy(path_dir_output,path,512);
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
            img2pixel_lowpass_image(s,in,in);
            img2pixel_sharpen_image(s,in,in);
            if(in!=NULL)
            {
               if(scale_mode==1)
                  out = SLK_rgb_sprite_create(in->width/width,in->height/height);
               img2pixel_process_image(s,in,out);

               char name[512];
               path_pop_ext(file.name,name,NULL);
               sprintf(tmp,"%s/%s.png",output_dir,name);

               image_save(s,tmp,out,pal);
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
         p->colors[c].rgb.r = r;
         p->colors[c].rgb.g = g;
         p->colors[c].rgb.b = b;
         p->colors[c].rgb.a = 255;
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
      p->colors[c].rgb.r = chartoi(buffer[0])*16+chartoi(buffer[1]);
      p->colors[c].rgb.g = chartoi(buffer[2])*16+chartoi(buffer[3]);
      p->colors[c].rgb.b = chartoi(buffer[4])*16+chartoi(buffer[5]);
      p->colors[c].rgb.a = 255;
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

void utility_init()
{
   FILE *f = fopen("settings.json","r");
   if(f==NULL)
      return;

   HLH_json5_root *root = HLH_json_parse_file_stream(f);

   strncpy(path_image_load,HLH_json_get_object_string(&root->root,"path_image_load","(NULL)"),512);
   strncpy(path_image_save,HLH_json_get_object_string(&root->root,"path_image_save","(NULL)"),512);
   strncpy(path_preset_load,HLH_json_get_object_string(&root->root,"path_preset_load","(NULL)"),512);
   strncpy(path_preset_save,HLH_json_get_object_string(&root->root,"path_preset_save","(NULL)"),512);
   strncpy(path_palette_load,HLH_json_get_object_string(&root->root,"path_palette_load","(NULL)"),512);
   strncpy(path_palette_save,HLH_json_get_object_string(&root->root,"path_palette_save","(NULL)"),512);
   strncpy(path_dir_input,HLH_json_get_object_string(&root->root,"path_dir_input","(NULL)"),512);
   strncpy(path_dir_output,HLH_json_get_object_string(&root->root,"path_dir_output","(NULL)"),512);
   win_preview_width = HLH_json_get_object_integer(&root->root,"preview_window_width",260);
   win_preview_height = HLH_json_get_object_integer(&root->root,"preview_window_height",286);
   gui_scale = HLH_json_get_object_integer(&root->root,"gui_scale",1);

   HLH_json_free(root);
}

void utility_exit()
{
   FILE *f = fopen("settings.json","w");
   if(f==NULL)
      return;

   HLH_json5_root *root = HLH_json_create_root();

   HLH_json_object_add_string(&root->root,"path_image_load",path_image_load);
   HLH_json_object_add_string(&root->root,"path_image_save",path_image_save);
   HLH_json_object_add_string(&root->root,"path_preset_load",path_preset_load);
   HLH_json_object_add_string(&root->root,"path_preset_save",path_preset_save);
   HLH_json_object_add_string(&root->root,"path_palette_load",path_palette_load);
   HLH_json_object_add_string(&root->root,"path_palette_save",path_palette_save);
   HLH_json_object_add_string(&root->root,"path_dir_input",path_dir_input);
   HLH_json_object_add_string(&root->root,"path_dir_output",path_dir_output);
   HLH_json_object_add_integer(&root->root,"preview_window_width",win_preview_width);
   HLH_json_object_add_integer(&root->root,"preview_window_height",win_preview_height);
   HLH_json_object_add_integer(&root->root,"gui_scale",gui_scale);

   HLH_json_write_file(f,&root->root);
   HLH_json_free(root);

   fclose(f);
}
//-------------------------------------
