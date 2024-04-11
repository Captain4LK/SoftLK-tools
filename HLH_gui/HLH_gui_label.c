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
static int label_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_label *HLH_gui_label_create(HLH_gui_element *parent, uint64_t flags, const char *text)
{
   HLH_gui_label *label = (HLH_gui_label *) HLH_gui_element_create(sizeof(*label), parent, flags, label_msg);
   label->e.type = HLH_GUI_LABEL;

   label->text_len = (int)strlen(text);
   label->text = malloc(label->text_len + 1);
   strcpy(label->text, text);

   return label;
}

static int label_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_label *label = (HLH_gui_label *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return label->text_len * HLH_GUI_GLYPH_WIDTH * HLH_gui_get_scale() + 2 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT * HLH_gui_get_scale() + 2 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {}
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      HLH_gui_draw_string(e, e->bounds, label->text, label->text_len, 0x1f1f1f, 1);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(label->text);
   }

   return 0;
}
//-------------------------------------
