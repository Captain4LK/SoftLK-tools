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
static int panel_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int panel_layout(HLH_gui_panel *p, HLH_gui_rect bounds, int measure);
static int panel_measure(HLH_gui_panel *p);
//-------------------------------------

//Function implementations

HLH_gui_panel *HLH_gui_panel_create(HLH_gui_element *parent, uint32_t flags)
{
   HLH_gui_panel *panel = (HLH_gui_panel *) HLH_gui_element_create(sizeof(*panel),parent,flags,panel_msg);

   return panel;
}

static int panel_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_panel *panel = (HLH_gui_panel *)e;
   int horizontal = panel->e.flags&HLH_GUI_PANEL_HORIZONTAL;

   if(msg==HLH_GUI_MSG_PAINT)
   {
      if(e->flags&HLH_GUI_PANEL_LIGHT)
         HLH_gui_draw_block(dp,e->bounds,0x5a5a5a);
      else if(e->flags&HLH_GUI_PANEL_DARK)
         HLH_gui_draw_block(dp,e->bounds,0x323232);
   }
   else if(msg==HLH_GUI_MSG_LAYOUT)
   {
      panel_layout(panel,e->bounds,0);
      HLH_gui_element_repaint(e,NULL);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return horizontal?panel_layout(panel,HLH_gui_rect_make(0,0,0,di),1):panel_measure(panel);
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return horizontal?panel_measure(panel):panel_layout(panel,HLH_gui_rect_make(0,di,0,0),1);
   }

   return 0;
}

static int panel_layout(HLH_gui_panel *p, HLH_gui_rect bounds, int measure)
{
   int horizontal = p->e.flags&HLH_GUI_PANEL_HORIZONTAL;
   int pos = horizontal?p->border.l*HLH_gui_get_scale():p->border.t*HLH_gui_get_scale();
   int space_h = bounds.r-bounds.l-p->border.r*HLH_gui_get_scale()-p->border.l*HLH_gui_get_scale();
   int space_v = bounds.b-bounds.t-p->border.b*HLH_gui_get_scale()-p->border.t*HLH_gui_get_scale();
   int available = horizontal?space_h:space_v;
   int fill = 0;
   int per_fill = 0;
   int count = 0;
   
   for(int i = 0;i<p->e.child_count;i++)
   {
      if(p->e.children[i]->flags&HLH_GUI_DESTROY||p->e.children[i]->flags&HLH_GUI_HIDDEN)
         continue;
      if(!measure)
         HLH_gui_element_msg(p->e.children[i],HLH_GUI_MSG_LAYOUT,0,NULL);

      count++;

      if(horizontal)
      {
         if(p->e.children[i]->flags&HLH_GUI_H_FILL)
            fill++;
         else if(available>0)
            available-=HLH_gui_element_msg(p->e.children[i],HLH_GUI_MSG_GET_WIDTH,space_v,NULL);
      }
      else
      {
         if(p->e.children[i]->flags&HLH_GUI_V_FILL)
            fill++;
         else if(available>0)
            available-=HLH_gui_element_msg(p->e.children[i],HLH_GUI_MSG_GET_HEIGHT,space_h,NULL);
      }
   }

   if(count)
      available-=(count-1)*p->gap*HLH_gui_get_scale();

   if(available>0&&fill)
      per_fill = available/fill;

   int border2 = horizontal?p->border.t*HLH_gui_get_scale():p->border.l*HLH_gui_get_scale();

   for(int i = 0;i<p->e.child_count;i++)
   {
      if(p->e.children[i]->flags&HLH_GUI_DESTROY||p->e.children[i]->flags&HLH_GUI_HIDDEN)
         continue;

      HLH_gui_element *child = p->e.children[i];

      if(horizontal)
      {
         int height = (child->flags&HLH_GUI_V_FILL)?space_v:HLH_gui_element_msg(child,HLH_GUI_MSG_GET_HEIGHT,(child->flags&HLH_GUI_H_FILL)?per_fill:0,NULL);
         int width = (child->flags&HLH_GUI_H_FILL)?per_fill:HLH_gui_element_msg(child,HLH_GUI_MSG_GET_WIDTH,height,NULL);
         HLH_gui_rect r = HLH_gui_rect_make(pos+bounds.l,pos+width+bounds.l,border2+(space_v-height)/2+bounds.t,border2+(space_v+height)/2+bounds.t);
         if(!measure)
            HLH_gui_element_move(child,r,0);
         pos+=width+p->gap*HLH_gui_get_scale();
      }
      else
      {
         int width = (child->flags&HLH_GUI_H_FILL)?space_h:HLH_gui_element_msg(child,HLH_GUI_MSG_GET_WIDTH,(child->flags&HLH_GUI_V_FILL)?per_fill:0,NULL);
         int height = (child->flags&HLH_GUI_V_FILL)?per_fill:HLH_gui_element_msg(child,HLH_GUI_MSG_GET_HEIGHT,width,NULL);
         HLH_gui_rect r = HLH_gui_rect_make(border2+(space_h-width)/2+bounds.l,border2+(space_h+width)/2+bounds.l,pos+bounds.t,pos+height+bounds.t);
         if(!measure)
            HLH_gui_element_move(child,r,0);
         pos+=height+p->gap*HLH_gui_get_scale();
      }
   }

   return pos-(count?p->gap*HLH_gui_get_scale():0)+(horizontal?p->border.r*HLH_gui_get_scale():p->border.b*HLH_gui_get_scale());
}

static int panel_measure(HLH_gui_panel *p)
{
   int horizontal = p->e.flags&HLH_GUI_PANEL_HORIZONTAL;
   int size = 0;

   for(int i = 0;i<p->e.child_count;i++)
   {
      if(p->e.children[i]->flags&HLH_GUI_DESTROY||p->e.children[i]->flags&HLH_GUI_HIDDEN)
         continue;

      int child_size = HLH_gui_element_msg(p->e.children[i],horizontal?HLH_GUI_MSG_GET_HEIGHT:HLH_GUI_MSG_GET_WIDTH,0,NULL);
      if(child_size>size)
         size = child_size;
   }

   int border = horizontal?p->border.t*HLH_gui_get_scale()+p->border.b*HLH_gui_get_scale():p->border.l*HLH_gui_get_scale()+p->border.r*HLH_gui_get_scale();
   return size+border;
}
//-------------------------------------
