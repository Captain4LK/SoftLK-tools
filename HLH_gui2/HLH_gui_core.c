/*
HLH_gui - gui framework

Written in 2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
#include "HLH_gui_internal.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static int core_window_count;
static HLH_gui_window **core_windows;
static int core_scale = 1;
static SDL_Surface *core_font_surface;

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
static int core_window_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

void HLH_gui_init(void)
{
   if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)<0)
      fprintf(stderr,"SDL_Init(): %s\n",SDL_GetError());

   //Create font from data in core_font
   core_font_surface = SDL_CreateRGBSurface(0,1024,16,32,0xff0000,0x00ff00,0x0000ff,0xff000000);
   if(core_font_surface==NULL)
   {
      fprintf(stderr,"SDL_CreateRGBSurface(): %s\n",SDL_GetError());
      exit(EXIT_FAILURE);
   }

   if(SDL_LockSurface(core_font_surface)<0)
      fprintf(stderr,"SDL_LockSurface(): %s\n",SDL_GetError());
   for(int c = 0;c<128;c++)
   {
      int index = c*2;
      for(int i = 0;i<128;i++)
      {
         int x = i&7;
         int y = i/8;
         int val = i/64;
         int bit = i&63;
         ((uint32_t *)core_font_surface->pixels)[y*(core_font_surface->pitch/4)+x+c*8] = (core_font[index+val]&((uint64_t)1<<bit))?0xffffffff:0x0;
      }
   }
   SDL_UnlockSurface(core_font_surface);
}

HLH_gui_window *HLH_gui_window_create(const char *title, int width, int height, const char *path_icon)
{
   HLH_gui_window *window = (HLH_gui_window *)HLH_gui_element_create(sizeof(*window),NULL,0,core_window_msg);
   window->e.window = window;
   //window->hover = &window->e;
   window->width = width;
   window->height = height;
   core_window_count++;
   core_windows = realloc(core_windows,sizeof(*core_windows)*core_window_count);
   core_windows[core_window_count-1] = window;

   if(SDL_CreateWindowAndRenderer(width,height,SDL_WINDOW_RESIZABLE,&window->window,&window->renderer)<0)
      fprintf(stderr,"SDL_CreateWindowAndRenderer(): %s\n",SDL_GetError());
   SDL_SetWindowTitle(window->window,title);

   window->target = SDL_CreateTexture(window->renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,window->width,window->height);
   if(window->target==NULL)
      fprintf(stderr,"SDL_CreateTexture(): %s\n",SDL_GetError());

   window->font = SDL_CreateTextureFromSurface(window->renderer,core_font_surface);
   if(window->font==NULL)
      fprintf(stderr,"SDL_CreateTextureFromSurface(): %s\n",SDL_GetError());

   if(path_icon!=NULL)
   {
      int w, h, n;
      unsigned char *data = stbi_load(path_icon,&w,&h,&n,4);
      if(data!=NULL)
      {
         SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data,w,h,32,w*4,0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
         window->icons = SDL_CreateTextureFromSurface(window->renderer,surface);
         SDL_FreeSurface(surface);
         stbi_image_free(data);
      }
   }

   return window;
}

int HLH_gui_message_loop(void)
{
   //Send fake resize event
   for(int i = 0;i<core_window_count;i++)
   {
      HLH_gui_window *win = core_windows[i];
      SDL_Event e;
      e.type = SDL_WINDOWEVENT;

      e.window.windowID = SDL_GetWindowID(win->window);
      if(e.window.windowID==0)
         fprintf(stderr,"SDL_GetWindowID(): %s\n",SDL_GetError());

      e.window.data1 = win->width;
      e.window.data2 = win->height;
      e.window.event = SDL_WINDOWEVENT_SIZE_CHANGED;
      win->width = -1;
      win->height = -1;

      if(SDL_PushEvent(&e)<0)
         fprintf(stderr,"SDL_PushEvent(): %s\n",SDL_GetError());
   }

   HLH_gui_mouse mouse = {0};

   for(;;)
   {
      SDL_Event event;
      if(!SDL_WaitEvent(&event))
         fprintf(stderr,"SDL_WaitEvent(): %s\n",SDL_GetError());

      HLH_gui_window *win = NULL;

      if(SDL_QuitRequested())
         return 0;

      switch(event.type)
      {
      case SDL_QUIT:
         for(int i = 0;i<core_window_count;i++)
            HLH_gui_element_destroy(&core_windows[i]->e);
         return 0;
      case SDL_DROPFILE:
         win = core_find_window(SDL_GetWindowFromID(event.drop.windowID));
         if(win==NULL)
            continue;

         //HLH_gui_element_msg(&win->e,HLH_GUI_MSG_FILE_DROP,0,event.drop.file);
         SDL_free(event.drop.file);
         break;
      case SDL_WINDOWEVENT:
         win = core_find_window(SDL_GetWindowFromID(event.window.windowID));
         if(win==NULL)
            continue;

         switch(event.window.event)
         {
         case SDL_WINDOWEVENT_EXPOSED:
            if(SDL_SetRenderTarget(win->renderer,NULL)<0)
               fprintf(stderr,"SDL_SetRenderTarget(): %s\n",SDL_GetError());
            if(SDL_RenderClear(win->renderer)<0)
               fprintf(stderr,"SDL_RenderClear(): %s\n",SDL_GetError());
            if(SDL_RenderCopy(win->renderer,win->target,NULL,NULL)<0)
               fprintf(stderr,"SDL_RenderCopy(): %s\n",SDL_GetError());
            SDL_RenderPresent(win->renderer);
            break;
         case SDL_WINDOWEVENT_SIZE_CHANGED:
            {
               int width = event.window.data1;
               int height = event.window.data2;
               if(win->width!=width||win->height!=height)
               {
                  win->width = width;
                  win->height = height;

                  SDL_DestroyTexture(win->target);
                  win->target = SDL_CreateTexture(win->renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,win->width,win->height);
                  if(win->target==NULL)
                     fprintf(stderr,"SDL_CreateTexture(): %s\n",SDL_GetError());
                  if(SDL_SetRenderTarget(win->renderer,win->target)<0)
                     fprintf(stderr,"SDL_SetRenderTarget(): %s\n",SDL_GetError());

                  win->e.bounds = HLH_gui_rect_make(0,0,win->width,win->height);

                  HLH_gui_element_pack(&win->e,win->e.bounds);
                  HLH_gui_element_redraw(&win->e);
               }
            }
            break;
         case SDL_WINDOWEVENT_LEAVE:

            mouse.pos.x = -1;
            mouse.pos.y = -1;
            HLH_gui_handle_mouse(&win->e,mouse);
            break;
         case SDL_WINDOWEVENT_CLOSE:
            //Close all if window 0, otherwise close current one
            if(win==core_windows[0])
            {
               for(int i = 0;i<core_window_count;i++)
                  HLH_gui_element_destroy(&core_windows[i]->e);

               return 0;
            }
            
            for(int i = 0;i<core_window_count;i++)
            {
               if(core_windows[i]==win)
               {
                  HLH_gui_element_destroy(&win->e);
                  core_windows[i] = core_windows[core_window_count-1];
                  core_window_count--;
                  core_windows = realloc(core_windows,sizeof(*core_windows)*core_window_count);
                  break;
               }
            }

            break;
         }
         break;
      case SDL_MOUSEMOTION:
         win = core_find_window(SDL_GetWindowFromID(event.window.windowID));
         if(win==NULL)
            continue;

         //Hack to prevent flooding the event queue
         SDL_GetMouseState(&win->mouse_x,&win->mouse_y);
         SDL_FlushEvent(SDL_MOUSEMOTION);

         mouse.pos.x = event.motion.x;
         mouse.pos.y = event.motion.y;
         HLH_gui_handle_mouse(&win->e,mouse);

         break;
      case SDL_MOUSEBUTTONDOWN:
         win = core_find_window(SDL_GetWindowFromID(event.window.windowID));
         if(win==NULL)
            continue;

         mouse.pos.x = event.motion.x;
         mouse.pos.y = event.motion.y;
         switch(event.button.button)
         {
         case SDL_BUTTON_LEFT: mouse.button|=HLH_GUI_MOUSE_LEFT; break;
         case SDL_BUTTON_RIGHT: mouse.button|=HLH_GUI_MOUSE_RIGHT; break;
         case SDL_BUTTON_MIDDLE: mouse.button|=HLH_GUI_MOUSE_MIDDLE; break;
         }
         HLH_gui_handle_mouse(&win->e,mouse);

         break;
      case SDL_MOUSEBUTTONUP:
         win = core_find_window(SDL_GetWindowFromID(event.window.windowID));
         if(win==NULL)
            continue;

         mouse.pos.x = event.motion.x;
         mouse.pos.y = event.motion.y;
         switch(event.button.button)
         {
         case SDL_BUTTON_LEFT: mouse.button&=~HLH_GUI_MOUSE_LEFT; break;
         case SDL_BUTTON_RIGHT: mouse.button&=~HLH_GUI_MOUSE_RIGHT; break;
         case SDL_BUTTON_MIDDLE: mouse.button&=~HLH_GUI_MOUSE_MIDDLE; break;
         }
         HLH_gui_handle_mouse(&win->e,mouse);

         break;
      }
   }
}

void HLH_gui_set_scale(int scale)
{
   core_scale = scale;
}

int HLH_gui_get_scale(void)
{
   return core_scale;
}

void HLH_gui_handle_mouse(HLH_gui_element *e, HLH_gui_mouse m)
{
   HLH_gui_element *click = NULL;

   if(e->flags&HLH_GUI_REMOUSE)
   {
      click = e->last_mouse;
   }
   else
   {
      click = HLH_gui_element_by_point(e,m.pos);
      HLH_gui_element *last = e->last_mouse;

      if(last!=NULL&&last!=click)
      {
         m.button|=HLH_GUI_MOUSE_OUT;
         HLH_gui_element_msg(last,HLH_GUI_MSG_HIT,0,&m);
         m.button&=~HLH_GUI_MOUSE_OUT;
      }
   }

   if(click!=NULL)
   {
      int remouse = HLH_gui_element_msg(click,HLH_GUI_MSG_HIT,0,&m);
      if(remouse)
         e->flags|=HLH_GUI_REMOUSE;
      else
         e->flags&=~HLH_GUI_REMOUSE;
      e->last_mouse = click;
   }
}

void HLH_gui_window_close(HLH_gui_window *win)
{
   SDL_Event event = {0};
   event.type = SDL_WINDOWEVENT;
   event.window.event = SDL_WINDOWEVENT_CLOSE;
   event.window.windowID = SDL_GetWindowID(win->window);

   SDL_PushEvent(&event);
}

uint32_t *HLH_gui_image_load(const char *path, int *width, int *height)
{
   if(path==NULL)
      return NULL;

   int n;
   return (uint32_t *)stbi_load(path,width,height,&n,4);
}

void HLH_gui_image_free(uint32_t *pix)
{
   if(pix==NULL)
      return;

   stbi_image_free(pix);
}

SDL_Texture *HLH_gui_texture_load(HLH_gui_window *win, const char *path, int *width, int *height)
{
   if(path==NULL||width==NULL||height==NULL)
      return NULL;

   int n;
   unsigned char *data = stbi_load(path,width,height,&n,4);
   if(data!=NULL)
   {
      SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data,*width,*height,32,(*width)*4,0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
      SDL_Texture *tex = SDL_CreateTextureFromSurface(win->renderer,surface);
      SDL_FreeSurface(surface);
      stbi_image_free(data);

      return tex;
   }

   return NULL;
}

SDL_Texture *HLH_gui_texture_from_data(HLH_gui_window *win, uint32_t *pix, int width, int height)
{
   if(pix==NULL)
      return NULL;

   SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(pix,width,height,32,width*4,0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
   SDL_Texture *tex = SDL_CreateTextureFromSurface(win->renderer,surface);
   SDL_FreeSurface(surface);

   return tex;
}

static int core_window_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
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
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
      HLH_gui_rect *space = dp;
      space->minx = 0;
      space->miny = 0;
      space->maxx = win->width;
      space->maxy = win->height;
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      SDL_DestroyTexture(win->target);
      SDL_DestroyTexture(win->font);
      if(win->icons!=NULL)
         SDL_DestroyTexture(win->icons);
      SDL_DestroyRenderer(win->renderer);
      SDL_DestroyWindow(win->window);
   }

   return 0;
}

HLH_gui_window *core_find_window(SDL_Window *win)
{
   if(win==NULL)
      fprintf(stderr,"SDL_GetWindowFromID: %s\n",SDL_GetError());

   for(uintptr_t i = 0;i<core_window_count;i++)
      if(core_windows[i]->window==win)
         return core_windows[i];

   return NULL;
}
//-------------------------------------
