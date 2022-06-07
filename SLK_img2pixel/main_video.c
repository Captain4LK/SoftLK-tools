/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LIBSLK_IMPLEMENTATION
#include "../headers/libSLK.h"

#define LIBSLK_GUI_IMPLEMENTATION
#include "../headers/libSLK_gui.h"

#include "../external/cute_files.h"
#include "../external/cute_path.h"
//-------------------------------------

//Internal includes
#include "utility.h"
#include "assets.h"
#include "image2pixel.h"
//-------------------------------------

//#defines
#define READ_ARG(I) \
   ((++(I))<argc?argv[(I)]:NULL)
//-------------------------------------

//Typedefs
typedef struct
{
   size_t width;
   size_t height;
   uint8_t data[];
}Frame;
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void print_help(int argc, char **argv);
static void parse_option(const char *name, const char *value);

//static int video_open(const char *path);

static Frame *frame_create(size_t width, size_t height);
static void frame_write(Frame *f);
static Frame *frame_read(Frame *f);
//-------------------------------------

//Function implementations

int main(int argc, char **argv)
{
   //Load default palette
   img2pixel_set_palette(assets_load_pal_default());

   //parse args
   for(int i = 1;i<argc;i++)
   {
      if(strcmp(argv[i],"-help")==0||
         strcmp(argv[i],"--help")==0)
      {
         print_help(argc,argv);
         return 0;
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

   Frame *fin = frame_read(NULL); 
   int width;
   int height;
   if(img2pixel_get_scale_mode()==0)
   {
      width = img2pixel_get_out_width();
      height = img2pixel_get_out_height();
   }
   else
   {
      width = fin->width/img2pixel_get_out_swidth();
      height = fin->height/img2pixel_get_out_sheight();
   }
   Frame *fout = frame_create(width*upscale,height*upscale);

   SLK_RGB_sprite *sin = SLK_rgb_sprite_create(fin->width,fin->height);
   SLK_RGB_sprite *sout = SLK_rgb_sprite_create(width,height);

   do
   {
      for(int i = 0;i<fin->width*fin->height;i++)
      {
         sin->data[i].rgb.r = fin->data[i*3];
         sin->data[i].rgb.g = fin->data[i*3+1];
         sin->data[i].rgb.b = fin->data[i*3+2];
         sin->data[i].rgb.a = 255;
      }

      //process image
      img2pixel_lowpass_image(sin,sin);
      img2pixel_sharpen_image(sin,sin);
      img2pixel_process_image(sin,sout);

      for(int y = 0;y<sout->height;y++)
      {
         for(int x = 0;x<sout->width;x++)
         {
            for(int y_ = 0;y_<upscale;y_++)
            {
               for(int x_ = 0;x_<upscale;x_++)
               {
                  fout->data[(y*upscale+y_)*3*fout->width+(x*upscale+x_)*3] = SLK_rgb_sprite_get_pixel(sout,x,y).rgb.r;
                  fout->data[(y*upscale+y_)*3*fout->width+(x*upscale+x_)*3+1] = SLK_rgb_sprite_get_pixel(sout,x,y).rgb.g;
                  fout->data[(y*upscale+y_)*3*fout->width+(x*upscale+x_)*3+2] = SLK_rgb_sprite_get_pixel(sout,x,y).rgb.b;
               }
            }
         }
      }

     frame_write(fout);
   }while((fin = frame_read(fin))!=NULL);

   return 0;
}

static void print_help(int argc, char **argv)
{
   printf("%s usage:\n"
          "%s -fin filename -fout filename [-foutp filename] [-fpre filename] [-quantize colors] {-set option value}\n"
          "   -help\t\tprint this text\n"
          "   -fin\t\t\tbideo file to process\n"
          "   -fpre\t\tpreset to use\n"
          "   -fout\t\tfile to write output to\n"
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
          "      sample_mode [0,6]\t\twhich downsampling algorithm to use (round,floor,ceil,linear,bicubic,supersampling,lanczos)\n"
          "      dither_mode [0,7]\t\twhich dithering algorithm to use (none, bayer 8x8, bayer 4x4, bayer 2x2, cluster 8x8, cluster 4x4, steinberg per component, steinberg distributed error)\n"
          "      distance_mode [0,7]\twhich color space to use for distance calculation (rgb,CIE76,CIE94,CIEDE2000,XYZ,YCC,YIQ,YUV)\n"
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
      img2pixel_set_brightness(atoi(value));
   else if(strcmp(name,"contrast")==0)
      img2pixel_set_contrast(atoi(value));
   else if(strcmp(name,"gamma")==0)
      img2pixel_set_gamma(atoi(value));
   else if(strcmp(name,"saturation")==0)
      img2pixel_set_saturation(atoi(value));
   else if(strcmp(name,"sharpen")==0)
      img2pixel_set_sharpen(atoi(value));
   else if(strcmp(name,"hue")==0)
      img2pixel_set_hue(atoi(value));
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
   else if(strcmp(name,"offset_x")==0)
      img2pixel_set_offset_x(atoi(value));
   else if(strcmp(name,"offset_y")==0)
      img2pixel_set_offset_y(atoi(value));
   else if(strcmp(name,"inline")==0)
      img2pixel_set_inline(atoi(value));
   else if(strcmp(name,"outline")==0)
      img2pixel_set_outline(atoi(value));
   else if(strcmp(name,"upscale")==0)
      upscale = atoi(value);
}

static Frame *frame_create(size_t width, size_t height)
{
    Frame *f = malloc(sizeof(*f)+width*height*3);
    f->width = width;
    f->height = height;
    return f;
}

static void frame_write(Frame *f)
{
    printf("P6\n%zu %zu\n255\n",f->width,f->height);
    fwrite(f->data,f->width*f->height,3,stdout);
}

static Frame *frame_read(Frame *f)
{
    size_t width, height;
    if(scanf("P6 %zu%zu%*d%*c", &width, &height) < 2) {
        free(f);
        return 0;
    }
    if (!f || f->width != width || f->height != height) {
        free(f);
        f = frame_create(width, height);
    }
    fread(f->data, width * height, 3, stdin);
    return f;
}
//-------------------------------------
