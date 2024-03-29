/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "HLH.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
//-------------------------------------

//#defines
#define PI32 3.14159265359f
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static SLK_image64 *slk_sample_round(const SLK_image64 *img, int width, int height, float x_off, float y_off);
static SLK_image64 *slk_sample_floor(const SLK_image64 *img, int width, int height, float x_off, float y_off);
static SLK_image64 *slk_sample_ceil(const SLK_image64 *img, int width, int height, float x_off, float y_off);
static SLK_image64 *slk_sample_linear(const SLK_image64 *img, int width, int height, float x_off, float y_off);
static SLK_image64 *slk_sample_bicubic(const SLK_image64 *img, int width, int height, float x_off, float y_off);
static SLK_image64 *slk_sample_lanczos(const SLK_image64 *img, int width, int height, float x_off, float y_off);

static float slk_blend_linear(float sx, float sy, float c0, float c1, float c2, float c3);
static float slk_blend_bicubic(float c0, float c1, float c2, float c3, float t);
static float slk_lanczos(float v);
//-------------------------------------

//Function implementations

SLK_image64 *SLK_image64_sample(const SLK_image64 *img, int width, int height, int sample_mode, float x_off, float y_off)
{
   //TODO(Captain4LK): integer only math?

   switch(sample_mode)
   {
   case 0: return slk_sample_round(img,width,height,x_off,y_off);
   case 1: return slk_sample_floor(img,width,height,x_off,y_off);
   case 2: return slk_sample_ceil(img,width,height,x_off,y_off);
   case 3: return slk_sample_linear(img,width,height,x_off,y_off);
   case 4: return slk_sample_bicubic(img,width,height,x_off,y_off);
   case 5: return slk_sample_lanczos(img,width,height,x_off,y_off);
   }

   return NULL;
}

static SLK_image64 *slk_sample_round(const SLK_image64 *img, int width, int height, float x_off, float y_off)
{
   SLK_image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->w = width;
   out->h = height;

   float w = (img->w-1)/(float)width;
   float h = (img->h-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         float dx = x+x_off;
         float dy = y+y_off;

         out->data[y*width+x] = img->data[(int)roundf(dy*h)*img->w+(int)roundf(dx*w)];
      }
   }

   return out;
}

static SLK_image64 *slk_sample_floor(const SLK_image64 *img, int width, int height, float x_off, float y_off)
{
   SLK_image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->w = width;
   out->h = height;

   float w = (img->w-1)/(float)width;
   float h = (img->h-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         float dx = x+x_off;
         float dy = y+y_off;

         out->data[y*width+x] = img->data[(int)floorf(dy*h)*img->w+(int)floorf(dx*w)];
      }
   }

   return out;
}

static SLK_image64 *slk_sample_ceil(const SLK_image64 *img, int width, int height, float x_off, float y_off)
{
   SLK_image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->w = width;
   out->h = height;

   float w = (img->w-1)/(float)width;
   float h = (img->h-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         float dx = x+x_off;
         float dy = y+y_off;

         out->data[y*width+x] = img->data[(int)ceilf(dy*h)*img->w+(int)ceilf(dx*w)];
      }
   }

   return out;
}

