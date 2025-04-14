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
#include "shared/color.h"
#include "shared/image.h"
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

typedef struct
{
   int start;
   int count;
   int range_red;
   int range_green;
   int range_blue;
   int range_max;
}slk_median_box;

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
static SLK_img8and32 slk_dither_closest(Image64 *img, const SLK_dither_config *config);
static SLK_img8and32 slk_assign_median(Image64 *img, const SLK_dither_config *config);
static void slk_dither_threshold_apply(Image64 *img, int dim, const float *threshold, const SLK_dither_config *config);
static SLK_img8and32 slk_dither_floyd(Image64 *img, const SLK_dither_config *config);
static SLK_img8and32 slk_dither_floyd2(Image64 *img, const SLK_dither_config *config);
static void slk_floyd_apply_error(Image64 *img, float er, float eg, float eb, int x, int y);

static void slk_color32_to_lab(uint32_t c, float *l0, float *l1, float *l2);
static void slk_color32_to_rgb(uint32_t c, float *l0, float *l1, float *l2);

static uint8_t slk_color_closest(uint64_t c, const SLK_dither_config *config);
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

static int color_cmp_r(const void *a, const void *b);
static int color_cmp_g(const void *a, const void *b);
static int color_cmp_b(const void *a, const void *b);
//-------------------------------------

//Function implementations

SLK_img8and32 image64_dither(Image64 *img, const SLK_dither_config *config)
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
   case SLK_DITHER_MEDIAN_CUT:
      return slk_assign_median(img,config);
   }
   
   return slk_dither_closest(img,config);
}

static SLK_img8and32 slk_dither_closest(Image64 *img, const SLK_dither_config *config)
{
   Image8 *out = image8_new(img->width,img->height);
   Image32 *out32 = image32_new(img->width,img->height);
   out->color_count = config->palette_colors;
   for(int i = 0;i<config->palette_colors;i++)
      out->palette[i] = config->palette[i];

#pragma omp parallel for
   for(int y = 0;y<img->height;y++)
   {
      for(int x = 0;x<img->width;x++)
      {
         uint64_t p = img->data[y*img->width+x];
         if((int)(color64_a(p)/128)<config->alpha_threshold)
         {
            out32->data[y*img->width+x] = 0;
            out->data[y*img->width+x] = 0;
            continue;
         }

         out->data[y*img->width+x] = slk_color_closest(p,config);
         out32->data[y*img->width+x] = out->palette[out->data[y * img->width +x]];
      }
   }

   return (SLK_img8and32){out, out32};
}

