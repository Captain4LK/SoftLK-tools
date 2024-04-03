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
static SLK_image32 *slk_dither_none(SLK_image64 *img, const SLK_dither_config *config);
static SLK_image32 *slk_dither_floyd(SLK_image64 *img, const SLK_dither_config *config);
static SLK_image32 *slk_dither_floyd2(SLK_image64 *img, const SLK_dither_config *config);
static void slk_floyd_apply_error(SLK_image64 *img, float er, float eg, float eb, int x, int y);

static slk_color3f *choose_centers(const SLK_dither_config *config);

static void slk_color32_to_lab(uint32_t c, float *l0, float *l1, float *l2);
static void slk_color32_to_rgb(uint32_t c, float *l0, float *l1, float *l2);

static uint32_t slk_color_closest(uint64_t c, const SLK_dither_config *config);
static float slk_dist_rgb_euclidian(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_rgb_weighted(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_rgb_redmean(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_cie76(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_cie94(float a0, float a1, float a2, float b0, float b1, float b2);
static float slk_dist_ciede2000(float b0, float b1, float b2, float c0, float c1, float c2);
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

   switch(config->dither_mode)
   {
   case SLK_DITHER_BAYER8X8: slk_dither_threshold_apply(img,3,slk_dither_threshold_bayer8x8,config); break;
   case SLK_DITHER_BAYER4X4: slk_dither_threshold_apply(img,2,slk_dither_threshold_bayer4x4,config); break;
   case SLK_DITHER_BAYER2X2: slk_dither_threshold_apply(img,1,slk_dither_threshold_bayer2x2,config); break;
   case SLK_DITHER_CLUSTER8X8: slk_dither_threshold_apply(img,3,slk_dither_threshold_cluster8x8,config); break;
   case SLK_DITHER_CLUSTER4X4: slk_dither_threshold_apply(img,2,slk_dither_threshold_cluster4x4,config); break;
   default: break;
   }

   if(config->use_kmeans)
   {
      return slk_dither_kmeans(img,config);
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

   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         if(SLK_color64_a(p)/128<config->alpha_threshold)
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
   uint8_t *asign = NULL;
   slk_color3f *centers = choose_centers(config);
   slk_color3f **clusters = malloc(sizeof(*clusters)*config->palette_colors);
   memset(clusters,0,sizeof(*clusters)*config->palette_colors);

   for(int iter = 0;iter<16;iter++)
   {
      //Reset clusters
      for(int j = 0;j<config->palette_colors;j++)
         HLH_array_length_set(clusters[j],0);

      HLH_array_length_set(asign,0);

      for(int j = 0;j<img->w*img->h;j++)
      {
         uint64_t p = img->data[j];
         float min_dist = 1e20;
         int min_index = 0;

         float c0 = 0.f;
         float c1 = 0.f;
         float c2 = 0.f;
         switch(config->color_dist)
         {
         case SLK_RGB_EUCLIDIAN:
         case SLK_RGB_WEIGHTED:
         case SLK_RGB_REDMEAN:
            slk_color32_to_rgb(SLK_color64_to_32(p),&c0,&c1,&c2);
            break;
         case SLK_LAB_CIE76:
         case SLK_LAB_CIE94:
         case SLK_LAB_CIEDE2000:
            slk_color32_to_lab(SLK_color64_to_32(p),&c0,&c1,&c2);
            break;
         }

         for(int i = 0;i<config->palette_colors;i++)
         {
            float dist = 0.f;

            switch(config->color_dist)
            {
            case SLK_RGB_EUCLIDIAN:
               dist = slk_dist_rgb_euclidian(c0,c1,c2, centers[i].c0,centers[i].c1,centers[i].c2);
               break;
            case SLK_RGB_WEIGHTED:
               dist = slk_dist_rgb_weighted(c0,c1,c2, centers[i].c0,centers[i].c1,centers[i].c2);
               break;
            case SLK_RGB_REDMEAN:
               dist = slk_dist_rgb_redmean(c0,c1,c2, centers[i].c0,centers[i].c1,centers[i].c2);
               break;
            case SLK_LAB_CIE76:
               dist = slk_dist_cie76(c0,c1,c2, centers[i].c0,centers[i].c1,centers[i].c2);
               break;
            case SLK_LAB_CIE94:
               dist = slk_dist_cie94(c0,c1,c2, centers[i].c0,centers[i].c1,centers[i].c2);
               break;
            case SLK_LAB_CIEDE2000:
               dist = slk_dist_ciede2000(c0,c1,c2, centers[i].c0,centers[i].c1,centers[i].c2);
               break;
            }

            if(dist<min_dist)
            {
               min_dist = dist;
               min_index = i;
            }
         }

         slk_color3f color = {0};
         color.c0 = c0;
         color.c1 = c1;
         color.c2 = c2;
         HLH_array_push(clusters[min_index],color);
         HLH_array_push(asign,min_index);
      }

      //Recalculate centers
      for(int j = 0;j<config->palette_colors;j++)
      {
         double sum_c0 = 0.;
         double sum_c1 = 0.;
         double sum_c2 = 0.;

         double length = (double)HLH_array_length(clusters[j]);
         for(int c = 0;c<HLH_array_length(clusters[j]);c++)
         {
            sum_c0+=clusters[j][c].c0;
            sum_c1+=clusters[j][c].c1;
            sum_c2+=clusters[j][c].c2;
         }

         double weight = 0.;
         if(config->palette_weight>=0)
            weight = (double)length/(1<<config->palette_weight);
         length+=weight;
         sum_c0+=slk_palette[j][0]*weight;
         sum_c1+=slk_palette[j][1]*weight;
         sum_c2+=slk_palette[j][2]*weight;

         if(length!=0.)
         {
            sum_c0/=length;
            sum_c1/=length;
            sum_c2/=length;
         }

         slk_color3f color = {0};
         color.c0 = sum_c0;
         color.c1 = sum_c1;
         color.c2 = sum_c2;
         centers[j] = color;
      }
   }

   HLH_array_free(centers);
   for(int i = 0;i<config->palette_colors;i++)
      HLH_array_free(clusters[i]);
   free(clusters);

   SLK_image32 *out = SLK_image32_dup64(img);
   for(int i = 0;i<img->w*img->h;i++)
      out->data[i] = config->palette[asign[i]];

   HLH_array_free(asign);

   return out;
}

static SLK_image32 *slk_dither_none(SLK_image64 *img, const SLK_dither_config *config)
{
   SLK_image32 *out = SLK_image32_dup64(img);

   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         if(SLK_color64_a(p)/128<config->alpha_threshold)
         {
            out->data[y*img->w+x] = 0;
            continue;
         }

         out->data[y*img->w+x] = slk_color_closest(p,config);
      }
   }

   return out;
}

static void slk_dither_threshold_apply(SLK_image64 *img, int dim, const float *threshold, const SLK_dither_config *config)
{
   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t p = img->data[y*img->w+x];
         uint8_t mod = (1<<dim)-1;
         uint8_t threshold_id = ((y&mod)<<dim)+(x&mod);
         uint64_t r = HLH_max(0,HLH_min(0x7fff,SLK_color64_r(p)+0x7fff*config->dither_amount*(threshold[threshold_id]-0.5f)));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,SLK_color64_g(p)+0x7fff*config->dither_amount*(threshold[threshold_id]-0.5f)));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,SLK_color64_b(p)+0x7fff*config->dither_amount*(threshold[threshold_id]-0.5f)));
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
         if(SLK_color64_a(p)/128<config->alpha_threshold)
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
         if(SLK_color64_a(p)/128<config->alpha_threshold)
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
      r = powf((r+0.055)/1.055,2.4)*100.0;
   else
      r = (r/12.92)*100.0;
  
   if(g>0.04045)
      g = powf((g+0.055)/1.055,2.4)*100.0;
   else
      g = (g/12.92)*100.0;
  
   if(b>0.04045)
      b = powf((b+0.055)/1.055,2.4)*100.0;
   else
      b = (b/12.92)*100.0;

   x = (r*0.4124+g*0.3576+b*0.1805)/95.05;
   y = (r*0.2126+g*0.7152+b*0.0722)/100.0;
   z = (r*0.0193+g*0.1192+b*0.9504)/108.89;
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

   return r1*r1+r2*r2+r3*r3;
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
//-------------------------------------
