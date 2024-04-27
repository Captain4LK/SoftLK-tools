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
static int dropdown_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void dropdown_draw(HLH_gui_dropdown *p);
//-------------------------------------

//Function implementations

HLH_gui_group *HLH_gui_menubar_create(HLH_gui_element *parent, uint64_t flags, uint64_t cflags, const char **labels, HLH_gui_element **panels, int child_count, HLH_gui_msg_handler msg_usr)
{
   HLH_gui_group *group = HLH_gui_group_create(parent, flags);

   uint64_t side = 0;
   uint64_t layout = cflags&HLH_GUI_LAYOUT;
   if(layout==HLH_GUI_LAYOUT_VERTICAL)
      side = HLH_GUI_LAYOUT_HORIZONTAL;
   else if(layout==HLH_GUI_LAYOUT_HORIZONTAL)
      side = HLH_GUI_LAYOUT_VERTICAL;

   for(int i = 0; i<child_count; i++)
   {
      HLH_gui_dropdown *drop = (HLH_gui_dropdown *) HLH_gui_element_create(sizeof(*drop), &group->e, cflags, dropdown_msg);
      drop->e.type = HLH_GUI_DROPDOWN;

      drop->text_len = (int)strlen(labels[i]);
      drop->text = malloc(drop->text_len + 1);
      drop->drop = panels[i];
      drop->drop->window = drop->e.window;
      drop->side = side;
      strcpy(drop->text, labels[i]);
   }

   return group;
}

void HLH_gui_menubar_label_set(HLH_gui_group *bar, const char *label, int which)
{
   if(bar==NULL)
      return;

   if(which<0||which>=bar->e.child_count)
      return;

   HLH_gui_dropdown *drop = (HLH_gui_dropdown *)bar->e.children[which];
   free(drop->text);
   drop->text_len = strlen(label);
   drop->text = malloc(drop->text_len+1);
   strcpy(drop->text,label);

   HLH_gui_element_layout(&drop->e.window->e, drop->e.window->e.bounds);
   HLH_gui_element_redraw(&drop->e.window->e);
}

static int dropdown_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_dropdown *drop = (HLH_gui_dropdown *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return drop->text_len * HLH_GUI_GLYPH_WIDTH * HLH_gui_get_scale() + 10 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT * HLH_gui_get_scale() + 8 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      dropdown_draw(drop);
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {}
   else if(msg==HLH_GUI_MSG_MOUSE)
   {
      HLH_gui_mouse *m = dp;
      int state_old = drop->state;
      HLH_gui_element *hit = NULL;

      if(!drop->state)
      {
         if(HLH_gui_rect_inside(drop->e.bounds, m->pos)&&m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
         {
            HLH_gui_rect bounds = drop->e.window->e.bounds;

            if(drop->side==HLH_GUI_LAYOUT_VERTICAL)
            {
               bounds.minx = drop->e.bounds.minx;
               bounds.miny = drop->e.bounds.maxy;
               HLH_gui_flag_set(drop->drop->flags,HLH_GUI_NO_CENTER_X,1);
               HLH_gui_flag_set(drop->drop->flags,HLH_GUI_NO_CENTER_Y,1);
            }
            else if(drop->side==HLH_GUI_LAYOUT_HORIZONTAL)
            {
               bounds.minx = drop->e.bounds.maxx;
               bounds.miny = drop->e.bounds.miny;
               HLH_gui_flag_set(drop->drop->flags,HLH_GUI_NO_CENTER_X,1);
               HLH_gui_flag_set(drop->drop->flags,HLH_GUI_NO_CENTER_Y,1);
            }

            HLH_gui_element_invisible(drop->drop, 0);
            HLH_gui_element_layout(drop->drop, bounds);
            HLH_gui_element_redraw(drop->drop);

            drop->state = 1;
         }
      }
      else
      {
         if(!HLH_gui_rect_inside(drop->e.bounds, m->pos))
         {
            int pass = 0;
            if(drop->side==HLH_GUI_LAYOUT_VERTICAL) pass = m->pos.y>=drop->e.bounds.maxy;
            else if(drop->side==HLH_GUI_LAYOUT_HORIZONTAL) pass = m->pos.x>=drop->e.bounds.maxx;

            if(pass)
            {
               hit = drop->drop;
               if(!(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE)))
                  drop->state = 0;
            }
            else
            {
               drop->state = 0;
            }
         }
         else if(!(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE)))
         {
            drop->state = 0;
         }
         else
         {
            hit = drop->drop;
         }

         if(!drop->state)
         {
            HLH_gui_element_invisible(drop->drop, 1);
            hit = drop->drop;

            HLH_gui_element_redraw(&drop->e.window->e);
            HLH_gui_overlay_clear(&drop->e);
         }
      }

      if(drop->state!=state_old)
         HLH_gui_element_redraw(&drop->e);
      if(hit!=NULL)
         HLH_gui_handle_mouse(hit, *m);

      return !!drop->state;
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(drop->text);

      if(drop->drop!=NULL)
         HLH_gui_element_destroy(drop->drop);
      //TODO(Captain4LK): free dropdown menus
   }

   return 0;
}

static void dropdown_draw(HLH_gui_dropdown *p)
{
   uint64_t style = p->e.flags & HLH_GUI_STYLE;
   if(style==HLH_GUI_STYLE_00)
   {
      HLH_gui_rect bounds = p->e.bounds;
      int scale = HLH_gui_get_scale();

      //Infill
      HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.minx + HLH_gui_get_scale(), bounds.miny + HLH_gui_get_scale(), bounds.maxx - HLH_gui_get_scale(), bounds.maxy - HLH_gui_get_scale()), 0xff5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(&p->e, bounds, 0xff000000);

      //Border
      if(p->state)
      {
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff000000);
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff000000);

         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xff323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff323232);

         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xffc8c8c8);
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xffc8c8c8);
      }

      HLH_gui_draw_string(&p->e, bounds, p->text, p->text_len, 0xff000000, 1);
   }
   else if(style==HLH_GUI_STYLE_01)
   {
      HLH_gui_rect bounds = p->e.bounds;

      if(p->state)
         HLH_gui_draw_rectangle_fill(&p->e, bounds, 0xff323232);
      else
         HLH_gui_draw_rectangle_fill(&p->e, bounds, 0xff5a5a5a);

      HLH_gui_draw_string(&p->e, bounds, p->text, p->text_len, 0xff000000, 1);
   }
}
//-------------------------------------
