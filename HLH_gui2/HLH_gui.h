/*
HLH_gui - gui framework

Written in 2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _HLH_GUI_H_


#define _HLH_GUI_H_

#include <SDL2/SDL.h>

#define HLH_GUI_GLYPH_WIDTH (9)
#define HLH_GUI_GLYPH_HEIGHT (16)

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
   HLH_GUI_MSG_CLICK = 5,
   HLH_GUI_MSG_CLICK_MENU = 6,
   HLH_GUI_MSG_HIT = 7,
   HLH_GUI_MSG_GET_PRIORITY = 8,
   HLH_GUI_MSG_SLIDER_VALUE_CHANGED = 9,
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

typedef struct
{
   uint8_t button;
   HLH_gui_point pos;
}HLH_gui_mouse;

#define HLH_GUI_MOUSE_LEFT      (UINT8_C(0x1))
#define HLH_GUI_MOUSE_RIGHT     (UINT8_C(0x2))
#define HLH_GUI_MOUSE_MIDDLE    (UINT8_C(0x4))
#define HLH_GUI_MOUSE_OUT       (UINT8_C(0x8))

//Flags (not enum because enums are int, we need u64)
//-------------------------------------
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

#define HLH_GUI_INVISIBLE       (UINT64_C(0x80))
#define HLH_GUI_IGNORE          (UINT64_C(0x100))
#define HLH_GUI_DESTROY         (UINT64_C(0x200))
#define HLH_GUI_MAX_X           (UINT64_C(0x400))
#define HLH_GUI_MAX_Y           (UINT64_C(0x800))
#define HLH_GUI_FIXED_X         (UINT64_C(0x1000))
#define HLH_GUI_FIXED_Y         (UINT64_C(0x2000))
#define HLH_GUI_FILL_X          (UINT64_C(0x4000))
#define HLH_GUI_FILL_Y          (UINT64_C(0x8000))
#define HLH_GUI_EXPAND          (UINT64_C(0x10000))
#define HLH_GUI_REMOUSE         (UINT64_C(0x20000))

#define HLH_GUI_STYLE           (UINT64_C(0x3c0000))
#define    HLH_GUI_STYLE_00     (UINT64_C(0x00000))
#define    HLH_GUI_STYLE_01     (UINT64_C(0x40000))
#define    HLH_GUI_STYLE_02     (UINT64_C(0x80000))
#define    HLH_GUI_STYLE_03     (UINT64_C(0xc0000))
#define    HLH_GUI_STYLE_04     (UINT64_C(0x100000))
#define    HLH_GUI_STYLE_05     (UINT64_C(0x140000))
#define    HLH_GUI_STYLE_06     (UINT64_C(0x180000))
#define    HLH_GUI_STYLE_07     (UINT64_C(0x1c0000))
#define    HLH_GUI_STYLE_08     (UINT64_C(0x200000))
#define    HLH_GUI_STYLE_09     (UINT64_C(0x240000))
#define    HLH_GUI_STYLE_10     (UINT64_C(0x280000))
#define    HLH_GUI_STYLE_11     (UINT64_C(0x2c0000))
#define    HLH_GUI_STYLE_12     (UINT64_C(0x300000))
#define    HLH_GUI_STYLE_13     (UINT64_C(0x340000))
#define    HLH_GUI_STYLE_14     (UINT64_C(0x380000))
#define    HLH_GUI_STYLE_15     (UINT64_C(0x3c0000))
//-------------------------------------

typedef int (*HLH_gui_msg_handler)(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);

struct HLH_gui_element
{
   //Public
   HLH_gui_point pad_in;
   HLH_gui_point pad_out;
   HLH_gui_point fixed_size;
   int usr;
   void *usr_ptr;

   //Private -- do not modify
   uint64_t flags;

   HLH_gui_window *window;

   HLH_gui_point size;
   HLH_gui_rect bounds;
   HLH_gui_point size_required;
   HLH_gui_point child_size_required;

   const char *type;

   HLH_gui_element *last_mouse;
   HLH_gui_element *parent;
   HLH_gui_element **children;
   int child_count;

   HLH_gui_msg_handler msg_base;
   HLH_gui_msg_handler msg_usr;
};

struct HLH_gui_window
{
   HLH_gui_element e;

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
}HLH_gui_group;

typedef struct
{
   HLH_gui_element e;

   int text_len;
   char *text;
   int state;
}HLH_gui_textbutton;

typedef struct
{
   HLH_gui_element e;

   int text_len;
   char *text;
   int state;
   int checked;
}HLH_gui_checkbutton;

typedef struct
{
   HLH_gui_element e;

   int text_len;
   char *text;
   int state;
   int checked;
}HLH_gui_radiobutton;

typedef struct
{
   HLH_gui_element e;

   int text_len;
   char *text;
}HLH_gui_label;

typedef struct
{
   HLH_gui_element e;

   int text_len;
   char *text;
   int state;
   int index;
}HLH_gui_menubutton;

typedef struct
{
   HLH_gui_element e;

   int text_len;
   char *text;
   int state;
   HLH_gui_element *pull;
   uint64_t side;
}HLH_gui_pulldown;

typedef struct
{
   HLH_gui_element e;

   int direction; //0 --> horizontal; 1 --> vertical
}HLH_gui_separator;

typedef struct
{
   HLH_gui_element e;

   int value;
   int range;
   int direction; //0 --> horizontal; 1 --> vertical
}HLH_gui_slider;

void HLH_gui_init(void);
HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height, const char *path_icon);
int HLH_gui_message_loop(void);
void HLH_gui_set_scale(int scale);
int HLH_gui_get_scale(void);
void HLH_gui_handle_mouse(HLH_gui_element *e, HLH_gui_mouse m);
void HLH_gui_window_close(HLH_gui_window *win);

//Element
HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint64_t flags, HLH_gui_msg_handler msg_handler);
int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
void HLH_gui_element_redraw(HLH_gui_element *e);
void HLH_gui_element_pack(HLH_gui_element *e, HLH_gui_rect space);
HLH_gui_point HLH_gui_element_size(HLH_gui_element *e, HLH_gui_point children);
void HLH_gui_element_child_space(HLH_gui_element *e, HLH_gui_rect *space);
HLH_gui_element *HLH_gui_element_by_point(HLH_gui_element *e, HLH_gui_point pt);
int HLH_gui_element_priority(HLH_gui_element *e, HLH_gui_point pt);
void HLH_gui_element_invisible(HLH_gui_element *e, int invisible);
void HLH_gui_element_ignore(HLH_gui_element *e, int ignore);
void HLH_gui_element_destroy(HLH_gui_element *e); //Only use on root elements (no parents or windows)

//Rectangle
HLH_gui_rect HLH_gui_rect_make(int minx, int miny, int maxx, int maxy);
int HLH_gui_rect_inside(HLH_gui_rect r, HLH_gui_point p);

//Point
HLH_gui_point HLH_gui_point_make(int x, int y);
HLH_gui_point HLH_gui_point_add(HLH_gui_point a, HLH_gui_point b);
HLH_gui_point HLH_gui_point_sub(HLH_gui_point a, HLH_gui_point b);

//Drawing
void HLH_gui_draw_rectangle(HLH_gui_element *e, HLH_gui_rect rect, uint32_t color_border);
void HLH_gui_draw_rectangle_fill(HLH_gui_element *e, HLH_gui_rect rect, uint32_t color);
void HLH_gui_draw_string(HLH_gui_element *e, HLH_gui_rect bounds, const char *text, int len, uint32_t color, int align_center);

//Groups
HLH_gui_group *HLH_gui_group_create(HLH_gui_element *parent, uint64_t flags);

//Label
HLH_gui_label *HLH_gui_label_create(HLH_gui_element *parent, uint64_t flags, const char *text);

//Buttons
HLH_gui_textbutton *HLH_gui_textbutton_create(HLH_gui_element *parent, uint64_t flags, const char *text);
HLH_gui_checkbutton *HLH_gui_checkbutton_create(HLH_gui_element *parent, uint64_t flags, const char *text);
void HLH_gui_checkbutton_set(HLH_gui_element *e, int checked, int trigger_msg, int redraw);
HLH_gui_radiobutton *HLH_gui_radiobutton_create(HLH_gui_element *parent, uint64_t flags, const char *text);
void HLH_gui_radiobutton_set(HLH_gui_radiobutton *r, int trigger_msg, int redraw);

//Menu
//Creates group with label_count buttons as children
HLH_gui_group *HLH_gui_menu_create(HLH_gui_element *parent, uint64_t flags, uint64_t cflags, const char **labels, int label_count, HLH_gui_msg_handler msg_usr);

//Menubar
HLH_gui_group *HLH_gui_menubar_create(HLH_gui_element *parent, uint64_t flags, uint64_t cflags, const char **labels, HLH_gui_element **panels, int child_count, HLH_gui_msg_handler msg_usr);

//Seperator
HLH_gui_separator *HLH_gui_separator_create(HLH_gui_element *parent, uint64_t flags, int direction);

//Slider
HLH_gui_slider *HLH_gui_slider_create(HLH_gui_element *parent, uint64_t flags, int direction);
void HLH_gui_slider_set(HLH_gui_slider *slider, int value, int range, int trigger_msg, int redraw);

#endif
