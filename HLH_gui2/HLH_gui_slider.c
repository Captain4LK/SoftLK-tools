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
static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void slider_draw(HLH_gui_slider *s);
//-------------------------------------

//Function implementations

HLH_gui_slider *HLH_gui_slider_create(HLH_gui_element *parent, uint64_t flags, int direction)
{
   HLH_gui_slider *slider = (HLH_gui_slider *) HLH_gui_element_create(sizeof(*slider),parent,flags,slider_msg);
   slider->direction = direction;
   slider->value = 100;
   slider->range = 100;

   return slider;
}

void HLH_gui_slider_set(HLH_gui_slider *slider, int value, int range, int trigger_msg, int redraw)
{
   if(slider->value!=value||slider->range!=range)
   {
      slider->value = value;
      slider->range = range;

      if(trigger_msg)
         HLH_gui_element_msg(&slider->e,HLH_GUI_MSG_SLIDER_VALUE_CHANGED,0,NULL);
      if(redraw)
         HLH_gui_element_redraw(&slider->e);
   }
}

static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_slider *slider = (HLH_gui_slider *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return HLH_GUI_GLYPH_HEIGHT*HLH_gui_get_scale()+8*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT*HLH_gui_get_scale()+8*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      slider_draw(slider);
   }
   else if(msg==HLH_GUI_MSG_HIT)
   {
      HLH_gui_mouse *m = dp;
      if(m->button&(HLH_GUI_MOUSE_LEFT|HLH_GUI_MOUSE_RIGHT|HLH_GUI_MOUSE_MIDDLE))
      {
         if(slider->direction==0)
         {
            int mx = m->pos.x-slider->e.bounds.minx;
            int width = slider->e.bounds.maxx-slider->e.bounds.minx-HLH_gui_get_scale()*6;
            int value = (mx*slider->range)/width;
            if(value<0) value = 0;
            if (value>slider->range) value = slider->range;

            if(slider->value!=value)
            {
               slider->value = value;
               HLH_gui_element_msg(&slider->e,HLH_GUI_MSG_SLIDER_VALUE_CHANGED,0,NULL);
               HLH_gui_element_redraw(&slider->e);
            }
         }
         else
         {
            int my = slider->e.bounds.maxy-m->pos.y;
            int height = slider->e.bounds.maxy-slider->e.bounds.miny-HLH_gui_get_scale()*6;
            int value = (my*slider->range)/height;
            if(value<0) value = 0;
            if(value>slider->range) value = slider->range;

            if(slider->value!=value)
            {
               slider->value = value;
               HLH_gui_element_msg(&slider->e,HLH_GUI_MSG_SLIDER_VALUE_CHANGED,0,NULL);
               HLH_gui_element_redraw(&slider->e);
            }
         }

         return 1;
      }
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
   }

   return 0;
}

static void slider_draw(HLH_gui_slider *s)
{
   if(s->direction==0)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = s->e.bounds;

      HLH_gui_draw_rectangle_fill(&s->e,bounds,0xff5a5a5a);

      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-1*scale),0xff323232);
      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0xff323232);

      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0xffc8c8c8);
      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0xffc8c8c8);

      int width = (s->value*(bounds.maxx-bounds.minx-scale*6))/s->range;
      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.minx+3*scale,bounds.miny+3*scale,bounds.minx+3*scale+width,bounds.maxy-3*scale),0xff323232);
   }
   else
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = s->e.bounds;

      HLH_gui_draw_rectangle_fill(&s->e,bounds,0xff5a5a5a);

      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-1*scale),0xff323232);
      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0xff323232);

      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0xffc8c8c8);
      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0xffc8c8c8);

      int height = (s->value*(bounds.maxy-bounds.miny-scale*6))/s->range;
      HLH_gui_draw_rectangle_fill(&s->e,HLH_gui_rect_make(bounds.minx+3*scale,bounds.maxy-3*scale-height,bounds.maxx-3*scale,bounds.maxy-3*scale),0xff323232);
   }
}
//-------------------------------------
