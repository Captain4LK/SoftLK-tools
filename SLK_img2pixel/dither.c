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
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "HLH.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
//-------------------------------------

//#defines
#define PI32 3.14159265359f
#define DEG2RAD(a) \
   ((a)*PI32/180.0f)
//-------------------------------------

//Typedefs
typedef struct
{
   float c0;
   float c1;
   float c2;
}slk_color3f;

typedef uint64_t rand_xor[2];
//-------------------------------------

//Variables
static const float slk_dither_threshold_bayer8x8[64] = 
{
    0.0f/64.0f,32.0f/64.0f, 8.0f/64.0f,40.0f/64.0f, 2.0f/64.0f,34.0f/64.0f,10.0f/64.0f,42.0f/64.0f,
   48.0f/64.0f,16.0f/64.0f,56.0f/64.0f,24.0f/64.0f,50.0f/64.0f,18.0f/64.0f,58.0f/64.0f,26.0f/64.0f,
   12.0f/64.0f,44.0f/64.0f, 4.0f/64.0f,36.0f/64.0f,14.0f/64.0f,46.0f/64.0f, 6.0f/64.0f,38.0f/64.0f,
   60.0f/64.0f,28.0f/64.0f,52.0f/64.0f,20.0f/64.0f,62.0f/64.0f,30.0f/64.0f,54.0f/64.0f,22.0f/64.0f,
    3.0f/64.0f,35.0f/64.0f,11.0f/64.0f,43.0f/64.0f, 1.0f/64.0f,33.0f/64.0f, 9.0f/64.0f,41.0f/64.0f,
   51.0f/64.0f,19.0f/64.0f,59.0f/64.0f,27.0f/64.0f,49.0f/64.0f,17.0f/64.0f,57.0f/64.0f,25.0f/64.0f,
   15.0f/64.0f,47.0f/64.0f, 7.0f/64.0f,39.0f/64.0f,13.0f/64.0f,45.0f/64.0f, 5.0f/64.0f,37.0f/64.0f,
   63.0f/64.0f,31.0f/64.0f,55.0f/64.0f,23.0f/64.0f,61.0f/64.0f,29.0f/64.0f,53.0f/64.0f,21.0f/64.0f,
};
static const float slk_dither_threshold_bayer4x4[16] = 
{
   0.0f/16.0f,8.0f/16.0f,2.0f/16.0f,10.0f/16.0f,
   12.0f/16.0f,4.0f/16.0f,14.0f/16.0f,6.0f/16.0f,
   3.0f/16.0f,11.0f/16.0f,1.0f/16.0f,9.0f/16.0f,
   15.0f/16.0f,7.0f/16.0f,13.0f/16.0f,5.0f/16.0f,
};
static const float slk_dither_threshold_bayer2x2[4] = 
{
   0.0f/4.0f,2.0f/4.0f,
   3.0f/4.0f,1.0f/4.0f
};
static const float slk_dither_threshold_cluster8x8[64] = 
{
   24.0f/64.0f,10.0f/64.0f,12.0f/64.0f,26.0f/64.0f,35.0f/64.0f,47.0f/64.0f,49.0f/64.0f,37.0f/64.0f,
   8.0f/64.0f,0.0f/64.0f,2.0f/64.0f,14.0f/64.0f,45.0f/64.0f,59.0f/64.0f,61.0f/64.0f,51.0f/64.0f,
   22.0f/64.0f,6.0f/64.0f,4.0f/64.0f,16.0f/64.0f,43.0f/64.0f,57.0f/64.0f,63.0f/64.0f,53.0f/64.0f,
   30.0f/64.0f,20.0f/64.0f,18.0f/64.0f,28.0f/64.0f,33.0f/64.0f,41.0f/64.0f,55.0f/64.0f,39.0f/64.0f,
   34.0f/64.0f,46.0f/64.0f,48.0f/64.0f,36.0f/64.0f,25.0f/64.0f,11.0f/64.0f,13.0f/64.0f,27.0f/64.0f,
   44.0f/64.0f,58.0f/64.0f,60.0f/64.0f,50.0f/64.0f,9.0f/64.0f,1.0f/64.0f,3.0f/64.0f,15.0f/64.0f,
   42.0f/64.0f,56.0f/64.0f,62.0f/64.0f,52.0f/64.0f,23.0f/64.0f,7.0f/64.0f,5.0f/64.0f,17.0f/64.0f,
   32.0f/64.0f,40.0f/64.0f,54.0f/64.0f,38.0f/64.0f,31.0f/64.0f,21.0f/64.0f,19.0f/64.0f,29.0f/64.0f,
};
static const float slk_dither_threshold_cluster4x4[16] = 
{
   12.0f/16.0f,5.0f/16.0f,6.0f/16.0f,13.0f/16.0f,
   4.0f/16.0f,0.0f/16.0f,1.0f/16.0f,7.0f/16.0f,
   11.0f/16.0f,3.0f/16.0f,2.0f/16.0f,8.0f/16.0f,
   15.0f/16.0f,10.0f/16.0f,9.0f/16.0f,14.0f/16.0f,
};

