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
static const char *radiobutton_type = "radiobutton";
//-------------------------------------

//Function prototypes
static int64_t radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp);
static void radiobutton_draw(HLH_gui_radiobutton *r);
//-------------------------------------

//Function implementations

HLH_gui_radiobutton *HLH_gui_radiobutton_create(HLH_gui_element *parent, HLH_gui_flags flags, const char *text, HLH_gui_rect *icon_bounds)
{
   HLH_gui_radiobutton *button = (HLH_gui_radiobutton *) HLH_gui_element_create(sizeof(*button), parent, flags, radiobutton_msg);
   button->e.type = HLH_GUI_RADIOBUTTON;

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

   HLH_gui_radiobutton_set(button, 0, 0);

   return button;
}

void HLH_gui_radiobutton_set(HLH_gui_radiobutton *r, int trigger_msg, int redraw)
{
   if(r==NULL)
      return;

   int previously = r->checked;
   if(r->e.parent!=NULL)
   {
      for(int i = 0; i<r->e.parent->child_count; i++)
      {
         HLH_gui_element *c = r->e.parent->children[i];

         if(c->type==HLH_GUI_RADIOBUTTON)
         {
            HLH_gui_radiobutton *b = (HLH_gui_radiobutton *)c;

            //Send message to previous button
            if(b->checked&&trigger_msg&&b->e.id!=r->e.id)
               HLH_gui_element_msg(&b->e, HLH_GUI_MSG_CLICK, 0, NULL);
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
      HLH_gui_element_msg(&r->e, HLH_GUI_MSG_CLICK, 1, NULL);
}

static int64_t radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   HLH_gui_radiobutton *button = (HLH_gui_radiobutton *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      if(button->is_icon)
         return (button->icon_bounds.max[0] - button->icon_bounds.min[0]) + 6 * HLH_gui_get_scale();
      else if(button->text_len>0)
         return (HLH_GUI_GLYPH_HEIGHT + 8) * HLH_gui_get_scale() + button->text_len * HLH_GUI_GLYPH_WIDTH * HLH_gui_get_scale() + 10 * HLH_gui_get_scale();
      else
         return (HLH_GUI_GLYPH_HEIGHT + 8) * HLH_gui_get_scale();
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
      radiobutton_draw(button);
   }
   else if(msg==HLH_GUI_MSG_MOUSE_LEAVE)
   {
      bool state_old = button->state;
      button->state = false;
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

      bool click = false;
      bool state_old = button->state;
      if(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
      {
         button->state = true;
         m->handled = true;
      }
      else
      {
         click = button->state;
         button->state = false;
      }

      if(click)
      {
         HLH_gui_radiobutton_set(button, 1, 1);
         button->state = false;
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
   uint64_t style = r->e.flags.style;

   if(r->is_icon)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + scale, bounds.min[1] + scale, bounds.max[0] - scale, bounds.max[1] - scale), HLH_gui_theme_current.bg);

      //Outline
      HLH_gui_draw_rectangle(&r->e, bounds, HLH_gui_theme_current.border);

      //Border
      if(r->state||r->checked)
      {
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.border);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.border);

         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_dark);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);

         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_light);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_light);
      }

      int width = r->icon_bounds.max[0] - r->icon_bounds.min[0];
      int height = r->icon_bounds.max[1] - r->icon_bounds.min[1];
      SDL_FRect src = {.x = r->icon_bounds.min[0], .y = r->icon_bounds.min[1], .w = width, .h = height};
      SDL_FRect dst = {.x = bounds.min[0] + 3 * scale, .y = bounds.min[1] + 3 * scale, .w = width, .h = height};
      SDL_RenderTexture(r->e.window->sdl_renderer, r->e.window->sdl_icons, &src, &dst);

      return;
   }

   if(style==0)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      //Infill
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + HLH_gui_get_scale(), bounds.min[1] + HLH_gui_get_scale(), bounds.max[0] - HLH_gui_get_scale(), bounds.max[1] - HLH_gui_get_scale()), HLH_gui_theme_current.bg);

      //Outline
      HLH_gui_draw_rectangle(&r->e, bounds, HLH_gui_theme_current.border);

      //Border
      if(r->state)
      {
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.border);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.border);

         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_dark);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);

         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_light);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_light);
      }

      //Checkbox
      int height = (bounds.max[1] - bounds.min[1]);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      int offset = (height - dim) / 2;
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + offset + 1 * scale, bounds.min[1] + offset + scale, bounds.min[0] + offset + 2 * scale, bounds.max[1] - offset), HLH_gui_theme_current.bevel_light);
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + offset + 1 * scale, bounds.max[1] - offset - scale, bounds.min[0] + offset + dim, bounds.max[1] - offset), HLH_gui_theme_current.bevel_light);

      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + offset + 2 * scale, bounds.min[1] + offset, bounds.min[0] + offset + dim + 1 * scale, bounds.min[1] + offset + scale), HLH_gui_theme_current.bevel_dark);
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + dim + offset + 0 * scale, bounds.min[1] + offset + scale, bounds.min[0] + dim + offset + 1 * scale, bounds.max[1] - offset - scale), HLH_gui_theme_current.bevel_dark);

      HLH_gui_draw_string(&r->e, HLH_gui_rect_make(bounds.min[0] + dim + 2 * scale, bounds.min[1], bounds.max[0], bounds.max[1]), r->text, r->text_len, HLH_gui_theme_current.text, 1);

      if(r->checked)
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + offset + 4 * scale, bounds.min[1] + offset + 3 * scale, bounds.min[0] + dim + offset - 2 * scale, bounds.min[1] + offset - 3 * scale + dim), HLH_gui_theme_current.border);
   }
   else if(style==1)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      if(r->state)
         HLH_gui_draw_rectangle_fill(&r->e, bounds, HLH_gui_theme_current.bevel_dark);
      else
         HLH_gui_draw_rectangle_fill(&r->e, bounds, HLH_gui_theme_current.bg);

      //Checkbox
      int height = (bounds.max[1] - bounds.min[1]);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      int offset = (height - dim) / 2;
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + offset + 1 * scale, bounds.min[1] + offset + scale, bounds.min[0] + offset + 2 * scale, bounds.max[1] - offset), HLH_gui_theme_current.bevel_light);
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + offset + 1 * scale, bounds.max[1] - offset - scale, bounds.min[0] + offset + dim, bounds.max[1] - offset), HLH_gui_theme_current.bevel_light);

      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + offset + 2 * scale, bounds.min[1] + offset, bounds.min[0] + offset + dim + 1 * scale, bounds.min[1] + offset + scale), HLH_gui_theme_current.bevel_dark);
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + dim + offset + 0 * scale, bounds.min[1] + offset + scale, bounds.min[0] + dim + offset + 1 * scale, bounds.max[1] - offset - scale), HLH_gui_theme_current.bevel_dark);

      HLH_gui_draw_string(&r->e, HLH_gui_rect_make(bounds.min[0] + dim + 2 * scale, bounds.min[1], bounds.max[0], bounds.max[1]), r->text, r->text_len, HLH_gui_theme_current.text, 1);

      if(r->checked)
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.min[0] + offset + 4 * scale, bounds.min[1] + offset + 3 * scale, bounds.min[0] + dim + offset - 2 * scale, bounds.min[1] + offset - 3 * scale + dim), HLH_gui_theme_current.border);
   }
   else if(style==2)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      if(r->checked)
         HLH_gui_draw_rectangle_fill(&r->e, bounds, HLH_gui_theme_current.bevel_dark);
      else
         HLH_gui_draw_rectangle_fill(&r->e, bounds, HLH_gui_theme_current.bg);

      //int height = (bounds.max[1] - bounds.min[1]);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      HLH_gui_draw_string(&r->e, HLH_gui_rect_make(bounds.min[0] + dim + 2 * scale, bounds.min[1], bounds.max[0], bounds.max[1]), r->text, r->text_len, HLH_gui_theme_current.text, 1);
   }
}
//-------------------------------------
