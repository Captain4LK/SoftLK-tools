/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _SLK_IMG2PIXEL_H_

#define _SLK_IMG2PIXEL_H_

typedef struct
{
   int w;
   int h;
   uint32_t data[];
}SLK_image32;

typedef struct
{
   int w;
   int h;
   uint64_t data[];
}SLK_image64;

uint64_t SLK_color32_to_64(uint32_t c);
uint32_t SLK_color64_to_32(uint64_t c);

SLK_image32 *SLK_image32_dup(const SLK_image32 *src);
SLK_image64 *SLK_image64_dup(const SLK_image64 *src);

void SLK_image64_blur(SLK_image64 *img, float sz);
void SLK_image64_sharpen(SLK_image64 *img, float amount);
void SLK_image32_kmeans(SLK_image32 *img, uint32_t *palette, int colors, uint64_t seed);
void SLK_image64_hscb(SLK_image64 *img, float hue, float saturation, float contrast, float brightness);

#endif
