/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _PROCESS_H_

#define _PROCESS_H_

extern int brightness;
extern int contrast;
extern int img_gamma;
extern int saturation;
extern int sharpen;
extern int dither_amount;
extern int alpha_threshold;
extern int gauss;

void process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out, SLK_Palette *palette, int sample_mode, int process_mode, int distance_mode);
void sharpen_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void lowpass_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out);

#endif
