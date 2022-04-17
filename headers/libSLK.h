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

#include <SDL2/SDL.h>

#define CUTE_PNG_IMPLEMENTATION
#define CUTE_PNG_ALLOC backend_malloc
#define CUTE_PNG_CALLOC backend_calloc
#define CUTE_PNG_FREE backend_free
#include "../external/cute_png.h"
//https://github.com/RandyGaul/cute_headers

#define HLH_SLK_IMPLEMENTATION
#define HLH_SLK_MALLOC backend_malloc
#define HLH_SLK_FREE backend_free
#include "../external/HLH_slk.h"

#define MAX_CONTROLLERS 4

typedef struct
{
   SDL_GameController *gamepad;
   SDL_Haptic *haptic;
   SDL_JoystickID id;
   int connected;
   uint8_t new_button_state[SLK_PAD_MAX];
   uint8_t old_button_state[SLK_PAD_MAX];
}Gamepad;

static SDL_Window *sdl_window;
static SDL_Renderer *renderer;
static SDL_Texture **layer_textures;
static int screen_width;
static int screen_height;
static int pixel_scale;
static int window_width;
static int window_height;
static int view_x;
static int view_y;
static int view_width;
static int view_height;
static int fps;
static int frametime;
static int framedelay;
static int framestart;
static float delta;
static uint8_t key_map[SDL_NUM_SCANCODES];
static uint8_t mouse_map[6];
static uint8_t gamepad_map[SDL_CONTROLLER_BUTTON_MAX];
static uint8_t new_key_state[SLK_KEY_MAX];
static uint8_t old_key_state[SLK_KEY_MAX];
static uint8_t new_mouse_state[SLK_BUTTON_MAX];
static uint8_t old_mouse_state[SLK_BUTTON_MAX];
static Gamepad gamepads[MAX_CONTROLLERS];
static int mouse_x_rel;
static int mouse_y_rel;
static char *text_input;
static int text_input_active;
static int text_input_max;
static int mouse_x;
static int mouse_y;
static int mouse_wheel;

static void *(*bmalloc)(size_t size) = backend_system_malloc;
static void (*bfree)(void *ptr) = backend_system_free;
static void *(*brealloc)(void *ptr, size_t size) = backend_system_realloc;

static int get_gamepad_index(int which);

//(should) center the viewport.
void backend_update_viewport()
{
   SDL_GetWindowSize(sdl_window,&window_width,&window_height);

   if(layer_dynamic)
   {
      view_width = window_width;
      view_height = window_height;
      view_x = 0;
      view_y = 0;
   }
   else
   {
      view_width = screen_width*pixel_scale;
      view_height = screen_height*pixel_scale;

      view_x = (window_width-view_width)/2;
      view_y = (window_height-view_height)/2;
   }

   SDL_Rect v;
   v.x = view_x;
   v.y = view_y;
   v.w = view_width;
   v.h = view_height;
   if(SDL_RenderSetViewport(renderer,&v)<0)
      SLK_log("failed to set render viewport: %s",SDL_GetError());
}

