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
static int separator_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void separator_draw(HLH_gui_separator *s);
//-------------------------------------

//Function implementations

HLH_gui_separator *HLH_gui_separator_create(HLH_gui_element *parent, uint64_t flags, int direction)
{
   HLH_gui_separator *separator = (HLH_gui_separator *) HLH_gui_element_create(sizeof(*separator),parent,flags,separator_msg);
   separator->direction = direction;

   return separator;
}

static int separator_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_separator *separator = (HLH_gui_separator *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      if(separator->direction==0)
         return 0;
      return HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      if(separator->direction==1)
         return 0;
      return HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      separator_draw(separator);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
   }

   return 0;
}

static void separator_draw(HLH_gui_separator *s)
{
   HLH_gui_draw_rectangle_fill(&s->e,s->e.bounds,0x000000);
}
//-------------------------------------
