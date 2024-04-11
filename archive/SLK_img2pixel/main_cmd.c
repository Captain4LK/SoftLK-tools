/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LIBSLK_IMPLEMENTATION
#define LIBSLK_BACKEND 2
#include "../headers/libSLK.h"
#include "../external/cute_files.h"
//-------------------------------------

//Internal includes
#include "image2pixel.h"
#include "utility.h"
#include "assets.h"
//-------------------------------------

//#defines
#define READ_ARG(I) \
   ((++(I))<argc?argv[(I)]:NULL)
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static SLK_RGB_sprite *sprite_in = NULL;
static const char *path_out = NULL;
static const char *path_out_pal = NULL;
static I2P_state state;
//-------------------------------------

//Function prototypes
static void print_help(int argc, char **argv);
static void parse_option(const char *name, const char *value);
//-------------------------------------

//Function implementations

int main(int argc, char **argv)
{
   //Load default palette
   img2pixel_state_init(&state);
   img2pixel_set_palette(&state,assets_load_pal_default());

   //parse args
   for(int i = 1;i<argc;i++)
   {
      if(strcmp(argv[i],"-help")==0||
         strcmp(argv[i],"--help")==0)
      {
         print_help(argc,argv);
         return 0;
      }
      else if(strcmp(argv[i],"-fin")==0)
      {
         sprite_in = image_load(READ_ARG(i));
      }
      else if(strcmp(argv[i],"-fout")==0)
      {
         path_out = READ_ARG(i);
      }
      else if(strcmp(argv[i],"-foutp")==0)
      {
         path_out_pal = READ_ARG(i);
      }
      else if(strcmp(argv[i],"-fpre")==0)
      {
         FILE *f = fopen(READ_ARG(i),"r");
         img2pixel_preset_load(&state,f);
         fclose(f);
      }
      else if(strcmp(argv[i],"-quantize")==0)
      {
         const char *next = READ_ARG(i);
         int colors = atoi(next);
         img2pixel_quantize(&state,colors,sprite_in);
      }
      else if(strcmp(argv[i],"-set")==0)
      {
         const char *name = READ_ARG(i); 
         const char *value = READ_ARG(i);
         parse_option(name,value);
      }
   }

   if(sprite_in==NULL)
   {
      printf("No input image specified\nTry %s -help\n",argv[0]); 
      return -1;
   }
   if(path_out==NULL)
   {
      printf("No output path specified\nTry %s -help\n",argv[0]);
      return -1;
   }

   //process image
   img2pixel_lowpass_image(&state,sprite_in,sprite_in);
   img2pixel_sharpen_image(&state,sprite_in,sprite_in);
   int width;
   int height;
   if(img2pixel_get_scale_mode(&state)==0)
   {
      width = img2pixel_get_out_width(&state);
      height = img2pixel_get_out_height(&state);
   }
   else
   {
      width = sprite_in->width/img2pixel_get_out_swidth(&state);
      height = sprite_in->height/img2pixel_get_out_sheight(&state);
   }
   SLK_RGB_sprite *out = SLK_rgb_sprite_create(width,height);
   img2pixel_process_image(&state,sprite_in,out);

   //Write image to output path
   image_save(&state,path_out,out,img2pixel_get_palette(&state));

   //Write palette to output path, if specified
   if(path_out_pal!=NULL)
      SLK_palette_save(path_out_pal,img2pixel_get_palette(&state));

   return 0;
}

