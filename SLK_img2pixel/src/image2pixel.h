/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _IMAGE2PIXEL_H_

#define _IMAGE2PIXEL_H_

void         img2pixel_preset_load(FILE *f);
void         img2pixel_preset_save(FILE *f);
void         img2pixel_sharpen_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void         img2pixel_lowpass_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void         img2pixel_quantize(int colors, SLK_RGB_sprite *in);
void         img2pixel_process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void         img2pixel_reset_to_defaults();

int          img2pixel_get_brightness();
void         img2pixel_set_brightness(int nbrightness);
int          img2pixel_get_contrast();
void         img2pixel_set_contrast(int ncontrast);
int          img2pixel_get_gamma();
void         img2pixel_set_gamma(int ngamma);
int          img2pixel_get_saturation();
void         img2pixel_set_saturation(int nsaturation);
int          img2pixel_get_sharpen();
void         img2pixel_set_sharpen(int nsharpen);
int          img2pixel_get_hue();
void         img2pixel_set_hue(int nhue);
int          img2pixel_get_dither_amount();
void         img2pixel_set_dither_amount(int namount);
int          img2pixel_get_alpha_threshold();
void         img2pixel_set_alpha_threshold(int nthreshold);
int          img2pixel_get_gauss();
void         img2pixel_set_gauss(int ngauss);
SLK_Palette *img2pixel_get_palette();
void         img2pixel_set_palette(SLK_Palette *npalette); //freeing is supposed to be handled by user!
int          img2pixel_get_scale_mode();
void         img2pixel_set_scale_mode(int nmode);
int          img2pixel_get_sample_mode();
void         img2pixel_set_sample_mode(int nmode);
int          img2pixel_get_process_mode();
void         img2pixel_set_process_mode(int nmode);
int          img2pixel_get_distance_mode();
void         img2pixel_set_distance_mode(int nmode);
int          img2pixel_get_out_width();
void         img2pixel_set_out_width(int nwidth);
int          img2pixel_get_out_height();
void         img2pixel_set_out_height(int nheight);
int          img2pixel_get_out_swidth();
void         img2pixel_set_out_swidth(int nwidth);
int          img2pixel_get_out_sheight();
void         img2pixel_set_out_sheight(int nheight);

#endif
