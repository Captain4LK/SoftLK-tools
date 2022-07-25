#ifndef _HLH_GUI_H_

/*
HLH_gui - single header gui

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#define _HLH_GUI_H_

#include <stdint.h>
#include <stddef.h>

#ifdef HLH_GUI_PLATTFORM_WIN32
#define Rectangle W32Rectangle
#include <windows.h>
#undef Rectangle
#endif

#ifdef HLH_GUI_PLATTFORM_LINUX
#define Window X11Window
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#undef Window
#endif

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

   HLH_GUI_LABEL_CENTER = 1<<0,

   HLH_GUI_PANEL_HORIZONTAL = 1<<0,
   HLH_GUI_PANEL_GRAY = 1<<1,
   HLH_GUI_PANEL_WHITE = 1<<2,
}HLH_gui_flag;

typedef int (*HLH_gui_msg_handler)(HLH_gui_element *e, HLH_gui_msg, int di, void *dp);

typedef struct
{
   int l,r,t,b;
}HLH_gui_rect;

typedef struct
{
   HLH_gui_rect clip;
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

#ifdef HLH_GUI_PLATTFORM_WIN32
   HWND hwnd;
   int tracking_leave;
#endif

#ifdef HLH_GUI_PLATTFORM_LINUX
   X11Window window;
   XImage *image;
#endif
};

void HLH_gui_init();
int HLH_gui_message_loop();

HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint32_t flags, HLH_gui_msg_handler msg_handler);

HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height);
HLH_gui_button *HLH_gui_button_create(HLH_gui_element *parent, uint32_t flags, const char *text, ptrdiff_t text_len);
HLH_gui_label *HLH_gui_label_create(HLH_gui_element *parent, uint32_t flags,  const char *text, ptrdiff_t text_len);
HLH_gui_panel *HLH_gui_panel_create(HLH_gui_element *parent, uint32_t flags);

int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
void HLH_gui_element_move(HLH_gui_element *e, HLH_gui_rect bounds, int always_layout);
void HLH_gui_element_repaint(HLH_gui_element *e, HLH_gui_rect *region);
HLH_gui_element *HLH_gui_element_find_by_point(HLH_gui_element *e, int x, int y);
void HLH_gui_element_destroy(HLH_gui_element *e);

void HLH_gui_label_set_text(HLH_gui_label *l, const char *text, ptrdiff_t text_len);

void HLH_gui_draw_block(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color);
void HLH_gui_draw_rectangle(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color_fill, uint32_t color_border);
void HLH_gui_draw_string(HLH_gui_painter *p, HLH_gui_rect bounds, const char *str, size_t bytes, uint32_t color, int align_center);

HLH_gui_rect HLH_gui_rect_make(int l, int r, int t, int b);
HLH_gui_rect HLH_gui_rect_intersect(HLH_gui_rect a, HLH_gui_rect b);
HLH_gui_rect HLH_gui_rect_bounding(HLH_gui_rect a, HLH_gui_rect b);
int HLH_gui_rect_valid(HLH_gui_rect r);
int HLH_gui_rect_equal(HLH_gui_rect a, HLH_gui_rect b);
int HLH_gui_rect_inside(HLH_gui_rect a, int x, int y);

void HLH_gui_string_copy(char **dest, size_t *dest_size, const char *src, ptrdiff_t src_len);

#endif //_HLH_GUI_H_

#ifdef HLH_GUI_IMPLEMENTATION
#ifndef HLH_GUI_IMPLEMENTATION_ONCE
#define HLH_GUI_IMPLEMENTATION_ONCE

#include <stdlib.h>
#include <string.h>

int hlh_gui_window_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
void hlh_gui_element_paint(HLH_gui_element *e, HLH_gui_painter *p);
void hlh_gui_update();
void hlh_gui_window_end_paint(HLH_gui_window *w, HLH_gui_painter *p);
void hlh_gui_window_input_event(HLH_gui_window *w, HLH_gui_msg msg, int di, void *dp);
void hlh_gui_window_set_pressed(HLH_gui_window *w, HLH_gui_element *e, int button);
int hlh_gui_button_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
int hlh_gui_label_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
int hlh_gui_panel_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);

int hlh_gui_panel_layout(HLH_gui_panel *p, HLH_gui_rect bounds, int measure);
int hlh_gui_panel_measure(HLH_gui_panel *p);

int hlh_gui_element_destroy(HLH_gui_element *e);

#define HLH_GUI_GLYPH_WIDTH (9)
#define HLH_GUI_GLYPH_HEIGHT (16)

static const uint64_t hlh_gui_font[] = 
{
   0x0000000000000000UL, 0x0000000000000000UL, 0xBD8181A5817E0000UL, 0x000000007E818199UL, 0xC3FFFFDBFF7E0000UL, 0x000000007EFFFFE7UL, 0x7F7F7F3600000000UL, 0x00000000081C3E7FUL, 
   0x7F3E1C0800000000UL, 0x0000000000081C3EUL, 0xE7E73C3C18000000UL, 0x000000003C1818E7UL, 0xFFFF7E3C18000000UL, 0x000000003C18187EUL, 0x3C18000000000000UL, 0x000000000000183CUL, 
   0xC3E7FFFFFFFFFFFFUL, 0xFFFFFFFFFFFFE7C3UL, 0x42663C0000000000UL, 0x00000000003C6642UL, 0xBD99C3FFFFFFFFFFUL, 0xFFFFFFFFFFC399BDUL, 0x331E4C5870780000UL, 0x000000001E333333UL, 
   0x3C666666663C0000UL, 0x0000000018187E18UL, 0x0C0C0CFCCCFC0000UL, 0x00000000070F0E0CUL, 0xC6C6C6FEC6FE0000UL, 0x0000000367E7E6C6UL, 0xE73CDB1818000000UL, 0x000000001818DB3CUL, 
   0x1F7F1F0F07030100UL, 0x000000000103070FUL, 0x7C7F7C7870604000UL, 0x0000000040607078UL, 0x1818187E3C180000UL, 0x0000000000183C7EUL, 0x6666666666660000UL, 0x0000000066660066UL, 
   0xD8DEDBDBDBFE0000UL, 0x00000000D8D8D8D8UL, 0x6363361C06633E00UL, 0x0000003E63301C36UL, 0x0000000000000000UL, 0x000000007F7F7F7FUL, 0x1818187E3C180000UL, 0x000000007E183C7EUL, 
   0x1818187E3C180000UL, 0x0000000018181818UL, 0x1818181818180000UL, 0x00000000183C7E18UL, 0x7F30180000000000UL, 0x0000000000001830UL, 0x7F060C0000000000UL, 0x0000000000000C06UL, 
   0x0303000000000000UL, 0x0000000000007F03UL, 0xFF66240000000000UL, 0x0000000000002466UL, 0x3E1C1C0800000000UL, 0x00000000007F7F3EUL, 0x3E3E7F7F00000000UL, 0x0000000000081C1CUL, 
   0x0000000000000000UL, 0x0000000000000000UL, 0x18183C3C3C180000UL, 0x0000000018180018UL, 0x0000002466666600UL, 0x0000000000000000UL, 0x36367F3636000000UL, 0x0000000036367F36UL, 
   0x603E0343633E1818UL, 0x000018183E636160UL, 0x1830634300000000UL, 0x000000006163060CUL, 0x3B6E1C36361C0000UL, 0x000000006E333333UL, 0x000000060C0C0C00UL, 0x0000000000000000UL, 
   0x0C0C0C0C18300000UL, 0x0000000030180C0CUL, 0x30303030180C0000UL, 0x000000000C183030UL, 0xFF3C660000000000UL, 0x000000000000663CUL, 0x7E18180000000000UL, 0x0000000000001818UL, 
   0x0000000000000000UL, 0x0000000C18181800UL, 0x7F00000000000000UL, 0x0000000000000000UL, 0x0000000000000000UL, 0x0000000018180000UL, 0x1830604000000000UL, 0x000000000103060CUL, 
   0xDBDBC3C3663C0000UL, 0x000000003C66C3C3UL, 0x1818181E1C180000UL, 0x000000007E181818UL, 0x0C183060633E0000UL, 0x000000007F630306UL, 0x603C6060633E0000UL, 0x000000003E636060UL, 
   0x7F33363C38300000UL, 0x0000000078303030UL, 0x603F0303037F0000UL, 0x000000003E636060UL, 0x633F0303061C0000UL, 0x000000003E636363UL, 0x18306060637F0000UL, 0x000000000C0C0C0CUL, 
   0x633E6363633E0000UL, 0x000000003E636363UL, 0x607E6363633E0000UL, 0x000000001E306060UL, 0x0000181800000000UL, 0x0000000000181800UL, 0x0000181800000000UL, 0x000000000C181800UL, 
   0x060C183060000000UL, 0x000000006030180CUL, 0x00007E0000000000UL, 0x000000000000007EUL, 0x6030180C06000000UL, 0x00000000060C1830UL, 0x18183063633E0000UL, 0x0000000018180018UL, 
   0x7B7B63633E000000UL, 0x000000003E033B7BUL, 0x7F6363361C080000UL, 0x0000000063636363UL, 0x663E6666663F0000UL, 0x000000003F666666UL, 0x03030343663C0000UL, 0x000000003C664303UL, 
   0x66666666361F0000UL, 0x000000001F366666UL, 0x161E1646667F0000UL, 0x000000007F664606UL, 0x161E1646667F0000UL, 0x000000000F060606UL, 0x7B030343663C0000UL, 0x000000005C666363UL, 
   0x637F636363630000UL, 0x0000000063636363UL, 0x18181818183C0000UL, 0x000000003C181818UL, 0x3030303030780000UL, 0x000000001E333333UL, 0x1E1E366666670000UL, 0x0000000067666636UL, 
   0x06060606060F0000UL, 0x000000007F664606UL, 0xC3DBFFFFE7C30000UL, 0x00000000C3C3C3C3UL, 0x737B7F6F67630000UL, 0x0000000063636363UL, 0x63636363633E0000UL, 0x000000003E636363UL, 
   0x063E6666663F0000UL, 0x000000000F060606UL, 0x63636363633E0000UL, 0x000070303E7B6B63UL, 0x363E6666663F0000UL, 0x0000000067666666UL, 0x301C0663633E0000UL, 0x000000003E636360UL, 
   0x18181899DBFF0000UL, 0x000000003C181818UL, 0x6363636363630000UL, 0x000000003E636363UL, 0xC3C3C3C3C3C30000UL, 0x00000000183C66C3UL, 0xDBC3C3C3C3C30000UL, 0x000000006666FFDBUL, 
   0x18183C66C3C30000UL, 0x00000000C3C3663CUL, 0x183C66C3C3C30000UL, 0x000000003C181818UL, 0x0C183061C3FF0000UL, 0x00000000FFC38306UL, 0x0C0C0C0C0C3C0000UL, 0x000000003C0C0C0CUL, 
   0x1C0E070301000000UL, 0x0000000040607038UL, 0x30303030303C0000UL, 0x000000003C303030UL, 0x0000000063361C08UL, 0x0000000000000000UL, 0x0000000000000000UL, 0x0000FF0000000000UL, 
   0x0000000000180C0CUL, 0x0000000000000000UL, 0x3E301E0000000000UL, 0x000000006E333333UL, 0x66361E0606070000UL, 0x000000003E666666UL, 0x03633E0000000000UL, 0x000000003E630303UL, 
   0x33363C3030380000UL, 0x000000006E333333UL, 0x7F633E0000000000UL, 0x000000003E630303UL, 0x060F0626361C0000UL, 0x000000000F060606UL, 0x33336E0000000000UL, 0x001E33303E333333UL, 
   0x666E360606070000UL, 0x0000000067666666UL, 0x18181C0018180000UL, 0x000000003C181818UL, 0x6060700060600000UL, 0x003C666660606060UL, 0x1E36660606070000UL, 0x000000006766361EUL, 
   0x18181818181C0000UL, 0x000000003C181818UL, 0xDBFF670000000000UL, 0x00000000DBDBDBDBUL, 0x66663B0000000000UL, 0x0000000066666666UL, 0x63633E0000000000UL, 0x000000003E636363UL, 
   0x66663B0000000000UL, 0x000F06063E666666UL, 0x33336E0000000000UL, 0x007830303E333333UL, 0x666E3B0000000000UL, 0x000000000F060606UL, 0x06633E0000000000UL, 0x000000003E63301CUL, 
   0x0C0C3F0C0C080000UL, 0x00000000386C0C0CUL, 0x3333330000000000UL, 0x000000006E333333UL, 0xC3C3C30000000000UL, 0x00000000183C66C3UL, 0xC3C3C30000000000UL, 0x0000000066FFDBDBUL, 
   0x3C66C30000000000UL, 0x00000000C3663C18UL, 0x6363630000000000UL, 0x001F30607E636363UL, 0x18337F0000000000UL, 0x000000007F63060CUL, 0x180E181818700000UL, 0x0000000070181818UL, 
   0x1800181818180000UL, 0x0000000018181818UL, 0x18701818180E0000UL, 0x000000000E181818UL, 0x000000003B6E0000UL, 0x0000000000000000UL, 0x63361C0800000000UL, 0x00000000007F6363UL,
};

struct
{
   HLH_gui_window **windows;
   size_t window_count;

#ifdef HLH_GUI_PLATTFORM_LINUX
   Display *display;
   Visual *visual;
   Atom window_closed_id;
#endif
}hlh_gui_state;

HLH_gui_rect HLH_gui_rect_make(int l, int r, int t, int b)
{
   return (HLH_gui_rect){.l = l, .r = r, .t = t, .b = b};
}

HLH_gui_rect HLH_gui_rect_intersect(HLH_gui_rect a, HLH_gui_rect b)
{
   if(a.l<b.l) a.l = b.l;
   if(a.t<b.t) a.t = b.t;
   if(a.r>b.r) a.r = b.r;
   if(a.b>b.b) a.b = b.b;

   return a;
}

HLH_gui_rect HLH_gui_rect_bounding(HLH_gui_rect a, HLH_gui_rect b)
{
   if(a.l>b.l) a.l = b.l;
   if(a.t>b.t) a.t = b.t;
   if(a.r<b.r) a.r = b.r;
   if(a.b<b.b) a.b = b.b;

   return a;
}

int HLH_gui_rect_valid(HLH_gui_rect r)
{
   return r.r>r.l&&r.b>r.t;
}

int HLH_gui_rect_equal(HLH_gui_rect a, HLH_gui_rect b)
{
   return (a.l==b.l)&&(a.r==b.r)&&(a.t==b.t)&&(a.b==b.b);
}

int HLH_gui_rect_inside(HLH_gui_rect a, int x, int y)
{
   return a.l<=x&&a.r>x&&a.t<=y&&a.b>y;
}

void HLH_gui_string_copy(char **dest, size_t *dest_size, const char *src, ptrdiff_t src_len)
{
   if(src_len==-1)
      src_len = strlen(src);

   *dest = realloc(*dest,src_len);
   *dest_size = src_len;
   memcpy(*dest,src,src_len);
}

HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint32_t flags, HLH_gui_msg_handler msg_handler)
{
   HLH_gui_element *e = calloc(1,bytes);
   e->flags = flags;
   e->msg_class = msg_handler;

   if(parent!=NULL)
   {
      e->window = parent->window;
      e->parent = parent;
      parent->child_count++;
      parent->children = realloc(parent->children,sizeof(*parent->children)*parent->child_count);
      parent->children[parent->child_count-1] = e;
   }

   return e;
}

HLH_gui_button *HLH_gui_button_create(HLH_gui_element *parent, uint32_t flags, const char *text, ptrdiff_t text_len)
{
   HLH_gui_button *button = HLH_gui_element_create(sizeof(*button),parent,flags,hlh_gui_button_msg);
   HLH_gui_string_copy(&button->text,&button->text_len,text,text_len);

   return button;
}

HLH_gui_label *HLH_gui_label_create(HLH_gui_element *parent, uint32_t flags,  const char *text, ptrdiff_t text_len)
{
   HLH_gui_label *label = HLH_gui_element_create(sizeof(*label),parent,flags,hlh_gui_label_msg);
   HLH_gui_string_copy(&label->text,&label->text_len,text,text_len);

   return label;
}

HLH_gui_panel *HLH_gui_panel_create(HLH_gui_element *parent, uint32_t flags)
{
   HLH_gui_panel *panel = HLH_gui_element_create(sizeof(*panel),parent,flags,hlh_gui_panel_msg);

   return panel;
}

int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(e->flags&HLH_GUI_DESTROY&&msg!=HLH_GUI_MSG_DESTROY)
      return 0;

   if(e->msg_usr!=NULL)
   {
      int res = e->msg_usr(e,msg,di,dp);
      if(res)
         return res;
   }

   if(e->msg_class!=NULL)
      return e->msg_class(e,msg,di,dp);
   return 0;
}

void HLH_gui_element_move(HLH_gui_element *e, HLH_gui_rect bounds, int always_layout)
{
   HLH_gui_rect old_clip = e->clip;
   e->clip = HLH_gui_rect_intersect(e->parent->clip,bounds);

   if(!HLH_gui_rect_equal(e->bounds,bounds)||!HLH_gui_rect_equal(e->clip,old_clip)||always_layout)
   {
      e->bounds = bounds;
      HLH_gui_element_msg(e,HLH_GUI_MSG_LAYOUT,0,NULL);
      HLH_gui_element_repaint(e,NULL);
   }
}

void HLH_gui_element_repaint(HLH_gui_element *e, HLH_gui_rect *region)
{
   if(region==NULL)
      region = &e->bounds;

   HLH_gui_rect r = HLH_gui_rect_intersect(*region,e->clip);

   if(HLH_gui_rect_valid(r))
   {
      if(HLH_gui_rect_valid(e->window->update_region))
         e->window->update_region = HLH_gui_rect_bounding(e->window->update_region,r);
      else
         e->window->update_region = r;
   }
}

HLH_gui_element *HLH_gui_element_find_by_point(HLH_gui_element *e, int x, int y)
{
   for(int i = 0;i<e->child_count;i++)
      if(HLH_gui_rect_inside(e->children[i]->clip,x,y))
         return HLH_gui_element_find_by_point(e->children[i],x,y);
   return e;
}

void HLH_gui_element_destroy(HLH_gui_element *e)
{
   if(e->flags&HLH_GUI_DESTROY)
      return;

   e->flags|=HLH_GUI_DESTROY;

   HLH_gui_element *ancestor = e->parent;
   while(ancestor!=NULL)
   {
      ancestor->flags|=HLH_GUI_DESTROY_DESCENDENT;
      ancestor = ancestor->parent;
   }

   for(int i = 0;i<e->child_count;i++)
      HLH_gui_element_destroy(e->children[i]);
}

void HLH_gui_label_set_text(HLH_gui_label *l, const char *text, ptrdiff_t text_len)
{
   HLH_gui_string_copy(&l->text,&l->text_len,text,text_len);
   HLH_gui_element_repaint(&l->e,NULL);
}

void HLH_gui_draw_block(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color)
{
   rect = HLH_gui_rect_intersect(p->clip,rect);

   for(int y = rect.t;y<rect.b;y++)
   {
      for(int x = rect.l;x<rect.r;x++)
      {
         p->data[y*p->width+x] = color;
      }
   }
}

void HLH_gui_draw_rectangle(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color_fill, uint32_t color_border)
{
   HLH_gui_draw_block(p,HLH_gui_rect_make(rect.l,rect.r,rect.t,rect.t+1),color_border);
   HLH_gui_draw_block(p,HLH_gui_rect_make(rect.l,rect.l+1,rect.t+1,rect.b-1),color_border);
   HLH_gui_draw_block(p,HLH_gui_rect_make(rect.r-1,rect.r,rect.t+1,rect.b-1),color_border);
   HLH_gui_draw_block(p,HLH_gui_rect_make(rect.l,rect.r,rect.b-1,rect.b),color_border);
   HLH_gui_draw_block(p,HLH_gui_rect_make(rect.l+1,rect.r-1,rect.t+1,rect.b-1),color_fill);
}

void HLH_gui_draw_string(HLH_gui_painter *p, HLH_gui_rect bounds, const char *str, size_t bytes, uint32_t color, int align_center)
{
   HLH_gui_rect old_clip = p->clip;
   p->clip = HLH_gui_rect_intersect(bounds,old_clip);
   int x = bounds.l;
   int y = (bounds.t+bounds.b-HLH_GUI_GLYPH_HEIGHT)/2;

   if(align_center)
      x+=(bounds.r-bounds.l-bytes*HLH_GUI_GLYPH_WIDTH)/2;

   for(int i = 0;i<bytes;i++)
   {
      uint8_t c = str[i];
      if(c>127)
         c = '?';

      HLH_gui_rect rect = HLH_gui_rect_intersect(p->clip,HLH_gui_rect_make(x,x+8,y,y+16));
      const uint8_t *data = (const uint8_t *)hlh_gui_font+c*16;

      for(int iy = rect.t;iy<rect.b;iy++)
      {
         uint32_t *dst = &p->data[iy*p->width+rect.l];
         uint8_t byte = data[iy-y];

         for(int ix = rect.l;ix<rect.r;ix++,dst++)
         {
            if(byte&(1<<(ix-x)))
               *dst = color;
         }
      }

      x+=HLH_GUI_GLYPH_WIDTH;
   }

   p->clip = old_clip;
}

void hlh_gui_element_paint(HLH_gui_element *e, HLH_gui_painter *p)
{
   HLH_gui_rect clip = HLH_gui_rect_intersect(e->clip,p->clip);

   if(!HLH_gui_rect_valid(clip))
      return;

   p->clip = clip;
   HLH_gui_element_msg(e,HLH_GUI_MSG_PAINT,0,p);

   for(uint32_t i = 0;i<e->child_count;i++)
   {
      p->clip = clip;
      hlh_gui_element_paint(e->children[i],p);
   }
}

void hlh_gui_update()
{
   for(int i = 0;i<hlh_gui_state.window_count;i++)
   {
      HLH_gui_window *win = hlh_gui_state.windows[i];

      if(hlh_gui_element_destroy(&win->e))
      {
         hlh_gui_state.windows[i] = hlh_gui_state.windows[hlh_gui_state.window_count-1];
         hlh_gui_state.window_count--;
         i--;
      }
      else if(HLH_gui_rect_valid(win->update_region))
      {
         HLH_gui_painter painter;
         painter.data = win->data;
         painter.width = win->width;
         painter.height = win->height;
         painter.clip = HLH_gui_rect_intersect(HLH_gui_rect_make(0,win->width,0,win->height),win->update_region);

         hlh_gui_element_paint(&win->e,&painter);
         hlh_gui_window_end_paint(win,&painter);
         win->update_region = HLH_gui_rect_make(0,0,0,0);
      }
   }
}

void hlh_gui_window_input_event(HLH_gui_window *w, HLH_gui_msg msg, int di, void *dp)
{
   if(w->pressed!=NULL)
   {
      if(msg==HLH_GUI_MSG_MOUSE_MOVE)
      {
         HLH_gui_element_msg(w->pressed,HLH_GUI_MSG_MOUSE_DRAG,di,dp);
      }
      else if(msg==HLH_GUI_MSG_LEFT_UP&&w->pressed_button==1)
      {
         if(w->hover==w->pressed)
            HLH_gui_element_msg(w->pressed,HLH_GUI_MSG_CLICK,di,dp);

         HLH_gui_element_msg(w->pressed,msg,di,dp);
         hlh_gui_window_set_pressed(w,NULL,1);
      }
      else if(msg==HLH_GUI_MSG_MIDDLE_UP&&w->pressed_button==2)
      {
         HLH_gui_element_msg(w->pressed,msg,di,dp);
         hlh_gui_window_set_pressed(w,NULL,2);
      }
      else if(msg==HLH_GUI_MSG_RIGHT_UP&&w->pressed_button==3)
      {
         HLH_gui_element_msg(w->pressed,msg,di,dp);
         hlh_gui_window_set_pressed(w,NULL,3);
      }
   }

   if(w->pressed!=NULL)
   {
      int inside = HLH_gui_rect_inside(w->pressed->clip,w->mouse_x,w->mouse_y);

      if(inside&&w->hover==&w->e)
      {
         w->hover = w->pressed;
         HLH_gui_element_msg(w->pressed,HLH_GUI_MSG_UPDATE,HLH_GUI_UPDATE_HOVER,NULL);
      }
      else if(!inside&&w->hover==w->pressed)
      {
         w->hover = &w->e;
         HLH_gui_element_msg(w->pressed,HLH_GUI_MSG_UPDATE,HLH_GUI_UPDATE_HOVER,NULL);
      }
   }
   else
   {
      HLH_gui_element *hover = HLH_gui_element_find_by_point(&w->e,w->mouse_x,w->mouse_y);

      if(msg==HLH_GUI_MSG_MOUSE_MOVE)
      {
         HLH_gui_element_msg(hover,msg,di,dp);
      }
      else if(msg==HLH_GUI_MSG_LEFT_DOWN)
      {
         hlh_gui_window_set_pressed(w,hover,1);
         HLH_gui_element_msg(hover,msg,di,dp);
      }
      else if(msg==HLH_GUI_MSG_MIDDLE_DOWN)
      {
         hlh_gui_window_set_pressed(w,hover,2);
         HLH_gui_element_msg(hover,msg,di,dp);
      }
      else if(msg==HLH_GUI_MSG_RIGHT_DOWN)
      {
         hlh_gui_window_set_pressed(w,hover,3);
         HLH_gui_element_msg(hover,msg,di,dp);
      }

      if(hover!=w->hover)
      {
         HLH_gui_element *prev = w->hover;
         w->hover = hover;
         HLH_gui_element_msg(prev,HLH_GUI_MSG_UPDATE,HLH_GUI_UPDATE_HOVER,NULL);
         HLH_gui_element_msg(w->hover,HLH_GUI_MSG_UPDATE,HLH_GUI_UPDATE_HOVER,NULL);
      }
   }

   hlh_gui_update();
}

void hlh_gui_window_set_pressed(HLH_gui_window *w, HLH_gui_element *e, int button)
{
   HLH_gui_element *prev = w->pressed;

   w->pressed = e;
   w->pressed_button = button;

   if(prev!=NULL)
      HLH_gui_element_msg(prev,HLH_GUI_MSG_UPDATE,HLH_GUI_UPDATE_PRESSED,NULL);
   if(e!=NULL)
      HLH_gui_element_msg(e,HLH_GUI_MSG_UPDATE,HLH_GUI_UPDATE_PRESSED,NULL);
}

int hlh_gui_button_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;

   if(msg==HLH_GUI_MSG_PAINT)
   {
      HLH_gui_painter *painter = dp;

      int pressed = e->window->pressed==e&&e->window->hover==e;

      uint32_t c = 0xffffff;
      HLH_gui_element_msg(e,HLH_GUI_MSG_BUTTON_GET_COLOR,0,&c);
      uint32_t c1 = pressed?0xffffff:0x000000;
      uint32_t c2 = pressed?0x000000:c;

      HLH_gui_draw_rectangle(painter,e->bounds,c2,c1);
      HLH_gui_draw_string(painter,e->bounds,button->text,button->text_len,c1,1);
   }
   else if(msg==HLH_GUI_MSG_UPDATE)
   {
      HLH_gui_element_repaint(e,NULL);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return 30+HLH_GUI_GLYPH_WIDTH*button->text_len;
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return 25;
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(button->text);
   }

   return 0;
}

int hlh_gui_label_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_label *label = (HLH_gui_label *)e;

   if(msg==HLH_GUI_MSG_PAINT)
   {
      HLH_gui_draw_string(dp,e->bounds,label->text,label->text_len,0x000000,e->flags&HLH_GUI_LABEL_CENTER);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return HLH_GUI_GLYPH_WIDTH*label->text_len;
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT;
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(label->text);
   }

   return 0;
}

int hlh_gui_panel_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_panel *panel = (HLH_gui_panel *)e;
   int horizontal = panel->e.flags&HLH_GUI_PANEL_HORIZONTAL;

   if(msg==HLH_GUI_MSG_PAINT)
   {
      if(e->flags&HLH_GUI_PANEL_GRAY)
         HLH_gui_draw_block(dp,e->bounds,0xcccccc);
      else if(e->flags&HLH_GUI_PANEL_WHITE)
         HLH_gui_draw_block(dp,e->bounds,0xffffff);
   }
   else if(msg==HLH_GUI_MSG_LAYOUT)
   {
      hlh_gui_panel_layout(panel,e->bounds,0);
      HLH_gui_element_repaint(e,NULL);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return horizontal?hlh_gui_panel_layout(panel,HLH_gui_rect_make(0,0,0,di),1):hlh_gui_panel_measure(panel);
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return horizontal?hlh_gui_panel_measure(panel):hlh_gui_panel_layout(panel,HLH_gui_rect_make(0,di,0,0),1);
   }

   return 0;
}

int hlh_gui_panel_layout(HLH_gui_panel *p, HLH_gui_rect bounds, int measure)
{
   int horizontal = p->e.flags&HLH_GUI_PANEL_HORIZONTAL;
   int pos = horizontal?p->border.l:p->border.t;
   int space_h = bounds.r-bounds.l-p->border.r-p->border.l;
   int space_v = bounds.b-bounds.t-p->border.b-p->border.t;
   int available = horizontal?space_h:space_v;
   int fill = 0;
   int per_fill = 0;
   int count = 0;
   
   for(int i = 0;i<p->e.child_count;i++)
   {
      if(p->e.children[i]->flags&HLH_GUI_DESTROY)
         continue;

      count++;

      if(horizontal)
      {
         if(p->e.children[i]->flags&HLH_GUI_H_FILL)
            fill++;
         else if(available>0)
            available-=HLH_gui_element_msg(p->e.children[i],HLH_GUI_MSG_GET_WIDTH,space_v,NULL);
      }
      else
      {
         if(p->e.children[i]->flags&HLH_GUI_V_FILL)
            fill++;
         else if(available>0)
            available-=HLH_gui_element_msg(p->e.children[i],HLH_GUI_MSG_GET_HEIGHT,space_h,NULL);
      }
   }

   if(count)
      available-=(count-1)*p->gap;

   if(available>0&&fill)
      per_fill = available/fill;

   int border2 = horizontal?p->border.t:p->border.l;

   for(int i = 0;i<p->e.child_count;i++)
   {
      if(p->e.children[i]->flags&HLH_GUI_DESTROY)
         continue;

      HLH_gui_element *child = p->e.children[i];

      if(horizontal)
      {
         int height = (child->flags&HLH_GUI_V_FILL)?space_v:HLH_gui_element_msg(child,HLH_GUI_MSG_GET_HEIGHT,(child->flags&HLH_GUI_H_FILL)?per_fill:0,NULL);
         int width = (child->flags&HLH_GUI_H_FILL)?per_fill:HLH_gui_element_msg(child,HLH_GUI_MSG_GET_WIDTH,height,NULL);
         HLH_gui_rect r = HLH_gui_rect_make(pos+bounds.l,pos+width+bounds.l,border2+(space_v-height)/2+bounds.t,border2+(space_v+height)/2+bounds.t);
         if(!measure)
            HLH_gui_element_move(child,r,0);
         pos+=width+p->gap;
      }
      else
      {
         int width = (child->flags&HLH_GUI_H_FILL)?space_h:HLH_gui_element_msg(child,HLH_GUI_MSG_GET_WIDTH,(child->flags&HLH_GUI_V_FILL)?per_fill:0,NULL);
         int height = (child->flags&HLH_GUI_V_FILL)?per_fill:HLH_gui_element_msg(child,HLH_GUI_MSG_GET_HEIGHT,width,NULL);
         HLH_gui_rect r = HLH_gui_rect_make(border2+(space_h-width)/2+bounds.l,border2+(space_h+width)/2+bounds.l,pos+bounds.t,pos+height+bounds.t);
         if(!measure)
            HLH_gui_element_move(child,r,0);
         pos+=height+p->gap;
      }
   }

   return pos-(count?p->gap:0)+(horizontal?p->border.r:p->border.b);
}

int hlh_gui_panel_measure(HLH_gui_panel *p)
{
   int horizontal = p->e.flags&HLH_GUI_PANEL_HORIZONTAL;
   int size = 0;

   for(int i = 0;i<p->e.child_count;i++)
   {
      if(p->e.children[i]->flags&HLH_GUI_DESTROY)
         continue;

      int child_size = HLH_gui_element_msg(p->e.children[i],horizontal?HLH_GUI_MSG_GET_HEIGHT:HLH_GUI_MSG_GET_WIDTH,0,NULL);
      if(child_size>size)
         size = child_size;
   }

   int border = horizontal?p->border.t+p->border.b:p->border.l+p->border.r;
   return size+border;
}

int hlh_gui_element_destroy(HLH_gui_element *e)
{
   if(e->flags&HLH_GUI_DESTROY_DESCENDENT)
   {
      e->flags^=HLH_GUI_DESTROY_DESCENDENT;

      for(int i = 0;i<e->child_count;i++)
      {
         if(hlh_gui_element_destroy(e->children[i]))
         {
            memmove(&e->children[i],&e->children[i+1],sizeof(HLH_gui_element *)*(e->child_count-i-1));
            e->child_count--;
            i--;
         }
      }
   }

   if(e->flags&HLH_GUI_DESTROY)
   {
      HLH_gui_element_msg(e,HLH_GUI_MSG_DESTROY,0,NULL);

      if(e->window->pressed==e)
         hlh_gui_window_set_pressed(e->window,NULL,0);

      if(e->window->hover==e)
         e->window->hover = &e->window->e;

      free(e->children);
      free(e);
      return 1;
   }

   return 0;
}

#ifdef HLH_GUI_PLATTFORM_WIN32

LRESULT CALLBACK hlh_gui_window_procedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
   Window *window = GetWindowLongPtr(hwnd,GWLP_USERDATA);

   if(window==NULL)
      return DefWindowProc(hwnd,message,wparam,lparam);

   if(message==WM_CLOSE)
   {
      PostQuitMessage(0);
   }
   else if(message==WM_SIZE)
   {
      RECT client;
      GetClientRect(hwnd,&client);
      window->width = client.right;
      window->height = client.bottom;
      window->e.bounds = HLH_gui_rect_make(0,window->width,0,window->height);
      window->e.clip = HLH_gui_rect_make(0,window->width,0,window->height);
      HLH_gui_element_msg(&window->e,HLH_GUI_MSG_LAYOUT,0,NULL);
      hlh_gui_update();
   }
   else if(message=WM_PAINT)
   {
      PAINTSTRUCT paint;
      HDC dc = BeginPaint(hwnd,&paint);
      BITMAPINFOHEADER info = {0};
      info.biSize = sizeof(info);
      info.biWidth = window->width;
      info.biHeight = -window->height;
      info.biPlanes = 1;
      info.biBitCount = 32;
      StretchDIBits(dc,0,0,window->e.bounds.r-window->e.bounds.l,window->e.bounds.b-window->e.bounds.t, 
            0,0,window->e.bounds.r-window->e.bounds.l,window->e.bounds.b-window->e.bounds.t,
            window->bits,(BITMAPINFO *)&info,DIB_RGB_COLORS,SRCCOPY);
      EndPaint(hwnd,&paint);
   }
   else
   {
      return DefWindowProc(hwnd,message,wparam,lparam);
   }

   return 0;
}

void HLH_gui_init()
{
   WNDCLASS windowClass = {0};
   windowClass.lpfnWndProc = hlh_gui_window_procedure;
   windowClass.hCursor = LoadCursor(NULL,IDC_ARROW);
   windowClass.lpszClassName = "UILibraryTutorial";
   RegisterClass(&windowClass);
}

HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height)
{
   HLH_gui_window *window = (HLH_gui_window *)HLH_gui_element_create(sizeof(*window),NULL,0,hlh_gui_window_msg);
   window->e.window = window;
   hlh_gui_state.window_count++;
   hlh_gui_state.windows = realloc(hlh_gui_state.windows,sizeof(*hlh_gui_state.windows)*hlh_gui_state.window_count);
   hlh_gui_state.windows[hlh_gui_state_window_count-1] = window;

   window->hwnd = CreateWindow("UILibraryTutorial",title,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,width,height,NULL,NULL,NULL,NULL);
   SetWindowLongPtr(window->hwnd,GWLP_USERDATA,(LONG_PTR)window);
   ShowWindow(window->hwnd,SW_SHOW);
   PostMessage(window->hwnd,WM_SIZE,0,0);

   return window;
}

int HLH_gui_message_loop()
{
   MSG msg = {0};

   while(GetMessage(&msg,NULL,0,0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   return msg.wparam;
}

int hlh_gui_window_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_LAYOUT&&e->child_count!=0)
   {
      HLH_gui_element_move(e->children[0],e->bounds,0);
      HLH_gui_element_repaint(e,NULL);
   }

   return 0;
}

void hlh_gui_window_end_paint(HLH_gui_window *w, HLH_gui_painter *p)
{
}

#endif

#ifdef HLH_GUI_PLATTFORM_LINUX

HLH_gui_window *hlh_gui_find_window(X11Window window)
{
   for(uintptr_t i = 0;i<hlh_gui_state.window_count;i++)
      if(hlh_gui_state.windows[i]->window==window)
         return hlh_gui_state.windows[i];

   return NULL;
}

void HLH_gui_init()
{
   hlh_gui_state.display = XOpenDisplay(NULL);
   hlh_gui_state.visual = XDefaultVisual(hlh_gui_state.display,0);
   hlh_gui_state.window_closed_id = XInternAtom(hlh_gui_state.display,"WM_DELETE_WINDOW",0);
}

HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height)
{
   HLH_gui_window *window = (HLH_gui_window *)HLH_gui_element_create(sizeof(*window),NULL,0,hlh_gui_window_msg);
   window->e.window = window;
   window->hover = &window->e;
   hlh_gui_state.window_count++;
   hlh_gui_state.windows = realloc(hlh_gui_state.windows,sizeof(*hlh_gui_state.windows)*hlh_gui_state.window_count);
   hlh_gui_state.windows[hlh_gui_state.window_count-1] = window;

   XSetWindowAttributes attributes = {0};
   window->window = XCreateWindow(hlh_gui_state.display,DefaultRootWindow(hlh_gui_state.display),0,0,width,height,0,0,InputOutput,CopyFromParent,CWOverrideRedirect,&attributes);
   XStoreName(hlh_gui_state.display,window->window,title);
   XSelectInput(hlh_gui_state.display,window->window,SubstructureNotifyMask|ExposureMask|PointerMotionMask 
               |ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask|StructureNotifyMask
               |EnterWindowMask|LeaveWindowMask|ButtonMotionMask|KeymapStateMask|FocusChangeMask|PropertyChangeMask);
   XMapRaised(hlh_gui_state.display,window->window);
   XSetWMProtocols(hlh_gui_state.display,window->window,&hlh_gui_state.window_closed_id,1);
   window->image = XCreateImage(hlh_gui_state.display,hlh_gui_state.visual,24,ZPixmap,0,NULL,10,10,32,0);

   return window;
}

int HLH_gui_message_loop()
{
   hlh_gui_update();

   for(;;)
   {
      XEvent event;
      XNextEvent(hlh_gui_state.display,&event);

      if(event.type==ClientMessage&&(Atom)event.xclient.data.l[0]==hlh_gui_state.window_closed_id)
      {
         return 0;
      }
      else if(event.type==Expose)
      {
         HLH_gui_window *window = hlh_gui_find_window(event.xexpose.window);
         if(window==NULL)
            continue;
         XPutImage(hlh_gui_state.display,window->window,DefaultGC(hlh_gui_state.display,0), 
                   window->image,0,0,0,0,window->width,window->height);
      }
      else if(event.type==ConfigureNotify)
      {
         HLH_gui_window *window = hlh_gui_find_window(event.xconfigure.window);
         if(window==NULL)
            continue;

         if(window->width!=event.xconfigure.width||window->height!=event.xconfigure.height)
         {
            window->width = event.xconfigure.width;
            window->height = event.xconfigure.height;
            window->data = realloc(window->data,window->width*window->height*sizeof(*window->data));
            window->image->width = window->width;
            window->image->height = window->height;
            window->image->bytes_per_line = window->width*4;
            window->image->data = (char *)window->data;
            window->e.bounds = HLH_gui_rect_make(0,window->width,0,window->height);
            window->e.clip = HLH_gui_rect_make(0,window->width,0,window->height);
            HLH_gui_element_msg(&window->e,HLH_GUI_MSG_LAYOUT,0,NULL);
            hlh_gui_update();
         }
      }
      else if(event.type==MotionNotify)
      {
         HLH_gui_window *window = hlh_gui_find_window(event.xmotion.window);
         if(window==NULL)
            continue;
         window->mouse_x = event.xmotion.x;
         window->mouse_y = event.xmotion.y;
         hlh_gui_window_input_event(window,HLH_GUI_MSG_MOUSE_MOVE,0,NULL);
      }
      else if(event.type==LeaveNotify)
      {
         HLH_gui_window *window = hlh_gui_find_window(event.xcrossing.window);
         if(window==NULL)
            continue;

         if(window->pressed==NULL)
         {
            window->mouse_x = -1;
            window->mouse_y = -1;
         }

         hlh_gui_window_input_event(window,HLH_GUI_MSG_MOUSE_MOVE,0,NULL);
      }
      else if(event.type==ButtonPress||event.type==ButtonRelease)
      {
         HLH_gui_window *window = hlh_gui_find_window(event.xbutton.window);
         if(window==NULL)
            continue;

         window->mouse_x = event.xbutton.x;
         window->mouse_y = event.xbutton.y;

         if(event.xbutton.button>=1&&event.xbutton.button<=3)
         {
            HLH_gui_msg msg = HLH_GUI_MSG_LEFT_UP;
            switch(event.xbutton.button)
            {
            case 1: msg = event.type==ButtonPress?HLH_GUI_MSG_LEFT_DOWN:HLH_GUI_MSG_LEFT_UP; break;
            case 2: msg = event.type==ButtonPress?HLH_GUI_MSG_MIDDLE_DOWN:HLH_GUI_MSG_MIDDLE_UP; break;
            case 3: msg = event.type==ButtonPress?HLH_GUI_MSG_RIGHT_DOWN:HLH_GUI_MSG_RIGHT_UP; break;
            }

            hlh_gui_window_input_event(window,msg,0,NULL);
         }
      }
   }
}

int hlh_gui_window_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_DESTROY)
   {
      HLH_gui_window *window = (HLH_gui_window *)e;
      free(window->data);
      window->image->data = NULL;
      XDestroyImage(window->image);
      XDestroyWindow(hlh_gui_state.display,window->window);
   }
   else if(msg==HLH_GUI_MSG_LAYOUT&&e->child_count!=0)
   {
      HLH_gui_element_move(e->children[0],e->bounds,0);
      HLH_gui_element_repaint(e,NULL);
   }

   return 0;
}

void hlh_gui_window_end_paint(HLH_gui_window *w, HLH_gui_painter *p)
{
   XPutImage(hlh_gui_state.display,w->window,DefaultGC(hlh_gui_state.display,0),w->image, 
             w->update_region.l,w->update_region.t,w->update_region.l,w->update_region.t,
             w->update_region.r-w->update_region.l,w->update_region.b-w->update_region.t);
}

#endif

#endif
#endif