static SLK_img8and32 slk_assign_median(Image64 *img, const SLK_dither_config *config)
{
   int target = HLH_max(1,HLH_min(config->target_colors,config->palette_colors));

   uint32_t *colors = calloc(2*img->width*img->height,sizeof(*colors));
   for(int i = 0;i<img->width*img->height;i++)
   {
      colors[i*2+0] = color64_to_32(img->data[i]);
      colors[i*2+1] = i;
   }
   slk_median_box *boxes = calloc(target,sizeof(*boxes));

   //Put all above alpha threshold at start
   int slow = 0;
   for(int i = 0;i<img->width*img->height;i++)
   {
      if(color32_a(colors[i*2+0])>=config->alpha_threshold)
      {
         uint32_t tmp = colors[2*i+0];
         colors[2*i+0] = colors[2*slow+0];
         colors[2*slow+0] = tmp;
         tmp = colors[2*i+1];
         colors[2*i+1] = colors[2*slow+1];
         colors[2*slow+1] = tmp;
         slow++;
      }
   }

   //Initial box
   boxes[0].start = 0;
   boxes[0].count = slow;
   int min_r = 255;
   int max_r = 0;
   int min_g = 255;
   int max_g = 0;
   int min_b = 255;
   int max_b = 0;
   for(int i = 0;i<boxes[0].count;i++)
   {
      uint32_t c = colors[2*(boxes[0].start+i)];
      min_r = HLH_min(min_r,(int)color32_r(c));
      max_r = HLH_max(max_r,(int)color32_r(c));
      min_g = HLH_min(min_g,(int)color32_g(c));
      max_g = HLH_max(max_g,(int)color32_g(c));
      min_b = HLH_min(min_b,(int)color32_b(c));
      max_b = HLH_max(max_b,(int)color32_b(c));
   }
   boxes[0].range_red = max_r-min_r;
   boxes[0].range_green = max_g-min_g;
   boxes[0].range_blue = max_b-min_b;
   boxes[0].range_max = HLH_max(HLH_max(max_r-min_r,max_g-min_g),max_b-min_b);

   int box_count;
   for(box_count = 1;box_count<target;box_count++)
   {
      //Choose box with largest range to subdivide
      int max = 0;
      int max_box = 0;
      for(int i = 0;i<box_count;i++)
      {
         if(boxes[i].range_max>max)
         {
            max = boxes[i].range_max;
            max_box = i;
         }
      }

      if(boxes[max_box].range_max==0)
         break;

      //Sort by largest range
      int largest = HLH_max(HLH_max(boxes[max_box].range_red,boxes[max_box].range_green),boxes[max_box].range_blue);
      if(largest==boxes[max_box].range_red)
         qsort(colors+2*(boxes[max_box].start),boxes[max_box].count,2*sizeof(*colors),color_cmp_r);
      else if(largest==boxes[max_box].range_green)
         qsort(colors+2*(boxes[max_box].start),boxes[max_box].count,2*sizeof(*colors),color_cmp_g);
      else if(largest==boxes[max_box].range_blue)
         qsort(colors+2*(boxes[max_box].start),boxes[max_box].count,2*sizeof(*colors),color_cmp_b);

      //Divide
      int len = 0;
      if(largest==boxes[max_box].range_red)
      {
         uint32_t cut = (color32_r(colors[2*boxes[max_box].start])+color32_r(colors[2*(boxes[max_box].start+boxes[max_box].count-1)]))/2;
         for(len = 0;;len++)
         {
            if(color32_r(colors[2*(boxes[max_box].start+len)])>cut)
               break;
         }
      }
      else if(largest==boxes[max_box].range_green)
      {
         uint32_t cut = (color32_g(colors[2*boxes[max_box].start])+color32_g(colors[2*(boxes[max_box].start+boxes[max_box].count-1)]))/2;
         for(len = 0;;len++)
         {
            if(color32_g(colors[2*(boxes[max_box].start+len)])>cut)
               break;
         }
      }
      else if(largest==boxes[max_box].range_blue)
      {
         uint32_t cut = (color32_b(colors[2*boxes[max_box].start])+color32_b(colors[2*(boxes[max_box].start+boxes[max_box].count-1)]))/2;
         for(len = 0;;len++)
         {
            if(color32_b(colors[2*(boxes[max_box].start+len)])>cut)
               break;
         }
      }
      int old_count = boxes[max_box].count;
      boxes[max_box].count = len;
      boxes[box_count].start = boxes[max_box].start+boxes[max_box].count;
      boxes[box_count].count = old_count-len;
      //printf("Divide box %d: box %d: %d; box %d: %d\n",max_box,max_box,boxes[max_box].count,box_count,boxes[box_count].count);

      //Recalculate ranges
      min_r = 255;
      max_r = 0;
      min_g = 255;
      max_g = 0;
      min_b = 255;
      max_b = 0;
      for(int i = 0;i<boxes[max_box].count;i++)
      {
         uint32_t c = colors[2*(boxes[max_box].start+i)];
         min_r = HLH_min(min_r,(int)color32_r(c));
         max_r = HLH_max(max_r,(int)color32_r(c));
         min_g = HLH_min(min_g,(int)color32_g(c));
         max_g = HLH_max(max_g,(int)color32_g(c));
         min_b = HLH_min(min_b,(int)color32_b(c));
         max_b = HLH_max(max_b,(int)color32_b(c));
      }
      boxes[max_box].range_red = max_r-min_r;
      boxes[max_box].range_green = max_g-min_g;
      boxes[max_box].range_blue = max_b-min_b;
      boxes[max_box].range_max = HLH_max(HLH_max(max_r-min_r,max_g-min_g),max_b-min_b);
      min_r = 255;
      max_r = 0;
      min_g = 255;
      max_g = 0;
      min_b = 255;
      max_b = 0;
      for(int i = 0;i<boxes[box_count].count;i++)
      {
         uint32_t c = colors[2*(boxes[box_count].start+i)];
         min_r = HLH_min(min_r,(int)color32_r(c));
         max_r = HLH_max(max_r,(int)color32_r(c));
         min_g = HLH_min(min_g,(int)color32_g(c));
         max_g = HLH_max(max_g,(int)color32_g(c));
         min_b = HLH_min(min_b,(int)color32_b(c));
         max_b = HLH_max(max_b,(int)color32_b(c));
      }
      boxes[box_count].range_red = max_r-min_r;
      boxes[box_count].range_green = max_g-min_g;
      boxes[box_count].range_blue = max_b-min_b;
      boxes[box_count].range_max = HLH_max(HLH_max(max_r-min_r,max_g-min_g),max_b-min_b);
   }

   //Calculate errors
   double *errors = calloc(config->palette_colors*box_count,sizeof(*errors));
   for(int i = 0;i<box_count;i++)
   {
      for(int j = 0;j<boxes[i].count;j++)
      {
         uint32_t c = colors[2*(boxes[i].start+j)];
         float c0,c1,c2;
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
      }
   }

   //Find best asignment
   uint8_t *assign_lowest = kuhn_match(box_count,config->palette_colors,errors);
   free(errors);
   Image8 *out = image8_new(img->width,img->height);
   Image32 *out32 = image32_new(img->width,img->height);
   out->color_count = config->palette_colors;
   for(int i = 0;i<config->palette_colors;i++)
      out->palette[i] = config->palette[i];
   for(int i = 0;i<box_count;i++)
   {
      for(int j = 0;j<boxes[i].count;j++)
      {
         uint32_t index = colors[2*(boxes[i].start+j)+1];
         uint32_t color = colors[2*(boxes[i].start+j)];
         if(color32_a(color)<config->alpha_threshold)
         {
            out->data[index] = 0;
            out32->data[index] = 0;
         }
         else
         {
            out->data[index] = assign_lowest[i];
            out32->data[index] = out->palette[out->data[index]];
         }
      }
   }

   free(colors);
   free(boxes);
   free(assign_lowest);

   return (SLK_img8and32){out, out32};
}

