/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021,2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _UTILITY_H_

#define _UTILITY_H_

extern int win_preview_width;
extern int win_preview_height;
extern int gui_scale;

SLK_RGB_sprite *image_select();
void image_write(I2P_state *s, SLK_RGB_sprite *img, SLK_Palette *pal);
FILE *json_select();
FILE *json_write();
SLK_Palette *palette_select();
void palette_write(SLK_Palette *pal);

SLK_RGB_sprite *image_load(const char *path);
SLK_Palette *palette_load(const char *path);
void image_save(I2P_state *s, const char *path, SLK_RGB_sprite *img, SLK_Palette *pal);
void dir_input_select();
void dir_output_select(I2P_state *s, int dither_mode, int sample_mode, int distance_mode, int scale_mode, int width, int height, SLK_Palette *pal);

void utility_init();
void utility_exit();

#endif
