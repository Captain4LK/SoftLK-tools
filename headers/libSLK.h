#ifndef _LIBSLK_H_

/*
libSLK - single header edition

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#include <stdio.h>
#include <stdint.h>

#define _LIBSLK_H_

typedef union
{
   uint32_t n;
   struct
   {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t a;
   }rgb;
}SLK_Color;

typedef struct
{
    SLK_Color colors[256];
    int used;
}SLK_Palette;

typedef struct
{
    int width;
    int height;
    int changed;
    SLK_Color *data;
}SLK_RGB_sprite;

typedef struct
{
   int width;
   int height;
   int changed;
   uint8_t *data;
}SLK_Pal_sprite;

typedef struct
{
   SLK_Pal_sprite *target;
   SLK_RGB_sprite *render;
   SLK_Palette *palette;
}SLK_Layer_pal;

typedef struct
{
   SLK_RGB_sprite *target;
}SLK_Layer_rgb;

typedef struct
{
   int type;
   int active;
   int dynamic;
   int resized;
   int x;
   int y;
   float scale;
   SLK_Color tint;

   union
   {
      SLK_Layer_pal type_0;
      SLK_Layer_rgb type_1;
   }as;
}SLK_Layer;

typedef struct
{
   int pressed;
   int released;
   int held;
}SLK_Button;

typedef void (*SLK_log_pointer_type)(const char *format, ...);

typedef enum 
{
   SLK_KEY_NONE,
   SLK_KEY_A,SLK_KEY_B,SLK_KEY_C,SLK_KEY_D,SLK_KEY_E,SLK_KEY_F,SLK_KEY_G,
   SLK_KEY_H,SLK_KEY_I,SLK_KEY_J,SLK_KEY_K,SLK_KEY_L,SLK_KEY_M,SLK_KEY_N,
   SLK_KEY_O,SLK_KEY_P,SLK_KEY_Q,SLK_KEY_R,SLK_KEY_S,SLK_KEY_T,SLK_KEY_U,
   SLK_KEY_V,SLK_KEY_W,SLK_KEY_X,SLK_KEY_Y,SLK_KEY_Z,
   SLK_KEY_0,SLK_KEY_1,SLK_KEY_2,SLK_KEY_3,SLK_KEY_4,SLK_KEY_5,
   SLK_KEY_6,SLK_KEY_7,SLK_KEY_8,SLK_KEY_9,
   SLK_KEY_F1,SLK_KEY_F2,SLK_KEY_F3,SLK_KEY_F4,SLK_KEY_F5,SLK_KEY_F6,
   SLK_KEY_F7,SLK_KEY_F8,SLK_KEY_F9,SLK_KEY_F10,SLK_KEY_F11,SLK_KEY_F12,
   SLK_KEY_UP,SLK_KEY_DOWN,SLK_KEY_LEFT,SLK_KEY_RIGHT,
   SLK_KEY_SPACE,SLK_KEY_TAB,SLK_KEY_SHIFT,SLK_KEY_CTRL,SLK_KEY_INS,
   SLK_KEY_DEL,SLK_KEY_HOME,SLK_KEY_END,SLK_KEY_PGUP,SLK_KEY_PGDN,
   SLK_KEY_BACK,SLK_KEY_ESCAPE,SLK_KEY_RETURN,SLK_KEY_ENTER,SLK_KEY_PAUSE,SLK_KEY_SCROLL,
   SLK_KEY_NP0,SLK_KEY_NP1,SLK_KEY_NP2,SLK_KEY_NP3,SLK_KEY_NP4,SLK_KEY_NP5,SLK_KEY_NP6,SLK_KEY_NP7,SLK_KEY_NP8,SLK_KEY_NP9,
   SLK_KEY_NP_MUL,SLK_KEY_NP_DIV,SLK_KEY_NP_ADD,SLK_KEY_NP_SUB,SLK_KEY_NP_DECIMAL,
   SLK_KEY_MAX,
}SLK_key;

typedef enum 
{
   SLK_PAD_A, SLK_PAD_B, SLK_PAD_X, SLK_PAD_Y, SLK_PAD_BACK, SLK_PAD_GUIDE,
   SLK_PAD_START, SLK_PAD_LEFTSTICK, SLK_PAD_RIGHTSTICK, SLK_PAD_LEFTSHOULDER,
   SLK_PAD_RIGHTSHOULDER, SLK_PAD_UP, SLK_PAD_DOWN, SLK_PAD_LEFT, SLK_PAD_RIGHT,
   SLK_PAD_MAX,
}SLK_gamepad_button;

typedef enum 
{
   SLK_BUTTON_LEFT,SLK_BUTTON_RIGHT,SLK_BUTTON_MIDDLE,SLK_BUTTON_X1,SLK_BUTTON_X2,
   SLK_BUTTON_MAX,
}SLK_mouse_button;

typedef enum 
{
   SLK_LAYER_PAL,SLK_LAYER_RGB,
}SLK_layer;

enum SLK_mask
{
  SLK_OPAQUE = 0,
  SLK_TRANSPARENT = 255,
};

typedef enum 
{
   SLK_FLIP_NONE = 0,
   SLK_FLIP_VERTICAL = 1,
   SLK_FLIP_HORIZONTAL = 2,
   SLK_FLIP_VERTHOR = SLK_FLIP_VERTICAL|SLK_FLIP_HORIZONTAL,
}SLK_flip;

enum SLK_window
{
   SLK_WINDOW_MAX = -1,
};

//Palette subsystem: SLK_palette.c
SLK_Palette    *SLK_palette_load(const char *path);
SLK_Palette    *SLK_palette_load_file(FILE *f);
void            SLK_palette_save(const char *path, const SLK_Palette *pal);
void            SLK_palette_save_file(FILE *f, const SLK_Palette *pal);
void            SLK_palette_set_color(SLK_Palette *palette, int index, SLK_Color color);
SLK_Color       SLK_palette_get_color(const SLK_Palette *palette, int index);

//Timer subsystem: SLK_timer.c
void            SLK_timer_set_fps(int FPS);
int             SLK_timer_get_fps();
void            SLK_timer_update();
float           SLK_timer_get_delta();

//Pal sprite subsystem: SLK_sprite_pal.c
SLK_Pal_sprite *SLK_pal_sprite_create(int width, int height);
void            SLK_pal_sprite_destroy(SLK_Pal_sprite *s);
uint8_t         SLK_pal_sprite_get_index(const SLK_Pal_sprite *s, int x, int y);
void            SLK_pal_sprite_set_index(SLK_Pal_sprite *s, int x, int y, uint8_t c);
SLK_Pal_sprite *SLK_pal_sprite_load(const char *path);
SLK_Pal_sprite *SLK_pal_sprite_load_file(FILE *f);
SLK_Pal_sprite *SLK_pal_sprite_load_mem(const void *data, int length);
void            SLK_pal_sprite_save(const char *path, const SLK_Pal_sprite *s, int rle);
void            SLK_pal_sprite_save_file(FILE *f, const SLK_Pal_sprite *s, int rle);
void            SLK_pal_sprite_copy(SLK_Pal_sprite *dst, const SLK_Pal_sprite *src);
void            SLK_pal_sprite_copy_partial(SLK_Pal_sprite *dst, const SLK_Pal_sprite *src, int x, int y, int ox, int oy, int width, int height);

//Draw pal subsystem: SLK_draw_pal.c
void            SLK_draw_pal_set_target(SLK_Pal_sprite *s);
SLK_Pal_sprite *SLK_draw_pal_get_target();
void            SLK_draw_pal_set_clear_index(uint8_t index);
void            SLK_draw_pal_load_font(const char *path);
void            SLK_draw_pal_set_font_sprite(SLK_Pal_sprite *font);
void            SLK_draw_pal_clear();
void            SLK_draw_pal_index(int x, int y, uint8_t index);
void            SLK_draw_pal_string(int x, int y, int scale, const char *text, uint8_t index);
void            SLK_draw_pal_sprite(const SLK_Pal_sprite *s, int x, int y);
void            SLK_draw_pal_sprite_partial(const SLK_Pal_sprite *s, int x, int y, int ox, int oy, int width, int height);
void            SLK_draw_pal_sprite_flip(const SLK_Pal_sprite *s, int x, int y, SLK_flip flip);
void            SLK_draw_pal_line(int x0, int y0, int x1, int y1, uint8_t index);
void            SLK_draw_pal_vertical_line(int x, int y0, int y1, uint8_t index);
void            SLK_draw_pal_horizontal_line(int x0, int x1, int y, uint8_t index);
void            SLK_draw_pal_rectangle(int x, int y, int width, int height, uint8_t index);
void            SLK_draw_pal_fill_rectangle(int x, int y, int width, int height, uint8_t index);
void            SLK_draw_pal_circle(int x, int y, int radius, uint8_t index);
void            SLK_draw_pal_fill_circle(int x, int y, int radius, uint8_t index);


//RGB sprite subsystem: SLK_sprite_rgb.c
SLK_RGB_sprite *SLK_rgb_sprite_create(int width, int height);
void            SLK_rgb_sprite_destroy(SLK_RGB_sprite *s);
SLK_Color       SLK_rgb_sprite_get_pixel(const SLK_RGB_sprite *s, int x, int y);
void            SLK_rgb_sprite_set_pixel(SLK_RGB_sprite *s, int x, int y, SLK_Color c);

SLK_RGB_sprite *SLK_rgb_sprite_load(const char *path);
SLK_RGB_sprite *SLK_rgb_sprite_load_file(FILE *f);
SLK_RGB_sprite *SLK_rgb_sprite_load_mem(const void *data, int length);
void            SLK_rgb_sprite_save(const char *path, const SLK_RGB_sprite *s);
void            SLK_rgb_sprite_save_file(FILE *f, const SLK_RGB_sprite *s);
void            SLK_rgb_sprite_copy(SLK_RGB_sprite *dst, const SLK_RGB_sprite *src);
void            SLK_rgb_sprite_copy_partial(SLK_RGB_sprite *dst, const SLK_RGB_sprite *src, int x, int y, int ox, int oy, int width, int height);

//Draw rgb subsystem: SLK_draw_rgb.c
SLK_RGB_sprite *SLK_draw_rgb_get_target();
void            SLK_draw_rgb_set_target(SLK_RGB_sprite *s);
void            SLK_draw_rgb_set_clear_color(SLK_Color color);
void            SLK_draw_rgb_set_changed(int changed);
int             SLK_draw_rgb_get_changed();
void            SLK_draw_rgb_load_font(const char *path);
void            SLK_draw_rgb_set_font_sprite(SLK_RGB_sprite *font);
void            SLK_draw_rgb_clear();
void            SLK_draw_rgb_color(int x, int y, SLK_Color color);
void            SLK_draw_rgb_string(int x, int y, int scale, const char *text, SLK_Color color);
void            SLK_draw_rgb_image_string(int x, int y, int scale, const char *text);
void            SLK_draw_rgb_sprite(const SLK_RGB_sprite *s, int x, int y);
void            SLK_draw_rgb_sprite_partial(const SLK_RGB_sprite *s, int x, int y, int ox, int oy, int width, int height);
void            SLK_draw_rgb_sprite_flip(const SLK_RGB_sprite *s, int x, int y, SLK_flip flip);
void            SLK_draw_rgb_line(int x0, int y0, int x1, int y1, SLK_Color color);
void            SLK_draw_rgb_vertical_line(int x, int y0, int y1, SLK_Color color);
void            SLK_draw_rgb_horizontal_line(int x0, int x1, int y, SLK_Color color);
void            SLK_draw_rgb_rectangle(int x, int y, int width, int height, SLK_Color color);
void            SLK_draw_rgb_fill_rectangle(int x, int y, int width, int height, SLK_Color color);
void            SLK_draw_rgb_circle(int x, int y, int radius, SLK_Color color);
void            SLK_draw_rgb_fill_circle(int x, int y, int radius, SLK_Color color);

//Color subsystem: SLK_color.c
SLK_Color       SLK_color_create(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SLK_Color       SLK_color_create_float(float r, float g, float b, float a);
SLK_Color       SLK_color_create_hsv(float h, float s, float v, float a);
SLK_Color       SLK_color_create_hsl(float h, float s, float l, float a);

//Input subsystem: SLK_input.c 
int             SLK_key_down(SLK_key key);
int             SLK_key_pressed(SLK_key key);
int             SLK_key_released(SLK_key key);
SLK_Button      SLK_key_get_state(SLK_key key);
int             SLK_gamepad_down(int id, SLK_gamepad_button key);
int             SLK_gamepad_pressed(int id, SLK_gamepad_button key);
int             SLK_gamepad_released(int id, SLK_gamepad_button key);
SLK_Button      SLK_gamepad_get_state(int id, SLK_gamepad_button key);
int             SLK_gamepad_count();
int             SLK_mouse_down(SLK_mouse_button key);
int             SLK_mouse_pressed(SLK_mouse_button key);
int             SLK_mouse_released(SLK_mouse_button key);
SLK_Button      SLK_mouse_get_state(SLK_mouse_button key);
int             SLK_mouse_wheel_get_scroll();
void            SLK_mouse_get_pos(int *x, int *y);
void            SLK_mouse_get_relative_pos(int *x, int *y);
void            SLK_mouse_get_layer_pos(unsigned index, int *x, int *y);
void            SLK_mouse_show_cursor(int shown);
void            SLK_mouse_set_relative(int relative);
void            SLK_mouse_capture(int capture);
void            SLK_text_input_start(char *text, int max_length);
void            SLK_text_input_stop();

//Layer subsystem: SLK_layer.c
void            SLK_layer_create(unsigned index, SLK_layer type);
void            SLK_layer_activate(unsigned index, int active);
void            SLK_layer_set_palette(unsigned index, SLK_Palette *pal);
void            SLK_layer_set_tint(unsigned index, SLK_Color tint);
void            SLK_layer_set_dynamic(unsigned index, int dynamic);
void            SLK_layer_set_pos(unsigned index, int x, int y);
void            SLK_layer_set_scale(unsigned index, float scale);
void            SLK_layer_set_size(unsigned index, int width, int height);
void            SLK_layer_get_size(unsigned index, int *width, int *height);
void            SLK_layer_set_current(unsigned index);
int             SLK_layer_get_resized(unsigned index);
SLK_Layer      *SLK_layer_get(unsigned index);

//Core subsystem: SLK_core.c 
void            SLK_setup(int width, int height, int layer_num, const char *title, int fullscreen, int scale, int resizable);
void            SLK_update();
void            SLK_core_set_title(const char *title);
void            SLK_core_set_fullscreen(int fullscreen);
void            SLK_core_set_visible(int visible);
void            SLK_core_set_icon(const SLK_RGB_sprite *icon);
int             SLK_core_get_width();
int             SLK_core_get_height();
int             SLK_core_running();
void            SLK_core_quit();
void            SLK_render_update();
void           *SLK_system_malloc(size_t size);
void            SLK_system_free(void *ptr);
void           *SLK_system_realloc(void *ptr, size_t size);
void            SLK_set_malloc(void *(*func)(size_t size));
void            SLK_set_free(void (*func)(void *ptr));
void            SLK_set_realloc(void *(*func)(void *ptr, size_t size));

void SLK_log(const char *w, ...);
#define SLK_log_line(w,...) do { char SLK_log_line_tmp[1024]; snprintf(SLK_log_line_tmp,1024,__VA_ARGS__); SLK_log(w " (%s:%u): %s\n",__FILE__,__LINE__,SLK_log_line_tmp); } while(0)

#endif //_LIBSLK_H_

#ifdef LIBSLK_IMPLEMENTATION
#ifndef LIBSLK_IMPLEMENTATION_ONCE
#define LIBSLK_IMPLEMENTATION_ONCE

//0 - sdl2
//1 - win32
#ifndef LIBSLK_BACKEND
#define LIBSLK_BACKEND 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define SWAP(x,y) \
            { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); }

#define INBOUNDS(LOWER,UPPER,NUMBER) \
         ((NUMBER)>=(LOWER)&&(NUMBER)<(UPPER))

#define SIGNUM(NUM) \
   (NUM==0?0:(NUM<0?-1:1))

static SLK_Pal_sprite *target_pal;
static SLK_Pal_sprite *target_pal_default;
static SLK_RGB_sprite *target_rgb;
static SLK_RGB_sprite *target_rgb_default;
static int layer_dynamic;
static int layer_count;
static SLK_Layer *layers;
static SLK_Layer *current_layer;

//Every backend needs to implement all of these functions. You can however just keep the implementation empty.
void backend_set_title(const char *title);
void backend_set_fullscreen(int fullscreen);
void backend_update_viewport();
void backend_set_visible(int visible);
void backend_set_icon(const SLK_RGB_sprite *icon);
int backend_get_width();
int backend_get_height();
int backend_get_view_width();
int backend_get_view_height();
int backend_get_view_x();
int backend_get_view_y();
int backend_get_win_width();
int backend_get_win_height();
void backend_set_fps(int FPS);
int backend_get_fps();
void backend_timer_update();
float backend_timer_get_delta();
void backend_handle_events();
void backend_setup(int width, int height, int layer_num, const char *title, int fullscreen, int scale, int resizable);
void backend_input_init();
void backend_show_cursor(int shown);
void backend_mouse_set_relative(int relative);
void backend_mouse_capture(int capture);
void backend_start_text_input(char *text, int max_length);
void backend_stop_text_input();
void backend_render_update();
void backend_create_layer(unsigned index, int type);
int backend_key_down(int key);
int backend_key_pressed(int key);
int backend_key_released(int key);
SLK_Button backend_key_get_state(int key);
int backend_mouse_down(int key);
int backend_mouse_pressed(int key);
int backend_mouse_released(int key);
SLK_Button backend_mouse_get_state(int key);
int backend_mouse_wheel_get_scroll();
int backend_gamepad_down(int index, int key);
int backend_gamepad_pressed(int index, int key);
int backend_gamepad_released(int index, int key);
SLK_Button backend_gamepad_get_state(int index, int key);
int backend_get_gamepad_count();
void backend_mouse_get_pos(int *x, int *y);
void backend_mouse_get_relative_pos(int *x, int *y);
SLK_RGB_sprite *backend_load_rgb(const char *path);
SLK_RGB_sprite *backend_load_rgb_file(FILE *f);
SLK_RGB_sprite *backend_load_rgb_mem(const void *data, int length);
void backend_save_rgb(const SLK_RGB_sprite *s, const char *path);
void backend_save_rgb_file(const SLK_RGB_sprite *s, FILE *f);
SLK_Pal_sprite *backend_load_pal(const char *path);
SLK_Pal_sprite *backend_load_pal_file(FILE *f);
SLK_Pal_sprite *backend_load_pal_mem(const void *data, int length);
void backend_save_pal(const SLK_Pal_sprite *s, const char *path, int rle);
void backend_save_pal_file(const SLK_Pal_sprite *s, FILE *f, int rle);
SLK_Palette *backend_load_palette(const char *path);
SLK_Palette *backend_load_palette_file(FILE *f);
void backend_save_palette(const char *path, const SLK_Palette *pal);
void backend_save_palette_file(FILE *f, const SLK_Palette *pal);
void backend_log(const char *w, va_list v);

//Memory managment
void *backend_system_malloc(size_t size);
void backend_system_free(void *ptr);
void *backend_system_realloc(void *ptr, size_t size);
void backend_set_malloc(void *(*func)(size_t size));
void backend_set_free(void (*func)(void *ptr));
void backend_set_realloc(void *(*func)(void *ptr, size_t size));
void *backend_malloc(size_t size);
void *backend_calloc(size_t num, size_t size);
void backend_free(void *ptr);
void *backend_realloc(void *ptr, size_t size);

//Creates a SLK_Color struct from 4 unsigned 8bit ints.
SLK_Color SLK_color_create(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
   SLK_Color c;
   c.rgb.r = r;
   c.rgb.g = g;
   c.rgb.b = b;
   c.rgb.a = a;
   
   return c;
} 

//Creates a SLK_Color struct from 4 floats.
SLK_Color SLK_color_create_float(float r, float g, float b, float a)
{
   SLK_Color c;
   c.rgb.r = r*255.0f;
   c.rgb.g = g*255.0f;
   c.rgb.b = b*255.0f;
   c.rgb.a = a*255.0f;

   return c;
}

//Creates a SLK_Color struct from 4 floats.
//h is expected to be in range [0,360].
SLK_Color SLK_color_create_hsv(float h, float s, float v, float a)
{
   SLK_Color c;
   if(s<=0.0f)
   {
      c.rgb.r = 0;
      c.rgb.g = 0;
      c.rgb.b = 0;
      c.rgb.a = a*255.0f;

      return c;
   }

   float ch = s*v;
   if(h>=360.0f)
      h = 0.0f;
   h/=60;
   float x = ch*(1-fabs(fmod(h,2)-1));
   float m = v-ch;
   float r,g,b;

   if(h>=0.0f&&h<=1.0f)
   {
      r = ch;
      g = x;
      b = 0.0f;
   }
   else if(h>1.0f&&h<=2.0f)
   {
      r = x;
      g = ch;
      b = 0.0f;
   }
   else if(h>2.0f&&h<=3.0f)
   {
      r = 0.0f;
      g = ch;
      b = x;
   }
   else if(h>3.0f&&h<=4.0f)
   {
      r = 0.0f;
      g = x;
      b = ch;
   }
   else if(h>4.0f&&h<=5.0f)
   {
      r = x;
      g = 0.0f;
      b = ch;
   }
   else
   {
      r = ch;
      g = 0.0f;
      b = x;
   }

   c.rgb.r = (r+m)*255.0f;
   c.rgb.g = (g+m)*255.0f;
   c.rgb.b = (b+m)*255.0f;
   c.rgb.a = a*255.0f;

   return c;
}

//Creates a SLK_Color struct from 4 floats.
//h is expected to be in range [0,360].
SLK_Color SLK_color_create_hsl(float h, float s, float l, float a)
{
   SLK_Color c;
   if(s<=0.0f)
   {
      c.rgb.r = 0;
      c.rgb.g = 0;
      c.rgb.b = 0;
      c.rgb.a = a*255.0f;

      return c;
   }

   float ch = (1.0f-fabs(2.0f*l-1.0f))*s;
   if(h>=360.0f)
      h = 0.0f;
   h/=60;
   float x = ch*(1-fabs(fmod(h,2)-1));
   float m = l-(ch/2.0f);
   float r,g,b;

   if(h>=0.0f&&h<=1.0f)
   {
      r = ch;
      g = x;
      b = 0.0f;
   }
   else if(h>1.0f&&h<=2.0f)
   {
      r = x;
      g = ch;
      b = 0.0f;
   }
   else if(h>2.0f&&h<=3.0f)
   {
      r = 0.0f;
      g = ch;
      b = x;
   }
   else if(h>3.0f&&h<=4.0f)
   {
      r = 0.0f;
      g = x;
      b = ch;
   }
   else if(h>4.0f&&h<=5.0f)
   {
      r = x;
      g = 0.0f;
      b = ch;
   }
   else
   {
      r = ch;
      g = 0.0f;
      b = x;
   }

   c.rgb.r = (r+m)*255.0f;
   c.rgb.g = (g+m)*255.0f;
   c.rgb.b = (b+m)*255.0f;
   c.rgb.a = a*255.0f;

   return c;
}

static int running;

//Sets the running flag to 0.
void SLK_core_quit()
{
   running = 0;
}

//Checks wether SoftLK is supposed to be running or not.
int SLK_core_running()
{
    return running;
}

//Sets the title of the window.
void SLK_core_set_title(const char *title)
{
   backend_set_title(title);
}

//Sets the window in windowed or fullscreen mode.
void SLK_core_set_fullscreen(int fullscreen)
{
   backend_set_fullscreen(fullscreen);
}

//Shows or hides the window.
void SLK_core_set_visible(int visible)
{
   backend_set_visible(visible);
}

//Sets the icon of the window.
void SLK_core_set_icon(const SLK_RGB_sprite *icon)
{
   backend_set_icon(icon);
}

//Returns the current viewport width.
int SLK_core_get_width()
{
   return backend_get_width();
}

//Returns the current viewport height.
int SLK_core_get_height()
{
   return backend_get_height();
}

//Updates the engine state.
//Updates the input and the timer.
void SLK_update()
{
   SLK_timer_update();

   backend_handle_events();
}

//The first function you should call in your code.
//Creates a window, sets its title and allocates space for the layers.
void SLK_setup(int width, int height, int layer_num, const char *title, int fullscreen, int scale, int resizable)
{
   //Allocate space for layers, max layer num is fixed.
   layers = backend_malloc(sizeof(layers[0])*layer_num);
   if(layers==NULL)
      SLK_log("malloc of size %zu failed, out of memory!",sizeof(layers[0])*layer_num);
   memset(layers,0,sizeof(layers[0])*layer_num);

   backend_setup(width,height,layer_num,title,fullscreen,scale,resizable);

   running = 1;
   SLK_core_set_fullscreen(fullscreen);

   //Setup keymaps, or whatever your favourite backend does here.
   backend_input_init();
}

//Clears the window and redraws the scene.
//Drawing is performed from back to front, layer 0 is always drawn last.
void SLK_render_update()
{
   backend_render_update();
}

//System specific malloc, gets used by SoftLK-lib internally if not 
//replace by SLK_set_malloc()
void *SLK_system_malloc(size_t size)
{
   return backend_system_malloc(size);
}

//System specific free, gets used by SoftLK-lib internally if not 
//replace by SLK_set_free()
void SLK_system_free(void *ptr)
{
   backend_system_free(ptr);
}

//System specific realloc, gets used by SoftLK-lib internally if not 
//replace by SLK_set_realloc()
void *SLK_system_realloc(void *ptr, size_t size)
{
   return backend_system_realloc(ptr,size);
}

void SLK_set_malloc(void *(*func)(size_t size))
{
   backend_set_malloc(func);
}

void SLK_set_free(void (*func)(void *ptr))
{
   backend_set_free(func);
}

void SLK_set_realloc(void *(*func)(void *ptr, size_t size))
{
   backend_set_realloc(func);
}

static uint8_t target_pal_clear;
static const SLK_Pal_sprite *text_sprite_pal;
static SLK_Pal_sprite *text_sprite_pal_default;

//Returns the target for drawing operations.
//Usefull for getting the default draw target.
SLK_Pal_sprite *SLK_draw_pal_get_target()
{
   return target_pal;
}

//Sets the target for drawing operations.
//Used for drawing to sprites, for creating spritesheets
//SLK_pal_sprite_copy(_partial) is advised.
void SLK_draw_pal_set_target(SLK_Pal_sprite *s)
{
   if(s==NULL)
   {
      target_pal = target_pal_default;
      return;
   }

   target_pal = s;
}

//Loads a new font sprite.
//Replaces the default font sprite.
void SLK_draw_pal_load_font(const char *path)
{
   SLK_pal_sprite_destroy(text_sprite_pal_default);
   text_sprite_pal_default = SLK_pal_sprite_load(path);
   text_sprite_pal = text_sprite_pal_default;
}

//Sets the current font sprite from a 
//sprite you have loaded in your code.
//Pass NULL to reset to default.
void SLK_draw_pal_set_font_sprite(SLK_Pal_sprite *font)
{
   if(font==NULL)
   {
      text_sprite_pal = text_sprite_pal_default;
      return;
   }

   text_sprite_pal = font;
}

//Sets the color wich the target is to be cleared to
//when calling SLK_draw_pal_clear.
void SLK_draw_pal_set_clear_index(uint8_t index)
{
   target_pal_clear = index;
}

//Clears the target to the color set by 
//SLK_draw_pal_set_clear_index.
void SLK_draw_pal_clear()
{
   memset(target_pal->data,target_pal_clear,sizeof(*target_pal->data)*target_pal->width*target_pal->height);
}

//Draws a single index to the draw target.
void SLK_draw_pal_index(int x, int y, uint8_t index)
{
   if(!index)
      return;

   if(INBOUNDS(0,target_pal->width,x)&&INBOUNDS(0,target_pal->height,y))
   {
      int ind = y*target_pal->width+x;
      target_pal->data[ind]= index;
   }
}

//Draws a string to the draw target.
//Color and scale must be specified.
void SLK_draw_pal_string(int x, int y, int scale, const char *text, uint8_t index)
{
   if(!index)
      return;

   int x_dim = text_sprite_pal->width/16;
   int y_dim = text_sprite_pal->height/6;
   int sx = 0;
   int sy = 0;

   for(int i = 0;text[i];i++)
   {
      if(text[i]=='\n')
      {
         sx = 0; 
         sy+=y_dim*scale;
         continue;
      }

      int ox = (text[i]-32)&15;
      int oy = (text[i]-32)/16;
      for(int y_ = 0;y_<y_dim;y_++)
      {
         for(int x_ = 0;x_<x_dim;x_++)
         {
            if(text_sprite_pal->data[(y_+oy*y_dim)*text_sprite_pal->width+x_+ox*x_dim])
               continue;
            for(int m = 0;m<scale;m++)
               for(int o = 0;o<scale;o++)
                  SLK_draw_pal_index(x+sx+(x_*scale)+o,y+sy+(y_*scale)+m,index);
         }
      }
      sx+=x_dim*scale;
   }
}

//Draws a sprite to the draw target.
void SLK_draw_pal_sprite(const SLK_Pal_sprite *s, int x, int y)
{
   //Clip source sprite
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = s->width;
   int draw_end_y = s->height;
   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x+draw_end_x>target_pal->width)
      draw_end_x = s->width+(target_pal->width-x-draw_end_x);
   if(y+draw_end_y>target_pal->height)
      draw_end_y = s->height+(target_pal->height-y-draw_end_y);

   //Clip dst sprite
   x = x<0?0:x;
   y = y<0?0:y;

   const uint8_t *src = &s->data[draw_start_x+draw_start_y*s->width];
   uint8_t *dst = &target_pal->data[x+y*target_pal->width];
   int src_step = -(draw_end_x-draw_start_x)+s->width;
   int dst_step = target_pal->width-(draw_end_x-draw_start_x);
    
   for(int y1 = draw_start_y;y1<draw_end_y;y1++,dst+=dst_step,src+=src_step)
      for(int x1 = draw_start_x;x1<draw_end_x;x1++,src++,dst++)
         *dst = *src?*src:*dst;
}

//Draws a specified part of a sprite to the draw target.
//This could be used for spritesheet drawing, but since the pal 
//drawing system is software accelerated, there is no speed penality
//when splitting the spritesheet into individual sprites.
void SLK_draw_pal_sprite_partial(const SLK_Pal_sprite *s, int x, int y, int ox, int oy, int width, int height)
{
   //Clip source sprite
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = width;
   int draw_end_y = height;
   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x+draw_end_x>target_pal->width)
      draw_end_x = width+(target_pal->width-x-draw_end_x);
   if(y+draw_end_y>target_pal->height)
      draw_end_y = height+(target_pal->height-y-draw_end_y);

   //Clip dst sprite
   x = x<0?0:x;
   y = y<0?0:y;

   const uint8_t *src = &s->data[draw_start_x+ox+(draw_start_y+oy)*s->width];
   uint8_t *dst = &target_pal->data[x+y*target_pal->width];
   int src_step = -(draw_end_x-draw_start_x)+s->width;
   int dst_step = target_pal->width-(draw_end_x-draw_start_x);
    
   for(int y1 = draw_start_y;y1<draw_end_y;y1++,dst+=dst_step,src+=src_step)
      for(int x1 = draw_start_x;x1<draw_end_x;x1++,src++,dst++)
         *dst = *src?*src:*dst;
}

//Draws a flipped sprite to the draw target.
//For vertical flipping pass SLK_FLIP_VERTICAL,
//for horizontal flipping pass SLK_FLIP_HORIZONTAL
//(Note: The values can be or'd together).
void SLK_draw_pal_sprite_flip(const SLK_Pal_sprite *s, int x, int y, SLK_flip flip)
{
   //Clip source sprite
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = s->width;
   int draw_end_y = s->height;
   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x+draw_end_x>target_pal->width)
      draw_end_x = s->width+(target_pal->width-x-draw_end_x);
   if(y+draw_end_y>target_pal->height)
      draw_end_y = s->height+(target_pal->height-y-draw_end_y);

   //Clip dst sprite
   x = x<0?0:x;
   y = y<0?0:y;

   const uint8_t *src = &s->data[0];
   uint8_t *dst = &target_pal->data[x+y*target_pal->width];
   int src_step = -(draw_end_x-draw_start_x)+s->width;
   int dst_step = target_pal->width-(draw_end_x-draw_start_x);

   switch(flip)
   {
   case SLK_FLIP_NONE:
      src+=draw_start_x+draw_start_y*s->width;
      src_step = -(draw_end_x-draw_start_x)+s->width;
      dst_step = target_pal->width-(draw_end_x-draw_start_x);
      for(int y1 = draw_start_y;y1<draw_end_y;y1++,dst+=dst_step,src+=src_step)
         for(int x1 = draw_start_x;x1<draw_end_x;x1++,src++,dst++)
            *dst = *src?*src:*dst;
      break;
   case SLK_FLIP_VERTICAL:
      src+=draw_start_x+s->width*(s->height-draw_start_y-1);
      src_step = -(draw_end_x-draw_start_x)-s->width;
      dst_step = target_pal->width-(draw_end_x-draw_start_x);
      for(int y1 = draw_start_y;y1<draw_end_y;y1++,dst+=dst_step,src+=src_step)
         for(int x1 = draw_start_x;x1<draw_end_x;x1++,src++,dst++)
            *dst = *src?*src:*dst;
      break;
   case SLK_FLIP_HORIZONTAL:
      src+=draw_start_y*s->width-draw_start_x+s->width-1;
      src_step = (draw_end_x-draw_start_x)+s->width;
      dst_step = target_pal->width-(draw_end_x-draw_start_x);
      for(int y1 = draw_start_y;y1<draw_end_y;y1++,dst+=dst_step,src+=src_step)
         for(int x1 = draw_start_x;x1<draw_end_x;x1++,src--,dst++)
            *dst = *src?*src:*dst;
      break;
   case SLK_FLIP_VERTHOR:
      src+=s->width*(s->height-draw_start_y-1);
      src+=-draw_start_x+s->width-1;
      src_step = (draw_end_x-draw_start_x)-s->width;
      dst_step = target_pal->width-(draw_end_x-draw_start_x);
      for(int y1 = draw_start_y;y1<draw_end_y;y1++,dst+=dst_step,src+=src_step)
         for(int x1 = draw_start_x;x1<draw_end_x;x1++,src--,dst++)
            *dst = *src?*src:*dst;
      break;
   }
}

//Draws a colored line between 2 points using
//the Bresenham line drawing algorythm.
void SLK_draw_pal_line(int x0, int y0, int x1, int y1, uint8_t index)
{
   if(!index)
      return;

   if(x0>x1||y0>y1)
   {
      SWAP(x0,x1);
      SWAP(y0,y1);
   }
   int dx = x1-x0;
   int ix = (dx>0)-(dx<0);
   dx = abs(dx)<<1;
   int dy = y1-y0;
   int iy = (dy>0)-(dy<0);
   dy = abs(dy)<<1;
   SLK_draw_pal_index(x0,y0,index);

   if(dx>=dy)
   {
      int error = dy-(dx>>1);
      while(x0!=x1)
      {
         if(error>0||(!error&&ix>0))
         {
            error-=dx;
            y0+=iy;
         }

         error+=dy;
         x0+=ix;

         SLK_draw_pal_index(x0,y0,index);
      }
   }
   else
   {
      int error = dx-(dy>>1);

      while(y0!=y1)
      {
         if(error>0||(!error&&iy>0))
         {
            error-=dy;
            x0+=ix;
         }

         error+=dx;
         y0+=iy;

         SLK_draw_pal_index(x0,y0,index);
      }
   }
}

//Draws a line between to points up to but not including the second point,
//with a fixed x value.
void SLK_draw_pal_vertical_line(int x, int y0, int y1, uint8_t index)
{
   if(!index)
      return;

   if(x<0||x>=target_pal->width||y0>=target_pal->height||y1<0)
      return;
   if(y0<0)
      y0 = 0;
   if(y1>target_pal->height)
      y1 = target_pal->height;

   for(int y = y0;y<y1;y++)
      target_pal->data[y*target_pal->width+x] = index;
}

//Draws a line between to points up to but not including the second point,
//with a fixed y value.
void SLK_draw_pal_horizontal_line(int x0, int x1, int y, uint8_t index)
{
   if(!index)
      return;

   if(x0>x1)
      SWAP(x0,x1);
   
   if(y<0||y>=target_pal->height||x0>=target_pal->width||x1<0)
      return;
   if(x0<0)
      x0 = 0;
   if(x1>target_pal->width)
      x1 = target_pal->width;

   uint8_t *dst = &target_pal->data[y*target_pal->width+x0];
   for(int x = x0;x<x1;x++,dst++)
      *dst = index;
}

//Draws the outline of a colored rectangle.
void SLK_draw_pal_rectangle(int x, int y, int width, int height, uint8_t index)
{
   if(!index)
      return;

   SLK_draw_pal_horizontal_line(x,x+width,y,index);
   SLK_draw_pal_horizontal_line(x,x+width,y+height-1,index);
   SLK_draw_pal_vertical_line(x,y,y+height,index);
   SLK_draw_pal_vertical_line(x+width-1,y,y+height-1,index);
}

//Draws a colored filled rectangle.
//The only function to not return when drawing with an
//index of 0, can thus be used for clearing parts of
//the target.
void SLK_draw_pal_fill_rectangle(int x, int y, int width, int height, uint8_t index)
{
   //Clip src rect
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = width;
   int draw_end_y = height;

   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x+draw_end_x>target_pal->width)
      draw_end_x = width+(target_pal->width-x-draw_end_x);
   if(y+draw_end_y>target_pal->height)
      draw_end_y = height+(target_pal->height-y-draw_end_y);
    
   //Clip dst rect
   x = x<0?0:x;
   y = y<0?0:y;

   uint8_t *dst = &target_pal->data[x+y*target_pal->width];
   int dst_step = target_pal->width-(draw_end_x-draw_start_x);
    
   for(int y1 = draw_start_y;y1<draw_end_y;y1++,dst+=dst_step)
      for(int x1 = draw_start_x;x1<draw_end_x;x1++,dst++)
         *dst = index;
}

//Draws the outline of a colored circle.
void SLK_draw_pal_circle(int x, int y, int radius, uint8_t index)
{
   if(!index)
      return;

   int x_ = 0;
   int y_ = radius;
   int d = 1-radius;

   SLK_draw_pal_index(x,y+radius,index);
   SLK_draw_pal_index(x,y-radius,index);
   SLK_draw_pal_index(x+radius,y,index);
   SLK_draw_pal_index(x-radius,y,index);

   while(x_<y_)
   {
      if(d<0)
      {
         d = d+2*x_+3;
         x_+=1;
      }
      else
      {
         d = d+2*(x_-y_)+5;
         x_+=1;
         y_-=1;
      }

      SLK_draw_pal_index(x+x_,y+y_,index);
      SLK_draw_pal_index(x+x_,y-y_,index);
      SLK_draw_pal_index(x-x_,y+y_,index);
      SLK_draw_pal_index(x-x_,y-y_,index);

      SLK_draw_pal_index(x+y_,y+x_,index);
      SLK_draw_pal_index(x+y_,y-x_,index);
      SLK_draw_pal_index(x-y_,y+x_,index);
      SLK_draw_pal_index(x-y_,y-x_,index);
   }
}

//Draws a colored filled circle.
void SLK_draw_pal_fill_circle(int x, int y, int radius, uint8_t index)
{
   if(!index)
      return;

   int x_ = 0;
   int y_ = radius;
   int d = 1-radius;

   SLK_draw_pal_horizontal_line(x-radius,x+radius,y,index);

   while(x_<y_)
   {
      if(d<0)
      {
         d = d+2*x_+3;
         x_+=1;
      }
      else
      {
         d = d+2*(x_-y_)+5;
         x_+=1;
         y_-=1;
      }

      SLK_draw_pal_horizontal_line(x-x_,x+x_,y+y_,index);
      SLK_draw_pal_horizontal_line(x-x_,x+x_,y-y_,index);
      SLK_draw_pal_horizontal_line(x-y_,x+y_,y+x_,index);
      SLK_draw_pal_horizontal_line(x-y_,x+y_,y-x_,index);
   }
}

static SLK_Color target_rgb_clear;
static SLK_RGB_sprite *text_sprite_rgb;
static SLK_RGB_sprite *text_sprite_rgb_default;

//Gets the current draw target.
//Usefull for getting the default draw target.
SLK_RGB_sprite *SLK_draw_rgb_get_target()
{
   return target_rgb;
}

//Sets the draw target for rgb drawing operations.
//Pass NULL to set to default draw target.
void SLK_draw_rgb_set_target(SLK_RGB_sprite *s)
{
   if(s==NULL)
   {
      target_rgb = target_rgb_default;
      return;
   }

   target_rgb = s;
}

//Sets the color the draw target is to be cleared to
//when calling SLK_draw_rgb_clear.
void SLK_draw_rgb_set_clear_color(SLK_Color color)
{
   target_rgb_clear = color;
}

//Sets wether the target has been changed.
//The gpu texture will only be updated if the target
//has been flagged as changed.
void SLK_draw_rgb_set_changed(int changed)
{
   target_rgb->changed = changed;
}

//Returns wether the changed
//attribute has been set on the current
//draw target.
int SLK_draw_rgb_get_changed()
{
   return target_rgb->changed;
}

//Loads a new font sprite.
//Replaces the default font sprite.
void SLK_draw_rgb_load_font(const char *path)
{
   SLK_rgb_sprite_destroy(text_sprite_rgb_default);
   text_sprite_rgb_default = SLK_rgb_sprite_load(path);
   text_sprite_rgb = text_sprite_rgb_default;
}

//Sets the current font sprite from a 
//sprite you have loaded in your code.
//Pass NULL to reset to default.
void SLK_draw_rgb_set_font_sprite(SLK_RGB_sprite *font)
{
   if(font==NULL)
   {
      text_sprite_rgb = text_sprite_rgb_default;
      return;
   }

   text_sprite_rgb = font;
}

//Clears the draw target to the color specified
//by SKL_draw_rgb_set_clear_color.
void SLK_draw_rgb_clear()
{
   for(int i = 0;i<target_rgb->width*target_rgb->height;i++)
      target_rgb->data[i] = target_rgb_clear;
}

//Draws a single pixel to the draw target.
void SLK_draw_rgb_color(int x, int y, SLK_Color color)
{
   if(color.rgb.a&&INBOUNDS(0,target_rgb->width,x)&&INBOUNDS(0,target_rgb->height,y))
      target_rgb->data[y*target_rgb->width+x] = color;
}

//Draws a string to the draw target.
//Color and scale must be specified.
void SLK_draw_rgb_string(int x, int y, int scale, const char *text, SLK_Color color)
{
   int x_dim = text_sprite_rgb->width/16;
   int y_dim = text_sprite_rgb->height/6;
   int sx = 0;
   int sy = 0;

   for(int i = 0;text[i];i++)
   {
      if(text[i]=='\n')
      {
         sx = 0;
         sy+=y_dim*scale;
         continue;
      }

      int ox = (text[i]-32)&15;
      int oy = (text[i]-32)/16;
      for(int y_ = 0;y_<y_dim;y_++)
      {
         for(int x_ = 0;x_<x_dim;x_++)
         {
            if(!text_sprite_rgb->data[(y_+oy*y_dim)*text_sprite_rgb->width+x_+ox*x_dim].rgb.a)
               continue;
            for(int m = 0;m<scale;m++)
               for(int o = 0;o<scale;o++)
                  SLK_draw_rgb_color(x+sx+(x_*scale)+o,y+sy+(y_*scale)+m,color);
         }
      }
      sx+=x_dim*scale;
   }
}

//Draws a string to the draw target.
//Draws the text directly from the font sprite instead of
//coloring visible pixels.
void SLK_draw_rgb_image_string(int x, int y, int scale, const char *text)
{
   int x_dim = text_sprite_rgb->width/16;
   int y_dim = text_sprite_rgb->height/6;
   int sx = 0;
   int sy = 0;

   for(int i = 0;text[i];i++)
   {
      if(text[i]=='\n')
      {
         sx = 0; 
         sy+=y_dim*scale;
         continue;
      }

      int ox = (text[i]-32)&15;
      int oy = (text[i]-32)/16;
      for(int y_ = 0;y_<y_dim;y_++)
      {
         for(int x_ = 0;x_<x_dim;x_++)
         {
            SLK_Color color = text_sprite_rgb->data[(y_+oy*y_dim)*text_sprite_rgb->width+x_+ox*x_dim];
            if(!color.rgb.a)
               continue;
            for(int m = 0;m<scale;m++)
               for(int o = 0;o<scale;o++)
                  SLK_draw_rgb_color(x+sx+(x_*scale)+o,y+sy+(y_*scale)+m,color);
         }
      }
      sx+=x_dim*scale;
   }
}

//Draws a sprite to the draw target.
//SoftLK does NOT do alpha blending in the software
//accelerated layers, but if you draw alpha,
//alpha blending will be applied to the layers below
//the current one.
void SLK_draw_rgb_sprite(const SLK_RGB_sprite *s, int x, int y)
{
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = s->width;
   int draw_end_y = s->height;

   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x+draw_end_x>target_rgb->width)
      draw_end_x = s->width+(target_rgb->width-x-draw_end_x);
   if(y+draw_end_y>target_rgb->height)
      draw_end_y = s->height+(target_rgb->height-y-draw_end_y);
    
   for(int y1 = draw_start_y;y1<draw_end_y;y1++)
   {
      for(int x1 = draw_start_x;x1<draw_end_x;x1++)
      {
         SLK_Color c = s->data[y1*s->width+x1];
         if(c.rgb.a)
            target_rgb->data[(y1+y)*target_rgb->width+x1+x] = c;
      }
   }
}

//Draws a specified part of a sprite, can be
//used for sprite sheets.
//There is no performance benefitt in doing
//so since rgb drawing is software accelerated 
//and the sprite data is located in RAM rather
//than in VRAM.
void SLK_draw_rgb_sprite_partial(const SLK_RGB_sprite *s, int x, int y, int ox, int oy, int width, int height)
{
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = width;
   int draw_end_y = height;

   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x+draw_end_x>target_rgb->width)
      draw_end_x = width+(target_rgb->width-x-draw_end_x);
   if(y+draw_end_y>target_rgb->height)
      draw_end_y = height+(target_rgb->height-y-draw_end_y);
    
   for(int y1 = draw_start_y;y1<draw_end_y;y1++)
   {
      for(int x1 = draw_start_x;x1<draw_end_x;x1++)
      {
         SLK_Color c = s->data[(y1+oy)*s->width+x1+ox];
         int index = (y1+y)*target_rgb->width+x1+x;
         if(c.rgb.a)
            target_rgb->data[index] = c;
      }
   }
}

//Draws a flipped sprite.
//Pass SLK_FLIP_VERTICAL for vertical flipping and
//SLK_FLIP_HORIZONTAL for horizontal flipping.
void SLK_draw_rgb_sprite_flip(const SLK_RGB_sprite *s, int x, int y, SLK_flip flip)
{
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = s->width;
   int draw_end_y = s->height;

   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x+draw_end_x>target_rgb->width)
      draw_end_x = s->width+(target_rgb->width-x-draw_end_x);
   if(y+draw_end_y>target_rgb->height)
      draw_end_y = s->height+(target_rgb->height-y-draw_end_y);

   switch(flip)
   {
   case SLK_FLIP_NONE:
      for(int y1 = draw_start_y;y1<draw_end_y;y1++)
      {
         for(int x1 = draw_start_x;x1<draw_end_x;x1++)
         {
            SLK_Color c = s->data[y1*s->width+x1];
            if(c.rgb.a)
               target_rgb->data[(y1+y)*target_rgb->width+x1+x] = c;
         }
      }
      break;
   case SLK_FLIP_VERTICAL:
      for(int y1 = draw_start_y;y1<draw_end_y;y1++)
      {
         for(int x1 = draw_start_x;x1<draw_end_x;x1++)
         {
            SLK_Color c = s->data[(s->height-y1-1)*s->width+x1];
            if(c.rgb.a)
               target_rgb->data[(y1+y)*target_rgb->width+x1+x] = c;
         }
      }
      break;
   case SLK_FLIP_HORIZONTAL:
      for(int y1 = draw_start_y;y1<draw_end_y;y1++)
      {
         for(int x1 = draw_start_x;x1<draw_end_x;x1++)
         {
            SLK_Color c = s->data[y1*s->width+(s->width-x1-1)];
            if(c.rgb.a)
               target_rgb->data[(y1+y)*target_rgb->width+x1+x] = c;
         }
      }
      break;
   case SLK_FLIP_VERTHOR:
      for(int y1 = draw_start_y;y1<draw_end_y;y1++)
      {
         for(int x1 = draw_start_x;x1<draw_end_x;x1++)
         {
            SLK_Color c = s->data[(s->height-y1-1)*s->width+(s->width-x1-1)];
            if(c.rgb.a)
               target_rgb->data[(y1+y)*target_rgb->width+x1+x] = c;
         }
      }
      break;
   }
}

//Draws a line between to points
//using the Bresenham line drawing algorythm.
void SLK_draw_rgb_line(int x0, int y0, int x1, int y1, SLK_Color color)
{
   if(x0>x1||y0>y1)
   {
      SWAP(x0,x1);
      SWAP(y0,y1);
   }
   int dx = x1-x0;
   int ix = (dx>0)-(dx<0);
   dx = abs(dx)<<1;
   int dy = y1-y0;
   int iy = (dy>0)-(dy<0);
   dy = abs(dy)<<1;
   SLK_draw_rgb_color(x0,y0,color);

   if(dx>=dy)
   {
      int error = dy-(dx>>1);
      while(x0!=x1)
      {
         if(error>0||(!error&&ix>0))
         {
            error-=dx;
            y0+=iy;
         }

         error+=dy;
         x0+=ix;

         SLK_draw_rgb_color(x0,y0,color);
      }
   }
   else
   {
      int error = dx-(dy>>1);

      while(y0!=y1)
      {
         if(error>0||(!error&&iy>0))
         {
            error-=dy;
            x0+=ix;
         }

         error+=dx;
         y0+=iy;

         SLK_draw_rgb_color(x0,y0,color);
      }
   }
}

//Draws a line between two points up two but not including the second point
//with fixed x coordinates.
void SLK_draw_rgb_vertical_line(int x, int y0, int y1, SLK_Color color)
{
   if(x<0||x>=target_rgb->width||y0>=target_rgb->height||y1<0)
      return;
   if(y0<0)
      y0 = 0;
   if(y1>target_rgb->height)
      y1 = target_rgb->height;

   for(int y = y0;y<y1;y++)
      target_rgb->data[y*target_rgb->width+x] = color;
}

//Draws a line between two points up two but not including the second point
//with fixed y coordinates.
void SLK_draw_rgb_horizontal_line(int x0, int x1, int y, SLK_Color color)
{
   if(y<0||y>=target_rgb->height||x0>=target_rgb->width||x1<0)
      return;
   if(x0<0)
      x0 = 0;
   if(x1>target_rgb->width)
      x1 = target_rgb->width;

   for(int x = x0;x<x1;x++)
      target_rgb->data[y*target_rgb->width+x] = color;
}

//Draws the outline of a colored rectangle.
void SLK_draw_rgb_rectangle(int x, int y, int width, int height, SLK_Color color)
{
   SLK_draw_rgb_horizontal_line(x,x+width,y,color);
   SLK_draw_rgb_horizontal_line(x,x+width,y+height-1,color);
   SLK_draw_rgb_vertical_line(x,y,y+height,color);
   SLK_draw_rgb_vertical_line(x+width-1,y,y+height-1,color);
}

//Draws a solid colored rectangle.
void SLK_draw_rgb_fill_rectangle(int x, int y, int width, int height, SLK_Color color)
{
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = width;
   int draw_end_y = height;

   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x+draw_end_x>target_rgb->width)
      draw_end_x = width+(target_rgb->width-x-draw_end_x);
   if(y+draw_end_y>target_rgb->height)
      draw_end_y = height+(target_rgb->height-y-draw_end_y);
    
   for(int y1 = draw_start_y;y1<draw_end_y;y1++)
      for(int x1 = draw_start_x;x1<draw_end_x;x1++)
         target_rgb->data[(y1+y)*target_rgb->width+x1+x] = color;
}

//Draws the outline of a colored circle.
void SLK_draw_rgb_circle(int x, int y, int radius, SLK_Color color)
{
   int x_ = 0;
   int y_ = radius;
   int d = 1-radius;

   SLK_draw_rgb_color(x,y+radius,color);
   SLK_draw_rgb_color(x,y-radius,color);
   SLK_draw_rgb_color(x+radius,y,color);
   SLK_draw_rgb_color(x-radius,y,color);

   while(x_<y_)
   {
      if(d<0)
      {
         d = d+2*x_+3;
         x_+=1;
      }
      else
      {
         d = d+2*(x_-y_)+5;
         x_+=1;
         y_-=1;
      }

      SLK_draw_rgb_color(x+x_,y+y_,color);
      SLK_draw_rgb_color(x+x_,y-y_,color);
      SLK_draw_rgb_color(x-x_,y+y_,color);
      SLK_draw_rgb_color(x-x_,y-y_,color);

      SLK_draw_rgb_color(x+y_,y+x_,color);
      SLK_draw_rgb_color(x+y_,y-x_,color);
      SLK_draw_rgb_color(x-y_,y+x_,color);
      SLK_draw_rgb_color(x-y_,y-x_,color);
   }
}

//Draws a solid colored circle.
void SLK_draw_rgb_fill_circle(int x, int y, int radius, SLK_Color color)
{
   int x_ = 0;
   int y_ = radius;
   int d = 1-radius;

   SLK_draw_rgb_horizontal_line(x-radius,x+radius,y,color);

   while(x_<y_)
   {
      if(d<0)
      {
         d = d+2*x_+3;
         x_+=1;
      }
      else
      {
         d = d+2*(x_-y_)+5;
         x_+=1;
         y_-=1;
      }

      SLK_draw_rgb_horizontal_line(x-x_,x+x_,y+y_,color);
      SLK_draw_rgb_horizontal_line(x-x_,x+x_,y-y_,color);
      SLK_draw_rgb_horizontal_line(x-y_,x+y_,y+x_,color);
      SLK_draw_rgb_horizontal_line(x-y_,x+y_,y-x_,color);
   }
}

void SLK_log(const char *w, ...)
{
   va_list args;
   va_start(args,w);
   backend_log(w,args);
   va_end(args);
}

//Returns wether the key belonging to 
//the SLK_key enum is held.
int SLK_key_down(SLK_key key)
{
   return backend_key_down(key);
}

//Returns wether the key belonging to 
//the SLK_key enum has been pressed.
int SLK_key_pressed(SLK_key key)
{
   return backend_key_pressed(key);
}

//Returns wether the key belonging to 
//the SLK_key enum has been released.
int SLK_key_released(SLK_key key)
{
   return backend_key_released(key);
}

//Returns the requested keys state
SLK_Button SLK_key_get_state(SLK_key key)
{
   return backend_key_get_state(key);
}

//Returns wether the button belonging to 
//the SLK_button enum is held on the specified gamepad.
int SLK_gamepad_down(int id, SLK_gamepad_button key)
{
   return backend_gamepad_down(id,key);
}

//Returns wether the button belonging to 
//the SLK_button enum has been pressed on the specified gamepad.
int SLK_gamepad_pressed(int id, SLK_gamepad_button key)
{
   return backend_gamepad_pressed(id,key);
}

//Returns wether the button belonging to 
//the SLK_button enum has been released on the specified gamepad.
int SLK_gamepad_released(int id, SLK_gamepad_button key)
{
   return backend_gamepad_released(id,key);
}

//Returns the requested gamepad buttons state
SLK_Button SLK_gamepad_get_state(int id, SLK_gamepad_button key)
{
   return backend_gamepad_get_state(id,key);
}

//Returns how many gamepads are currently plugged in.
int SLK_gamepad_count()
{
   return backend_get_gamepad_count();
}

//Returns wether the mouse button
//refered to by SLK_mouse_button enum
//is held.
int SLK_mouse_down(SLK_mouse_button key)
{
   return backend_mouse_down(key);
}

//Returns wether the mouse button
//refered to by SLK_mouse_button enum
//has been pressed.
int SLK_mouse_pressed(SLK_mouse_button key)
{
   return backend_mouse_pressed(key);
}

//Returns wether the mouse button
//refered to by SLK_mouse_button enum
//has been released.
int SLK_mouse_released(SLK_mouse_button key)
{
   return backend_mouse_released(key);
}

//Returns the requested buttons state
SLK_Button SLK_mouse_get_state(SLK_mouse_button key)
{
   return backend_mouse_get_state(key);
}

//Returns the amount the mouse wheel has been scrolled.
//Negative: towards the user.
int SLK_mouse_wheel_get_scroll()
{
   return backend_mouse_wheel_get_scroll();
}

//Stores the current mouse position
//in the provided pointers.
void SLK_mouse_get_pos(int *x, int *y)
{
   backend_mouse_get_pos(x,y);
}

//Stores the mouse position relative
//to the last position in the 
//provided pointers.
void SLK_mouse_get_relative_pos(int *x, int *y)
{
   backend_mouse_get_relative_pos(x,y);
}

//Gets the mouse position relative to a layer.
//Layer scaling and position are being considered.
void SLK_mouse_get_layer_pos(unsigned index, int *x, int *y)
{
   int mouse_x, mouse_y;
   SLK_mouse_get_pos(&mouse_x,&mouse_y);

   if(index<layer_count)
   {
      *x = mouse_x;
      *y = mouse_y;

      *x-=layers[index].x;
      *y-=layers[index].y;

      *x/=layers[index].scale;
      *y/=layers[index].scale;
   }
}

//Sets wether the cursor should be shown.
void SLK_mouse_show_cursor(int shown)
{
   backend_show_cursor(shown);
}

//Sets wether the mouse cursor is captured and only relative
//mouse motion is registerd.
void SLK_mouse_set_relative(int relative)
{
   backend_mouse_set_relative(relative);
}

//Sets wether to capture mouse events globally.
void SLK_mouse_capture(int capture)
{
   backend_mouse_capture(capture);
}

//Starts text ínput and appends the characters
//to the provided char pointer.
//Note: SoftLK does not allocate new memory,
//you need to do that yourself.
void SLK_text_input_start(char *text, int max_length)
{
   backend_start_text_input(text,max_length);
}

//Stops the text input.
void SLK_text_input_stop()
{
   backend_stop_text_input();
}

static uint32_t rgb332_int[256] = 
{
   0xFF000000, 0xFF000020, 0xFF000040, 0xFF000060, 0xFF000080, 0xFF0000A0, 0xFF0000C0, 0xFF0000E0, 0xFF002000, 0xFF002020, 0xFF002040, 0xFF002060, 0xFF002080, 0xFF0020A0, 0xFF0020C0, 0xFF0020E0,
   0xFF004000, 0xFF004020, 0xFF004040, 0xFF004060, 0xFF004080, 0xFF0040A0, 0xFF0040C0, 0xFF0040E0, 0xFF006000, 0xFF006020, 0xFF006040, 0xFF006060, 0xFF006080, 0xFF0060A0, 0xFF0060C0, 0xFF0060E0,
   0xFF008000, 0xFF008020, 0xFF008040, 0xFF008060, 0xFF008080, 0xFF0080A0, 0xFF0080C0, 0xFF0080E0, 0xFF00A000, 0xFF00A020, 0xFF00A040, 0xFF00A060, 0xFF00A080, 0xFF00A0A0, 0xFF00A0C0, 0xFF00A0E0,
   0xFF00C000, 0xFF00C020, 0xFF00C040, 0xFF00C060, 0xFF00C080, 0xFF00C0A0, 0xFF00C0C0, 0xFF00C0E0, 0xFF00E000, 0xFF00E020, 0xFF00E040, 0xFF00E060, 0xFF00E080, 0xFF00E0A0, 0xFF00E0C0, 0xFF00E0E0,
   0xFF400000, 0xFF400020, 0xFF400040, 0xFF400060, 0xFF400080, 0xFF4000A0, 0xFF4000C0, 0xFF4000E0, 0xFF402000, 0xFF402020, 0xFF402040, 0xFF402060, 0xFF402080, 0xFF4020A0, 0xFF4020C0, 0xFF4020E0,
   0xFF404000, 0xFF404020, 0xFF404040, 0xFF404060, 0xFF404080, 0xFF4040A0, 0xFF4040C0, 0xFF4040E0, 0xFF406000, 0xFF406020, 0xFF406040, 0xFF406060, 0xFF406080, 0xFF4060A0, 0xFF4060C0, 0xFF4060E0,
   0xFF408000, 0xFF408020, 0xFF408040, 0xFF408060, 0xFF408080, 0xFF4080A0, 0xFF4080C0, 0xFF4080E0, 0xFF40A000, 0xFF40A020, 0xFF40A040, 0xFF40A060, 0xFF40A080, 0xFF40A0A0, 0xFF40A0C0, 0xFF40A0E0,
   0xFF40C000, 0xFF40C020, 0xFF40C040, 0xFF40C060, 0xFF40C080, 0xFF40C0A0, 0xFF40C0C0, 0xFF40C0E0, 0xFF40E000, 0xFF40E020, 0xFF40E040, 0xFF40E060, 0xFF40E080, 0xFF40E0A0, 0xFF40E0C0, 0xFF40E0E0,
   0xFF800000, 0xFF800020, 0xFF800040, 0xFF800060, 0xFF800080, 0xFF8000A0, 0xFF8000C0, 0xFF8000E0, 0xFF802000, 0xFF802020, 0xFF802040, 0xFF802060, 0xFF802080, 0xFF8020A0, 0xFF8020C0, 0xFF8020E0, 
   0xFF804000, 0xFF804020, 0xFF804040, 0xFF804060, 0xFF804080, 0xFF8040A0, 0xFF8040C0, 0xFF8040E0, 0xFF806000, 0xFF806020, 0xFF806040, 0xFF806060, 0xFF806080, 0xFF8060A0, 0xFF8060C0, 0xFF8060E0, 
   0xFF808000, 0xFF808020, 0xFF808040, 0xFF808060, 0xFF808080, 0xFF8080A0, 0xFF8080C0, 0xFF8080E0, 0xFF80A000, 0xFF80A020, 0xFF80A040, 0xFF80A060, 0xFF80A080, 0xFF80A0A0, 0xFF80A0C0, 0xFF80A0E0,
   0xFF80C000, 0xFF80C020, 0xFF80C040, 0xFF80C060, 0xFF80C080, 0xFF80C0A0, 0xFF80C0C0, 0xFF80C0E0, 0xFF80E000, 0xFF80E020, 0xFF80E040, 0xFF80E060, 0xFF80E080, 0xFF80E0A0, 0xFF80E0C0, 0xFF80E0E0,
   0xFFC00000, 0xFFC00020, 0xFFC00040, 0xFFC00060, 0xFFC00080, 0xFFC000A0, 0xFFC000C0, 0xFFC000E0, 0xFFC02000, 0xFFC02020, 0xFFC02040, 0xFFC02060, 0xFFC02080, 0xFFC020A0, 0xFFC020C0, 0xFFC020E0,
   0xFFC04000, 0xFFC04020, 0xFFC04040, 0xFFC04060, 0xFFC04080, 0xFFC040A0, 0xFFC040C0, 0xFFC040E0, 0xFFC06000, 0xFFC06020, 0xFFC06040, 0xFFC06060, 0xFFC06080, 0xFFC060A0, 0xFFC060C0, 0xFFC060E0, 
   0xFFC08000, 0xFFC08020, 0xFFC08040, 0xFFC08060, 0xFFC08080, 0xFFC080A0, 0xFFC080C0, 0xFFC080E0, 0xFFC0A000, 0xFFC0A020, 0xFFC0A040, 0xFFC0A060, 0xFFC0A080, 0xFFC0A0A0, 0xFFC0A0C0, 0xFFC0A0E0, 
   0xFFC0C000, 0xFFC0C020, 0xFFC0C040, 0xFFC0C060, 0xFFC0C080, 0xFFC0C0A0, 0xFFC0C0C0, 0xFFC0C0E0, 0xFFC0E000, 0xFFC0E020, 0xFFC0E040, 0xFFC0E060, 0xFFC0E080, 0xFFC0E0A0, 0xFFC0E0C0, 0xFFC0E0E0,
};
static SLK_Palette *rgb332 = (SLK_Palette*)rgb332_int;

//Initializes the requested layer to the specified type.
//These types are currently availible:
//SLK_LAYER_RGB,
//SLK_LAYER_PAL
void SLK_layer_create(unsigned index, SLK_layer type)
{
   if(index>=layer_count)
      return;

   layers[index].type = type;
   layers[index].active = 1;
   layers[index].tint = SLK_color_create(255,255,255,255);
   layers[index].x = 0;
   layers[index].y = 0;
   layers[index].scale = 1.0f;

   switch(type)
   {
   case SLK_LAYER_PAL:
      {
         int screen_width = backend_get_width();
         int screen_height = backend_get_height();
         layers[index].as.type_0.target = SLK_pal_sprite_create(screen_width,screen_height);
         layers[index].as.type_0.render = SLK_rgb_sprite_create(screen_width,screen_height);
         backend_create_layer(index,type);
         layers[index].as.type_0.palette = rgb332;
      }
      break;
   case SLK_LAYER_RGB:
      {
         int screen_width = backend_get_width();
         int screen_height = backend_get_height();
         layers[index].as.type_1.target = SLK_rgb_sprite_create(screen_width,screen_height);
         backend_create_layer(index,type);
      }
      break;
   }
}

//Sets wether the layers is supposed to be drawn.
void SLK_layer_activate(unsigned index, int active)
{
   if(index>=layer_count)
      return;

   layers[index].active = active;
}

//Sets the palette of a layer.
//Only works for SLK_LAYER_PAL layer type.
void SLK_layer_set_palette(unsigned index, SLK_Palette *pal)
{
   if(index>=layer_count||layers[index].type!=SLK_LAYER_PAL)
      return;

   if(pal==NULL)
      layers[index].as.type_0.palette = rgb332;
   else
      layers[index].as.type_0.palette = pal;
}

//Sets the tint a layers is supposed to be drawn with.
void SLK_layer_set_tint(unsigned index, SLK_Color tint)
{
   if(index<layer_count)
      layers[index].tint = tint;
}

//Sets wether the layers should be resized on 
//window resize.
void SLK_layer_set_dynamic(unsigned index, int dynamic)
{
   if(index<layer_count)
      layers[index].dynamic = dynamic;
}

//Sets the position the layers is supposed to be
//drawn at.
void SLK_layer_set_pos(unsigned index, int x, int y)
{
   if(index<layer_count)
   {
      layers[index].x = x;
      layers[index].y = y;
   }
}

//Sets the factor the layer is supposed to be scale
//with when drawn.
void SLK_layer_set_scale(unsigned index, float scale)
{
   if(index<layer_count)
      layers[index].scale = scale;
}

//Sets the size of a non
//dynamic layer.
void SLK_layer_set_size(unsigned index, int width, int height)
{
   if(index<layer_count)
   {
      layers[index].resized = 1;

      if(layers[index].type==SLK_LAYER_PAL)
      {
         if(layers[index].as.type_0.target==NULL||layers[index].as.type_0.render==NULL)
         {
            SLK_log("layer %d has not been created yet",index);
            return;
         }

         SLK_Pal_sprite *sprite_new = SLK_pal_sprite_create(width,height);
         
         SLK_rgb_sprite_destroy(layers[index].as.type_0.render);
         layers[index].as.type_0.render = SLK_rgb_sprite_create(width,height);

         SLK_pal_sprite_copy(sprite_new,layers[index].as.type_0.target);
         SLK_pal_sprite_destroy(layers[index].as.type_0.target);
         layers[index].as.type_0.target = sprite_new;
      }
      else if(layers[index].type==SLK_LAYER_RGB)
      {
         if(layers[index].as.type_1.target==NULL)
         {
            SLK_log("layer %d has not been created yet",index);
            return;
         }

         SLK_RGB_sprite *sprite_new = SLK_rgb_sprite_create(width,height);
         sprite_new->changed = layers[index].as.type_1.target->changed;

         SLK_rgb_sprite_copy(sprite_new,layers[index].as.type_1.target);
         SLK_rgb_sprite_destroy(layers[index].as.type_1.target);
         layers[index].as.type_1.target = sprite_new;
      }

      SLK_layer_set_current(index);
   }
}

//Stores the dimensions of the specified layer in the specified pointers.
void SLK_layer_get_size(unsigned index, int *width, int *height)
{
   if(index>=layer_count)
      return;

   if(layers[index].type==SLK_LAYER_RGB)
   {
      if(width)
         *width = layers[index].as.type_1.target->width;
      if(height)
         *height = layers[index].as.type_1.target->height;
   }
   else if(layers[index].type==SLK_LAYER_PAL)
   {
      if(width)
         *width = layers[index].as.type_0.target->width;
      if(height)
         *height = layers[index].as.type_0.target->height;
   } 
}

//Sets wich layer is the current default draw target.
//Also overwrites the current draw target.
void SLK_layer_set_current(unsigned index)
{
   if(index>=layer_count)
      return;

   current_layer = &layers[index];

   if(layers[index].type==SLK_LAYER_PAL)
   {
      target_pal_default = layers[index].as.type_0.target;
      target_pal = layers[index].as.type_0.target;
   }
   else if(layers[index].type==SLK_LAYER_RGB)
   {
      target_rgb_default = layers[index].as.type_1.target;
      target_rgb = layers[index].as.type_1.target;
   }
}

//Returns wether the layer has been resized.
//A layer counts as resized between a SLK_layer_set_size call and a SLK_render_update call.
int SLK_layer_get_resized(unsigned index)
{
   if(index>=layer_count)
      return 0;

   return layers[index].resized;
}

//Returns the specified layer if it
//exists.
SLK_Layer *SLK_layer_get(unsigned index)
{
   if(index>=layer_count)
      return NULL;

   return &layers[index];
}

//Reads a palette from a .pal file.
SLK_Palette *SLK_palette_load(const char *path)
{
   return backend_load_palette(path);
}

//Reads a palette from a .pal file.
SLK_Palette *SLK_palette_load_file(FILE *f)
{
   return backend_load_palette_file(f);
}

//Writes a palette to a .pal file
void SLK_palette_save(const char *path, const SLK_Palette *pal)
{
   backend_save_palette(path,pal);
}

//Writes a palette to a .pal file
void SLK_palette_save_file(FILE *f, const SLK_Palette *pal)
{
   backend_save_palette_file(f,pal);
}

//Sets the color of a palette at the
//desired index.
//Usefull for rapidly chaning certain colors of a sprite,
//eg. for simple animations.
void SLK_palette_set_color(SLK_Palette *palette, int index, SLK_Color color)
{
   if(index>=0&&index<256)
      palette->colors[index] = color;
}

//Returns the color of a palette at the
//desired index.
SLK_Color SLK_palette_get_color(const SLK_Palette *palette, int index)
{
   if(index>=0&&index<256)
      return palette->colors[index];
   else
      return SLK_color_create(0,0,0,255);
}

//Creates a sprite with the requested dimensions
//and returns a pointer to its location.
SLK_Pal_sprite *SLK_pal_sprite_create(int width, int height)
{
   SLK_Pal_sprite *s = backend_malloc(sizeof(*s));
   if(s==NULL)
      SLK_log("malloc of size %zu failed, out of memory!",sizeof(*s));

   s->width = width;
   s->height = height;

   s->data = backend_malloc(width*height*sizeof(*s->data));
   if(s->data==NULL)
      SLK_log("malloc of size %zu failed, out of memory!",width*height*sizeof(*s->data));

   memset(s->data,0,sizeof(*s->data)*width*height);

   return s;
}

//Destroys a previously allocated sprite.
//The user may not now that s->data also 
//needs to be freed.
void SLK_pal_sprite_destroy(SLK_Pal_sprite *s)
{
   if(s==NULL)
      return;

   backend_free(s->data);
   backend_free(s);
}

//Returns the index at the specified
//location.
uint8_t SLK_pal_sprite_get_index(const SLK_Pal_sprite *s, int x, int y)
{
   if(INBOUNDS(0,s->width,x)&&INBOUNDS(0,s->height,y))
      return s->data[y*s->width+x];
   else
      return 0;
}

//Sets the index at the specified
//position.
void SLK_pal_sprite_set_index(SLK_Pal_sprite *s, int x, int y, uint8_t c)
{
   if(INBOUNDS(0,s->width,x)&&INBOUNDS(0,s->height,y))
      s->data[y*s->width+x] = c;
}

//Loads a sprite from a .slk file.
SLK_Pal_sprite *SLK_pal_sprite_load(const char *path)
{
   return backend_load_pal(path);
}

//Loads a sprite from a .slk file.
SLK_Pal_sprite *SLK_pal_sprite_load_file(FILE *f)
{
   return backend_load_pal_file(f);
}

//Loads a sprite from a .slk file.
SLK_Pal_sprite *SLK_pal_sprite_load_mem(const void *data, int length)
{
   return backend_load_pal_mem(data,length);
}

//Writes a sprite to a file
//with the above specified
//layout.
//rle modes:
//0: No RLE
//1: RLE for indices
void SLK_pal_sprite_save(const char *path, const SLK_Pal_sprite *s, int rle)
{
   backend_save_pal(s,path,rle);
}

//Writes a sprite to a file stream
//with the above specified
//layout.
//rle modes:
//0: No RLE
//1: RLE for indices
void SLK_pal_sprite_save_file(FILE *f, const SLK_Pal_sprite *s, int rle)
{
   backend_save_pal_file(s,f,rle);
}

//Copies a specified part of the data of a sprite 
//to another one.
//Usefull for splitting a texture atlas in individual sprites.
void SLK_pal_sprite_copy_partial(SLK_Pal_sprite *dst, const SLK_Pal_sprite *src, int x, int y, int ox, int oy, int width, int height)
{
   for(int tx = 0; tx < width; tx++)
   {
      for(int ty = 0; ty < height; ty++)
      {
         uint8_t c = SLK_pal_sprite_get_index(src, tx + ox, ty + oy);
         SLK_pal_sprite_set_index(dst,x + tx, y +ty, c);
      }
   }
}

//Copies the data from a sprite to another one.
//Usefull for duplicating sprites.
void SLK_pal_sprite_copy(SLK_Pal_sprite *dst, const SLK_Pal_sprite *src)
{
   for(int x = 0;x<src->width;x++)
      for(int y = 0;y<src->height;y++)
         SLK_pal_sprite_set_index(dst,x,y,SLK_pal_sprite_get_index(src,x,y));
}

//Creates a sprites with the specified dimensions and clear it.
SLK_RGB_sprite *SLK_rgb_sprite_create(int width, int height)
{   
   SLK_RGB_sprite *s = backend_malloc(sizeof(*s));
   if(s==NULL)
      SLK_log("malloc of size %zu failed, out of memory!",sizeof(*s));
   
   s->width = width;
   s->height = height;
   
   s->data = backend_malloc(width*height*sizeof(*s->data));
   if(s->data==NULL)
      SLK_log("malloc of size %zu failed, out of memory!",width*height*sizeof(*s->data));

   memset(s->data,0,width*height*sizeof(*s->data));
    
   return s;
}

//Destroys a previously allocated sprite.
//The user may not know that s->data also needs
//to be freed.
void SLK_rgb_sprite_destroy(SLK_RGB_sprite *s)
{
   if(s==NULL)
      return;

   backend_free(s->data);
   backend_free(s);
}

//Returns the color at the specified position of a sprite.
SLK_Color SLK_rgb_sprite_get_pixel(const SLK_RGB_sprite *s, int x, int y)
{
   if(INBOUNDS(0,s->width,x)&&INBOUNDS(0,s->height,y))
      return s->data[y*s->width+x];
   else
      return SLK_color_create(0,0,0,0);
}

//Sets the color of a sprite at the specified position.
//Similar to SLK_draw_rgb_color, but ignores alpha value.
void SLK_rgb_sprite_set_pixel(SLK_RGB_sprite *s, int x, int y, SLK_Color c)
{
   if(INBOUNDS(0,s->width,x)&&INBOUNDS(0,s->height,y))
      s->data[y*s->width+x] = c;
}

//Loads a sprite from a png file.
SLK_RGB_sprite *SLK_rgb_sprite_load(const char *path)
{
   return backend_load_rgb(path);
}

//Loads a sprite from a png file.
SLK_RGB_sprite *SLK_rgb_sprite_load_file(FILE *f)
{
   return backend_load_rgb_file(f);
}

//Loads a sprite from a png file.
SLK_RGB_sprite *SLK_rgb_sprite_load_mem(const void *data, int length)
{
   return backend_load_rgb_mem(data, length);
}

///Writes a sprite to a png file.
void SLK_rgb_sprite_save(const char *path, const SLK_RGB_sprite *s)
{
   backend_save_rgb(s,path);
}

///Writes a sprite to a png file.
void SLK_rgb_sprite_save_file(FILE *f, const SLK_RGB_sprite *s)
{
   backend_save_rgb_file(s,f);
}

//Copies the data of a sprite to another one.
//Usefull for duplicating sprites.
void SLK_rgb_sprite_copy(SLK_RGB_sprite *dst, const SLK_RGB_sprite *src)
{
   for(int x = 0;x<src->width;x++)
      for(int y = 0;y<src->height;y++)
         SLK_rgb_sprite_set_pixel(dst,x,y,SLK_rgb_sprite_get_pixel(src,x,y));
}

//Copies a specified part of a sprite to another one.
//Usefull for creating sprite sheets.
void SLK_rgb_sprite_copy_partial(SLK_RGB_sprite *dst, const SLK_RGB_sprite *src, int x, int y, int ox, int oy, int width, int height)
{
   for(int tx = 0;tx<width;tx++)
      for(int ty = 0;ty<height;ty++)
         SLK_rgb_sprite_set_pixel(dst,x+tx,y+ty,SLK_rgb_sprite_get_pixel(src,ox+tx,oy+ty));
}

//Sets the target fps.
//Pass a value of 0 or lower to set maximum fps.
//Hardlimited to 1000 fps because SDL_GetTicks can't go
//smaller than milliseconds.
void SLK_timer_set_fps(int FPS)
{
   backend_set_fps(FPS);
}

//Returns the currently targeted fps.
//Don't know how this could be usefull,
//but anyway, here it is.
int SLK_timer_get_fps()
{
   return backend_get_fps();
}

//Updates the timings and sleeps
//the needed amount of time.
//Already gets called in SLK_update,
//only use if you know
//what you are doing.
void SLK_timer_update()
{
   backend_timer_update();
}

//Returns the time the last frame has taken
//in seconds.
//SLK is designed to use fixed framerates,
//but if you desire to do something else
//I won't stop you.
float SLK_timer_get_delta()
{
   return backend_timer_get_delta();
}

//BACKEND
//-----------------------------------------------

#if LIBSLK_BACKEND == 0
#include "libSLK_sdl.h"
#elif LIBSLK_BACKEND == 1
#include "libSLK_win.h"
#endif
//-----------------------------------------------

#undef SWAP
#undef INBOUNDS
#undef SIGNUM

#endif
#endif
