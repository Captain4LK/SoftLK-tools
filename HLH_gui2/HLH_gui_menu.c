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
static int menubutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void menubutton_draw(HLH_gui_menubutton *b);
//-------------------------------------

//Function implementations

HLH_gui_group *HLH_gui_menu_create(HLH_gui_element *parent, uint64_t flags, uint64_t cflags, const char **labels, int label_count, HLH_gui_msg_handler msg_usr)
{
   HLH_gui_group *group = HLH_gui_group_create(parent,flags);

   for(int i = 0;i<label_count;i++)
   {
      HLH_gui_menubutton *button = (HLH_gui_menubutton *) HLH_gui_element_create(sizeof(*button),&group->e,cflags,menubutton_msg);
      button->e.type = "menubutton";

      button->index = i;
      button->text_len = (int)strlen(labels[i]);
      button->text = malloc(button->text_len+1);
      button->e.msg_usr = msg_usr;
      strcpy(button->text,labels[i]);
   }

   return group;
}

static int menubutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_menubutton *button = (HLH_gui_menubutton *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return button->text_len*HLH_GUI_GLYPH_WIDTH*HLH_gui_get_scale()+10*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT*HLH_gui_get_scale()+8*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      menubutton_draw(button);
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

      if(click||state_old!=button->state)
         HLH_gui_element_redraw(e);

      if(click)
      {
         HLH_gui_element_msg(e,HLH_GUI_MSG_CLICK_MENU,button->index,NULL);
         button->state = 0;
      }
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(button->text);
   }

   return 0;
}

static void menubutton_draw(HLH_gui_menubutton *b)
{
   uint64_t style = b->e.flags&HLH_GUI_STYLE;

   if(style==HLH_GUI_STYLE_00)
   {
      HLH_gui_rect bounds = b->e.bounds;
      int scale = HLH_gui_get_scale();

      //Infill
      HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.minx+scale,bounds.miny+scale,bounds.maxx-scale,bounds.maxy-scale),0xff5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(&b->e,bounds,0xff000000);

      //Border
      if(b->state)
      {
         HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-2*scale),0xff000000);
         HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0xff000000);

         HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0xff323232);
         HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0xff323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.miny+2*scale,bounds.minx+2*scale,bounds.maxy-2*scale),0xff323232);
         HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.minx+1*scale,bounds.maxy-2*scale,bounds.maxx-2*scale,bounds.maxy-1*scale),0xff323232);

         HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.maxx-2*scale,bounds.miny+2*scale,bounds.maxx-1*scale,bounds.maxy-2*scale),0xffc8c8c8);
         HLH_gui_draw_rectangle_fill(&b->e,HLH_gui_rect_make(bounds.minx+2*scale,bounds.miny+1*scale,bounds.maxx-1*scale,bounds.miny+2*scale),0xffc8c8c8);
      }

      HLH_gui_draw_string(&b->e,bounds,b->text,b->text_len,0xff000000,1);
   }
   else if(style==HLH_GUI_STYLE_01)
   {
      HLH_gui_rect bounds = b->e.bounds;

      if(b->state)
         HLH_gui_draw_rectangle_fill(&b->e,bounds,0xff323232);
      else
         HLH_gui_draw_rectangle_fill(&b->e,bounds,0xff5a5a5a);

      HLH_gui_draw_string(&b->e,bounds,b->text,b->text_len,0xff000000,1);
   }
}
//-------------------------------------
