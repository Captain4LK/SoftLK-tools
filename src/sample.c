/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <SLK/SLK.h>
#include "../external/tinyfiledialogs.h"
//-------------------------------------

//Internal includes
#include "sample.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static SLK_Color sample_noname(float x, float y, SLK_RGB_sprite *sprite);
//-------------------------------------

//Function implementations
SLK_Color sample_pixel(float x, float y, int sample_mode, SLK_RGB_sprite *sprite)
{
   SLK_Color out = SLK_color_create(0,0,0,255);
   switch(sample_mode)
   {
   case 0:
      out = sample_noname(x,y,sprite);
      break;
   }

   return out;
}

static SLK_Color sample_noname(float x, float y, SLK_RGB_sprite *sprite)
{
   double sx = x*sprite->width;   
   double sy = y*sprite->height;   

   return SLK_rgb_sprite_get_pixel(sprite,sx,sy);
}
//-------------------------------------
