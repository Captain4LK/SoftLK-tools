/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _PROJECT_H_

#define _PROJECT_H_

#include <stdint.h>
#include "shared/image.h"

typedef enum
{
   TOOL_PEN,
   TOOL_LINE,
   TOOL_FLOOD,
   TOOL_RECT_OUTLINE,
   TOOL_RECT_FILL,
   TOOL_GRADIENT,
   TOOL_SPLINE,
   TOOL_SPRAY,
   TOOL_POLYLINE,
   TOOL_POLYFORM,
   TOOL_CIRCLE_OUTLINE,
   TOOL_CIRCLE_FILL,
   TOOL_SELECT_RECT,
   TOOL_SELECT_LASSO,
   TOOL_PIPETTE,
}Tool_type;

typedef struct
{
   Tool_type selected;

   struct
   {
      int place_mode;
   }pen;
   struct
   {
      int32_t x0;
      int32_t y0;
      int32_t x1;
      int32_t y2;
   }line;
}Toolbox;

typedef struct
{
   int32_t width;
   int32_t height;
   uint8_t data[];
}Brush;

typedef struct
{
   uint8_t hidden;
   uint8_t data[];
}Layer;

typedef struct
{
   uint32_t palette[256];
   int32_t palette_colors;

   uint8_t palette_selected;

   uint8_t color_white;

   //0 - 47 default
   //48 - 59 custom
   //60 - 63 internal use
   uint8_t brush_selected;
   Brush *brushes[64];
}Settings;

typedef struct
{
   int32_t x0;
   int32_t y0;
   int32_t x1;
   int32_t y1;

   uint8_t button;
}Draw_state;

typedef struct
{
   int32_t width;
   int32_t height;
   //uint32_t palette[256];
   //int32_t palette_colors;

   Image32 *combined;

   int num_layers;
   Layer **layers;

   char file[1024];
   char ext[32];

   //From here:
   //Internal, not saved
   Layer *old;
   uint32_t *undo_map;

   uint32_t *bitmap;

   //uint8_t palette_selected;

   int layer_selected;
   Toolbox tools;

   //Undo
   //--------------------------------
   uint8_t *undo_buffer;
   uint8_t *redo_buffer;
   int32_t undo_len;
   int32_t undo_pos;
   int32_t redo_len;
   int32_t redo_pos;
   uint32_t undo_entry_len;
   uint32_t redo_entry_len;
   //--------------------------------

   Draw_state state;
}Project;

Project *project_new(int32_t width, int32_t height, const Settings *settings);
Project *project_from_image8(Settings *settings, const Image8 *img);
void project_free(Project *project);

Image32 *project_to_image32(const Project *project, const Settings *settings);
Image8 *project_to_image8(const Project *project, const Settings *settings);

void project_layer_add(Project *project, int pos);
void project_layer_delete(Project *project, int pos);

void project_update(Project *project, int x, int y, const Settings *settings);
void project_update_full(Project *project, const Settings *settings);

#endif
