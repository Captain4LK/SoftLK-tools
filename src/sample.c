/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
#include <SLK/SLK.h>
#include "../external/tinyfiledialogs.h"
//-------------------------------------

//Internal includes
#include "process.h"
#include "sample.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void sample_noname(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height);
//-------------------------------------

//Function implementations

void sample_image(const SLK_RGB_sprite *in, Big_pixel *out, int sample_mode, int width, int height)
{
   switch(sample_mode)
   {
   case 0:
   default:
      sample_noname(in,out,width,height);
      break;
   }
}

static void sample_noname(const SLK_RGB_sprite *in, Big_pixel *out, int width, int height)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double sx = ((double)x/(double)width)*in->width;   
         double sy = ((double)y/(double)height)*in->height;   
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,sx,sy);
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}
//-------------------------------------
