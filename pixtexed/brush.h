/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _BRUSH_H_

#define _BRUSH_H_

#include "HLH_gui.h"
#include "project.h"

typedef struct
{
   HLH_gui_element e;

   int brush_num;
   Project *project;
   Settings *settings;

   HLH_gui_rect icon_bounds;
   int state;
}GUI_brush;

GUI_brush *gui_brush_create(HLH_gui_element *parent, uint64_t flags, Project *project, Settings *settings, int brush_num);
void gui_brushes_update(HLH_gui_window *window, const Settings *settings);

int brush_place(Project *project, const Settings *settings, const Brush *brush, int x, int y, int layer,uint8_t color);

#endif
