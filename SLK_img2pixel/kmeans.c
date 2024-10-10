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
#include <limits.h>

#ifdef _OPENMP
#include <omp.h>
#endif
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
static uint32_t *choose_centers(Image32 *img, int k, uint64_t seed, int kmeanspp);

static uint64_t rand_murmur3_avalanche64(uint64_t h);
static void rand_xor_seed(rand_xor *xor, uint64_t seed);
static uint64_t rand_xor_next(rand_xor *xor);
//-------------------------------------

//Function implementations

void image32_kmeans(Image32 *img, uint32_t *palette, int colors, uint64_t seed, int kmeanspp)
{
   if(img==NULL)
      return;
   if(palette==NULL)
      return;

   uint32_t *centers = choose_centers(img,colors,seed,kmeanspp);
   uint32_t **clusters = malloc(sizeof(*clusters)*colors);
   memset(clusters,0,sizeof(*clusters)*colors);

#ifdef _OPENMP
   omp_lock_t locks[256];
   for(int i = 0;i<colors;i++)
      omp_init_lock(&locks[i]);
#endif

   for(int i = 0;i<colors;i++)
      palette[i] = centers[i];

   for(int i = 0;i<8;i++)
   {
      //Reset clusters
      for(int j = 0;j<colors;j++)
         HLH_array_length_set(clusters[j],0);

#pragma omp parallel for
      for(int j = 0;j<img->width*img->height;j++)
      {
         uint32_t cur = img->data[j];
         int32_t cr = SLK_color32_r(cur);
         int32_t cg = SLK_color32_g(cur);
         int32_t cb = SLK_color32_b(cur);
         int32_t ca = SLK_color32_a(cur);

         uint64_t dist_min = UINT64_MAX;
         int min_i = 0;
         for(int c = 0;c<(int)HLH_array_length(centers);c++)
         {
            int32_t r = SLK_color32_r(centers[c]);
            int32_t g = SLK_color32_g(centers[c]);
            int32_t b = SLK_color32_b(centers[c]);
            int32_t a = SLK_color32_a(centers[c]);

            uint64_t dist = (cr-r)*(cr-r);
            dist+=(cg-g)*(cg-g);
            dist+=(cb-b)*(cb-b);

            if(dist<dist_min)
            {
               dist_min = dist;
               min_i = c;
            }
         }

#ifdef _OPENMP
         omp_set_lock(&locks[min_i]);
#endif

         HLH_array_push(clusters[min_i],cur);

#ifdef _OPENMP
         omp_unset_lock(&locks[min_i]);
#endif
      }

      //Recalculate centers
#pragma omp parallel for
      for(int j = 0;j<colors;j++)
      {
         uint64_t sum_r = 0;
         uint64_t sum_g = 0;
         uint64_t sum_b = 0;
         for(int c = 0;c<(int)HLH_array_length(clusters[j]);c++)
         {
            sum_r+=SLK_color32_r(clusters[j][c]);
            sum_g+=SLK_color32_g(clusters[j][c]);
            sum_b+=SLK_color32_b(clusters[j][c]);
         }

         if(HLH_array_length(clusters[j])>0)
         {
            uint32_t r = sum_r/HLH_array_length(clusters[j]);
            uint32_t g = sum_g/HLH_array_length(clusters[j]);
            uint32_t b = sum_b/HLH_array_length(clusters[j]);
            uint32_t a = 255;
            centers[j] = (r)|(g<<8)|(b<<16)|(a<<24);
            palette[j] = centers[j];
         }
         //Choose random data point in that case
         //Not the best solution but better than not filling these colors
         //This path doesn't really seem to get hit though
         else
         {
            centers[j] = img->data[rand()%(img->width*img->height)];
            palette[j] = 0xff000000;
         }

      }
   }

#ifdef _OPENMP
   for(int i = 0;i<colors;i++)
      omp_destroy_lock(&locks[i]);
#endif

   HLH_array_free(centers);
   for(int i = 0;i<colors;i++)
      HLH_array_free(clusters[i]);
   free(clusters);
}

