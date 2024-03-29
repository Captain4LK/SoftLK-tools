/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
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
//-------------------------------------

//Function implementations
uint64_t SLK_color32_to_64(uint32_t c)
{
   uint64_t color = c;

   color = ((color&0xff000000)<<24) 
         | ((color&0x00ff0000)<<16) 
         | ((color&0x0000ff00)<< 8) 
         | ((color&0x000000ff)    );
   color = ((color | (color<<8))>>1)&0x7fff7fff7fff7fffULL;

   return color;
}

uint16_t SLK_color64_r(uint64_t c)
{
   return c&0x7fff;
}

uint16_t SLK_color64_g(uint64_t c)
{
   return (c>>16)&0x7fff;
}

uint16_t SLK_color64_b(uint64_t c)
{
   return (c>>32)&0x7fff;
}

uint16_t SLK_color64_a(uint64_t c)
{
   return (c>>48)&0x7fff;
}

uint32_t SLK_color64_to_32(uint64_t c)
{
   uint32_t c0 = (c>>7)&255;
   uint32_t c1 = (c>>23)&255;
   uint32_t c2 = (c>>39)&255;
   uint32_t c3 = (c>>55)&255;

   return c0|(c1<<8)|(c2<<16)|(c3<<24);
}

uint8_t SLK_color32_r(uint32_t c)
{
   return c&255;
}

uint8_t SLK_color32_g(uint32_t c)
{
   return (c>>8)&255;
}

uint8_t SLK_color32_b(uint32_t c)
{
   return (c>>16)&255;
}

uint8_t SLK_color32_a(uint32_t c)
{
   return (c>>24)&255;
}
//-------------------------------------
