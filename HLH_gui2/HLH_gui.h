/*
HLH_gui - gui framework

Written in 2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _HLH_GUI_H_


#define _HLH_GUI_H_

#include <SDL2/SDL.h>

typedef struct HLH_gui_element HLH_gui_element;
typedef struct HLH_gui_window HLH_gui_window;

typedef enum
{
   HLH_GUI_MSG_INVALID = -1,
   HLH_GUI_MSG_DESTROY = 0,
   HLH_GUI_MSG_DRAW = 1,
}HLH_gui_msg;

typedef struct
{
   int minx, miny;
   int maxx,maxy;
}HLH_gui_rect;

typedef struct
{
   int x;
   int y;
}HLH_gui_point;

//Flags (not enum because enum is int, we need u64)
#define HLH_GUI_INVISIBLE  UINT64_C(1<< 0)
#define HLH_GUI_IGNORE     UINT64_C(1<< 1)
#define HLH_GUI_DESTROY    UINT64_C(1<< 2)
#define HLH_GUI_MAX_X      UINT64_C(1<< 3)
#define HLH_GUI_MAX_Y      UINT64_C(1<< 4)
#define HLH_GUI_FIXED_X    UINT64_C(1<< 5)
#define HLH_GUI_FIXED_Y    UINT64_C(1<< 6)
#define HLH_GUI_PACK_NORTH UINT64_C(1<< 7)
#define HLH_GUI_PACK_EAST  UINT64_C(1<< 8)
#define HLH_GUI_PACK_SOUTH UINT64_C(1<< 9)
#define HLH_GUI_PACK_WEST  UINT64_C(1<<10)

typedef int (*HLH_gui_msg_handler)(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);

struct HLH_gui_element
{
   uint64_t flags;

   HLH_gui_window *window;

   HLH_gui_rect bounds;
   HLH_gui_point size_required;
   HLH_gui_point child_size_required;

   HLH_gui_element *parent;
   HLH_gui_element **children;
   HLH_gui_element *next;
   int child_count;

   HLH_gui_msg_handler msg_base;
   HLH_gui_msg_handler msg_usr;
};

struct HLH_gui_window
{
   HLH_gui_element e;
   HLH_gui_element *hover;
   HLH_gui_element *pressed;

   int width;
   int height;

   int mouse_x;
   int mouse_y;

   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Texture *target;
   SDL_Texture *font;
};

void HLH_gui_init(void);
HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height);
int HLH_gui_message_loop(void);

HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint32_t flags, HLH_gui_msg_handler msg_handler);
int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
void HLH_gui_element_redraw(HLH_gui_element *e);
void HLH_gui_element_pack(HLH_gui_element *e, HLH_gui_rect space);

HLH_gui_rect HLH_gui_rect_make(int minx, int miny, int maxx, int maxy);

HLH_gui_point HLH_gui_point_make(int x, int y);

#endif
