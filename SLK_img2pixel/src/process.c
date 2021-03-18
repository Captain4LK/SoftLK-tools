/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <SLK/SLK.h>
//-------------------------------------

//Internal includes
#include "process.h"
#include "palette.h"
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
    0,32, 8,40, 2,34,10,42,
   48,16,56,24,50,18,58,26,
   12,44, 4,36,14,46, 6,38,
   60,28,52,20,62,30,54,22,
    3,35,11,43, 1,33, 9,41,
   51,19,59,27,49,17,57,25,
   15,47, 7,39,13,45, 5,37,
   63,31,55,23,61,29,53,21,
};
static const int16_t dither_threshold_none[64] = {0};

int brightness = 0;
int contrast = 0;
int img_gamma = 100;
int saturation = 100;
int dither_amount = 250;
int alpha_threshold = 128;
int sharpen = 0;
int gauss = 80;
//-------------------------------------

//Function prototypes
static void orderd_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, const int16_t *dither_threshold, int distance_mode);
static void floyd_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, int distance_mode);
static void floyd2_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, int distance_mode);
static void floyd_apply_error(Big_pixel *d, double error_r, double error_g, double error_b, int x, int y, int width, int height);
static void dither_image(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *palette, int process_mode, int width, int height, int distance_mode);
static double gauss_calc(double x, double y, double sigma);
static SLK_Color kernel_data_get(int x, int y, int width, int height, const SLK_RGB_sprite *data);
//-------------------------------------

//Function implementations

//"Glue" function, samples, processes and dithers the input image
void process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out, SLK_Palette *palette, int sample_mode, int process_mode, int distance_mode)
{
   Big_pixel *tmp_data = malloc(sizeof(*tmp_data)*out->width*out->height);
   if(tmp_data==NULL)
      return;

   palette_setup(palette,distance_mode);

   //Downsample image before processing it. 
   //Every image operation except kernel based ones (sharpness, gaussian blur)
   //is done after downsampling.
   sample_image(in,tmp_data,sample_mode,out->width,out->height);

   //Adjust range of values
   float gamma_factor = (float)img_gamma/100.0f;
   float contrast_factor = (259.0f*(255.0f+(float)contrast))/(255.0f*(259.0f-(float)contrast));
   float saturation_factor = (float)saturation/100.0f;
   float brightness_factor = (float)brightness/255.0f;

   //Setup "matrix"
   //saturation, brighness and contrast are implemented with a color matrix
   //(with unused parts removed).
   //See here: https://docs.rainmeter.net/tips/colormatrix-guide/
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
   //-------------------------------------

   for(int y = 0;y<out->height;y++)
   {
      for(int x = 0;x<out->width;x++)
      {
         Big_pixel in = tmp_data[y*out->width+x];

         //Saturation, brightness and contrast
         float r = (float)in.r;
         float g = (float)in.g;
         float b = (float)in.b;
         in.r = MAX(0,MIN(0xff,(int)(rr*r)+(gr*g)+(br*b)+wr));
         in.g = MAX(0,MIN(0xff,(int)(rg*r)+(gg*g)+(bg*b)+wg));
         in.b = MAX(0,MIN(0xff,(int)(rb*r)+(gb*g)+(bb*b)+wb));

         //Gamma
         //Only ajust if not the default value --> better performance
         if(img_gamma!=100)
         {
            in.r = MAX(0,MIN(0xff,(int)(255.0f*pow((float)in.r/255.0f,gamma_factor))));
            in.g = MAX(0,MIN(0xff,(int)(255.0f*pow((float)in.g/255.0f,gamma_factor))));
            in.b = MAX(0,MIN(0xff,(int)(255.0f*pow((float)in.b/255.0f,gamma_factor))));
         }

         tmp_data[y*out->width+x] = in;
      }
   }

   //Dithering is done after all image processing
   //If it was done at any other time, it would
   //resoult in different colors than the palette
   dither_image(tmp_data,out,palette,process_mode,out->width,out->height,distance_mode);

   //Clean up
   free(tmp_data);
}

