/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
//-------------------------------------

//Internal includes
#include "../external/HLH.h"
#include "img2pixel.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
typedef uint64_t rand_xor[2];
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static uint32_t *choose_centers(SLK_image32 *img, int k, uint64_t seed);

static uint64_t rand_murmur3_avalanche64(uint64_t h);
static void rand_xor_seed(rand_xor *xor, uint64_t seed);
static uint64_t rand_xor_next(rand_xor *xor);
//-------------------------------------

//Function implementations

void SLK_image32_kmeans(SLK_image32 *img, uint32_t *palette, int colors, uint64_t seed)
{
   uint8_t *asign = NULL;
   uint32_t *centers = choose_centers(img,colors,seed);
   uint32_t **clusters = malloc(sizeof(*clusters)*colors);
   memset(clusters,0,sizeof(*clusters)*colors);

   for(int i = 0;i<16;i++)
   {
      //Reset clusters
      for(int j = 0;j<colors;j++)
         HLH_array_length_set(clusters[j],0);

      HLH_array_length_set(asign,0);

      for(int j = 0;j<img->w*img->h;j++)
      {
         uint32_t cur = img->data[j];
         int32_t cr = cur&255;
         int32_t cg = (cur>>8)&255;
         int32_t cb = (cur>>16)&255;
         int32_t ca = (cur>>24)&255;

         uint64_t dist_min = UINT64_MAX;
         int min_i = 0;
         for(int c = 0;c<HLH_array_length(centers);c++)
         {
            int32_t r = centers[c]&255;
            int32_t g = (centers[c]>>8)&255;
            int32_t b = (centers[c]>>16)&255;
            int32_t a = (centers[c]>>24)&255;

            uint64_t dist = (cr-r)*(cr-r);
            dist+=(cg-g)*(cg-g);
            dist+=(cb-b)*(cb-b);

            //uint64_t dist = (cur.r-centers[c].r)*(cur.r-centers[c].r);
            //dist+=(cur.g-centers[c].g)*(cur.g-centers[c].g);
            //dist+=(cur.b-centers[c].b)*(cur.b-centers[c].b);

            if(dist<dist_min)
            {
               dist_min = dist;
               min_i = c;
            }
         }

         HLH_array_push(clusters[min_i],cur);
         HLH_array_push(asign,min_i);
      }

      //Recalculate centers
      for(int j = 0;j<colors;j++)
      {
         uint64_t sum_r = 0;
         uint64_t sum_g = 0;
         uint64_t sum_b = 0;
         for(int c = 0;c<HLH_array_length(clusters[j]);c++)
         {
            sum_r+=clusters[j][c]&255;
            sum_g+=(clusters[j][c]>>8)&255;
            sum_b+=(clusters[j][c]>>16)&255;
         }

         if(HLH_array_length(clusters[j])>0)
         {
            centers[j]&=0xff000000;
            centers[j]|= (sum_r/HLH_array_length(clusters[j]));
            centers[j]|= (sum_g/HLH_array_length(clusters[j]))<<8;
            centers[j]|= (sum_b/HLH_array_length(clusters[j]))<<16;
         }
         //Choose random data point in that case
         //Not the best solution but better than not filling these colors
         //This path doesn't really seem to get hit though
         else
         {
            centers[j] = img->data[rand()%(img->w*img->h)];
         }
      }
   }

   HLH_array_free(centers);
   for(int i = 0;i<colors;i++)
      HLH_array_free(clusters[i]);
   free(clusters);
}

static uint32_t *choose_centers(SLK_image32 *img, int k, uint64_t seed)
{
   rand_xor rng;
   rand_xor_seed(&rng,seed);

   uint32_t *centers = NULL;

   //Choose initial center
   int index = rand_xor_next(&rng)%(img->w*img->h);
   HLH_array_push(centers,img->data[index]);

   uint64_t *distance = NULL;
   HLH_array_length_set(distance,img->w*img->h);
   for(int i = 0;i<img->w*img->h;i++)
      distance[i] = UINT64_MAX;
   
   for(int i = 1;i<k;i++)
   {
      uint64_t dist_sum = 0;
      for(int j = 0;j<img->w*img->h;j++)
      {
         uint32_t cur = img->data[j];
         int32_t cr = cur&255;
         int32_t cg = (cur>>8)&255;
         int32_t cb = (cur>>16)&255;
         int32_t ca = (cur>>24)&255;

         int center_index = HLH_array_length(centers)-1;
         int32_t r = centers[center_index]&255;
         int32_t g = (centers[center_index]>>8)&255;
         int32_t b = (centers[center_index]>>16)&255;
         int32_t a = (centers[center_index]>>24)&255;

         uint64_t dist = (cr-r)*(cr-r);
         dist+=(cg-g)*(cg-g);
         dist+=(cb-b)*(cb-b);
         //uint64_t dist = (centers[center_index].r-cur.r)*(centers[center_index].r-cur.r);
         //dist+=(centers[center_index].g-cur.g)*(centers[center_index].g-cur.g);
         //dist+=(centers[center_index].b-cur.b)*(centers[center_index].b-cur.b);

         if(dist<distance[j])
            distance[j] = dist;
         dist_sum+=distance[j];
      }

      //Weighted random to choose next centeroid
      uint64_t random = 0;
      if(dist_sum!=0)
         rand_xor_next(&rng)%dist_sum;
      uint64_t dist_cur = 0;
      for(int j = 0;j<img->w*img->h;j++)
      {
         dist_cur+=distance[j];
         if(random<dist_cur)
         {
            HLH_array_push(centers,img->data[j]);
            break;
         }
      }
   }

   HLH_array_free(distance);

   return centers;
}

static uint64_t rand_murmur3_avalanche64(uint64_t h)
{
   h ^= h >> 33;
   h *= 0xff51afd7ed558ccd;
   h ^= h >> 33;
   h *= 0xc4ceb9fe1a85ec53;
   h ^= h >> 33;

   return h;
}

static void rand_xor_seed(rand_xor *xor, uint64_t seed)
{
   uint64_t value = rand_murmur3_avalanche64((seed << 1) | 1);
   (*xor)[0] = value;
   value = rand_murmur3_avalanche64(value);
   (*xor)[1] = value;
}

static uint64_t rand_xor_next(rand_xor *xor)
{
   uint64_t x = (*xor)[0];
   uint64_t const y = (*xor)[1];
   (*xor)[0] = y;
   x ^= x << 23;
   x ^= x >> 17;
   x ^= y ^ (y >> 26);
   (*xor)[1] = x;

   return x + y;
}
//-------------------------------------
