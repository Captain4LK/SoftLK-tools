/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define HLH_IMPLEMENTATION
#include "HLH.h"
#define HLH_JSON_IMPLEMENTATION
#include "HLH_json.h"
#define OPTPARSE_IMPLEMENTATION
#include "optparse.h"

#include "HLH_gui.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
//#include "util.h"
//-------------------------------------

//#defines
#define PATH_MAX 1024
#define PATH_EXT 32
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static float blur_amount = 0;
static int sample_mode = 0;
static float x_offset = 0.f;
static float y_offset = 0.f;
static int scale_relative = 0;
static int size_relative_x = 2;
static int size_relative_y = 2;
static int size_absolute_x = 64;
static int size_absolute_y = 64;
static float sharp_amount = 0.f;
static float brightness = 0.f;
static float contrast = 1.f;
static float saturation = 1.f;
static float hue = 0.f;
static float gamma = 1.f;
static int kmeanspp = 0;
SLK_dither_config dither_config = {.alpha_threshold = 128, .dither_mode = SLK_DITHER_BAYER4X4, .color_dist = SLK_RGB_REDMEAN,
                                   .target_colors = 8, .dither_amount = 0.2f, .palette_colors = 256, .palette =
   {
      0xffff00ff, 0xff000000, 0xff080808, 0xff0f0f0f, 0xff1f1f1f, 0xff2f2f2f, 0xff3f3f3f, 0xff4b4b4b,
      0xff5b5b5b, 0xff6b6b6b, 0xff7b7b7b, 0xff8b8b8b, 0xff9b9b9b, 0xffababab, 0xffbbbbbb, 0xffcbcbcb,
      0xffdbdbdb, 0xffebebeb, 0xff302b31, 0xff474350, 0xff616372, 0xff7e879a, 0xffa9b1c2, 0xffcfd3e2,
      0xffa3adc9, 0xff7b7da8, 0xff5e5584, 0xff493a5f, 0xff392642, 0xff271425, 0xff3f1f42, 0xff573263,
      0xff6c4b82, 0xff876bb0, 0xff9c94d1, 0xffbfc2ee, 0xff989be0, 0xff7b73ca, 0xff66539f, 0xff4e3d76,
      0xff372156, 0xff311036, 0xff4a2354, 0xff64357d, 0xff804ea9, 0xff9f71ca, 0xffc697e1, 0xffedbcf8,
      0xfff58cea, 0xffd45dc2, 0xffb83793, 0xff8a276f, 0xff61164f, 0xff31092e, 0xff591354, 0xff891f7e,
      0xffb126b5, 0xffd04adb, 0xffd378f5, 0xffcdacfb, 0xffc17dff, 0xffaa48f2, 0xff8925c5, 0xff620f8a,
      0xff4a0f57, 0xff2f0b33, 0xff441b57, 0xff57248a, 0xff682fbd, 0xff8563ff, 0xff9999ff, 0xffb3c5f0,
      0xff8097f2, 0xff6062f0, 0xff412fcc, 0xff372096, 0xff2b1465, 0xff1b0c3a, 0xff180e65, 0xff1c1f95,
      0xff2632c4, 0xff4159e2, 0xff708dff, 0xff99bfff, 0xff6f90e3, 0xff5b69b5, 0xff424b8e, 0xff32326d,
      0xff1e1f4a, 0xff10122b, 0xff1a2848, 0xff2f4264, 0xff416187, 0xff5986ae, 0xff78abd1, 0xff99d3f0,
      0xff6ab1e1, 0xff3f7fcc, 0xff2c57ab, 0xff22368f, 0xff1c2866, 0xff141b3d, 0xff142561, 0xff193e8c,
      0xff215cb5, 0xff2f88e1, 0xff55bbff, 0xff88e4ff, 0xff66c2e1, 0xff4a95b9, 0xff337399, 0xff22517b,
      0xff14375a, 0xff0c1f3d, 0xff092b6a, 0xff0a4296, 0xff0b66c1, 0xff0795e9, 0xff2fc7f9, 0xff6beffa,
      0xff3ebde8, 0xff2d92ce, 0xff2869aa, 0xff1e4982, 0xff152f59, 0xff132135, 0xff23384e, 0xff345569,
      0xff487186, 0xff6096aa, 0xff78c0cf, 0xffa4e0e1, 0xff87c1c2, 0xff62969b, 0xff48747b, 0xff37565e,
      0xff253c43, 0xff14272e, 0xff113a45, 0xff1a586b, 0xff217e8f, 0xff2ea7b3, 0xff57d9d2, 0xff8effeb,
      0xff53e0c4, 0xff2eb891, 0xff259468, 0xff1f7349, 0xff184a2d, 0xff102b17, 0xff204227, 0xff32633b,
      0xff428056, 0xff5da67c, 0xff78cfa6, 0xffa4f4d2, 0xff73e6a1, 0xff29d655, 0xff27ab2e, 0xff1b7d1b,
      0xff1f4d0f, 0xff1c2b0a, 0xff334511, 0xff476b18, 0xff519425, 0xff62c932, 0xff88ff75, 0xffaeffbe,
      0xff87c787, 0xff6fa162, 0xff52753f, 0xff43542f, 0xff30361b, 0xff241f0d, 0xff3d3711, 0xff5b5c17,
      0xff738a21, 0xff94bf2d, 0xffb9f962, 0xffdcfeb6, 0xffd6f165, 0xffc8d112, 0xffa09316, 0xff715e13,
      0xff4f3909, 0xff251b07, 0xff463e20, 0xff695f30, 0xff8a844a, 0xffadb06a, 0xffcbdb8c, 0xffeaf3ba,
      0xffe3dc82, 0xffd5b051, 0xffb68427, 0xff995624, 0xff6f3217, 0xff331610, 0xff402e20, 0xff594b35,
      0xff756949, 0xff9e966a, 0xffcac693, 0xfff3ebc7, 0xffffcba1, 0xffff9470, 0xffbd624f, 0xff993c3c,
      0xff5c2528, 0xff663b3c, 0xff8a5b58, 0xffb87e76, 0xffdba2a6, 0xffffcad9, 0xffcf99ab, 0xffad7284,
      0xff7d4f5c, 0xff5e383f, 0xff47222a, 0xff290e18, 0xff581b38, 0xff8a2859, 0xffbd357c, 0xffe85ba5,
      0xfff288cd, 0xffeeb7e4, 0xffb395bf, 0xff98749e, 0xff805778, 0xff664058, 0xff4b2c3e, 0xff2d1723,
      0xff3c2f36, 0xff584750, 0xff78676d, 0xff9e8b93, 0xffc8b5bd, 0xffe7dee1, 0xffc7b8b6, 0xff9b8d86,
      0xff736762, 0xff524741, 0xff382f29, 0xff1c1613, 0xff302f27, 0xff464639, 0xff636857, 0xff828d78,
      0xffaab9a6, 0xffbbe1cc, 0xff96c2a7, 0xff729c82, 0xff59775c, 0xff415740, 0xff2d3a25, 0xff171f12,
   }
};
static uint8_t tint_red = 255;
static uint8_t tint_green = 255;
static uint8_t tint_blue = 255;
//-------------------------------------