//Handles window and input events.
void backend_handle_events()
{
   mouse_wheel = 0;
   memcpy(old_key_state,new_key_state,sizeof(new_key_state));
   memcpy(old_mouse_state,new_mouse_state,sizeof(new_mouse_state));
   for(int i = 0;i<MAX_CONTROLLERS;i++)
      memcpy(gamepads[i].old_button_state,gamepads[i].new_button_state,sizeof(gamepads[0].new_button_state));

   //Event managing
   SDL_Event event;
   while(SDL_PollEvent(&event))
   {
      switch(event.type)
      {
      case SDL_QUIT:
         SLK_core_quit();
         break;
      case SDL_KEYDOWN:
         if(text_input_active&&event.key.keysym.sym==SDLK_BACKSPACE&&text_input[0]!='\0')
            text_input[strlen(text_input)-1] = '\0';
         if(event.key.state==SDL_PRESSED)
            new_key_state[key_map[event.key.keysym.scancode]] = 1;
         break;
      case SDL_KEYUP:
         if(event.key.state==SDL_RELEASED)
            new_key_state[key_map[event.key.keysym.scancode]] = 0;
         break;
      case SDL_MOUSEBUTTONDOWN:
         if(event.button.state==SDL_PRESSED)
            new_mouse_state[mouse_map[event.button.button]] = 1;
         break;
      case SDL_MOUSEBUTTONUP:
         if(event.button.state==SDL_RELEASED)
            new_mouse_state[mouse_map[event.button.button]] = 0;
         break;       
      case SDL_TEXTINPUT:
         if(text_input_active&&strlen(text_input)+strlen(event.text.text)<text_input_max)
            strcat(text_input,event.text.text);
         break;
      case SDL_MOUSEWHEEL:
         mouse_wheel = event.wheel.y;
         break;
      case SDL_CONTROLLERBUTTONDOWN:
         if(event.cbutton.state==SDL_PRESSED)
         {
            int id = get_gamepad_index(event.cbutton.which);
            gamepads[id].new_button_state[gamepad_map[event.cbutton.button]] = 1;
         }
         break;
      case SDL_CONTROLLERBUTTONUP:
         if(event.cbutton.state==SDL_RELEASED)
         {
            int id = get_gamepad_index(event.cbutton.which);
            gamepads[id].new_button_state[gamepad_map[event.cbutton.button]] = 0;
         }
         break;
      case SDL_CONTROLLERDEVICEADDED:
         {
            int which = event.cdevice.which;
            if(which<MAX_CONTROLLERS)
            {
               gamepads[which].gamepad = SDL_GameControllerOpen(which);
               gamepads[which].connected = 1;
               SDL_Joystick *j = SDL_GameControllerGetJoystick(gamepads[which].gamepad);
               gamepads[which].id = SDL_JoystickInstanceID(j);
            }
         }
         break;
      case SDL_CONTROLLERDEVICEREMOVED:
         {
            int which = event.cdevice.which;
            if(which<0)
               break;
            int id = get_gamepad_index(which);
            gamepads[id].connected = 0;
            SDL_GameControllerClose(gamepads[id].gamepad);
         }
         break;
      case SDL_WINDOWEVENT:
         if(event.window.event==SDL_WINDOWEVENT_RESIZED)
         {
            if(layer_dynamic)
            {
               int new_width = event.window.data1/pixel_scale+1;
               int new_height = event.window.data2/pixel_scale+1;
               screen_width = new_width;
               screen_height = new_height;

               for(int l = 0;l<layer_count;l++)
               {
                  if(layers[l].dynamic)
                     SLK_layer_set_size(l,new_width,new_height);
               }
            }
         }
         backend_update_viewport();
         break;
      }
   }
   //-------------------------------------------
   
   int x,y;
   SDL_GetMouseState(&x,&y);

   x-=view_x;
   y-=view_y;
   mouse_x = x/pixel_scale;
   mouse_y = y/pixel_scale;

   SDL_GetRelativeMouseState(&mouse_x_rel,&mouse_y_rel);
   mouse_x_rel = mouse_x_rel/pixel_scale;
   mouse_y_rel = mouse_y_rel/pixel_scale;

   if(mouse_x>=screen_width)
     mouse_x = screen_width-1;
   if(mouse_y>=screen_height)
     mouse_y = screen_height-1;

   if(mouse_x<0)
     mouse_x = 0;
   if(mouse_y<0)
     mouse_y = 0;
}

//Creates the window, etc.
void backend_setup(int width, int height, int layer_num, const char *title, int fullscreen, int scale, int resizable)
{
   pixel_scale = scale;
   screen_width = width;
   screen_height = height;
   layer_count = layer_num;
   layer_dynamic = resizable;

   Uint32 flags = 
#ifndef __EMSCRIPTEN__
   SDL_INIT_EVERYTHING;
#else
   SDL_INIT_VIDEO|SDL_INIT_EVENTS;
#endif

   if(SDL_Init(flags)<0)
   {
      SLK_log_line("SDL_Init","%s",SDL_GetError());
      exit(-1);
   }

   if(pixel_scale==SLK_WINDOW_MAX)
   {
      SDL_Rect max_size;

      if(SDL_GetDisplayUsableBounds(0,&max_size)<0)
      {
         SLK_log_line("SDL_GetDisplayUsableBounds","%s",SDL_GetError());
      }
      else
      {
         int max_x,max_y;

         max_x = max_size.w/screen_width;
         max_y = max_size.h/screen_height;

         pixel_scale = (max_x>max_y)?max_y:max_x;
      }
      
   }

   if(pixel_scale<=0)
      pixel_scale = 1;

   if(resizable)
      sdl_window = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width*pixel_scale,height*pixel_scale,SDL_WINDOW_RESIZABLE);
   else
      sdl_window = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width*pixel_scale,height*pixel_scale,0);

   if(sdl_window==NULL)
   {
      SLK_log_line("SDL_CreateWindow","%s",SDL_GetError());
      exit(-1);
   }

   renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
   if(!renderer)
   {
      SLK_log_line("SDL_CreateRenderer","%s",SDL_GetError());
      exit(-1);
   }

   SDL_SetRenderDrawColor(renderer,0,0,0,0);

   layer_textures = backend_malloc(sizeof(*layer_textures)*layer_num);
   if(layer_textures==NULL)
      SLK_log("malloc of size %zu failed, out of memory!",sizeof(*layer_textures)*layer_num);

   memset(layer_textures,0,sizeof(*layer_textures)*layer_num);
   backend_update_viewport();
}

