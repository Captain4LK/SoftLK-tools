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

#include "HLH.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void slk_hue(uint64_t *color, float hue);
//-------------------------------------

//Function implementations

void SLK_image64_hscb(SLK_image64 *img, float hue, float saturation, float contrast, float brightness)
{
   if(img==NULL)
      return;

   float t = (1.0f-contrast)/2.0f;

   float sr = (1.0f-saturation)*0.3086f;
   float sg = (1.0f-saturation)*0.6094f;
   float sb = (1.0f-saturation)*0.0820f;

   float rr = contrast*(sr+saturation);
   float rg = contrast*sr;
   float rb = contrast*sr;

   float gr = contrast*sg;
   float gg = contrast*(sg+saturation);
   float gb = contrast*sg;

   float br = contrast*sb;
   float bg = contrast*sb;
   float bb = contrast*(sb+saturation);

   float wr = (t+brightness)*(float)0x7fff;
   float wg = (t+brightness)*(float)0x7fff;
   float wb = (t+brightness)*(float)0x7fff;

#pragma omp parallel for
   for(int y = 0;y<img->h;y++)
   {
      for(int x = 0;x<img->w;x++)
      {
         uint64_t in = img->data[y*img->w+x];
         uint64_t a = SLK_color64_a(in);
         
         if(hue!=0.f)
            slk_hue(&in,hue);

         float fr = (float)SLK_color64_r(in);
         float fg = (float)SLK_color64_g(in);
         float fb = (float)SLK_color64_b(in);
         
         uint64_t r = HLH_max(0,HLH_min(0x7fff,(int)((rr*fr)+(gr*fg)+(br*fb)+wr)));
         uint64_t g = HLH_max(0,HLH_min(0x7fff,(int)((rg*fr)+(gg*fg)+(bg*fb)+wg)));
         uint64_t b = HLH_max(0,HLH_min(0x7fff,(int)((rb*fr)+(gb*fg)+(bb*fb)+wb)));

         img->data[y*img->w+x] = (r)|(g<<16)|(b<<32)|(a<<48);
      }
   }
}

static void slk_hue(uint64_t *color, float hue)
{
   float h = 0.f;
   float s = 0.f;
   float v = 0.f;

   {
      float r = SLK_color64_r(*color)/(float)0x7fff;
      float g = SLK_color64_g(*color)/(float)0x7fff;
      float b = SLK_color64_b(*color)/(float)0x7fff;
      float cmax = HLH_max(r,HLH_max(g,b));
      float cmin = HLH_min(r,HLH_min(g,b));
      float diff = cmax-cmin;
      
      if(cmax==cmin) h = 0.f;
      else if(cmax==r) h = fmodf(((g-b)/diff),6.f);
      else if(cmax==g) h = (b-r)/diff+2.f;
      else if(cmax==b) h = (r-g)/diff+4.f;
      h*=60.f;
      s = diff/cmax;
      v = cmax;
   }

   /*float r = (float)c.rgb.r/255.0f;
   float g = (float)c.rgb.g/255.0f;
   float b = (float)c.rgb.b/255.0f;
   float cmax = HLH_max(r,HLH_max(g,b));
   float cmin = HLH_min(r,HLH_min(g,b));
   float diff = cmax-cmin;
   Color_d3 hsv = {0};

   if(cmax==cmin)
      hsv.c0 = 0.0f;
   else if(cmax==r)
      hsv.c0 = fmod(((g-b)/diff),6.0f);
   else if(cmax==g)
      hsv.c0 = (b-r)/diff+2.0f;
   else if(cmax==b)
      hsv.c0 = (r-g)/diff+4.0f;

   hsv.c2 = cmax;
   hsv.c0*=60.0f;
   hsv.c1 = diff/hsv.c2;

   return hsv;*/

   h+=hue;

   {
      float r = 0.f;
      float g = 0.f;
      float b = 0.f;

      while(h<0.f) h+=360.f;
      while(h>360.f) h-=360.f;

      float c = v*s;
      float x = c*(1.f-fabsf(fmodf(h/60.f,2.f)-1.f));
      float m = v-c;

      if(h>=0.f&&h<60.f)
      {
         r = c+m;
         g = x+m;
         b = m;
      }
      else if(h>=60.f&&h<120.f)
      {
         r = x+m;
         g = c+m;
         b = m;
      }
      else if(h>=120.f&&h<180.f)
      {
         r = m;
         g = c+m;
         b = x+m;
      }
      else if(h>=180.f&&h<240.f)
      {
         r = m;
         g = x+m;
         b = c+m;
      }
      else if(h>=240.f&&h<300.f)
      {
         r = x+m;
         g = m;
         b = c+m;
      }
      else if(h>=300.f&&h<360.f)
      {
         r = c+m;
         g = m;
         b = x+m;
      }

      uint64_t cr = HLH_max(0,HLH_min(0x7fff,(int)(0x7fff*r)));
      uint64_t cg = HLH_max(0,HLH_min(0x7fff,(int)(0x7fff*g)));
      uint64_t cb = HLH_max(0,HLH_min(0x7fff,(int)(0x7fff*b)));
      uint64_t ca = SLK_color64_a(*color);
      *color = (cr)|(cg<<16)|(cb<<32)|(ca<<48);
   }
}
//-------------------------------------