//Function prototypes
static void print_help(int argc, char **argv);
static void dump_defaults();
static int slk_path_pop_ext(const char *path, char *out, char *ext);
//-------------------------------------

//Function implementations

int main(int argc, char **argv)
{
   struct optparse_long longopts[] =
   {
      {"in", 'i', OPTPARSE_REQUIRED},
      {"out", 'o', OPTPARSE_REQUIRED},
      {"outp", 'P', OPTPARSE_REQUIRED},
      {"preset", 'p', OPTPARSE_REQUIRED},
      {"gen", 'g', OPTPARSE_REQUIRED},
      {"dump-defaults", 'd', OPTPARSE_NONE},
      {"help", 'h', OPTPARSE_NONE},
      {0},
   };

   const char *path_in = NULL;
   const char *path_out = NULL;
   const char *path_preset = NULL;
   const char *path_palette = NULL;
   int generate_palette = 0;
   int generate_colors = 0;

   int option;
   struct optparse options;
   optparse_init(&options, argv);
   while((option = optparse_long(&options, longopts, NULL))!=-1)
   {
      switch(option)
      {
      case 'i':
         path_in = options.optarg;
         break;
      case 'o':
         path_out = options.optarg;
         break;
      case 'p':
         path_preset = options.optarg;
         break;
      case 'P':
         path_palette = options.optarg;
         break;
      case 'g':
         generate_palette = 1;
         generate_colors = strtol(options.optarg,NULL,10);
         break;
      case 'd':
         dump_defaults();
         return 0;
      case 'h':
         print_help(argc, argv);
         return 0;
      }
   }

   if(path_in==NULL)
   {
      fprintf(stderr,"No input image specified; Try  %s --help\n",argv[0]);
      return -1;
   }

   if(path_out==NULL)
   {
      fprintf(stderr,"No output image specified; Try  %s --help\n",argv[0]);
      return -1;
   }

   //Load input
   //-------------------------------------
   FILE *f = fopen(path_in,"rb");
   if(f==NULL)
   {
      fprintf(stderr,"Failed to open image file '%s'\n",path_in);
      return -1;
   }

   int width,height;
   uint32_t *data = HLH_gui_image_load(f,&width,&height);

   if(data==NULL||width<=0||height<=0)
   {
   }

   SLK_image32 *img = malloc(sizeof(*img)+sizeof(*img->data)*width*height);
   img->w = width;
   img->h = height;
   memcpy(img->data,data,sizeof(*img->data)*width*height);
   HLH_gui_image_free(data);
   fclose(f);
   //-------------------------------------

   //Load preset (if specified)
   if(path_preset!=NULL)
   {
      f = fopen(path_preset,"r");
      if(f==NULL)
         fprintf(stderr,"Failed to open preset file '%s'; using defaults instead\n",path_preset);

      HLH_json5 fallback = {0};
      HLH_json5_root *root = HLH_json_parse_file_stream(f);

      blur_amount = HLH_json_get_object_real(&root->root,"blur_amount",0.);
      sample_mode = HLH_json_get_object_integer(&root->root,"sample_mode",0);
      x_offset = HLH_json_get_object_real(&root->root,"x_offset",0.f);
      y_offset = HLH_json_get_object_real(&root->root,"y_offset",0.f);
      scale_relative = HLH_json_get_object_boolean(&root->root,"scale_relative",0);
      size_relative_x = HLH_json_get_object_integer(&root->root,"size_relative_x",2);
      size_relative_y = HLH_json_get_object_integer(&root->root,"size_relative_y",2);
      size_absolute_x = HLH_json_get_object_integer(&root->root,"size_absolute_x",64);
      size_absolute_y = HLH_json_get_object_integer(&root->root,"size_absolute_y",64);
      sharp_amount = HLH_json_get_object_real(&root->root,"sharp_amount",0.f);
      brightness = HLH_json_get_object_real(&root->root,"brightness",0.f);
      contrast = HLH_json_get_object_real(&root->root,"contrast",1.f);
      saturation = HLH_json_get_object_real(&root->root,"saturation",1.f);
      hue = HLH_json_get_object_real(&root->root,"hue",0.f);
      gamma = HLH_json_get_object_real(&root->root,"gamma",1.f);
      kmeanspp = HLH_json_get_object_boolean(&root->root,"kmeanspp",1);
      tint_red = HLH_json_get_object_integer(&root->root,"tint_red",255);
      tint_green = HLH_json_get_object_integer(&root->root,"tint_green",255);
      tint_blue = HLH_json_get_object_integer(&root->root,"tint_blue",255);
      dither_config.alpha_threshold = HLH_json_get_object_integer(&root->root,"dither_alpha_threshold",128);
      dither_config.dither_amount = HLH_json_get_object_real(&root->root,"dither_dither_amount",0.2f);
      dither_config.target_colors = HLH_json_get_object_integer(&root->root,"dither_target_colors",8);
      //dither_config.use_median = HLH_json_get_object_boolean(&root->root,"dither_use_median",0);
      dither_config.dither_mode = HLH_json_get_object_integer(&root->root,"dither_dither_mode",2);
      dither_config.color_dist = HLH_json_get_object_integer(&root->root,"dither_color_dist",2);
      dither_config.palette_colors = HLH_json_get_object_integer(&root->root,"dither_palette_colors",2);
      HLH_json5 *array = HLH_json_get_object_array(&root->root,"dither_palette",&fallback);
      for(int i = 0;i<256;i++)
         dither_config.palette[i] = HLH_json_get_array_integer(array,i,0);

      HLH_json_free(root);

      if(f!=NULL)
         fclose(f);
   }

   if(scale_relative)
   {
      width = img->w/HLH_non_zero(size_relative_x);
      height = img->h/HLH_non_zero(size_relative_y);
   }
   else
   {
      width = size_absolute_x;
      height = size_absolute_y;
   }
   SLK_image64 *img64 = SLK_image64_dup32(img);
   SLK_image64_blur(img64,blur_amount);
   SLK_image64 *sampled = SLK_image64_sample(img64,width,height,sample_mode,x_offset,y_offset);
   free(img64);
   SLK_image64_sharpen(sampled,sharp_amount);
   SLK_image64_hscb(sampled,hue,saturation,contrast,brightness);
   SLK_image64_gamma(sampled,gamma);
   SLK_image64_tint(sampled,tint_red,tint_green,tint_blue);

   if(generate_palette)

   {
      if(generate_colors>0)
         dither_config.palette_colors = generate_colors;
      SLK_image32 *img_kmeans = SLK_image32_dup64(sampled);
      SLK_image32_kmeans(img_kmeans,dither_config.palette,dither_config.palette_colors,time(NULL),kmeanspp);
      free(img_kmeans);
   }

   if(path_palette!=NULL)
   {
      FILE *fp = fopen(path_palette,"w");
      if(fp!=NULL)
      {
         char ext[512] = {0};
         slk_path_pop_ext(path_palette,NULL,ext);
         SLK_palette_save(fp,dither_config.palette,dither_config.palette_colors,ext);
         fclose(fp);
      }
   }

   SLK_image32 *out = SLK_image64_dither(sampled,&dither_config);

   f = fopen(path_out,"wb");
   if(f==NULL)
   {
      fprintf(stderr,"Failed to open output image '%s'\n",path_out);
      return -1;
   }

   char ext[512];
   slk_path_pop_ext(path_out,NULL,ext);
   //HLH_gui_image_save(f,out->data,out->w,out->h,ext);
   if(strcmp(ext,"pcx")==0||strcmp(ext,"PCX")==0)
      SLK_image32_write_pcx(f,out,dither_config.palette,dither_config.palette_colors);
   else
      HLH_gui_image_save(f,out->data,out->w,out->h,ext);

   fclose(f);

   return 0;
}