//Clears the window and redraws the scene.
//Drawing is performed from back to front, layer 0 is always drawn last.
void backend_render_update()
{
   SDL_RenderClear(renderer);

   for(int l = layer_count-1;l>=0;l--)
   {
      layers[l].resized = 0;

      if(layers[l].active)
      {
         switch(layers[l].type)
         {
         case SLK_LAYER_PAL:
         {
            float width = (float)layers[l].as.type_0.target->width*layers[l].scale*pixel_scale;
            float height = (float)layers[l].as.type_0.target->height*layers[l].scale*pixel_scale;
            float x = (float)layers[l].x*pixel_scale;
            float y = (float)layers[l].y*pixel_scale;
            SDL_Rect dst_rect;
            dst_rect.x = x;
            dst_rect.y = y;
            dst_rect.w = width;
            dst_rect.h = height;

            for(int i = 0;i<layers[l].as.type_0.render->width*layers[l].as.type_0.render->height;i++)
               layers[l].as.type_0.render->data[i] = layers[l].as.type_0.palette->colors[layers[l].as.type_0.target->data[i]];

            int w, h;
            SDL_QueryTexture(layer_textures[l], NULL, NULL, &w, &h);

            if(w!=layers[l].as.type_0.target->width||h!=layers[l].as.type_0.target->height)
            {
               SDL_DestroyTexture(layer_textures[l]);
               layer_textures[l] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,layers[l].as.type_0.target->width,layers[l].as.type_0.target->height);
               SDL_SetTextureBlendMode(layer_textures[l],SDL_BLENDMODE_BLEND);
            }

            void *data;
            int stride;
            SDL_LockTexture(layer_textures[l],NULL,&data,&stride);
            memcpy(data,layers[l].as.type_0.render->data,sizeof(*layers[l].as.type_0.render->data)*layers[l].as.type_0.target->width*layers[l].as.type_0.target->height);
            SDL_UnlockTexture(layer_textures[l]);

            SDL_SetTextureColorMod(layer_textures[l],layers[l].tint.rgb.r,layers[l].tint.rgb.g,layers[l].tint.rgb.b);
            SDL_SetTextureAlphaMod(layer_textures[l],layers[l].tint.rgb.a);
            SDL_RenderCopy(renderer,layer_textures[l],NULL,&dst_rect);

            break;
         }
         case SLK_LAYER_RGB:
         {
            int width = (float)layers[l].as.type_1.target->width*layers[l].scale*pixel_scale;
            int height = (float)layers[l].as.type_1.target->height*layers[l].scale*pixel_scale;
            int x = (float)layers[l].x*pixel_scale;
            int y = (float)layers[l].y*pixel_scale;
            SDL_Rect dst_rect;
            dst_rect.x = x;
            dst_rect.y = y;
            dst_rect.w = width;
            dst_rect.h = height;

            if(layers[l].as.type_1.target->changed)
            {
               int w, h;
               SDL_QueryTexture(layer_textures[l], NULL, NULL, &w, &h);

               if(w!=layers[l].as.type_1.target->width||h!=layers[l].as.type_1.target->height)
               {
                  SDL_DestroyTexture(layer_textures[l]);
                  layer_textures[l] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,layers[l].as.type_1.target->width,layers[l].as.type_1.target->height);
                  SDL_SetTextureBlendMode(layer_textures[l],SDL_BLENDMODE_BLEND);
               }
               void *data;
               int stride;
               SDL_LockTexture(layer_textures[l],NULL,&data,&stride);
               memcpy(data,layers[l].as.type_1.target->data,sizeof(*layers[l].as.type_1.target->data)*layers[l].as.type_1.target->width*layers[l].as.type_1.target->height);
               SDL_UnlockTexture(layer_textures[l]);
               layers[l].as.type_1.target->changed = 0;
            }
            SDL_SetTextureColorMod(layer_textures[l],layers[l].tint.rgb.r,layers[l].tint.rgb.g,layers[l].tint.rgb.b);
            SDL_SetTextureAlphaMod(layer_textures[l],layers[l].tint.rgb.a);
            SDL_RenderCopy(renderer,layer_textures[l],NULL,&dst_rect);

            break;
         }
         }
      }
   }
   
   SDL_RenderPresent(renderer);
}

