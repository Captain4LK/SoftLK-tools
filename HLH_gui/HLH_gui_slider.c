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
#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_slider *HLH_gui_slider_create(HLH_gui_element *parent, uint32_t flags, int value, int min, int max)
{
   HLH_gui_slider *slider = (HLH_gui_slider *) HLH_gui_element_create(sizeof(*slider),parent,flags,slider_msg);
   slider->value = value;
   slider->min = min;
   slider->max = max;

   return slider;
}

void HLH_gui_slider_set_value(HLH_gui_slider *slider, int value)
{
   slider->value = value;
   HLH_gui_element_repaint(&slider->e,NULL);
}

static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_slider *s = e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      //TODO
      return 100;
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return 25*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_PAINT)
   {
      HLH_gui_draw_block(dp,e->bounds,0x0);
      float t = ((float)(s->value-s->min)/(float)(s->max-s->min));
      HLH_gui_draw_block(dp,HLH_gui_rect_make(e->bounds.l+t*(e->bounds.r-e->bounds.l-1)-HLH_gui_get_scale(),e->bounds.l+t*(e->bounds.r-e->bounds.l-1)+HLH_gui_get_scale(),e->bounds.t+2*HLH_gui_get_scale(),e->bounds.b-2*HLH_gui_get_scale()),0xc8c8c8);
   }
   else if(msg==HLH_GUI_MSG_MOUSE_DRAG||msg==HLH_GUI_MSG_LEFT_DOWN)
   {
      int width = e->bounds.r-e->bounds.l;
      if(width==0)
         return 0;
      float t = ((float)(e->window->mouse_x-e->bounds.l)/(float)width);
      s->value = s->min+t*(s->max-s->min);
      s->value = MAX(s->min,MIN(s->value,s->max));
      HLH_gui_element_repaint(e,NULL);
   }

   return 0;
}
//-------------------------------------
