/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
uint16_t SLK_color64_r(uint64_t c);
uint16_t SLK_color64_g(uint64_t c);
uint16_t SLK_color64_b(uint64_t c);
uint16_t SLK_color64_a(uint64_t c);
uint32_t SLK_color64_to_32(uint64_t c);
uint8_t SLK_color32_r(uint32_t c);
uint8_t SLK_color32_g(uint32_t c);
uint8_t SLK_color32_b(uint32_t c);
uint8_t SLK_color32_a(uint32_t c);

SLK_image32 *SLK_image32_dup(const SLK_image32 *src);
SLK_image32 *SLK_image32_dup64(const SLK_image64 *src);
SLK_image64 *SLK_image64_dup(const SLK_image64 *src);
SLK_image64 *SLK_image64_dup32(const SLK_image32 *src);

void SLK_image64_blur(SLK_image64 *img, float sz);
void SLK_image64_sharpen(SLK_image64 *img, float amount);
void SLK_image32_kmeans(SLK_image32 *img, uint32_t *palette, int colors, uint64_t seed);
void SLK_image64_hscb(SLK_image64 *img, float hue, float saturation, float contrast, float brightness);
void SLK_image64_gamma(SLK_image64 *img, float gamma);
SLK_image32 *SLK_image64_dither(SLK_image64 *img, int dither_mode, int distance_mode, int alpha_threshold);
SLK_image64 *SLK_image64_sample(const SLK_image64 *img, int width, int height, int sample_mode, float x_off, float y_off);

#endif
