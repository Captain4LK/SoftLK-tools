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
static void group_draw(HLH_gui_group *g);
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
   HLH_gui_group *group = (HLH_gui_group *)e;
   uint64_t style = group->e.flags&HLH_GUI_STYLE;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      HLH_gui_point *in = dp;

      if(style==HLH_GUI_STYLE_00)
         return in->x;
      else if(style==HLH_GUI_STYLE_01)
         return in->x+HLH_gui_get_scale()*2;
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      HLH_gui_point *in = dp;

      if(style==HLH_GUI_STYLE_00)
         return in->y+0;
      else if(style==HLH_GUI_STYLE_01)
         return in->y+HLH_gui_get_scale()*2;
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
      HLH_gui_rect *space = dp;

      if(style==HLH_GUI_STYLE_00)
      {
      }
      else if(style==HLH_GUI_STYLE_01)
      {
         space->minx+=HLH_gui_get_scale();
         space->miny+=HLH_gui_get_scale();
         space->maxx-=HLH_gui_get_scale()*2;
         space->maxy-=HLH_gui_get_scale()*2;
      }
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      group_draw(group);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
   }

   return 0;
}

static void group_draw(HLH_gui_group *g)
{
   uint64_t style = g->e.flags&HLH_GUI_STYLE;

   if(style==HLH_GUI_STYLE_00)
   {
      HLH_gui_draw_rectangle_fill(&g->e,g->e.bounds,0xff5a5a5a);
   }
   else if(style==HLH_GUI_STYLE_01)
   {
      HLH_gui_draw_rectangle_fill(&g->e,g->e.bounds,0xff5a5a5a);
      HLH_gui_draw_rectangle(&g->e,g->e.bounds,0xff000000);
   }
}
//-------------------------------------
