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
static int checkbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_checkbutton *HLH_gui_checkbutton_create(HLH_gui_element *parent, uint64_t flags, const char *text)
{
   HLH_gui_checkbutton *button = (HLH_gui_checkbutton *) HLH_gui_element_create(sizeof(*button),parent,flags,checkbutton_msg);
   button->e.type = "checkbutton";

   button->text_len = (int)strlen(text);
   button->text = malloc(button->text_len+1);
   strcpy(button->text,text);

   return button;
}

void HLH_gui_checkbutton_set(HLH_gui_element *e, int checked, int trigger_msg, int redraw)
{
   if(e==NULL)
      return;

   HLH_gui_checkbutton *b = (HLH_gui_checkbutton *)e;
   int previously = b->checked;
   b->checked = checked;
   if(previously!=b->checked)
   {
      if(redraw)
         HLH_gui_element_redraw(e);

      if(trigger_msg)
         HLH_gui_element_msg(e,HLH_GUI_MSG_CLICK,b->checked,NULL);
   }
}

static int checkbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_checkbutton *button = (HLH_gui_checkbutton *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return (HLH_GUI_GLYPH_HEIGHT+8)*HLH_gui_get_scale()+button->text_len*HLH_GUI_GLYPH_WIDTH*HLH_gui_get_scale()+10*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT*HLH_gui_get_scale()+8*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      int scale = HLH_gui_get_scale();

      //Infill
      HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+HLH_gui_get_scale(),e->bounds.miny+HLH_gui_get_scale(),e->bounds.maxx-HLH_gui_get_scale(),e->bounds.maxy-HLH_gui_get_scale()),0x5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(e,e->bounds,0x000000);

      //Border
      if(button->state)
      {
         HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+1*scale,e->bounds.miny+2*scale,e->bounds.minx+2*scale,e->bounds.maxy-2*scale),0x000000);
         HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+1*scale,e->bounds.maxy-2*scale,e->bounds.maxx-2*scale,e->bounds.maxy-1*scale),0x000000);

         HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.maxx-2*scale,e->bounds.miny+2*scale,e->bounds.maxx-1*scale,e->bounds.maxy-2*scale),0x323232);
         HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+2*scale,e->bounds.miny+1*scale,e->bounds.maxx-1*scale,e->bounds.miny+2*scale),0x323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+1*scale,e->bounds.miny+2*scale,e->bounds.minx+2*scale,e->bounds.maxy-1*scale),0x323232);
         HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+1*scale,e->bounds.maxy-2*scale,e->bounds.maxx-2*scale,e->bounds.maxy-1*scale),0x323232);

         HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.maxx-2*scale,e->bounds.miny+2*scale,e->bounds.maxx-1*scale,e->bounds.maxy-2*scale),0xc8c8c8);
         HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+2*scale,e->bounds.miny+1*scale,e->bounds.maxx-1*scale,e->bounds.miny+2*scale),0xc8c8c8);
      }

      //Checkbox
      int height = (e->bounds.maxy-e->bounds.miny);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      int offset = (height-dim)/2;
      HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+offset+1*scale,e->bounds.miny+offset+scale,e->bounds.minx+offset+2*scale,e->bounds.maxy-offset),0xc8c8c8);
      HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+offset+1*scale,e->bounds.maxy-offset-scale,e->bounds.minx+offset+dim,e->bounds.maxy-offset),0xc8c8c8);

      HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+offset+2*scale,e->bounds.miny+offset,e->bounds.minx+offset+dim+1*scale,e->bounds.miny+offset+scale),0x323232);
      HLH_gui_draw_rectangle_fill(e,HLH_gui_rect_make(e->bounds.minx+dim+offset+0*scale,e->bounds.miny+offset+scale,e->bounds.minx+dim+offset+1*scale,e->bounds.maxy-offset-scale),0x323232);

      HLH_gui_draw_string(e,HLH_gui_rect_make(e->bounds.minx+dim+2*scale,e->bounds.miny,e->bounds.maxx,e->bounds.maxy),button->text,button->text_len,0x000000,1);

      if(button->checked)
         HLH_gui_draw_string(e,HLH_gui_rect_make(e->bounds.minx+offset+3*scale,e->bounds.miny+offset+scale,e->bounds.minx+offset+1*scale+dim,e->bounds.miny+offset+scale+dim),"X",1,0x000000,1);
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
   }
   else if(msg==HLH_GUI_MSG_HIT)
   {
      HLH_gui_mouse *m = dp;

      int click = 0;
      int state_old = button->state;
      if(m->button&HLH_GUI_MOUSE_OUT)
      {
         button->state = 0;
      }
      else if(m->button&(HLH_GUI_MOUSE_LEFT|HLH_GUI_MOUSE_RIGHT|HLH_GUI_MOUSE_MIDDLE))
      {
         button->state = 1;
      }
      else
      {
         click = button->state==1;
         button->state = 0;
      }

      if(click)
      {
         button->checked = !button->checked;
         HLH_gui_element_msg(e,HLH_GUI_MSG_CLICK,button->checked,NULL);
         button->state = 0;
      }

      if(click||state_old!=button->state)
         HLH_gui_element_redraw(e);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(button->text);
   }

   return 0;
}
//-------------------------------------
