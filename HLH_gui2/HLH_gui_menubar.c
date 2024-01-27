/*
HLH_gui - gui framework

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
static int pulldown_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void pulldown_draw(HLH_gui_pulldown *p);
//-------------------------------------

//Function implementations

HLH_gui_group *HLH_gui_menubar_create(HLH_gui_element *parent, uint64_t flags, uint64_t cflags, const char **labels, HLH_gui_element **panels, int child_count, HLH_gui_msg_handler msg_usr)
{
   HLH_gui_group *group = HLH_gui_group_create(parent,flags);

   uint64_t side = 0;
   switch(cflags&HLH_GUI_PACK)
   {
   case HLH_GUI_PACK_EAST:
   case HLH_GUI_PACK_WEST:
      side = HLH_GUI_PACK_SOUTH;
      break;
   case HLH_GUI_PACK_NORTH:
   case HLH_GUI_PACK_SOUTH:
      side = HLH_GUI_PACK_EAST;
      break;
   }

   for(int i = 0;i<child_count;i++)
   {
      HLH_gui_pulldown *pull = (HLH_gui_pulldown *) HLH_gui_element_create(sizeof(*pull),&group->e,cflags,pulldown_msg);
      pull->e.type = "pulldown";

      pull->text_len = (int)strlen(labels[i]);
      pull->text = malloc(pull->text_len+1);
      pull->pull = panels[i];
      pull->pull->window = pull->e.window;
      pull->side = side;
      strcpy(pull->text,labels[i]);
   }

   return group;
}

static int pulldown_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_pulldown *pull = (HLH_gui_pulldown *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return pull->text_len*HLH_GUI_GLYPH_WIDTH*HLH_gui_get_scale()+10*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT*HLH_gui_get_scale()+8*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      pulldown_draw(pull);
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {
   }
   else if(msg==HLH_GUI_MSG_HIT)
   {
      HLH_gui_mouse *m = dp;
      int state_old = pull->state;
      HLH_gui_element *hit = NULL;

      if(!pull->state)
      {
         if(HLH_gui_rect_inside(pull->e.bounds,m->pos)&&m->button&(HLH_GUI_MOUSE_LEFT|HLH_GUI_MOUSE_RIGHT|HLH_GUI_MOUSE_MIDDLE))
         {
            HLH_gui_rect bounds = pull->e.window->e.bounds;
            
            pull->pull->flags&=~HLH_GUI_PLACE;

            if(pull->side==HLH_GUI_PACK_SOUTH)
            {
               bounds.minx = pull->e.bounds.minx;
               bounds.miny = pull->e.bounds.maxy;
               pull->pull->flags|=HLH_GUI_PLACE_NW;
            }
            else if(pull->side==HLH_GUI_PACK_EAST)
            {
               bounds.minx = pull->e.bounds.maxx;
               bounds.miny = pull->e.bounds.miny;
               pull->pull->flags|=HLH_GUI_PLACE_NW;
            }

            HLH_gui_element_invisible(pull->pull,0);
            HLH_gui_element_pack(pull->pull,bounds);
            HLH_gui_element_redraw(pull->pull);

            pull->state = 1;
         }
      }
      else
      {
         if(!HLH_gui_rect_inside(pull->e.bounds,m->pos))
         {
            int pass = 0;
            if(pull->side==HLH_GUI_PACK_SOUTH) pass = m->pos.y>=pull->e.bounds.maxy;
            else if(pull->side==HLH_GUI_PACK_EAST) pass = m->pos.x>=pull->e.bounds.maxx;

            if(pass)
            {
               hit = pull->pull;
               if(!(m->button&(HLH_GUI_MOUSE_LEFT|HLH_GUI_MOUSE_RIGHT|HLH_GUI_MOUSE_MIDDLE)))
                  pull->state = 0;
            }
            else
            {
               pull->state = 0;
            }
         }
         else if(!(m->button&(HLH_GUI_MOUSE_LEFT|HLH_GUI_MOUSE_RIGHT|HLH_GUI_MOUSE_MIDDLE)))
         {
            pull->state = 0;
         }
         else
         {
            hit = pull->pull;
         }

         if(!pull->state)
         {
            HLH_gui_element_invisible(pull->pull,1);
            hit = pull->pull;

            HLH_gui_element_redraw(&pull->e.window->e);
            HLH_gui_overlay_clear(&pull->e);
         }
      }
      
      if(pull->state!=state_old)
         HLH_gui_element_redraw(&pull->e);
      if(hit!=NULL)
         HLH_gui_handle_mouse(hit,*m);

      return !!pull->state;
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(pull->text);

      if(pull->pull!=NULL)
         HLH_gui_element_destroy(pull->pull);
      //TODO(Captain4LK): free pulldown menus
   }

   return 0;
}

static void pulldown_draw(HLH_gui_pulldown *p)
{
   uint64_t style = p->e.flags&HLH_GUI_STYLE;
   if(style==HLH_GUI_STYLE_00)
   {
      HLH_gui_rect bounds = p->e.bounds;
      int scale = HLH_gui_get_scale();

      //Infill
      HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.minx+HLH_gui_get_scale(),bounds.miny+HLH_gui_get_scale(),bounds.maxx-HLH_gui_get_scale(),bounds.maxy-HLH_gui_get_scale()),0xff5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(&p->e,bounds,0xff000000);

      //Border
      if(p->state)
      {
         HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-2*scale),0xff000000);
         HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0xff000000);

         HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0xff323232);
         HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0xff323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-2*scale),0xff323232);
         HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0xff323232);

         HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0xffc8c8c8);
         HLH_gui_draw_rectangle_fill(&p->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0xffc8c8c8);
      }

      HLH_gui_draw_string(&p->e,bounds,p->text,p->text_len,0xff000000,1);
   }
   else if(style==HLH_GUI_STYLE_01)
   {
      HLH_gui_rect bounds = p->e.bounds;

      if(p->state)
         HLH_gui_draw_rectangle_fill(&p->e,bounds,0xff323232);
      else
         HLH_gui_draw_rectangle_fill(&p->e,bounds,0xff5a5a5a);

      HLH_gui_draw_string(&p->e,bounds,p->text,p->text_len,0xff000000,1);
   }
}
//-------------------------------------
