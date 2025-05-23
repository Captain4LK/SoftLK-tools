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
#include "shared/image.h"
#include "shared/color.h"
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
static Image64 *slk_sample_nearest(const Image64 *img, int width, int height, float x_off, float y_off);
static Image64 *slk_sample_linear(const Image64 *img, int width, int height, float x_off, float y_off);
static Image64 *slk_sample_bicubic(const Image64 *img, int width, int height, float x_off, float y_off);
static Image64 *slk_sample_lanczos(const Image64 *img, int width, int height, float x_off, float y_off);
static Image64 *slk_sample_cluster(const Image64 *img, int width, int height, float x_off, float y_off);

static float slk_blend_linear(float sx, float sy, float c0, float c1, float c2, float c3);
static float slk_blend_bicubic(float c0, float c1, float c2, float c3, float t);
static float slk_lanczos(float v);
//-------------------------------------

//Function implementations

Image64 *image64_sample(const Image64 *img, int width, int height, int sample_mode, float x_off, float y_off)
{
   if(img==NULL)
      return image64_dup(img);
   //TODO(Captain4LK): integer only math?
   width = HLH_max(1,width);
   height = HLH_max(1,height);

   switch(sample_mode)
   {
   case 0: return slk_sample_nearest(img,width,height,x_off,y_off);
   case 1: return slk_sample_linear(img,width,height,x_off,y_off);
   case 2: return slk_sample_bicubic(img,width,height,x_off,y_off);
   case 3: return slk_sample_lanczos(img,width,height,x_off,y_off);
   case 4: return slk_sample_cluster(img,width,height,x_off,y_off);
   }

   return NULL;
}

static Image64 *slk_sample_nearest(const Image64 *img, int width, int height, float x_off, float y_off)
{
   Image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->width = width;
   out->height = height;

   float w = ((float)img->width-1)/(float)width;
   float h = ((float)img->height-1)/(float)height;

#pragma omp parallel for
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         float dx = (float)x+x_off+0.5f;
         float dy = (float)y+y_off+0.5f;

         int ix = HLH_max(0,HLH_min(img->width-1,(int)roundf(dx*w)));
         int iy = HLH_max(0,HLH_min(img->height-1,(int)roundf(dy*h)));
         out->data[y*width+x] = img->data[iy*img->width+ix];
      }
   }

   return out;
}

static Image64 *slk_sample_linear(const Image64 *img, int width, int height, float x_off, float y_off)
{
   Image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->width = width;
   out->height = height;

   float fw = (float)(img->width-1)/(float)width;
   float fh = (float)(img->height-1)/(float)height;

#pragma omp parallel for
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((float)x+x_off+0.5f)*fw);
         int iy = (int)(((float)y+y_off+0.5f)*fh);
         float six = (((float)x+x_off+0.5f)*fw)-(float)ix;
         float siy = (((float)y+y_off+0.5f)*fh)-(float)iy;

         uint64_t p0 = UINT64_C(0x7fff000000000000);
         uint64_t p1 = UINT64_C(0x7fff000000000000);
         uint64_t p2 = UINT64_C(0x7fff000000000000);
         uint64_t p3 = UINT64_C(0x7fff000000000000);

         if(ix>=0&&ix<img->width&&iy>=0&&iy<img->height)
            p0 = img->data[iy*img->width+ix];
         if(ix+1>=0&&ix+1<img->width&&iy>=0&&iy<img->height)
            p1 = img->data[iy*img->width+ix+1];
         if(ix>=0&&ix<img->width&&iy+1>=0&&iy+1<img->height)
            p2 = img->data[(iy+1)*img->width+ix];
         if(ix+1>=0&&ix+1<img->width&&iy+1>=0&&iy+1<img->height)
            p3 = img->data[(iy+1)*img->width+ix+1];

         float c0 = slk_blend_linear(six,siy,(float)color64_r(p0),(float)color64_r(p1),(float)color64_r(p2),(float)color64_r(p3));
         float c1 = slk_blend_linear(six,siy,(float)color64_g(p0),(float)color64_g(p1),(float)color64_g(p2),(float)color64_g(p3));
         float c2 = slk_blend_linear(six,siy,(float)color64_b(p0),(float)color64_b(p1),(float)color64_b(p2),(float)color64_b(p3));
         float c3 = slk_blend_linear(six,siy,(float)color64_a(p0),(float)color64_a(p1),(float)color64_a(p2),(float)color64_a(p3));

         uint64_t r = HLH_max(0,HLH_min(0x7fff,(int)c0));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,(int)c1));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,(int)c2));
         uint64_t a = HLH_max(0,HLH_min(0x7fff,(int)c3));

         out->data[y*out->width+x] = (r)|(g<<16)|(b<<32)|(a<<48);
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

