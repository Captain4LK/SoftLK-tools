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
//-------------------------------------

//Internal includes
#include "img2pixel.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static const float slk_dither_threshold_bayer8x8[64] = 
{
    0.0f/64.0f,32.0f/64.0f, 8.0f/64.0f,40.0f/64.0f, 2.0f/64.0f,34.0f/64.0f,10.0f/64.0f,42.0f/64.0f,
   48.0f/64.0f,16.0f/64.0f,56.0f/64.0f,24.0f/64.0f,50.0f/64.0f,18.0f/64.0f,58.0f/64.0f,26.0f/64.0f,
   12.0f/64.0f,44.0f/64.0f, 4.0f/64.0f,36.0f/64.0f,14.0f/64.0f,46.0f/64.0f, 6.0f/64.0f,38.0f/64.0f,
   60.0f/64.0f,28.0f/64.0f,52.0f/64.0f,20.0f/64.0f,62.0f/64.0f,30.0f/64.0f,54.0f/64.0f,22.0f/64.0f,
    3.0f/64.0f,35.0f/64.0f,11.0f/64.0f,43.0f/64.0f, 1.0f/64.0f,33.0f/64.0f, 9.0f/64.0f,41.0f/64.0f,
   51.0f/64.0f,19.0f/64.0f,59.0f/64.0f,27.0f/64.0f,49.0f/64.0f,17.0f/64.0f,57.0f/64.0f,25.0f/64.0f,
   15.0f/64.0f,47.0f/64.0f, 7.0f/64.0f,39.0f/64.0f,13.0f/64.0f,45.0f/64.0f, 5.0f/64.0f,37.0f/64.0f,
   63.0f/64.0f,31.0f/64.0f,55.0f/64.0f,23.0f/64.0f,61.0f/64.0f,29.0f/64.0f,53.0f/64.0f,21.0f/64.0f,
};
static const float slk_dither_threshold_bayer4x4[16] = 
{
   0.0f/16.0f,8.0f/16.0f,2.0f/16.0f,10.0f/16.0f,
   12.0f/16.0f,4.0f/16.0f,14.0f/16.0f,6.0f/16.0f,
   3.0f/16.0f,11.0f/16.0f,1.0f/16.0f,9.0f/16.0f,
   15.0f/16.0f,7.0f/16.0f,13.0f/16.0f,5.0f/16.0f,
};
static const float slk_dither_threshold_bayer2x2[4] = 
{
   0.0f/4.0f,2.0f/4.0f,
   3.0f/4.0f,1.0f/4.0f
};
static const float slk_dither_threshold_cluster8x8[64] = 
{
   24.0f/64.0f,10.0f/64.0f,12.0f/64.0f,26.0f/64.0f,35.0f/64.0f,47.0f/64.0f,49.0f/64.0f,37.0f/64.0f,
   8.0f/64.0f,0.0f/64.0f,2.0f/64.0f,14.0f/64.0f,45.0f/64.0f,59.0f/64.0f,61.0f/64.0f,51.0f/64.0f,
   22.0f/64.0f,6.0f/64.0f,4.0f/64.0f,16.0f/64.0f,43.0f/64.0f,57.0f/64.0f,63.0f/64.0f,53.0f/64.0f,
   30.0f/64.0f,20.0f/64.0f,18.0f/64.0f,28.0f/64.0f,33.0f/64.0f,41.0f/64.0f,55.0f/64.0f,39.0f/64.0f,
   34.0f/64.0f,46.0f/64.0f,48.0f/64.0f,36.0f/64.0f,25.0f/64.0f,11.0f/64.0f,13.0f/64.0f,27.0f/64.0f,
   44.0f/64.0f,58.0f/64.0f,60.0f/64.0f,50.0f/64.0f,9.0f/64.0f,1.0f/64.0f,3.0f/64.0f,15.0f/64.0f,
   42.0f/64.0f,56.0f/64.0f,62.0f/64.0f,52.0f/64.0f,23.0f/64.0f,7.0f/64.0f,5.0f/64.0f,17.0f/64.0f,
   32.0f/64.0f,40.0f/64.0f,54.0f/64.0f,38.0f/64.0f,31.0f/64.0f,21.0f/64.0f,19.0f/64.0f,29.0f/64.0f,
};
static const float slk_dither_threshold_cluster4x4[16] = 
{
   12.0f/16.0f,5.0f/16.0f,6.0f/16.0f,13.0f/16.0f,
   4.0f/16.0f,0.0f/16.0f,1.0f/16.0f,7.0f/16.0f,
   11.0f/16.0f,3.0f/16.0f,2.0f/16.0f,8.0f/16.0f,
   15.0f/16.0f,10.0f/16.0f,9.0f/16.0f,14.0f/16.0f,
};
//-------------------------------------

//Function prototypes
static SLK_image32 *slk_dither_threshold(SLK_image64 *img, int dim, const float *threshold, int distance_mode, int alpha_threshold);
static SLK_image32 *slk_dither_none(SLK_image64 *img, int distance_mode, int alpha_threshold);
//-------------------------------------

//Function implementations

SLK_image32 *SLK_image64_dither(SLK_image64 *img, int dither_mode, int distance_mode, int alpha_threshold)
{
   if(disance_mode==8)
   {
      return SLK_image32_dup64(img);
   }

   switch(dither_mode)
   {
   case 0: return slk_dither_none(img,distance_mode);
   case 1: return slk_dither_threshold(img,3,slk_dither_threshold_bayer8x8,distance_mode);
   }
   
   return SLK_image32_dup64(img);
}

static SLK_image32 *slk_dither_none(SLK_image64 *img, int distance_mode, int alpha_threshold)
{
   SLK_image32 *out = SLK_image32_dup64(img);

   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         if(SLK_color64_a(p)/128<alpha_threshold)
         {
            out->data[y*img->w+x] = 0;
            continue;
         }


      }
   }

   return;
}

static SLK_image32 *slk_dither_threshold(SLK_image64 *img, int dim, const float *threshold, int distance_mode, int alpha_threshold)
{
}
//-------------------------------------