//Sharpens an image, input and output dimensions must be equal
void sharpen_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   if(in==NULL||out==NULL||in->width!=out->width||in->height!=out->height)
      return;

   SLK_RGB_sprite *tmp_data2 = SLK_rgb_sprite_create(out->width,out->height);
   if(tmp_data2==NULL)
      return;
   
   SLK_rgb_sprite_copy(tmp_data2,in);

   //Setup sharpening kernel
   float sharpen_factor = (float)sharpen/100.0f;
   float sharpen_kernel[3][3] = {
      {0.0f,-1.0f*sharpen_factor,0.0f},
      {-1.0f*sharpen_factor,4.0f*sharpen_factor+1.0f,-1.0f*sharpen_factor},
      {0.0f,-1.0f*sharpen_factor,0.0f},
   };

   for(int y = 0;y<out->height;y++)
   {
      for(int x = 0;x<out->width;x++)
      {
         float r = 0.0f;
         float g = 0.0f;
         float b = 0.0f;

         //Apply kernel
         for(int yk = -1;yk<2;yk++)
         {
            for(int xk = -1;xk<2;xk++)
            {
               SLK_Color c = kernel_data_get(x+xk,y+yk,out->width,out->height,tmp_data2);

               r+=sharpen_kernel[yk+1][xk+1]*(float)c.r;
               g+=sharpen_kernel[yk+1][xk+1]*(float)c.g;
               b+=sharpen_kernel[yk+1][xk+1]*(float)c.b;
            }
         }

         out->data[y*out->width+x].r = MAX(0,MIN(0xff,(int)r));
         out->data[y*out->width+x].g = MAX(0,MIN(0xff,(int)g));
         out->data[y*out->width+x].b = MAX(0,MIN(0xff,(int)b));
         out->data[y*out->width+x].a = in->data[y*out->width+x].a;
      }
   }

   //Cleanup
   SLK_rgb_sprite_destroy(tmp_data2);
}

void lowpass_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   if(in==NULL||out==NULL||in->width!=out->width||in->height!=out->height)
      return;

   SLK_RGB_sprite *tmp_data2 = SLK_rgb_sprite_create(out->width,out->height);
   if(tmp_data2==NULL)
      return;
   
   SLK_rgb_sprite_copy(tmp_data2,in);

   //Setup lowpass kernel
   //We need to divide each cell
   //by the sum of all cells to make 
   //sure the sum of all cells will be 1
   //otherwise the blur would either darken
   //or lighten the image
   double gauss_factor = (double)gauss/100.0f;
   double lowpass_kernel[7][7];
   for(int y = 0;y<7;y++)
      for(int x = 0;x<7;x++)
         lowpass_kernel[x][y] = gauss_calc((double)x-3.0f,(double)y-3.0f,gauss_factor);
   double norm_val = 0.0f;
   for(int y = 0;y<7;y++)
      for(int x = 0;x<7;x++)
         norm_val+=lowpass_kernel[x][y];
   for(int y = 0;y<7;y++)
      for(int x = 0;x<7;x++)
         lowpass_kernel[x][y] = lowpass_kernel[x][y]/norm_val;

   for(int y = 0;y<out->height;y++)
   {
      for(int x = 0;x<out->width;x++)
      {
         double r = 0.0f;
         double g = 0.0f;
         double b = 0.0f;

         //Apply kernel
         for(int yk = -3;yk<4;yk++)
         {
            for(int xk = -3;xk<4;xk++)
            {
               SLK_Color c = kernel_data_get(x+xk,y+yk,out->width,out->height,tmp_data2);

               r+=lowpass_kernel[xk+3][yk+3]*(double)c.r;
               g+=lowpass_kernel[xk+3][yk+3]*(double)c.g;
               b+=lowpass_kernel[xk+3][yk+3]*(double)c.b;
            }
         }

         out->data[y*out->width+x].r = MAX(0,MIN(0xff,(int)r));
         out->data[y*out->width+x].g = MAX(0,MIN(0xff,(int)g));
         out->data[y*out->width+x].b = MAX(0,MIN(0xff,(int)b));
         out->data[y*out->width+x].a = in->data[y*out->width+x].a;
      }
   }

   //Cleanup
   SLK_rgb_sprite_destroy(tmp_data2);
}

