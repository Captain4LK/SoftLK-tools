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
static int frame_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_frame *HLH_gui_frame_create(HLH_gui_element *parent, uint64_t flags)
{
   HLH_gui_frame *frame = (HLH_gui_frame *) HLH_gui_element_create(sizeof(*frame),parent,flags,frame_msg);
   frame->e.type = "frame";

   return frame;
}

static int frame_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      HLH_gui_point *in = dp;
      return in->x+6;
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      HLH_gui_point *in = dp;
      return in->y+6;
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
      HLH_gui_rect *space = dp;
      space->minx+=3;
      space->miny+=3;
      space->maxx-=6;
      space->maxy-=6;
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      HLH_gui_draw_rectangle_fill(e,e->bounds,0x5a5a5a);
      HLH_gui_draw_rectangle(e,e->bounds,0x000000);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
   }

   return 0;
}
//-------------------------------------