static SLK_image64 *slk_sample_linear(const SLK_image64 *img, int width, int height, float x_off, float y_off)
{
   SLK_image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->w = width;
   out->h = height;

   float fw = (float)(img->w-1)/(float)width;
   float fh = (float)(img->h-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((float)x+x_off)*fw);
         int iy = (int)(((float)y+y_off)*fh);
         float six = (((float)x+x_off)*fw)-(float)ix;
         float siy = (((float)y+y_off)*fh)-(float)iy;

         uint64_t p0 = UINT64_C(0x7fff000000000000);
         uint64_t p1 = UINT64_C(0x7fff000000000000);
         uint64_t p2 = UINT64_C(0x7fff000000000000);
         uint64_t p3 = UINT64_C(0x7fff000000000000);

         if(ix>=0&&ix<img->w&&iy>=0&&iy<img->h)
            p0 = img->data[iy*img->w+ix];
         if(ix+1>=0&&ix+1<img->w&&iy>=0&&iy<img->h)
            p1 = img->data[iy*img->w+ix+1];
         if(ix>=0&&ix<img->w&&iy+1>=0&&iy+1<img->h)
            p2 = img->data[(iy+1)*img->w+ix];
         if(ix+1>=0&&ix+1<img->w&&iy+1>=0&&iy+1<img->h)
            p3 = img->data[(iy+1)*img->w+ix+1];

         float c0 = slk_blend_linear(six,siy,(float)SLK_color64_r(p0),(float)SLK_color64_r(p1),(float)SLK_color64_r(p2),(float)SLK_color64_r(p3));
         float c1 = slk_blend_linear(six,siy,(float)SLK_color64_g(p0),(float)SLK_color64_g(p1),(float)SLK_color64_g(p2),(float)SLK_color64_g(p3));
         float c2 = slk_blend_linear(six,siy,(float)SLK_color64_b(p0),(float)SLK_color64_b(p1),(float)SLK_color64_b(p2),(float)SLK_color64_b(p3));
         float c3 = slk_blend_linear(six,siy,(float)SLK_color64_a(p0),(float)SLK_color64_a(p1),(float)SLK_color64_a(p2),(float)SLK_color64_a(p3));

         uint64_t r = HLH_max(0,HLH_min(0x7fff,(int)c0));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,(int)c1));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,(int)c2));
         uint64_t a = HLH_max(0,HLH_min(0x7fff,(int)c3));

         out->data[y*out->w+x] = (r)|(g<<16)|(b<<32)|(a<<48);

#if 0
   //color = ((color&0xff000000)<<24) 
         //float c0 = (float)(

         SLK_Color c;
         SLK_Color c1,c2,c3,c4;
         c1 = SLK_rgb_sprite_get_pixel(in,ix,iy);
         c2 = SLK_rgb_sprite_get_pixel(in,ix+1,iy);
         c3 = SLK_rgb_sprite_get_pixel(in,ix,iy+1);
         c4 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+1);

         //r value
         float c1t = ((1.0f-six)*(float)c1.rgb.r+six*(float)c2.rgb.r);
         float c2t = ((1.0f-six)*(float)c3.rgb.r+six*(float)c4.rgb.r);
         c.rgb.r = (int)((1.0f-siy)*c1t+siy*c2t);

         //g value
         c1t = ((1.0f-six)*(float)c1.rgb.g+six*(float)c2.rgb.g);
         c2t = ((1.0f-six)*(float)c3.rgb.g+six*(float)c4.rgb.g);
         c.rgb.g = (int)((1.0f-siy)*c1t+siy*c2t);

         //b value
         c1t = ((1.0f-six)*(float)c1.rgb.b+six*(float)c2.rgb.b);
         c2t = ((1.0f-six)*(float)c3.rgb.b+six*(float)c4.rgb.b);
         c.rgb.b = (int)((1.0f-siy)*c1t+siy*c2t);

         //a value
         c1t = ((1.0f-six)*(float)c1.rgb.a+six*(float)c2.rgb.a);
         c2t = ((1.0f-six)*(float)c3.rgb.a+six*(float)c4.rgb.a);
         c.rgb.a = (int)((1.0f-siy)*c1t+siy*c2t);

         out[y*width+x].rgb.r = c.rgb.r;
         out[y*width+x].rgb.b = c.rgb.b;
         out[y*width+x].rgb.g = c.rgb.g;
         out[y*width+x].rgb.a = c.rgb.a;
#endif
      }
   }

   return out;
}

static float slk_blend_linear(float sx, float sy, float c0, float c1, float c2, float c3)
{
   float t0 = (1.f-sx)*c0+sx*c1;
   float t1 = (1.f-sx)*c2+sx*c3;
   return (1.f-sy)*t0+sy*t1;
}

