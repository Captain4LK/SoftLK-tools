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
static void checkbutton_draw(HLH_gui_checkbutton *c);
//-------------------------------------

//Function implementations

HLH_gui_checkbutton *HLH_gui_checkbutton_create(HLH_gui_element *parent, uint64_t flags, const char *text, HLH_gui_rect *icon_bounds)
{
   HLH_gui_checkbutton *button = (HLH_gui_checkbutton *) HLH_gui_element_create(sizeof(*button), parent, flags, checkbutton_msg);
   button->e.type = "checkbutton";

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
         HLH_gui_element_msg(e, HLH_GUI_MSG_CLICK, b->checked, NULL);
   }
}

static int checkbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_checkbutton *button = (HLH_gui_checkbutton *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      if(button->is_icon)
         return (button->icon_bounds.maxx - button->icon_bounds.minx) + 6 * HLH_gui_get_scale();
      else
         return (HLH_GUI_GLYPH_HEIGHT + 8) * HLH_gui_get_scale() + button->text_len * HLH_GUI_GLYPH_WIDTH * HLH_gui_get_scale() + 10 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      if(button->is_icon)
         return (button->icon_bounds.maxy - button->icon_bounds.miny) + 6 * HLH_gui_get_scale();
      else
         return HLH_GUI_GLYPH_HEIGHT * HLH_gui_get_scale() + 8 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      checkbutton_draw(button);
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {}
   else if(msg==HLH_GUI_MSG_HIT)
   {
      HLH_gui_mouse *m = dp;

      int click = 0;
      int state_old = button->state;
      if(m->button & HLH_GUI_MOUSE_OUT)
      {
         button->state = 0;
      }
      else if(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
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
   uint64_t style = c->e.flags & HLH_GUI_STYLE;

   if(c->is_icon)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = c->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + scale, bounds.miny + scale, bounds.maxx - scale, bounds.maxy - scale), 0xff5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(&c->e, bounds, 0xff000000);

      //Border
      if(c->state||c->checked)
      {
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff000000);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff000000);

         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xff323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff323232);

         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xffc8c8c8);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xffc8c8c8);
      }

      int width = c->icon_bounds.maxx - c->icon_bounds.minx;
      int height = c->icon_bounds.maxy - c->icon_bounds.miny;
      SDL_Rect src = {.x = c->icon_bounds.minx, .y = c->icon_bounds.miny, .w = width, .h = height};
      SDL_Rect dst = {.x = bounds.minx + 3 * scale, .y = bounds.miny + 3 * scale, .w = width, .h = height};
      SDL_RenderCopy(c->e.window->renderer, c->e.window->icons, &src, &dst);

      return;
   }

   if(style==HLH_GUI_STYLE_00)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = c->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + scale, bounds.miny + scale, bounds.maxx - scale, bounds.maxy - scale), 0xff5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(&c->e, bounds, 0xff000000);

      //Border
      if(c->state)
      {
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff000000);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff000000);

         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xff323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 1 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff323232);

         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xffc8c8c8);
         HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xffc8c8c8);
      }

      //Checkbox
      int height = (bounds.maxy - bounds.miny);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*scale;
      int offset = (height - dim) / 2;
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + offset + 1 * scale, bounds.miny + offset + scale, bounds.minx + offset + 2 * scale, bounds.maxy - offset), 0xffc8c8c8);
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + offset + 1 * scale, bounds.maxy - offset - scale, bounds.minx + offset + dim, bounds.maxy - offset), 0xffc8c8c8);

      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + offset + 2 * scale, bounds.miny + offset, bounds.minx + offset + dim + 1 * scale, bounds.miny + offset + scale), 0xff323232);
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + dim + offset + 0 * scale, bounds.miny + offset + scale, bounds.minx + dim + offset + 1 * scale, bounds.maxy - offset - scale), 0xff323232);

      HLH_gui_draw_string(&c->e, HLH_gui_rect_make(bounds.minx + dim + 2 * scale, bounds.miny, bounds.maxx, bounds.maxy), c->text, c->text_len, 0xff000000, 1);

      if(c->checked)
         HLH_gui_draw_string(&c->e, HLH_gui_rect_make(bounds.minx + offset + 3 * scale, bounds.miny + offset + scale, bounds.minx + offset + 1 * scale + dim, bounds.miny + offset + scale + dim), "X", 1, 0xff000000, 1);
   }
   else if(style==HLH_GUI_STYLE_01)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = c->e.bounds;

      if(c->state)
         HLH_gui_draw_rectangle_fill(&c->e, bounds, 0xff323232);
      else
         HLH_gui_draw_rectangle_fill(&c->e, bounds, 0xff5a5a5a);

      //Checkbox
      int height = (bounds.maxy - bounds.miny);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*scale;
      int offset = (height - dim) / 2;
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + offset + 1 * scale, bounds.miny + offset + scale, bounds.minx + offset + 2 * scale, bounds.maxy - offset), 0xffc8c8c8);
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + offset + 1 * scale, bounds.maxy - offset - scale, bounds.minx + offset + dim, bounds.maxy - offset), 0xffc8c8c8);

      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + offset + 2 * scale, bounds.miny + offset, bounds.minx + offset + dim + 1 * scale, bounds.miny + offset + scale), 0xff323232);
      HLH_gui_draw_rectangle_fill(&c->e, HLH_gui_rect_make(bounds.minx + dim + offset + 0 * scale, bounds.miny + offset + scale, bounds.minx + dim + offset + 1 * scale, bounds.maxy - offset - scale), 0xff323232);

      HLH_gui_draw_string(&c->e, HLH_gui_rect_make(bounds.minx + dim + 2 * scale, bounds.miny, bounds.maxx, bounds.maxy), c->text, c->text_len, 0xff000000, 1);

      if(c->checked)
         HLH_gui_draw_string(&c->e, HLH_gui_rect_make(bounds.minx + offset + 3 * scale, bounds.miny + offset + scale, bounds.minx + offset + 1 * scale + dim, bounds.miny + offset + scale + dim), "X", 1, 0xff000000, 1);
   }
}
//-------------------------------------
