/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <limits.h>
#include <SLK/SLK.h>
#include "../external/tinyfiledialogs.h"
//-------------------------------------

//Internal includes
#include "process.h"
//-------------------------------------

//#defines
#define MIN(a,b) (a<b?a:b)
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static const uint8_t dither_threshold_normal[64] = 
{
   0 ,48,12,60, 3,51,15,63,
   32,16,44,28,35,19,47,31,
   8 ,56, 4,52,11,59, 7,55,
   40,24,36,20,43,27,39,23, 
   2 ,50,14,62, 1,49,13,61, 
   34,18,46,30,33,17,45,29, 
   10,58, 6,54, 9,57, 5,53, 
   42,26,38,22,41,25,37,21
};

static const uint8_t dither_threshold_some[64] = 
{
   0 ,24, 6,30, 1,26, 7,32,
   16, 8,22,14,17,10,24,15,
   4 ,28, 2,26, 5,29, 4,28,
   20,12,18,10,21,14,19,11, 
   1 ,25, 7,31, 1,25, 7,30, 
   17, 9,23,15,16, 8,22,15, 
    5,29, 3,27, 4,28, 3,26, 
   21,13,19,11,21,12,18,10
};

static const uint8_t dither_threshold_little[64] = 
{
   0, 12, 3, 15, 0, 13, 3, 16,
   8, 4, 11, 7, 8, 5, 12, 8,
   2, 14, 1, 13, 2, 15, 2, 14,
   10, 6, 9, 5, 10, 7, 8, 5,
   0, 13, 3, 15, 1, 12, 3, 15,
   9, 4, 11, 8, 8, 4, 11, 7,
   2, 15, 1, 14, 2, 14, 1, 13,
   10, 7, 9, 5, 10, 6, 9, 5
};
static const uint8_t *dither_threshold = dither_threshold_little;
//-------------------------------------

//Function prototypes
static SLK_Color orderd_dither(int x, int y, SLK_Color in, SLK_Palette *pal);
static SLK_Color find_closest(SLK_Color in, SLK_Palette *pal);
static int color_dist2(SLK_Color c0, SLK_Color c1);
//-------------------------------------

//Function implementations

SLK_Color process_pixel(int x, int y, int process_mode, SLK_Color in, SLK_Palette *pal)
{
   SLK_Color out = SLK_color_create(0,0,0,255);
   if(pal==NULL)
      return out;
   switch(process_mode)
   {
   case 0: //No dithering
      out = find_closest(in,pal);
      break;
   case 1: //Ordered dithering (level 0)
      dither_threshold = dither_threshold_little;
      out = orderd_dither(x,y,in,pal);
      break;
   case 2: //Ordered dithering (level 1)
      dither_threshold = dither_threshold_some;
      out = orderd_dither(x,y,in,pal);
      break;
   case 3: //Ordered dithering (level 2)
      dither_threshold = dither_threshold_normal;
      out = orderd_dither(x,y,in,pal);
      break;
   }

   return out;
}

static SLK_Color orderd_dither(int x, int y, SLK_Color in, SLK_Palette *pal)
{
   uint8_t tresshold_id = ((y & 7) << 3) + (x & 7);
   SLK_Color out; 
   out.a = in.a;

   SLK_Color c;
   c.r = MIN((in.r+dither_threshold[tresshold_id]),0xff);
   c.g = MIN((in.g+dither_threshold[tresshold_id]),0xff);
   c.b = MIN((in.b+dither_threshold[tresshold_id]),0xff);
   c.a = in.a;
   out = find_closest(c,pal);

   return out;
}

static int color_dist2(SLK_Color c0, SLK_Color c1)
{
   int diff_r = c1.r-c0.r;
   int diff_g = c1.g-c0.g;
   int diff_b = c1.b-c0.b;

   return (diff_r*diff_r+diff_g*diff_g+diff_b*diff_b);
}

static SLK_Color find_closest(SLK_Color in, SLK_Palette *pal)
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
