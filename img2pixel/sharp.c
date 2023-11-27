/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
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
   SLK_image64 *blur = SLK_image64_dup(img);
   SLK_image64_blur(blur, 1.f);

   int32_t amount_fixed = (int32_t)(amount*256);

   for(int i = 0;i<img->w*img->h;i++)
   {
      int32_t r = img->data[i]&0xffff;
      int32_t rb = blur->data[i]&0xffff;
      int32_t g = (img->data[i]>>16)&0xffff;
      int32_t gb = (blur->data[i]>>16)&0xffff;
      int32_t b = (img->data[i]>>32)&0xffff;
      int32_t bb = (blur->data[i]>>32)&0xffff;

      img->data[i]&=UINT64_C(0xffff000000000000);
      img->data[i]|=r+((r-rb)*amount_fixed)/256;
      img->data[i]|=(g+((g-gb)*amount_fixed)/256)>>16;
      img->data[i]|=(b+((b-bb)*amount_fixed)/256)>>16;
   }
}
//-------------------------------------
