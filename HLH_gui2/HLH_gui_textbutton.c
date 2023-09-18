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
static int textbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_textbutton *HLH_gui_textbutton_create(HLH_gui_element *parent, uint64_t flags, const char *text)
{
   HLH_gui_textbutton *button = (HLH_gui_textbutton *) HLH_gui_element_create(sizeof(*button),parent,flags,textbutton_msg);

   button->text_len = (int)strlen(text);
   button->text = malloc(button->text_len+1);
   strcpy(button->text,text);

   return button;
}

static int textbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_textbutton *button = (HLH_gui_textbutton *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return button->text_len*HLH_GUI_GLYPH_WIDTH*HLH_gui_get_scale()+6*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT*HLH_gui_get_scale()+6*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      HLH_gui_draw_rectangle_fill(e,e->bounds,0x00ff00);
      HLH_gui_draw_rectangle(e,e->bounds,0xff0000);
      HLH_gui_draw_string(e,e->bounds,button->text,button->text_len,0xffffff,1);
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
   }
   else if(msg==HLH_GUI_MSG_HIT)
   {
      HLH_gui_mouse *m = dp;

      int click = 0;

      if(m->button&HLH_GUI_BUTTON_OUT)
      {
      }
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(button->text);
   }

   return 0;
}
//-------------------------------------
