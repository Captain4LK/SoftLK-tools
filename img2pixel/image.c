/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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

SLK_image32 *SLK_image32_dup(const SLK_image32 *src)
{
   if(src==NULL||src->w<=0||src->h<=0)
      return NULL;

   SLK_image32 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->w*src->h);
   img->w = src->w;
   img->h = src->h;
   memcpy(img->data,src->data,sizeof(*img->data)*img->w*img->h);

   return img;
}

SLK_image32 *SLK_image32_dup64(const SLK_image64 *src)
{
   if(src==NULL||src->w<=0||src->h<=0)
      return NULL;

   SLK_image32 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->w*src->h);
   img->w = src->w;
   img->h = src->h;
   for(int i = 0;i<img->w*img->h;i++)
      img->data[i] = SLK_color64_to_32(src->data[i]);

   return img;
}

SLK_image64 *SLK_image64_dup(const SLK_image64 *src)
{
   if(src==NULL||src->w<=0||src->h<=0)
      return NULL;

   SLK_image64 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->w*src->h);
   img->w = src->w;
   img->h = src->h;
   memcpy(img->data,src->data,sizeof(*img->data)*img->w*img->h);

   return img;
}

SLK_image64 *SLK_image64_dup32(const SLK_image32 *src)
{
   if(src==NULL||src->w<=0||src->h<=0)
      return NULL;

   SLK_image64 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->w*src->h);
   img->w = src->w;
   img->h = src->h;
   for(int i = 0;i<img->w*img->h;i++)
      img->data[i] = SLK_color32_to_64(src->data[i]);

   return img;
}
//-------------------------------------
