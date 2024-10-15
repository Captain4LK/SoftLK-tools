/*
Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdint.h>

#include "HLH_gui.h"

#define RPNG_IMPLEMENTATION
#define RPNG_DEFLATE_IMPLEMENTATION
#define RPNG_SHOW_LOG_INFO
#include "3rd/rpng.h"
//-------------------------------------

//Internal includes
#include "shared/color.h"
#include "shared/png.h"
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

int png_save(const Image8 *img, const char *path)
{
   if(img==NULL||path==NULL)
      return 1;

   rpng_palette palette = {0};
   palette.color_count = img->color_count;
   rpng_color colors[256] = {0};
   for(int i = 0;i<img->color_count;i++)
   {
      colors[i].r = (uint8_t)color32_r(img->palette[i]);
      colors[i].g = (uint8_t)color32_g(img->palette[i]);
      colors[i].b = (uint8_t)color32_b(img->palette[i]);
      colors[i].a = (uint8_t)color32_a(img->palette[i]);
   }
   palette.colors = colors;
   return rpng_save_image_indexed(path,(const char *)img->data,img->width,img->height,palette);;
}

Image8 *png_load(const char *path)
{
   if(path==NULL)
      return NULL;

   int width;
   int height;
   int color_channels;
   int bit_depth;
   rpng_palette palette = {0};
   char *data = rpng_load_image_indexed(path,&width,&height,&palette);
   if(data!=NULL)
   {
      Image8 *img = image8_new(width,height);
      img->color_count = palette.color_count;
      for(int i = 0;i<palette.color_count;i++)
         img->palette[i] = color32(palette.colors[i].r,palette.colors[i].g,palette.colors[i].b,palette.colors[i].a);
      for(int i = 0;i<width*height;i++)
         img->data[i] = data[i];

      free(data);
      free(palette.colors);

      return img;
   }

   data = rpng_load_image(path,&width,&height,&color_channels,&bit_depth);
   if(data==NULL)
      return NULL;

   if(color_channels==1)
   {
      Image8 *img = image8_new(width,height);
      img->color_count = 256;
      for(int i = 0;i<256;i++)
         img->palette[i] = color32((uint8_t)i,(uint8_t)i,(uint8_t)i,255);

      if(bit_depth==16)
      {
         uint16_t *pix = (uint16_t *)data;
         for(int i = 0;i<width*height;i++)
            img->data[i] = (uint8_t)(pix[i]/256);
      }
      else
      {
         for(int i = 0;i<width*height;i++)
            img->data[i] = data[i];
      }

      free(data);

      return img;
   }
   else if(color_channels==2)
   {
      Image8 *img = image8_new(width,height);
      img->color_count = 256;
      for(int i = 0;i<256;i++)
         img->palette[i] = color32((uint8_t)i,(uint8_t)i,(uint8_t)i,255);

      if(bit_depth==16)
      {
         uint16_t *pix = (uint16_t *)data;
         for(int i = 0;i<width*height;i++)
            img->data[i] = (uint8_t)(pix[2*i]/256);
      }
      else
      {
         for(int i = 0;i<width*height;i++)
            img->data[i] = data[2*i];
      }

      free(data);

      return img;
   }
   else if(color_channels==3)
   {
      Image32 *img = image32_new(width,height);

      if(bit_depth==16)
      {
         uint16_t *pix = (uint16_t *)data;
         for(int i = 0;i<width*height;i++)
            img->data[i] = color32((uint8_t)(pix[3*i]/256),(uint8_t)(pix[3*i+1]/256),(uint8_t)(pix[3*i+2]/256),255);
      }
      else
      {
         uint8_t *pix = (uint8_t *)data;
         for(int i = 0;i<width*height;i++)
            img->data[i] = color32(pix[3*i],pix[3*i+1],pix[3*i+2],255);
      }

      Image8 *img8 = image32to8(img);
      free(img);
      return img8;
   }
   else if(color_channels==4)
   {
      Image32 *img = image32_new(width,height);

      if(bit_depth==16)
      {
         uint16_t *pix = (uint16_t *)data;
         for(int i = 0;i<width*height;i++)
            img->data[i] = color32((uint8_t)(pix[4*i]/256),(uint8_t)(pix[4*i+1]/256),(uint8_t)(pix[4*i+2]/256),255);
      }
      else
      {
         uint8_t *pix = (uint8_t *)data;
         for(int i = 0;i<width*height;i++)
         {
            img->data[i] = color32(pix[4*i],pix[4*i+1],pix[4*i+2],255);
         }
      }

      Image8 *img8 = image32to8(img);
      free(img);
      return img8;
   }

   free(data);

   return NULL;
}
//-------------------------------------
