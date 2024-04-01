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

SLK_image32 *SLK_image32_postprocess(SLK_image32 *img, uint32_t *color_inline, uint32_t *color_outline)
{
   SLK_image32 *out = SLK_image32_dup(img);

   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         if(color_inline!=NULL&&SLK_color32_a(img->data[y*img->w+x])!=0)
         {
            int empty = 0;
            if(y-1>=0&&y-1<img->h&&SLK_color32_a(img->data[(y-1)*img->w+x])==0)
               empty++;
            if(y+1>=0&&y+1<img->h&&SLK_color32_a(img->data[(y+1)*img->w+x])==0)
               empty++;
            if(x-1>=0&&x-1<img->w&&SLK_color32_a(img->data[y*img->w+x-1])==0)
               empty++;
            if(x+1>=0&&x+1<img->w&&SLK_color32_a(img->data[y*img->w+x+1])==0)
               empty++;

            if(empty!=0)
               out->data[y*img->w+x] = *color_outline
         }

         if(color_outline!=NULL&&SLK_color32_a(img->data[y*img->w+x])==0)
         {
            int empty = 0;
            if(y-1>=0&&y-1<img->h&&SLK_color32_a(img->data[(y-1)*img->w+x])!=0)
               empty++;
            if(y+1>=0&&y+1<img->h&&SLK_color32_a(img->data[(y+1)*img->w+x])!=0)
               empty++;
            if(x-1>=0&&x-1<img->w&&SLK_color32_a(img->data[y*img->w+x-1])!=0)
               empty++;
            if(x+1>=0&&x+1<img->w&&SLK_color32_a(img->data[y*img->w+x+1])!=0)
               empty++;

            if(empty!=0)
               out->data[y*img->w+x] = *color_outline
         }
      }
   }

   return out;
}
//-------------------------------------