uint32_t image32_kmeans_largest(Image32 *img, uint32_t *palette, int colors, uint64_t seed)
{
   if(img==NULL||img->width<=0||img->height<=0)
      return 0xff000000;
   if(palette==NULL)
      return 0xff000000;

   uint32_t *centers = choose_centers(img,colors,seed,1);
   uint32_t **clusters = malloc(sizeof(*clusters)*colors);
   memset(clusters,0,sizeof(*clusters)*colors);

   for(int i = 0;i<8;i++)
   {
      //Reset clusters
      for(int j = 0;j<colors;j++)
         HLH_array_length_set(clusters[j],0);

      for(int j = 0;j<img->width*img->height;j++)
      {
         uint32_t cur = img->data[j];
         int32_t cr = SLK_color32_r(cur);
         int32_t cg = SLK_color32_g(cur);
         int32_t cb = SLK_color32_b(cur);

         uint64_t dist_min = UINT64_MAX;
         int min_i = 0;
         for(int c = 0;c<(int)HLH_array_length(centers);c++)
         {
            int32_t r = SLK_color32_r(centers[c]);
            int32_t g = SLK_color32_g(centers[c]);
            int32_t b = SLK_color32_b(centers[c]);

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
      for(int j = 0;j<colors;j++)
      {
         uint64_t sum_r = 0;
         uint64_t sum_g = 0;
         uint64_t sum_b = 0;
         for(int c = 0;c<(int)HLH_array_length(clusters[j]);c++)
         {
            sum_r+=SLK_color32_r(clusters[j][c]);
            sum_g+=SLK_color32_g(clusters[j][c]);
            sum_b+=SLK_color32_b(clusters[j][c]);
         }

         if(HLH_array_length(clusters[j])>0)
         {
            uint32_t r = (sum_r/HLH_array_length(clusters[j]));
            uint32_t g = (sum_g/HLH_array_length(clusters[j]));
            uint32_t b = (sum_b/HLH_array_length(clusters[j]));
            uint32_t a = 255;
            centers[j] = (r)|(g<<8)|(b<<16)|(a<<24);
         }
         //Choose random data point in that case
         //Not the best solution but better than not filling these colors
         //This path doesn't really seem to get hit though
         else
         {
            centers[j] = img->data[rand()%(img->width*img->height)];
         }

         palette[j] = centers[j];
      }
   }
   
   uint32_t largest = palette[0];
   size_t max_size = 0;
   for(int i = 0;i<colors;i++)
   {
      if(HLH_array_length(clusters[i])>max_size)
      {
         max_size = HLH_array_length(clusters[i]);
         largest = palette[i];
      }
   }

   HLH_array_free(centers);
   for(int i = 0;i<colors;i++)
      HLH_array_free(clusters[i]);
   free(clusters);

   return largest;
}

static uint32_t *choose_centers(Image32 *img, int k, uint64_t seed, int kmeanspp)
{
   if(img==NULL||img->width<=0||img->height<=0)
      return NULL;

   rand_xor rng;
   rand_xor_seed(&rng,seed);
   uint32_t *centers = NULL;

   if(!kmeanspp)
   {
      for(int i = 0;i<k;i++)
      {
         int index = rand_xor_next(&rng)%(img->width*img->height);
         HLH_array_push(centers,img->data[index]);
      }

      return centers;
   }

   //Choose initial center
   int index = rand_xor_next(&rng)%(img->width*img->height);
   HLH_array_push(centers,img->data[index]);

   uint64_t *distance = NULL;
   HLH_array_length_set(distance,img->width*img->height);
   for(int i = 0;i<img->width*img->height;i++)
      distance[i] = UINT64_MAX;
   
   for(int i = 1;i<k;i++)
   {
      uint64_t dist_sum = 0;
      for(int j = 0;j<img->width*img->height;j++)
      {
         uint32_t cur = img->data[j];
         int32_t cr = SLK_color32_r(cur);
         int32_t cg = SLK_color32_g(cur);
         int32_t cb = SLK_color32_b(cur);
         int32_t ca = SLK_color32_a(cur);

         int center_index = HLH_array_length(centers)-1;
         int32_t r = SLK_color32_r(centers[center_index]);
         int32_t g = SLK_color32_g(centers[center_index]);
         int32_t b = SLK_color32_b(centers[center_index]);
         int32_t a = SLK_color32_a(centers[center_index]);

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
            HLH_array_push(centers,img->data[j]);
            found = 1;
            break;
         }
      }

      if(!found)
         HLH_array_push(centers,img->data[rand_xor_next(&rng)%(img->width*img->height)]);
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