static float slk_palette[256][3];
static int slk_palette_size = 0;
//-------------------------------------

//Function prototypes
static SLK_image32 *slk_dither_closest(SLK_image64 *img, const SLK_dither_config *config);
static SLK_image32 *slk_dither_kmeans(SLK_image64 *img, const SLK_dither_config *config);
static void slk_dither_threshold_apply(SLK_image64 *img, int dim, const float *threshold, const SLK_dither_config *config);
static SLK_image32 *slk_dither_floyd(SLK_image64 *img, const SLK_dither_config *config);
static SLK_image32 *slk_dither_floyd2(SLK_image64 *img, const SLK_dither_config *config);
static void slk_floyd_apply_error(SLK_image64 *img, float er, float eg, float eb, int x, int y);

//static slk_color3f *choose_centers(const SLK_dither_config *config);
static uint32_t *choose_centers(SLK_image64 *img, int k, uint64_t seed);

static void slk_color32_to_lab(uint32_t c, float *l0, float *l1, float *l2);
static void slk_color32_to_rgb(uint32_t c, float *l0, float *l1, float *l2);

static uint32_t slk_color_closest(uint64_t c, const SLK_dither_config *config);
static float slk_dist_rgb_euclidian(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_rgb_weighted(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_rgb_redmean(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_cie76(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_cie94(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_ciede2000(float b0, float b1, float b2, float c0, float c1, float c2);

static uint64_t rand_murmur3_avalanche64(uint64_t h);
static void rand_xor_seed(rand_xor *xor, uint64_t seed);
static uint64_t rand_xor_next(rand_xor *xor);

static uint8_t *kuhn_match(int n, int m, double *table);
static void kuhn_reduce_rows(int n, int m, double *table);
static uint8_t *kuhn_mark(int n, int m, double *table);
static int kuhn_is_done(int n, int m, uint8_t *marks, uint8_t *covered);
static int kuhn_find_prime(int n, int m, double *table, uint8_t *marks, uint8_t *row_covered,uint8_t *col_covered,uint32_t *prime);
static void kuhn_add_subtract(int n, int m, double *table, uint8_t *row_covered,uint8_t *col_covered);
static void kuhn_alt_marks(int n, int m, uint8_t *marks, uint32_t *alt, int *col_marks, int *row_primes, uint32_t *prime);
static uint8_t *kuhn_assign(int n, int m, uint8_t *marks);
//-------------------------------------

//Function implementations

SLK_image32 *SLK_image64_dither(SLK_image64 *img, const SLK_dither_config *config)
{
   //Convert palette to desired format
   switch(config->color_dist)
   {
   case SLK_RGB_EUCLIDIAN:
   case SLK_RGB_WEIGHTED:
   case SLK_RGB_REDMEAN:
      slk_palette_size = config->palette_colors;
      for(int i = 0;i<config->palette_colors;i++)
         slk_color32_to_rgb(config->palette[i],&slk_palette[i][0],&slk_palette[i][1],&slk_palette[i][2]);
      break;
   case SLK_LAB_CIE76:
   case SLK_LAB_CIE94:
   case SLK_LAB_CIEDE2000:
      slk_palette_size = config->palette_colors;
      for(int i = 0;i<config->palette_colors;i++)
         slk_color32_to_lab(config->palette[i],&slk_palette[i][0],&slk_palette[i][1],&slk_palette[i][2]);
      break;
   }

   if(config->use_kmeans)
   {
      return slk_dither_kmeans(img,config);
   }

   switch(config->dither_mode)
   {
   case SLK_DITHER_BAYER8X8: slk_dither_threshold_apply(img,3,slk_dither_threshold_bayer8x8,config); break;
   case SLK_DITHER_BAYER4X4: slk_dither_threshold_apply(img,2,slk_dither_threshold_bayer4x4,config); break;
   case SLK_DITHER_BAYER2X2: slk_dither_threshold_apply(img,1,slk_dither_threshold_bayer2x2,config); break;
   case SLK_DITHER_CLUSTER8X8: slk_dither_threshold_apply(img,3,slk_dither_threshold_cluster8x8,config); break;
   case SLK_DITHER_CLUSTER4X4: slk_dither_threshold_apply(img,2,slk_dither_threshold_cluster4x4,config); break;
   default: break;
   }

   switch(config->dither_mode)
   {
   case SLK_DITHER_NONE:
   case SLK_DITHER_BAYER8X8:
   case SLK_DITHER_BAYER4X4:
   case SLK_DITHER_BAYER2X2:
   case SLK_DITHER_CLUSTER8X8:
   case SLK_DITHER_CLUSTER4X4:
      return slk_dither_closest(img,config);
   case SLK_DITHER_FLOYD: return slk_dither_floyd(img,config);
   case SLK_DITHER_FLOYD2: return slk_dither_floyd2(img,config);
   }
   
   return slk_dither_closest(img,config);
}

static SLK_image32 *slk_dither_closest(SLK_image64 *img, const SLK_dither_config *config)
{
   SLK_image32 *out = SLK_image32_dup64(img);

#pragma omp parallel for
   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         if((int)(SLK_color64_a(p)/128)<config->alpha_threshold)
         {
            out->data[y*img->w+x] = 0;
            continue;
         }

         out->data[y*img->w+x] = slk_color_closest(p,config);
      }
   }

   return out;
}

static SLK_image32 *slk_dither_kmeans(SLK_image64 *img, const SLK_dither_config *config)
{
   int target = HLH_min(config->target_colors,config->palette_colors);
   //Create clusters
   uint8_t *asign = NULL;
   uint32_t *centers = choose_centers(img,target,0xdeadbeef);
   uint32_t **clusters = malloc(sizeof(*clusters)*target);
   memset(clusters,0,sizeof(*clusters)*target);
   HLH_array_length_set(asign,img->w*img->h);

#ifdef _OPENMP
   omp_lock_t locks[256];
   for(int i = 0;i<target;i++)
      omp_init_lock(&locks[i]);
#endif


   for(int i = 0;i<16;i++)
   {
      //Reset clusters
      for(int j = 0;j<target;j++)
         HLH_array_length_set(clusters[j],0);

#pragma omp parallel for
      for(int j = 0;j<img->w*img->h;j++)
      {
         uint32_t cur = SLK_color64_to_32(img->data[j]);
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

         asign[j] = min_i;
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
      for(int j = 0;j<target;j++)
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
         }
         //Choose random data point in that case
         //Not the best solution but better than not filling these colors
         //This path doesn't really seem to get hit though
         else
         {
            centers[j] = SLK_color64_to_32(img->data[rand()%(img->w*img->h)]);
         }

      }
   }


#ifdef _OPENMP
   for(int i = 0;i<target;i++)
      omp_destroy_lock(&locks[i]);
#endif

   int slow = 0;
   uint8_t *assign_map = malloc(sizeof(*assign_map)*target);

   //Compact clusters
   for(int i = 0;i<target;i++)
   {
      if(HLH_array_length(clusters[i])>0)
      {
         if(i!=slow)
            HLH_array_free(clusters[slow]);

         assign_map[i] = slow;
         clusters[slow] = clusters[i];
         if(i!=slow)
            clusters[i] = NULL;
         slow++;
      }
   }

   //Count non-zero
   int num_non_zero = 0;
   for(int i = 0;i<target;i++)
   {
      if(HLH_array_length(clusters[i])>0)
         num_non_zero++;
   }

   //Calculate errors
   double *errors = calloc(config->palette_colors*num_non_zero,sizeof(*errors));
   int cluster_current = 0;
   for(int i = 0;i<target;i++)
   {
      if(HLH_array_length(clusters[i])<=0)
         continue;

      //for(int j = 0;j<HLH_array_length(clusters[i]);j++)
      //{
         float c0,c1,c2;
         uint32_t c = centers[i];
         switch(config->color_dist)
         {
         case SLK_RGB_EUCLIDIAN:
         case SLK_RGB_WEIGHTED:
         case SLK_RGB_REDMEAN:
            slk_color32_to_rgb(c,&c0,&c1,&c2);
            break;
         case SLK_LAB_CIE76:
         case SLK_LAB_CIE94:
         case SLK_LAB_CIEDE2000:
            slk_color32_to_lab(c,&c0,&c1,&c2);
            break;
         }

         for(int p = 0;p<config->palette_colors;p++)
         {
            float dist = 0.f;
            switch(config->color_dist)
            {
            case SLK_RGB_EUCLIDIAN:
               dist = slk_dist_rgb_euclidian(slk_palette[p][0],slk_palette[p][1],slk_palette[p][2], c0, c1, c2);
               break;
            case SLK_RGB_WEIGHTED:
               dist = slk_dist_rgb_weighted(slk_palette[p][0],slk_palette[p][1],slk_palette[p][2], c0, c1, c2);
               break;
            case SLK_RGB_REDMEAN:
               dist = slk_dist_rgb_redmean(slk_palette[p][0],slk_palette[p][1],slk_palette[p][2], c0, c1, c2);
               break;
            case SLK_LAB_CIE76:
               dist = slk_dist_cie76(slk_palette[p][0],slk_palette[p][1],slk_palette[p][2], c0, c1, c2);
               break;
            case SLK_LAB_CIE94:
               dist = slk_dist_cie94(slk_palette[p][0],slk_palette[p][1],slk_palette[p][2], c0, c1, c2);
               break;
            case SLK_LAB_CIEDE2000:
               dist = slk_dist_ciede2000(slk_palette[p][0],slk_palette[p][1],slk_palette[p][2], c0, c1, c2);
               break;
            }

            errors[i*config->palette_colors+p]+=dist;
         }
      //}
   }

   //Find best asignment
   uint8_t *asign_lowest = kuhn_match(num_non_zero,config->palette_colors,errors);
   free(errors);
   SLK_image32 *out = SLK_image32_dup64(img);
   for(int i = 0;i<img->w*img->h;i++)
      out->data[i] = config->palette[asign_lowest[assign_map[asign[i]]]];

   HLH_array_free(centers);
   for(int i = 0;i<target;i++)
      HLH_array_free(clusters[i]);
   free(clusters);
   free(assign_map);
   free(asign_lowest);
   HLH_array_free(asign);

   return out;
}

static void slk_dither_threshold_apply(SLK_image64 *img, int dim, const float *threshold, const SLK_dither_config *config)
{
#pragma omp parallel for
   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         uint8_t mod = (1<<dim)-1;
         uint8_t threshold_id = ((y&mod)<<dim)+(x&mod);
         uint64_t r = HLH_max(0,HLH_min(0x7fff,SLK_color64_r(p)+0x7fff*(config->dither_amount/8)*(threshold[threshold_id]-0.5f)));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,SLK_color64_g(p)+0x7fff*(config->dither_amount/8)*(threshold[threshold_id]-0.5f)));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,SLK_color64_b(p)+0x7fff*(config->dither_amount/8)*(threshold[threshold_id]-0.5f)));
         uint64_t a = SLK_color64_a(p);
         img->data[y*img->w+x] = (r)|(g<<16)|(b<<32)|(a<<48);
      }
   }
}