static SLK_image64 *slk_sample_bicubic(const SLK_image64 *img, int width, int height, float x_off, float y_off)
{
   SLK_image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->w = width;
   out->h = height;

   float fw = (float)(img->w-1)/(float)width;
   float fh = (float)(img->h-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((float)x+x_off)*fw);
         int iy = (int)(((float)y+y_off)*fh);
         float six = (((float)x+x_off)*fw)-(float)ix;
         float siy = (((float)y+y_off)*fh)-(float)iy;

         uint64_t p00 = UINT64_C(0x7fff000000000000);
         uint64_t p01 = UINT64_C(0x7fff000000000000);
         uint64_t p02 = UINT64_C(0x7fff000000000000);
         uint64_t p03 = UINT64_C(0x7fff000000000000);
         uint64_t p10 = UINT64_C(0x7fff000000000000);
         uint64_t p11 = UINT64_C(0x7fff000000000000);
         uint64_t p12 = UINT64_C(0x7fff000000000000);
         uint64_t p13 = UINT64_C(0x7fff000000000000);
         uint64_t p20 = UINT64_C(0x7fff000000000000);
         uint64_t p21 = UINT64_C(0x7fff000000000000);
         uint64_t p22 = UINT64_C(0x7fff000000000000);
         uint64_t p23 = UINT64_C(0x7fff000000000000);
         uint64_t p30 = UINT64_C(0x7fff000000000000);
         uint64_t p31 = UINT64_C(0x7fff000000000000);
         uint64_t p32 = UINT64_C(0x7fff000000000000);
         uint64_t p33 = UINT64_C(0x7fff000000000000);

         if(ix-1>=0&&ix-1<img->w&&iy-1>=0&&iy-1<img->h)
            p00 = img->data[(iy-1)*img->w+ix-1];
         if(ix>=0&&ix<img->w&&iy-1>=0&&iy-1<img->h)
            p01 = img->data[(iy-1)*img->w+ix];
         if(ix+1>=0&&ix+1<img->w&&iy-1>=0&&iy-1<img->h)
            p02 = img->data[(iy-1)*img->w+ix+1];
         if(ix+2>=0&&ix+2<img->w&&iy-1>=0&&iy-1<img->h)
            p03 = img->data[(iy-1)*img->w+ix+2];

         if(ix-1>=0&&ix-1<img->w&&iy>=0&&iy<img->h)
            p10 = img->data[(iy)*img->w+ix-1];
         if(ix>=0&&ix<img->w&&iy>=0&&iy<img->h)
            p11 = img->data[(iy)*img->w+ix];
         if(ix+1>=0&&ix+1<img->w&&iy>=0&&iy<img->h)
            p12 = img->data[(iy)*img->w+ix+1];
         if(ix+2>=0&&ix+2<img->w&&iy>=0&&iy<img->h)
            p13 = img->data[(iy)*img->w+ix+2];

         if(ix-1>=0&&ix-1<img->w&&iy+1>=0&&iy+1<img->h)
            p20 = img->data[(iy+1)*img->w+ix-1];
         if(ix>=0&&ix<img->w&&iy+1>=0&&iy+1<img->h)
            p21 = img->data[(iy+1)*img->w+ix];
         if(ix+1>=0&&ix+1<img->w&&iy+1>=0&&iy+1<img->h)
            p22 = img->data[(iy+1)*img->w+ix+1];
         if(ix+2>=0&&ix+2<img->w&&iy+1>=0&&iy+1<img->h)
            p23 = img->data[(iy+1)*img->w+ix+2];

         if(ix-1>=0&&ix-1<img->w&&iy+2>=0&&iy+2<img->h)
            p30 = img->data[(iy+2)*img->w+ix-1];
         if(ix>=0&&ix<img->w&&iy+2>=0&&iy+2<img->h)
            p31 = img->data[(iy+2)*img->w+ix];
         if(ix+1>=0&&ix+1<img->w&&iy+2>=0&&iy+2<img->h)
            p32 = img->data[(iy+2)*img->w+ix+1];
         if(ix+2>=0&&ix+2<img->w&&iy+2>=0&&iy+2<img->h)
            p33 = img->data[(iy+2)*img->w+ix+2];

         float c0 = slk_blend_bicubic((float)SLK_color64_r(p00),(float)SLK_color64_r(p01),(float)SLK_color64_r(p02),(float)SLK_color64_r(p03),six);
         float c1 = slk_blend_bicubic((float)SLK_color64_r(p10),(float)SLK_color64_r(p11),(float)SLK_color64_r(p12),(float)SLK_color64_r(p13),six);
         float c2 = slk_blend_bicubic((float)SLK_color64_r(p20),(float)SLK_color64_r(p21),(float)SLK_color64_r(p22),(float)SLK_color64_r(p23),six);
         float c3 = slk_blend_bicubic((float)SLK_color64_r(p30),(float)SLK_color64_r(p31),(float)SLK_color64_r(p32),(float)SLK_color64_r(p33),six);
         uint64_t r = HLH_max(0,HLH_min(0x7fff,(int)slk_blend_bicubic(c0,c1,c2,c3,siy)));

         c0 = slk_blend_bicubic((float)SLK_color64_g(p00),(float)SLK_color64_g(p01),(float)SLK_color64_g(p02),(float)SLK_color64_g(p03),six);
         c1 = slk_blend_bicubic((float)SLK_color64_g(p10),(float)SLK_color64_g(p11),(float)SLK_color64_g(p12),(float)SLK_color64_g(p13),six);
         c2 = slk_blend_bicubic((float)SLK_color64_g(p20),(float)SLK_color64_g(p21),(float)SLK_color64_g(p22),(float)SLK_color64_g(p23),six);
         c3 = slk_blend_bicubic((float)SLK_color64_g(p30),(float)SLK_color64_g(p31),(float)SLK_color64_g(p32),(float)SLK_color64_g(p33),six);
         uint64_t g = HLH_max(0,HLH_min(0x7fff,(int)slk_blend_bicubic(c0,c1,c2,c3,siy)));

         c0 = slk_blend_bicubic((float)SLK_color64_b(p00),(float)SLK_color64_b(p01),(float)SLK_color64_b(p02),(float)SLK_color64_b(p03),six);
         c1 = slk_blend_bicubic((float)SLK_color64_b(p10),(float)SLK_color64_b(p11),(float)SLK_color64_b(p12),(float)SLK_color64_b(p13),six);
         c2 = slk_blend_bicubic((float)SLK_color64_b(p20),(float)SLK_color64_b(p21),(float)SLK_color64_b(p22),(float)SLK_color64_b(p23),six);
         c3 = slk_blend_bicubic((float)SLK_color64_b(p30),(float)SLK_color64_b(p31),(float)SLK_color64_b(p32),(float)SLK_color64_b(p33),six);
         uint64_t b = HLH_max(0,HLH_min(0x7fff,(int)slk_blend_bicubic(c0,c1,c2,c3,siy)));

         c0 = slk_blend_bicubic((float)SLK_color64_a(p00),(float)SLK_color64_a(p01),(float)SLK_color64_a(p02),(float)SLK_color64_a(p03),six);
         c1 = slk_blend_bicubic((float)SLK_color64_a(p10),(float)SLK_color64_a(p11),(float)SLK_color64_a(p12),(float)SLK_color64_a(p13),six);
         c2 = slk_blend_bicubic((float)SLK_color64_a(p20),(float)SLK_color64_a(p21),(float)SLK_color64_a(p22),(float)SLK_color64_a(p23),six);
         c3 = slk_blend_bicubic((float)SLK_color64_a(p30),(float)SLK_color64_a(p31),(float)SLK_color64_a(p32),(float)SLK_color64_a(p33),six);
         uint64_t a = HLH_max(0,HLH_min(0x7fff,(int)slk_blend_bicubic(c0,c1,c2,c3,siy)));

         out->data[y*out->w+x] = (r)|(g<<16)|(b<<32)|(a<<48);
#if 0
         int ix = (int)(((float)x+foffx)*fw);
         int iy = (int)(((float)y+foffy)*fh);
         float six = (((float)x+foffx)*fw)-(float)ix;
         float siy = (((float)y+foffy)*fh)-(float)iy;

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
         float c0 = cubic_hermite((float)c00.rgb.r,(float)c10.rgb.r,(float)c20.rgb.r,(float)c30.rgb.r,six);
         float c1 = cubic_hermite((float)c01.rgb.r,(float)c11.rgb.r,(float)c21.rgb.r,(float)c31.rgb.r,six);
         float c2 = cubic_hermite((float)c02.rgb.r,(float)c12.rgb.r,(float)c22.rgb.r,(float)c32.rgb.r,six);
         float c3 = cubic_hermite((float)c03.rgb.r,(float)c13.rgb.r,(float)c23.rgb.r,(float)c33.rgb.r,six);
         float val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].rgb.r = HLH_max(0x0,HLH_min(0xff,(int)val));

         //g value
         c0 = cubic_hermite((float)c00.rgb.g,(float)c10.rgb.g,(float)c20.rgb.g,(float)c30.rgb.g,six);
         c1 = cubic_hermite((float)c01.rgb.g,(float)c11.rgb.g,(float)c21.rgb.g,(float)c31.rgb.g,six);
         c2 = cubic_hermite((float)c02.rgb.g,(float)c12.rgb.g,(float)c22.rgb.g,(float)c32.rgb.g,six);
         c3 = cubic_hermite((float)c03.rgb.g,(float)c13.rgb.g,(float)c23.rgb.g,(float)c33.rgb.g,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].rgb.g = HLH_max(0x0,HLH_min(0xff,(int)val));

         //b value
         c0 = cubic_hermite((float)c00.rgb.b,(float)c10.rgb.b,(float)c20.rgb.b,(float)c30.rgb.b,six);
         c1 = cubic_hermite((float)c01.rgb.b,(float)c11.rgb.b,(float)c21.rgb.b,(float)c31.rgb.b,six);
         c2 = cubic_hermite((float)c02.rgb.b,(float)c12.rgb.b,(float)c22.rgb.b,(float)c32.rgb.b,six);
         c3 = cubic_hermite((float)c03.rgb.b,(float)c13.rgb.b,(float)c23.rgb.b,(float)c33.rgb.b,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].rgb.b = HLH_max(0x0,HLH_min(0xff,(int)val));

         //a value
         c0 = cubic_hermite((float)c00.rgb.a,(float)c10.rgb.a,(float)c20.rgb.a,(float)c30.rgb.a,six);
         c1 = cubic_hermite((float)c01.rgb.a,(float)c11.rgb.a,(float)c21.rgb.a,(float)c31.rgb.a,six);
         c2 = cubic_hermite((float)c02.rgb.a,(float)c12.rgb.a,(float)c22.rgb.a,(float)c32.rgb.a,six);
         c3 = cubic_hermite((float)c03.rgb.a,(float)c13.rgb.a,(float)c23.rgb.a,(float)c33.rgb.a,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].rgb.a = HLH_max(0x0,HLH_min(0xff,(int)val));
