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
static int group_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_group *HLH_gui_group_create(HLH_gui_element *parent, uint64_t flags)
{
   HLH_gui_group *group = (HLH_gui_group *) HLH_gui_element_create(sizeof(*group),parent,flags,group_msg);
   group->e.type = "group";

   return group;
}

static int group_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      HLH_gui_point *in = dp;
      return in->x+0;
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      HLH_gui_point *in = dp;
      return in->y+0;
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
      HLH_gui_rect *space = dp;
      space->minx+=0;
      space->miny+=0;
      space->maxx-=0;
      space->maxy-=0;
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      HLH_gui_draw_rectangle_fill(e,e->bounds,0x5a5a5a);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
   }

   return 0;
}
//-------------------------------------