void backend_create_layer(unsigned index, int type)
{
   if(index>=layer_count)
      return;

   switch(type)
   {
   case SLK_LAYER_PAL:
      layer_textures[index] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,screen_width,screen_height);
      if(layer_textures[index]==NULL)
         SLK_log("failed to create texture for layer %d: %s",index,SDL_GetError());

      if(SDL_SetTextureBlendMode(layer_textures[index],SDL_BLENDMODE_BLEND)<0)
         SLK_log("failed to set texture blend mode: %s",SDL_GetError());
      break;
   case SLK_LAYER_RGB:
      layer_textures[index] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,screen_width,screen_height);
      if(layer_textures[index]==NULL)
         SLK_log("failed to create texture for layer %d: %s",index,SDL_GetError());

      if(SDL_SetTextureBlendMode(layer_textures[index],SDL_BLENDMODE_BLEND)<0)
         SLK_log("failed to set texture blend mode: %s",SDL_GetError());
      break;
   }
}

//Set the window title.
void backend_set_title(const char *title)
{
   SDL_SetWindowTitle(sdl_window,title);
}

//Toggles fullscreen.
void backend_set_fullscreen(int fullscreen)
{
   if(fullscreen)
   {
      if(SDL_SetWindowFullscreen(sdl_window,SDL_WINDOW_FULLSCREEN_DESKTOP)!=0)
         SLK_log("failed to fullscreen window: %s",SDL_GetError());
   }
   else
   {
      if(SDL_SetWindowFullscreen(sdl_window,0)!=0)
         SLK_log("failed to exit fullscreen: %s",SDL_GetError());

      SDL_SetWindowSize(sdl_window,screen_width*pixel_scale,screen_height*pixel_scale);
   }

   backend_update_viewport();
}

//Sets wether the window is visible.
void backend_set_visible(int visible)
{
   if(visible)
      SDL_ShowWindow(sdl_window);
   else
      SDL_HideWindow(sdl_window);
}

//Sets the window icon.
void backend_set_icon(const SLK_RGB_sprite *icon)
{
   SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(icon->data,icon->width,icon->height,32,icon->width*4,0xf000,0x0f00,0x00f0,0x000f);
   if(surface==NULL)
   {
      SLK_log("failed to create sdl surface from rgb sprite: %s",SDL_GetError());
      return;
   }

   SDL_SetWindowIcon(sdl_window,surface);
   SDL_FreeSurface(surface);
}

//Returns the viewport width adjusted to pixel scale.
int backend_get_width()
{
   return screen_width;
}

//Returns the viewport height adjusted to pixel scale.
int backend_get_height()
{
   return screen_height;
}

//Returns the view width.
int backend_get_view_width()
{
   return view_width;
}

//Returns the view height.
int backend_get_view_height()
{
   return view_height;
}

//Returns the view x pos.
int backend_get_view_x()
{
   return view_x;
}

//Returns the view y pos.
int backend_get_view_y()
{
   return view_y;
}

//Returns the window width.
int backend_get_win_width()
{
   return window_width;
}

//Returns the window height.
int backend_get_win_height()
{
   return window_height;
}

//Sets the target/maximum fps.
void backend_set_fps(int FPS)
{
   if(FPS<1||FPS>1000)
      fps = 1000;
   else
      fps = FPS;

   framedelay = 1000/fps;
}

//Returns the current target fps.
int backend_get_fps()
{
   return fps;
}

//Limits the fps to the target fps.
void backend_timer_update()
{
   frametime = SDL_GetTicks()-framestart;

   if(framedelay>frametime)
      SDL_Delay(framedelay-frametime);

   delta = (float)(SDL_GetTicks()-framestart)/1000.0f;
   framestart = SDL_GetTicks();
}

//Returns the delta time of the last frame.
float backend_timer_get_delta()
{
   return delta;
}