static void print_help(int argc, char **argv)
{
   fprintf(stderr,"%s usage:\n"
          "%s --in filename --out filename [--preset preset]\n"
          "   --in\t\t\timage file to process\n"
          "   --out\t\toutput file\n"
          "   --outp\t\toutput palette\n"
          "   --dump-defaults\twrite default preset to stdout\n"
          "   --gen COLORS\t\tgenerate color palette\n"
          "   --preset\t\tpreset to use for processing\n"
          "   --help\t\tprint this text\n",
         argv[0],argv[0]);
}

static void dump_defaults()
{
   HLH_json5_root *root = HLH_json_create_root();
   HLH_json_object_add_real(&root->root,"blur_amount",blur_amount);
   HLH_json_object_add_integer(&root->root,"sample_mode",sample_mode);
   HLH_json_object_add_real(&root->root,"x_offset",x_offset);
   HLH_json_object_add_real(&root->root,"y_offset",y_offset);
   HLH_json_object_add_boolean(&root->root,"scale_relative",scale_relative);
   HLH_json_object_add_integer(&root->root,"size_relative_x",size_relative_x);
   HLH_json_object_add_integer(&root->root,"size_relative_y",size_relative_y);
   HLH_json_object_add_integer(&root->root,"size_absolute_x",size_absolute_x);
   HLH_json_object_add_integer(&root->root,"size_absolute_y",size_absolute_y);
   HLH_json_object_add_real(&root->root,"sharp_amount",sharp_amount);
   HLH_json_object_add_real(&root->root,"brightness",brightness);
   HLH_json_object_add_real(&root->root,"contrast",contrast);
   HLH_json_object_add_real(&root->root,"saturation",saturation);
   HLH_json_object_add_real(&root->root,"hue",hue);
   HLH_json_object_add_real(&root->root,"gamma",gamma);
   HLH_json_object_add_boolean(&root->root,"kmeanspp",kmeanspp);
   HLH_json_object_add_integer(&root->root,"dither_alpha_threshold",dither_config.alpha_threshold);
   HLH_json_object_add_real(&root->root,"dither_dither_amount",dither_config.dither_amount);
   HLH_json_object_add_integer(&root->root,"dither_target_colors",dither_config.target_colors);
   HLH_json_object_add_integer(&root->root,"dither_dither_mode",dither_config.dither_mode);
   HLH_json_object_add_integer(&root->root,"dither_color_dist",dither_config.color_dist);
   HLH_json_object_add_integer(&root->root,"tint_red",tint_red);
   HLH_json_object_add_integer(&root->root,"tint_green",tint_green);
   HLH_json_object_add_integer(&root->root,"tint_blue",tint_blue);
   HLH_json_object_add_integer(&root->root,"dither_palette_colors",dither_config.palette_colors);
   HLH_json5 array = HLH_json_create_array();
   for(int i = 0;i<256;i++)
      HLH_json_array_add_integer(&array,dither_config.palette[i]);
   HLH_json_object_add_array(&root->root,"dither_palette",array);

   HLH_json_write_file(stdout,&root->root);
   HLH_json_free(root);

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
//-------------------------------------
