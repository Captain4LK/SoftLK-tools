/*
HLH_gui - gui framework

Written in 2023,2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
static int64_t checkbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp);
static void checkbutton_draw(HLH_gui_checkbutton *c);
//-------------------------------------

//Function implementations

HLH_gui_checkbutton *HLH_gui_checkbutton_create(HLH_gui_element *parent, HLH_gui_flags flags, const char *text, HLH_gui_rect *icon_bounds)
{
   HLH_gui_checkbutton *button = (HLH_gui_checkbutton *) HLH_gui_element_create(sizeof(*button), parent, flags, checkbutton_msg);
   button->e.type = HLH_GUI_CHECKBUTTON;

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

void HLH_gui_checkbutton_set(HLH_gui_checkbutton *c, int checked, int trigger_msg, int redraw)
{
   if(c==NULL)
      return;

   int previously = c->checked;
   c->checked = checked;
   if(previously!=c->checked)
   {
      if(redraw)
         HLH_gui_element_redraw(&c->e);

      if(trigger_msg)
         HLH_gui_element_msg(&c->e, HLH_GUI_MSG_CLICK, c->checked, NULL);
   }
}

static int64_t checkbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   HLH_gui_checkbutton *button = (HLH_gui_checkbutton *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      if(button->is_icon)
         return (button->icon_bounds.max[0] - button->icon_bounds.min[0]) + 6 * HLH_gui_get_scale();
      else
         return (HLH_GUI_GLYPH_HEIGHT + 8) * HLH_gui_get_scale() + button->text_len * HLH_GUI_GLYPH_WIDTH * HLH_gui_get_scale() + 10 * HLH_gui_get_scale();
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
      checkbutton_draw(button);
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

      if(click)
      {
         button->checked = !button->checked;
         HLH_gui_element_msg(e, HLH_GUI_MSG_CLICK, button->checked, NULL);
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

static void checkbutton_draw(HLH_gui_checkbutton *c)
{
   uint64_t style = c->e.flags.style;

   if(c->is_icon)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = c->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + scale, bounds.min[1] + scale, bounds.max[0] - scale, bounds.max[1] - scale), HLH_gui_theme_current.bg);

      //Outline
      HLH_gui_draw_rectangle(&c->e, bounds, HLH_gui_theme_current.border);

      //Border
      if(c->state||c->checked)
      {
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.border);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.border);

         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_dark);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);

         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_light);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_light);
      }

      int width = c->icon_bounds.max[0] - c->icon_bounds.min[0];
      int height = c->icon_bounds.max[1] - c->icon_bounds.min[1];
      SDL_FRect src = {.x = c->icon_bounds.min[0], .y = c->icon_bounds.min[1], .w = width, .h = height};
      SDL_FRect dst = {.x = bounds.min[0] + 3 * scale, .y = bounds.min[1] + 3 * scale, .w = width, .h = height};
      SDL_RenderTexture(c->e.window->sdl_renderer, c->e.window->sdl_icons, &src, &dst);

      return;
   }

   if(style==0)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = c->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + scale, bounds.min[1] + scale, bounds.max[0] - scale, bounds.max[1] - scale), HLH_gui_theme_current.bg);

      //Outline
      HLH_gui_draw_rectangle(&c->e, bounds, HLH_gui_theme_current.border);

      //Border
      if(c->state)
      {
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.border);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.border);

         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_dark);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);

         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_light);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_light);
      }

      //Checkbox
      int height = (bounds.max[1] - bounds.min[1]);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*scale;
      int offset = (height - dim) / 2;
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + offset + 1 * scale, bounds.min[1] + offset + scale, bounds.min[0] + offset + 2 * scale, bounds.max[1] - offset), HLH_gui_theme_current.bevel_light);
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + offset + 1 * scale, bounds.max[1] - offset - scale, bounds.min[0] + offset + dim, bounds.max[1] - offset), HLH_gui_theme_current.bevel_light);

      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + offset + 2 * scale, bounds.min[1] + offset, bounds.min[0] + offset + dim + 1 * scale, bounds.min[1] + offset + scale), HLH_gui_theme_current.bevel_dark);
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + dim + offset + 0 * scale, bounds.min[1] + offset + scale, bounds.min[0] + dim + offset + 1 * scale, bounds.max[1] - offset - scale), HLH_gui_theme_current.bevel_dark);

      HLH_gui_draw_string(&c->e, HLH_gui_rect_make(bounds.min[0] + dim + 2 * scale, bounds.min[1], bounds.max[0], bounds.max[1]), c->text, c->text_len, HLH_gui_theme_current.text, 1);

      if(c->checked)
         HLH_gui_draw_string(&c->e, HLH_gui_rect_make(bounds.min[0] + offset + 3 * scale, bounds.min[1] + offset + scale, bounds.min[0] + offset + 1 * scale + dim, bounds.min[1] + offset + scale + dim), "X", 1, HLH_gui_theme_current.text, 1);
   }
   else if(style==1)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = c->e.bounds;

      if(c->state)
         HLH_gui_draw_rectangle_fill(&c->e, bounds, HLH_gui_theme_current.bevel_dark);
      else
         HLH_gui_draw_rectangle_fill(&c->e, bounds, HLH_gui_theme_current.bg);

      //Checkbox
      int height = (bounds.max[1] - bounds.min[1]);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*scale;
      int offset = (height - dim) / 2;
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + offset + 1 * scale, bounds.min[1] + offset + scale, bounds.min[0] + offset + 2 * scale, bounds.max[1] - offset), HLH_gui_theme_current.bevel_light);
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + offset + 1 * scale, bounds.max[1] - offset - scale, bounds.min[0] + offset + dim, bounds.max[1] - offset), HLH_gui_theme_current.bevel_light);

      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + offset + 2 * scale, bounds.min[1] + offset, bounds.min[0] + offset + dim + 1 * scale, bounds.min[1] + offset + scale), HLH_gui_theme_current.bevel_dark);
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.min[0] + dim + offset + 0 * scale, bounds.min[1] + offset + scale, bounds.min[0] + dim + offset + 1 * scale, bounds.max[1] - offset - scale), HLH_gui_theme_current.bevel_dark);

      HLH_gui_draw_string(&c->e, HLH_gui_rect_make(bounds.min[0] + dim + 2 * scale, bounds.min[1], bounds.max[0], bounds.max[1]), c->text, c->text_len, HLH_gui_theme_current.text, 1);

      if(c->checked)
         HLH_gui_draw_string(&c->e, HLH_gui_rect_make(bounds.min[0] + offset + 3 * scale, bounds.min[1] + offset + scale, bounds.min[0] + offset + 1 * scale + dim, bounds.min[1] + offset + scale + dim), "X", 1, HLH_gui_theme_current.text, 1);
   }
}
//-------------------------------------
