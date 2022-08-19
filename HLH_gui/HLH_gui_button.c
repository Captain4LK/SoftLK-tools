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
static int button_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_button *HLH_gui_button_create(HLH_gui_element *parent, uint32_t flags, const char *text, ptrdiff_t text_len)
{
   HLH_gui_button *button = (HLH_gui_button *) HLH_gui_element_create(sizeof(*button),parent,flags,button_msg);
   HLH_gui_string_copy(&button->text,&button->text_len,text,text_len);

   return button;
}

static int button_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;

   if(msg==HLH_GUI_MSG_PAINT)
   {
      HLH_gui_painter *painter = dp;

      int pressed = e->window->pressed==e&&e->window->hover==e;
      int scale = HLH_gui_get_scale();

      //Draw
      //Outline
      HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l,e->bounds.r,e->bounds.t,e->bounds.t+2*scale),0x000000);
      HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l,e->bounds.l+2*scale,e->bounds.t+2*scale,e->bounds.b-2*scale),0x000000);
      HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.r-2*scale,e->bounds.r,e->bounds.t+2*scale,e->bounds.b-2*scale),0x000000);
      HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l,e->bounds.r,e->bounds.b-2*scale,e->bounds.b),0x000000);

      //Infill
      HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l+2*scale,e->bounds.r-2*scale,e->bounds.t+2*scale,e->bounds.b-2*scale),0x5a5a5a);

      //Border
      if(pressed)
      {
         HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l+2*scale,e->bounds.l+4*scale,e->bounds.t+4*scale,e->bounds.b-4*scale),0x000000);
         HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l+2*scale,e->bounds.r-4*scale,e->bounds.b-4*scale,e->bounds.b-2*scale),0x000000);

         HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.r-4*scale,e->bounds.r-2*scale,e->bounds.t+4*scale,e->bounds.b-4*scale),0x323232);
         HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l+4*scale,e->bounds.r-2*scale,e->bounds.t+2*scale,e->bounds.t+4*scale),0x323232);
      }
      else
      {
         HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l+2*scale,e->bounds.l+4*scale,e->bounds.t+4*scale,e->bounds.b-4*scale),0x323232);
         HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l+2*scale,e->bounds.r-4*scale,e->bounds.b-4*scale,e->bounds.b-2*scale),0x323232);

         HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.r-4*scale,e->bounds.r-2*scale,e->bounds.t+4*scale,e->bounds.b-4*scale),0xc8c8c8);
         HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l+4*scale,e->bounds.r-2*scale,e->bounds.t+2*scale,e->bounds.t+4*scale),0xc8c8c8);
      }

      //Text
      HLH_gui_draw_string(painter,e->bounds,button->text,button->text_len,0x000000,1);
      //-------------------------------------
   }
   else if(msg==HLH_GUI_MSG_UPDATE)
   {
      HLH_gui_element_repaint(e,NULL);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return 18*HLH_gui_get_scale()+HLH_GUI_GLYPH_WIDTH*button->text_len*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return 25*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(button->text);
   }

   return 0;
}
//-------------------------------------
