/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _IMAGE2PIXEL_H_

#define _IMAGE2PIXEL_H_

extern int brightness;
extern int contrast;
extern int img_gamma;
extern int saturation;
extern int sharpen;
extern int dither_amount;
extern int alpha_threshold;
extern int gauss;
extern SLK_Palette *palette;
extern int pixel_scale_mode;
extern int pixel_sample_mode;
extern int pixel_process_mode;
extern int pixel_distance_mode;
extern int image_out_width;
extern int image_out_height;
extern int image_out_swidth;
extern int image_out_sheight;

void img2pixel_preset_load(FILE *f);
void img2pixel_preset_save(FILE *f);
void img2pixel_sharpen_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void img2pixel_lowpass_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void img2pixel_quantize(int colors, SLK_RGB_sprite *in);
void img2pixel_process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out);

#endif
