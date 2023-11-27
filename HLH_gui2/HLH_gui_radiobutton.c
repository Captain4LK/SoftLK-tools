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
static const char *radiobutton_type = "radiobutton";
//-------------------------------------

//Function prototypes
static int radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void radiobutton_draw(HLH_gui_radiobutton *r);
//-------------------------------------

//Function implementations

HLH_gui_radiobutton *HLH_gui_radiobutton_create(HLH_gui_element *parent, uint64_t flags, const char *text, HLH_gui_rect *icon_bounds)
{
   HLH_gui_radiobutton *button = (HLH_gui_radiobutton *) HLH_gui_element_create(sizeof(*button),parent,flags,radiobutton_msg);
   button->e.type = radiobutton_type;

   if(text!=NULL)
   {
      button->text_len = (int)strlen(text);
      button->text = malloc(button->text_len+1);
      strcpy(button->text,text);
   }
   else if(icon_bounds!=NULL)
   {
      button->is_icon = 1;
      button->icon_bounds = *icon_bounds;
   }

   HLH_gui_radiobutton_set(button,0,0);

   return button;
}

void HLH_gui_radiobutton_set(HLH_gui_radiobutton *r, int trigger_msg, int redraw)
{
   if(r==NULL)
      return;

   int previously = r->checked;
   if(r->e.parent!=NULL)
   {
      for(int i = 0;i<r->e.parent->child_count;i++)
      {
         HLH_gui_element *c = r->e.parent->children[i];

         //Valid pointer comparison
         if(c->type==radiobutton_type)
         {
            HLH_gui_radiobutton *b = (HLH_gui_radiobutton *)c;

            //Send message to previous button
            if(b->checked&&trigger_msg)
               HLH_gui_element_msg(&b->e,HLH_GUI_MSG_CLICK,0,NULL);
            b->checked = 0;
         }
      }
   }

   r->checked = 1;
   if(redraw)
   {
      if(r->e.parent!=NULL)
         HLH_gui_element_redraw(r->e.parent);
      else
         HLH_gui_element_redraw(&r->e);
   }

   if(trigger_msg&&!previously)
      HLH_gui_element_msg(&r->e,HLH_GUI_MSG_CLICK,1,NULL);
}

