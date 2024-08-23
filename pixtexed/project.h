/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _PROJECT_H_

#define _PROJECT_H_

#include <stdint.h>
#include "layer.h"
#include "image.h"

typedef struct
{
   int32_t last_x;
   int32_t last_y;

   uint8_t button;
}Draw_state;

typedef struct
{
   int32_t width;
   int32_t height;
   uint32_t palette[256];

   uint32_t *bitmap;

   int num_layers;
   Layer **layers;

   //Internal, not saved
   Draw_state state;
}Project;

Project *project_new(int32_t width, int32_t height);
Image32 *project_to_image32(const Project *project);
void project_free(Project *project);

#endif
