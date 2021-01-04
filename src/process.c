/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <math.h>
#include <stdint.h>
#include <limits.h>
#include <SLK/SLK.h>
#include "../external/tinyfiledialogs.h"
//-------------------------------------

//Internal includes
#include "process.h"
#include "sample.h"
//-------------------------------------

//#defines
#define MIN(a,b) (a<b?a:b)
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static const uint8_t dither_threshold_normal[64] = 
{
   //Old dither patterns
   /*0 ,48,12,60, 3,51,15,63,
   32,16,44,28,35,19,47,31,
   8 ,56, 4,52,11,59, 7,55,
   40,24,36,20,43,27,39,23, 
   2 ,50,14,62, 1,49,13,61, 
   34,18,46,30,33,17,45,29, 
   10,58, 6,54, 9,57, 5,53, 
   42,26,38,22,41,25,37,21*/
   0,32,8,40,2,34,10,42,
   48,16,56,24,50,18,58,26,
   12,44,4,36,14,46,6,38,
   60,28,52,20,62,30,54,22,
   3,35,11,43,1,33,9,41,
   51,19,59,27,49,17,57,25,
   15,47,7,39,13,45,5,37,
   63,31,55,23,61,29,53,21,
};

static const uint8_t dither_threshold_some[64] = 
{
   //Old dither patterns
   /*0 ,24, 6,30, 1,26, 7,32,
   16, 8,22,14,17,10,24,15,
   4 ,28, 2,26, 5,29, 4,28,
   20,12,18,10,21,14,19,11, 
   1 ,25, 7,31, 1,25, 7,30, 
   17, 9,23,15,16, 8,22,15, 
    5,29, 3,27, 4,28, 3,26, 
   21,13,19,11,21,12,18,10*/
   0,16,4,20,1,17,5,21,
   24,8,28,12,25,9,29,13,
   6,22,2,18,7,23,3,19,
   30,14,26,10,31,15,27,11,
   2,18,6,22,1,17,5,21,
   26,10,30,14,25,9,29,13,
   8,24,4,20,7,23,3,19,
   32,16,28,12,31,15,27,11,
};

static const uint8_t dither_threshold_little[64] = 
{
   //Old dither patterns
   /*0, 12, 3, 15, 0, 13, 3, 16,
   8, 4, 11, 7, 8, 5, 12, 8,
   2, 14, 1, 13, 2, 15, 2, 14,
   10, 6, 9, 5, 10, 7, 8, 5,
   0, 13, 3, 15, 1, 12, 3, 15,
   9, 4, 11, 8, 8, 4, 11, 7,
   2, 15, 1, 14, 2, 14, 1, 13,
   10, 7, 9, 5, 10, 6, 9, 5*/
   0,8,2,10,1,9,3,11,
   12,4,14,6,13,5,15,7,
   3,11,1,9,4,12,2,10,
   15,7,13,5,16,8,14,6,
   1,9,3,11,0,8,2,10,
   13,5,15,7,12,4,14,6,
   4,12,2,10,3,11,1,9,
   16,8,14,6,15,7,13,5,
};

static const uint8_t dither_threshold_none[64] = {0};

static const uint8_t *dither_threshold = dither_threshold_little;
static Big_pixel *tmp_data = NULL;
//-------------------------------------

//Function prototypes
static void orderd_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height);
static void floyd_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height);
static void floyd2_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height);
static void floyd_apply_error(Big_pixel *d, double error_r, double error_g, double error_b, int x, int y, int width, int height);
static SLK_Color find_closest(Big_pixel in, SLK_Palette *pal);
static int64_t color_dist2(Big_pixel c0, SLK_Color c1);
static void dither_image(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *palette, int process_mode, int width, int height);
//-------------------------------------

//Function implementations

void process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out, SLK_Palette *palette, int sample_mode, int process_mode)
{
   if(tmp_data)
      free(tmp_data);
   tmp_data = malloc(sizeof(*tmp_data)*out->width*out->height);

   sample_image(in,tmp_data,sample_mode,out->width,out->height);

   dither_image(tmp_data,out,palette,process_mode,out->width,out->height);
}

