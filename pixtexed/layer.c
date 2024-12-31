/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <string.h>

#include "HLH.h"
//-------------------------------------

//Internal includes
#include "layer.h"
#include "canvas.h"
#include "shared/color.h"
#include "palette.h"
#include "undo.h"
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
   Layer *l = calloc(1,sizeof(*l)+size*2);

   l->type = LAYER_BLEND;
   l->opacity = 1.f;

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

void layer_update_settings(Project *project, Layer *layer, const Settings *settings)
{
   float s0 = layer->opacity;
   float s1 = 1.f-layer->opacity;

   if(layer->type==LAYER_BLEND)
   {
      for(int c0 = 0;c0<256;c0++)
      {
         float cr0 = (float)color32_r(settings->palette[c0]);
         float cg0 = (float)color32_g(settings->palette[c0]);
         float cb0 = (float)color32_b(settings->palette[c0]);

         for(int c1 = 0;c1<256;c1++)
         {
            float cr1 = (float)color32_r(settings->palette[c1]);
            float cg1 = (float)color32_g(settings->palette[c1]);
            float cb1 = (float)color32_b(settings->palette[c1]);

            if(c1==0)
               layer->lut[c0][c1] = (uint8_t)c0;
            else if(c0==0)
               layer->lut[c0][c1] = (uint8_t)c1;
            else
               layer->lut[c0][c1] = palette_closest(settings,color32((uint8_t)(cr0*s1+cr1*s0),(uint8_t)(cg0*s1+cg1*s0),(uint8_t)(cb0*s1+cb1*s0),255));
         }
      }
   }
   else if(layer->type==LAYER_BUMP)
   {
      FILE *f = fopen("/tmp/out.ppm","w");
      fprintf(f,"P3\n%d %d\n255\n",256,256);
      for(int c0 = 0;c0<256;c0++)
      {
         //float cr0 = (float)color32_r(settings->palette[c0]);
         //float cg0 = (float)color32_g(settings->palette[c0]);
         //float cb0 = (float)color32_b(settings->palette[c0]);
         float s = (float)c0/8.f;

         for(int c1 = 0;c1<256;c1++)
         {
            float cr1 = (float)color32_r(settings->palette[c1]);
            float cg1 = (float)color32_g(settings->palette[c1]);
            float cb1 = (float)color32_b(settings->palette[c1]);
            cr1 = HLH_min(cr1*s,255.f);
            cg1 = HLH_min(cg1*s,255.f);
            cb1 = HLH_min(cb1*s,255.f);

            layer->lut[c1][c0] = palette_closest(settings,color32((uint8_t)(cr1),(uint8_t)(cg1),(uint8_t)(cb1),255));



            uint32_t c = settings->palette[layer->lut[c1][c0]];
            fprintf(f,"%d %d %d\n",color32_r(c),color32_g(c),color32_b(c));
         }
      }

      fclose(f);

      //FILE *f = fopen("/tmp/out.ppm","w");
      //fprintf(f,"P3\n%d %d\n255\n",project->width,project->height);

      //Ambient light, so that flat areas stay original color
      float base = 1.f-layer->light_dir_nz;

      for(int y = 0; y<project->height; y++)
      {
         for(int x = 0; x<project->width; x++)
         {
            int px = x%project->width;
            int py = y%project->height;

            uint32_t p0 = layer->data[py*project->width+px];
            uint32_t p1 = layer->data[HLH_wrap(py+1,project->height)*project->width+px];
            uint32_t p2 = layer->data[py*project->width+HLH_wrap(px+1,project->width)];
            uint32_t p3 = layer->data[HLH_wrap(py+1,project->height)*project->width+HLH_wrap(px+1,project->width)];

            float h0 = (float)(color32_r(p0)+color32_g(p0)+color32_b(p0))/(3.f*256.f);
            float h1 = (float)(color32_r(p1)+color32_g(p1)+color32_b(p1))/(3.f*256.f);
            float h2 = (float)(color32_r(p2)+color32_g(p2)+color32_b(p2))/(3.f*256.f);
            float h3 = (float)(color32_r(p3)+color32_g(p3)+color32_b(p3))/(3.f*256.f);

            float nx = ((h0-h2)+(h0-h3))/2.0f;
            float ny = ((h0-h1)+(h0-h3))/2.0f;
            float nz = 1.f;
            float len = sqrtf(nx*nx+ny*ny+nz*nz);
            nx/=len;
            ny/=len;
            nz/=len;

            float diff = nx*layer->light_dir_nx;
            diff+=ny*layer->light_dir_ny;
            diff+=nz*layer->light_dir_nz;

            diff+=base;

            int idiff = 0;
            if(fabsf(diff-1.f)<1e-2)
               idiff = 8;
            else if(diff<1.f)
               idiff = (int)floorf(diff*8.f);
            else
               idiff = (int)ceil(diff*8.f);

            layer->data[project->height*project->width+py*project->width+px] = (uint8_t)HLH_max(0,HLH_min(idiff,255));
            project_update(project,x,y,settings);
         }
      }

      //fclose(f);
   }
}

void layer_update_contents(Layer *layer)
{
   if(layer->type==LAYER_BLEND)
      return;
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

   if(button==HLH_GUI_MOUSE_LEFT)
   {
      l->canvas->project->layer_selected = l->layer_num;
      l->canvas->project->layers[l->layer_num]->hidden = 0;
   }
   else if(button==HLH_GUI_MOUSE_RIGHT)
   {
      if(l->layer_num==l->canvas->project->layer_selected)
      {
         int found = 0;
         for(int i = 0;i<l->canvas->project->num_layers-1;i++)
         {
            if(i==l->layer_num)
               continue;
            if(!l->canvas->project->layers[i]->hidden)
            {
               found = 1;
               break;
            }
         }

         for(int i = 0;i<l->canvas->project->num_layers-1;i++)
         {
            if(i==l->layer_num)
               continue;

            if(found)
            {
               if(!l->canvas->project->layers[i]->hidden)
                  l->canvas->project->layers[i]->hidden = 2;
            }
            else if(l->canvas->project->layers[i]->hidden==2)
            {
               l->canvas->project->layers[i]->hidden = 0;
            }
         }
      }
      else
      {
         l->canvas->project->layers[l->layer_num]->hidden = !l->canvas->project->layers[l->layer_num]->hidden;
      }
   }

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
