/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _TOOL_H_

#define _TOOL_H_

#include <stdint.h>

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
   TOOL_SPHERE_GRAD,
}Tool_type;

typedef struct
{
   Tool_type selected;

   struct
   {
   }pen;
   struct
   {
      int32_t x0;
      int32_t y0;
      int32_t x1;
      int32_t y2;
   }line;
}Toolbox;

#endif
