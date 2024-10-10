/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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

void image64_blur(Image64 *img, float sz)
{
   if(img==NULL)
      return;

   if(sz<=0.01f)
      return;

   //Can't blur images this small
   if(img->width<=(int)sz||img->height<=(int)sz)
      return;

   //Horizontal
#pragma omp parallel
   {
      uint64_t *buffer0 = malloc(sizeof(*buffer0)*img->width);
      uint64_t *buffer1 = malloc(sizeof(*buffer1)*img->width);
#pragma omp for
      for(int y = 0;y<img->height;y++)
      {
         boxblur_line((uint16_t *)(img->data+y*img->width),(uint16_t *)buffer0,img->width,sz);
         boxblur_line((uint16_t *)buffer0,(uint16_t *)buffer1,img->width,sz);
         boxblur_line((uint16_t *)buffer1,(uint16_t *)(img->data+y*img->width),img->width,sz);
      }
      free(buffer0);
      free(buffer1);
   }

   //Vertical
#pragma omp parallel
   {
      uint64_t *buffer0 = malloc(sizeof(*buffer0)*img->height);
      uint64_t *buffer1 = malloc(sizeof(*buffer1)*img->height);
#pragma omp for
      for(int x = 0;x<img->width;x++)
      {
         for(int y = 0;y<img->height;y++)
            buffer0[y] = img->data[y*img->width+x];

         boxblur_line((uint16_t *)buffer0,(uint16_t *)buffer1,img->height,sz);
         boxblur_line((uint16_t *)buffer1,(uint16_t *)buffer0,img->height,sz);
         boxblur_line((uint16_t *)buffer0,(uint16_t *)buffer1,img->height,sz);

         for(int y = 0;y<img->height;y++)
            img->data[y*img->width+x] = buffer0[y];
      }
      free(buffer0);
      free(buffer1);
   }
}

static void boxblur_line(const uint16_t * restrict src, uint16_t * restrict dst, int width, float rad)
{
   int r = (int)rad;
   int32_t alpha = ((int32_t)(rad*64.f))&63;
   int32_t alpha1 = 64-alpha;
   int32_t alpha_total = alpha-alpha1;
   int32_t s1,s2,d;
   s1 = s2 = -((r+1)/2)*4;
   d = 0;

   int32_t amp = (65536*64)/HLH_max(1,(2*r+1)*64+alpha*2);
   int32_t amp_clip;
   if(amp>128)
      amp_clip = (((int64_t)65536*65536*64)/amp)-1;
   else
      amp_clip = 0x7fffffff;

   int32_t sum_r = 0;
   int32_t sum_g = 0;
   int32_t sum_b = 0;
   int32_t sum_a = 0;

   sum_r+=src[0]*(alpha+alpha1)*(r);
   sum_g+=src[1]*(alpha+alpha1)*(r);
   sum_b+=src[2]*(alpha+alpha1)*(r);
   sum_a+=src[3]*(alpha+alpha1)*(r);
   sum_r+=src[0]*alpha;
   sum_g+=src[1]*alpha;
   sum_b+=src[2]*alpha;
   sum_a+=src[3]*alpha;
   sum_r+=src[0]*alpha;
   sum_g+=src[1]*alpha;
   sum_b+=src[2]*alpha;
   sum_a+=src[3]*alpha;
   s1 = 0;

   for(int i = 0;i<r;i++)
   {
      sum_r+=src[s1]*alpha1+src[s1+4]*alpha;
      sum_g+=src[s1+1]*alpha1+src[s1+5]*alpha;
      sum_b+=src[s1+2]*alpha1+src[s1+6]*alpha;
      sum_a+=src[s1+3]*alpha1+src[s1+7]*alpha;
      s1+=4;
   }

   for(int i = 0;i<r+1;i++)
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
   s2 = 0;

   for(int i = 0;i<width-2*r-2;i++)
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

   for(int i = 0;i<r+1;i++)
   {
      sum_r+=src[(width-1)*4]*(alpha1+alpha);
      sum_g+=src[(width-1)*4+1]*(alpha1+alpha);
      sum_b+=src[(width-1)*4+2]*(alpha1+alpha);
      sum_a+=src[(width-1)*4+3]*(alpha1+alpha);

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
