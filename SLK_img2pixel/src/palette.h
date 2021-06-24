/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _PALETTE_H_

#define _PALETTE_H_

void palette_setup(SLK_Palette *pal, int distance_mode);
SLK_Color palette_find_closest(SLK_Palette *pal, SLK_Color c, int distance_mode);

#endif
