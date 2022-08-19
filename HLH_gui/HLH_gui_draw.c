/*
HLH_gui - gui framework

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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

void HLH_gui_draw_block(HLH_gui_painter *p, HLH_gui_rect rect, uint32_t color)
{
   rect = HLH_gui_rect_intersect(p->clip,rect);
   if(!HLH_gui_rect_valid(rect))
      return;

   SDL_Rect r;
   r.x = rect.l;
   r.y = rect.t;
   r.w = rect.r-rect.l;
   r.h = rect.b-rect.t;
   SDL_SetRenderDrawColor(p->win->renderer,color&255,(color>>8)&255,(color>>16)&255,(color>>24)&255);
   SDL_RenderFillRect(p->win->renderer,&r);
}

void HLH_gui_draw_string(HLH_gui_painter *p, HLH_gui_rect bounds, const char *str, size_t bytes, uint32_t color, int align_center)
{
   int scale = HLH_gui_get_scale();
   HLH_gui_rect old_clip = p->clip;
   p->clip = HLH_gui_rect_intersect(bounds,old_clip);
   int x = bounds.l;
   int y = (bounds.t+bounds.b-HLH_GUI_GLYPH_HEIGHT*scale)/2;

   if(align_center)
      x+=(bounds.r-bounds.l-bytes*HLH_GUI_GLYPH_WIDTH*scale)/2;

   SDL_Rect clip;
   clip.x = p->clip.l;
   clip.y = p->clip.t;
   clip.w = p->clip.r-p->clip.l;
   clip.h = p->clip.b-p->clip.t;
   SDL_RenderSetClipRect(p->win->renderer,&clip);
   SDL_SetTextureColorMod(p->win->font,color&255,(color>>8)&255,(color>>16)&255);

   for(int i = 0;i<bytes;i++)
   {
      uint8_t c = str[i];
      if(c>127)
         c = '?';

      SDL_Rect dst;
      dst.x = x;
      dst.y = y;
      dst.w = 8*scale;
      dst.h = 16*scale;
      SDL_Rect src;
      src.x = c*8;
      src.y = 0;
      src.w = 8;
      src.h = 16;
      SDL_RenderCopy(p->win->renderer,p->win->font,&src,&dst);

      x+=HLH_GUI_GLYPH_WIDTH*scale;
   }

   SDL_RenderSetClipRect(p->win->renderer,NULL);

   p->clip = old_clip;
}
//-------------------------------------