static void dither_image(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *palette, int process_mode, int width, int height)
{
   switch(process_mode)
   {
   case 0: //No dithering
      dither_threshold = dither_threshold_none;
      orderd_dither(d,out,palette,width,height);
      break;
   case 1: //Ordered dithering (level 0)
      dither_threshold = dither_threshold_little;
      orderd_dither(d,out,palette,width,height);
      break;
   case 2: //Ordered dithering (level 1)
      dither_threshold = dither_threshold_some;
      orderd_dither(d,out,palette,width,height);
      break;
   case 3: //Ordered dithering (level 2)
      dither_threshold = dither_threshold_normal;
      orderd_dither(d,out,palette,width,height);
      break;
   case 4: //Floyd-Steinberg dithering (per color error)
      floyd_dither(d,out,palette,width,height);
      break;
   case 5: //Floyd-Steinberg dithering (distributed error)
      floyd2_dither(d,out,palette,width,height);
      break;
   }
}

static void orderd_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      { 
         Big_pixel in = d[y*width+x];
         if(in.a<255)
         {
            out->data[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         uint8_t tresshold_id = ((y & 7) << 3) + (x & 7);
         Big_pixel c;
         c.r = MIN((in.r+dither_threshold[tresshold_id]),0xff);
         c.g = MIN((in.g+dither_threshold[tresshold_id]),0xff);
         c.b = MIN((in.b+dither_threshold[tresshold_id]),0xff);
         c.a = in.a;
         out->data[y*width+x] = find_closest(c,pal);
         out->data[y*width+x].a = 255;
      }
   }
}

static void floyd_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         Big_pixel in = d[y*width+x];
         if(in.a<255)
         {
            out->data[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         
         SLK_Color p = find_closest(in,pal);
         double error_r = (double)in.r-(double)p.r;
         double error_g = (double)in.g-(double)p.g;
         double error_b = (double)in.b-(double)p.b;
         floyd_apply_error(d,error_r*(7.0/16.0),error_g*(7.0/16.0),error_b*(7.0/16.0),x+1,y,width,height);
         floyd_apply_error(d,error_r*(3.0/16.0),error_g*(3.0/16.0),error_b*(3.0/16.0),x-1,y+1,width,height);
         floyd_apply_error(d,error_r*(5.0/16.0),error_g*(5.0/16.0),error_b*(5.0/16.0),x,y+1,width,height);
         floyd_apply_error(d,error_r*(1.0/16.0),error_g*(1.0/16.0),error_b*(1.0/16.0),x+1,y+1,width,height);

         out->data[y*width+x] = p;
         out->data[y*width+x].a = 255;
      }
   }
}

static void floyd2_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         Big_pixel in = d[y*width+x];
         if(in.a<255)
         {
            out->data[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         
         SLK_Color p = find_closest(in,pal);
         double error = (double)in.r-(double)p.r;
         error+=(double)in.g-(double)p.g;
         error+=(double)in.b-(double)p.b;
         error = error/3.0;
         floyd_apply_error(d,error*(7.0/16.0),error*(7.0/16.0),error*(7.0/16.0),x+1,y,width,height);
         floyd_apply_error(d,error*(3.0/16.0),error*(3.0/16.0),error*(3.0/16.0),x-1,y+1,width,height);
         floyd_apply_error(d,error*(5.0/16.0),error*(5.0/16.0),error*(5.0/16.0),x,y+1,width,height);
         floyd_apply_error(d,error*(1.0/16.0),error*(1.0/16.0),error*(1.0/16.0),x+1,y+1,width,height);

         out->data[y*width+x] = p;
         out->data[y*width+x].a = 255;
      }
   }
}

static void floyd_apply_error(Big_pixel *d, double error_r, double error_g, double error_b, int x, int y, int width, int height)
{
   if(x>width-1||x<0||y>height-1||y<0)
      return;

   Big_pixel *in = &d[y*width+x];
   in->r = in->r+error_r;
   in->g = in->g+error_g;
   in->b = in->b+error_b;
}

static int64_t color_dist2(Big_pixel c0, SLK_Color c1)
{
   int64_t diff_r = c1.r-c0.r;
   int64_t diff_g = c1.g-c0.g;
   int64_t diff_b = c1.b-c0.b;

   return (diff_r*diff_r+diff_g*diff_g+diff_b*diff_b);
}

static SLK_Color find_closest(Big_pixel in, SLK_Palette *pal)
{
   if(in.a==0)
      return pal->colors[0];

   int min_dist = INT_MAX;
   int min_index = 0;

   for(int i = 0;i<pal->used;i++)
   {   
      int dist = color_dist2(in,pal->colors[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}
//-------------------------------------
