/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HLH.h"
//-------------------------------------

//Internal includes
#include "image.h"
#include "color.h"
#include "rand.h"

#include "pcx.h"
#include "png.h"
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
   img->color_count = 256;

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

Image8 *image32to8(const Image32 *img)
{
   uint32_t palette[256];
   uint32_t centers[256] = {0};
   uint32_t *clusters[256] = {0};

   //Choose centers
   //-------------------------------------
   rand_xor rng;
   rand_xor_seed(&rng,0);

   int index = rand_xor_next(&rng)%(img->width*img->height);
   centers[0] = img->data[index];

   uint64_t *distance = NULL;
   HLH_array_length_set(distance,img->width*img->height);
   for(int i = 0;i<img->width*img->height;i++)
      distance[i] = UINT64_MAX;

   for(int i = 1;i<256;i++)
   {
      uint64_t dist_sum = 0;
      for(int j = 0;j<img->width*img->height;j++)
      {
         uint32_t cur = img->data[j];
         int32_t cr = color32_r(cur);
         int32_t cg = color32_g(cur);
         int32_t cb = color32_b(cur);
         int32_t ca = color32_a(cur);

         int center_index = i-1;
         int32_t r = color32_r(centers[center_index]);
         int32_t g = color32_g(centers[center_index]);
         int32_t b = color32_b(centers[center_index]);
         int32_t a = color32_a(centers[center_index]);

         uint64_t dist = (cr-r)*(cr-r);
         dist+=(cg-g)*(cg-g);
         dist+=(cb-b)*(cb-b);

         if(dist<distance[j])
            distance[j] = dist;
         dist_sum+=distance[j];
      }

      //Weighted random to choose next centeroid
      uint64_t random = 0;
      if(dist_sum!=0)
         random = rand_xor_next(&rng)%dist_sum;
      int found = 0;
      uint64_t dist_cur = 0;
      for(int j = 0;j<img->width*img->height;j++)
      {
         dist_cur+=distance[j];
         if(random<dist_cur)
         {
            centers[i] = img->data[j];
            //HLH_array_push(centers,img->data[j]);
            found = 1;
            break;
         }
      }

      if(!found)
         centers[i] = img->data[rand_xor_next(&rng)%(img->width*img->height)];
         //HLH_array_push(centers,img->data[rand_xor_next(&rng)%(img->w*img->h)]);
   }

   HLH_array_free(distance);
   //-------------------------------------

   for(int i = 0;i<256;i++)
      palette[i] = centers[i];

   for(int i = 0;i<16;i++)
   {
      for(int j = 0;j<256;j++)
         HLH_array_length_set(clusters[j],0);

      for(int j = 0;j<img->width*img->height;j++)
      {
         uint32_t cur = img->data[j];
         int32_t cr = color32_r(cur);
         int32_t cg = color32_g(cur);
         int32_t cb = color32_b(cur);
         int32_t ca = color32_a(cur);

         uint64_t dist_min = UINT64_MAX;
         int min_i = 0;
         for(int c = 0;c<256;c++)
         {
            int32_t r = color32_r(centers[c]);
            int32_t g = color32_g(centers[c]);
            int32_t b = color32_b(centers[c]);
            int32_t a = color32_a(centers[c]);

            uint64_t dist = (cr-r)*(cr-r);
            dist+=(cg-g)*(cg-g);
            dist+=(cb-b)*(cb-b);

            if(dist<dist_min)
            {
               dist_min = dist;
               min_i = c;
            }
         }

         HLH_array_push(clusters[min_i],cur);
      }

      //Recalculate centers
      for(int j = 0;j<256;j++)
      {
         uint64_t sum_r = 0;
         uint64_t sum_g = 0;
         uint64_t sum_b = 0;
         for(int c = 0;c<(int)HLH_array_length(clusters[j]);c++)
         {
            sum_r+=color32_r(clusters[j][c]);
            sum_g+=color32_g(clusters[j][c]);
            sum_b+=color32_b(clusters[j][c]);
         }

         if(HLH_array_length(clusters[j])>0)
         {
            uint32_t r = sum_r/HLH_array_length(clusters[j]);
            uint32_t g = sum_g/HLH_array_length(clusters[j]);
            uint32_t b = sum_b/HLH_array_length(clusters[j]);
            uint32_t a = 255;
            centers[j] = color32(r,g,b,a);
            palette[j] = centers[j];
         }
         //Choose random data point in that case
         //Not the best solution but better than not filling these colors
         //This path doesn't really seem to get hit though
         else
         {
            centers[j] = img->data[rand()%(img->width*img->height)];
            palette[j] = color32(0,0,0,255);
         }

      }
   }

   for(int i = 0;i<256;i++)
      HLH_array_free(clusters[i]);

   Image8 *img8 = image8_new(img->width,img->height);
   img8->color_count = 256;
   for(int i = 0;i<256;i++)
      img8->palette[i] = palette[i];

   for(int i = 0;i<img->width*img->height;i++)
   {
      uint32_t cur = img->data[i];
      int32_t cr = color32_r(cur);
      int32_t cg = color32_g(cur);
      int32_t cb = color32_b(cur);
      int32_t ca = color32_a(cur);

      uint8_t index_min = 0;
      uint64_t dist_min = UINT64_MAX;

      for(int j = 0;j<256;j++)
      {
         int32_t r = color32_r(palette[j]);
         int32_t g = color32_g(palette[j]);
         int32_t b = color32_b(palette[j]);
         int32_t a = color32_a(palette[j]);

         uint64_t dist = (cr-r)*(cr-r);
         dist+=(cg-g)*(cg-g);
         dist+=(cb-b)*(cb-b);

         if(dist<dist_min)
         {
            dist_min = dist;
            index_min = j;
         }
      }

      img8->data[i] = index_min;
   }

   return img8;
}

Image8 *Image8_dup(const Image8 *src)
{
   if(src==NULL||src->width<=0||src->height<=0)
      return NULL;

   Image8 *img = malloc(sizeof(*img)+sizeof(*img->data)*src->width*src->height);
   img->width = src->width;
   img->height = src->height;
   img->color_count = src->color_count;
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

int image8_save(const Image8 *img, const char *path, const char *ext)
{
   if(ext==NULL||path==NULL)
      return 1;
   if(strlen(ext)==0||strlen(path)==0)
      return 1;

   if(strcmp(ext,"pcx")==0||strcmp(ext,"PCX")==0)
      return pcx_save(img,path);
   else if(strcmp(ext,"png")==0||strcmp(ext,"PNG")==0)
      return png_save(img,path);

   return 1;
}

Image8 *image8_load(const char *path, const char *ext)
{
   if(ext==NULL||path==NULL)
      return NULL;
   if(strlen(ext)==0||strlen(path)==0)
      return NULL;

   if(strcmp(ext,"pcx")==0||strcmp(ext,"PCX")==0)
      return pcx_load(path);
   else if(strcmp(ext,"png")==0||strcmp(ext,"PNG")==0)
      return png_load(path);

   return NULL;
}
//-------------------------------------
