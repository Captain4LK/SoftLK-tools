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
#include <SLK/SLK.h>
#include "../../external/cute_files.h"
//-------------------------------------

//Internal includes
#include "utility.h"
#include "image2pixel.h"
//-------------------------------------

//#defines
#define READ_ARG(I) \
   ((++(I))<argc?argv[(I)]:NULL)
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static SLK_RGB_sprite *sprite_in = NULL;
static const char *path_out;
//-------------------------------------

//Function prototypes
static void print_help(int argc, char **argv);
static void parse_option(const char *name, const char *value);
//-------------------------------------

//Function implementations

int main(int argc, char **argv)
{
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
      else if(strcmp(argv[i],"-fpre")==0)
      {
         FILE *f = fopen(READ_ARG(i),"r");
         img2pixel_preset_load(f);
         fclose(f);
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
   img2pixel_lowpass_image(sprite_in,sprite_in);
   img2pixel_sharpen_image(sprite_in,sprite_in);

   //Write image to output path

   return 0;
}

static void print_help(int argc, char **argv)
{
   printf("%s usage:\n"
          "%s -fin filename -fout filename [-fpre filename] {-set option value}\n"
          "   -help\tprint this text\n"
          "   -fin\t\timage file to process\n"
          "   -fout\tfile to write output to (.png or .slk)\n"
          "   -fpre\tpreset to use\n"
          "   -set\t\tmanually set parameters\n"
          "   option\tone of the following parameters\n"
          "      brightness [-255,255]\tbrightness adjustment\n"
          "      contrast [1,600]\t\tcontrast adjustment\n"
          "      gamma [1,800]\t\tgamma adjustment\n"
          "      saturation [1,600]\tsaturation adjustment\n"
          "      sharpen [0,100]\t\tsharpen kernel strength\n"
          "      dither [0,1000]\t\tdithering amount\n"
          "      alpha_threshold [0,255]\talpha threshold\n"
          "      gauss [0,500]\t\tgaussian blur strength\n"
          "      palette FILENAME\t\tpalette to use (.pal, .png, .gpl, .hex)\n"
          "      scale_mode [0,1]\t\twhether to scale image to fixed dimension (0) or scale down by factor (1)\n"
          "      sample_mode [0,6]\t\twhich downsampling algorithm to use (round,floor,ceil,linear,bicubic,supersampling,lanczos)\n"
          "      dither_mode [0,4]\t\twhich dithering algorithm to use (none, ordered with bias, odered without bias, steinberg per component, steinberg distributed error)\n"
          "      distance_mode [0,7]\twhich color space to use for distance calculation (rgb,CIE76,CIE94,CIEDE2000,XYZ,YCC,YIQ,YUV)\n"
          "      width [1,infinity[\timage output width, only used if scale_mode is set to 0\n"
          "      height [1,infinity[\timage output height, only used if scale_mode is set to 0\n"
          "      scale_x [1,infinity[\timage output scale factor x-axis, only used if scale_mode is set to 1\n"
          "      scale_y [1,infinity[\timage output scale factor y-axis, only used if scale_mode is set to 1\n",
         argv[0],argv[0]);
}

static void parse_option(const char *name, const char *value)
{
   if(strcmp(name,"brightness")==0)
      img2pixel_set_brightness(atoi(value));
   else if(strcmp(name,"contrast")==0)
      img2pixel_set_contrast(atoi(value));
   else if(strcmp(name,"gamma")==0)
      img2pixel_set_gamma(atoi(value));
   else if(strcmp(name,"saturation")==0)
      img2pixel_set_saturation(atoi(value));
   else if(strcmp(name,"sharpen")==0)
      img2pixel_set_sharpen(atoi(value));
   else if(strcmp(name,"dither")==0)
      img2pixel_set_dither_amount(atoi(value));
   else if(strcmp(name,"alpha_threshold")==0)
      img2pixel_set_alpha_threshold(atoi(value));
   else if(strcmp(name,"gauss")==0)
      img2pixel_set_gauss(atoi(value));
   else if(strcmp(name,"palette")==0)
      img2pixel_set_palette(palette_load(value));
   else if(strcmp(name,"scale_mode")==0)
      img2pixel_set_scale_mode(atoi(value));
   else if(strcmp(name,"sample_mode")==0)
      img2pixel_set_sample_mode(atoi(value));
   else if(strcmp(name,"dither_mode")==0)
      img2pixel_set_process_mode(atoi(value));
   else if(strcmp(name,"distance_mode")==0)
      img2pixel_set_distance_mode(atoi(value));
   else if(strcmp(name,"width")==0)
      img2pixel_set_out_width(atoi(value));
   else if(strcmp(name,"height")==0)
      img2pixel_set_out_height(atoi(value));
   else if(strcmp(name,"scale_x")==0)
      img2pixel_set_out_swidth(atoi(value));
   else if(strcmp(name,"scale_y")==0)
      img2pixel_set_out_sheight(atoi(value));
}
//-------------------------------------