//Helper function for lowpass_image
static double gauss_calc(double x, double y, double sigma)
{
   double val = (1.0f/(2.0f*M_PI*sigma*sigma))*pow(M_E,-((x*x+y*y)/(2*sigma*sigma)));
   return val;
}

//Helper function for lowpass_image and shapren_image
static SLK_Color kernel_data_get(int x, int y, int width, int height, const SLK_RGB_sprite *data)
{
   if(x<0)
      return kernel_data_get(0,y,width,height,data);
   if(y<0)
      return kernel_data_get(x,0,width,height,data);
   if(x>width-1)
      return kernel_data_get(width-1,y,width,height,data);
   if(y>height-1)
      return kernel_data_get(x,height-1,width,height,data);

   return data->data[y*width+x];
}

//Dithers an image to the provided palette using the specified mode
static void dither_image(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *palette, int process_mode, int width, int height, int distance_mode)
{
   switch(process_mode)
   {
   case 0: //No dithering
      orderd_dither(d,out,palette,width,height,dither_threshold_none,distance_mode);
      break;
   case 1: //Ordered dithering (positiv map --> bias towards lighter colors)
      {
      int16_t dither_threshold_tmp[64] = {0};
      for(int i = 0;i<64;i++)
         dither_threshold_tmp[i] = (int)((float)dither_threshold_normal[i]*((float)dither_amount/1000.0f));
      orderd_dither(d,out,palette,width,height,dither_threshold_tmp,distance_mode);
      }
      break;
   case 2: //Ordered dithering (positiv and negativ map --> no bias)
      {
      int16_t dither_threshold_tmp[64] = {0};
      for(int i = 0;i<64;i++)
         dither_threshold_tmp[i] = (int)((float)(dither_threshold_normal[i]-31)*((float)dither_amount/1000.0f));
      orderd_dither(d,out,palette,width,height,dither_threshold_tmp,distance_mode);
      }
      break;
   case 3: //Floyd-Steinberg dithering (per color component error)
      floyd_dither(d,out,palette,width,height,distance_mode);
      break;
   case 4: //Floyd-Steinberg dithering (distributed error)
      floyd2_dither(d,out,palette,width,height,distance_mode);
      break;
   }
}

//Applies ordered dithering to the input
static void orderd_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, const int16_t *dither_threshold, int distance_mode)
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

         //Add a value to the color depending on the position,
         //this creates the dithering effect
         uint8_t tresshold_id = ((y&7)<<3)+(x&7);
         Big_pixel c;
         c.r = MAX(0,MIN(0xff,(in.r+dither_threshold[tresshold_id])));
         c.g = MAX(0,MIN(0xff,(in.g+dither_threshold[tresshold_id])));
         c.b = MAX(0,MIN(0xff,(in.b+dither_threshold[tresshold_id])));
         c.a = in.a;
         out->data[y*width+x] = palette_find_closest(pal,c,distance_mode);
         out->data[y*width+x].a = 255;
      }
   }
}

//Applies Floyd-Steinberg dithering to the input
//This version uses per color component errror values,
//this usually does not work well with most palettes
static void floyd_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, int distance_mode)
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
         
         SLK_Color p = palette_find_closest(pal,in,distance_mode);
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

//Applies Floyd-Steinberg dithering to the input
//This version uses distributed error values,
//this results in better results for most palettes
static void floyd2_dither(Big_pixel *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, int distance_mode)
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
         
         SLK_Color p = palette_find_closest(pal,in,distance_mode);
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

//Helper function for floyd_dither and floyd2_dither
static void floyd_apply_error(Big_pixel *d, double error_r, double error_g, double error_b, int x, int y, int width, int height)
{
   if(x>width-1||x<0||y>height-1||y<0)
      return;

   Big_pixel *in = &d[y*width+x];
   in->r = in->r+error_r;
   in->g = in->g+error_g;
   in->b = in->b+error_b;
}
//-------------------------------------
