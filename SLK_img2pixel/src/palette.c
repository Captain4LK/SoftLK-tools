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

//There is M_PI in math.h, but my linter complains when I use it 
//since it doesn't find the definition
#define PI 3.14159265358979323846

#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))

#define DEG2RAD(a) \
   ((a)*PI/180.0f)
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

typedef struct
{
   double h;
   double s;
   double l;
}Color_hsl;
//-------------------------------------

//Variables
static SLK_Color palette_dist0[256];
static Color_lab palette_dist1[256]; //and dist2, dist3
//-------------------------------------

//Function prototypes
static Color_lab color_to_lab(SLK_Color c);
static SLK_Color palette_find_closest_dist0(SLK_Palette *pal, Big_pixel c);
static SLK_Color palette_find_closest_dist1(SLK_Palette *pal, Big_pixel c);
static SLK_Color palette_find_closest_dist2(SLK_Palette *pal, Big_pixel c);
static SLK_Color palette_find_closest_dist3(SLK_Palette *pal, Big_pixel c);
static int64_t dist0_color_dist2(Big_pixel c0, SLK_Color c1);
static double dist1_color_dist2(Color_lab c0, Color_lab c1);
static double dist2_color_dist2(Color_lab c0, Color_lab c1);
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
   case 1: //Lab
   case 2: //Lab
   case 3: //Lab
      for(int i = 0;i<pal->used;i++)
         palette_dist1[i] = color_to_lab(pal->colors[i]);
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
   case 1: //CIE76
      out = palette_find_closest_dist1(pal,c);
      break;
   case 2: //CIE94
      out = palette_find_closest_dist2(pal,c);
      break;
   case 3: //CIEDE200
      out = palette_find_closest_dist3(pal,c);
      break;
   }

   return out;
}

//Convert to xyz then to lab color space
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
   Color_lab in = color_to_lab(cin);

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

static SLK_Color palette_find_closest_dist2(SLK_Palette *pal, Big_pixel c)
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
      double dist = dist2_color_dist2(in,palette_dist1[i]);
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
      double dist = dist3_color_dist2(in,palette_dist1[i]);
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

static double dist1_color_dist2(Color_lab c0, Color_lab c1)
{
   double diff_l = c1.l-c0.l;
   double diff_a = c1.a-c0.a;
   double diff_b = c1.b-c0.b;

   return diff_l*diff_l+diff_a*diff_a+diff_b*diff_b;
}

static double dist2_color_dist2(Color_lab c0, Color_lab c1)
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

static double dist3_color_dist2(Color_lab c0, Color_lab c1)
{
   double C1 = sqrt(c0.a*c0.a+c0.b*c0.b);
   double C2 = sqrt(c1.a*c1.a+c1.b*c1.b);
   double C_ = (C1+C2)/2.0f;

   double C_p2 = pow(C_,7.0f);
   double v = 0.5f*(1.0f-sqrt(C_p2/(C_p2+6103515625.0f)));
   double a1 = (1.0f+v)*c0.a;
   double a2 = (1.0f+v)*c1.a;

   double Cs1 = sqrt(a1*a1+c0.b*c0.b);
   double Cs2 = sqrt(a2*a2+c1.b*c1.b);

   double h1 = 0.0f;
   if(c0.b!=0||a1!=0)
   {
      h1 = atan2(c0.b,a1);
      if(h1<0)
         h1+=2.0f*PI;
   }
   double h2 = 0.0f;
   if(c1.b!=0||a2!=0)
   {
      h2 = atan2(c1.b,a2);
      if(h2<0)
         h2+=2.0f*PI;
   }

   double L = c1.l-c0.l;
   double Cs = Cs2-Cs1;
   double h = 0.0f;
   if(Cs1*Cs2!=0.0f)
   {
      h = h2-h1;
      if(h<-PI)
         h+=2*PI;
      else if(h>PI)
         h-=2*PI;
   }
   double H = 2.0f*sqrt(Cs1*Cs2)*sin(h/2.0f);

   double L_ = (c0.l+c1.l)/2.0f;
   double Cs_ = (Cs1+Cs2)/2.0f;
   double H_ = h1+h2;
   if(Cs1*Cs2!=0.0f)
   {
      if(fabs(h1-h2)<=PI)
         H_ = (h1+h2)/2.0f;
      else if(h1+h2<2*PI)
         H_ = (h1+h2+2*PI)/2.0f;
      else
         H_ = (h1+h2-2*PI)/2.0f;
   }

   double T = 1.0f-0.17f*cos(H_-DEG2RAD(30.0f))+0.24f*cos(2.0f*H_)+0.32f*cos(3.0f*H_+DEG2RAD(6.0f))-0.2f*cos(4.0f*H_-DEG2RAD(63.0f));
   v = DEG2RAD(60.0f)*exp(-1.0f*((H_-DEG2RAD(275.0f))/DEG2RAD(25.0f))*((H_-DEG2RAD(275.0f))/DEG2RAD(25.0f)));
   double Cs_p2 = pow(Cs_,7.0f);
   double RC = 2.0f*sqrt(Cs_p2/(Cs_p2+6103515625.0f));
   double RT = -1.0f*sin(v)*RC;
   double SL = 1.0f+(0.015f*(L_-50.0f)*(L_-50.0f))/sqrt(20.0f+(L_-50.0f)*(L_-50.0f));
   double SC = 1.0f+0.045f*Cs_;
   double SH = 1.0f+0.015f*Cs_*T;

   return (L/SL)*(L/SL)+(Cs/SC)*(Cs/SC)+(H/SH)*(H/SH)+RT*(Cs/SC)*(H_/SH);
}
//-------------------------------------
