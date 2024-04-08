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
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

void SLK_image64_gamma(SLK_image64 *img, float gamma)
{
   if(gamma==0.f)
      return;

#pragma omp parallel for
   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         uint64_t a = SLK_color64_a(p);
         
         float fr = (float)SLK_color64_r(p);
         float fg = (float)SLK_color64_g(p);
         float fb = (float)SLK_color64_b(p);

         uint64_t r = HLH_max(0,HLH_min(0x7fff,(int)((float)0x7fff*powf(fr/(float)0x7fff,gamma))));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,(int)((float)0x7fff*powf(fg/(float)0x7fff,gamma))));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,(int)((float)0x7fff*powf(fb/(float)0x7fff,gamma))));

         img->data[y*img->w+x] = (r)|(g<<16)|(b<<32)|(a<<48);
      }
   }
}
//-------------------------------------
