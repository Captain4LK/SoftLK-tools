/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
#include <math.h>
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
static void sample_supersample(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height);
static void sample_lanczos(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height);
static double lanczos(double v);
//-------------------------------------

//Function implementations

//Downsamples an image to the specified dimensions
void sample_image(const SLK_RGB_sprite *in, Big_pixel *out, int sample_mode, int width, int height)
{
   switch(sample_mode)
   {
   case 0: sample_round(in,out,width,height); break;
   case 1: sample_floor(in,out,width,height); break;
   case 2: sample_ceil(in,out,width,height); break;
   case 3: sample_linear(in,out,width,height); break;
   case 4: sample_bicubic(in,out,width,height); break;
   case 5: sample_supersample(in,out,width,height); break;
   case 6: sample_lanczos(in,out,width,height); break;
   }
}

//Nearest neighbour sampling,
//rounding the position
static void sample_round(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,round((float)x*fw),round((float)y*fh));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

//Nearest neighbour sampling,
//flooring the position
static void sample_floor(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,floor((float)x*fw),floor((float)y*fh));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

//Nearest neighbour sampling,
//ceiling the position
static void sample_ceil(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,ceil((float)x*fw),ceil((float)y*fh));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

//Bilinear sampling
static void sample_linear(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)((float)x*fw);
         int iy = (int)((float)y*fh);
         float six = ((float)x*fw)-(float)ix;
         float siy = ((float)y*fh)-(float)iy;

         SLK_Color c;
         SLK_Color c1,c2,c3,c4;
         c1 = SLK_rgb_sprite_get_pixel(in,ix,iy);
         c2 = SLK_rgb_sprite_get_pixel(in,ix+1,iy);
         c3 = SLK_rgb_sprite_get_pixel(in,ix,iy+1);
         c4 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+1);

         //r value
         float c1t = ((1.0f-six)*(float)c1.r+six*(float)c2.r);
         float c2t = ((1.0f-six)*(float)c3.r+six*(float)c4.r);
         c.r = (int)((1.0f-siy)*c1t+siy*c2t);

         //g value
         c1t = ((1.0f-six)*(float)c1.g+six*(float)c2.g);
         c2t = ((1.0f-six)*(float)c3.g+six*(float)c4.g);
         c.g = (int)((1.0f-siy)*c1t+siy*c2t);

         //b value
         c1t = ((1.0f-six)*(float)c1.b+six*(float)c2.b);
         c2t = ((1.0f-six)*(float)c3.b+six*(float)c4.b);
         c.b = (int)((1.0f-siy)*c1t+siy*c2t);

         //a value
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

//Bicubic sampling
static void sample_bicubic(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)((float)x*fw);
         int iy = (int)((float)y*fh);
         float six = (x*fw)-(float)ix;
         float siy = (y*fh)-(float)iy;

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
         out[y*width+x].r = MAX(0,MIN(0xff,(int)val));

         //g value
         c0 = cubic_hermite((float)c00.g,(float)c10.g,(float)c20.g,(float)c30.g,six);
         c1 = cubic_hermite((float)c01.g,(float)c11.g,(float)c21.g,(float)c31.g,six);
         c2 = cubic_hermite((float)c02.g,(float)c12.g,(float)c22.g,(float)c32.g,six);
         c3 = cubic_hermite((float)c03.g,(float)c13.g,(float)c23.g,(float)c33.g,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].g = MAX(0,MIN(0xff,(int)val));

         //b value
         c0 = cubic_hermite((float)c00.b,(float)c10.b,(float)c20.b,(float)c30.b,six);
         c1 = cubic_hermite((float)c01.b,(float)c11.b,(float)c21.b,(float)c31.b,six);
         c2 = cubic_hermite((float)c02.b,(float)c12.b,(float)c22.b,(float)c32.b,six);
         c3 = cubic_hermite((float)c03.b,(float)c13.b,(float)c23.b,(float)c33.b,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].b = MAX(0,MIN(0xff,(int)val));

         //a value
         c0 = cubic_hermite((float)c00.a,(float)c10.a,(float)c20.a,(float)c30.a,six);
         c1 = cubic_hermite((float)c01.a,(float)c11.a,(float)c21.a,(float)c31.a,six);
         c2 = cubic_hermite((float)c02.a,(float)c12.a,(float)c22.a,(float)c32.a,six);
         c3 = cubic_hermite((float)c03.a,(float)c13.a,(float)c23.a,(float)c33.a,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].a = MAX(0,MIN(0xff,(int)val));
      }
   }
}

//Helper function for sample_bicubic
static float cubic_hermite (float a, float b, float c, float d, float t)
{
   float a_ = -a/2.0f+(3.0f*b)/2.0f-(3.0f*c)/2.0f+d/2.0f;   
   float b_ = a-(5.0f*b)/2.0f+2.0f*c-d/2.0f; 
   float c_ = -a/2.0f+c/2.0f;
   float d_ = b;

   return a_*t*t*t+b_*t*t+c_*t+d_;
}

