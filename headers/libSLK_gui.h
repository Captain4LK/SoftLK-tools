#ifndef _LIBSLK_GUI_H_

/*
libSLK_gui - single header edition

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#define _LIBSLK_H_

typedef struct SLK_gui_element SLK_gui_element;

typedef struct
{
   int x;
   int y;
   int w;
   int h;
}SLK_gui_rectangle;

typedef enum 
{
   SLK_GUI_ELEMENT_BUTTON, SLK_GUI_ELEMENT_LABEL,
   SLK_GUI_ELEMENT_ICON, SLK_GUI_ELEMENT_SLIDER,
   SLK_GUI_ELEMENT_IMAGE, SLK_GUI_ELEMENT_TABBAR,
   SLK_GUI_ELEMENT_VTABBAR,
}SLK_gui_element_type;

typedef struct
{
   SLK_gui_rectangle pos;
   SLK_Button state;
   char text[256];
   int text_x;
   int selected;
}SLK_gui_button;

typedef struct
{
   SLK_gui_rectangle pos;
   char text[512];
   int text_x;
}SLK_gui_label;

typedef struct
{
   SLK_gui_rectangle pos;
   SLK_gui_rectangle frames[2];
   SLK_RGB_sprite *sprite;
   SLK_Button state;
   int selected;
}SLK_gui_icon;

typedef struct
{
   SLK_gui_rectangle pos;
   int value; //Supposed to be accessed by user
   int min;
   int max;
   int selected;
}SLK_gui_slider;

typedef struct
{
   SLK_gui_rectangle pos;
   SLK_RGB_sprite *sprite;
}SLK_gui_image;

typedef struct
{
   SLK_gui_rectangle pos;
   int current_tab;
   int tabs;
   char **tabs_text;
   int *tabs_text_x;

   SLK_gui_element **elements;
}SLK_gui_tabbar;

typedef struct
{
   SLK_gui_rectangle pos;
   int current_tab;
   int tabs;
   char **tabs_text;
   int *tabs_text_x;

   SLK_gui_element **elements;
}SLK_gui_vtabbar;

struct SLK_gui_element
{
   SLK_gui_element_type type;

   union
   {
      SLK_gui_button button;
      SLK_gui_label label;
      SLK_gui_icon icon;
      SLK_gui_slider slider;
      SLK_gui_image image;
      SLK_gui_tabbar tabbar;
      SLK_gui_vtabbar vtabbar;
   };

   SLK_gui_element *next;
};

typedef struct
{
   SLK_gui_rectangle pos;
   SLK_gui_rectangle posm;
   int moveable;
   int locked;
   char title[256];
   int title_x;

   SLK_gui_element *elements;
}SLK_gui_window;

SLK_gui_window  *SLK_gui_window_create(int x, int y, int width, int height); //Position can be changed, width and height are constants
void             SLK_gui_window_destroy(SLK_gui_window *w);
void             SLK_gui_window_set_title(SLK_gui_window *w, const char *title);
void             SLK_gui_window_draw(const SLK_gui_window *w);
void             SLK_gui_window_set_moveable(SLK_gui_window *w, int move);
void             SLK_gui_window_update_input(SLK_gui_window *w, SLK_Button button_left, SLK_Button button_right, int cursor_x, int cursor_y);
void             SLK_gui_window_add_element(SLK_gui_window *w, SLK_gui_element *e);

SLK_gui_element *SLK_gui_button_create(int x, int y, int width, int height, const char *label);
SLK_gui_element *SLK_gui_icon_create(int x, int y, int width, int height, SLK_RGB_sprite *sprite, SLK_gui_rectangle frame_up, SLK_gui_rectangle frame_down);
SLK_gui_element *SLK_gui_label_create(int x, int y, int width, int height, const char *label);
void             SLK_gui_label_set_text(SLK_gui_element *element, const char *label);
SLK_gui_element *SLK_gui_slider_create(int x, int y, int width, int height, int min, int max);
SLK_gui_element *SLK_gui_image_create(int x, int y, int width, int height, SLK_RGB_sprite *sprite, SLK_gui_rectangle frame);
void             SLK_gui_image_update(SLK_gui_element *element, SLK_RGB_sprite *sprite, SLK_gui_rectangle frame);
SLK_gui_element *SLK_gui_tabbar_create(int x, int y, int width, int height, int tab_count, const char **tabs_text);
void             SLK_gui_tabbar_add_element(SLK_gui_element *bar, int tab, SLK_gui_element *element_new);
SLK_gui_element *SLK_gui_vtabbar_create(int x, int y, int width, int tab_count, const char **tabs_text);
void             SLK_gui_vtabbar_add_element(SLK_gui_element *bar, int tab, SLK_gui_element *element_new);

void             SLK_gui_set_font(SLK_RGB_sprite *f);
SLK_RGB_sprite  *SLK_gui_get_font();
void             SLK_gui_set_font_color(SLK_Color color);
void             SLK_gui_set_colors(SLK_Color c0, SLK_Color c1, SLK_Color c2, SLK_Color c3, SLK_Color c4);

#endif //_LIBSLK_GUI_H_

#ifdef LIBSLK_GUI_IMPLEMENTATION
#ifndef LIBSLK_GUI_IMPLEMENTATION_ONCE
#define LIBSLK_GUI_IMPLEMENTATION_ONCE

#define INSIDE(x,y,rx,ry,rw,rh) \
   ((x)>(rx)&&(x)<(rx)+(rw)&&(y)>(ry)&&(y)<(ry)+(rh))

#define MIN(a,b) \
   ((a)<(b)?(a):(b))

#define MAX(a,b) \
   ((a)>(b)?(a):(b))

#define MALLOC_CHECK(m,s) \
   if((m)==NULL) \
      SLK_error("malloc of size %zu failed, out of memory!",(s))

static SLK_RGB_sprite *slk_gui_font;
static SLK_Color slk_gui_color_0;
static SLK_Color slk_gui_color_1;
static SLK_Color slk_gui_color_2;
static SLK_Color slk_gui_color_3;
static SLK_Color slk_gui_color_4;
static SLK_Color slk_gui_color_5;

static int slk_gui_clip_text(char *dst, const char *src, int dst_size, SLK_gui_rectangle area);
static void slk_gui_draw(const SLK_gui_window *w, SLK_gui_element *elements);
static void slk_gui_input(SLK_gui_window *w, SLK_gui_element *elements, SLK_Button button_left, SLK_Button button_right, int cursor_x, int cursor_y);

void SLK_gui_set_font(SLK_RGB_sprite *f)
{
   slk_gui_font = f;
}

SLK_RGB_sprite *SLK_gui_get_font()
{
   return slk_gui_font;
}

void SLK_gui_set_colors(SLK_Color c0, SLK_Color c1, SLK_Color c2, SLK_Color c3, SLK_Color c4)
{
   slk_gui_color_0 = c0;
   slk_gui_color_1 = c1;
   slk_gui_color_2 = c2;
   slk_gui_color_3 = c3;
   slk_gui_color_4 = c4;
}

void SLK_gui_set_font_color(SLK_Color color)
{
   slk_gui_color_5 = color;
} 

//Internal
static int slk_gui_clip_text(char *dst, const char *src, int dst_size, SLK_gui_rectangle area)
{
   int text_max_x = area.w/8;
   int text_max_y = area.h/8;
   int c = 0;
   int ci = 0;
   int x = 0;
   int y = 0;

   while(y<text_max_y)
   {
      x = 0;
      while(x<text_max_x)
      {
         if(src[ci]!='\n'&&src[ci]!='\0')
            dst[c] = src[ci];
         else if(src[ci]=='\n')
         {
            ci++;
            break;
         }
         else
            break;
         x++;
         c++;
         ci++;
      }
      dst[c] = '\n';
      c++;
      y++;
   }
   dst[c] = '\0';

   if(text_max_y>1) //Align left
      return 0;

   //Algin center (only applies to first line)
   int text_x = strlen(dst)-1;

   return (area.w-text_x*8)/2;
}

SLK_gui_element *SLK_gui_button_create(int x, int y, int width, int height, const char *label)
{
   SLK_gui_element *e = backend_malloc(sizeof(*e));
   MALLOC_CHECK(e,sizeof(*e));

   e->next = NULL;
   e->type = SLK_GUI_ELEMENT_BUTTON;
   e->button.pos.x = x;
   e->button.pos.y = y;
   e->button.pos.w = width;
   e->button.pos.h = height;
   e->button.state.held = 0;
   e->button.state.pressed = 0;
   e->button.state.released = 0;
   e->button.text_x = 2+slk_gui_clip_text(e->button.text,label,255,(SLK_gui_rectangle){.x = 2,.y = 2,.w = width-4,.h = height-4});
   e->button.selected = 0;

   return e;
}

SLK_gui_element *SLK_gui_label_create(int x, int y, int width, int height, const char *label)
{
   SLK_gui_element *e = backend_malloc(sizeof(*e));
   MALLOC_CHECK(e,sizeof(*e));

   e->next = NULL;
   e->type = SLK_GUI_ELEMENT_LABEL;
   e->label.pos.x = x;
   e->label.pos.y = y;
   e->label.pos.w = width;
   e->label.pos.h = height;
   e->label.text_x = 2+slk_gui_clip_text(e->label.text,label,255,(SLK_gui_rectangle){.x = 2,.y = 2,.w = width-4,.h = height-4});

   return e;
}

void SLK_gui_label_set_text(SLK_gui_element *element, const char *label)
{
   if(element->type!=SLK_GUI_ELEMENT_LABEL)
      return;

   element->label.text_x = 2+slk_gui_clip_text(element->label.text,label,255,(SLK_gui_rectangle){.x = 2,.y = 2,.w = element->label.pos.w-4,.h = element->label.pos.h-4});
}

SLK_gui_element *SLK_gui_icon_create(int x, int y, int width, int height, SLK_RGB_sprite *sprite, SLK_gui_rectangle frame_up, SLK_gui_rectangle frame_down)
{
   SLK_gui_element *e = backend_malloc(sizeof(*e));
   MALLOC_CHECK(e,sizeof(*e));

   e->next = NULL;
   e->type = SLK_GUI_ELEMENT_ICON;
   e->icon.sprite = sprite;
   e->icon.pos.x = x;
   e->icon.pos.y = y;
   e->icon.pos.w = width;
   e->icon.pos.h = height;
   e->icon.frames[0] = frame_up;
   e->icon.frames[1] = frame_down;
   e->icon.state.held = 0;
   e->icon.state.pressed = 0;
   e->icon.state.released = 0;
   e->icon.selected = 0;

   return e;
}

SLK_gui_element *SLK_gui_slider_create(int x, int y, int width, int height, int min, int max)
{
   SLK_gui_element *e = backend_malloc(sizeof(*e));
   MALLOC_CHECK(e,sizeof(*e));

   e->next = NULL;
   e->type = SLK_GUI_ELEMENT_SLIDER;
   e->slider.pos.x = x;
   e->slider.pos.y = y;
   e->slider.pos.w = width;
   e->slider.pos.h = height;
   e->slider.min = min;
   e->slider.max = max;
   e->slider.value = min;
   e->slider.selected = 0;
   
   return e;
}

SLK_gui_element *SLK_gui_image_create(int x, int y, int width, int height, SLK_RGB_sprite *sprite, SLK_gui_rectangle frame)
{
   SLK_gui_element *e = backend_malloc(sizeof(*e));
   MALLOC_CHECK(e,sizeof(*e));

   e->next = NULL;
   e->type = SLK_GUI_ELEMENT_IMAGE;
   e->image.sprite = SLK_rgb_sprite_create(width,height);
   e->image.pos.x = x;
   e->image.pos.y = y;
   e->image.pos.w = width;
   e->image.pos.h = height;
   SLK_gui_image_update(e,sprite,frame);

   return e;
}

void SLK_gui_image_update(SLK_gui_element *element, SLK_RGB_sprite *sprite, SLK_gui_rectangle frame)
{
   //Sample image
   int width = element->image.pos.w;
   int height = element->image.pos.h;
   SLK_RGB_sprite *old = SLK_draw_rgb_get_target();
   SLK_draw_rgb_set_target(element->image.sprite);
   SLK_draw_rgb_set_clear_color(slk_gui_color_0);
   SLK_draw_rgb_clear();

   //Special case: Image fits perfectly
   if(frame.w==width&&frame.h<=height)
   {
      int iy = (height-frame.h)/2; 
      SLK_rgb_sprite_copy_partial(element->image.sprite,sprite,0,iy,frame.x,frame.y,frame.w,frame.h);
   }
   else if(frame.h==height&&frame.w<=width)
   {
      int ix = (width-frame.w)/2; 
      SLK_rgb_sprite_copy_partial(element->image.sprite,sprite,ix,0,frame.x,frame.y,frame.w,frame.h);
   }
   else
   {
      int fwidth;
      int fheight;
      if(frame.w>frame.h)
      {
         fwidth = width;
         fheight = ((float)frame.h/(float)frame.w)*height;
      }
      else
      {
         fheight = height;
         fwidth = ((float)frame.w/(float)frame.h)*width;
      }
      int ix = (width-fwidth)/2;
      int iy = (height-fheight)/2;
      float fw = (float)sprite->width/(float)fwidth;
      float fh = (float)sprite->height/(float)fheight;

      for(int sx = 0;sx<fwidth;sx++)
      {
         for(int sy = 0;sy<fheight;sy++)
         {
            SLK_Color c = SLK_rgb_sprite_get_pixel(sprite,fw*sx,fh*sy);
            SLK_rgb_sprite_set_pixel(element->image.sprite,sx+ix,sy+iy,c);
         }
      }
   }

   SLK_draw_rgb_set_target(old);
}

SLK_gui_element *SLK_gui_tabbar_create(int x, int y, int width, int height, int tab_count, const char **tabs_text)
{
   int tab_width = width/tab_count;
   SLK_gui_element *e = backend_malloc(sizeof(*e));
   MALLOC_CHECK(e,sizeof(*e));

   e->next = NULL;
   e->type = SLK_GUI_ELEMENT_TABBAR;
   e->tabbar.elements = backend_malloc(sizeof(*e->tabbar.elements)*tab_count);
   MALLOC_CHECK(e->tabbar.elements,sizeof(*e->tabbar.elements)*tab_count);

   e->tabbar.pos.x = x;
   e->tabbar.pos.y = y;
   e->tabbar.pos.w = width;
   e->tabbar.pos.h = height;
   e->tabbar.current_tab = 0;
   e->tabbar.tabs = tab_count;
   e->tabbar.tabs_text = backend_malloc(sizeof(*e->tabbar.tabs_text)*tab_count);
   MALLOC_CHECK(e->tabbar.tabs_text,sizeof(*e->tabbar.tabs_text)*tab_count);
   e->tabbar.tabs_text_x = backend_malloc(sizeof(*e->tabbar.tabs_text_x)*tab_count);
   MALLOC_CHECK(e->tabbar.tabs_text_x,sizeof(*e->tabbar.tabs_text_x)*tab_count);
   for(int i = 0;i<tab_count;i++)
   {
      e->tabbar.elements[i] = NULL;
      e->tabbar.tabs_text[i] = backend_malloc(sizeof(**e->tabbar.tabs_text)*256);
      MALLOC_CHECK(e->tabbar.tabs_text[i],sizeof(**e->tabbar.tabs_text)*256);
      e->tabbar.tabs_text_x[i] = slk_gui_clip_text(e->tabbar.tabs_text[i],tabs_text[i],256,(SLK_gui_rectangle){0,0,tab_width,height})+i*tab_width;
   }

   return e;
}

void SLK_gui_tabbar_add_element(SLK_gui_element *bar, int tab, SLK_gui_element *element_new)
{
   element_new->next = bar->tabbar.elements[tab];
   bar->tabbar.elements[tab] = element_new;
}

SLK_gui_element *SLK_gui_vtabbar_create(int x, int y, int width, int tab_count, const char **tabs_text)
{
   int tab_width = width;
   SLK_gui_element *e = backend_malloc(sizeof(*e));
   MALLOC_CHECK(e,sizeof(*e));

   e->next = NULL;
   e->type = SLK_GUI_ELEMENT_VTABBAR;
   e->vtabbar.elements = backend_malloc(sizeof(*e->vtabbar.elements)*tab_count);;
   MALLOC_CHECK(e->vtabbar.elements,sizeof(*e->vtabbar.elements)*tab_count);

   e->vtabbar.pos.x = x;
   e->vtabbar.pos.y = y;
   e->vtabbar.pos.w = width;
   e->vtabbar.pos.h = 14*tab_count;
   e->vtabbar.current_tab = 0;
   e->vtabbar.tabs = tab_count;
   e->vtabbar.tabs_text = backend_malloc(sizeof(*e->vtabbar.tabs_text)*tab_count);
   MALLOC_CHECK(e->vtabbar.tabs_text,sizeof(*e->vtabbar.tabs_text)*tab_count);
   e->vtabbar.tabs_text_x = backend_malloc(sizeof(*e->vtabbar.tabs_text_x)*tab_count);
   MALLOC_CHECK(e->vtabbar.tabs_text_x,sizeof(*e->vtabbar.tabs_text_x)*tab_count);

   for(int i = 0;i<tab_count;i++)
   {
      e->vtabbar.elements[i] = NULL;
      e->vtabbar.tabs_text[i] = backend_malloc(sizeof(**e->vtabbar.tabs_text)*256);
      MALLOC_CHECK(e->vtabbar.tabs_text[i],sizeof(**e->vtabbar.tabs_text)*256);
      e->vtabbar.tabs_text_x[i] = slk_gui_clip_text(e->vtabbar.tabs_text[i],tabs_text[i],256,(SLK_gui_rectangle){0,0,tab_width,14});
   }

   return e;
}

void SLK_gui_vtabbar_add_element(SLK_gui_element *bar, int tab, SLK_gui_element *element_new)
{
   element_new->next = bar->vtabbar.elements[tab];
   bar->vtabbar.elements[tab] = element_new;
}

SLK_gui_window *SLK_gui_window_create(int x, int y, int width, int height)
{
   SLK_gui_window *w = backend_malloc(sizeof(*w));
   MALLOC_CHECK(w,sizeof(*w));

   w->pos.x = x;
   w->pos.y = y;
   w->pos.w = width;
   w->posm.w = width;
   w->pos.h = height;
   w->posm.h = height;
   w->title[0] = '\0';
   w->moveable = 0;
   w->elements = NULL;
   w->locked = 0;

   return w;
}

void SLK_gui_window_destroy(SLK_gui_window *w)
{
   backend_free(w);
}

void SLK_gui_window_set_title(SLK_gui_window *w, const char *title)
{
   w->title_x = 2+slk_gui_clip_text(w->title,title,255,(SLK_gui_rectangle){.x = 2,.y = 2,.w = w->pos.w-4,.h = 8});
}

void SLK_gui_window_draw(const SLK_gui_window *w)
{
   ///Draw frame and fill area
   SLK_draw_rgb_fill_rectangle(w->pos.x+1,w->pos.y+1,w->pos.w-2,w->pos.h-2,slk_gui_color_0);
   SLK_draw_rgb_vertical_line(w->pos.x,w->pos.y+1,w->pos.y+w->pos.h,slk_gui_color_3);
   SLK_draw_rgb_vertical_line(w->pos.x+w->pos.w-1,w->pos.y,w->pos.y+w->pos.h-1,slk_gui_color_1);
   SLK_draw_rgb_horizontal_line(w->pos.x+1,w->pos.x+w->pos.w-1,w->pos.y,slk_gui_color_1);
   SLK_draw_rgb_horizontal_line(w->pos.x+1,w->pos.x+w->pos.w-1,w->pos.y+w->pos.h-1,slk_gui_color_3);
   SLK_draw_rgb_color(w->pos.x,w->pos.y,slk_gui_color_2);
   SLK_draw_rgb_color(w->pos.x+w->pos.w-1,w->pos.y+w->pos.h-1,slk_gui_color_2);

   //Draw title bar
   SLK_draw_rgb_fill_rectangle(w->pos.x+2,w->pos.y+2,w->pos.w-4,10,slk_gui_color_1);
   SLK_draw_rgb_horizontal_line(w->pos.x+1,w->pos.x+w->pos.w-1,w->pos.y+12,slk_gui_color_3);
   SLK_draw_rgb_set_font_sprite(slk_gui_font);
   SLK_draw_rgb_string(w->pos.x+w->title_x,w->pos.y+3,1,w->title,slk_gui_color_5);

   //Draw elements
   slk_gui_draw(w,w->elements);
   
   //Draw moving preview
   if(w->moveable==2)
      SLK_draw_rgb_rectangle(w->posm.x,w->posm.y,w->pos.w,w->pos.h,slk_gui_color_1);
}

void SLK_gui_window_set_moveable(SLK_gui_window *w, int move)
{
   w->moveable = move;
}

void SLK_gui_window_update_input(SLK_gui_window *w, SLK_Button button_left, SLK_Button button_right, int cursor_x, int cursor_y)
{
   //Check if cursor is in window to begin with
   //goto prevents another useless if indentation
   if(!INSIDE(cursor_x,cursor_y,w->pos.x,w->pos.y,w->pos.w,w->pos.h))
      goto OUTSIDE;

   if(button_left.pressed)
   {
      //Title bar --> move window
      if(w->moveable==1&&INSIDE(cursor_x,cursor_y,w->pos.x+2,w->pos.y,w->pos.w-2,12))
      {
         w->posm.w = cursor_x-w->pos.x;
         w->posm.h = cursor_y-w->pos.y;
         w->posm.x = w->pos.x+w->posm.w;
         w->posm.y = w->pos.y+w->posm.h;
         w->moveable = 2;
      }
   }

   //Check all buttons
   if(w->moveable!=2)
      slk_gui_input(w,w->elements,button_left,button_right,cursor_x,cursor_y);

OUTSIDE:

   if(w->moveable==2)
   {
      if(button_left.held)
      {
         w->posm.x = -w->posm.w+cursor_x;
         w->posm.y = -w->posm.h+cursor_y;
      }
      else
      {
         w->moveable = 1;
         w->pos.x = w->posm.x;
         w->pos.y = w->posm.y;
      }
   }
}

void SLK_gui_window_add_element(SLK_gui_window *w, SLK_gui_element *e)
{
   e->next = w->elements;
   w->elements = e;
}

static void slk_gui_draw(const SLK_gui_window *w, SLK_gui_element *elements)
{
   SLK_gui_element *e = elements;
   while(e)
   {
      switch(e->type)
      {
      case SLK_GUI_ELEMENT_BUTTON:
         SLK_draw_rgb_rectangle(e->button.pos.x+w->pos.x,e->button.pos.y+w->pos.y,e->button.pos.w,e->button.pos.h,slk_gui_color_4);
         if(e->button.state.held)
         {
            SLK_draw_rgb_vertical_line(e->button.pos.x+1+w->pos.x,e->button.pos.y+2+w->pos.y,e->button.pos.y+2+e->button.pos.h+w->pos.y-3,slk_gui_color_4);
            SLK_draw_rgb_vertical_line(e->button.pos.x+1+e->button.pos.w-3+w->pos.x,e->button.pos.y+1+w->pos.y,e->button.pos.y+2+e->button.pos.h+w->pos.y-4,slk_gui_color_3);
            SLK_draw_rgb_horizontal_line(e->button.pos.x+2+w->pos.x,e->button.pos.x+e->button.pos.w-2+w->pos.x,e->button.pos.y+1+w->pos.y,slk_gui_color_3);
            SLK_draw_rgb_horizontal_line(e->button.pos.x+2+w->pos.x,e->button.pos.x+e->button.pos.w-2+w->pos.x,e->button.pos.y+1+e->button.pos.h-3+w->pos.y,slk_gui_color_4);
         }
         else
         {
            SLK_draw_rgb_vertical_line(e->button.pos.x+1+w->pos.x,e->button.pos.y+2+w->pos.y,e->button.pos.y+2+e->button.pos.h+w->pos.y-3,slk_gui_color_3);
            SLK_draw_rgb_vertical_line(e->button.pos.x+1+e->button.pos.w-3+w->pos.x,e->button.pos.y+1+w->pos.y,e->button.pos.y+2+e->button.pos.h+w->pos.y-4,slk_gui_color_1);
            SLK_draw_rgb_horizontal_line(e->button.pos.x+2+w->pos.x,e->button.pos.x+e->button.pos.w-2+w->pos.x,e->button.pos.y+1+w->pos.y,slk_gui_color_1);
            SLK_draw_rgb_horizontal_line(e->button.pos.x+2+w->pos.x,e->button.pos.x+e->button.pos.w-2+w->pos.x,e->button.pos.y+1+e->button.pos.h-3+w->pos.y,slk_gui_color_3);
         }
         SLK_draw_rgb_string(e->button.pos.x+e->button.text_x+w->pos.x,e->button.pos.y+w->pos.y+3,1,e->button.text,slk_gui_color_5);
         break;
      case SLK_GUI_ELEMENT_LABEL:
         SLK_draw_rgb_string(e->label.pos.x+e->label.text_x+w->pos.x,e->label.pos.y+w->pos.y,1,e->label.text,slk_gui_color_5);
         break;
      case SLK_GUI_ELEMENT_ICON:
         if(e->icon.state.held)
            SLK_draw_rgb_sprite_partial(e->icon.sprite,e->icon.pos.x+w->pos.x,e->icon.pos.y+w->pos.y,e->icon.frames[1].x,e->icon.frames[1].y,e->icon.frames[1].w,e->icon.frames[1].h);
         else
            SLK_draw_rgb_sprite_partial(e->icon.sprite,e->icon.pos.x+w->pos.x,e->icon.pos.y+w->pos.y,e->icon.frames[0].x,e->icon.frames[0].y,e->icon.frames[0].w,e->icon.frames[0].h);
         break;
      case SLK_GUI_ELEMENT_SLIDER:
            SLK_draw_rgb_fill_rectangle(e->slider.pos.x+w->pos.x,e->slider.pos.y+w->pos.y,e->slider.pos.w,e->slider.pos.h,slk_gui_color_4);
            float t = ((float)(e->slider.value-e->slider.min)/(float)(e->slider.max-e->slider.min));
            if(e->slider.pos.w>e->slider.pos.h)
               SLK_draw_rgb_vertical_line(w->pos.x+e->slider.pos.x+t*(e->slider.pos.w-1),e->slider.pos.y+w->pos.y+1,e->slider.pos.y+e->slider.pos.h+w->pos.y-1,slk_gui_color_1);
            else
               SLK_draw_rgb_horizontal_line(e->slider.pos.x+w->pos.x+1,e->slider.pos.x+e->slider.pos.w+w->pos.x-1,w->pos.y+e->slider.pos.y+e->slider.pos.h-1-t*(e->slider.pos.h-1),slk_gui_color_1); 
         break;
      case SLK_GUI_ELEMENT_IMAGE:
         SLK_draw_rgb_sprite(e->image.sprite,e->image.pos.x+w->pos.x,e->image.pos.y+w->pos.y);
         break;
      case SLK_GUI_ELEMENT_TABBAR:
         for(int i = 0;i<e->tabbar.tabs;i++)
         {
            int tab_width = e->tabbar.pos.w/e->tabbar.tabs;
            if(i==e->tabbar.current_tab)
               SLK_draw_rgb_fill_rectangle(e->tabbar.pos.x+i*tab_width+w->pos.x,e->tabbar.pos.y+w->pos.y,tab_width,e->tabbar.pos.h,slk_gui_color_2);
            else
               SLK_draw_rgb_fill_rectangle(e->tabbar.pos.x+i*tab_width+w->pos.x,e->tabbar.pos.y+w->pos.y,tab_width,e->tabbar.pos.h,slk_gui_color_3);
            SLK_draw_rgb_string(e->tabbar.pos.x+w->pos.x+e->tabbar.tabs_text_x[i],e->tabbar.pos.y+w->pos.y+3,1,e->tabbar.tabs_text[i],slk_gui_color_5);
         }
         if(e->tabbar.elements[e->tabbar.current_tab]!=NULL)
            slk_gui_draw(w,e->tabbar.elements[e->tabbar.current_tab]);
         break;
      case SLK_GUI_ELEMENT_VTABBAR:
         for(int i = 0;i<e->vtabbar.tabs;i++)
         {
            int tab_width = e->vtabbar.pos.w;
            int tab_height = e->vtabbar.pos.h/e->vtabbar.tabs;
            if(i==e->vtabbar.current_tab)
               SLK_draw_rgb_fill_rectangle(e->vtabbar.pos.x+w->pos.x,e->vtabbar.pos.y+w->pos.y+i*14,tab_width,tab_height,slk_gui_color_2);
            else
               SLK_draw_rgb_fill_rectangle(e->vtabbar.pos.x+w->pos.x,e->vtabbar.pos.y+w->pos.y+i*14,tab_width,tab_height,slk_gui_color_3);
            SLK_draw_rgb_string(e->vtabbar.pos.x+w->pos.x+e->vtabbar.tabs_text_x[i],e->vtabbar.pos.y+w->pos.y+3+i*14,1,e->vtabbar.tabs_text[i],slk_gui_color_5);
         }
         if(e->vtabbar.elements[e->vtabbar.current_tab]!=NULL) 
            slk_gui_draw(w,e->vtabbar.elements[e->vtabbar.current_tab]);
         break;
      }
      e = e->next;
   }
}

static void slk_gui_input(SLK_gui_window *w, SLK_gui_element *elements, SLK_Button button_left, SLK_Button button_right, int cursor_x, int cursor_y)
{
   SLK_gui_element *e = elements;
   while(e)
   {
      if(e->type==SLK_GUI_ELEMENT_BUTTON&&(!w->locked||e->button.selected))
      {
         int status = 0;
         if(button_left.held||button_right.held)
         {
            if((status = INSIDE(cursor_x,cursor_y,w->pos.x+e->button.pos.x,w->pos.y+e->button.pos.y,e->button.pos.w,e->button.pos.h)))
            {
               w->locked = 1;
               e->button.selected = 1;
            }
         }
         else
         {
            e->button.selected = 0;
            w->locked = 0;
            status = 0;
         }
         e->button.state.pressed = !e->button.state.held&&status;
         e->button.state.released = e->button.state.held&&!status;
         e->button.state.held = status;
      }
      else if(e->type==SLK_GUI_ELEMENT_ICON&&(!w->locked||e->icon.selected))
      {
         int status = 0;
         if(button_left.held||button_right.held)
         {
            if((status = INSIDE(cursor_x,cursor_y,w->pos.x+e->icon.pos.x,w->pos.y+e->icon.pos.y,e->icon.pos.w,e->icon.pos.h)))
            {
               w->locked = 1;
               e->icon.selected = 1;
            }
         }
         else
         {
            e->icon.selected = 0;
            w->locked = 0;
            status = 0;
         }
         e->icon.state.pressed = !e->icon.state.held&&status;
         e->icon.state.released = e->icon.state.held&&!status;
         e->icon.state.held = status;
      }
      else if(e->type==SLK_GUI_ELEMENT_SLIDER)
      {
         if(!w->locked&&INSIDE(cursor_x,cursor_y,w->pos.x+e->slider.pos.x,w->pos.y+e->slider.pos.y,e->slider.pos.w,e->slider.pos.h)&&button_left.held)
         {
            e->slider.selected = 1;
            w->locked = 1;
         }
         else if(!button_left.held)
         {
            e->slider.selected = 0;
            w->locked = 0;
         }

         if(e->slider.selected)
         {
            if(e->slider.pos.w>e->slider.pos.h)
            {
               float t = ((float)(cursor_x-w->pos.x-e->slider.pos.x)/(float)e->slider.pos.w);
               e->slider.value = e->slider.min+t*(e->slider.max-e->slider.min);
               e->slider.value = MAX(e->slider.min,MIN(e->slider.value,e->slider.max));
            }
            else
            {
               float t = ((float)(cursor_y-w->pos.y-e->slider.pos.y)/(float)e->slider.pos.h);
               e->slider.value = e->slider.max-(e->slider.min+t*(e->slider.max-e->slider.min));
               e->slider.value = MAX(e->slider.min,MIN(e->slider.max,e->slider.value));
            }
         }
      }
      else if(e->type==SLK_GUI_ELEMENT_TABBAR)
      {
         if(button_left.pressed&&INSIDE(cursor_x,cursor_y,w->pos.x+e->tabbar.pos.x,w->pos.y+e->tabbar.pos.y,e->tabbar.pos.w,e->tabbar.pos.h))
            e->tabbar.current_tab = (cursor_x-w->pos.x-e->tabbar.pos.x)/(e->tabbar.pos.w/e->tabbar.tabs);
         if(e->tabbar.elements[e->tabbar.current_tab]!=NULL)
            slk_gui_input(w,e->tabbar.elements[e->tabbar.current_tab],button_left,button_right,cursor_x,cursor_y);
      }
      else if(e->type==SLK_GUI_ELEMENT_VTABBAR)
      {
         int height = 14*e->vtabbar.tabs;
         if(button_left.pressed&&INSIDE(cursor_x,cursor_y,w->pos.x+e->vtabbar.pos.x,w->pos.y+e->vtabbar.pos.y,e->vtabbar.pos.w,height))
            e->vtabbar.current_tab = (cursor_y-w->pos.y-e->vtabbar.pos.y)/(height/e->vtabbar.tabs);
         if(e->vtabbar.elements[e->vtabbar.current_tab]!=NULL)
            slk_gui_input(w,e->vtabbar.elements[e->vtabbar.current_tab],button_left,button_right,cursor_x,cursor_y);
      }

      e = e->next;
   }
}

#undef INSIDE
#undef MIN
#undef MAX
#undef MALLOC_CHECK

#endif
#endif