static int radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_radiobutton *button = (HLH_gui_radiobutton *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      if(button->is_icon)
         return (button->icon_bounds.maxx-button->icon_bounds.minx)+6*HLH_gui_get_scale();
      else
         return (HLH_GUI_GLYPH_HEIGHT+8)*HLH_gui_get_scale()+button->text_len*HLH_GUI_GLYPH_WIDTH*HLH_gui_get_scale()+10*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      if(button->is_icon)
         return (button->icon_bounds.maxy-button->icon_bounds.miny)+6*HLH_gui_get_scale();
      else
         return HLH_GUI_GLYPH_HEIGHT*HLH_gui_get_scale()+8*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      radiobutton_draw(button);
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
         HLH_gui_radiobutton_set(button,1,1);
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

static void radiobutton_draw(HLH_gui_radiobutton *r)
{
   uint64_t style = r->e.flags&HLH_GUI_STYLE;

   if(r->is_icon)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+scale,bounds.miny+scale,bounds.maxx-scale,bounds.maxy-scale),0x5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(&r->e,bounds,0x000000);

      //Border
      if(r->state||r->checked)
      {
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-2*scale),0x000000);
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0x000000);

         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0x323232);
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0x323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-2*scale),0x323232);
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0x323232);

         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0xc8c8c8);
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0xc8c8c8);
      }

      int width = r->icon_bounds.maxx-r->icon_bounds.minx;
      int height = r->icon_bounds.maxy-r->icon_bounds.miny;
      SDL_Rect src = {.x = r->icon_bounds.minx, .y = r->icon_bounds.miny, .w = width, .h = height};
      SDL_Rect dst = {.x = bounds.minx+3*scale,.y = bounds.miny+3*scale, .w = width, .h = height};
      SDL_RenderCopy(r->e.window->renderer,r->e.window->icons,&src,&dst);

      return;
   }

   if(style==HLH_GUI_STYLE_00)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+HLH_gui_get_scale(),bounds.miny+HLH_gui_get_scale(),bounds.maxx-HLH_gui_get_scale(),bounds.maxy-HLH_gui_get_scale()),0x5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(&r->e,bounds,0x000000);

      //Border
      if(r->state)
      {
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-2*scale),0x000000);
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0x000000);

         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0x323232);
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0x323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-1*scale),0x323232);
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0x323232);

         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0xc8c8c8);
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0xc8c8c8);
      }

      //Checkbox
      int height = (bounds.maxy-bounds.miny);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      int offset = (height-dim)/2;
      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+offset+1*scale,bounds.miny+offset+scale,bounds.minx+offset+2*scale,bounds.maxy-offset),0xc8c8c8);
      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+offset+1*scale,bounds.maxy-offset-scale,bounds.minx+offset+dim,bounds.maxy-offset),0xc8c8c8);

      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+offset+2*scale,bounds.miny+offset,bounds.minx+offset+dim+1*scale,bounds.miny+offset+scale),0x323232);
      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+dim+offset+0*scale,bounds.miny+offset+scale,bounds.minx+dim+offset+1*scale,bounds.maxy-offset-scale),0x323232);

      HLH_gui_draw_string(&r->e,HLH_gui_rect_make(bounds.minx+dim+2*scale,bounds.miny,bounds.maxx,bounds.maxy),r->text,r->text_len,0x000000,1);

      if(r->checked)
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+offset+4*scale,bounds.miny+offset+3*scale,bounds.minx+dim+offset-2*scale,bounds.miny+offset-3*scale+dim),0x000000);
   }
   else if(style==HLH_GUI_STYLE_01)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      if(r->state)
         HLH_gui_draw_rectangle_fill(&r->e,bounds,0x323232);
      else
         HLH_gui_draw_rectangle_fill(&r->e,bounds,0x5a5a5a);

      //Checkbox
      int height = (bounds.maxy-bounds.miny);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      int offset = (height-dim)/2;
      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+offset+1*scale,bounds.miny+offset+scale,bounds.minx+offset+2*scale,bounds.maxy-offset),0xc8c8c8);
      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+offset+1*scale,bounds.maxy-offset-scale,bounds.minx+offset+dim,bounds.maxy-offset),0xc8c8c8);

      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+offset+2*scale,bounds.miny+offset,bounds.minx+offset+dim+1*scale,bounds.miny+offset+scale),0x323232);
      HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+dim+offset+0*scale,bounds.miny+offset+scale,bounds.minx+dim+offset+1*scale,bounds.maxy-offset-scale),0x323232);

      HLH_gui_draw_string(&r->e,HLH_gui_rect_make(bounds.minx+dim+2*scale,bounds.miny,bounds.maxx,bounds.maxy),r->text,r->text_len,0x000000,1);

      if(r->checked)
         HLH_gui_draw_rectangle_fill(&r->e,HLH_gui_rect_make(bounds.minx+offset+4*scale,bounds.miny+offset+3*scale,bounds.minx+dim+offset-2*scale,bounds.miny+offset-3*scale+dim),0x000000);
   }
   else if(style==HLH_GUI_STYLE_02)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      if(r->checked)
         HLH_gui_draw_rectangle_fill(&r->e,bounds,0x323232);
      else
         HLH_gui_draw_rectangle_fill(&r->e,bounds,0x5a5a5a);

      int height = (bounds.maxy-bounds.miny);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      int offset = (height-dim)/2;
      HLH_gui_draw_string(&r->e,HLH_gui_rect_make(bounds.minx+dim+2*scale,bounds.miny,bounds.maxx,bounds.maxy),r->text,r->text_len,0x000000,1);
   }
}
//-------------------------------------