//Supersampling --> works best in most cases
static void sample_supersample(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         Big_pixel p = {0};
         float n = 0.0f;
         float r = 0.0f;
         float g = 0.0f;
         float b = 0.0f;
         float a = 0.0f;

         for(int sy = (int)((float)y*fh);sy<(int)ceil((float)(y+1)*fh);sy++)
         {
            float wy = 1.0f;
            if(sy<(int)((float)y*fh))
               wy = (float)sy-(float)y*fh+1.0f;
            else if((float)sy+1.0f>(float)(y+1.0f)*fh)
               wy = (float)(y+1.0f)*fh-(float)sy;

            for(int sx = (int)((float)x*fw);sx<(int)ceil((float)(x+1)*fw);sx++)
            {
               float wx = 1.0f;
               if(sx<(int)((float)x*fw))
                  wx = (float)sx-(float)x*fw+1.0f;
               else if((float)sx+1.0f>(float)(x+1.0f)*fw)
                  wx = (float)(x+1.0f)*fw-(float)sx;
               
               SLK_Color c = SLK_rgb_sprite_get_pixel(in,sx,sy);
               n+=wx*wy;
               r+=wx*wy*(float)c.r;
               g+=wx*wy*(float)c.g;
               b+=wx*wy*(float)c.b;
               a+=wx*wy*(float)c.a;
            }
         }

         p.r = r/n;
         p.g = g/n;
         p.b = b/n;
         p.a = a/n;
         out[y*width+x] = p;
      }
   }
}

//Lanczos downsampling
//The only one to use doubles 
//for best possible result
static void sample_lanczos(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   double fw = (double)(in->width-1)/(double)width;
   double fh = (double)(in->height-1)/(double)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)((double)x*fw);
         int iy = (int)((double)y*fh);
         double sx = ((double)x*fw)-(double)ix;
         double sy = ((double)y*fh)-(double)iy;
         Big_pixel p = {0};

         double a0 = lanczos(sx+2.0f);
         double a1 = lanczos(sx+1.0f);
         double a2 = lanczos(sx);
         double a3 = lanczos(sx-1.0f);
         double a4 = lanczos(sx-2.0f);
         double a5 = lanczos(sx-3.0f);
         double b0 = lanczos(sy+2.0f);
         double b1 = lanczos(sy+1.0f);
         double b2 = lanczos(sy);
         double b3 = lanczos(sy-1.0f);
         double b4 = lanczos(sy-2.0f);
         double b5 = lanczos(sy-3.0f);

         double r[6];
         double g[6];
         double b[6];
         double a[6];
         for(int i = 0;i<6;i++)
         {
            SLK_Color p0 = SLK_rgb_sprite_get_pixel(in,ix-2,iy-2+i);
            SLK_Color p1 = SLK_rgb_sprite_get_pixel(in,ix-1,iy-2+i);
            SLK_Color p2 = SLK_rgb_sprite_get_pixel(in,ix,iy-2+i);
            SLK_Color p3 = SLK_rgb_sprite_get_pixel(in,ix+1,iy-2+i);
            SLK_Color p4 = SLK_rgb_sprite_get_pixel(in,ix+2,iy-2+i);
            SLK_Color p5 = SLK_rgb_sprite_get_pixel(in,ix+3,iy-2+i);

            r[i] = a0*(double)p0.r+a1*(double)p1.r+a2*(double)p2.r+a3*(double)p3.r+a4*(double)p4.r+a5*(double)p5.r;
            g[i] = a0*(double)p0.g+a1*(double)p1.g+a2*(double)p2.g+a3*(double)p3.g+a4*(double)p4.g+a5*(double)p5.g;
            b[i] = a0*(double)p0.b+a1*(double)p1.b+a2*(double)p2.b+a3*(double)p3.b+a4*(double)p4.b+a5*(double)p5.b;
            a[i] = a0*(double)p0.a+a1*(double)p1.a+a2*(double)p2.a+a3*(double)p3.a+a4*(double)p4.a+a5*(double)p5.a;
         }

         p.r = MAX(0,MIN(0xff,(int)(b0*r[0]+b1*r[1]+b2*r[2]+b3*r[3]+b4*r[4]+b5*r[5])));
         p.g = MAX(0,MIN(0xff,(int)(b0*g[0]+b1*g[1]+b2*g[2]+b3*g[3]+b4*g[4]+b5*g[5])));
         p.b = MAX(0,MIN(0xff,(int)(b0*b[0]+b1*b[1]+b2*b[2]+b3*b[3]+b4*b[4]+b5*b[5])));
         p.a = MAX(0,MIN(0xff,(int)(b0*a[0]+b1*a[1]+b2*a[2]+b3*a[3]+b4*a[4]+b5*a[5])));
         out[y*width+x] = p;
      }
   }
}

//Helper function for sample_lanczos
static double lanczos(double v)
{
   if(v==0.0f)
      return 1.0f;
   if(v>3.0f)
      return 0.0f;
   if(v<-3.0f)
      return 0.0f;

   return ((3.0f*sin(M_PI*v)*sin(M_PI*v/3.0f))/(M_PI*M_PI*v*v));
}
//-------------------------------------
