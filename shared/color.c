/*
Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include "HLH_gui.h"
#include "HLH.h"
//-------------------------------------

//Internal includes
#include "shared/color.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

uint64_t color32_to_64(uint32_t c)
{
   uint64_t r = color32_r(c)<<7;
   uint64_t g = color32_g(c)<<7;
   uint64_t b = color32_b(c)<<7;
   uint64_t a = color32_a(c)<<7;

   return (r)|(g<<16)|(b<<32)|(a<<48);
}

uint32_t color64_to_32(uint64_t c)
{
   uint32_t r = (uint32_t)(color64_r(c)>>7);
   uint32_t g = (uint32_t)(color64_g(c)>>7);
   uint32_t b = (uint32_t)(color64_b(c)>>7);
   uint32_t a = (uint32_t)(color64_a(c)>>7);

   return (r)|(g<<8)|(b<<16)|(a<<24);
}

uint64_t color64_r(uint64_t c)
{
   return c&0x7fff;
}

uint64_t color64_g(uint64_t c)
{
   return (c>>16)&0x7fff;
}

uint64_t color64_b(uint64_t c)
{
   return (c>>32)&0x7fff;
}

uint64_t color64_a(uint64_t c)
{
   return (c>>48)&0x7fff;
}

uint32_t color32_r(uint32_t c)
{
   return c&255;
}

uint32_t color32_g(uint32_t c)
{
   return (c>>8)&255;
}

uint32_t color32_b(uint32_t c)
{
   return (c>>16)&255;
}

uint32_t color32_a(uint32_t c)
{
   return (c>>24)&255;
}

colorf lsrgb_from_color32(uint32_t c)
{
   float c0 = (float)color32_r(c)/255.f;
   float c1 = (float)color32_g(c)/255.f;
   float c2 = (float)color32_b(c)/255.f;
   float c3 = (float)color32_a(c)/255.f;

   if(c0>=0.0031308f)
      c0 = 1.055f*powf(c0,1.f/2.4f)-0.055f;
   else
      c0 = 12.92f*c0;

   if(c1>=0.0031308f)
      c1 = 1.055f*powf(c1,1.f/2.4f)-0.055f;
   else
      c1 = 12.92f*c1;

   if(c2>=0.0031308f)
      c2 = 1.055f*powf(c2,1.f/2.4f)-0.055f;
   else
      c2 = 12.92f*c2;

   return (colorf){{c0,c1,c2,c3}};
}

uint32_t color32_from_lsrgb(colorf lsrgb)
{
   float c0 = lsrgb.c[0];
   float c1 = lsrgb.c[1];
   float c2 = lsrgb.c[2];
   float c3 = lsrgb.c[3];

   if(c0>=0.04045f)
      c0 = powf((c0+0.055f)/(1.055f),2.4f);
   else
      c0 = c0/12.92f;

   if(c1>=0.04045f)
      c1 = powf((c1+0.055f)/(1.055f),2.4f);
   else
      c1 = c1/12.92f;

   if(c2>=0.04045f)
      c2 = powf((c2+0.055f)/(1.055f),2.4f);
   else
      c2 = c2/12.92f;

   uint8_t r = (uint8_t)HLH_min(255,HLH_max(0,(int)(c0*255.f)));
   uint8_t g = (uint8_t)HLH_min(255,HLH_max(0,(int)(c1*255.f)));
   uint8_t b = (uint8_t)HLH_min(255,HLH_max(0,(int)(c2*255.f)));
   uint8_t a = (uint8_t)HLH_min(255,HLH_max(0,(int)(c3*255.f)));
   return color32(r,g,b,a);
}

colorf oklab_from_lsrgb(colorf lsrgb)
{
   float l = 0.4122214708f * lsrgb.c[0] + 0.5363325363f * lsrgb.c[1] + 0.0514459929f * lsrgb.c[2];
   float m = 0.2119034982f * lsrgb.c[0] + 0.6806995451f * lsrgb.c[1] + 0.1073969566f * lsrgb.c[2];
   float s = 0.0883024619f * lsrgb.c[0] + 0.2817188376f * lsrgb.c[1] + 0.6299787005f * lsrgb.c[2];
   l = cbrtf(l);
   m = cbrtf(m);
   s = cbrtf(s);

   float c0 = 0.2104542553f*l + 0.7936177850f*m - 0.0040720468f*s;
   float c1 = 1.9779984951f*l - 2.4285922050f*m + 0.4505937099f*s;
   float c2 = 0.0259040371f*l + 0.7827717662f*m - 0.8086757660f*s;

   return (colorf){{c0,c1,c2,lsrgb.c[3]}};
}

colorf lsrgb_from_oklab(colorf oklab)
{
   float l = oklab.c[0] + 0.3963377774f * oklab.c[1] + 0.2158037573f * oklab.c[2];
   float m = oklab.c[0] - 0.1055613458f * oklab.c[1] - 0.0638541728f * oklab.c[2];
   float s = oklab.c[0] - 0.0894841775f * oklab.c[1] - 1.2914855480f * oklab.c[2];
   l = l*l*l;
   m = m*m*m;
   s = s*s*s;

   float c0 = 4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
   float c1 = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
   float c2 = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

   return (colorf){{c0,c1,c2,oklab.c[3]}};
}
//-------------------------------------
