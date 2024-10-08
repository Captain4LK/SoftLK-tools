/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _SLK_IMG2PIXEL_H_

#define _SLK_IMG2PIXEL_H_

#include <stdio.h>

#include "shared/image.h"

/*typedef struct
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
}SLK_image64;*/

typedef enum
{
   SLK_RGB_EUCLIDIAN,
   SLK_RGB_WEIGHTED,
   SLK_RGB_REDMEAN,
   SLK_LAB_CIE76,
   SLK_LAB_CIE94,
   SLK_LAB_CIEDE2000,
}SLK_color_distance;

typedef enum
{
   SLK_DITHER_NONE,
   SLK_DITHER_BAYER8X8,
   SLK_DITHER_BAYER4X4,
   SLK_DITHER_BAYER2X2,
   SLK_DITHER_CLUSTER8X8,
   SLK_DITHER_CLUSTER4X4,
   SLK_DITHER_FLOYD,
   SLK_DITHER_FLOYD2,
   SLK_DITHER_MEDIAN_CUT,
}SLK_dither;

typedef struct
{
   SLK_color_distance color_dist;
   SLK_dither dither_mode;
   int alpha_threshold;
   uint32_t palette[256];
   int palette_colors;
   float dither_amount;

   //int use_median;
   int target_colors;
}SLK_dither_config;

uint64_t SLK_color32_to_64(uint32_t c);
uint64_t SLK_color64_r(uint64_t c);
uint64_t SLK_color64_g(uint64_t c);
uint64_t SLK_color64_b(uint64_t c);
uint64_t SLK_color64_a(uint64_t c);
uint32_t SLK_color64_to_32(uint64_t c);
uint32_t SLK_color32_r(uint32_t c);
uint32_t SLK_color32_g(uint32_t c);
uint32_t SLK_color32_b(uint32_t c);
uint32_t SLK_color32_a(uint32_t c);

//Max 256
void SLK_palette_load(FILE *f, uint32_t *colors, int *color_count, const char *ext);
void SLK_palette_save(FILE *f, uint32_t *colors, int color_count, const char *ext);

//SLK_image32 *SLK_image32_dup(const SLK_image32 *src);
//SLK_image32 *SLK_image32_dup64(const SLK_image64 *src);
//SLK_image64 *SLK_image64_dup(const SLK_image64 *src);
//SLK_image64 *SLK_image64_dup32(const SLK_image32 *src);

//void SLK_image32_write_pcx(FILE *f, const SLK_image32 *img, uint32_t *colors, int color_count);

void image64_blur(Image64 *img, float sz);
void image64_sharpen(Image64 *img, float amount);
void image32_kmeans(Image32 *img, uint32_t *palette, int colors, uint64_t seed, int kmeanspp);
uint32_t image32_kmeans_largest(Image32 *img, uint32_t *palette, int colors, uint64_t seed);
void image64_hscb(Image64 *img, float hue, float saturation, float contrast, float brightness);
void image64_gamma(Image64 *img, float gamma);
void image64_tint(Image64 *img, uint8_t tint_r, uint8_t tint_g, uint8_t tint_b);
Image32 *image32_postprocess(Image32 *img, uint32_t *color_inline, uint32_t *color_outline);
Image64 *image64_sample(const Image64 *img, int width, int height, int sample_mode, float x_off, float y_off);
//modifies input image!
Image8 *image64_dither(Image64 *img, const SLK_dither_config *config);

#endif
