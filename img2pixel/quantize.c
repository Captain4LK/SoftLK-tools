/*
Image quantization using k-means clustering

Written in 2021,2022,2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//Based on: https://github.com/ogus/kmeans-quantizer (wtfpl)

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <time.h>

#define CUTE_PNG_IMPLEMENTATION
#include "../external/cute_png.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "../external/optparse.h"

#define HLH_IMPLEMENTATION
#include "../single_header/HLH.h"
//-------------------------------------

//Internal includes
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static cp_pixel_t **cluster_list = NULL;
static cp_pixel_t *centroid_list = NULL;
static int *assignment = NULL;
static int quant_k = 16;

static struct
{
   cp_pixel_t colors[256];
   int color_count;
}palette_in = {0};

typedef uint64_t rand_xor[2];
//-------------------------------------

//Function prototypes
static void cluster_list_init();
static void cluster_list_free();
static void compute_kmeans(cp_image_t *data, int pal_in);
static void get_cluster_centroid(cp_image_t *data, int pal_in);
static cp_pixel_t colors_mean(cp_pixel_t *color_list);
static cp_pixel_t pick_random_color(cp_image_t *data);
static int nearest_color_idx(cp_pixel_t color, cp_pixel_t *color_list);
static double distance(cp_pixel_t color0, cp_pixel_t color1);
static double colors_variance(cp_pixel_t *color_list);

static cp_pixel_t *choose_centers(cp_image_t *data, int k, uint64_t seed);

static uint64_t rand_murmur3_avalanche64(uint64_t h);
static void rand_xor_seed(rand_xor *xor, uint64_t seed);
static uint64_t rand_xor_next(rand_xor *xor);

static void print_help(char **argv);
//-------------------------------------

//Function implementations

int main(int argc, char **argv)
{
   const char *path_img = NULL;
   const char *path_img_out = NULL;
   const char *path_pal = NULL;
   const char *path_pal_out = NULL;

   //Parse arguments
   struct optparse_long longopts[] =
   {
      {"pal-out", 'O', OPTPARSE_REQUIRED},
      {"img-out", 'o', OPTPARSE_REQUIRED},
      {"pal", 'p', OPTPARSE_REQUIRED},
      {"img", 'i', OPTPARSE_REQUIRED},
      {"colors", 'c', OPTPARSE_REQUIRED},
      {"new", 'n', OPTPARSE_NONE},
      {"help", 'h', OPTPARSE_NONE},
      {0},
   };
   int new = 0;
   int option;
   struct optparse options;
   optparse_init(&options, argv);
   while((option = optparse_long(&options, longopts, NULL))!=-1)
   {
      switch(option)
      {
      case 'O':
         path_pal_out = options.optarg;
         break;
      case 'o':
         path_img_out = options.optarg;
         break;
      case 'p':
         path_pal = options.optarg;
         break;
      case 'i':
         path_img = options.optarg;
         break;
      case 'h':
         print_help(argv);
         exit(EXIT_SUCCESS);
         break;
      case 'c':
         quant_k = options.optarg?atoi(options.optarg):16;
         break;
      case 'n':
         new = 1;
         break;
      case '?':
         fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
         exit(EXIT_FAILURE);
         break;
      }
   }

   if(path_img==NULL)
   {
      fprintf(stderr,"No input image specified, try %s -h for help\n",argv[0]);
      return 0;
   }

   cp_image_t img = cp_load_png(path_img);
   if(img.pix==NULL)
   {

      fprintf(stderr,"Couldn't load image '%s'\n",path_img);
      return 0;
   }

   if(new)
   {
      srand(time(NULL));
      uint8_t *asign = NULL;
      cp_pixel_t *centers = choose_centers(&img,quant_k,time(NULL));
      cp_pixel_t **clusters = malloc(sizeof(*clusters)*quant_k);
      memset(clusters,0,sizeof(*clusters)*quant_k);
      for(int i = 0;i<16;i++)
      {
         //Reset clusters
         for(int j = 0;j<quant_k;j++)
            HLH_array_length_set(clusters[j],0);

         HLH_array_length_set(asign,0);

         for(int j = 0;j<img.w*img.h;j++)
         {
            cp_pixel_t cur = img.pix[j];
            uint64_t dist_min = UINT64_MAX;
            int min_i = 0;
            for(int c = 0;c<HLH_array_length(centers);c++)
            {
               uint64_t dist = (cur.r-centers[c].r)*(cur.r-centers[c].r);
               dist+=(cur.g-centers[c].g)*(cur.g-centers[c].g);
               dist+=(cur.b-centers[c].b)*(cur.b-centers[c].b);

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
         for(int j = 0;j<quant_k;j++)
         {
            uint64_t sum_r = 0;
            uint64_t sum_g = 0;
            uint64_t sum_b = 0;
            for(int c = 0;c<HLH_array_length(clusters[j]);c++)
            {
               sum_r+=clusters[j][c].r;
               sum_g+=clusters[j][c].g;
               sum_b+=clusters[j][c].b;
            }

            if(HLH_array_length(clusters[j])>0)
            {
               centers[j].r = sum_r/HLH_array_length(clusters[j]);
               centers[j].g = sum_g/HLH_array_length(clusters[j]);
               centers[j].b = sum_b/HLH_array_length(clusters[j]);
            }
            //Choose random data point in that case
            //Not the best solution but better than not filling these colors
            //This path doesn't really seem to get hit though
            else
            {
               centers[j] = img.pix[rand()%(img.w*img.h)];
            }
         }
      }

      for(int i = 0;i<HLH_array_length(centers);i++)
         printf("%d %d %d\n",centers[i].r,centers[i].g,centers[i].b);

      for(int i = 0;i<img.w*img.h;i++)
         img.pix[i] = centers[asign[i]];
      cp_save_png(path_img_out,&img);

      HLH_array_free(centers);

      return 0;
   }

   if(path_pal!=NULL)
   {
      FILE *f = fopen(path_pal,"r");

      fscanf(f,"JASC-PAL\n0100\n%d\n",&palette_in.color_count);
      for(int i = 0;i<palette_in.color_count;i++)
      {
         fscanf(f,"%"SCNu8 "%"SCNu8 "%"SCNu8"\n",&palette_in.colors[i].r,&palette_in.colors[i].g,&palette_in.colors[i].b);
         palette_in.colors[i].a = 255;
      }

      quant_k = palette_in.color_count;

      fclose(f);
   }

   compute_kmeans(&img,path_pal!=NULL);

   for(int i = 0;i<img.w*img.h;i++)
   {
      if(path_pal!=NULL)
      {
         img.pix[i] = palette_in.colors[assignment[i]];
      }
      else
      {
         img.pix[i] = centroid_list[assignment[i]];
         img.pix[i].a = 255;
      }
   }

   if(path_img_out!=NULL)
      cp_save_png(path_img_out,&img);

   if(path_pal_out!=NULL)
   {
      FILE *f = fopen(path_pal_out,"w");

      fprintf(f,"JASC-PAL\n0100\n%d\n",quant_k);
      for(int i = 0;i<quant_k;i++)
         fprintf(f,"%d %d %d\n",centroid_list[i].r,centroid_list[i].g,centroid_list[i].b);

      fclose(f);
   }

   return 0;
}

static void cluster_list_init()
{
   cluster_list_free(quant_k);
   
   cluster_list = calloc(sizeof(*cluster_list),quant_k);
}

static void cluster_list_free()
{
   if(cluster_list==NULL)
      return;

   for(int i = 0;i<quant_k;i++)
      HLH_array_free(cluster_list[i]);

   free(cluster_list);
   cluster_list = NULL;
}

static void compute_kmeans(cp_image_t *data, int pal_in)
{
   srand(time(NULL));
   cluster_list_init();
   centroid_list = malloc(sizeof(*centroid_list)*quant_k);
   assignment = malloc(sizeof(*assignment)*(data->w*data->h));
   for(int i = 0;i<(data->w*data->h);i++)
      assignment[i] = -1;

   int iter = 0;
   int max_iter = 16;
   double *previous_variance = malloc(sizeof(*previous_variance)*quant_k);
   double variance = 0.0;
   double delta = 0.0;
   double delta_max = 0.0;
   double threshold = 0.00005f;
   for(int i = 0;i<quant_k;i++)
      previous_variance[i] = 1.0;

   for(;;)
   {
      get_cluster_centroid(data,pal_in);
      cluster_list_init();
      for(int i = 0;i<data->w*data->h;i++)
      {
         cp_pixel_t color = data->pix[i];
         assignment[i] = nearest_color_idx(color,centroid_list);
         HLH_array_push(cluster_list[assignment[i]],color);
      }

      delta_max = 0.0;
      for(int i = 0;i<quant_k;i++)
      {
         variance = colors_variance(cluster_list[i]);
         delta = fabs(previous_variance[i]-variance);
         delta_max = HLH_max(delta,delta_max);
         previous_variance[i] = variance;
      }

      if(delta_max<threshold||iter++>max_iter)
         break;
   }

   cluster_list_free();
   free(previous_variance);
}

static void get_cluster_centroid(cp_image_t *data, int pal_in)
{
   for(int i = 0;i<quant_k;i++)
   {
      if(HLH_array_length(cluster_list[i])>0)
      {
         centroid_list[i] = colors_mean(cluster_list[i]);
      }
      else
      {
         if(pal_in)
            centroid_list[i] = palette_in.colors[i];
         else
            centroid_list[i] = pick_random_color(data);
      }
   }
}

static cp_pixel_t colors_mean(cp_pixel_t *color_list)
{
   int r = 0,g = 0,b = 0;
   int length = HLH_array_length(color_list);
   for(int i = 0;i<length;i++)
   {
      r+=color_list[i].r;
      g+=color_list[i].g;
      b+=color_list[i].b;
   }

   if(length!=0)
   {
      r/=length;
      g/=length;
      b/=length;
   }

   return (cp_pixel_t){.r = r, .g = g, .b = b};
}

static cp_pixel_t pick_random_color(cp_image_t *data)
{
   return data->pix[(int)(((double)rand()/(double)RAND_MAX)*data->w*data->h)];
}

static int nearest_color_idx(cp_pixel_t color, cp_pixel_t *color_list)
{
   double dist_min = 0xfff;
   double dist = 0.0;
   int idx = 0;
   for(int i = 0;i<quant_k;i++)
   {
      dist = distance(color,color_list[i]);
      if(dist<dist_min)
      {
         dist_min = dist;
         idx = i;
      }
   }

   return idx;
}

static double distance(cp_pixel_t color0, cp_pixel_t color1)
{
   double mr = 0.5*(color0.r+color1.r),
      dr = color0.r-color1.r,
      dg = color0.g-color1.g,
      db = color0.b-color1.b;
   double distance = (2.0*dr*dr)+(4.0*dg*dg)+(3.0*db*db)+(mr*((dr*dr)-(db*db))/256.0);
   return sqrt(distance)/(3.0*255.0);
}

static double colors_variance(cp_pixel_t *color_list)
{
   int length = HLH_array_length(color_list);
   cp_pixel_t mean = colors_mean(color_list);
   double dist = 0.0;
   double dist_sum = 0.0;
   for(int i = 0;i<length;i++)
   {
      dist = distance(color_list[i],mean);
      dist_sum+=dist*dist;
   }

   return dist_sum/(double)length;
}

static cp_pixel_t *choose_centers(cp_image_t *data, int k, uint64_t seed)
{
   puts("H");
   rand_xor rng;
   rand_xor_seed(&rng,seed);

   cp_pixel_t *centers = NULL;

   //Choose initial center
   int index = rand_xor_next(&rng)%(data->w*data->h);
   HLH_array_push(centers,data->pix[index]);

   uint64_t *distance = NULL;
   HLH_array_length_set(distance,data->w*data->h);
   for(int i = 0;i<data->w*data->h;i++)
      distance[i] = UINT64_MAX;
   
   for(int i = 1;i<k;i++)
   {
      uint64_t dist_sum = 0;
      for(int j = 0;j<data->w*data->h;j++)
      {
         cp_pixel_t cur = data->pix[j];
         int center_index = HLH_array_length(centers)-1;

         uint64_t dist = (centers[center_index].r-cur.r)*(centers[center_index].r-cur.r);
         dist+=(centers[center_index].g-cur.g)*(centers[center_index].g-cur.g);
         dist+=(centers[center_index].b-cur.b)*(centers[center_index].b-cur.b);

         if(dist<distance[j])
            distance[j] = dist;
         dist_sum+=distance[j];
      }

      //Weighted random to choose next centeroid
      uint64_t random = 0;
      if(dist_sum!=0)
         rand_xor_next(&rng)%dist_sum;
      uint64_t dist_cur = 0;
      for(int j = 0;j<data->w*data->h;j++)
      {
         dist_cur+=distance[j];
         if(random<dist_cur)
         {
            HLH_array_push(centers,data->pix[j]);
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

static void print_help(char **argv)
{
   fprintf(stderr,"Usage: %s --img PATH [OPTIONS]\n"
          "   --img PATH     image file to process\n"
          "   --img-out PATH processed image\n"
          "   --pal PATH     palette to convert image to\n"
          "   --pal-out PATH generated palette\n"
          "   --colors NUM   targeted color amount\n",
         argv[0]);
}
//------------------------------------
