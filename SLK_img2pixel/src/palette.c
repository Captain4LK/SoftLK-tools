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
#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))

#define DEG2RAD(a) \
   ((a)*M_PI/180.0f)
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
   double x;
   double y;
   double z;
}Color_xyz;

typedef struct
{
   double y;
   double cb;
   double cr;
}Color_ycc;

typedef struct
{
   double y;
   double i;
   double q;
}Color_yiq;

typedef struct
{
   double y;
   double u;
   double v;
}Color_yuv;
//-------------------------------------

//Variables
static SLK_Color palette_rgb[256];
static Color_lab palette_lab[256];
static Color_xyz palette_xyz[256];
static Color_ycc palette_ycc[256];
static Color_yiq palette_yiq[256];
static Color_yuv palette_yuv[256];
//-------------------------------------

//Function prototypes
static Color_lab color_to_lab(SLK_Color c);
static Color_xyz color_to_xyz(SLK_Color c);
static Color_ycc color_to_ycc(SLK_Color c);
static Color_yiq color_to_yiq(SLK_Color c);
static Color_yuv color_to_yuv(SLK_Color c);
static SLK_Color palette_find_closest_rgb(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_cie76(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_cie94(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_ciede2000(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_xyz(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_ycc(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_yiq(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_yuv(SLK_Palette *pal, SLK_Color c);
static int64_t rgb_color_dist2(SLK_Color c0, SLK_Color c1);
static double cie94_color_dist2(Color_lab c0, Color_lab c1);
static double ciede2000_color_dist2(Color_lab c0, Color_lab c1);
static double color_dist2(double a0, double a1, double a2, double b0, double b1, double b2);
//-------------------------------------

//Function implementations

void palette_setup(SLK_Palette *pal, int distance_mode)
{
   switch(distance_mode)
   {
   case 0: //RGB
      for(int i = 0;i<pal->used;i++)
         palette_rgb[i] = pal->colors[i];
      break;
   case 1:
   case 2:
   case 3: //Lab
      for(int i = 0;i<pal->used;i++)
         palette_lab[i] = color_to_lab(pal->colors[i]);
      break;
   case 4: //XYZ
      for(int i = 0;i<pal->used;i++)
         palette_xyz[i] = color_to_xyz(pal->colors[i]);
      break;
   case 5: //YCC
      for(int i = 0;i<pal->used;i++)
         palette_ycc[i] = color_to_ycc(pal->colors[i]);
      break;
   case 6: //YIQ
      for(int i = 0;i<pal->used;i++)
         palette_yiq[i] = color_to_yiq(pal->colors[i]);
      break;
   case 7: //YUV
      for(int i = 0;i<pal->used;i++)
         palette_yuv[i] = color_to_yuv(pal->colors[i]);
      break;
   }
}

SLK_Color palette_find_closest(SLK_Palette *pal, SLK_Color c, int distance_mode)
{
   SLK_Color out = {0};

   switch(distance_mode)
   {
   case 0: //RGB
      out = palette_find_closest_rgb(pal,c);
      break;
   case 1: //CIE76
      out = palette_find_closest_cie76(pal,c);
      break;
   case 2: //CIE94
      out = palette_find_closest_cie94(pal,c);
      break;
   case 3: //CIEDE200
      out = palette_find_closest_ciede2000(pal,c);
      break;
   case 4: //XYZ
      out = palette_find_closest_xyz(pal,c);
      break;
   case 5: //YCC
      out = palette_find_closest_ycc(pal,c);
      break;
   case 6: //YIQ
      out = palette_find_closest_yiq(pal,c);
      break;
   case 7: //YUV
      out = palette_find_closest_yuv(pal,c);
      break;
   }

   return out;
}

//Convert to xyz then to lab color space
static Color_lab color_to_lab(SLK_Color c)
{
   Color_lab l;
   Color_xyz xyz = color_to_xyz(c) ;
 
   //x component
   if(xyz.x>0.008856f)
      xyz.x = pow(xyz.x,1.0f/3.0f);
   else
      xyz.x = (7.787f*xyz.x)+(16.0f/116.0f);

   //y component
   if(xyz.y>0.008856f)
      xyz.y = pow(xyz.y,1.0f/3.0f);
   else
      xyz.y = (7.787f*xyz.y)+(16.0f/116.0f);

   //z component
   if(xyz.z>0.008856f)
      xyz.z = pow(xyz.z,1.0f/3.0f);
   else
      xyz.z = (7.787f*xyz.z)+(16.0f/116.0f);

   l.l = 116.0f*xyz.y-16.0f;
   l.a = 500.0f*(xyz.x-xyz.y);
   l.b = 200.0f*(xyz.y-xyz.z);

   return l;
}

static Color_xyz color_to_xyz(SLK_Color c)
{
   double r,g,b;
   Color_xyz x;

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

   x.x = (r*0.4124f+g*0.3576f+b*0.1805f)/95.047f;
   x.y = (r*0.2126+g*0.7152+b*0.0722)/100.0f;
   x.z = (r*0.0193+g*0.1192+b*0.9504)/108.883f;
 
   return x;
}

static Color_ycc color_to_ycc(SLK_Color c)
{
   double r = (double)c.r;
   double g = (double)c.g;
   double b = (double)c.b;
   Color_ycc y;

   y.y = 0.299f*r+0.587f*g+0.114f*b;
   y.cb = -0.16874f*r-0.33126f*g+0.5f*b;
   y.cr = 0.5f*r-0.41869f*g-0.08131f*b;

   return y;
}

static Color_yiq color_to_yiq(SLK_Color c)
{
   double r = (double)c.r;
   double g = (double)c.g;
   double b = (double)c.b;
   Color_yiq y;

   y.y = 0.2999f*r+0.587f*g+0.114f*b;
   y.i = 0.595716f*r-0.274453f*g-0.321264f*b;
   y.q = 0.211456f*r-0.522591f*g+0.31135f*b;

   return y;
}

static Color_yuv color_to_yuv(SLK_Color c)
{
   double r = (double)c.r;
   double g = (double)c.g;
   double b = (double)c.b;
   Color_yuv y;

   y.y = 0.2999f*r+0.587f*g+0.114f*b;
   y.u = 0.492f*(b-y.y);
   y.v = 0.887f*(r-y.y);

   return y;
}

static SLK_Color palette_find_closest_rgb(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   int64_t min_dist = INT64_MAX;
   int min_index = 0;

   for(int i = 0;i<pal->used;i++)
   {   
      int64_t dist = rgb_color_dist2(c,palette_rgb[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_cie76(SLK_Palette *pal, SLK_Color c)
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
      double dist = color_dist2(in.l,in.a,in.b,palette_lab[i].l,palette_lab[i].a,palette_lab[i].b);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_cie94(SLK_Palette *pal, SLK_Color c)
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
      double dist = cie94_color_dist2(in,palette_lab[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_ciede2000(SLK_Palette *pal, SLK_Color c)
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
      double dist = ciede2000_color_dist2(in,palette_lab[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_xyz(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_xyz in = color_to_xyz(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.x,in.y,in.z,palette_xyz[i].x,palette_xyz[i].y,palette_xyz[i].z);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_ycc(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_ycc in = color_to_ycc(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.y,in.cb,in.cr,palette_ycc[i].y,palette_ycc[i].cb,palette_ycc[i].cr);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_yiq(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_yiq in = color_to_yiq(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.y,in.i,in.q,palette_yiq[i].y,palette_yiq[i].i,palette_yiq[i].q);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_yuv(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_yuv in = color_to_yuv(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.y,in.u,in.v,palette_yuv[i].y,palette_yuv[i].u,palette_yuv[i].v);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static int64_t rgb_color_dist2(SLK_Color c0, SLK_Color c1)
{
   int64_t diff_r = c1.r-c0.r;
   int64_t diff_g = c1.g-c0.g;
   int64_t diff_b = c1.b-c0.b;

   return (diff_r*diff_r+diff_g*diff_g+diff_b*diff_b);
}

static double cie94_color_dist2(Color_lab c0, Color_lab c1)
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

static double ciede2000_color_dist2(Color_lab c0, Color_lab c1)
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
         h1+=2.0f*M_PI;
   }
   double h2 = 0.0f;
   if(c1.b!=0||a2!=0)
   {
      h2 = atan2(c1.b,a2);
      if(h2<0)
         h2+=2.0f*M_PI;
   }

   double L = c1.l-c0.l;
   double Cs = Cs2-Cs1;
   double h = 0.0f;
   if(Cs1*Cs2!=0.0f)
   {
      h = h2-h1;
      if(h<-M_PI)
         h+=2*M_PI;
      else if(h>M_PI)
         h-=2*M_PI;
   }
   double H = 2.0f*sqrt(Cs1*Cs2)*sin(h/2.0f);

   double L_ = (c0.l+c1.l)/2.0f;
   double Cs_ = (Cs1+Cs2)/2.0f;
   double H_ = h1+h2;
   if(Cs1*Cs2!=0.0f)
   {
      if(fabs(h1-h2)<=M_PI)
         H_ = (h1+h2)/2.0f;
      else if(h1+h2<2*M_PI)
         H_ = (h1+h2+2*M_PI)/2.0f;
      else
         H_ = (h1+h2-2*M_PI)/2.0f;
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

static double color_dist2(double a0, double a1, double a2, double b0, double b1, double b2)
{
   double diff_0 = b0-a0;
   double diff_1 = b1-a1;
   double diff_2 = b2-a2;

   return diff_0*diff_0+diff_1*diff_1+diff_2*diff_2;
}
//-------------------------------------
