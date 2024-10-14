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

Image32 *Image32_postprocess(Image32 *img, uint32_t *color_inline, uint32_t *color_outline)
{
   Image32 *out = image32_dup(img);

   for(int y = 0;y<img->height;y++)
   {
      for(int x = 0;x<img->width;x++)
      {
         if(color_inline!=NULL&&color32_a(img->data[y*img->width+x])!=0)
         {
            int empty = 0;
            if(y-1>=0&&y-1<img->height&&color32_a(img->data[(y-1)*img->width+x])==0)
               empty++;
            if(y+1>=0&&y+1<img->height&&color32_a(img->data[(y+1)*img->width+x])==0)
               empty++;
            if(x-1>=0&&x-1<img->width&&color32_a(img->data[y*img->width+x-1])==0)
               empty++;
            if(x+1>=0&&x+1<img->width&&color32_a(img->data[y*img->width+x+1])==0)
               empty++;

            if(empty!=0)
               out->data[y*img->width+x] = *color_inline;
         }

         if(color_outline!=NULL&&color32_a(img->data[y*img->width+x])==0)
         {
            int empty = 0;
            if(y-1>=0&&y-1<img->height&&color32_a(img->data[(y-1)*img->width+x])!=0)
               empty++;
            if(y+1>=0&&y+1<img->height&&color32_a(img->data[(y+1)*img->width+x])!=0)
               empty++;
            if(x-1>=0&&x-1<img->width&&color32_a(img->data[y*img->width+x-1])!=0)
               empty++;
            if(x+1>=0&&x+1<img->width&&color32_a(img->data[y*img->width+x+1])!=0)
               empty++;

            if(empty!=0)
               out->data[y*img->width+x] = *color_outline;
         }
      }
   }

   return out;
}
//-------------------------------------
