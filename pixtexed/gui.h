/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _GUI_H_

#define _GUI_H_

#include "HLH_gui.h"

#include "project.h"

typedef struct GUI_state GUI_state;

typedef struct
{
   HLH_gui_element e;

   float x;
   float y;
   float scale;
   Project *project;
   Settings *settings;
   GUI_state *gui;
   SDL_Texture *img;

   int shift_down;
   int ctrl_down;
   int alt_down;
}GUI_canvas;

typedef struct
{
   HLH_gui_element e;

   //Project *project;
   //Settings *settings;
   GUI_canvas *canvas;
   int layer_num;
   int text_len;
   char *text;
   int state;
   int checked;
}GUI_layer;

typedef struct
{
   HLH_gui_element e;

   GUI_canvas *canvas;
   Tool_type type;

   HLH_gui_rect icon_bounds;
   int state;
   int checked;
}GUI_tool;

struct GUI_state
{
   HLH_gui_entry *entry_img_width;
   HLH_gui_entry *entry_img_height;

   HLH_gui_radiobutton *palette_colors[256];
   GUI_layer *layers[16];
   GUI_tool *tools[15];

   GUI_canvas *canvas;
};

extern GUI_state gui_state;

void gui_construct(void);

#endif
