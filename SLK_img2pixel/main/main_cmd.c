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
#include <stdarg.h>

#define HLH_IMPLEMENTATION
#include "HLH.h"
#define HLH_PATH_IMPLEMENTATION
#include "HLH_path.h"
#define HLH_RW_IMPLEMENTATION
#include "HLH_rw.h"
#define HLH_JSON_IMPLEMENTATION
#include "HLH_json.h"
#define OPTPARSE_IMPLEMENTATION
#include "optparse.h"
#define FOPEN_UTF8_IMPLEMENTATION
#include "../../external/fopen_utf8.h"
#define CUTE_FILES_IMPLEMENTATION
#include "cute_files.h"

#include "HLH_gui.h"
//-------------------------------------

//Lua
#include "external/lua/lua.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lualib.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
#include "shared/gif.h"
#include "shared/gif_read.h"
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
static SLK_dither_config dither_config = {.alpha_threshold = 128, .dither_mode = SLK_DITHER_BAYER4X4, .color_dist = SLK_RGB_REDMEAN,
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
static const char *path_palette = NULL;
static int generate_palette = 0;
static int generate_colors = 0;
//-------------------------------------

//Function prototypes
static void print_help(int argc, char **argv);
static void dump_defaults();
static int slk_path_pop_ext(const char *path, char *out, char *ext);
static int slk_ext_is(const char *ext, const char *name);
static int slk_process_image(const char *path_in, const char *path_out);
static int slk_load_preset(const char *path);
static int slk_run_script(const char *path);
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
      {"script", 's', OPTPARSE_REQUIRED},
      {"dump-defaults", 'd', OPTPARSE_NONE},
      {"help", 'h', OPTPARSE_NONE},
      {0},
   };

   const char *path_in = NULL;
   const char *path_out = NULL;
   const char *path_preset = NULL;
   const char *path_script = NULL;

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
      case 's':
         path_script = options.optarg;
         break;
      case 'd':
         dump_defaults();
         return 0;
      case 'h':
         print_help(argc, argv);
         return 0;
      }
   }

   if(path_preset!=NULL)
      slk_load_preset(path_preset);

   if(path_script!=NULL)
      return slk_run_script(path_script);

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

   return slk_process_image(path_in,path_out);
}

static int slk_load_preset(const char *path)
{
   FILE *f = fopen(path,"r");
   if(f==NULL)
   {
      fprintf(stderr,"Failed to open preset file '%s'; using defaults instead\n",path);
      return -1;
   }

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
   dither_config.dither_mode = HLH_json_get_object_integer(&root->root,"dither_dither_mode",2);
   dither_config.color_dist = HLH_json_get_object_integer(&root->root,"dither_color_dist",2);
   dither_config.palette_colors = HLH_json_get_object_integer(&root->root,"dither_palette_colors",2);
   HLH_json5 *array = HLH_json_get_object_array(&root->root,"dither_palette",&fallback);
   for(int i = 0;i<256;i++)
      dither_config.palette[i] = HLH_json_get_array_integer(array,i,0);

   HLH_json_free(root);
   fclose(f);

   return 0;
}