static void slk_dither_threshold_apply(Image64 *img, int dim, const float *threshold, const SLK_dither_config *config)
{
#pragma omp parallel for
   for(int y = 0;y<img->height;y++)
   {
      for(int x = 0;x<img->width;x++)
      {
         uint64_t p = img->data[y*img->width+x];
         uint8_t mod = (uint8_t)((1<<dim)-1);
         uint8_t threshold_id = (uint8_t)(((y&mod)<<dim)+(x&mod));
         uint64_t r = HLH_max(0,HLH_min(0x7fff,(int64_t)color64_r(p)+(int64_t)(0x7fff*(config->dither_amount/8)*(threshold[threshold_id]-0.5f))));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,(int64_t)color64_g(p)+(int64_t)(0x7fff*(config->dither_amount/8)*(threshold[threshold_id]-0.5f))));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,(int64_t)color64_b(p)+(int64_t)(0x7fff*(config->dither_amount/8)*(threshold[threshold_id]-0.5f))));
         uint64_t a = color64_a(p);
         img->data[y*img->width+x] = (r)|(g<<16)|(b<<32)|(a<<48);
      }
   }
}

static SLK_img8and32 slk_dither_floyd(Image64 *img, const SLK_dither_config *config)
{
   Image8 *out = image8_new(img->width,img->height);
   Image32 *out32 = image32_new(img->width,img->height);
   out->color_count = config->palette_colors;
   for(int i = 0;i<config->palette_colors;i++)
      out->palette[i] = config->palette[i];

   for(int y = 0;y<img->height;y++)
   {
      for(int x = 0;x<img->width;x++)
      {
         uint64_t p = img->data[y*img->width+x];
         if((int)(color64_a(p)/128)<config->alpha_threshold)
         {
            out->data[y*img->width+x] = 0;
            out32->data[y*img->width+x] = 0;
            continue;
         }

         uint8_t c = slk_color_closest(p,config);
         float error_r = (float)color32_r(color64_to_32(p))-(float)color32_r(config->palette[c]);
         float error_g = (float)color32_g(color64_to_32(p))-(float)color32_g(config->palette[c]);
         float error_b = (float)color32_b(color64_to_32(p))-(float)color32_b(config->palette[c]);
         slk_floyd_apply_error(img,(error_r*7.f)/16.f,(error_g*7.f)/16.f,(error_b*7.f)/16.f,x+1,y);
         slk_floyd_apply_error(img,(error_r*3.f)/16.f,(error_g*3.f)/16.f,(error_b*3.f)/16.f,x-1,y+1);
         slk_floyd_apply_error(img,(error_r*5.f)/16.f,(error_g*5.f)/16.f,(error_b*5.f)/16.f,x,y+1);
         slk_floyd_apply_error(img,(error_r*1.f)/16.f,(error_g*1.f)/16.f,(error_b*1.f)/16.f,x+1,y+1);

         out->data[y*img->width+x] = c;
         out32->data[y * img->width + x] = out->palette[out->data[y * img->width +x]];
      }
   }

   return (SLK_img8and32){out, out32};
}

