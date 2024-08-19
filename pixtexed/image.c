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

PTD_image8 *PTD_image8_dup(const PTD_image8 *src)
{
   if(src==NULL||src->width<=0||src->height<=0)
      return NULL;

   PTD_image8 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->width*src->height);
   img->width = src->width;
   img->height = src->height;
   memcpy(img->data,src->data,sizeof(*img->data)*img->width*img->height);

   return img;
}

PTD_image32 *PTD_image32_dup(const PTD_image32 *src)
{
   if(src==NULL||src->width<=0||src->height<=0)
      return NULL;

   PTD_image32 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->width*src->height);
   img->width = src->width;
   img->height = src->height;
   memcpy(img->data,src->data,sizeof(*img->data)*img->width*img->height);

   return img;
}

PTD_image64 *PTD_image64_dup(const PTD_image64 *src)
{
   if(src==NULL||src->width<=0||src->height<=0)
      return NULL;

   PTD_image64 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->width*src->height);
   img->width = src->width;
   img->height = src->height;
   memcpy(img->data,src->data,sizeof(*img->data)*img->width*img->height);

   return img;
}
//-------------------------------------
