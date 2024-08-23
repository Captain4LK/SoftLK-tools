/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <string.h>
//-------------------------------------

//Internal includes
#include "image.h"
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

Image8 *image8_new(int32_t width, int32_t height)
{
   if(width<=0||height<=0)
      return NULL;

   Image8 *img = calloc(1,sizeof(*img)+sizeof(*img->data)*width*height);
   img->width = width;
   img->height = height;

   return img;
}

Image32 *image32_new(int32_t width, int32_t height)
{
   if(width<=0||height<=0)
      return NULL;

   Image32 *img = calloc(1,sizeof(*img)+sizeof(*img->data)*width*height);
   img->width = width;
   img->height = height;

   return img;
}

Image64 *image64_new(int32_t width, int32_t height)
{
   if(width<=0||height<=0)
      return NULL;

   Image64 *img = calloc(1,sizeof(*img)+sizeof(*img->data)*width*height);
   img->width = width;
   img->height = height;

   return img;
}

Image8 *Image8_dup(const Image8 *src)
{
   if(src==NULL||src->width<=0||src->height<=0)
      return NULL;

   Image8 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->width*src->height);
   img->width = src->width;
   img->height = src->height;
   memcpy(img->data,src->data,sizeof(*img->data)*img->width*img->height);

   return img;
}

Image32 *Image32_dup(const Image32 *src)
{
   if(src==NULL||src->width<=0||src->height<=0)
      return NULL;

   Image32 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->width*src->height);
   img->width = src->width;
   img->height = src->height;
   memcpy(img->data,src->data,sizeof(*img->data)*img->width*img->height);

   return img;
}

Image64 *Image64_dup(const Image64 *src)
{
   if(src==NULL||src->width<=0||src->height<=0)
      return NULL;

   Image64 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->width*src->height);
   img->width = src->width;
   img->height = src->height;
   memcpy(img->data,src->data,sizeof(*img->data)*img->width*img->height);

   return img;
}
//-------------------------------------