static SLK_img8and32 slk_dither_floyd2(Image64 *img, const SLK_dither_config *config)
{
   Image8 *out = image8_new(img->width,img->height);
   Image32 *out32 = image32_new(img->width,img->height);
   out->color_count = config->palette_colors;
   for(int i = 0;i<config->palette_colors;i++)
      out->palette[i] = config->palette[i];

   for(int y = 0;y<img->height;y++)
   {
      for(int x = 0;x<img->width;x++)
      {
         uint64_t p = img->data[y*img->width+x];
         if((int)(color64_a(p)/128)<config->alpha_threshold)
         {
            out->data[y*img->width+x] = 0;
            out32->data[y*img->width+x] = 0;
            continue;
         }

         uint8_t c = slk_color_closest(p,config);
         float error = (float)color32_r(color64_to_32(p))-(float)color32_r(config->palette[c]);
         error+=(float)color32_g(color64_to_32(p))-(float)color32_g(config->palette[c]);
         error+=(float)color32_b(color64_to_32(p))-(float)color32_b(config->palette[c]);
         error/=3.f;
         slk_floyd_apply_error(img,(error*7.f)/16.f,(error*7.f)/16.f,(error*7.f)/16.f,x+1,y);
         slk_floyd_apply_error(img,(error*3.f)/16.f,(error*3.f)/16.f,(error*3.f)/16.f,x-1,y+1);
         slk_floyd_apply_error(img,(error*5.f)/16.f,(error*5.f)/16.f,(error*5.f)/16.f,x,y+1);
         slk_floyd_apply_error(img,(error*1.f)/16.f,(error*1.f)/16.f,(error*1.f)/16.f,x+1,y+1);

         out->data[y*img->width+x] = c;
         out32->data[y*img->width+x] = out->palette[out->data[y * img->width + x]];
      }
   }

   return (SLK_img8and32){out, out32};
}

static void slk_floyd_apply_error(Image64 *img, float er, float eg, float eb, int x, int y)
{
   if(x<0||x>=img->width||y<0||y>=img->height)
      return;

   uint64_t p = img->data[y*img->width+x];
   uint64_t r = HLH_max(0,HLH_min(0x7fff,(int64_t)((float)color64_r(p)+er*128.f)));
   uint64_t g = HLH_max(0,HLH_min(0x7fff,(int64_t)((float)color64_g(p)+eg*128.f)));
   uint64_t b = HLH_max(0,HLH_min(0x7fff,(int64_t)((float)color64_b(p)+eb*128.f)));
   uint64_t a = color64_a(p);

   img->data[y*img->width+x] = (r)|(g<<16)|(b<<32)|(a<<48);
}

static void slk_color32_to_rgb(uint32_t c, float *l0, float *l1, float *l2)
{
   *l0 = (float)color32_r(c)/255.f;
   *l1 = (float)color32_g(c)/255.f;
   *l2 = (float)color32_b(c)/255.f;
}

