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
#include "shared/color.h"
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

void image64_tint(Image64 *img, uint8_t tint_r, uint8_t tint_g, uint8_t tint_b)
{
   if(tint_r==255&&tint_g==255&&tint_b==255)
      return;

   uint64_t tr = tint_r;
   uint64_t tg = tint_g;
   uint64_t tb = tint_b;

#pragma omp parallel for
   for(int y = 0;y<img->height;y++)
   {
      for(int x = 0;x<img->width;x++)
      {
         uint64_t p = img->data[y*img->width+x];
         uint64_t r = color64_r(p);
         uint64_t g = color64_g(p);
         uint64_t b = color64_b(p);
         uint64_t a = color64_a(p);

         r = (r*tr)/255;
         g = (g*tg)/255;
         b = (b*tb)/255;

         img->data[y*img->width+x] = r|(g<<16)|(b<<32)|(a<<48);
      }
   }
}
//-------------------------------------