//Creates the keymap.
void backend_input_init()
{
   key_map[0x00] = SLK_KEY_NONE;
   key_map[SDL_SCANCODE_A] = SLK_KEY_A;
   key_map[SDL_SCANCODE_B] = SLK_KEY_B;
   key_map[SDL_SCANCODE_C] = SLK_KEY_C;
   key_map[SDL_SCANCODE_D] = SLK_KEY_D;
   key_map[SDL_SCANCODE_E] = SLK_KEY_E;
   key_map[SDL_SCANCODE_F] = SLK_KEY_F;
   key_map[SDL_SCANCODE_G] = SLK_KEY_G;
   key_map[SDL_SCANCODE_H] = SLK_KEY_H;
   key_map[SDL_SCANCODE_I] = SLK_KEY_I;
   key_map[SDL_SCANCODE_J] = SLK_KEY_J;
   key_map[SDL_SCANCODE_K] = SLK_KEY_K;
   key_map[SDL_SCANCODE_L] = SLK_KEY_L;
   key_map[SDL_SCANCODE_M] = SLK_KEY_M;
   key_map[SDL_SCANCODE_N] = SLK_KEY_N;
   key_map[SDL_SCANCODE_O] = SLK_KEY_O;
   key_map[SDL_SCANCODE_P] = SLK_KEY_P;
   key_map[SDL_SCANCODE_Q] = SLK_KEY_Q;
   key_map[SDL_SCANCODE_R] = SLK_KEY_R;
   key_map[SDL_SCANCODE_S] = SLK_KEY_S;
   key_map[SDL_SCANCODE_T] = SLK_KEY_T;
   key_map[SDL_SCANCODE_U] = SLK_KEY_U;
   key_map[SDL_SCANCODE_V] = SLK_KEY_V;
   key_map[SDL_SCANCODE_W] = SLK_KEY_W;
   key_map[SDL_SCANCODE_X] = SLK_KEY_X;
   key_map[SDL_SCANCODE_Y] = SLK_KEY_Y;
   key_map[SDL_SCANCODE_Z] = SLK_KEY_Z;

   key_map[SDL_SCANCODE_F1] = SLK_KEY_F1;
   key_map[SDL_SCANCODE_F2] = SLK_KEY_F2;
   key_map[SDL_SCANCODE_F3] = SLK_KEY_F3;
   key_map[SDL_SCANCODE_F4] = SLK_KEY_F4;
   key_map[SDL_SCANCODE_F5] = SLK_KEY_F5;
   key_map[SDL_SCANCODE_F6] = SLK_KEY_F6;
   key_map[SDL_SCANCODE_F7] = SLK_KEY_F7;
   key_map[SDL_SCANCODE_F8] = SLK_KEY_F8;
   key_map[SDL_SCANCODE_F9] = SLK_KEY_F9;
   key_map[SDL_SCANCODE_F10] = SLK_KEY_F10;
   key_map[SDL_SCANCODE_F11] = SLK_KEY_F11;
   key_map[SDL_SCANCODE_F12] = SLK_KEY_F12;

   key_map[SDL_SCANCODE_DOWN] = SLK_KEY_DOWN;
   key_map[SDL_SCANCODE_LEFT] = SLK_KEY_LEFT;
   key_map[SDL_SCANCODE_RIGHT] = SLK_KEY_RIGHT;
   key_map[SDL_SCANCODE_UP] = SLK_KEY_UP;
   key_map[SDL_SCANCODE_RETURN] = SLK_KEY_ENTER;

   key_map[SDL_SCANCODE_BACKSPACE] = SLK_KEY_BACK;
   key_map[SDL_SCANCODE_ESCAPE] = SLK_KEY_ESCAPE;
   key_map[SDL_SCANCODE_TAB] = SLK_KEY_TAB;
   key_map[SDL_SCANCODE_LGUI] = SLK_KEY_HOME;
   key_map[SDL_SCANCODE_END] = SLK_KEY_END;
   key_map[SDL_SCANCODE_PAGEUP] = SLK_KEY_PGUP;
   key_map[SDL_SCANCODE_PAGEDOWN] = SLK_KEY_PGDN;
   key_map[SDL_SCANCODE_INSERT] = SLK_KEY_INS;
   key_map[SDL_SCANCODE_LSHIFT] = SLK_KEY_SHIFT;
   key_map[SDL_SCANCODE_RSHIFT] = SLK_KEY_SHIFT;
   key_map[SDL_SCANCODE_LCTRL] = SLK_KEY_CTRL;
   key_map[SDL_SCANCODE_RCTRL] = SLK_KEY_CTRL;
   key_map[SDL_SCANCODE_SPACE] = SLK_KEY_SPACE;

   key_map[SDL_SCANCODE_0] = SLK_KEY_0;
   key_map[SDL_SCANCODE_1] = SLK_KEY_1;
   key_map[SDL_SCANCODE_2] = SLK_KEY_2;
   key_map[SDL_SCANCODE_3] = SLK_KEY_3;
   key_map[SDL_SCANCODE_4] = SLK_KEY_4;
   key_map[SDL_SCANCODE_5] = SLK_KEY_5;
   key_map[SDL_SCANCODE_6] = SLK_KEY_6;
   key_map[SDL_SCANCODE_7] = SLK_KEY_7;
   key_map[SDL_SCANCODE_8] = SLK_KEY_8;
   key_map[SDL_SCANCODE_9] = SLK_KEY_9;

   key_map[SDL_SCANCODE_KP_0] = SLK_KEY_NP0;
   key_map[SDL_SCANCODE_KP_1] = SLK_KEY_NP1;
   key_map[SDL_SCANCODE_KP_2] = SLK_KEY_NP2;
   key_map[SDL_SCANCODE_KP_3] = SLK_KEY_NP3;
   key_map[SDL_SCANCODE_KP_4] = SLK_KEY_NP4;
   key_map[SDL_SCANCODE_KP_5] = SLK_KEY_NP5;
   key_map[SDL_SCANCODE_KP_6] = SLK_KEY_NP6;
   key_map[SDL_SCANCODE_KP_7] = SLK_KEY_NP7;
   key_map[SDL_SCANCODE_KP_8] = SLK_KEY_NP8;
   key_map[SDL_SCANCODE_KP_9] = SLK_KEY_NP9;
   key_map[SDL_SCANCODE_KP_MULTIPLY] = SLK_KEY_NP_MUL;
   key_map[SDL_SCANCODE_KP_PLUS] = SLK_KEY_NP_ADD;
   key_map[SDL_SCANCODE_KP_DIVIDE] = SLK_KEY_NP_DIV;
   key_map[SDL_SCANCODE_KP_MINUS] = SLK_KEY_NP_SUB;
   key_map[SDL_SCANCODE_KP_PERIOD] = SLK_KEY_NP_DECIMAL;

   mouse_map[SDL_BUTTON_LEFT] = SLK_BUTTON_LEFT;
   mouse_map[SDL_BUTTON_RIGHT] = SLK_BUTTON_RIGHT;
   mouse_map[SDL_BUTTON_MIDDLE] = SLK_BUTTON_MIDDLE;
   mouse_map[SDL_BUTTON_X1] = SLK_BUTTON_X1;
   mouse_map[SDL_BUTTON_X2] = SLK_BUTTON_X2;

   gamepad_map[SDL_CONTROLLER_BUTTON_A] = SLK_PAD_A;
   gamepad_map[SDL_CONTROLLER_BUTTON_B] = SLK_PAD_B;
   gamepad_map[SDL_CONTROLLER_BUTTON_X] = SLK_PAD_X;
   gamepad_map[SDL_CONTROLLER_BUTTON_Y] = SLK_PAD_Y;
   gamepad_map[SDL_CONTROLLER_BUTTON_BACK] = SLK_PAD_BACK;
   gamepad_map[SDL_CONTROLLER_BUTTON_GUIDE] = SLK_PAD_GUIDE;
   gamepad_map[SDL_CONTROLLER_BUTTON_START] = SLK_PAD_START;
   gamepad_map[SDL_CONTROLLER_BUTTON_LEFTSTICK] = SLK_PAD_LEFTSTICK;
   gamepad_map[SDL_CONTROLLER_BUTTON_RIGHTSTICK] = SLK_PAD_RIGHTSTICK;
   gamepad_map[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = SLK_PAD_LEFTSHOULDER;
   gamepad_map[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = SLK_PAD_RIGHTSHOULDER;
   gamepad_map[SDL_CONTROLLER_BUTTON_DPAD_UP] = SLK_PAD_UP;
   gamepad_map[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = SLK_PAD_DOWN;
   gamepad_map[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = SLK_PAD_LEFT;
   gamepad_map[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = SLK_PAD_RIGHT;

   //Clear key states, just in case,
   //should already be empty since known at compile time
   memset(new_key_state,0,sizeof(new_key_state));
   memset(old_key_state,0,sizeof(old_key_state));
   memset(new_mouse_state,0,sizeof(new_mouse_state));
   memset(old_mouse_state,0,sizeof(old_mouse_state));
   for(int i = 0;i<MAX_CONTROLLERS;i++)
   {
      memset(gamepads[i].new_button_state,0,sizeof(gamepads[i].new_button_state));
      memset(gamepads[i].old_button_state,0,sizeof(gamepads[i].old_button_state));
   }
}

//Shows or hides the mouse cursor.
void backend_show_cursor(int shown)
{
   if(SDL_ShowCursor(shown?SDL_ENABLE:SDL_DISABLE)<0)
      SLK_log("failed to show/hide cursor: %s",SDL_GetError());
}

//Sets wether the mouse cursor is captured and only relative
//mouse motion is registerd.
void backend_mouse_set_relative(int relative)
{
   if(SDL_SetRelativeMouseMode(relative)<0)
      SLK_log("failed to set relative mouse mode: %s",SDL_GetError());
}

//Sets wether to track mouse events globally.
void backend_mouse_capture(int capture)
{
   if(SDL_CaptureMouse(capture)<0)
      SLK_log("failed to capture/release mouse: %s",SDL_GetError());
}

//Starts text input.
void backend_start_text_input(char *text, int max_length)
{
   text_input = text;
   text_input_active = 1;
   text_input_max = max_length;

   SDL_StartTextInput();
}

//Stops text input.
void backend_stop_text_input()
{
   text_input_active = 0;
   
   SDL_StopTextInput();
}

int backend_key_down(int key)
{
   return new_key_state[key];
}

int backend_key_pressed(int key)
{
   return new_key_state[key]&&!old_key_state[key];
}

int backend_key_released(int key)
{
   return !new_key_state[key]&&old_key_state[key];
}

SLK_Button backend_key_get_state(int key)
{
   SLK_Button out;
   out.pressed = new_key_state[key]&&!old_key_state[key];
   out.released = !new_key_state[key]&&old_key_state[key];
   out.held = new_key_state[key];

   return out;
}

int backend_mouse_down(int key)
{
   return new_mouse_state[key];
}

int backend_mouse_pressed(int key)
{
   return new_mouse_state[key]&&!old_mouse_state[key];
}

int backend_mouse_released(int key)
{
   return !new_mouse_state[key]&&old_mouse_state[key];
}

SLK_Button backend_mouse_get_state(int key)
{
   SLK_Button out;
   out.pressed = new_mouse_state[key]&&!old_mouse_state[key];
   out.held = new_mouse_state[key];
   out.released = !new_mouse_state[key]&&old_mouse_state[key];

   return out;
}

int backend_mouse_wheel_get_scroll()
{
   return mouse_wheel;
}

int backend_gamepad_down(int index, int key)
{
   return gamepads[index].new_button_state[key];
}

int backend_gamepad_pressed(int index, int key)
{
   return gamepads[index].new_button_state[key]&&!gamepads[index].old_button_state[key];
}

int backend_gamepad_released(int index, int key)
{
   return !gamepads[index].new_button_state[key]&&gamepads[index].old_button_state[key];
}

SLK_Button backend_gamepad_get_state(int index, int key)
{
   SLK_Button out;
   out.pressed = gamepads[index].new_button_state[key]&&!gamepads[index].old_button_state[key];
   out.held = gamepads[index].new_button_state[key];
   out.released = !gamepads[index].new_button_state[key]&&gamepads[index].old_button_state[key];

   return out;
}

int backend_get_gamepad_count()
{
   return SDL_NumJoysticks();
}

void backend_mouse_get_pos(int *x, int *y)
{
   *x = mouse_x;
   *y = mouse_y;
}

void backend_mouse_get_relative_pos(int *x, int *y)
{
   *x = mouse_x_rel;
   *y = mouse_y_rel;
}

static int get_gamepad_index(int which)
{

   for(int i = 0;i<MAX_CONTROLLERS;i++)
      if(gamepads[i].connected&&gamepads[i].id==which)
         return i;

   return -1;
}

SLK_RGB_sprite *backend_load_rgb(const char *path)
{
   cp_image_t img = cp_load_png(path);
   SLK_RGB_sprite *out = NULL;
   if(img.pix==NULL)
   {
      SLK_log("failed to load png %s\n",path);
      return NULL;
   }

   out = SLK_rgb_sprite_create(img.w,img.h);
   memcpy(out->data,img.pix,img.w*img.h*sizeof(*out->data));
   cp_free_png(&img);

   return out;
}

SLK_RGB_sprite *backend_load_rgb_file(FILE *f)
{
   int size = 0;
   fseek(f,0,SEEK_END);
   size = ftell(f);
   fseek(f,0,SEEK_SET);
   char *data = backend_malloc(size+1);
   fread(data,size,1,f);
   data[size] = 0;
   SLK_RGB_sprite *out = backend_load_rgb_mem(data,size);
   backend_free(data);

   return out;
}

SLK_RGB_sprite *backend_load_rgb_mem(const void *data, int length)
{
   cp_image_t img = cp_load_png_mem(data,length);
   SLK_RGB_sprite *out = NULL;
   if(img.pix==NULL)
   {
      SLK_log("failed to load png from memory buffer");
      return NULL;
   }

   out = SLK_rgb_sprite_create(img.w,img.h);
   memcpy(out->data,img.pix,img.w*img.h*sizeof(*out->data));
   cp_free_png(&img);

   return out;
}


void backend_save_rgb(const SLK_RGB_sprite *s, const char *path)
{
   if(path==NULL)
      return;
   FILE *f = fopen(path,"wb");
   if(f==NULL)
      SLK_log("failed to open %s for writing",path);

   backend_save_rgb_file(s,f);

   fclose(f);
}

void backend_save_rgb_file(const SLK_RGB_sprite *s, FILE *f)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't write png to disk");
      return;
   }

   cp_image_t img;
   img.w = s->width;
   img.h = s->height;
   img.pix = (cp_pixel_t *)s->data;
   cp_save_png(f,&img);
}

SLK_Pal_sprite *backend_load_pal(const char *path)
{
   FILE *f = fopen(path,"rb");
   if(f==NULL)
   {
      SLK_log("failed to open %s for reading",path);
      return NULL;
   }

   SLK_Pal_sprite *out = backend_load_pal_file(f);
   fclose(f);
   return out;
}

SLK_Pal_sprite *backend_load_pal_file(FILE *f)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't read .slk file");
      return NULL;
   }

   int size = 0;
   char *data = NULL;
   fseek(f,0,SEEK_END);
   size = ftell(f);
   fseek(f,0,SEEK_SET);
   data = backend_malloc(size+1);
   fread(data,size,1,f);
   data[size] = 0;
   SLK_Pal_sprite *s = backend_load_pal_mem(data,size);
   backend_free(data);

   return s;
}

SLK_Pal_sprite *backend_load_pal_mem(const void *data, int length)
{
   HLH_slk *img = HLH_slk_image_load_mem_buffer(data,length);
   SLK_Pal_sprite *s = SLK_pal_sprite_create(img->width,img->height);
   memcpy(s->data,img->data,sizeof(*s->data)*s->width*s->height);
   HLH_slk_image_free(img);

   return s;
}

void backend_save_pal(const SLK_Pal_sprite *s, const char *path, int rle)
{
   FILE *f = fopen(path,"wb");

   if(f==NULL)
   {
      SLK_log("failed to open %s for writing",path);
      return;
   }

   backend_save_pal_file(s,f,rle);
      
   fclose(f);
}

void backend_save_pal_file(const SLK_Pal_sprite *s, FILE *f, int rle)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't write palette to disk");
      return;
   }

   HLH_slk img;
   img.width = s->width;
   img.height = s->height;
   img.data = (uint8_t *)s->data;
   HLH_slk_image_write(&img,f,rle);
}