static Image64 *slk_sample_bicubic(const Image64 *img, int width, int height, float x_off, float y_off)
{
   Image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->width = width;
   out->height = height;

   float fw = (float)(img->width-1)/(float)width;
   float fh = (float)(img->height-1)/(float)height;

#pragma omp parallel for
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((float)x+x_off+0.5f)*fw);
         int iy = (int)(((float)y+y_off+0.5f)*fh);
         float six = (((float)x+x_off+0.5f)*fw)-(float)ix;
         float siy = (((float)y+y_off+0.5f)*fh)-(float)iy;

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

         if(ix-1>=0&&ix-1<img->width&&iy-1>=0&&iy-1<img->height)
            p00 = img->data[(iy-1)*img->width+ix-1];
         if(ix>=0&&ix<img->width&&iy-1>=0&&iy-1<img->height)
            p01 = img->data[(iy-1)*img->width+ix];
         if(ix+1>=0&&ix+1<img->width&&iy-1>=0&&iy-1<img->height)
            p02 = img->data[(iy-1)*img->width+ix+1];
         if(ix+2>=0&&ix+2<img->width&&iy-1>=0&&iy-1<img->height)
            p03 = img->data[(iy-1)*img->width+ix+2];

         if(ix-1>=0&&ix-1<img->width&&iy>=0&&iy<img->height)
            p10 = img->data[(iy)*img->width+ix-1];
         if(ix>=0&&ix<img->width&&iy>=0&&iy<img->height)
            p11 = img->data[(iy)*img->width+ix];
         if(ix+1>=0&&ix+1<img->width&&iy>=0&&iy<img->height)
            p12 = img->data[(iy)*img->width+ix+1];
         if(ix+2>=0&&ix+2<img->width&&iy>=0&&iy<img->height)
            p13 = img->data[(iy)*img->width+ix+2];

         if(ix-1>=0&&ix-1<img->width&&iy+1>=0&&iy+1<img->height)
            p20 = img->data[(iy+1)*img->width+ix-1];
         if(ix>=0&&ix<img->width&&iy+1>=0&&iy+1<img->height)
            p21 = img->data[(iy+1)*img->width+ix];
         if(ix+1>=0&&ix+1<img->width&&iy+1>=0&&iy+1<img->height)
            p22 = img->data[(iy+1)*img->width+ix+1];
         if(ix+2>=0&&ix+2<img->width&&iy+1>=0&&iy+1<img->height)
            p23 = img->data[(iy+1)*img->width+ix+2];

         if(ix-1>=0&&ix-1<img->width&&iy+2>=0&&iy+2<img->height)
            p30 = img->data[(iy+2)*img->width+ix-1];
         if(ix>=0&&ix<img->width&&iy+2>=0&&iy+2<img->height)
            p31 = img->data[(iy+2)*img->width+ix];
         if(ix+1>=0&&ix+1<img->width&&iy+2>=0&&iy+2<img->height)
            p32 = img->data[(iy+2)*img->width+ix+1];
         if(ix+2>=0&&ix+2<img->width&&iy+2>=0&&iy+2<img->height)
            p33 = img->data[(iy+2)*img->width+ix+2];

         float c0 = slk_blend_bicubic((float)color64_r(p00),(float)color64_r(p01),(float)color64_r(p02),(float)color64_r(p03),six);
         float c1 = slk_blend_bicubic((float)color64_r(p10),(float)color64_r(p11),(float)color64_r(p12),(float)color64_r(p13),six);
         float c2 = slk_blend_bicubic((float)color64_r(p20),(float)color64_r(p21),(float)color64_r(p22),(float)color64_r(p23),six);
         float c3 = slk_blend_bicubic((float)color64_r(p30),(float)color64_r(p31),(float)color64_r(p32),(float)color64_r(p33),six);
         uint64_t r = HLH_max(0,HLH_min(0x7fff,(int)slk_blend_bicubic(c0,c1,c2,c3,siy)));

         c0 = slk_blend_bicubic((float)color64_g(p00),(float)color64_g(p01),(float)color64_g(p02),(float)color64_g(p03),six);
         c1 = slk_blend_bicubic((float)color64_g(p10),(float)color64_g(p11),(float)color64_g(p12),(float)color64_g(p13),six);
         c2 = slk_blend_bicubic((float)color64_g(p20),(float)color64_g(p21),(float)color64_g(p22),(float)color64_g(p23),six);
         c3 = slk_blend_bicubic((float)color64_g(p30),(float)color64_g(p31),(float)color64_g(p32),(float)color64_g(p33),six);
         uint64_t g = HLH_max(0,HLH_min(0x7fff,(int)slk_blend_bicubic(c0,c1,c2,c3,siy)));

         c0 = slk_blend_bicubic((float)color64_b(p00),(float)color64_b(p01),(float)color64_b(p02),(float)color64_b(p03),six);
         c1 = slk_blend_bicubic((float)color64_b(p10),(float)color64_b(p11),(float)color64_b(p12),(float)color64_b(p13),six);
         c2 = slk_blend_bicubic((float)color64_b(p20),(float)color64_b(p21),(float)color64_b(p22),(float)color64_b(p23),six);
         c3 = slk_blend_bicubic((float)color64_b(p30),(float)color64_b(p31),(float)color64_b(p32),(float)color64_b(p33),six);
         uint64_t b = HLH_max(0,HLH_min(0x7fff,(int)slk_blend_bicubic(c0,c1,c2,c3,siy)));

         c0 = slk_blend_bicubic((float)color64_a(p00),(float)color64_a(p01),(float)color64_a(p02),(float)color64_a(p03),six);
         c1 = slk_blend_bicubic((float)color64_a(p10),(float)color64_a(p11),(float)color64_a(p12),(float)color64_a(p13),six);
         c2 = slk_blend_bicubic((float)color64_a(p20),(float)color64_a(p21),(float)color64_a(p22),(float)color64_a(p23),six);
         c3 = slk_blend_bicubic((float)color64_a(p30),(float)color64_a(p31),(float)color64_a(p32),(float)color64_a(p33),six);
         uint64_t a = HLH_max(0,HLH_min(0x7fff,(int)slk_blend_bicubic(c0,c1,c2,c3,siy)));

         out->data[y*out->width+x] = (r)|(g<<16)|(b<<32)|(a<<48);
      }
   }

   return out;
}

