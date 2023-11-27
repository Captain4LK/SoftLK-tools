/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "HLH.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void boxblur_line(const uint16_t * restrict src, uint16_t * restrict dst, int width, float rad);
//-------------------------------------

//Function implementations

void SLK_image64_blur(SLK_image64 *img, float sz)
{
   //Can't blur images this small
   if(img->w<=(int)sz||img->h<=(int)sz)
      return;

   //Horizontal
   uint64_t *buffer0 = malloc(sizeof(*buffer0)*img->h);
   uint64_t *buffer1 = malloc(sizeof(*buffer1)*img->h);
   for(int y = 0;y<img->h;y++)
   {
      boxblur_line((uint16_t *)(img->data+y*img->w),(uint16_t *)buffer0,img->w,sz);
      boxblur_line((uint16_t *)buffer0,(uint16_t *)buffer1,img->w,sz);
      boxblur_line((uint16_t *)buffer1,(uint16_t *)(img->data+y*img->w),img->w,sz);
   }

   //Vertical
   buffer0 = realloc(buffer0,sizeof(*buffer0)*img->w);
   buffer1 = realloc(buffer1,sizeof(*buffer1)*img->w);
   for(int x = 0;x<img->w;x++)
   {
      for(int y = 0;y<img->h;y++)
         buffer0[y] = img->data[y*img->w+x];

      boxblur_line((uint16_t *)buffer0,(uint16_t *)buffer1,img->h,sz);
      boxblur_line((uint16_t *)buffer1,(uint16_t *)buffer0,img->h,sz);
      boxblur_line((uint16_t *)buffer0,(uint16_t *)buffer1,img->h,sz);

      for(int y = 0;y<img->h;y++)
         img->data[y*img->w+x] = buffer0[y];
   }

   free(buffer0);
   free(buffer1);
}

static void boxblur_line(const uint16_t * restrict src, uint16_t * restrict dst, int width, float rad)
{
   int r = (int)rad;
   int32_t alpha = ((int32_t)(rad*64.f))&63;
   int32_t alpha1 = 64-alpha;
   int32_t alpha_total = alpha-alpha1;
   int32_t s1,s2,d;
   s1 = s2 = -((2*r+2)/2)*4;
   d = 0;

   int32_t amp_div = HLH_max(1,(2*r+1)*64+alpha*2);
   int32_t amp = (65536.*64)/HLH_max(1,(2*r+1)*64+alpha*2);
   int32_t amp_clip;
   if(amp>128)
      amp_clip = (((int64_t)65536*65536*64)/amp)-1;
   else
      amp_clip = 0x7fffffff;

   int32_t sum_r = src[0]*alpha_total;
   int32_t sum_g = src[1]*alpha_total;
   int32_t sum_b = src[2]*alpha_total;
   int32_t sum_a = src[3]*alpha_total;

   for(int i = 0;i<r+1;i++)
   {
      sum_r+=src[0]*(alpha+alpha1);
      sum_g+=src[1]*(alpha+alpha1);
      sum_b+=src[2]*(alpha+alpha1);
      sum_a+=src[3]*(alpha+alpha1);
      s1+=4;
      //if(i==r) //TODO: maybe?
         //pix1 = src[1];
   }

   for(int i = 0;i<r;i++)
   {
      sum_r+=src[s1]*alpha1+src[s1+4]*alpha;
      sum_g+=src[s1+1]*alpha1+src[s1+5]*alpha;
      sum_b+=src[s1+2]*alpha1+src[s1+6]*alpha;
      sum_a+=src[s1+3]*alpha1+src[s1+7]*alpha;
      s1+=4;
   }

   for(int i = 0;i<=r;i++)
   {
      sum_r+=src[s1]*alpha1+src[s1+4]*alpha;
      sum_g+=src[s1+1]*alpha1+src[s1+5]*alpha;
      sum_b+=src[s1+2]*alpha1+src[s1+6]*alpha;
      sum_a+=src[s1+3]*alpha1+src[s1+7]*alpha;
      s1+=4;

      uint64_t cr = (((uint64_t)sum_r*amp)/(65536*64));
      uint64_t cg = (((uint64_t)sum_g*amp)/(65536*64));
      uint64_t cb = (((uint64_t)sum_b*amp)/(65536*64));
      uint64_t ca = (((uint64_t)sum_a*amp)/(65536*64));
      dst[d] = cr;
      dst[d+1] = cg;
      dst[d+2] = cb;
      dst[d+3] = ca;
      d+=4;

      sum_r-=src[0]*(alpha+alpha1);
      sum_g-=src[1]*(alpha+alpha1);
      sum_b-=src[2]*(alpha+alpha1);
      sum_a-=src[3]*(alpha+alpha1);
      s2+=4;
   }

   for(int i = r+1;i<width-r;i++)
   {
      sum_r+=src[s1]*alpha1+src[s1+4]*alpha;
      sum_g+=src[s1+1]*alpha1+src[s1+5]*alpha;
      sum_b+=src[s1+2]*alpha1+src[s1+6]*alpha;
      sum_a+=src[s1+3]*alpha1+src[s1+7]*alpha;
      s1+=4;

      uint64_t cr = (((uint64_t)sum_r*amp)/(65536*64));
      uint64_t cg = (((uint64_t)sum_g*amp)/(65536*64));
      uint64_t cb = (((uint64_t)sum_b*amp)/(65536*64));
      uint64_t ca = (((uint64_t)sum_a*amp)/(65536*64));
      dst[d] = cr;
      dst[d+1] = cg;
      dst[d+2] = cb;
      dst[d+3] = ca;
      d+=4;

      sum_r-=src[s2]*alpha+src[s2+4]*alpha1;
      sum_g-=src[s2+1]*alpha+src[s2+5]*alpha1;
      sum_b-=src[s2+2]*alpha+src[s2+6]*alpha1;
      sum_a-=src[s2+3]*alpha+src[s2+7]*alpha1;
      s2+=4;
   }

   for(int i = width-r;i<width;i++)
   {
      sum_r+=src[width*4-1]*alpha1+src[width*4-1+4]*alpha;
      sum_g+=src[width*4-1+1]*alpha1+src[width*4-1+5]*alpha;
      sum_b+=src[width*4-1+2]*alpha1+src[width*4-1+6]*alpha;
      sum_a+=src[width*4-1+3]*alpha1+src[width*4-1+7]*alpha;
      s1+=4;

      uint64_t cr = (((uint64_t)sum_r*amp)/(65536*64));
      uint64_t cg = (((uint64_t)sum_g*amp)/(65536*64));
      uint64_t cb = (((uint64_t)sum_b*amp)/(65536*64));
      uint64_t ca = (((uint64_t)sum_a*amp)/(65536*64));
      dst[d] = cr;
      dst[d+1] = cg;
      dst[d+2] = cb;
      dst[d+3] = ca;
      d+=4;

      sum_r-=src[s2]*alpha+src[s2+4]*alpha1;
      sum_g-=src[s2+1]*alpha+src[s2+5]*alpha1;
      sum_b-=src[s2+2]*alpha+src[s2+6]*alpha1;
      sum_a-=src[s2+3]*alpha+src[s2+7]*alpha1;
      s2+=4;
   }
}
//-------------------------------------
