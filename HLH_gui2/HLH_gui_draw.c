/*
HLH_gui - gui framework

Written in 2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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

void HLH_gui_draw_rectangle(HLH_gui_element *e, HLH_gui_rect rect, uint32_t color_border)
{
   HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(rect.minx,rect.miny,rect.maxx,rect.miny+HLH_gui_get_scale()*2),color_border);
   HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(rect.minx,rect.miny+HLH_gui_get_scale()*2,rect.minx+HLH_gui_get_scale()*2,rect.maxy-HLH_gui_get_scale()*2),color_border);
   HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(rect.maxx-HLH_gui_get_scale()*2,rect.miny+HLH_gui_get_scale()*2,rect.maxx,rect.maxy-HLH_gui_get_scale()*2),color_border);
   HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(rect.minx,rect.maxy-HLH_gui_get_scale()*2,rect.maxx,rect.maxy),color_border);

   //HLH_gui_draw_block(p,HLH_gui_rect_make(rect.l,rect.r,rect.t,rect.t+2*HLH_gui_get_scale()),color_border);
   //HLH_gui_draw_block(p,HLH_gui_rect_make(rect.l,rect.l+2*HLH_gui_get_scale(),rect.t+2*HLH_gui_get_scale(),rect.b-2*HLH_gui_get_scale()),color_border);
   //HLH_gui_draw_block(p,HLH_gui_rect_make(rect.r-2*HLH_gui_get_scale(),rect.r,rect.t+2*HLH_gui_get_scale(),rect.b-2*HLH_gui_get_scale()),color_border);
   //HLH_gui_draw_block(p,HLH_gui_rect_make(rect.l,rect.r,rect.b-2*HLH_gui_get_scale(),rect.b),color_border);
}

void HLH_gui_draw_rectangle_fill(HLH_gui_element *e, HLH_gui_rect rect, uint32_t color)
{
   HLH_gui_window *win = e->window;

   SDL_Rect r;
   r.x = rect.minx;
   r.y = rect.miny;
   r.w = rect.maxx-rect.minx;
   r.h = rect.maxy-rect.miny;

   uint8_t cr = color&255;
   uint8_t cg = (color>>8)&255;
   uint8_t cb = (color>>16)&255;
   uint8_t ca = (uint8_t)((color>>24)&255);
   SDL_SetRenderDrawColor(win->renderer,cr,cg,cb,ca);
   SDL_RenderFillRect(win->renderer,&r);
}
//-------------------------------------