#endif
      }
   }

   return out;
}

static float slk_blend_bicubic(float c0, float c1, float c2, float c3, float t)
{
   float a0 = -0.5f*c0+1.5f*c1-1.5f*c2+0.5f*c3;
   float a1 = c0-2.5f*c1+2.f*c2-0.5*c3;
   float a2 = -0.5f*c0+0.5f*c2;
   float a3 = c1;

   return a0*t*t*t+a1*t*t+a2*t+a3;
}

static SLK_image64 *slk_sample_lanczos(const SLK_image64 *img, int width, int height, float x_off, float y_off)
{
   SLK_image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->w = width;
   out->h = height;

   float fw = (float)(img->w-1)/(float)width;
   float fh = (float)(img->h-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((float)x+x_off)*fw);
         int iy = (int)(((float)y+y_off)*fh);
         float six = (((float)x+x_off)*fw)-(float)ix;
         float siy = (((float)y+y_off)*fh)-(float)iy;

         float a0 = slk_lanczos(six+2.f);
         float a1 = slk_lanczos(six+1.f);
         float a2 = slk_lanczos(six);
         float a3 = slk_lanczos(six-1.f);
         float a4 = slk_lanczos(six-2.f);
         float a5 = slk_lanczos(six-3.f);
         float b0 = slk_lanczos(siy+2.f);
         float b1 = slk_lanczos(siy+1.f);
         float b2 = slk_lanczos(siy);
         float b3 = slk_lanczos(siy-1.f);
         float b4 = slk_lanczos(siy-2.f);
         float b5 = slk_lanczos(siy-3.f);

         float cr[6] = {0};
         float cg[6] = {0};
         float cb[6] = {0};
         float ca[6] = {0};
         for(int i = 0;i<6;i++)
         {
            uint64_t p00 = UINT64_C(0x7fff000000000000);
            uint64_t p01 = UINT64_C(0x7fff000000000000);
            uint64_t p02 = UINT64_C(0x7fff000000000000);
            uint64_t p03 = UINT64_C(0x7fff000000000000);
            uint64_t p04 = UINT64_C(0x7fff000000000000);
            uint64_t p05 = UINT64_C(0x7fff000000000000);

            if(ix-2>=0&&ix-2<img->w&&iy-2+i>=0&&iy-2+i<img->h)
               p00 = img->data[(iy-2+i)*img->w+ix-2];
            if(ix-1>=0&&ix-1<img->w&&iy-2+i>=0&&iy-2+i<img->h)
               p01 = img->data[(iy-2+i)*img->w+ix-1];
            if(ix>=0&&ix<img->w&&iy-2+i>=0&&iy-2+i<img->h)
               p02 = img->data[(iy-2+i)*img->w+ix];
            if(ix+1>=0&&ix+1<img->w&&iy-2+i>=0&&iy-2+i<img->h)
               p03 = img->data[(iy-2+i)*img->w+ix+1];
            if(ix+2>=0&&ix+2<img->w&&iy-2+i>=0&&iy-2+i<img->h)
               p04 = img->data[(iy-2+i)*img->w+ix+2];
            if(ix+3>=0&&ix+3<img->w&&iy-2+i>=0&&iy-2+i<img->h)
               p05 = img->data[(iy-2+i)*img->w+ix+3];

            cr[i] = a0*(float)SLK_color64_r(p00)+a1*(float)SLK_color64_r(p01)+a2*(float)SLK_color64_r(p02)+a3*(float)SLK_color64_r(p03)+a4*(float)SLK_color64_r(p04)+a5*(float)SLK_color64_r(p05);
            cg[i] = a0*(float)SLK_color64_g(p00)+a1*(float)SLK_color64_g(p01)+a2*(float)SLK_color64_g(p02)+a3*(float)SLK_color64_g(p03)+a4*(float)SLK_color64_g(p04)+a5*(float)SLK_color64_g(p05);
            cb[i] = a0*(float)SLK_color64_b(p00)+a1*(float)SLK_color64_b(p01)+a2*(float)SLK_color64_b(p02)+a3*(float)SLK_color64_b(p03)+a4*(float)SLK_color64_b(p04)+a5*(float)SLK_color64_b(p05);
            ca[i] = a0*(float)SLK_color64_a(p00)+a1*(float)SLK_color64_a(p01)+a2*(float)SLK_color64_a(p02)+a3*(float)SLK_color64_a(p03)+a4*(float)SLK_color64_a(p04)+a5*(float)SLK_color64_a(p05);
         }

         uint64_t r = HLH_max(0,HLH_min(0x7fff,b0*cr[0]+b1*cr[1]+b2*cr[2]+b3*cr[3]+b4*cr[4]+b5*cr[5]));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,b0*cg[0]+b1*cg[1]+b2*cg[2]+b3*cg[3]+b4*cg[4]+b5*cg[5]));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,b0*cb[0]+b1*cb[1]+b2*cb[2]+b3*cb[3]+b4*cb[4]+b5*cb[5]));
         uint64_t a = HLH_max(0,HLH_min(0x7fff,b0*ca[0]+b1*ca[1]+b2*ca[2]+b3*ca[3]+b4*ca[4]+b5*ca[5]));

         out->data[y*out->w+x] = (r)|(g<<16)|(b<<32)|(a<<48);
      }
   }

   return out;
}

static float slk_lanczos(float v)
{
   if(v==0.f)
      return 1.f;
   if(v>3.f||v<-3.f)
      return 0.f;

   return ((3.f*sinf(PI32*v)*sinf(PI32*v/3.f))/(PI32*PI32*v*v));
}
//-------------------------------------