SLK_Palette *backend_load_palette(const char *path)
{
   FILE *f = fopen(path,"r");
   if(f==NULL)
   {
      SLK_log("failed to open %s for reading",path);
      return NULL; 
   }

   SLK_Palette *palette = backend_load_palette_file(f);
   fclose(f);

   return palette;
}

void backend_save_palette(const char *path, const SLK_Palette *pal)
{
   FILE *f = fopen(path,"w");
   if(f==NULL)
   {
      SLK_log("failed to open %s for writing",path);
      return;
   }

   backend_save_palette_file(f,pal);

  fclose(f);
}

SLK_Palette *backend_load_palette_file(FILE *f)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't load palette");
      return NULL;
   }

   char buffer[512];
   int colors = 0,i,found;
   int r,g,b,a;

   SLK_Palette *palette = backend_malloc(sizeof(*palette));
   if(palette==NULL)
      SLK_log("malloc of size %zu failed, out of memory!",sizeof(*palette));

   memset(palette,0,sizeof(*palette));
   for(i = 0;i<259&&fgets(buffer,512,f);i++)
   {
      if(i==2)
      {
         sscanf(buffer,"%d",&found);
         palette->used = found;
      }
      else if(i>2&&buffer[0]!='\0')
      {
         if(sscanf(buffer,"%d %d %d %d",&r,&g,&b,&a)!=4)
         {
            sscanf(buffer,"%d %d %d",&r,&g,&b);
            a = 255;
         }

         palette->colors[colors].rgb.r = r;
         palette->colors[colors].rgb.g = g;
         palette->colors[colors].rgb.b = b;
         palette->colors[colors].rgb.a = a;
         colors++;
      }
   }

   return palette;
}

