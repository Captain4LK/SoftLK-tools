/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <string.h>
//-------------------------------------

//Internal includes
#include "layer.h"
#include "canvas.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static int gui_layer_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

Layer *layer_new(size_t size)
{
   Layer *l = calloc(1,sizeof(*l)+size);

   return l;
}

void layer_free(Layer *layer)
{
   if(layer==NULL)
      return;

   free(layer);
}

void layer_copy(Layer *dst, const Layer *src, size_t size)
{
   memcpy(dst,src,size);
}

GUI_layer *gui_layer_create(HLH_gui_element *parent, uint64_t flags, GUI_canvas *canvas, int layer_num)
{
   GUI_layer *layer = (GUI_layer *)HLH_gui_element_create(sizeof(*layer),parent,flags,gui_layer_msg);
   layer->e.type = HLH_GUI_USER+1;
   layer->layer_num = layer_num;
   layer->canvas = canvas;

   char text[512];
   snprintf(text,512,"%d",layer_num+1);
   layer->text_len = (int)strlen(text);
   layer->text = malloc(layer->text_len + 1);
   strcpy(layer->text, text);

   return layer;
}

void gui_layer_set(GUI_layer *l, uint8_t button)
{
   if(l==NULL)
      return;

   if(button!=HLH_GUI_MOUSE_RIGHT)
   {
      if(l->e.parent!=NULL)
      {
         for(int i = 0; i<l->e.parent->child_count; i++)
         {
            HLH_gui_element *c = l->e.parent->children[i];

            if(c->type==HLH_GUI_USER+1)
            {
               GUI_layer *b = (GUI_layer *)c;

               //Send message to previous button
               if(b->checked&&b->e.id!=l->e.id)
                  HLH_gui_element_msg(&b->e, HLH_GUI_MSG_CLICK, 0, NULL);
               b->checked = 0;
            }
         }
      }
      l->checked = 1;
   }

   HLH_gui_element_msg(&l->e, HLH_GUI_MSG_CLICK, button, NULL);
   if(l->e.parent!=NULL)
      HLH_gui_element_redraw(l->e.parent);
   else
      HLH_gui_element_redraw(&l->e);

   project_update_full(l->canvas->project,l->canvas->settings);
   gui_canvas_update_project(l->canvas,l->canvas->project);
   HLH_gui_element_redraw(&l->canvas->e);
}

static int gui_layer_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   GUI_layer *layer = (GUI_layer *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return HLH_GUI_GLYPH_WIDTH*HLH_gui_get_scale()*3+10*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT * HLH_gui_get_scale() + 8 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {}
   else if(msg==HLH_GUI_MSG_MOUSE)
   {
      HLH_gui_mouse *m = dp;

      if(!m->button)
      {
         layer->state = 1;
      }
      if(layer->state&&m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
      {
         gui_layer_set(layer,m->button);
         layer->state = 0;
      }
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      free(layer->text);
   }
   else if(msg==HLH_GUI_MSG_MOUSE_LEAVE)
   {
      layer->state = 0;
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = layer->e.bounds;

      //Infill
      if(layer->checked)
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + HLH_gui_get_scale(), bounds.miny + HLH_gui_get_scale(), bounds.maxx - HLH_gui_get_scale(), bounds.maxy - HLH_gui_get_scale()), 0xffeeeeee);
      else if(!layer->canvas->project->layers[layer->layer_num]->hidden)
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + HLH_gui_get_scale(), bounds.miny + HLH_gui_get_scale(), bounds.maxx - HLH_gui_get_scale(), bounds.maxy - HLH_gui_get_scale()), 0xff404040);
      else
         HLH_gui_draw_rectangle_fill(&layer->e, HLH_gui_rect_make(bounds.minx + HLH_gui_get_scale(), bounds.miny + HLH_gui_get_scale(), bounds.maxx - HLH_gui_get_scale(), bounds.maxy - HLH_gui_get_scale()), 0xff000000);

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
         HLH_gui_draw_string(&layer->e, bounds, layer->text, layer->text_len, 0xffeeeeee, 1);
   }

   return 0;
}
//-------------------------------------
