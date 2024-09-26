/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//-------------------------------------

//Internal includes
#include "tool.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static int gui_tool_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void gui_tool_update_icons(GUI_tool *tool);
//-------------------------------------

//Function implementations

GUI_tool *gui_tool_create(HLH_gui_element *parent, uint64_t flags, GUI_canvas *canvas, Tool_type type)
{
   GUI_tool *tool = (GUI_tool *)HLH_gui_element_create(sizeof(*tool),parent,flags,gui_tool_msg);
   tool->e.type = HLH_GUI_USER+2;
   tool->canvas = canvas;
   tool->type = type;

   gui_tool_update_icons(tool);

   return tool;
}

void gui_tool_set(GUI_tool *t, uint8_t button)
{
   if(t==NULL)
      return;

   //if(button!=HLH_GUI_MOUSE_RIGHT)
   {
      if(t->e.parent!=NULL)
      {
         for(int i = 0; i<t->e.parent->child_count; i++)
         {
            HLH_gui_element *c = t->e.parent->children[i];

            if(c->type==HLH_GUI_USER+2)
            {
               GUI_tool *b = (GUI_tool *)c;

               //Send message to previous button
               if(b->checked&&b->e.id!=t->e.id)
               {
                  b->icon_bounds.miny = 0;
                  b->icon_bounds.maxy = 14;
                  //HLH_gui_element_msg(&b->e, HLH_GUI_MSG_CLICK, 0, NULL);
               }
               b->checked = 0;
            }
         }
      }
      t->checked = 1;
   }

   t->icon_bounds.miny = 14;
   t->icon_bounds.maxy = 28;

   if(button&HLH_GUI_MOUSE_RIGHT)
   {
      switch(t->type)
      {
      case TOOL_PEN:
         break;
      }
   }

   t->canvas->project->tools.selected = t->type;

   if(t->e.parent!=NULL)
      HLH_gui_element_redraw(t->e.parent);
   else
      HLH_gui_element_redraw(&t->e);
}

static void gui_tool_update_icons(GUI_tool *tool)
{
   switch(tool->type)
   {
   case TOOL_PEN:
      tool->icon_bounds.minx = 0;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_LINE:
      tool->icon_bounds.minx = 4*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 5*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_FLOOD:
      tool->icon_bounds.minx = 7*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 8*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_RECT_OUTLINE:
      tool->icon_bounds.minx = 9*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 10*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_RECT_FILL:
      tool->icon_bounds.minx = 10*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 11*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_GRADIENT:
      tool->icon_bounds.minx = 11*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 12*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_SPLINE:
      tool->icon_bounds.minx = 12*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 13*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_SPRAY:
      tool->icon_bounds.minx = 14*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 15*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_POLYLINE:
      tool->icon_bounds.minx = 15*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 16*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_POLYFORM:
      tool->icon_bounds.minx = 16*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 17*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_CIRCLE_OUTLINE:
      tool->icon_bounds.minx = 17*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 18*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_CIRCLE_FILL:
      tool->icon_bounds.minx = 21*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 22*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_SELECT_RECT:
      tool->icon_bounds.minx = 25*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 26*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_SELECT_LASSO:
      tool->icon_bounds.minx = 26*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 27*14;
      tool->icon_bounds.maxy = 14;
      break;
   case TOOL_PIPETTE:
      tool->icon_bounds.minx = 27*14;
      tool->icon_bounds.miny = 0;
      tool->icon_bounds.maxx = 28*14;
      tool->icon_bounds.maxy = 14;
      break;
   }
}

static int gui_tool_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   GUI_tool *tool = (GUI_tool *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return 32*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return 32*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {}
   else if(msg==HLH_GUI_MSG_MOUSE)
   {
      HLH_gui_mouse *m = dp;

      if(!m->button)
      {
         tool->state = 1;
      }
      if(tool->state&&m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
      {
         gui_tool_set(tool,m->button);
         tool->state = 0;
      }
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
   }
   else if(msg==HLH_GUI_MSG_MOUSE_LEAVE)
   {
      tool->state = 0;
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = tool->e.bounds;

      //Infill
      //if(tool->checked)
         //HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.minx + HLH_gui_get_scale(), bounds.miny + HLH_gui_get_scale(), bounds.maxx - HLH_gui_get_scale(), bounds.maxy - HLH_gui_get_scale()), 0xffeeeeee);
      //else
         //HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.minx + HLH_gui_get_scale(), bounds.miny + HLH_gui_get_scale(), bounds.maxx - HLH_gui_get_scale(), bounds.maxy - HLH_gui_get_scale()), 0xff5a5a5a);

      //Outline
      HLH_gui_draw_rectangle(&tool->e, bounds, 0xff000000);

      //Border
      if(tool->state)
      {
         HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff000000);
         HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff000000);

         HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xff323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 1 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff323232);

         HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xffc8c8c8);
         HLH_gui_draw_rectangle_fill(&tool->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xffc8c8c8);
      }

      //Icon
      int width = tool->icon_bounds.maxx - tool->icon_bounds.minx;
      int height = tool->icon_bounds.maxy - tool->icon_bounds.miny;
      SDL_Rect src = {.x = tool->icon_bounds.minx, .y = tool->icon_bounds.miny, .w = width, .h = height};
      SDL_Rect dst = {.x = bounds.minx + 2 * scale, .y = bounds.miny + 2 * scale, .w = width*2*scale, .h = height*2*scale};
      SDL_RenderCopy(tool->e.window->renderer, tool->e.window->icons, &src, &dst);

      /*
      //Outline
      HLH_gui_draw_rectangle(&layer->e, bounds, 0xff000000);

      //Border
      if(layer->state)
      {
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff000000);
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff000000);

         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xff323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 1 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff323232);

         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xffc8c8c8);
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xffc8c8c8);
      }

      int height = (bounds.maxy - bounds.miny);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      int offset = (height - dim) / 2;

      if(layer->checked)
         HLH_gui_draw_string(&layer->e, bounds, layer->text, layer->text_len, 0xff000000, 1);
      else
         HLH_gui_draw_string(&layer->e, bounds, layer->text, layer->text_len, 0xffeeeeee, 1);*/
   }

   return 0;
}
//-------------------------------------
