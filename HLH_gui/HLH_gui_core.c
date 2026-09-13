/*
HLH_gui - gui framework

Written in 2023,2024,2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"
#define STBIW_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

#include "HLH.h"
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
#include "HLH_gui_internal.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
typedef struct
{
   int window_count;
   HLH_gui_window **windows;
   int scale;
   SDL_Surface *font_surface;

   uint32_t timer_event;
   uint32_t open_file_event;
   uint32_t open_folder_event;
   uint32_t save_file_event;

   HLH_gui_mouse mouse;
}Context;
//-------------------------------------

//Variables
static Context ctx = {.scale = 1};
//static int core_window_count;
//static HLH_gui_window **core_windows;
//static int core_scale = 1;
//static SDL_Surface *core_font_surface;
//uint32_t HLH_gui_timer_event = 0;
//static int textinput_count = 0;

static const uint64_t core_font[] =
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
//-------------------------------------

//Function prototypes
HLH_gui_window *core_find_window(SDL_Window *win);
static int64_t core_window_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp);

static void image_write_func(void *context, void *data, int size);
//-------------------------------------

//Function implementations

void HLH_gui_init(void)
{
   if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
      fprintf(stderr, "SDL_Init(): %s\n", SDL_GetError());

   //Create font from data in core_font
   ctx.font_surface = SDL_CreateSurface(1024, 16, SDL_PIXELFORMAT_RGBA8888);
   if(ctx.font_surface==NULL)
   {
      fprintf(stderr, "SDL_CreateSurface(): %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
   }

   if(!SDL_LockSurface(ctx.font_surface))
      fprintf(stderr, "SDL_LockSurface(): %s\n", SDL_GetError());
   for(int c = 0; c<128; c++)
   {
      int index = c * 2;
      for(int i = 0; i<128; i++)
      {
         int x = i & 7;
         int y = i / 8;
         int val = i / 64;
         int bit = i & 63;
         ((uint32_t *)ctx.font_surface->pixels)[y * (ctx.font_surface->pitch / 4) + x + c * 8] = (core_font[index + val] & ((uint64_t)1 << bit))?0xffffffff:0x0;
      }
   }
   SDL_UnlockSurface(ctx.font_surface);

   HLH_gui_set_scale(1);

   ctx.timer_event = SDL_RegisterEvents(1);
   ctx.open_file_event = SDL_RegisterEvents(1);
   ctx.save_file_event = SDL_RegisterEvents(1);
   ctx.open_folder_event = SDL_RegisterEvents(1);
}

HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height, const char *path_icon)
{
   HLH_gui_window *window = (HLH_gui_window *)HLH_gui_element_create(sizeof(*window), NULL, (HLH_gui_flags){}, core_window_msg);
   window->e.window = window;
   window->e.type = HLH_GUI_WINDOW;
   //window->hover = &window->e;
   window->width = width;
   window->height = height;
   window->blocking = NULL;
   ctx.window_count++;
   ctx.windows = realloc(ctx.windows, sizeof(*ctx.windows) * ctx.window_count);
   ctx.windows[ctx.window_count - 1] = window;

   if(!SDL_CreateWindowAndRenderer(title, 1, 1, SDL_WINDOW_RESIZABLE, &window->sdl_window, &window->sdl_renderer))
      fprintf(stderr, "SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
   SDL_SetWindowTitle(window->sdl_window, title);

   window->sdl_target = SDL_CreateTexture(window->sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, window->width, window->height);
   if(window->sdl_target==NULL)
      fprintf(stderr, "SDL_CreateTexture() failed: %s\n", SDL_GetError());

   window->sdl_overlay = SDL_CreateTexture(window->sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, window->width, window->height);
   if(window->sdl_overlay==NULL)
      fprintf(stderr, "SDL_CreateTexture(): %s\n", SDL_GetError());
   if(!SDL_SetTextureBlendMode(window->sdl_overlay, SDL_BLENDMODE_BLEND))
      fprintf(stderr, "SDL_SetTextureBlendMode() failed: %s\n", SDL_GetError());

   window->sdl_font = SDL_CreateTextureFromSurface(window->sdl_renderer, ctx.font_surface);
   if(window->sdl_font==NULL)
      fprintf(stderr, "SDL_CreateTextureFromSurface() failed: %s\n", SDL_GetError());

   if(!SDL_SetWindowSize(window->sdl_window, width, height))
   {
      fprintf(stderr, "SDL_SetWindowSize() failed: %s\n", SDL_GetError());
   }

   if(path_icon != NULL)
   {
      int w, h, n;
      unsigned char *data = stbi_load(path_icon, &w, &h, &n, 4);
      if(data!=NULL)
      {
         SDL_Surface *surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA8888, data, w * 4);
         window->sdl_icons = SDL_CreateTextureFromSurface(window->sdl_renderer, surface);
         SDL_SetTextureBlendMode(window->sdl_icons,SDL_BLENDMODE_BLEND);
         SDL_DestroySurface(surface);
         stbi_image_free(data);
      }
   }

   //Send fake resize event
   // Need this, since child elements don't exist at this
   // TODO: using a custom event for this purpose would be better
   SDL_Event e;
   e.type = SDL_EVENT_WINDOW_RESIZED;

   e.window.windowID = SDL_GetWindowID(window->sdl_window);
   if(e.window.windowID==0)
      fprintf(stderr, "SDL_GetWindowID(): %s\n", SDL_GetError());

   e.window.data1 = window->width;
   e.window.data2 = window->height;
   window->width = -1;
   window->height = -1;

   if(!SDL_PushEvent(&e))
      fprintf(stderr, "SDL_PushEvent(): %s\n", SDL_GetError());

   return window;
}

static void core_quit_event(void)
{
   for(int i = 0; i < ctx.window_count; i++)
   {
      HLH_gui_element_destroy(&ctx.windows[i]->e);
   }
}

static HLH_gui_window *core_exposed_event(SDL_WindowID window_id)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   if(!SDL_SetRenderTarget(win->sdl_renderer, NULL))
   {
      fprintf(stderr, "SDL_SetRenderTarget() failed: %s\n", SDL_GetError());
   }

   if(!SDL_RenderClear(win->sdl_renderer))
   {
      fprintf(stderr, "SDL_RenderClear() failed: %s\n", SDL_GetError());
   }

   if(!SDL_RenderTexture(win->sdl_renderer, win->sdl_target, NULL, NULL))
   {
      fprintf(stderr, "SDL_RenderTexture() failed: %s\n", SDL_GetError());
   }

   if(!SDL_RenderTexture(win->sdl_renderer, win->sdl_overlay, NULL, NULL))
   {
      fprintf(stderr, "SDL_RenderTexture() failed: %s\n", SDL_GetError());
   }

   if(!SDL_RenderPresent(win->sdl_renderer))
   {
      fprintf(stderr, "SDL_RenderPresent() failed: %s\n", SDL_GetError());
   }

   return win;
}

static HLH_gui_window *core_resized_event(SDL_WindowID window_id, int width, int height)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   SDL_FlushEvent(SDL_EVENT_WINDOW_RESIZED);
   SDL_FlushEvent(SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED);
   SDL_GetWindowSizeInPixels(win->sdl_window, &width, &height);
   if(win->width!=width||win->height!=height)
   {
      win->width = width;
      win->height = height;

      SDL_DestroyTexture(win->sdl_target);
      win->sdl_target = SDL_CreateTexture(win->sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, win->width, win->height);
      if(win->sdl_target==NULL)
         fprintf(stderr, "SDL_CreateTexture() failed: %s\n", SDL_GetError());

      SDL_DestroyTexture(win->sdl_overlay);
      win->sdl_overlay = SDL_CreateTexture(win->sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, win->width, win->height);
      if(win->sdl_overlay==NULL)
         fprintf(stderr, "SDL_CreateTexture() failed: %s\n", SDL_GetError());

      if(!SDL_SetTextureBlendMode(win->sdl_overlay, SDL_BLENDMODE_BLEND))
         fprintf(stderr, "SDL_SetTextureBlendMode() failed: %s\n", SDL_GetError());

      if(!SDL_SetRenderTarget(win->sdl_renderer, win->sdl_target))
         fprintf(stderr, "SDL_SetRenderTarget() failed: %s\n", SDL_GetError());

      win->e.bounds = HLH_gui_rect_make(0, 0, win->width, win->height);

      HLH_gui_element_layout(&win->e, win->e.bounds);
      HLH_gui_element_redraw(&win->e);
   }

   return win;
}

static HLH_gui_window *core_mouse_leave_event(SDL_WindowID window_id)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   ctx.mouse.pos[0]= -1;
   ctx.mouse.pos[1] = -1;
   ctx.mouse.wheel = 0;
   ctx.mouse.rel[0] = 0;
   ctx.mouse.rel[1] = 0;
   HLH_gui_handle_mouse(&win->e, ctx.mouse);

   return win;
}

static bool core_close_event(SDL_WindowID window_id)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return false;
   }

   //Close all if window 0, otherwise close current one
   if(win == ctx.windows[0])
   {
      for(int i = 0; i < ctx.window_count; i++)
      {
         HLH_gui_element_destroy(&ctx.windows[i]->e);
      }

      return true;
   }

   for(int i = 0; i < ctx.window_count; i++)
   {
      if(ctx.windows[i]->blocking == win)
         ctx.windows[i]->blocking = NULL;
   }

   for(int i = 0; i < ctx.window_count; i++)
   {
      if(ctx.windows[i]==win)
      {
         HLH_gui_element_destroy(&win->e);
         ctx.windows[i] = ctx.windows[ctx.window_count - 1];
         ctx.window_count--;
         ctx.windows = realloc(ctx.windows, sizeof(*ctx.windows) * ctx.window_count);
      }
   }

   return false;
}

static HLH_gui_window *core_mouse_motion_event(SDL_WindowID window_id, float xrel, float yrel, float x, float y)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   ctx.mouse.rel[0] = xrel;
   ctx.mouse.rel[1] = yrel;
   ctx.mouse.pos[0] = x;
   ctx.mouse.pos[1] = y;

   // Hack to prevent flooding event queue unless necessary
   if(!win->mouse_move_no_skip)
   {
      SDL_GetMouseState(&ctx.mouse.pos[0], &ctx.mouse.pos[1]);
      SDL_FlushEvent(SDL_EVENT_MOUSE_MOTION);
   }

   ctx.mouse.wheel = 0;
   HLH_gui_handle_mouse(&win->e, ctx.mouse);

   return win;
}

static HLH_gui_window *core_mouse_wheel_event(SDL_WindowID window_id, float wheel)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   ctx.mouse.wheel = wheel;
   HLH_gui_handle_mouse(&win->e, ctx.mouse);

   return win;
}

static HLH_gui_window *core_key_down_event(SDL_WindowID window_id, SDL_Keycode keycode,
                                           SDL_Scancode scancode, bool repeat)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   if(win->keyboard != NULL)
   {
      HLH_gui_textinput input;
      input.type = 1; // TODO: enum
      input.keycode = keycode;
      HLH_gui_element_msg(win->keyboard, HLH_GUI_MSG_TEXTINPUT, 0, &input);
   }
   else
   {
      if(repeat)
      {
         HLH_gui_element_msg_all(&win->e, HLH_GUI_MSG_BUTTON_REPEAT, scancode, NULL);
      }
      else
      {
         HLH_gui_element_msg_all(&win->e, HLH_GUI_MSG_BUTTON_DOWN, scancode, NULL);
      }
   }

   win = core_find_window(SDL_GetWindowFromID(window_id));
   return win;
}

static HLH_gui_window *core_key_up_event(SDL_WindowID window_id, SDL_Scancode scancode)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   HLH_gui_element_msg_all(&win->e, HLH_GUI_MSG_BUTTON_UP, scancode, NULL);

   win = core_find_window(SDL_GetWindowFromID(window_id));
   return win;
}

static HLH_gui_window *core_drop_file_event(SDL_WindowID window_id, const char *data)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   // TODO: HLH_string
   HLH_gui_element_msg(&win->e, HLH_GUI_MSG_DRAGNDROP, 0, (void *)data);

   return win;
}

static HLH_gui_window *core_button_down_event(SDL_WindowID window_id, float x, float y,
                                              uint8_t button, uint8_t clicks)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   ctx.mouse.pos[0] = x;
   ctx.mouse.pos[1] = y;
   switch(button)
   {
   case 1:
      ctx.mouse.button |= HLH_GUI_MOUSE_LEFT;
      if(clicks == 2)
      {
         ctx.mouse.button |= HLH_GUI_MOUSE_DBLE_LEFT;
      }
      break;
   case 3:
      ctx.mouse.button |= HLH_GUI_MOUSE_RIGHT;
      if(clicks == 2)
      {
         ctx.mouse.button |= HLH_GUI_MOUSE_DBLE_RIGHT;
      }
      break;
   case 2:
      ctx.mouse.button |= HLH_GUI_MOUSE_MIDDLE;
      break;
   case 4:
      ctx.mouse.button |= HLH_GUI_MOUSE_X1;
      break;
   case 5:
      ctx.mouse.button |= HLH_GUI_MOUSE_X2;
      break;
   }

   HLH_gui_handle_mouse(&win->e, ctx.mouse);

   win = core_find_window(SDL_GetWindowFromID(window_id));
   return win;
}

static HLH_gui_window *core_button_up_event(SDL_WindowID window_id, uint8_t button)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   switch(button)
   {
   case 1:
      ctx.mouse.button &= ~HLH_GUI_MOUSE_LEFT;
      ctx.mouse.button &= ~HLH_GUI_MOUSE_DBLE_LEFT;
      break;
   case 3:
      ctx.mouse.button &= ~HLH_GUI_MOUSE_RIGHT;
      ctx.mouse.button &= ~HLH_GUI_MOUSE_DBLE_RIGHT;
      break;
   case 2:
      ctx.mouse.button &= ~HLH_GUI_MOUSE_MIDDLE;
      break;
   case 4:
      ctx.mouse.button &= ~HLH_GUI_MOUSE_X1;
      break;
   case 5:
      ctx.mouse.button &= ~HLH_GUI_MOUSE_X2;
      break;
   }

   HLH_gui_handle_mouse(&win->e, ctx.mouse);

   win = core_find_window(SDL_GetWindowFromID(window_id));
   return win;
}

static HLH_gui_window *core_textinput_event(SDL_WindowID window_id, const char *text)
{
   HLH_gui_window *win = core_find_window(SDL_GetWindowFromID(window_id));
   if(win == NULL)
   {
      return NULL;
   }

   for(int i = 0; i < strlen(text); i += 1)
   {
      HLH_gui_textinput input;
      input.type = 0;
      input.ch = text[i];
      HLH_gui_element_msg(win->keyboard, HLH_GUI_MSG_TEXTINPUT, 0, &input);
   }

   win = core_find_window(SDL_GetWindowFromID(window_id));
   return win;
}

int HLH_gui_message_loop(void)
{
   for(;;)
   {
      SDL_Event event;
      if(!SDL_WaitEvent(&event))
      {
         fprintf(stderr, "SDL_WaitEvent() failed: %s\n", SDL_GetError());
      }

      HLH_gui_window *win = NULL;

      switch(event.type)
      {
      case SDL_EVENT_QUIT:
         core_quit_event();
         return 0;
      case SDL_EVENT_WINDOW_SHOWN:
      case SDL_EVENT_WINDOW_EXPOSED:
         win = core_exposed_event(event.window.windowID);
         break;
      case SDL_EVENT_WINDOW_RESIZED:
      case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
         win = core_resized_event(event.window.windowID, event.window.data1, event.window.data2);
         break;
      case SDL_EVENT_WINDOW_MOUSE_LEAVE:
         win = core_mouse_leave_event(event.window.windowID);
         break;
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      {
         bool quit = core_close_event(event.window.windowID);
         if(quit)
         {
            return 0;
         }
         break;
      }
      case SDL_EVENT_MOUSE_MOTION:
         win = core_mouse_motion_event(event.motion.windowID,
                                       event.motion.xrel, event.motion.yrel,
                                       event.motion.x, event.motion.y);
         break;
      case SDL_EVENT_MOUSE_WHEEL:
         win = core_mouse_wheel_event(event.wheel.windowID, event.wheel.y);
         break;
      case SDL_EVENT_KEY_DOWN:
         win = core_key_down_event(event.key.windowID, event.key.key, event.key.scancode, event.key.repeat);
         break;
      case SDL_EVENT_KEY_UP:
         win = core_key_up_event(event.key.windowID, event.key.scancode);
         break;
      case SDL_EVENT_DROP_FILE:
         win = core_drop_file_event(event.drop.windowID, event.drop.data);
         break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
         if(event.button.down)
         {
            win = core_button_down_event(event.button.windowID, event.button.x, event.button.y,
                                         event.button.button, event.button.clicks);
         }
         break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
         if(!event.button.down)
         {
            win = core_button_up_event(event.button.windowID, event.button.button);
         }
         break;
      case SDL_EVENT_TEXT_INPUT:
         win = core_textinput_event(event.text.windowID, event.text.text);
         break;
      }

      if(event.type == ctx.open_file_event)
      {
         win = core_find_window(SDL_GetWindowFromID(event.user.windowID));
         if(win != NULL)
         {
            HLH_gui_element_msg(&win->e, HLH_GUI_MSG_OPENFILE, 0, event.user.data1);
         }

         // delete msg context
         HLH_gui_open_file_msg *msg = event.user.data1;
         for(size_t i = 0; i < msg->file_list_size; i += 1)
         {
            free((void *)msg->file_list[i]);
         }
         free(msg->file_list);
         free(msg);

         win = core_find_window(SDL_GetWindowFromID(event.user.windowID));
      }
      else if(event.type == ctx.save_file_event)
      {
         win = core_find_window(SDL_GetWindowFromID(event.user.windowID));
         if(win != NULL)
         {
            HLH_gui_element_msg(&win->e, HLH_GUI_MSG_SAVEFILE, 0, event.user.data1);
         }

         // delete msg context
         HLH_gui_save_file_msg *msg = event.user.data1;
         for(size_t i = 0; i < msg->file_list_size; i += 1)
         {
            free((void *)msg->file_list[i]);
         }
         free(msg->file_list);
         free(msg);

         win = core_find_window(SDL_GetWindowFromID(event.user.windowID));
      }
      else if(event.type == ctx.open_folder_event)
      {
         win = core_find_window(SDL_GetWindowFromID(event.user.windowID));
         if(win != NULL)
         {
            HLH_gui_element_msg(&win->e, HLH_GUI_MSG_OPENFOLDER, 0, event.user.data1);
         }

         // delete msg context
         HLH_gui_open_folder_msg *msg = event.user.data1;
         for(size_t i = 0; i < msg->folder_list_size; i += 1)
         {
            free((void *)msg->folder_list[i]);
         }
         free(msg->folder_list);
         free(msg);

         win = core_find_window(SDL_GetWindowFromID(event.user.windowID));
      }

      if(win != NULL && win->redraw)
      {
         if(!SDL_SetRenderTarget(win->sdl_renderer, win->sdl_target))
         {
            fprintf(stderr, "SDL_SetRenderTarget() failed: %s\n", SDL_GetError());
         }

         HLH_gui_element_redraw_msg(&win->e);
         win->redraw = false;

         if(!SDL_SetRenderTarget(win->sdl_renderer, NULL))
         {
            fprintf(stderr, "SDL_SetRenderTarget() failed: %s\n", SDL_GetError());
         }

         if(!SDL_RenderClear(win->sdl_renderer))
         {
            fprintf(stderr, "SDL_RenderClear() failed: %s\n", SDL_GetError());
         }

         if(!SDL_RenderTexture(win->sdl_renderer, win->sdl_target, NULL, NULL))
         {
            fprintf(stderr, "SDL_RenderTexture() failed: %s\n", SDL_GetError());
         }

         if(!SDL_RenderTexture(win->sdl_renderer, win->sdl_overlay, NULL, NULL))
         {
            fprintf(stderr, "SDL_RenderTexture() failed: %s\n", SDL_GetError());
         }

         if(!SDL_RenderPresent(win->sdl_renderer))
         {
            fprintf(stderr, "SDL_RenderPresent() failed: %s\n", SDL_GetError());
         }
      }
   }
}

void HLH_gui_set_scale(int scale)
{
   ctx.scale = scale;
}

int HLH_gui_get_scale(void)
{
   return ctx.scale;
}


static void core_handle_mouse(HLH_gui_element *root, HLH_gui_element *e, HLH_gui_mouse *mouse)
{
   int32_t old_trans[2];
   old_trans[0] = e->window->translation[0];
   old_trans[1] = e->window->translation[1];
   e->window->translation[0] +=e->translate[0];
   e->window->translation[1] +=e->translate[1];
   float pt[2];
   pt[0] = mouse->pos[0];
   pt[1] = mouse->pos[1];

   for(int i = 0; i < e->child_count; i += 1)
   {
      HLH_gui_element *child = e->children[i];
      if(child->flags.ignore)
      {
         continue;
      }

      HLH_gui_rect b = child->bounds;
      b.min[0] += e->window->translation[0];
      b.min[1] += e->window->translation[1];
      b.max[0] += e->window->translation[0];
      b.max[1] += e->window->translation[1];
      if(pt[0] >= b.min[0] && pt[1] >= b.min[1] &&
         pt[0] <= b.max[0] && pt[1] <= b.max[1])
      {
         core_handle_mouse(root, child, mouse);
      }
   }

   e->window->translation[0] = old_trans[0];
   e->window->translation[1] = old_trans[1];

   if(!mouse->handled)
   {
      int64_t capture = HLH_gui_element_msg(e, HLH_GUI_MSG_MOUSE, 0, mouse);
      if(mouse->handled)
      {
         if(root->last_mouse != NULL && root->last_mouse != e)
         {
            HLH_gui_element_msg(root->last_mouse, HLH_GUI_MSG_MOUSE_LEAVE, 0, NULL);
         }

         root->flags.capture_mouse = !!capture;
         root->last_mouse = e;
      }
   }
}

void HLH_gui_handle_mouse(HLH_gui_element *e, HLH_gui_mouse m)
{
   if(e->flags.capture_mouse)
   {
      HLH_gui_element *click = e->last_mouse;
      if(click != NULL)
      {
         int64_t capture = HLH_gui_element_msg(click, HLH_GUI_MSG_MOUSE, 0, &m);
         e->flags.capture_mouse = !!capture;
         e->last_mouse = click;
      }
   }
   else
   {
      m.handled = false;
      if(!HLH_gui_element_ignored(e))
      {
         core_handle_mouse(e, e, &m);
      }
   }
}

void HLH_gui_window_close(HLH_gui_window *win)
{
   SDL_Event event = {0};
   event.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
   event.window.windowID = SDL_GetWindowID(win->sdl_window);

   SDL_PushEvent(&event);
}

void HLH_gui_overlay_clear(HLH_gui_element *e)
{
   SDL_Texture *target = SDL_GetRenderTarget(e->window->sdl_renderer);

   if(!SDL_SetRenderTarget(e->window->sdl_renderer, e->window->sdl_overlay))
   {
      fprintf(stderr, "SDL_SetRenderTarget() failed: %s\n", SDL_GetError());
   }

   if(!SDL_SetRenderDrawColor(e->window->sdl_renderer, 0, 0, 0, 0))
   {
      fprintf(stderr, "SDL_SetRenderDrawColor() failed: %s\n", SDL_GetError());
   }

   if(!SDL_RenderClear(e->window->sdl_renderer))
   {
      fprintf(stderr, "SDL_RenderClear() failed: %s\n", SDL_GetError());
   }

   if(!SDL_SetRenderDrawColor(e->window->sdl_renderer, 0, 0, 0, 255))
   {
      fprintf(stderr, "SDL_SetRenderDrawColor() failed: %s\n", SDL_GetError());
   }

   if(!SDL_SetRenderTarget(e->window->sdl_renderer, target))
   {
      fprintf(stderr, "SDL_SetRenderTarget() failed: %s\n", SDL_GetError());
   }
}

void HLH_gui_window_block(HLH_gui_window *root, HLH_gui_window *blocking)
{
   root->blocking = blocking;
}

void HLH_gui_textinput_start(HLH_gui_element *e)
{
   if(e==NULL)
      return;

   HLH_gui_textinput_stop(e->window);

   e->window->keyboard = e;
   if(!SDL_StartTextInput(e->window->sdl_window))
   {
      fprintf(stderr, "SDL_StartTextInput() failed: %s\n", SDL_GetError());
   }
}

void HLH_gui_textinput_stop(HLH_gui_window *w)
{
   if(w==NULL)
      return;

   if(w->keyboard!=NULL)
   {
      HLH_gui_element_msg(w->keyboard,HLH_GUI_MSG_TEXTINPUT_END,0,NULL);
      w->keyboard = NULL;
      if(!SDL_StopTextInput(w->sdl_window))
      {
         fprintf(stderr, "SDL_StopTextInput() failed: %s\n", SDL_GetError());
      }
   }
}

uint32_t *HLH_gui_image_load(FILE *fp, int *width, int *height)
{
   if(fp==NULL)
      return NULL;

   int n;
   return (uint32_t *)stbi_load_from_file(fp, width, height, &n, 4);
}

void HLH_gui_image_free(uint32_t *pix)
{
   if(pix==NULL)
      return;

   stbi_image_free(pix);
}

void HLH_gui_image_save(FILE *fp, uint32_t *data, int width, int height, const char *ext)
{
   if(fp==NULL||data==NULL||width<=0||height<=0)
      return;
   if(strcmp(ext,"bmp")==0)
      stbi_write_bmp_to_func(image_write_func,fp,width,height,4,data);
   else if(strcmp(ext,"tga")==0)
      stbi_write_tga_to_func(image_write_func,fp,width,height,4,data);
   else if(strcmp(ext,"jpg")==0||strcmp(ext,"JPG")==0)
      stbi_write_jpg_to_func(image_write_func,fp,width,height,4,data,96);
   //Save as png if unknown
   //Could error out here, but better to save something
   else
      stbi_write_png_to_func(image_write_func,fp,width,height,4,data,width*4);
}

SDL_Texture *HLH_gui_texture_load(HLH_gui_window *win, const char *path, int *width, int *height)
{
   if(path==NULL||width==NULL||height==NULL)
      return NULL;

   int n;
   unsigned char *data = stbi_load(path, width, height, &n, 4);
   if(data!=NULL)
   {
      SDL_Surface *surface = SDL_CreateSurfaceFrom(*width, *height, SDL_PIXELFORMAT_RGBA8888, data, (*width) * 4);
      SDL_Texture *tex = SDL_CreateTextureFromSurface(win->sdl_renderer, surface);
      SDL_DestroySurface(surface);
      stbi_image_free(data);

      return tex;
   }

   return NULL;
}

SDL_Texture *HLH_gui_texture_from_data(HLH_gui_window *win, uint32_t *pix, int width, int height)
{
   if(pix==NULL)
      return NULL;

   SDL_Surface *surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, pix, width * 4);
   SDL_Texture *tex = SDL_CreateTextureFromSurface(win->sdl_renderer, surface);
   SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
   SDL_DestroySurface(surface);

   return tex;
}

static int64_t core_window_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   HLH_gui_window *win = (HLH_gui_window *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return win->width;
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return win->height;
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      SDL_DestroyTexture(win->sdl_target);
      SDL_DestroyTexture(win->sdl_overlay);
      SDL_DestroyTexture(win->sdl_font);
      if(win->sdl_icons!=NULL)
         SDL_DestroyTexture(win->sdl_icons);
      SDL_DestroyRenderer(win->sdl_renderer);
      SDL_DestroyWindow(win->sdl_window);
   }

   return 0;
}

HLH_gui_window *core_find_window(SDL_Window *win)
{
   if(win==NULL)
   {
      return NULL;
   }

   for(uintptr_t i = 0; i<ctx.window_count; i++)
      if(ctx.windows[i]->sdl_window==win)
         return ctx.windows[i];

   return NULL;
}

static void image_write_func(void *context, void *data, int size)
{
   fwrite(data,size,1,(FILE *)context);
}

uint32_t core_timer_callback(void *userdata, SDL_TimerID timer_id, uint32_t interval)
{
   HLH_gui_element *e = userdata;
   SDL_Event event;
   event.type = ctx.timer_event;
   event.user.windowID = SDL_GetWindowID(e->window->sdl_window);
   event.user.data1 = e;
   SDL_PushEvent(&event);

   return interval;
}

char *HLH_gui_strdup(const char *str)
{
   size_t len = strlen(str);
   char *str_new = calloc(len + 1, 1);
   memcpy(str_new, str, len);
   return str_new;
}

void core_open_file_callback(void *userdata, const char * const *filelist, int filter)
{
   HLH_gui_dialog_internal_ctx *dialog_ctx = userdata;

   int32_t filelist_len = 0;
   while(filelist != NULL)
   {
      if(filelist[filelist_len] == NULL)
      {
         break;
      }
      filelist_len += 1;
   }

   HLH_gui_open_file_msg *msg = calloc(1, sizeof(*msg));
   msg->ident = dialog_ctx->ident;
   msg->filter = filter;
   msg->file_list = calloc(filelist_len, sizeof(*msg->file_list));
   msg->file_list_size = filelist_len;
   for(int i = 0; i < filelist_len; i += 1)
   {
      msg->file_list[i] = HLH_gui_strdup(filelist[i]);
   }

   SDL_Event event;
   event.type = ctx.open_file_event;
   event.user.windowID = SDL_GetWindowID(dialog_ctx->window->sdl_window);
   event.user.data1 = msg;
   SDL_PushEvent(&event);

   for(size_t i = 0; i < dialog_ctx->filters_size; i += 1)
   {
      free((void *)dialog_ctx->filters[i].name);
      free((void *)dialog_ctx->filters[i].pattern);
   }
   free(dialog_ctx->filters);
   free(dialog_ctx);
}

void core_save_file_callback(void *userdata, const char * const *filelist, int filter)
{
   HLH_gui_dialog_internal_ctx *dialog_ctx = userdata;

   int32_t filelist_len = 0;
   while(filelist != NULL)
   {
      if(filelist[filelist_len] == NULL)
      {
         break;
      }
      filelist_len += 1;
   }

   HLH_gui_save_file_msg *msg = calloc(1, sizeof(*msg));
   msg->ident = dialog_ctx->ident;
   msg->filter = filter;
   msg->file_list = calloc(filelist_len, sizeof(*msg->file_list));
   msg->file_list_size = filelist_len;
   for(int i = 0; i < filelist_len; i += 1)
   {
      msg->file_list[i] = HLH_gui_strdup(filelist[i]);
   }

   SDL_Event event;
   event.type = ctx.save_file_event;
   event.user.windowID = SDL_GetWindowID(dialog_ctx->window->sdl_window);
   event.user.data1 = msg;
   SDL_PushEvent(&event);

   for(size_t i = 0; i < dialog_ctx->filters_size; i += 1)
   {
      free((void *)dialog_ctx->filters[i].name);
      free((void *)dialog_ctx->filters[i].pattern);
   }
   free(dialog_ctx->filters);
   free(dialog_ctx);
}

void core_open_folder_callback(void *userdata, const char * const *folderlist, int filter)
{
   HLH_gui_dialog_internal_ctx *dialog_ctx = userdata;

   int32_t folderlist_len = 0;
   while(folderlist != NULL)
   {
      if(folderlist[folderlist_len] == NULL)
      {
         break;
      }
      folderlist_len += 1;
   }

   HLH_gui_open_folder_msg *msg = calloc(1, sizeof(*msg));
   msg->ident = dialog_ctx->ident;
   msg->folder_list = calloc(folderlist_len, sizeof(*msg->folder_list));
   msg->folder_list_size = folderlist_len;
   for(int i = 0; i < folderlist_len; i += 1)
   {
      msg->folder_list[i] = HLH_gui_strdup(folderlist[i]);
   }

   SDL_Event event;
   event.type = ctx.open_folder_event;
   event.user.windowID = SDL_GetWindowID(dialog_ctx->window->sdl_window);
   event.user.data1 = msg;
   SDL_PushEvent(&event);

   free(dialog_ctx);
}
//-------------------------------------
