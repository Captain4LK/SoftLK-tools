/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//wchar_t my ass
//whats utf8? - microshaft
//windows is the only reason this file exists

//External includes
#include <stdint.h>
#include "../headers/libSLK.h"
#include <wchar.h>
#include <windows.h>
#define FOPEN_UTF8_IMPLEMENTATION
#include "../external/fopen_utf8.h"
#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#define CUTE_FILES_IMPLEMENTATION
#include "../external/cute_files.h"
#define CUTE_PATH_IMPLEMENTATION
#include "../external/cute_path.h"
#include "../external/HLH_json.h"
#include "../external/nfd.h"
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
static char path_image_load[512] = {0};
static char path_image_save[512] = {0};
static char path_preset_load[512] = {0};
static char path_preset_save[512] = {0};
static char path_dir_input[512] = {0};
static char path_dir_output[512] = {0};
static char path_palette_load[512] = {0};
static char path_palette_save[512] = {0};

int upscale = 1;
//-------------------------------------

//Function prototypes
static uint8_t find_palette(SLK_Color in, SLK_Palette *pal);
static void image_save_w(const wchar_t *path, SLK_RGB_sprite *img, SLK_Palette *pal);
static SLK_Palette *palette_png(FILE *f);
static SLK_Palette *palette_gpl(FILE *f);
static SLK_Palette *palette_hex(FILE *f);
static int chartoi(char in);

static int convert_utf8_to_wchar(wchar_t *buffer, size_t bufferlen, const char* input);
//-------------------------------------

//Function implementations

SLK_RGB_sprite *image_select()
{
   NFD_Init();

   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_OpenDialogU8(&file_path,NULL,0,path_image_load);

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return SLK_rgb_sprite_create(1,1);
   }

   if(file_path!=NULL)
      path_pop(file_path,path_image_load,NULL);

   SLK_RGB_sprite *load = image_load(file_path);

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return load;
}

void image_write(SLK_RGB_sprite *img, SLK_Palette *pal)
{
   NFD_Init();
   
   nfdu8filteritem_t filter_item[2] = {{"PNG", "png"}, {"SLK", "slk"}};
   nfdu8char_t *file_path = NULL;

   nfdresult_t result = NFD_SaveDialogU8(&file_path,filter_item,2,path_image_save,"untitled.png");

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return;
   }

   if(file_path!=NULL)
      path_pop(file_path,path_image_save,NULL);

   wchar_t buffer[512];
   convert_utf8_to_wchar(buffer,512,file_path);
   image_save_w((wchar_t *)buffer,img,pal);

   NFD_FreePathU8(file_path);
   NFD_Quit();
}

FILE *json_select()
{
   NFD_Init();

   nfdu8filteritem_t filter_item[1] = {{"JSON", "json"}};
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_OpenDialogU8(&file_path,filter_item,1,path_preset_load);

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return NULL;
   }

   FILE *ret = NULL;
   if(file_path!=NULL)
   {
      ret = fopen_utf8(file_path,"r");
      path_pop(file_path,path_preset_load,NULL);
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return ret;
}

FILE *json_write()
{
   NFD_Init();

   nfdu8filteritem_t filter_item[1] = {{"JSON", "json"}};
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_SaveDialogU8(&file_path,filter_item,1,path_preset_save,"untitled.json");

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return NULL;
   }

   FILE *ret = NULL;
   if(file_path!=NULL)
   {
      ret = fopen_utf8(file_path,"w");
      path_pop(file_path,path_preset_save,NULL);
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return ret;
}

SLK_Palette *palette_select()
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

   SLK_Palette *p = NULL;
   if(file_path!=NULL)
   {
      path_pop(file_path,path_palette_load,NULL);
      FILE *f = fopen_utf8(file_path,"rb");
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
      }
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();

   return p;
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
   NFD_Init();

   nfdu8filteritem_t filter_item[1] = {{"pal", "pal"}};
   nfdu8char_t *file_path = NULL;
   nfdresult_t result = NFD_SaveDialogU8(&file_path,filter_item,1,path_palette_save,"untitled.pal");

   if(result!=NFD_OKAY)
   {
      NFD_Quit();
      return;
   }

   if(file_path!=NULL)
   {
      path_pop(file_path,path_palette_save,NULL);
      FILE *f = fopen_utf8(file_path,"w");
      if(f)
      {
         SLK_palette_save_file(f,pal);
         fclose(f);
      }
   }

   NFD_FreePathU8(file_path);
   NFD_Quit();
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
         if(!img->data[i].rgb.a)
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
         if(!img->data[i].rgb.a)
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
      if(pal->colors[i].rgb.r==in.rgb.r&&pal->colors[i].rgb.g==in.rgb.g&&pal->colors[i].rgb.b==in.rgb.b)
         return i;

   return 0;
}

void dir_input_select()
{
   NFD_Init();

   nfdu8char_t *path = NULL;
   nfdresult_t result = NFD_PickFolderU8(&path,path_dir_input);

   if(result!=NFD_OKAY)
   {
      input_dir[0] = btowc('\0');
   }
   else
   {
      strncpy(path_dir_input,path,512);
      convert_utf8_to_wchar(input_dir,512,path);
      NFD_FreePathU8(path);
   }

   NFD_Quit();
}

void dir_output_select(int dither_mode, int sample_mode, int distance_mode, int scale_mode, int width, int height, SLK_Palette *pal)
{
   NFD_Init();

   nfdu8char_t *path = NULL;
   nfdresult_t result = NFD_PickFolderU8(&path,path_dir_output);

   if(result!=NFD_OKAY)
   {
      output_dir[0] = '\0';
      NFD_Quit();
      return;
   }
   else
   {
      strncpy(path_dir_output,path,512);
      convert_utf8_to_wchar(output_dir,512,path);
      NFD_FreePathU8(path);
      NFD_Quit();
   }

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

   HLH_json_write_file(f,&root->root);
   HLH_json_free(root);

   fclose(f);
}

static int convert_utf8_to_wchar(wchar_t *buffer, size_t bufferlen, const char* input)
{
   return MultiByteToWideChar(CP_UTF8,0,input,-1,buffer,(int)bufferlen);
}
//-------------------------------------