static float slk_blend_bicubic(float c0, float c1, float c2, float c3, float t)
{
   float a0 = -0.5f*c0+1.5f*c1-1.5f*c2+0.5f*c3;
   float a1 = c0-2.5f*c1+2.f*c2-0.5f*c3;
   float a2 = -0.5f*c0+0.5f*c2;
   float a3 = c1;

   return a0*t*t*t+a1*t*t+a2*t+a3;
}

static Image64 *slk_sample_lanczos(const Image64 *img, int width, int height, float x_off, float y_off)
{
   Image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->width = width;
   out->height = height;

   float fw = (float)(img->width-1)/(float)width;
   float fh = (float)(img->height-1)/(float)height;

#pragma omp parallel for
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((float)x+x_off+0.5f)*fw);
         int iy = (int)(((float)y+y_off+0.5f)*fh);
         float six = (((float)x+x_off+0.5f)*fw)-(float)ix;
         float siy = (((float)y+y_off+0.5f)*fh)-(float)iy;

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

            if(ix-2>=0&&ix-2<img->width&&iy-2+i>=0&&iy-2+i<img->height)
               p00 = img->data[(iy-2+i)*img->width+ix-2];
            if(ix-1>=0&&ix-1<img->width&&iy-2+i>=0&&iy-2+i<img->height)
               p01 = img->data[(iy-2+i)*img->width+ix-1];
            if(ix>=0&&ix<img->width&&iy-2+i>=0&&iy-2+i<img->height)
               p02 = img->data[(iy-2+i)*img->width+ix];
            if(ix+1>=0&&ix+1<img->width&&iy-2+i>=0&&iy-2+i<img->height)
               p03 = img->data[(iy-2+i)*img->width+ix+1];
            if(ix+2>=0&&ix+2<img->width&&iy-2+i>=0&&iy-2+i<img->height)
               p04 = img->data[(iy-2+i)*img->width+ix+2];
            if(ix+3>=0&&ix+3<img->width&&iy-2+i>=0&&iy-2+i<img->height)
               p05 = img->data[(iy-2+i)*img->width+ix+3];

            cr[i] = a0*(float)color64_r(p00)+a1*(float)color64_r(p01)+a2*(float)color64_r(p02)+a3*(float)color64_r(p03)+a4*(float)color64_r(p04)+a5*(float)color64_r(p05);
            cg[i] = a0*(float)color64_g(p00)+a1*(float)color64_g(p01)+a2*(float)color64_g(p02)+a3*(float)color64_g(p03)+a4*(float)color64_g(p04)+a5*(float)color64_g(p05);
            cb[i] = a0*(float)color64_b(p00)+a1*(float)color64_b(p01)+a2*(float)color64_b(p02)+a3*(float)color64_b(p03)+a4*(float)color64_b(p04)+a5*(float)color64_b(p05);
            ca[i] = a0*(float)color64_a(p00)+a1*(float)color64_a(p01)+a2*(float)color64_a(p02)+a3*(float)color64_a(p03)+a4*(float)color64_a(p04)+a5*(float)color64_a(p05);
         }

         uint64_t r = HLH_max(0,HLH_min(0x7fff,(int)(b0*cr[0]+b1*cr[1]+b2*cr[2]+b3*cr[3]+b4*cr[4]+b5*cr[5])));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,(int)(b0*cg[0]+b1*cg[1]+b2*cg[2]+b3*cg[3]+b4*cg[4]+b5*cg[5])));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,(int)(b0*cb[0]+b1*cb[1]+b2*cb[2]+b3*cb[3]+b4*cb[4]+b5*cb[5])));
         uint64_t a = HLH_max(0,HLH_min(0x7fff,(int)(b0*ca[0]+b1*ca[1]+b2*ca[2]+b3*ca[3]+b4*ca[4]+b5*ca[5])));

         out->data[y*out->width+x] = (r)|(g<<16)|(b<<32)|(a<<48);
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

