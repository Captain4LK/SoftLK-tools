/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _IMAGE_H_

#define _IMAGE_H_

#include <stdio.h>
#include <stdint.h>

typedef struct
{
   int32_t width;
   int32_t height;
   uint32_t palette[256];
   uint8_t data[];
}Image8;

typedef struct
{
   int32_t width;
   int32_t height;
   uint32_t data[];
}Image32;

typedef struct
{
   int32_t width;
   int32_t height;
   uint64_t data[];
}Image64;

Image8 *image8_new(int32_t width, int32_t height);
Image32 *image32_new(int32_t width, int32_t height);
Image64 *image64_new(int32_t width, int32_t height);

Image8 *Image8_dup(const Image8 *src);
Image32 *Image32_dup(const Image32 *src);
Image64 *Image64_dup(const Image64 *src);

int image8_save(const Image8 *img, const char *path, const char *ext);

#endif
