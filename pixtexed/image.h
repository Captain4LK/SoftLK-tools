/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _IMAGE_H_

#define _IMAGE_H_

#include <stdint.h>

typedef struct
{
   int32_t width;
   int32_t height;
   uint32_t palette[256];
   uint8_t data[];
}PTD_image8;

typedef struct
{
   int32_t width;
   int32_t height;
   uint32_t data[];
}PTD_image32;

typedef struct
{
   int32_t width;
   int32_t height;
   uint64_t data[];
}PTD_image64;

PTD_image8 *PTD_image8_dup(const PTD_image8 *src);
PTD_image32 *PTD_image32_dup(const PTD_image32 *src);
//PTD_image32 *PTD_image32_dup64(const PTD_image64 *src);
PTD_image64 *PTD_image64_dup(const PTD_image64 *src);
//PTD_image64 *PTD_image64_dup32(const PTD_image32 *src);

#endif
