/*
Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include "HLH_gui.h"
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
//-------------------------------------