static int slk_process_image(const char *path_in, const char *path_out)
{
   //Load input
   //-------------------------------------
   char ext_in[512];
   slk_path_pop_ext(path_in,NULL,ext_in);
   int in_is_gif = slk_ext_is(ext_in,"gif");

   int width,height;
   Image32 *img = NULL;

   //All decoded frames of a GIF input, and their delays (in centiseconds,
   //GIF's native delay unit) - both NULL/1 for non-GIF input.
   Image32 **gif_in_frames = NULL;
   int *gif_in_delays_cs = NULL;
   int frame_count = 1;
   int frame_width = 0;
   int frame_height = 0;

   if(in_is_gif)
   {
      if(!SLK_gif_read_all_frames(path_in,&gif_in_frames,&frame_count,&gif_in_delays_cs))
      {
         fprintf(stderr,"Failed to decode GIF '%s'\n",path_in);
         return -1;
      }

      width = gif_in_frames[0]->width;
      height = gif_in_frames[0]->height;
      frame_width = width;
      frame_height = height;

      //Everything below (preset loading, palette generation, dithering, ...) is
      //written in terms of a single Image32 "img" - build one from GIF frame 0
      //so all of that code keeps working unmodified. The remaining frames (if
      //any) are processed further down, once the final config is known.
      img = image32_dup(gif_in_frames[0]);
   }
   else
   {
      FILE *f = fopen(path_in,"rb");
      if(f==NULL)
      {
         fprintf(stderr,"Failed to open image file '%s'\n",path_in);
         return -1;
      }

      uint32_t *data = HLH_gui_image_load(f,&width,&height);

      if(data==NULL||width<=0||height<=0)
      {
         fprintf(stderr,"Failed to decode image '%s'\n",path_in);
         fclose(f);
         return -1;
      }

      img = malloc(sizeof(*img)+sizeof(*img->data)*width*height);
      img->width = width;
      img->height = height;
      memcpy(img->data,data,sizeof(*img->data)*width*height);
      HLH_gui_image_free(data);
      fclose(f);

      frame_width = width;
      frame_height = height;
   }
   //-------------------------------------

   if(scale_relative)
   {
      width = img->width/HLH_non_zero(size_relative_x);
      height = img->height/HLH_non_zero(size_relative_y);
   }
   else
   {
      width = size_absolute_x;
      height = size_absolute_y;
   }
   Image64 *img64 = image32to64(img);
   image64_blur(img64,blur_amount);
   Image64 *sampled = image64_sample(img64,width,height,sample_mode,x_offset,y_offset);
   free(img64);
   image64_sharpen(sampled,sharp_amount);
   image64_hscb(sampled,hue,saturation,contrast,brightness);
   image64_gamma(sampled,gamma);
   image64_tint(sampled,tint_red,tint_green,tint_blue);

   if(generate_palette)
   {
      if(generate_colors>0)
         dither_config.palette_colors = generate_colors;
      Image32 *img_kmeans = image64to32(sampled);
      image32_kmeans(img_kmeans,dither_config.palette,dither_config.palette_colors,time(NULL),kmeanspp);
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

   SLK_img8and32 output = image64_dither(sampled, &dither_config);
   //Image8 *out = image64_dither(sampled,&dither_config);

   char ext[512];
   slk_path_pop_ext(path_out,NULL,ext);
   int out_is_gif = slk_ext_is(ext,"gif");

   //GIF output (of a still image, or of every frame of a GIF input) - all
   //frames need to share one global palette, which is exactly what
   //dither_config.palette already is, so we can feed image64_dither's index
   //buffers straight into the GIF writer.
   if(out_is_gif||frame_count>1)
   {
      uint8_t **gif_out_frames = malloc(sizeof(*gif_out_frames)*(size_t)frame_count);
      int *gif_out_delays = malloc(sizeof(*gif_out_delays)*(size_t)frame_count);
      size_t frame_bytes = (size_t)output.img8->width*(size_t)output.img8->height;

      //Frame 0 was already processed above (same as the non-GIF path)
      gif_out_frames[0] = malloc(frame_bytes);
      memcpy(gif_out_frames[0],output.img8->data,frame_bytes);
      gif_out_delays[0] = (frame_count>1&&gif_in_delays_cs!=NULL)?gif_in_delays_cs[0]:10;

      for(int fidx = 1;fidx<frame_count;fidx++)
      {
         Image64 *fimg64 = image32to64(gif_in_frames[fidx]);
         image64_blur(fimg64,blur_amount);
         Image64 *fsampled = image64_sample(fimg64,width,height,sample_mode,x_offset,y_offset);
         free(fimg64);
         image64_sharpen(fsampled,sharp_amount);
         image64_hscb(fsampled,hue,saturation,contrast,brightness);
         image64_gamma(fsampled,gamma);
         image64_tint(fsampled,tint_red,tint_green,tint_blue);

         SLK_img8and32 foutput = image64_dither(fsampled,&dither_config);
         free(fsampled);

         gif_out_frames[fidx] = malloc(frame_bytes);
         memcpy(gif_out_frames[fidx],foutput.img8->data,frame_bytes);
         gif_out_delays[fidx] = gif_in_delays_cs[fidx];

         free(foutput.img8);
         free(foutput.img32);
      }

      if(out_is_gif)
      {
         if(!SLK_gif_write(path_out,width,height,dither_config.palette,dither_config.palette_colors,gif_out_frames,frame_count,gif_out_delays,1,0))
            fprintf(stderr,"Failed to write GIF output '%s'\n",path_out);
      }

      for(int fidx = 0;fidx<frame_count;fidx++)
         free(gif_out_frames[fidx]);
      free(gif_out_frames);
      free(gif_out_delays);
   }

   //Non-GIF output: only ever writes frame 0 (matches the pre-existing
   //behaviour when the input isn't animated)
   if(!out_is_gif)
   {
      if(strcmp(ext, "PCX") == 0 || strcmp(ext, "pcx") == 0)
      {
         image8_save(output.img8,path_out,ext);
      }
      else
      {
         FILE *fp = fopen(path_out, "wb");
         if(fp != NULL)
         {
            HLH_gui_image_save(fp, output.img32->data, output.img32->width, output.img32->height, ext);
            fclose(fp);
         }
      }
   }

   if(gif_in_frames!=NULL)
   {
      for(int fidx = 0;fidx<frame_count;fidx++)
         free(gif_in_frames[fidx]);
      free(gif_in_frames);
   }
   if(gif_in_delays_cs!=NULL)
      free(gif_in_delays_cs);

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
          "   --script\t\trun a Lua script instead of a single --in/--out conversion\n"
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

//Case-insensitive extension check, so "gif"/"GIF"/"Gif" all match
static int slk_ext_is(const char *ext, const char *name)
{
   if(ext==NULL||name==NULL)
      return 0;

   size_t i = 0;
   for(;ext[i]!='\0'&&name[i]!='\0';i++)
   {
      char a = ext[i];
      char b = name[i];
      if(a>='A'&&a<='Z') a = (char)(a-'A'+'a');
      if(b>='A'&&b<='Z') b = (char)(b-'A'+'a');
      if(a!=b)
         return 0;
   }

   return ext[i]=='\0'&&name[i]=='\0';
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

//Lua scripting
//-------------------------------------

//Holds the string a Lua script last assigned to img2pixel.palette_output_path,
//since path_palette (used by slk_process_image) just keeps a pointer -
//it needs to point at memory that outlives the Lua value on the stack.
static char lua_path_palette_buf[1024] = {0};

static int lua_img2pixel_process(lua_State *L)
{
   const char *in = luaL_checkstring(L,1);
   const char *out = luaL_checkstring(L,2);
   lua_pushboolean(L,slk_process_image(in,out)==0);
   return 1;
}

static int lua_img2pixel_load_preset(lua_State *L)
{
   const char *path = luaL_checkstring(L,1);
   lua_pushboolean(L,slk_load_preset(path)==0);
   return 1;
}

static int lua_img2pixel_get_palette_color(lua_State *L)
{
   lua_Integer index = luaL_checkinteger(L,1);
   if(index<0||index>=256)
      return luaL_error(L,"palette index out of range (0-255): %d",(int)index);
   lua_pushinteger(L,(lua_Integer)(dither_config.palette[index]&0xffffff));
   return 1;
}

static int lua_img2pixel_set_palette_color(lua_State *L)
{
   lua_Integer index = luaL_checkinteger(L,1);
   lua_Integer rgb = luaL_checkinteger(L,2);
   if(index<0||index>=256)
      return luaL_error(L,"palette index out of range (0-255): %d",(int)index);
   dither_config.palette[index] = 0xff000000u|((uint32_t)rgb&0xffffff);
   return 0;
}

static int lua_img2pixel_print(lua_State *L)
{
   int n = lua_gettop(L);
   for(int i = 1;i<=n;i++)
   {
      if(i>1)
         fputc('\t',stdout);
      fputs(luaL_tolstring(L,i,NULL),stdout);
      lua_pop(L,1);
   }
   fputc('\n',stdout);
   return 0;
}

//Scalar settings a script can read/write on img2pixel, same names as the JSON preset
typedef enum
{
   LUA_FIELD_FLOAT,
   LUA_FIELD_INT,
   LUA_FIELD_BOOL,
   LUA_FIELD_U8,
   LUA_FIELD_STRING,
}Lua_field_type;

typedef struct
{
   const char *name;
   void *ptr;
   Lua_field_type type;
}Lua_field;

static Lua_field lua_fields[] =
{
   {"blur_amount",&blur_amount,LUA_FIELD_FLOAT},
   {"sample_mode",&sample_mode,LUA_FIELD_INT},
   {"x_offset",&x_offset,LUA_FIELD_FLOAT},
   {"y_offset",&y_offset,LUA_FIELD_FLOAT},
   {"scale_relative",&scale_relative,LUA_FIELD_BOOL},
   {"size_relative_x",&size_relative_x,LUA_FIELD_INT},
   {"size_relative_y",&size_relative_y,LUA_FIELD_INT},
   {"size_absolute_x",&size_absolute_x,LUA_FIELD_INT},
   {"size_absolute_y",&size_absolute_y,LUA_FIELD_INT},
   {"sharp_amount",&sharp_amount,LUA_FIELD_FLOAT},
   {"brightness",&brightness,LUA_FIELD_FLOAT},
   {"contrast",&contrast,LUA_FIELD_FLOAT},
   {"saturation",&saturation,LUA_FIELD_FLOAT},
   {"hue",&hue,LUA_FIELD_FLOAT},
   {"gamma",&gamma,LUA_FIELD_FLOAT},
   {"kmeanspp",&kmeanspp,LUA_FIELD_BOOL},
   {"tint_red",&tint_red,LUA_FIELD_U8},
   {"tint_green",&tint_green,LUA_FIELD_U8},
   {"tint_blue",&tint_blue,LUA_FIELD_U8},
   {"dither_alpha_threshold",&dither_config.alpha_threshold,LUA_FIELD_INT},
   {"dither_amount",&dither_config.dither_amount,LUA_FIELD_FLOAT},
   {"target_colors",&dither_config.target_colors,LUA_FIELD_INT},
   {"dither_mode",&dither_config.dither_mode,LUA_FIELD_INT},
   {"color_dist",&dither_config.color_dist,LUA_FIELD_INT},
   {"palette_colors",&dither_config.palette_colors,LUA_FIELD_INT},
   {"generate_palette",&generate_palette,LUA_FIELD_BOOL},
   {"generate_colors",&generate_colors,LUA_FIELD_INT},
};
#define LUA_FIELD_COUNT (sizeof(lua_fields)/sizeof(lua_fields[0]))

static Lua_field *lua_field_find(const char *name)
{
   for(size_t i = 0;i<LUA_FIELD_COUNT;i++)
      if(strcmp(lua_fields[i].name,name)==0)
         return &lua_fields[i];
   return NULL;
}

static int lua_img2pixel_index(lua_State *L)
{
   const char *key = luaL_checkstring(L,2);

   if(strcmp(key,"palette_output_path")==0)
   {
      lua_pushstring(L,lua_path_palette_buf);
      return 1;
   }

   Lua_field *field = lua_field_find(key);
   if(field==NULL)
   {
      lua_pushnil(L);
      return 1;
   }

   switch(field->type)
   {
   case LUA_FIELD_FLOAT: lua_pushnumber(L,(double)(*(float *)field->ptr)); break;
   case LUA_FIELD_INT: lua_pushinteger(L,*(int *)field->ptr); break;
   case LUA_FIELD_BOOL: lua_pushboolean(L,*(int *)field->ptr); break;
   case LUA_FIELD_U8: lua_pushinteger(L,*(uint8_t *)field->ptr); break;
   case LUA_FIELD_STRING: lua_pushstring(L,*(char **)field->ptr); break;
   }

   return 1;
}

static int lua_img2pixel_newindex(lua_State *L)
{
   const char *key = luaL_checkstring(L,2);

   if(strcmp(key,"palette_output_path")==0)
   {
      const char *s = luaL_checkstring(L,3);
      strncpy(lua_path_palette_buf,s,sizeof(lua_path_palette_buf)-1);
      lua_path_palette_buf[sizeof(lua_path_palette_buf)-1] = '\0';
      path_palette = lua_path_palette_buf[0]!='\0'?lua_path_palette_buf:NULL;
      return 0;
   }

   Lua_field *field = lua_field_find(key);
   if(field==NULL)
      return luaL_error(L,"img2pixel has no setting named '%s'",key);

   switch(field->type)
   {
   case LUA_FIELD_FLOAT: *(float *)field->ptr = (float)luaL_checknumber(L,3); break;
   case LUA_FIELD_INT: *(int *)field->ptr = (int)luaL_checkinteger(L,3); break;
   case LUA_FIELD_BOOL: *(int *)field->ptr = lua_toboolean(L,3); break;
   case LUA_FIELD_U8: *(uint8_t *)field->ptr = (uint8_t)luaL_checkinteger(L,3); break;
   case LUA_FIELD_STRING: break; //no plain-string fields currently need direct assignment
   }

   return 0;
}

//Small named-constant tables so scripts can write e.g.
//img2pixel.dither_mode = img2pixel.DITHER.STUCKI instead of a bare number.
static void lua_push_dither_enum_table(lua_State *L)
{
   lua_newtable(L);
#define ENUMVAL(name,value) lua_pushinteger(L,value); lua_setfield(L,-2,name)
   ENUMVAL("NONE",SLK_DITHER_NONE);
   ENUMVAL("BAYER8X8",SLK_DITHER_BAYER8X8);
   ENUMVAL("BAYER4X4",SLK_DITHER_BAYER4X4);
   ENUMVAL("BAYER2X2",SLK_DITHER_BAYER2X2);
   ENUMVAL("CLUSTER8X8",SLK_DITHER_CLUSTER8X8);
   ENUMVAL("CLUSTER4X4",SLK_DITHER_CLUSTER4X4);
   ENUMVAL("FLOYD",SLK_DITHER_FLOYD);
   ENUMVAL("FLOYD2",SLK_DITHER_FLOYD2);
   ENUMVAL("MEDIAN_CUT",SLK_DITHER_MEDIAN_CUT);
   ENUMVAL("BAYER5X5",SLK_DITHER_BAYER5X5);
   ENUMVAL("BAYER3X3",SLK_DITHER_BAYER3X3);
   ENUMVAL("STUCKI",SLK_DITHER_STUCKI);
   ENUMVAL("BURKES",SLK_DITHER_BURKES);
   ENUMVAL("SIERRA",SLK_DITHER_SIERRA);
   ENUMVAL("SIERRA_TWOROW",SLK_DITHER_SIERRA_TWOROW);
   ENUMVAL("SIERRA_LITE",SLK_DITHER_SIERRA_LITE);
   ENUMVAL("PICOCAD",SLK_DITHER_PICOCAD);
#undef ENUMVAL
}

static void lua_push_colordist_enum_table(lua_State *L)
{
   lua_newtable(L);
#define ENUMVAL(name,value) lua_pushinteger(L,value); lua_setfield(L,-2,name)
   ENUMVAL("RGB_EUCLIDIAN",SLK_RGB_EUCLIDIAN);
   ENUMVAL("RGB_WEIGHTED",SLK_RGB_WEIGHTED);
   ENUMVAL("RGB_REDMEAN",SLK_RGB_REDMEAN);
   ENUMVAL("LAB_CIE76",SLK_LAB_CIE76);
   ENUMVAL("LAB_CIE94",SLK_LAB_CIE94);
   ENUMVAL("LAB_CIEDE2000",SLK_LAB_CIEDE2000);
#undef ENUMVAL
}

static int slk_run_script(const char *path)
{
   lua_State *L = luaL_newstate();
   if(L==NULL)
   {
      fprintf(stderr,"Failed to create Lua state (out of memory)\n");
      return -1;
   }
   luaL_openlibs(L);

   lua_newtable(L); //the img2pixel table

   lua_pushcfunction(L,lua_img2pixel_process);
   lua_setfield(L,-2,"process");
   lua_pushcfunction(L,lua_img2pixel_load_preset);
   lua_setfield(L,-2,"load_preset");
   lua_pushcfunction(L,lua_img2pixel_get_palette_color);
   lua_setfield(L,-2,"get_palette_color");
   lua_pushcfunction(L,lua_img2pixel_set_palette_color);
   lua_setfield(L,-2,"set_palette_color");

   lua_push_dither_enum_table(L);
   lua_setfield(L,-2,"DITHER");
   lua_push_colordist_enum_table(L);
   lua_setfield(L,-2,"COLORDIST");

   //Metatable routes any other field (blur_amount, dither_mode, ...) straight
   //through to the C globals slk_process_image() reads - see lua_fields[].
   lua_newtable(L);
   lua_pushcfunction(L,lua_img2pixel_index);
   lua_setfield(L,-2,"__index");
   lua_pushcfunction(L,lua_img2pixel_newindex);
   lua_setfield(L,-2,"__newindex");
   lua_setmetatable(L,-2);

   lua_setglobal(L,"img2pixel");

   lua_register(L,"print",lua_img2pixel_print);

   if(luaL_dofile(L,path)!=LUA_OK)
   {
      fprintf(stderr,"Lua script error: %s\n",lua_tostring(L,-1));
      lua_close(L);
      return -1;
   }

   lua_close(L);
   return 0;
}
//-------------------------------------
