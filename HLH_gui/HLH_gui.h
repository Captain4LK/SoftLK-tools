/*
HLH_gui - gui framework

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
   HLH_GUI_MSG_NO_BLOCK_START = 0,
   HLH_GUI_MSG_DESTROY = 0,
   HLH_GUI_MSG_DRAW = 1,
   HLH_GUI_MSG_GET_WIDTH = 2,
   HLH_GUI_MSG_GET_HEIGHT = 3,
   HLH_GUI_MSG_GET_CHILD_SPACE = 4,
   HLH_GUI_MSG_NO_BLOCK_END = 5,
   HLH_GUI_MSG_CLICK = 6,
   HLH_GUI_MSG_CLICK_MENU = 7,
   HLH_GUI_MSG_MOUSE = 8,
   HLH_GUI_MSG_SLIDER_VALUE_CHANGED = 9,
   HLH_GUI_MSG_BUTTON_DOWN = 10,
   HLH_GUI_MSG_BUTTON_REPEAT = 11,
   HLH_GUI_MSG_BUTTON_UP = 12,
   HLH_GUI_MSG_TIMER = 13,
   HLH_GUI_MSG_TEXTINPUT = 14,
   HLH_GUI_MSG_TEXTINPUT_END = 15,
   HLH_GUI_MSG_MOUSE_LEAVE = 16,
   HLH_GUI_MSG_USER_START = 17,
}HLH_gui_msg;

typedef struct
{
   int minx, miny;
   int maxx, maxy;
}HLH_gui_rect;

typedef struct
{
   int x;
   int y;
}HLH_gui_point;

typedef struct
{
   uint8_t button;
   int wheel;
   HLH_gui_point pos;
   HLH_gui_point rel;
}HLH_gui_mouse;

typedef enum
{
   HLH_GUI_UNKNOWN = 0,
   HLH_GUI_GROUP,
   HLH_GUI_LABEL,
   HLH_GUI_BUTTON,
   HLH_GUI_CHECKBUTTON,
   HLH_GUI_RADIOBUTTON,
   HLH_GUI_SEPARATOR,
   HLH_GUI_SLIDER,
   HLH_GUI_IMAGE,
   HLH_GUI_IMGCMP,
   HLH_GUI_ENTRY,
   HLH_GUI_WINDOW,
   HLH_GUI_DROPDOWN,
   HLH_GUI_MENUBUTTON,
   HLH_GUI_USER, //choose user type relative to this enum: type = HLH_GUI_USER+X
}HLH_gui_type;

#define HLH_GUI_MOUSE_LEFT      (UINT8_C(0x1))
#define HLH_GUI_MOUSE_RIGHT     (UINT8_C(0x2))
#define HLH_GUI_MOUSE_MIDDLE    (UINT8_C(0x4))
#define HLH_GUI_MOUSE_DBLE      (UINT8_C(0x8))

//Flags (not enum because enums are int, we need u64)
//-------------------------------------
#define HLH_GUI_LAYOUT               (UINT64_C(0x1))
#define    HLH_GUI_LAYOUT_VERTICAL   (UINT64_C(0x0))
#define    HLH_GUI_LAYOUT_HORIZONTAL (UINT64_C(0x1))

#define HLH_GUI_NO_CENTER_X     (UINT64_C(0x2))
#define HLH_GUI_NO_CENTER_Y     (UINT64_C(0x4))

#define HLH_GUI_INVISIBLE       (UINT64_C(0x8))
#define HLH_GUI_IGNORE          (UINT64_C(0x10))
#define HLH_GUI_DESTROY         (UINT64_C(0x20))
#define HLH_GUI_FILL_X          (UINT64_C(0x100))
#define HLH_GUI_FILL_Y          (UINT64_C(0x200))
#define HLH_GUI_FILL            (HLH_GUI_FILL_X|HLH_GUI_FILL_Y)
#define HLH_GUI_CAPTURE_MOUSE   (UINT64_C(0x400))

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

#define HLH_GUI_NO_PARENT       (UINT64_C(0x400000))
#define HLH_GUI_OVERLAY         (UINT64_C(0x800000))
//-------------------------------------

#define HLH_gui_flag_set(var,flag,value) do { uint64_t val = (!!value)*flag; var&=~(uint64_t)flag; var|=val; } while(0)

typedef int (*HLH_gui_msg_handler)(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);

struct HLH_gui_element
{
   //Public
   HLH_gui_point pad_in;
   HLH_gui_point pad_out;
   uint32_t usr;
   void *usr_ptr;
   HLH_gui_msg_handler msg_usr;

   //Private -- do not modify
   uint64_t flags;
   uint64_t id;

   HLH_gui_window *window;

   int needs_redraw;

   HLH_gui_rect bounds;
   HLH_gui_point size_required;
   HLH_gui_point child_size_required;
   SDL_TimerID timer;
   int timer_interval;

   HLH_gui_type type;

   HLH_gui_element *last_mouse;
   HLH_gui_element *parent;
   HLH_gui_element **children;
   int child_count;

   HLH_gui_msg_handler msg_base;
};

struct HLH_gui_window
{
   HLH_gui_element e;

   int width;
   int height;

   int mouse_x;
   int mouse_y;

   HLH_gui_element *keyboard;
   HLH_gui_window *blocking;

   HLH_gui_element **redraw;

   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Texture *target;
   SDL_Texture *overlay;
   SDL_Texture *font;
   SDL_Texture *icons;
};

typedef struct
{
   HLH_gui_element e;
}HLH_gui_group;

typedef struct
{
   HLH_gui_element e;

   int is_icon;
   HLH_gui_rect icon_bounds;
   int text_len;
   char *text;
   int state;
}HLH_gui_button;

typedef struct
{
   HLH_gui_element e;

   int is_icon;
   HLH_gui_rect icon_bounds;
   int text_len;
   char *text;
   int state;
   int checked;
}HLH_gui_checkbutton;

typedef struct
{
   HLH_gui_element e;

   int is_icon;
   HLH_gui_rect icon_bounds;
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
   HLH_gui_element *drop;
   uint64_t side;
}HLH_gui_dropdown;

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

typedef struct
{
   HLH_gui_element e;

   int width;
   int height;
   SDL_Texture *img;
}HLH_gui_image;

typedef struct
{
   HLH_gui_element e;

   //both images need same aspect ratio!!!
   int width0;
   int height0;
   int width1;
   int height1;
   int slider; //Value from 0 to 2048
   SDL_Texture *img0;
   SDL_Texture *img1;
}HLH_gui_imgcmp;

typedef struct
{
   HLH_gui_element e;

   char *entry;
   int len;
   int max_len;
   int state;
   int active;
}HLH_gui_entry;

typedef struct
{
   int type; //0 --> char; 1 --> keycode
   char ch;
   SDL_Keycode keycode;
}HLH_gui_textinput;

void HLH_gui_init(void);
HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height, const char *path_icon);
int HLH_gui_message_loop(void);
void HLH_gui_set_scale(int scale);
int HLH_gui_get_scale(void);
void HLH_gui_handle_mouse(HLH_gui_element *e, HLH_gui_mouse m);
void HLH_gui_window_close(HLH_gui_window *win);
void HLH_gui_overlay_clear(HLH_gui_element *e);
void HLH_gui_window_block(HLH_gui_window *root, HLH_gui_window *blocking);
void HLH_gui_textinput_start(HLH_gui_element *e);
void HLH_gui_textinput_stop(HLH_gui_window *w);

//Element
HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint64_t flags, HLH_gui_msg_handler msg_handler);
int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
int HLH_gui_element_msg_all(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
void HLH_gui_element_redraw(HLH_gui_element *e);
void HLH_gui_element_redraw_now(HLH_gui_element *e);
void HLH_gui_element_redraw_msg(HLH_gui_element *e);
void HLH_gui_element_layout(HLH_gui_element *e, HLH_gui_rect space);
HLH_gui_point HLH_gui_element_size(HLH_gui_element *e, HLH_gui_point children);
HLH_gui_element *HLH_gui_element_by_point(HLH_gui_element *e, HLH_gui_point pt);
void HLH_gui_element_invisible(HLH_gui_element *e, int invisible);
void HLH_gui_element_ignore(HLH_gui_element *e, int ignore);
int HLH_gui_element_ignored(HLH_gui_element *e);
void HLH_gui_element_destroy(HLH_gui_element *e); //Only use on root elements (no parents or windows)
void HLH_gui_element_timer(HLH_gui_element *e, int interval); //Use sparingly

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
HLH_gui_button *HLH_gui_button_create(HLH_gui_element *parent, uint64_t flags, const char *text, HLH_gui_rect *icon_bounds);
HLH_gui_checkbutton *HLH_gui_checkbutton_create(HLH_gui_element *parent, uint64_t flags, const char *text, HLH_gui_rect *icon_bounds);
void HLH_gui_checkbutton_set(HLH_gui_checkbutton *c, int checked, int trigger_msg, int redraw);
HLH_gui_radiobutton *HLH_gui_radiobutton_create(HLH_gui_element *parent, uint64_t flags, const char *text, HLH_gui_rect *icon_bounds);
void HLH_gui_radiobutton_set(HLH_gui_radiobutton *r, int trigger_msg, int redraw);

//Menu
//Creates group with label_count buttons as children
HLH_gui_group *HLH_gui_menu_create(HLH_gui_element *parent, uint64_t flags, uint64_t cflags, const char **labels, int label_count, HLH_gui_msg_handler msg_usr);

//Menubar
HLH_gui_group *HLH_gui_menubar_create(HLH_gui_element *parent, uint64_t flags, uint64_t cflags, const char **labels, HLH_gui_element **panels, int child_count, HLH_gui_msg_handler msg_usr);
void HLH_gui_menubar_label_set(HLH_gui_group *bar, const char *label, int which);

//Seperator
HLH_gui_separator *HLH_gui_separator_create(HLH_gui_element *parent, uint64_t flags, int direction);

//Slider
HLH_gui_slider *HLH_gui_slider_create(HLH_gui_element *parent, uint64_t flags, int direction);
void HLH_gui_slider_set(HLH_gui_slider *slider, int value, int range, int trigger_msg, int redraw);

//Image
HLH_gui_image *HLH_gui_img_create_path(HLH_gui_element *parent, uint64_t flags, const char *path);
HLH_gui_image *HLH_gui_img_create_data(HLH_gui_element *parent, uint64_t flags, uint32_t *pix, int width, int height);
void HLH_gui_img_update(HLH_gui_image *img, uint32_t *pix, int width, int height, int redraw); //If dimensions changed (and no EXPAND flag), relayouting needed!!!

//Imgcmp
HLH_gui_imgcmp *HLH_gui_imgcmp_create(HLH_gui_element *parent, uint64_t flags, uint32_t *pix0, int width0, int height0, uint32_t *pix1, int width1, int height1);
void HLH_gui_imgcmp_update0(HLH_gui_imgcmp *img, uint32_t *pix, int width, int height, int redraw); //If dimensions changed (and no EXPAND flag), relayouting needed!!!
void HLH_gui_imgcmp_update1(HLH_gui_imgcmp *img, uint32_t *pix, int width, int height, int redraw); //If dimensions changed (and no EXPAND flag), relayouting needed!!!

//Entry
HLH_gui_entry *HLH_gui_entry_create(HLH_gui_element *parent, uint64_t flags, int max_len);
void HLH_gui_entry_set(HLH_gui_entry *entry, char *text);

//Utils
uint32_t *HLH_gui_image_load(FILE *fp, int *width, int *height);
void HLH_gui_image_free(uint32_t *pix);
void HLH_gui_image_save(FILE *fp, uint32_t *data, int width, int height, const char *ext);

#endif
