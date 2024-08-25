/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _LAYER_H_

#define _LAYER_H_

#include <stddef.h>
#include "image.h"

typedef struct
{
   int tmp;
   uint8_t data[];
}Layer;

Layer *layer_new(size_t size);
void layer_free(Layer *layer);
void layer_copy(Layer *dst, const Layer *src, size_t size);

#endif
