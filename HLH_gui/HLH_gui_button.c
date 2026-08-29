/*
HLH_gui - gui framework

Written in 2023, 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
static int64_t button_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp);
static void button_draw(HLH_gui_button *t);
//-------------------------------------

//Function implementations

HLH_gui_button *HLH_gui_button_create(HLH_gui_element *parent, HLH_gui_flags flags, const char *text, HLH_gui_rect *icon_bounds)
{
   HLH_gui_button *button = (HLH_gui_button *) HLH_gui_element_create(sizeof(*button), parent, flags, button_msg);
   button->e.type = HLH_GUI_BUTTON;

   if(text!=NULL)
   {
      button->text_len = (int)strlen(text);
      button->text = malloc(button->text_len + 1);
      strcpy(button->text, text);
   }
   else if(icon_bounds!=NULL)
   {
      button->is_icon = 1;
      button->icon_bounds = *icon_bounds;
   }

   return button;
}

static int64_t button_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      if(button->is_icon)
         return (button->icon_bounds.max[0] - button->icon_bounds.min[0]) + 6 * HLH_gui_get_scale();
      else
         return button->text_len * HLH_GUI_GLYPH_WIDTH * HLH_gui_get_scale() + 10 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      if(button->is_icon)
         return (button->icon_bounds.max[1] - button->icon_bounds.min[1]) + 6 * HLH_gui_get_scale();
      else
         return HLH_GUI_GLYPH_HEIGHT * HLH_gui_get_scale() + 8 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      button_draw(button);
   }
   else if(msg==HLH_GUI_MSG_MOUSE_LEAVE)
   {
      int state_old = button->state;
      button->state = 0;
      if(state_old!=button->state)
         HLH_gui_element_redraw(e);
   }
   else if(msg==HLH_GUI_MSG_MOUSE)
   {
      HLH_gui_mouse *m = dp;

      if(button->state)
      {
         m->handled = true;
      }

      int click = 0;
      int state_old = button->state;
      if(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
      {
         button->state = 1;
         m->handled = true;
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
         HLH_gui_element_msg(e, HLH_GUI_MSG_CLICK, 0, NULL);
         button->state = 0;
      }
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      if(button->text!=NULL)
         free(button->text);
   }

   return 0;
}

static void button_draw(HLH_gui_button *t)
{
   uint64_t style = t->e.flags.style;

   if(t->is_icon)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = t->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + scale, bounds.min[1] + scale, bounds.max[0] - scale, bounds.max[1] - scale), HLH_gui_theme_current.bg);

      //Outline
      HLH_gui_draw_rectangle(&t->e, bounds, HLH_gui_theme_current.border);

      //Border
      if(t->state)
      {
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.border);
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.border);

         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_dark);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);

         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_light);
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_light);
      }

      int width = t->icon_bounds.max[0] - t->icon_bounds.min[0];
      int height = t->icon_bounds.max[1] - t->icon_bounds.min[1];
      SDL_FRect src = {.x = t->icon_bounds.min[0], .y = t->icon_bounds.min[1], .w = width, .h = height};
      SDL_FRect dst = {.x = bounds.min[0] + 3 * scale, .y = bounds.min[1] + 3 * scale, .w = width, .h = height};
      SDL_RenderTexture(t->e.window->sdl_renderer, t->e.window->sdl_icons, &src, &dst);

      return;
   }

   if(style==0)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = t->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + scale, bounds.min[1] + scale, bounds.max[0] - scale, bounds.max[1] - scale), HLH_gui_theme_current.bg);

      //Outline
      HLH_gui_draw_rectangle(&t->e, bounds, HLH_gui_theme_current.border);

      //Border
      if(t->state)
      {
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.border);
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.border);

         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_dark);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);

         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_light);
         HLH_gui_draw_rectangle_fill(&t->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_light);
      }

      HLH_gui_draw_string(&t->e, bounds, t->text, t->text_len, HLH_gui_theme_current.text, 1);
   }
   else if(style==1)
   {
      HLH_gui_rect bounds = t->e.bounds;

      if(t->state)
         HLH_gui_draw_rectangle_fill(&t->e, bounds, HLH_gui_theme_current.bevel_dark);
      else
         HLH_gui_draw_rectangle_fill(&t->e, bounds, HLH_gui_theme_current.bg);

      HLH_gui_draw_string(&t->e, bounds, t->text, t->text_len, HLH_gui_theme_current.text, 1);
   }
}
//-------------------------------------
