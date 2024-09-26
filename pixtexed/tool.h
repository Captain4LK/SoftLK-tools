/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _TOOL_H_

#define _TOOL_H_

#include <stdint.h>

#include "gui.h"

GUI_tool *gui_tool_create(HLH_gui_element *parent, uint64_t flags, GUI_canvas *canvas, Tool_type type);
void gui_tool_set(GUI_tool *t, uint8_t button);

#endif
