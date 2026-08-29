/*
HLH_gui - gui framework

Written in 2023,2024,2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
static int64_t dropdown_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp);
static void dropdown_draw(HLH_gui_dropdown *p);
//-------------------------------------

//Function implementations

HLH_gui_group *HLH_gui_menubar_create(HLH_gui_element *parent, HLH_gui_flags flags, HLH_gui_flags cflags,
                                      const char **labels, HLH_gui_element **panels, int child_count, HLH_gui_msg_handler msg_usr)
{
   HLH_gui_group *group = HLH_gui_group_create(parent, flags);

   bool side = flags.layout == HLH_GUI_LAYOUT_VERTICAL;
   /*
   uint64_t layout = cflags.layout;
   if(layout==HLH_GUI_LAYOUT_VERTICAL)
      side = HLH_GUI_LAYOUT_VERTICAL;
   else if(layout==HLH_GUI_LAYOUT_HORIZONTAL)
      side = HLH_GUI_LAYOUT_HORIZONTAL;
   */

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
   drop->text_len = (int)strlen(label);
   drop->text = malloc(drop->text_len+1);
   strcpy(drop->text,label);

   HLH_gui_element_layout(&drop->e.window->e, drop->e.window->e.bounds);
   HLH_gui_element_redraw(&drop->e.window->e);
}

static int64_t dropdown_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
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
   else if(msg==HLH_GUI_MSG_MOUSE)
   {
      HLH_gui_mouse *m = dp;
      int state_old = drop->state;
      HLH_gui_element *hit = NULL;

      if(drop->state == 0)
      {
         if(HLH_gui_rect_inside(drop->e.bounds, (HLH_gui_point){m->pos[0], m->pos[1]})&&
            (m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE)))
         {
            HLH_gui_rect bounds = drop->e.window->e.bounds;

            if(!drop->side)
            {
               bounds.min[0] = drop->e.bounds.min[0];
               bounds.min[1] = drop->e.bounds.max[1];
               drop->drop->flags.center_x = false;
               drop->drop->flags.center_y = false;
            }
            else
            {
               bounds.min[0] = drop->e.bounds.max[0];
               bounds.min[1] = drop->e.bounds.min[1];
               drop->drop->flags.center_x = false;
               drop->drop->flags.center_y = false;
            }

            HLH_gui_element_invisible(drop->drop, 0);
            HLH_gui_element_layout(drop->drop, bounds);
            HLH_gui_element_redraw(drop->drop);

            drop->state = 1;
         }
      }
      else if(drop->state == 1)
      {
         if(HLH_gui_rect_inside(drop->drop->bounds, (HLH_gui_point){m->pos[0], m->pos[1]}))
         {
            hit = drop->drop;
         }

         if(!(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE)))
         {
            if(HLH_gui_rect_inside(drop->drop->bounds, (HLH_gui_point){m->pos[0], m->pos[1]}))
            {
               hit = drop->drop;

               drop->state = 0;

               HLH_gui_element_invisible(drop->drop, 1);
               hit = drop->drop;

               HLH_gui_element_redraw(&drop->e.window->e);
               HLH_gui_overlay_clear(&drop->e);
            }
            else if(HLH_gui_rect_inside(drop->e.bounds, (HLH_gui_point){m->pos[0], m->pos[1]}))
            {
               drop->state = 2;
            }
            else
            {
               drop->state = 0;

               HLH_gui_element_invisible(drop->drop, 1);
               hit = drop->drop;

               HLH_gui_element_redraw(&drop->e.window->e);
               HLH_gui_overlay_clear(&drop->e);
            }
         }
      }
      else if(drop->state == 2)
      {
         if(HLH_gui_rect_inside(drop->drop->bounds, (HLH_gui_point){m->pos[0], m->pos[1]}))
         {
            hit = drop->drop;
         }
         if((m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE)))
         {
            drop->state = 3;
         }
      }
      else if(drop->state == 3)
      {
         if(HLH_gui_rect_inside(drop->drop->bounds, (HLH_gui_point){m->pos[0], m->pos[1]}))
         {
            hit = drop->drop;
         }
         if(!(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE)))
         {
            if(HLH_gui_rect_inside(drop->drop->bounds, (HLH_gui_point){m->pos[0], m->pos[1]}))
            {
               hit = drop->drop;
               drop->state = 0;

               HLH_gui_element_invisible(drop->drop, 1);
               hit = drop->drop;

               HLH_gui_element_redraw(&drop->e.window->e);
               HLH_gui_overlay_clear(&drop->e);
            }
            else
            {
               drop->state = 0;

               HLH_gui_element_invisible(drop->drop, 1);

               HLH_gui_element_redraw(&drop->e.window->e);
               HLH_gui_overlay_clear(&drop->e);
            }
         }
      }

      if(drop->state!=state_old)
         HLH_gui_element_redraw(&drop->e);
      if(hit!=NULL)
      {
         HLH_gui_handle_mouse(hit, *m);
      }

      m->handled = true;

      return drop->state > 0;
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
   uint64_t style = p->e.flags.style;
   if(style==0)
   {
      HLH_gui_rect bounds = p->e.bounds;
      int scale = HLH_gui_get_scale();

      //Infill
      HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.min[0] + HLH_gui_get_scale(), bounds.min[1] + HLH_gui_get_scale(), bounds.max[0] - HLH_gui_get_scale(), bounds.max[1] - HLH_gui_get_scale()), HLH_gui_theme_current.bg);

      //Outline
      HLH_gui_draw_rectangle(&p->e, bounds, HLH_gui_theme_current.border);

      //Border
      if(p->state)
      {
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.border);
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.border);

         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_dark);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);

         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_light);
         HLH_gui_draw_rectangle_fill(&p->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_light);
      }

      HLH_gui_draw_string(&p->e, bounds, p->text, p->text_len, HLH_gui_theme_current.text, 1);
   }
   else if(style==1)
   {
      HLH_gui_rect bounds = p->e.bounds;

      if(p->state)
         HLH_gui_draw_rectangle_fill(&p->e, bounds, HLH_gui_theme_current.bevel_dark);
      else
         HLH_gui_draw_rectangle_fill(&p->e, bounds, HLH_gui_theme_current.bg);

      HLH_gui_draw_string(&p->e, bounds, p->text, p->text_len, HLH_gui_theme_current.text, 1);
   }
}
//-------------------------------------