static SLK_image32 *slk_dither_floyd(SLK_image64 *img, const SLK_dither_config *config)
{
   SLK_image32 *out = SLK_image32_dup64(img);

   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         if((int)(SLK_color64_a(p)/128)<config->alpha_threshold)
         {
            out->data[y*img->w+x] = 0;
            continue;
         }

         uint32_t c = slk_color_closest(p,config);
         float error_r = (float)SLK_color32_r(SLK_color64_to_32(p))-(float)SLK_color32_r(c);
         float error_g = (float)SLK_color32_g(SLK_color64_to_32(p))-(float)SLK_color32_g(c);
         float error_b = (float)SLK_color32_b(SLK_color64_to_32(p))-(float)SLK_color32_b(c);
         slk_floyd_apply_error(img,(error_r*7.f)/16.f,(error_g*7.f)/16.f,(error_b*7.f)/16.f,x+1,y);
         slk_floyd_apply_error(img,(error_r*3.f)/16.f,(error_g*3.f)/16.f,(error_b*3.f)/16.f,x-1,y+1);
         slk_floyd_apply_error(img,(error_r*5.f)/16.f,(error_g*5.f)/16.f,(error_b*5.f)/16.f,x,y+1);
         slk_floyd_apply_error(img,(error_r*1.f)/16.f,(error_g*1.f)/16.f,(error_b*1.f)/16.f,x+1,y+1);

         out->data[y*img->w+x] = c;
      }
   }

   return out;
}

