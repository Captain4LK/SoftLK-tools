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
   HLH_GUI_MSG_GET_WIDTH = 2,
   HLH_GUI_MSG_GET_HEIGHT = 3,
   HLH_GUI_MSG_GET_CHILD_SPACE = 4,
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
#define HLH_GUI_PACK            (UINT64_C(0x7))
#define    HLH_GUI_PACK_NORTH   (UINT64_C(0x0))
#define    HLH_GUI_PACK_EAST    (UINT64_C(0x1))
#define    HLH_GUI_PACK_SOUTH   (UINT64_C(0x2))
#define    HLH_GUI_PACK_WEST    (UINT64_C(0x3))
#define    HLH_GUI_PACK_CENTER  (UINT64_C(0x4))

#define HLH_GUI_PLACE           (UINT64_C(0x78))
#define    HLH_GUI_PLACE_CENTER (UINT64_C(0x0))
#define    HLH_GUI_PLACE_NORTH  (UINT64_C(0x8))
#define    HLH_GUI_PLACE_EAST   (UINT64_C(0x10))
#define    HLH_GUI_PLACE_SOUTH  (UINT64_C(0x18))
#define    HLH_GUI_PLACE_WEST   (UINT64_C(0x20))
#define    HLH_GUI_PLACE_NE     (UINT64_C(0x28))
#define    HLH_GUI_PLACE_SE     (UINT64_C(0x30))
#define    HLH_GUI_PLACE_NW     (UINT64_C(0x38))
#define    HLH_GUI_PLACE_SW     (UINT64_C(0x40))

#define HLH_GUI_INVISIBLE    (UINT64_C(0x80))
#define HLH_GUI_IGNORE       (UINT64_C(0x100))
#define HLH_GUI_DESTROY      (UINT64_C(0x200))
#define HLH_GUI_MAX_X        (UINT64_C(0x400))
#define HLH_GUI_MAX_Y        (UINT64_C(0x800))
#define HLH_GUI_FIXED_X      (UINT64_C(0x1000))
#define HLH_GUI_FIXED_Y      (UINT64_C(0x2000))
#define HLH_GUI_FILL_X       (UINT64_C(0x4000))
#define HLH_GUI_FILL_Y       (UINT64_C(0x8000))
#define HLH_GUI_EXPAND       (UINT64_C(0x10000))


typedef int (*HLH_gui_msg_handler)(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);

struct HLH_gui_element
{
   uint64_t flags;

   HLH_gui_window *window;

   HLH_gui_point pad_in;
   HLH_gui_point pad_out;
   HLH_gui_point fixed_size;
   HLH_gui_point size;

   HLH_gui_rect bounds;
   HLH_gui_point size_required;
   HLH_gui_point child_size_required;

   HLH_gui_element *parent;
   HLH_gui_element **children;
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

typedef struct
{
   HLH_gui_element e;
}HLH_gui_frame;

void HLH_gui_init(void);
HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height);
int HLH_gui_message_loop(void);
void HLH_gui_set_scale(int scale);
int HLH_gui_get_scale(void);

//Element
HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint64_t flags, HLH_gui_msg_handler msg_handler);
int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
void HLH_gui_element_redraw(HLH_gui_element *e);
void HLH_gui_element_pack(HLH_gui_element *e, HLH_gui_rect space);
HLH_gui_point HLH_gui_element_size(HLH_gui_element *e, HLH_gui_point children);
void HLH_gui_element_child_space(HLH_gui_element *e, HLH_gui_rect *space);

//Rectangle
HLH_gui_rect HLH_gui_rect_make(int minx, int miny, int maxx, int maxy);

//Point
HLH_gui_point HLH_gui_point_make(int x, int y);
HLH_gui_point HLH_gui_point_add(HLH_gui_point a, HLH_gui_point b);
HLH_gui_point HLH_gui_point_sub(HLH_gui_point a, HLH_gui_point b);

//Drawing
void HLH_gui_draw_rectangle(HLH_gui_element *e, HLH_gui_rect rect, uint32_t color_border);
void HLH_gui_draw_rectangle_fill(HLH_gui_element *e, HLH_gui_rect rect, uint32_t color);

//Frame
HLH_gui_frame *HLH_gui_frame_create(HLH_gui_element *parent, uint64_t flags);

#endif