static Image64 *slk_sample_cluster(const Image64 *img, int width, int height, float x_off, float y_off)
{

   float w = (float)img->width/(float)width;
   float h = (float)img->height/(float)height;

   float grid_x = ((float)img->width/(float)(width));
   float grid_y = ((float)img->height/(float)(height));
   int igrid_x = (int)grid_x;
   int igrid_y = (int)grid_y;
   if(igrid_x<=0||igrid_y<=0)
   {
      return slk_sample_nearest(img,width,height,x_off,y_off);
   }

   Image64 *out = malloc(sizeof(*out)+sizeof(*out->data)*width*height);
   out->width = width;
   out->height = height;

#pragma omp parallel
   {
      Image32 *cluster = malloc(sizeof(*cluster)+sizeof(*cluster->data)*igrid_x*igrid_y);
      cluster->width = igrid_x;
      cluster->height = igrid_y;
      uint32_t colors[16];

#pragma omp for
      for(int y = 0;y<height;y++)
      {
         for(int x = 0;x<width;x++)
         {
            float dx = (float)x+x_off+0.5f;
            float dy = (float)y+y_off+0.5f;

            int ix = HLH_max(0,HLH_min(img->width-1,(int)roundf(dx*w)));
            int iy = HLH_max(0,HLH_min(img->height-1,(int)roundf(dy*h)));
            uint64_t a = color64_a(img->data[iy*img->width+ix]);

            for(int gy = 0;gy<igrid_y;gy++)
            {
               for(int gx = 0;gx<igrid_x;gx++)
               {
                  float fx = (float)x;
                  float fy = (float)y;
                  float fgx = (float) gx;
                  float fgy = (float) gy;

                  cluster->data[gy*igrid_x+gx] = 0xff000000;
                  if(fx*grid_x+fgx>=0&&fx*grid_x+fgx<img->width&&fy*grid_y+fgy>=0&&fy*grid_y+fgy<img->height)
                     cluster->data[gy*igrid_x+gx] = color64_to_32(img->data[((int)(fy*grid_y)+gy)*img->width+(int)(fx*grid_x)+gx]);
               }
            }
            uint64_t c = color32_to_64(image32_kmeans_largest(cluster,colors,3,0xdeadbeef));
            uint64_t r = color64_r(c);
            uint64_t g = color64_g(c);
            uint64_t b = color64_b(c);
            out->data[y*width+x] = (r)|(g<<16)|(b<<32)|(a<<48);
         }
      }

      free(cluster);
   }

   return out;
}
//-------------------------------------
