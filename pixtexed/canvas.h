/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _CANVAS_H_

#define _CANVAS_H_

#include "HLH_gui.h"
#include "project.h"
#include "gui.h"

GUI_canvas *gui_canvas_create(HLH_gui_element *parent, uint64_t flags, Project *project, Settings *settings, GUI_state *gui);
void gui_canvas_update_project(GUI_canvas *canvas, Project *project);

#endif
