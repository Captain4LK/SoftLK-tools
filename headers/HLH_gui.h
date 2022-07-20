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
   HLH_GUI_MSG_USR,
}HLH_gui_msg;

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

struct HLH_gui_window
{
   HLH_gui_element e;
   uint32_t *data;
   int width;
   int height;
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

HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height);
HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint32_t flags, HLH_gui_msg_handler msg_handler);

int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
void HLH_gui_element_move(HLH_gui_element *e, HLH_gui_rect bounds, int always_layout);
void HLH_gui_element_repaint(HLH_gui_element *e, HLH_gui_rect *region);

void HLH_gui_draw_block(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color);
void HLH_gui_draw_rectangle(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color_fill, uint32_t color_border);

HLH_gui_rect HLH_gui_rect_make(int l, int r, int t, int b);
HLH_gui_rect HLH_gui_rect_intersect(HLH_gui_rect a, HLH_gui_rect b);
HLH_gui_rect HLH_gui_rect_bounding(HLH_gui_rect a, HLH_gui_rect b);
int HLH_gui_rect_valid(HLH_gui_rect r);
int HLH_gui_rect_equal(HLH_gui_rect a, HLH_gui_rect b);
int HLH_gui_rect_inside(HLH_gui_rect a, int x, int y);

void HLH_gui_string_copy(char **dest, size_t *dest_size, const char *src, ptrdiff_t src_size);

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

void HLH_gui_string_copy(char **dest, size_t *dest_size, const char *src, ptrdiff_t src_size)
{
   if(src_size==-1)
      src_size = strlen(src);

   *dest = realloc(*dest,src_size);
   *dest_size = src_size;
   memcpy(*dest,src,src_size);
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

int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
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

      if(HLH_gui_rect_valid(win->update_region))
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
   }
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
   XPutImage(hlh_gui_state.display,w->window,DefaultGC(hlh_gui_state.display,0),w->image, 
             w->update_region.l,w->update_region.t,w->update_region.l,w->update_region.t,
             w->update_region.r-w->update_region.l,w->update_region.b-w->update_region.t);
}

#endif

#endif
#endif
