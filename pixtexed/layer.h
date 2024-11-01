/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _LAYER_H_

#define _LAYER_H_

#include <stddef.h>
#include "shared/image.h"
#include "gui.h"


Layer *layer_new(size_t size);
void layer_free(Layer *layer);
void layer_copy(Layer *dst, const Layer *src, size_t size);

//Call after updating
//Call both after changing layer type
//Call update_settings after changing palette
void layer_update_settings(Project *project, Layer *layer, const Settings *settings);
void layer_update_contents(Layer *layer);

GUI_layer *gui_layer_create(HLH_gui_element *parent, uint64_t flags, GUI_canvas *canvas, int layer_num);
void gui_layer_set(GUI_layer *l, uint8_t button);

#endif