static SLK_image32 *slk_dither_floyd2(SLK_image64 *img, const SLK_dither_config *config)
{
   SLK_image32 *out = SLK_image32_dup64(img);

   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         if((int)(SLK_color64_a(p)/128)<config->alpha_threshold)
         {
            out->data[y*img->w+x] = 0;
            continue;
         }

         uint32_t c = slk_color_closest(p,config);
         float error = (float)SLK_color32_r(SLK_color64_to_32(p))-(float)SLK_color32_r(c);
         error+=(float)SLK_color32_g(SLK_color64_to_32(p))-(float)SLK_color32_g(c);
         error+=(float)SLK_color32_b(SLK_color64_to_32(p))-(float)SLK_color32_b(c);
         error/=3.f;
         slk_floyd_apply_error(img,(error*7.f)/16.f,(error*7.f)/16.f,(error*7.f)/16.f,x+1,y);
         slk_floyd_apply_error(img,(error*3.f)/16.f,(error*3.f)/16.f,(error*3.f)/16.f,x-1,y+1);
         slk_floyd_apply_error(img,(error*5.f)/16.f,(error*5.f)/16.f,(error*5.f)/16.f,x,y+1);
         slk_floyd_apply_error(img,(error*1.f)/16.f,(error*1.f)/16.f,(error*1.f)/16.f,x+1,y+1);

         out->data[y*img->w+x] = c;
      }
   }

   return out;
}

