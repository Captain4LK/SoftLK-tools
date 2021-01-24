/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
#include <SLK/SLK.h>
//-------------------------------------

//Internal includes
#include "process.h"
#include "sample.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void sample_round(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height);
static void sample_floor(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height);
static void sample_ceil(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height);
static void sample_linear(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height);
static void sample_bicubic(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height);
//-------------------------------------

//Function implementations

void sample_image(const SLK_RGB_sprite *in, Big_pixel *out, int sample_mode, int width, int height)
{
   switch(sample_mode)
   {
   case 0: sample_round(in,out,width,height); break;
   case 1: sample_floor(in,out,width,height); break;
   case 2: sample_ceil(in,out,width,height); break;
   case 3: sample_linear(in,out,width,height); break;
   case 4: sample_bicubic(in,out,width,height); break;
   }
}

static void sample_round(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double sx = ((double)x/(double)width)*(double)in->width;   
         double sy = ((double)y/(double)height)*(double)in->height;   
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,round(sx),round(sy));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

static void sample_floor(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double sx = ((double)x/(double)width)*(double)in->width;   
         double sy = ((double)y/(double)height)*(double)in->height;   
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,floor(sx),floor(sy));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

static void sample_ceil(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double sx = ((double)x/(double)width)*(double)in->width;   
         double sy = ((double)y/(double)height)*(double)in->height;   
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,ceil(sx),ceil(sy));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

static void sample_linear(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double sx = ((double)x/(double)width)*(double)in->width;   
         double sy = ((double)y/(double)height)*(double)in->height;   
         double six = ((double)x/(double)width);
         double siy = ((double)y/(double)height);

         SLK_Color c;
         SLK_Color c1,c2,c3,c4;
         c1 = SLK_rgb_sprite_get_pixel(in,(sx),(sy));
         c2 = SLK_rgb_sprite_get_pixel(in,(sx+1),(sy));
         c3 = SLK_rgb_sprite_get_pixel(in,(sx),(sy+1));
         c4 = SLK_rgb_sprite_get_pixel(in,(sx+1),(sy+1));
         float c1t = ((1.0f-six)*(float)c1.r+six*(float)c2.r);
         float c2t = ((1.0f-six)*(float)c3.r+six*(float)c4.r);
         c.r = (int)((1.0f-siy)*c1t+siy*c2t);
         c1t = ((1.0f-six)*(float)c1.g+six*(float)c2.g);
         c2t = ((1.0f-six)*(float)c3.g+six*(float)c4.g);
         c.g = (int)((1.0f-siy)*c1t+siy*c2t);
         c1t = ((1.0f-six)*(float)c1.b+six*(float)c2.b);
         c2t = ((1.0f-six)*(float)c3.b+six*(float)c4.b);
         c.b = (int)((1.0f-siy)*c1t+siy*c2t);
         c1t = ((1.0f-six)*(float)c1.a+six*(float)c2.a);
         c2t = ((1.0f-six)*(float)c3.a+six*(float)c4.a);
         c.a = (int)((1.0f-siy)*c1t+siy*c2t);
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

static void sample_bicubic(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double sx = ((double)x/(double)width)*(double)in->width;   
         double sy = ((double)y/(double)height)*(double)in->height;   
         double six = ((double)x/(double)width);
         double siy = ((double)y/(double)height);

         SLK_Color c;
         SLK_Color c1,c2,c3,c4;
         c1 = SLK_rgb_sprite_get_pixel(in,floor(sx),floor(sy));
         c2 = SLK_rgb_sprite_get_pixel(in,ceil(sx),floor(sy));
         c3 = SLK_rgb_sprite_get_pixel(in,floor(sx),ceil(sy));
         c4 = SLK_rgb_sprite_get_pixel(in,ceil(sx),ceil(sy));
         float c1t = ((1.0f-six)*(float)c1.r+six*(float)c2.r);
         float c2t = ((1.0f-six)*(float)c3.r+six*(float)c4.r);
         c.r = (int)((1.0f-siy)*c1t+siy*c2t);
         c1t = ((1.0f-six)*(float)c1.g+six*(float)c2.g);
         c2t = ((1.0f-six)*(float)c3.g+six*(float)c4.g);
         c.g = (int)((1.0f-siy)*c1t+siy*c2t);
         c1t = ((1.0f-six)*(float)c1.b+six*(float)c2.b);
         c2t = ((1.0f-six)*(float)c3.b+six*(float)c4.b);
         c.b = (int)((1.0f-siy)*c1t+siy*c2t);
         c1t = ((1.0f-six)*(float)c1.a+six*(float)c2.a);
         c2t = ((1.0f-six)*(float)c3.a+six*(float)c4.a);
         c.a = (int)((1.0f-siy)*c1t+siy*c2t);
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}
//-------------------------------------
