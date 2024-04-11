/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <stdint.h>

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
//-------------------------------------

//Function implementations

void SLK_image64_sharpen(SLK_image64 *img, float amount)
{
   if(amount<1e-2)
      return;

   SLK_image64 *blur = SLK_image64_dup(img);
   SLK_image64_blur(blur, 1.f);

   int32_t amount_fixed = (int32_t)(amount*256);

#pragma omp parallel for
   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         int i = y*img->w+x;
         int32_t r = SLK_color64_r(img->data[i]);
         int32_t rb = SLK_color64_r(blur->data[i]);
         int32_t g = SLK_color64_g(img->data[i]);
         int32_t gb = SLK_color64_g(blur->data[i]);
         int32_t b = SLK_color64_b(img->data[i]);
         int32_t bb = SLK_color64_b(blur->data[i]);

         uint64_t cr = HLH_max(0,HLH_min(0x7fff,r+((r-rb)*amount_fixed)/256));
         uint64_t cg = HLH_max(0,HLH_min(0x7fff,g+((g-gb)*amount_fixed)/256));
         uint64_t cb = HLH_max(0,HLH_min(0x7fff,b+((b-bb)*amount_fixed)/256));
         uint64_t ca = SLK_color64_a(img->data[i]);

         img->data[i] = cr|(cg<<16)|(cb<<32)|(ca<<48);
      }
   }

   free(blur);
}
//-------------------------------------
