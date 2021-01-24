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
#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))
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
static float cubic_hermite (float a, float b, float c, float d, float t);
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
         int ix = (int)round(sx);
         int iy = (int)round(sy);

         SLK_Color c;
         SLK_Color c1,c2,c3,c4;
         c1 = SLK_rgb_sprite_get_pixel(in,ix,iy);
         c2 = SLK_rgb_sprite_get_pixel(in,ix+1,iy);
         c3 = SLK_rgb_sprite_get_pixel(in,ix,iy+1);
         c4 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+1);

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
         int ix = (int)round(sx);
         int iy = (int)round(sy);

         SLK_Color c00,c10,c20,c30;
         SLK_Color c01,c11,c21,c31;
         SLK_Color c02,c12,c22,c32;
         SLK_Color c03,c13,c23,c33;

         c00 = SLK_rgb_sprite_get_pixel(in,ix-1,iy-1);
         c10 = SLK_rgb_sprite_get_pixel(in,ix,iy-1);
         c20 = SLK_rgb_sprite_get_pixel(in,ix+1,iy-1);
         c30 = SLK_rgb_sprite_get_pixel(in,ix+2,iy-1);

         c01 = SLK_rgb_sprite_get_pixel(in,ix-1,iy);
         c11 = SLK_rgb_sprite_get_pixel(in,ix,iy);
         c21 = SLK_rgb_sprite_get_pixel(in,ix+1,iy);
         c31 = SLK_rgb_sprite_get_pixel(in,ix+2,iy);

         c02 = SLK_rgb_sprite_get_pixel(in,ix-1,iy+1);
         c12 = SLK_rgb_sprite_get_pixel(in,ix,iy+1);
         c22 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+1);
         c32 = SLK_rgb_sprite_get_pixel(in,ix+2,iy+1);

         c03 = SLK_rgb_sprite_get_pixel(in,ix-1,iy+2);
         c13 = SLK_rgb_sprite_get_pixel(in,ix,iy+2);
         c23 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+2);
         c33 = SLK_rgb_sprite_get_pixel(in,ix+2,iy+2);

         //r value
         float c0 = cubic_hermite((float)c00.r,(float)c10.r,(float)c20.r,(float)c30.r,six);
         float c1 = cubic_hermite((float)c01.r,(float)c11.r,(float)c21.r,(float)c31.r,six);
         float c2 = cubic_hermite((float)c02.r,(float)c12.r,(float)c22.r,(float)c32.r,six);
         float c3 = cubic_hermite((float)c03.r,(float)c13.r,(float)c23.r,(float)c33.r,six);
         float val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].r = MAX(0,MIN(255,(int)val));

         //g value
         c0 = cubic_hermite((float)c00.g,(float)c10.g,(float)c20.g,(float)c30.g,six);
         c1 = cubic_hermite((float)c01.g,(float)c11.g,(float)c21.g,(float)c31.g,six);
         c2 = cubic_hermite((float)c02.g,(float)c12.g,(float)c22.g,(float)c32.g,six);
         c3 = cubic_hermite((float)c03.g,(float)c13.g,(float)c23.g,(float)c33.g,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].g = MAX(0,MIN(255,(int)val));

         //b value
         c0 = cubic_hermite((float)c00.b,(float)c10.b,(float)c20.b,(float)c30.b,six);
         c1 = cubic_hermite((float)c01.b,(float)c11.b,(float)c21.b,(float)c31.b,six);
         c2 = cubic_hermite((float)c02.b,(float)c12.b,(float)c22.b,(float)c32.b,six);
         c3 = cubic_hermite((float)c03.b,(float)c13.b,(float)c23.b,(float)c33.b,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].b = MAX(0,MIN(255,(int)val));

         //a value
         c0 = cubic_hermite((float)c00.a,(float)c10.a,(float)c20.a,(float)c30.a,six);
         c1 = cubic_hermite((float)c01.a,(float)c11.a,(float)c21.a,(float)c31.a,six);
         c2 = cubic_hermite((float)c02.a,(float)c12.a,(float)c22.a,(float)c32.a,six);
         c3 = cubic_hermite((float)c03.a,(float)c13.a,(float)c23.a,(float)c33.a,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].a = MAX(0,MIN(255,(int)val));
      }
   }
}

static float cubic_hermite (float a, float b, float c, float d, float t)
{
   float a_ = -a/2.0f+(3.0f*b)/2.0f-(3.0f*c)/2.0f+d/2.0f;   
   float b_ = a-(5.0f*b)/2.0f+2.0f*c-d/2.0f; 
   float c_ = -a/2.0f+c/2.0f;
   float d_ = b;

   return a_*t*t*t+b_*t*t+c_*t+d_;
}
//-------------------------------------