void backend_save_palette_file(FILE *f, const SLK_Palette *pal)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't write palette to disk");
      return;
   }

   fprintf(f,"JASC-PAL\n0100\n%d\n",pal->used);
   for(int i = 0;i<pal->used;i++)
   {
      if(pal->colors[i].rgb.a!=255)
         fprintf(f,"%d %d %d %d\n",pal->colors[i].rgb.r,pal->colors[i].rgb.g,pal->colors[i].rgb.b,pal->colors[i].rgb.a);
      else
         fprintf(f,"%d %d %d\n",pal->colors[i].rgb.r,pal->colors[i].rgb.g,pal->colors[i].rgb.b);
   }
}

void *backend_system_malloc(size_t size)
{
   return malloc(size);
}

void backend_system_free(void *ptr)
{
   free(ptr);
}

void *backend_system_realloc(void *ptr, size_t size)
{
   return realloc(ptr,size);
}

void backend_set_malloc(void *(*func)(size_t size))
{
   bmalloc = func;
}

void backend_set_free(void (*func)(void *ptr))
{
   bfree = func;
}

void backend_set_realloc(void *(*func)(void *ptr, size_t size))
{
   brealloc = func;
}

void *backend_malloc(size_t size)
{
   return bmalloc(size);
}

void *backend_calloc(size_t num, size_t size)
{
   void *mem = backend_malloc(num*size);
   memset(mem,0,num*size);
   return mem;
}

void backend_free(void *ptr)
{
   bfree(ptr);
}

void *backend_realloc(void *ptr, size_t size)
{
   return brealloc(ptr,size);
}

void backend_log(const char *w, va_list v)
{
   vprintf(w,v);
}

#undef MAX_CONTROLLERS
//-----------------------------------------------

#undef SWAP
#undef INBOUNDS
#undef SIGNUM

#endif
#endif
