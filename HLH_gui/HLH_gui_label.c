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
static int label_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_label *HLH_gui_label_create(HLH_gui_element *parent, uint32_t flags,  const char *text, ptrdiff_t text_len)
{
   HLH_gui_label *label = (HLH_gui_label *) HLH_gui_element_create(sizeof(*label),parent,flags,label_msg);
   HLH_gui_string_copy(&label->text,&label->text_len,text,text_len);

   return label;
}

void HLH_gui_label_set_text(HLH_gui_label *l, const char *text, ptrdiff_t text_len)
{
   HLH_gui_string_copy(&l->text,&l->text_len,text,text_len);
   HLH_gui_element_repaint(&l->e,NULL);
}

static int label_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_label *label = (HLH_gui_label *)e;

   if(msg==HLH_GUI_MSG_PAINT)
   {
      HLH_gui_draw_string(dp,e->bounds,label->text,label->text_len,0x000000,e->flags&HLH_GUI_LABEL_CENTER);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return HLH_GUI_GLYPH_WIDTH*label->text_len*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(label->text);
   }

   return 0;
}
//-------------------------------------
