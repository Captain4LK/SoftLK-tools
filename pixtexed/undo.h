/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _UNDO_H_

#define _UNDO_H_

#include "project.h"
#include "gui.h"

void undo_init(Project *p);
void undo_free(Project *p);
void undo_reset(Project *p);
void undo(Project *p, const Settings *settings, GUI_state *gui);
void redo(Project *p, const Settings *settings, GUI_state *gui);

void undo_begin_layer_chunks(Project *p);
void undo_track_layer_chunk(Project *p, int x, int y, int32_t layer);
void undo_end_layer_chunks(Project *p);
void undo_track_layer_add(Project *p);
void undo_track_layer_delete(Project *p, int pos);
void undo_begin_layer_settings(Project *p);
void undo_track_layer_settings(Project *p, int32_t layer);
void undo_end_layer_settings(Project *p);
void undo_track_layer_move(Project *p, int32_t layer, int8_t direction);

#endif
