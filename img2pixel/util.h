/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _UTIL_H_

#define _UTIL_H_

SLK_image32 *image_select();
const char *palette_load_select();
const char *preset_load_select();

const char *image_save_select();
const char *palette_save_select();
const char *preset_save_select();

#endif
