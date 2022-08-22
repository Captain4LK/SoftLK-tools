/*
HLH_gui - gui framework

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
   HLH_GUI_MSG_LAYOUT,
   HLH_GUI_MSG_PAINT,
   HLH_GUI_MSG_UPDATE,
   HLH_GUI_MSG_MOUSE_MOVE,
   HLH_GUI_MSG_DESTROY,

   HLH_GUI_MSG_LEFT_DOWN,
   HLH_GUI_MSG_RIGHT_DOWN,
   HLH_GUI_MSG_MIDDLE_DOWN,
   HLH_GUI_MSG_LEFT_UP,
   HLH_GUI_MSG_RIGHT_UP,
   HLH_GUI_MSG_MIDDLE_UP,
   HLH_GUI_MSG_MOUSE_DRAG,
   HLH_GUI_MSG_CLICK,

   HLH_GUI_MSG_BUTTON_GET_COLOR,
   HLH_GUI_MSG_GET_WIDTH,
   HLH_GUI_MSG_GET_HEIGHT,

   HLH_GUI_MSG_SLIDER_CHANGED_VALUE,
   HLH_GUI_MSG_TAB_CHANGED,

   HLH_GUI_MSG_USR,
}HLH_gui_msg;

typedef enum
{
   HLH_GUI_UPDATE_HOVER = 1,
   HLH_GUI_UPDATE_PRESSED = 2,
}HLH_gui_const;

typedef enum
{
   HLH_GUI_DESTROY = 1<<16,
   HLH_GUI_DESTROY_DESCENDENT = 1<<17,
   HLH_GUI_V_FILL = 1<<18,
   HLH_GUI_H_FILL = 1<<19,
   HLH_GUI_HIDDEN = 1<<20,

   HLH_GUI_LABEL_CENTER = 1<<0,

   HLH_GUI_PANEL_HORIZONTAL = 1<<0,
   HLH_GUI_PANEL_DARK = 1<<1,
   HLH_GUI_PANEL_LIGHT = 1<<2,
}HLH_gui_flag;

typedef int (*HLH_gui_msg_handler)(HLH_gui_element *e, HLH_gui_msg, int di, void *dp);

typedef struct
{
   int l,r,t,b;
}HLH_gui_rect;

typedef struct
{
   HLH_gui_rect clip;
   HLH_gui_window *win;
   uint32_t *data;
   int width;
   int height;
}HLH_gui_painter;

struct HLH_gui_element
{
   uint32_t flags;
   uint32_t child_count;
   HLH_gui_rect bounds;
   HLH_gui_rect clip;
   HLH_gui_element *parent;
   HLH_gui_element **children;
   HLH_gui_window *window;
   void *cp;
   HLH_gui_msg_handler msg_class;
   HLH_gui_msg_handler msg_usr;
};

typedef struct
{
   HLH_gui_element e;
   char *text;
   size_t text_len;
}HLH_gui_button;

typedef struct
{
   HLH_gui_element e;
   char *text;
   size_t text_len;
}HLH_gui_label;

typedef struct
{
   HLH_gui_element e;
   HLH_gui_rect border;
   int gap;
}HLH_gui_panel;

typedef struct
{
   HLH_gui_element e;
   int width;
   int height;
   SDL_Texture *texture;
   //uint32_t *data;
}HLH_gui_image;

typedef struct
{
   HLH_gui_element e;
   int tabs;
   int tab_current;
   char **labels;
}HLH_gui_vtab;

typedef struct
{
   HLH_gui_element e;
   int tabs;
   int tab_current;
   char **labels;
}HLH_gui_htab;

typedef struct
{
   HLH_gui_element e;
   int value;
   int min;
   int max;
}HLH_gui_slider;

struct HLH_gui_window
{
   HLH_gui_element e;
   uint32_t *data;
   int width;
   int height;

   //Input
   int mouse_x;
   int mouse_y;
   int pressed_button;
   HLH_gui_element *hover;
   HLH_gui_element *pressed;

   HLH_gui_rect update_region;

   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Texture *target;
   SDL_Texture *font;
};

#define HLH_GUI_GLYPH_WIDTH (9)
#define HLH_GUI_GLYPH_HEIGHT (16)

//Core
void HLH_gui_init(void);
int HLH_gui_message_loop(void);
void HLH_gui_set_scale(int scale);
int HLH_gui_get_scale(void);
HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height);
void HLH_gui_string_copy(char **dest, size_t *dest_size, const char *src, ptrdiff_t src_len);

//Elements
HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint32_t flags, HLH_gui_msg_handler msg_handler);
void HLH_gui_element_move(HLH_gui_element *e, HLH_gui_rect bounds, int always_layout);
void HLH_gui_element_repaint(HLH_gui_element *e, HLH_gui_rect *region);
int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
HLH_gui_element *HLH_gui_element_find_by_point(HLH_gui_element *e, int x, int y);
void HLH_gui_element_destroy(HLH_gui_element *e);

//Rectangles
HLH_gui_rect HLH_gui_rect_make(int l, int r, int t, int b);
HLH_gui_rect HLH_gui_rect_intersect(HLH_gui_rect a, HLH_gui_rect b);
HLH_gui_rect HLH_gui_rect_bounding(HLH_gui_rect a, HLH_gui_rect b);
int HLH_gui_rect_valid(HLH_gui_rect r);
int HLH_gui_rect_equal(HLH_gui_rect a, HLH_gui_rect b);
int HLH_gui_rect_inside(HLH_gui_rect a, int x, int y);

//Drawing
void HLH_gui_draw_block(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color);
void HLH_gui_draw_string(HLH_gui_painter *p, HLH_gui_rect bounds, const char *str, size_t bytes, uint32_t color, int align_center);
void HLH_gui_draw_rectangle(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color_fill, uint32_t color_border);

//button
HLH_gui_button *HLH_gui_button_create(HLH_gui_element *parent, uint32_t flags, const char *text, ptrdiff_t text_len);

//label
HLH_gui_label *HLH_gui_label_create(HLH_gui_element *parent, uint32_t flags,  const char *text, ptrdiff_t text_len);
void HLH_gui_label_set_text(HLH_gui_label *l, const char *text, ptrdiff_t text_len);

//panel
HLH_gui_panel *HLH_gui_panel_create(HLH_gui_element *parent, uint32_t flags);

//htab
HLH_gui_htab *HLH_gui_htab_create(HLH_gui_element *parent, uint32_t flags);
void HLH_gui_htab_set(HLH_gui_htab *h, int tab, const char *str);

//vtab
HLH_gui_vtab *HLH_gui_vtab_create(HLH_gui_element *parent, uint32_t flags);
void HLH_gui_vtab_set(HLH_gui_vtab *h, int tab, const char *str);

//image
HLH_gui_image *HLH_gui_image_create(HLH_gui_element *parent, uint32_t flags, int width, int height, uint32_t *data);
void HLH_gui_image_update(HLH_gui_image *img, int width, int height, uint32_t *data);

//slider
HLH_gui_slider *HLH_gui_slider_create(HLH_gui_element *parent, uint32_t flags, int value, int min, int max);
void HLH_gui_slider_set_value(HLH_gui_slider *slider, int value);

#endif
