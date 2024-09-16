/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _UTIL_H_

#define _UTIL_H_

//Close with fclose() when done
FILE *image_load_select();
FILE *palette_load_select(char ext[512]);
FILE *preset_load_select();
void dir_input_select(char path[512]);
void dir_output_select(char path[512]);

//Close with fclose() when done
FILE *image_save_select(char ext[512]);
FILE *palette_save_select(char ext[512]);
FILE *preset_save_select();

void settings_load(const char *path);
void settings_save();

#endif
