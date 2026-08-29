/*
HLH_gui - gui framework

Written in 2023,2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

//External includes
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
//-------------------------------------

//#defines
#define GROUP_MAX(a, b) ((a) > (b) ? (a) : (b))
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static int64_t group_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp);
static void group_draw(HLH_gui_group *g);
//-------------------------------------

//Function implementations

HLH_gui_group *HLH_gui_group_create(HLH_gui_element *parent, HLH_gui_flags flags)
{
   HLH_gui_group *group = (HLH_gui_group *) HLH_gui_element_create(sizeof(*group), parent, flags, group_msg);
   group->e.type = HLH_GUI_GROUP;

   return group;
}

static int64_t group_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   HLH_gui_group *group = (HLH_gui_group *)e;
   uint64_t style = group->e.flags.style;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      int32_t (*space)[2] = dp;

      if(style==0)
      {
         return GROUP_MAX((*space)[0], group->e.size_min[0]);
      }
      else if(style==1)
      {
         return GROUP_MAX((*space)[0], group->e.size_min[0]) + 2 * HLH_gui_get_scale();
      }
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      int32_t (*space)[2] = dp;

      /*
      if group.flags.style == 0
      {
         return i64(max(space[0], group.size_min[1]))
      }
      else if group.flags.style == 1
      {
         return i64(max(space[0], group.size_min[1])) + 2 * i64(get_scale())
      }
      */

      if(style == 0)
      {
         return GROUP_MAX((*space)[0], group->e.size_min[1]);
      }
      else if(style == 1)
      {
         return GROUP_MAX((*space)[0], group->e.size_min[1]) + 2 * HLH_gui_get_scale();
      }
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_PAD)
   {
      int32_t (*pad)[2][2] = dp;
      if(style == 1)
      {
         (*pad)[0][0] = HLH_gui_get_scale();
         (*pad)[0][1] = HLH_gui_get_scale();
         (*pad)[1][0] = HLH_gui_get_scale();
         (*pad)[1][1] = HLH_gui_get_scale();
      }
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      group_draw(group);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {}

   return 0;
}

static void group_draw(HLH_gui_group *g)
{
   uint64_t style = g->e.flags.style;

   if(style==0)
   {
      HLH_gui_draw_rectangle_fill(&g->e, g->e.bounds, HLH_gui_theme_current.bg);
   }
   else if(style==1)
   {
      HLH_gui_draw_rectangle_fill(&g->e, g->e.bounds, HLH_gui_theme_current.bg);
      HLH_gui_draw_rectangle(&g->e, g->e.bounds, HLH_gui_theme_current.border);
   }
}

#undef GROUP_MAX
//-------------------------------------