static void slk_floyd_apply_error(SLK_image64 *img, float er, float eg, float eb, int x, int y)
{
   if(x<0||x>=img->w||y<0||y>=img->h)
      return;

   uint64_t p = img->data[y*img->w+x];
   uint64_t r = HLH_max(0,HLH_min(0x7fff,SLK_color64_r(p)+er*128.f));
   uint64_t g = HLH_max(0,HLH_min(0x7fff,SLK_color64_g(p)+eg*128.f));
   uint64_t b = HLH_max(0,HLH_min(0x7fff,SLK_color64_b(p)+eb*128.f));
   uint64_t a = SLK_color64_a(p);

   img->data[y*img->w+x] = (r)|(g<<16)|(b<<32)|(a<<48);
}

#if 0
static slk_color3f *choose_centers(const SLK_dither_config *config)
{
   slk_color3f *centers = NULL;

   for(int i = 0;i<config->palette_colors;i++)
   {
      slk_color3f c = {0};
      c.c0 = slk_palette[i][0];
      c.c1 = slk_palette[i][1];
      c.c2 = slk_palette[i][2];
      HLH_array_push(centers,c);
   }

   return centers;
}
#endif

static uint32_t *choose_centers(SLK_image64 *img, int k, uint64_t seed)
{
   rand_xor rng;
   rand_xor_seed(&rng,seed);
   uint32_t *centers = NULL;

   //Choose initial center
   int index = rand_xor_next(&rng)%(img->w*img->h);
   HLH_array_push(centers,SLK_color64_to_32(img->data[index]));

   uint64_t *distance = NULL;
   HLH_array_length_set(distance,img->w*img->h);
   for(int i = 0;i<img->w*img->h;i++)
      distance[i] = UINT64_MAX;
   
   for(int i = 1;i<k;i++)
   {
      uint64_t dist_sum = 0;
      for(int j = 0;j<img->w*img->h;j++)
      {
         uint64_t cur = img->data[j];
         int32_t cr = SLK_color32_r(SLK_color64_to_32(cur));
         int32_t cg = SLK_color32_g(SLK_color64_to_32(cur));
         int32_t cb = SLK_color32_b(SLK_color64_to_32(cur));
         int32_t ca = SLK_color32_a(SLK_color64_to_32(cur));

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
      for(int j = 0;j<img->w*img->h;j++)
      {
         dist_cur+=distance[j];
         if(random<dist_cur)
         {
            HLH_array_push(centers,SLK_color64_to_32(img->data[j]));
            found = 1;
            break;
         }
      }

      if(!found)
      {
         HLH_array_push(centers,SLK_color64_to_32(img->data[rand_xor_next(&rng)%(img->w*img->h)]));
      }
   }

   HLH_array_free(distance);

   return centers;
}

static void slk_color32_to_rgb(uint32_t c, float *l0, float *l1, float *l2)
{
   *l0 = (float)SLK_color32_r(c)/255.f;
   *l1 = (float)SLK_color32_g(c)/255.f;
   *l2 = (float)SLK_color32_b(c)/255.f;
}

static void slk_color32_to_lab(uint32_t c, float *l0, float *l1, float *l2)
{
   //Convert to rgb
   float r = (float)SLK_color32_r(c)/255.f;
   float g = (float)SLK_color32_g(c)/255.f;
   float b = (float)SLK_color32_b(c)/255.f;

   //Convert to xyz
   //-------------------------------------
   float x = 0.f;
   float y = 0.f;
   float z = 0.f;

   if(r>0.04045)
      r = powf((r+0.055)/1.055,2.4);
   else
      r = (r/12.92);
  
   if(g>0.04045)
      g = powf((g+0.055)/1.055,2.4);
   else
      g = (g/12.92);
  
   if(b>0.04045)
      b = powf((b+0.055)/1.055,2.4);
   else
      b = (b/12.92);

   x = (r*0.4124+g*0.3576+b*0.1805);
   y = (r*0.2126+g*0.7152+b*0.0722);
   z = (r*0.0193+g*0.1192+b*0.9504);
   //-------------------------------------

   //Convert to lab
   if(x>0.008856)
      x = powf(x,1.0/3.0);
   else
      x = (7.787*x)+(16.0/116.0);

   if(y>0.008856)
      y = powf(y,1.0/3.0);
   else
      y = (7.787*y)+(16.0/116.0);

   if(z>0.008856)
      z = powf(z,1.0/3.0);
   else
      z = (7.787*z)+(16.0/116.0);

   *l0 = 116.0*y-16.0;
   *l1 = 500.0*(x-y);
   *l2 = 200.0*(y-z);
   //-------------------------------------
}

static uint32_t slk_color_closest(uint64_t c, const SLK_dither_config *config)
{
   float min_dist = 1e12;
   int min_index = 0;

   float c0 = 0.f;
   float c1 = 0.f;
   float c2 = 0.f;
   switch(config->color_dist)
   {
   case SLK_RGB_EUCLIDIAN:
   case SLK_RGB_WEIGHTED:
   case SLK_RGB_REDMEAN:
      slk_color32_to_rgb(SLK_color64_to_32(c),&c0,&c1,&c2);
      break;
   case SLK_LAB_CIE76:
   case SLK_LAB_CIE94:
   case SLK_LAB_CIEDE2000:
      slk_color32_to_lab(SLK_color64_to_32(c),&c0,&c1,&c2);
      break;
   }

   for(int i = 0;i<slk_palette_size;i++)
   {
      float dist = 0.f;

      switch(config->color_dist)
      {
      case SLK_RGB_EUCLIDIAN:
         dist = slk_dist_rgb_euclidian(c0,c1,c2, slk_palette[i][0],slk_palette[i][1],slk_palette[i][2]);
         break;
      case SLK_RGB_WEIGHTED:
         dist = slk_dist_rgb_weighted(c0,c1,c2, slk_palette[i][0],slk_palette[i][1],slk_palette[i][2]);
         break;
      case SLK_RGB_REDMEAN:
         dist = slk_dist_rgb_redmean(c0,c1,c2, slk_palette[i][0],slk_palette[i][1],slk_palette[i][2]);
         break;
      case SLK_LAB_CIE76:
         dist = slk_dist_cie76(c0,c1,c2, slk_palette[i][0],slk_palette[i][1],slk_palette[i][2]);
         break;
      case SLK_LAB_CIE94:
         dist = slk_dist_cie94(c0,c1,c2, slk_palette[i][0],slk_palette[i][1],slk_palette[i][2]);
         break;
      case SLK_LAB_CIEDE2000:
         dist = slk_dist_ciede2000(c0,c1,c2, slk_palette[i][0],slk_palette[i][1],slk_palette[i][2]);
         break;
      }

      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return config->palette[min_index];
}

static float slk_dist_rgb_euclidian(float a0, float a1, float a2, float b0, float b1, float b2)
{
   return (b0-a0)*(b0-a0)+(b1-a1)*(b1-a1)+(b2-a2)*(b2-a2);
}

static float slk_dist_rgb_weighted(float a0, float a1, float a2, float b0, float b1, float b2)
{
   if(a0+b0<1.f)
      return (b0-a0)*(b0-a0)*2.f+(b1-a1)*(b1-a1)*4.f+(b2-a2)*(b2-a2)*3.f;
   else
      return (b0-a0)*(b0-a0)*3.f+(b1-a1)*(b1-a1)*4.f+(b2-a2)*(b2-a2)*2.f;
}

static float slk_dist_rgb_redmean(float a0, float a1, float a2, float b0, float b1, float b2)
{
   float r = 0.5f*255.f*(a0+b0);
   return (2.f+r/256.f)*(b0-a0)*(b0-a0)+4.f*(b1-a1)*(b1-a1)+(2.f+(255.f-r)/256.f)*(b2-a2)*(b2-a2);
}

static float slk_dist_cie76(float a0, float a1, float a2, float b0, float b1, float b2)
{
   return (b0-a0)*(b0-a0)+(b1-a1)*(b1-a1)+(b2-a2)*(b2-a2);
}

static float slk_dist_cie94(float a0, float a1, float a2, float b0, float b1, float b2)
{
   float L = a0-b0;
   float C1 = sqrtf(a1*a1+a2*a2);
   float C2 = sqrtf(b1*b1+b2*b2);
   float C = C1-C2;
   float H = sqrtf((a1-b1)*(a1-b1)+(a2-b2)*(a2-b2)-C*C);
   float r1 = L;
   float r2 = C/(1.0f+0.045f*C1);
   float r3 = H/(1.0f+0.015f*C1);

   float dist = r1*r1+r2*r2+r3*r3;
   if(isnan(dist))
      dist = 1e15;
   return dist;
}

static float slk_dist_ciede2000(float b0, float b1, float b2, float c0, float c1, float c2)
{
   double C1 = sqrt(b1*b1+b2*b2);
   double C2 = sqrt(c1*c1+c2*c2);
   double C_ = (C1+C2)/2.0f;

   double C_p2 = pow(C_,7.0f);
   double v = 0.5f*(1.0f-sqrt(C_p2/(C_p2+6103515625.0f)));
   double a1 = (1.0f+v)*b1;
   double a2 = (1.0f+v)*c1;

   double Cs1 = sqrt(a1*a1+b2*b2);
   double Cs2 = sqrt(a2*a2+c2*c2);

   double h1 = 0.0f;
   if(b2!=0||a1!=0)
   {
      h1 = atan2(b2,a1);
      if(h1<0)
         h1+=2.0f*PI32;
   }
   double h2 = 0.0f;
   if(c2!=0||a2!=0)
   {
      h2 = atan2(c2,a2);
      if(h2<0)
         h2+=2.0f*PI32;
   }

   double L = c0-b0;
   double Cs = Cs2-Cs1;
   double h = 0.0f;
   if(Cs1*Cs2!=0.0f)
   {
      h = h2-h1;
      if(h<-PI32)
         h+=2*PI32;
      else if(h>PI32)
         h-=2*PI32;
   }
   double H = 2.0f*sqrt(Cs1*Cs2)*sin(h/2.0f);

   double L_ = (b0+c0)/2.0f;
   double Cs_ = (Cs1+Cs2)/2.0f;
   double H_ = h1+h2;
   if(Cs1*Cs2!=0.0f)
   {
      if(fabs(h1-h2)<=PI32)
         H_ = (h1+h2)/2.0f;
      else if(h1+h2<2*PI32)
         H_ = (h1+h2+2*PI32)/2.0f;
      else
         H_ = (h1+h2-2*PI32)/2.0f;
   }

   double T = 1.0f-0.17f*cos(H_-DEG2RAD(30.0f))+0.24f*cos(2.0f*H_)+0.32f*cos(3.0f*H_+DEG2RAD(6.0f))-0.2f*cos(4.0f*H_-DEG2RAD(63.0f));
   v = DEG2RAD(60.0f)*exp(-1.0f*((H_-DEG2RAD(275.0f))/DEG2RAD(25.0f))*((H_-DEG2RAD(275.0f))/DEG2RAD(25.0f)));
   double Cs_p2 = pow(Cs_,7.0f);
   double RC = 2.0f*sqrt(Cs_p2/(Cs_p2+6103515625.0f));
   double RT = -1.0f*sin(v)*RC;
   double SL = 1.0f;
   double SC = 1.0f+0.045f*Cs_;
   double SH = 1.0f+0.015f*Cs_*T;

   return (L/SL)*(L/SL)+(Cs/SC)*(Cs/SC)+(H/SH)*(H/SH)+RT*(Cs/SC)*(H_/SH);
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

//From: https://github.com/maandree/hungarian-algorithm-n3
/**
 * O(n³) implementation of the Hungarian algorithm
 * 
 * Copyright (C) 2011, 2014, 2020  Mattias Andrée
 * 
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */

static uint8_t *kuhn_match(int n, int m, double *table)
{
   uint8_t *row_covered = calloc(n,sizeof(*row_covered));
   uint8_t *col_covered = calloc(m,sizeof(*col_covered));
   int *row_primes = calloc(n,sizeof(*row_primes));
   int *col_marks = calloc(m,sizeof(*col_marks));
   uint32_t *alt = calloc(n*m,sizeof(*alt));

   kuhn_reduce_rows(n,m,table);
   uint8_t *marks = kuhn_mark(n,m,table);
   uint32_t prime = 0;

   //int i = 0;
   while(!kuhn_is_done(n,m,marks,col_covered))
   {
      while(!kuhn_find_prime(n,m,table,marks,row_covered,col_covered,&prime))
         kuhn_add_subtract(n,m,table,row_covered,col_covered);
      //puts("TAB");
      //for(int i = 0;i<m*n;i++)
         //printf("%f\n",table[i]);
      //puts("--");
      kuhn_alt_marks(n,m,marks,alt,col_marks,row_primes,&prime);
      memset(row_covered,0,sizeof(*row_covered)*n);
      memset(col_covered,0,sizeof(*col_covered)*m);
      //puts("MARK");
      //for(int i = 0;i<m*n;i++)
         //printf("%d\n",marks[i]);
      //puts("--");
      //if(i++>3)
      //break;
   }
   
   free(row_covered);
   free(col_covered);
   free(alt);
   free(row_primes);
   free(col_marks);

   uint8_t *assign = kuhn_assign(n,m,marks);
   free(marks);

   return assign;
}

static void kuhn_reduce_rows(int n, int m, double *table)
{
   for(int i = 0;i<n;i++)
   {
      double min = table[i*m];
      for(int j = 1;j<m;j++)
      {
         if(table[i*m+j]<min)
            min = table[i*m+j];
      }
      for(int j = 0;j<m;j++)
         table[i*m+j]-=min;
   }
}

static uint8_t *kuhn_mark(int n, int m, double *table)
{
   uint8_t *marks = calloc(n*m,sizeof(*marks));

   uint8_t *row_covered = calloc(n,sizeof(*row_covered));
   uint8_t *col_covered = calloc(m,sizeof(*col_covered));

   for(int i = 0;i<n;i++)
   {
      for(int j = 0;j<m;j++)
      {
         if(!row_covered[i]&&!col_covered[j]&&table[i*m+j]==0.)
         {
            marks[i*m+j] = 1;
            row_covered[i] = 1;
            col_covered[j] = 1;
         }
      }
   }

   free(row_covered);
   free(col_covered);

   return marks;
}

static int kuhn_find_prime(int n, int m, double *table, uint8_t *marks, uint8_t *row_covered,uint8_t *col_covered,uint32_t *prime)
{
   uint32_t *zeroes = HLH_bitmap_create(n*m);

   for(int i = 0;i<n;i++)
   {
      if(row_covered[i])
         continue;
      for(int j = 0;j<m;j++)
      {
         //if(col_covered[j]||table[i*m+j]!=0.)
            //continue;
         if(!col_covered[j]&&table[i*m+j]==0.)
            HLH_bitmap_set(zeroes,i*m+j);
      }
   }

   for(;;)
   {
      intptr_t p = HLH_bitmap_first_set(zeroes);
      if(p<0)
      {
         HLH_bitmap_free(zeroes);
         return 0;
      }
      //printf("Bit %d\n",p);
      
      intptr_t row = p/m;
      intptr_t col = p%m;
      marks[row*m+col] = 2;
      int mark_in_row = 0;
      for(int j = 0;j<m;j++)
      {
         if(marks[row*m+j]==1)
         {
            mark_in_row = 1;
            col = j;
         }
      }

      if(mark_in_row)
      {
			row_covered[row] = 1;
			col_covered[col] = 0;
         for(int i = 0;i<n;i++)
         {
            if(table[i*m+col]==0.&&row!=i)
            {
               if(!row_covered[i]&&!col_covered[col])
                  HLH_bitmap_set(zeroes,i*m+col);
               else
                  HLH_bitmap_unset(zeroes,i*m+col);
            }
         }

         for(int j = 0;j<m;j++)
         {
            if(table[row*m+j]==0.&&col!=j)
            {
               if(!row_covered[row]&&!col_covered[j])
                  HLH_bitmap_set(zeroes,row*m+j);
               else
                  HLH_bitmap_unset(zeroes,row*m+j);
            }
         }

			if (!row_covered[row] && !col_covered[col])
				HLH_bitmap_set(zeroes, row * m + col);
			else
				HLH_bitmap_unset(zeroes, row * m + col);
         
      }
      else
      {
         HLH_bitmap_free(zeroes);
         *prime = row*m+col;
         return 1;
      }
   }

   HLH_bitmap_free(zeroes);

   return 0;
}

static void kuhn_add_subtract(int n, int m, double *table, uint8_t *row_covered,uint8_t *col_covered)
{
   double min = 1e24;

   for(int i = 0;i<n;i++)
   {
      if(row_covered[i])
         continue;
      for(int j = 0;j<m;j++)
      {
         if(!col_covered[j]&&table[i*m+j]<min)
            min = table[i*m+j];
      }
   }

   for(int i = 0;i<n;i++)
   {
      for(int j = 0;j<m;j++)
      {
         if(row_covered[i])
            table[i*m+j]+=min;
         if(!col_covered[j])
            table[i*m+j]-=min;
      }
   }
}

static int kuhn_is_done(int n, int m, uint8_t *marks, uint8_t *covered)
{
   memset(covered,0,sizeof(*covered)*m);

   int num_done = 0;
   for(int j = 0;j<m;j++)
   {
      for(int i = 0;i<n;i++)
      {
         if(marks[i*m+j]==1)
         {
            covered[j] = 1;
            num_done++;
            break;
         }
      }
   }

   //printf("%d %d\n",num_done,n);

   return num_done==n;
}

static void kuhn_alt_marks(int n, int m, uint8_t *marks, uint32_t *alt, int *col_marks, int *row_primes, uint32_t *prime)
{
   int row,col;
   alt[0] = *prime;
   for(int i = 0;i<n;i++)
      row_primes[i] = -1;
   for(int i = 0;i<m;i++)
      col_marks[i] = -1;

   for(int i = 0;i<n;i++)
   {
      for(int j = 0;j<m;j++)
      {
         if(marks[i*m+j]==1)
            col_marks[j] = i;
         else if(marks[i*m+j]==2)
            row_primes[i] = j;
      }
   }

   int index = 0;
   while((row = col_marks[alt[index]%m])>=0)
   {
      index++;
      alt[index] = row*m+alt[index-1]%m;
      col = row_primes[alt[index]/m];
      index++;
      alt[index] = (alt[index-1]/m)*m+col;
   }

   for(int i = 0;i<=index;i++)
   {
      uint8_t *markx = &marks[alt[i]];
      //printf("Set %d\n",alt[i]);
      *markx = *markx==1?0:1;
   }

   for(int i = 0;i<n;i++)
   {
      for(int j = 0;j<m;j++)
      {
         if(marks[i*m+j]==2)
            marks[i*m+j] = 0;
      }
   }
}

static uint8_t *kuhn_assign(int n, int m, uint8_t *marks)
{
   uint8_t *assign = calloc(n,sizeof(*assign));
   for(int i = 0;i<n;i++)
   {
      for(int j = 0;j<m;j++)
      {
         if(marks[i*m+j]==1)
         {
            assign[i] = j;
         }
      }
   }

   return assign;
}
//-------------------------------------
