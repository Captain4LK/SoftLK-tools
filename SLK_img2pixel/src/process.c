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
//-------------------------------------

//Internal includes
#include "process.h"
#include "sample.h"
//-------------------------------------

//#defines
#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static const int16_t dither_threshold_normal[64] = 
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

static const int16_t dither_threshold_none[64] = {0};

static int16_t dither_threshold_tmp[64] = {0};
static const int16_t *dither_threshold = dither_threshold_none;
static Big_pixel *tmp_data = NULL;
static Big_pixel *tmp_data2 = NULL;

int brightness = 0;
int contrast = 0;
int img_gamma = 100;
int saturation = 100;
int dither_amount = 250;
int alpha_threshold = 128;
int sharpen = 0;
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

   //Setup "matrix"
   float contrast_factor = (259.0f*(255.0f+(float)contrast))/(255.0f*(259.0f-(float)contrast));
   float gamma_factor = (float)img_gamma/100.0f;
   float saturation_factor = (float)saturation/100.0f;
   float brightness_factor = (float)brightness/255.0f;

   float t = (1.0f-contrast_factor)/2.0f;
   float sr = (1.0f-saturation_factor)*0.3086f;
   float sg = (1.0f-saturation_factor)*0.6094f;
   float sb = (1.0f-saturation_factor)*0.0820f;

   float rr = contrast_factor*(sr+saturation_factor);
   float rg = contrast_factor*sr;
   float rb = contrast_factor*sr;

   float gr = contrast_factor*sg;
   float gg = contrast_factor*(sg+saturation_factor);
   float gb = contrast_factor*sg;

   float br = contrast_factor*sb;
   float bg = contrast_factor*sb;
   float bb = contrast_factor*(sb+saturation_factor);

   float wr = (t+brightness_factor)*255.0f;
   float wg = (t+brightness_factor)*255.0f;
   float wb = (t+brightness_factor)*255.0f;

   for(int y = 0;y<out->height;y++)
   {
      for(int x = 0;x<out->width;x++)
      {
         Big_pixel in = tmp_data[y*out->width+x];
         float r = (float)in.r;
         float g = (float)in.g;
         float b = (float)in.b;
         in.r = MAX(0,MIN(255,(int)(rr*r)+(gr*g)+(br*b)+wr));
         in.g = MAX(0,MIN(255,(int)(rg*r)+(gg*g)+(bg*b)+wg));
         in.b = MAX(0,MIN(255,(int)(rb*r)+(gb*g)+(bb*b)+wb));
         tmp_data[y*out->width+x] = in;

         //Gamma
         if(img_gamma!=100)
         {
            Big_pixel in = tmp_data[y*out->width+x];
            in.r = MAX(0,MIN(255,(int)(255.0f*pow((float)in.r/255.0f,gamma_factor))));
            in.g = MAX(0,MIN(255,(int)(255.0f*pow((float)in.g/255.0f,gamma_factor))));
            in.b = MAX(0,MIN(255,(int)(255.0f*pow((float)in.b/255.0f,gamma_factor))));
            tmp_data[y*out->width+x] = in;
         }
      }
   }

   dither_image(tmp_data,out,palette,process_mode,out->width,out->height);
}

void sharpen_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   if(in==NULL||out==NULL||in->width!=out->width||in->height!=out->height)
      return;

   tmp_data2 = malloc(sizeof(*tmp_data2)*out->width*out->height);
   for(int i = 0;i<out->width*out->height;i++)
   {
      tmp_data2[i].r = in->data[i].r;
      tmp_data2[i].g = in->data[i].g;
      tmp_data2[i].b = in->data[i].b;
      tmp_data2[i].a = in->data[i].a;
   }

   //Sharpen image
   float sharpen_factor = (float)sharpen/100.0f;
   float sharpen_kernel[3][3] = {
      {-1.0f*sharpen_factor,-1.0f*sharpen_factor-1.0f*sharpen_factor},
      {-1.0f*sharpen_factor,8.0f*sharpen_factor+1.0f,-1.0f*sharpen_factor},
      {-1.0f*sharpen_factor,-1.0f*sharpen_factor-1.0f*sharpen_factor},
   };
   if(sharpen!=0)
   {
      for(int y = 1;y<out->height-1;y++)
      {
         for(int x = 1;x<out->width-1;x++)
         {
            float r = 0.0f;
            float g = 0.0f;
            float b = 0.0f;
            for(int yk = -1;yk<2;yk++)
            {
               for(int xk = -1;xk<2;xk++)
               {
                  Big_pixel in = tmp_data2[(y+yk)*out->width+x+xk];
                  r+=sharpen_kernel[yk+1][xk+1]*(float)in.r;
                  g+=sharpen_kernel[yk+1][xk+1]*(float)in.g;
                  b+=sharpen_kernel[yk+1][xk+1]*(float)in.b;
               }
            }
            out->data[y*out->width+x].r = MAX(0,MIN(255,(int)r));
            out->data[y*out->width+x].g = MAX(0,MIN(255,(int)g));
            out->data[y*out->width+x].b = MAX(0,MIN(255,(int)b));
            out->data[y*out->width+x].a = in->data[y*out->width+x].a;
         }
      }
   }

   free(tmp_data2);
}

static void dither_image(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *palette, int process_mode, int width, int height)
{
   switch(process_mode)
   {
   case 0: //No dithering
      dither_threshold = dither_threshold_none;
      orderd_dither(d,out,palette,width,height);
      break;
   case 1: //Ordered dithering (positiv map)
      for(int i = 0;i<64;i++)
         dither_threshold_tmp[i] = (int)((float)dither_threshold_normal[i]*((float)dither_amount/1000.0f));
      dither_threshold = dither_threshold_tmp;
      orderd_dither(d,out,palette,width,height);
      break;
   case 2: //Ordered dithering (positiv and negativ map)
      for(int i = 0;i<64;i++)
         dither_threshold_tmp[i] = (int)((float)(dither_threshold_normal[i]-31)*((float)dither_amount/1000.0f));
      dither_threshold = dither_threshold_tmp;
      orderd_dither(d,out,palette,width,height);
      break;
   case 3: //Floyd-Steinberg dithering (per color component error)
      floyd_dither(d,out,palette,width,height);
      break;
   case 4: //Floyd-Steinberg dithering (distributed error)
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
         if(in.a<alpha_threshold)
         {
            out->data[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         uint8_t tresshold_id = ((y & 7) << 3) + (x & 7);
         Big_pixel c;
         c.r = MAX(0,MIN((in.r+dither_threshold[tresshold_id]),0xff));
         c.g = MAX(0,MIN((in.g+dither_threshold[tresshold_id]),0xff));
         c.b = MAX(0,MIN((in.b+dither_threshold[tresshold_id]),0xff));
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
         if(in.a<alpha_threshold)
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
         if(in.a<alpha_threshold)
         {
            out->data[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         
         SLK_Color p = find_closest(in,pal);
         double error = ((double)in.r-(double)p.r);
         error+=((double)in.g-(double)p.g);
         error+=((double)in.b-(double)p.b);
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

   int64_t min_dist = INT_MAX;
   int64_t min_index = 0;

   for(int64_t i = 0;i<pal->used;i++)
   {   
      int64_t dist = color_dist2(in,pal->colors[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}
//-------------------------------------
