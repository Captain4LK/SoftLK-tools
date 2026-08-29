/*
HLH_gui - gui framework

Written in 2023, 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

//External includes
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

void HLH_gui_draw_disable_clip(HLH_gui_element * e)
{
   if(!SDL_SetRenderClipRect(e->window->sdl_renderer, NULL))
   {
      fprintf(stderr, "SDL_SetRenderClipRect() failed: %s\n", SDL_GetError());
   }
}

void HLH_gui_draw_set_clip_rect(HLH_gui_element *e, HLH_gui_rect rect)
{
   SDL_Rect new_sdl;
   new_sdl.x = rect.min[0];
   new_sdl.y = rect.min[1];
   new_sdl.w = rect.max[0] - rect.min[0];
   new_sdl.h = rect.max[1] - rect.min[1];
   SDL_SetRenderClipRect(e->window->sdl_renderer, &new_sdl);
}

void HLH_gui_draw_rectangle(HLH_gui_element *e, HLH_gui_rect rect, uint32_t color_border)
{
   HLH_gui_draw_rectangle_fill(e, HLH_gui_rect_make(rect.min[0], rect.min[1], rect.max[0], rect.min[1] + HLH_gui_get_scale() * 1), color_border);
   HLH_gui_draw_rectangle_fill(e, HLH_gui_rect_make(rect.min[0], rect.min[1] + HLH_gui_get_scale() * 1, rect.min[0] + HLH_gui_get_scale() * 1, rect.max[1] - HLH_gui_get_scale() * 1), color_border);
   HLH_gui_draw_rectangle_fill(e, HLH_gui_rect_make(rect.max[0] - HLH_gui_get_scale() * 1, rect.min[1] + HLH_gui_get_scale() * 1, rect.max[0], rect.max[1] - HLH_gui_get_scale() * 1), color_border);
   HLH_gui_draw_rectangle_fill(e, HLH_gui_rect_make(rect.min[0], rect.max[1] - HLH_gui_get_scale() * 1, rect.max[0], rect.max[1]), color_border);
}

void HLH_gui_draw_rectangle_fill(HLH_gui_element *e, HLH_gui_rect rect, uint32_t color)
{
   HLH_gui_window *win = e->window;

   SDL_FRect r;
   r.x = rect.min[0];
   r.y = rect.min[1];
   r.w = rect.max[0] - rect.min[0];
   r.h = rect.max[1] - rect.min[1];

   uint8_t cr = color & 255;
   uint8_t cg = (color >> 8) & 255;
   uint8_t cb = (color >> 16) & 255;
   uint8_t ca = (uint8_t)((color >> 24) & 255);
   SDL_SetRenderDrawColor(win->sdl_renderer, cr, cg, cb, ca);
   SDL_RenderFillRect(win->sdl_renderer, &r);
}

void HLH_gui_draw_string(HLH_gui_element *e, HLH_gui_rect bounds, const char *text, int len, uint32_t color, int align_center)
{
   SDL_Rect clip = {0};
   clip.x = bounds.min[0];
   clip.y = bounds.min[1];
   clip.w = bounds.max[0]-bounds.min[0];
   clip.h = bounds.max[1]-bounds.min[1];
   SDL_SetRenderClipRect(e->window->sdl_renderer, &clip);

   int scale = HLH_gui_get_scale();
   int x = bounds.min[0];
   int y = (bounds.min[1] + bounds.max[1] - HLH_GUI_GLYPH_HEIGHT * scale) / 2;

   if(align_center)
      x += (bounds.max[0] - bounds.min[0] - len * HLH_GUI_GLYPH_WIDTH * scale) / 2;

   if(x<bounds.min[0])
      x = bounds.min[0]+((bounds.max[0]-bounds.min[0])-len*HLH_GUI_GLYPH_WIDTH*scale);

   SDL_SetTextureColorMod(e->window->sdl_font, color & 255, (color >> 8) & 255, (color >> 16) & 255);

   for(int i = 0; i<len; i++)
   {
      uint8_t c = text[i];
      if(c>127)
         c = '?';

      SDL_FRect dst;
      dst.x = x;
      dst.y = y;
      dst.w = 8 * scale;
      dst.h = 16 * scale;
      SDL_FRect src;
      src.x = c * 8;
      src.y = 0;
      src.w = 8;
      src.h = 16;
      SDL_RenderTexture(e->window->sdl_renderer, e->window->sdl_font, &src, &dst);

      x += HLH_GUI_GLYPH_WIDTH * scale;
   }

   SDL_SetRenderClipRect(e->window->sdl_renderer, NULL);
}
//-------------------------------------
