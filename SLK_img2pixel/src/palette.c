/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <math.h>
#include <SLK/SLK.h>
//-------------------------------------

//Internal includes
#include "process.h"
#include "palette.h"
//-------------------------------------

//#defines
#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))
//-------------------------------------

//Typedefs
typedef struct
{
   double l;
   double a;
   double b;
}Color_lab;

typedef struct
{
   double h;
   double s;
   double v;
}Color_hsv;
//-------------------------------------

//Variables
static SLK_Color palette_dist0[256];
static Color_hsv palette_dist1[256];
static Color_lab palette_dist3[256];
//-------------------------------------

//Function prototypes
static Color_lab color_to_lab(SLK_Color c);
static Color_hsv color_to_hsv(SLK_Color c);
static SLK_Color palette_find_closest_dist0(SLK_Palette *pal, Big_pixel c);
static SLK_Color palette_find_closest_dist1(SLK_Palette *pal, Big_pixel c);
static SLK_Color palette_find_closest_dist3(SLK_Palette *pal, Big_pixel c);
static int64_t dist0_color_dist2(Big_pixel c0, SLK_Color c1);
static double dist1_color_dist2(Color_hsv c0, Color_hsv c1);
static double dist3_color_dist2(Color_lab c0, Color_lab c1);
//-------------------------------------

//Function implementations

void palette_setup(SLK_Palette *pal, int distance_mode)
{
   switch(distance_mode)
   {
   case 0: //RGB
      for(int i = 0;i<pal->used;i++)
         palette_dist0[i] = pal->colors[i];
      break;
   case 1: //HSV
      for(int i = 0;i<pal->used;i++)
         palette_dist1[i] = color_to_hsv(pal->colors[i]);
      break;
   case 3: //Lab
      for(int i = 0;i<pal->used;i++)
         palette_dist3[i] = color_to_lab(pal->colors[i]);
      break;
   }
}

SLK_Color palette_find_closest(SLK_Palette *pal, Big_pixel c, int distance_mode)
{
   SLK_Color out = {0};

   switch(distance_mode)
   {
   case 0: //RGB
      out = palette_find_closest_dist0(pal,c);
      break;
   case 1: //HSV
      out = palette_find_closest_dist1(pal,c);
      break;
   case 3: //Lab
      out = palette_find_closest_dist3(pal,c);
      break;
   }

   return out;
}

static Color_hsv color_to_hsv(SLK_Color c)
{
   Color_hsv h;
   double min,max,delta;

   min = MIN(c.r,MIN(c.g,c.b));
   max = MAX(c.r,MAX(c.g,c.b));

   h.v = max;
   delta = max-min;
   if(delta<0.00001f)
   {
      h.s = 0.0f;
      h.h = 0.0f;
      return h;
   }
   if(max>0.0f)
   {
      h.s = delta/max;
   }
   else
   {
      h.s = 0.0f;
      h.h = 0.0f;
      return h;
   }

   if(c.r>=max)
      h.h = (double)(c.g-c.b)/delta;
   else if(c.g>=max)
      h.h = 2.0f+(double)(c.b-c.r)/delta;
   else
      h.h = 4.0f+(double)(c.r-c.g)/delta;
   h.h*=60.f;
   if(h.h<0.0f)
      h.h+=360.0f;
   
   return h;
}

static Color_lab color_to_lab(SLK_Color c)
{
   double r,g,b;
   double x,y,z;
   Color_lab l;

   //red component
   r = (double)c.r/255.0f; 
   if(r>0.04045f)
      r = pow((r+0.055f)/1.055f,2.4f)*100.0f;
   else
      r = (r/12.92f)*100.0f;
  
   //green component
   g = (double)c.g/255.0f; 
   if(g>0.04045f)
      g = pow((g+0.055f)/1.055f,2.4f)*100.0f;
   else
      g = (g/12.92f)*100.0f;
  
   //blue component
   b = (double)c.b/255.0f; 
   if(b>0.04045f)
      b = pow((b+0.055f)/1.055f,2.4f)*100.0f;
   else
      b = (b/12.92f)*100.0f;

   x = (r*0.4124f+g*0.3576f+b*0.1805f)/95.047f;
   y = (r*0.2126+g*0.7152+b*0.0722)/100.0f;
   z = (r*0.0193+g*0.1192+b*0.9504)/108.883f;
 
   //x component
   if(x>0.008856f)
      x = pow(x,1.0f/3.0f);
   else
      x = (7.787f*x)+(16.0f/116.0f);

   //y component
   if(y>0.008856f)
      y = pow(y,1.0f/3.0f);
   else
      y = (7.787f*y)+(16.0f/116.0f);

   //z component
   if(z>0.008856f)
      z = pow(z,1.0f/3.0f);
   else
      z = (7.787f*z)+(16.0f/116.0f);

   l.l = 116.0f*y-16.0f;
   l.a = 500.0f*(x-y);
   l.b = 200.0f*(y-z);

   return l;
}

static SLK_Color palette_find_closest_dist0(SLK_Palette *pal, Big_pixel c)
{
   if(c.a==0)
      return pal->colors[0];

   int64_t min_dist = INT64_MAX;
   int min_index = 0;

   for(int i = 0;i<pal->used;i++)
   {   
      int64_t dist = dist0_color_dist2(c,palette_dist0[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_dist1(SLK_Palette *pal, Big_pixel c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_hsv in = color_to_hsv(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = dist1_color_dist2(in,palette_dist1[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_dist3(SLK_Palette *pal, Big_pixel c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_lab in = color_to_lab(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = dist3_color_dist2(in,palette_dist3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static int64_t dist0_color_dist2(Big_pixel c0, SLK_Color c1)
{
   int64_t diff_r = c1.r-c0.r;
   int64_t diff_g = c1.g-c0.g;
   int64_t diff_b = c1.b-c0.b;

   return (diff_r*diff_r+diff_g*diff_g+diff_b*diff_b);
}

static double dist1_color_dist2(Color_hsv c0, Color_hsv c1)
{
   double diff_h = MIN(fabs(c1.h-c0.h),360.0f-fabs(c1.h-c0.h))/180.0f;
   double diff_s = c1.s-c0.s;
   double diff_v = (c1.v-c0.v)*(1.0f/255.0f);

   return (diff_h*diff_h+diff_s*diff_s+diff_v*diff_v);
}

static double dist3_color_dist2(Color_lab c0, Color_lab c1)
{
   double L = c0.l-c1.l;
   double C1 = sqrt(c0.a*c0.a+c0.b*c0.b);
   double C2 = sqrt(c1.a*c1.a+c1.b*c1.b);
   double C = C1-C2;
   double H = sqrt((c0.a-c1.a)*(c0.a-c1.a)+(c0.b-c1.b)*(c0.b-c1.b)-C*C);
   double r1 = L;
   double r2 = C/(1.0f+0.045f*C1);
   double r3 = H/(1.0f+0.015f*C1);

   return r1*r1+r2*r2+r3*r3;
}
//-------------------------------------