static void slk_color32_to_lab(uint32_t c, float *l0, float *l1, float *l2)
{
   //Convert to rgb
   float r = (float)color32_r(c)/255.f;
   float g = (float)color32_g(c)/255.f;
   float b = (float)color32_b(c)/255.f;

   //Convert to xyz
   //-------------------------------------
   float x = 0.f;
   float y = 0.f;
   float z = 0.f;

   if(r>0.04045)
      r = powf((r+0.055f)/1.055f,2.4f);
   else
      r = (r/12.92f);
  
   if(g>0.04045f)
      g = powf((g+0.055f)/1.055f,2.4f);
   else
      g = (g/12.92f);
  
   if(b>0.04045f)
      b = powf((b+0.055f)/1.055f,2.4f);
   else
      b = (b/12.92f);

   x = (r*0.4124f+g*0.3576f+b*0.1805f);
   y = (r*0.2126f+g*0.7152f+b*0.0722f);
   z = (r*0.0193f+g*0.1192f+b*0.9504f);
   //-------------------------------------

   //Convert to lab
   if(x>0.008856f)
      x = powf(x,1.0f/3.0f);
   else
      x = (7.787f*x)+(16.0f/116.0f);

   if(y>0.008856f)
      y = powf(y,1.0f/3.0f);
   else
      y = (7.787f*y)+(16.0f/116.0f);

   if(z>0.008856f)
      z = powf(z,1.0f/3.0f);
   else
      z = (7.787f*z)+(16.0f/116.0f);

   *l0 = 116.0f*y-16.0f;
   *l1 = 500.0f*(x-y);
   *l2 = 200.0f*(y-z);
   //-------------------------------------
}

static uint8_t slk_color_closest(uint64_t c, const SLK_dither_config *config)
{
   float min_dist = 1e12f;
   uint8_t min_index = 0;

   float c0 = 0.f;
   float c1 = 0.f;
   float c2 = 0.f;
   switch(config->color_dist)
   {
   case SLK_RGB_EUCLIDIAN:
   case SLK_RGB_WEIGHTED:
   case SLK_RGB_REDMEAN:
      slk_color32_to_rgb(color64_to_32(c),&c0,&c1,&c2);
      break;
   case SLK_LAB_CIE76:
   case SLK_LAB_CIE94:
   case SLK_LAB_CIEDE2000:
      slk_color32_to_lab(color64_to_32(c),&c0,&c1,&c2);
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
         min_index = (uint8_t)i;
      }
   }

   return min_index;
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
      dist = 1e15f;
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

   return (float)((L/SL)*(L/SL)+(Cs/SC)*(Cs/SC)+(H/SH)*(H/SH)+RT*(Cs/SC)*(H_/SH));
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

   while(!kuhn_is_done(n,m,marks,col_covered))
   {
      while(!kuhn_find_prime(n,m,table,marks,row_covered,col_covered,&prime))
         kuhn_add_subtract(n,m,table,row_covered,col_covered);
      kuhn_alt_marks(n,m,marks,alt,col_marks,row_primes,&prime);
      memset(row_covered,0,sizeof(*row_covered)*n);
      memset(col_covered,0,sizeof(*col_covered)*m);
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
         *prime = (uint32_t)(row*m+col);
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
            assign[i] = (uint8_t)j;
         }
      }
   }

   return assign;
}

static int color_cmp_r(const void *a, const void *b)
{
   uint32_t ca = *((uint32_t *)a);
   uint32_t cb = *((uint32_t *)b);
   return (int)color32_r(ca)-(int)color32_r(cb);
}

static int color_cmp_g(const void *a, const void *b)
{
   uint32_t ca = *((uint32_t *)a);
   uint32_t cb = *((uint32_t *)b);
   return (int)color32_g(ca)-(int)color32_g(cb);
}

static int color_cmp_b(const void *a, const void *b)
{
   uint32_t ca = *((uint32_t *)a);
   uint32_t cb = *((uint32_t *)b);
   return (int)color32_b(ca)-(int)color32_b(cb);
}
//-------------------------------------