static void print_help(int argc, char **argv)
{
   printf("%s usage:\n"
          "%s -fin filename -fout filename [-foutp filename] [-fpre filename] [-quantize colors] {-set option value}\n"
          "   -help\t\tprint this text\n"
          "   -fin\t\t\timage file to process\n"
          "   -fpre\t\tpreset to use\n"
          "   -fout\t\tfile to write output to (.png or .slk)\n"
          "   -foutp\t\tfile to write palette to (.pal)\n"
          "   -quantize [1,255]\ttry to automatically create a palette from the input image\n"
          "   -set\t\t\tmanually set parameters\n"
          "   option\t\tone of the following parameters\n"
          "      brightness [-255,255]\tbrightness adjustment\n"
          "      contrast [1,600]\t\tcontrast adjustment\n"
          "      gamma [1,800]\t\tgamma adjustment\n"
          "      saturation [1,600]\tsaturation adjustment\n"
          "      sharpen [0,100]\t\tsharpen kernel strength\n"
          "      hue [-360,360]\t\thue adjustment\n"
          "      dither [0,1000]\t\tdithering amount\n"
          "      alpha_threshold [0,255]\talpha threshold\n"
          "      gauss [0,500]\t\tgaussian blur strength\n"
          "      palette FILENAME\t\tpalette to use (.pal, .png, .gpl, .hex)\n"
          "      scale_mode [0,1]\t\twhether to scale image to fixed dimension (0) or scale down by factor (1)\n"
          "      sample_mode [0,5]\t\twhich downsampling algorithm to use (round,floor,ceil,linear,bicubic,lanczos)\n"
          "      dither_mode [0,7]\t\twhich dithering algorithm to use (none, bayer 8x8, bayer 4x4, bayer 2x2, cluster 8x8, cluster 4x4, steinberg per component, steinberg distributed error)\n"
          "      distance_mode [0,8]\twhich color space to use for distance calculation (rgb,CIE76,CIE94,CIEDE2000,XYZ,YCC,YIQ,YUV,K-Means)\n"
          "      width [1,infinity[\timage output width, only used if scale_mode is set to 0\n"
          "      height [1,infinity[\timage output height, only used if scale_mode is set to 0\n"
          "      scale_x [1,infinity[\timage output scale factor x-axis, only used if scale_mode is set to 1\n"
          "      scale_y [1,infinity[\timage output scale factor y-axis, only used if scale_mode is set to 1\n"
          "      offset_x [0,100]\t\tsampling x-axis offset\n"
          "      offset_y [0,100]\t\tsampling y-axis offset\n"
          "      inline [-1,255]\t\tinline color, -1 to disable\n"
          "      outline [-1,255]\t\toutline color, -1 to disable\n"
          "      upscale [1,infinity[\tfactor to nearest neighbour upscale the output image\n",
         argv[0],argv[0]);
}

static void parse_option(const char *name, const char *value)
{
   if(strcmp(name,"brightness")==0)
      img2pixel_set_brightness(&state,atoi(value));
   else if(strcmp(name,"contrast")==0)
      img2pixel_set_contrast(&state,atoi(value));
   else if(strcmp(name,"gamma")==0)
      img2pixel_set_gamma(&state,atoi(value));
   else if(strcmp(name,"saturation")==0)
      img2pixel_set_saturation(&state,atoi(value));
   else if(strcmp(name,"sharpen")==0)
      img2pixel_set_sharpen(&state,atoi(value));
   else if(strcmp(name,"hue")==0)
      img2pixel_set_hue(&state,atoi(value));
   else if(strcmp(name,"dither")==0)
      img2pixel_set_dither_amount(&state,atoi(value));
   else if(strcmp(name,"alpha_threshold")==0)
      img2pixel_set_alpha_threshold(&state,atoi(value));
   else if(strcmp(name,"gauss")==0)
      img2pixel_set_gauss(&state,atoi(value));
   else if(strcmp(name,"palette")==0)
      img2pixel_set_palette(&state,palette_load(value));
   else if(strcmp(name,"scale_mode")==0)
      img2pixel_set_scale_mode(&state,atoi(value));
   else if(strcmp(name,"sample_mode")==0)
      img2pixel_set_sample_mode(&state,atoi(value));
   else if(strcmp(name,"dither_mode")==0)
      img2pixel_set_process_mode(&state,atoi(value));
   else if(strcmp(name,"distance_mode")==0)
      img2pixel_set_distance_mode(&state,atoi(value));
   else if(strcmp(name,"width")==0)
      img2pixel_set_out_width(&state,atoi(value));
   else if(strcmp(name,"height")==0)
      img2pixel_set_out_height(&state,atoi(value));
   else if(strcmp(name,"scale_x")==0)
      img2pixel_set_out_swidth(&state,atoi(value));
   else if(strcmp(name,"scale_y")==0)
      img2pixel_set_out_sheight(&state,atoi(value));
   else if(strcmp(name,"offset_x")==0)
      img2pixel_set_offset_x(&state,atoi(value));
   else if(strcmp(name,"offset_y")==0)
      img2pixel_set_offset_y(&state,atoi(value));
   else if(strcmp(name,"inline")==0)
      img2pixel_set_inline(&state,atoi(value));
   else if(strcmp(name,"outline")==0)
      img2pixel_set_outline(&state,atoi(value));
   else if(strcmp(name,"upscale")==0)
      state.upscale = atoi(value);
}
//-------------------------------------
