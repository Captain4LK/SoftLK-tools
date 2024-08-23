/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include "HLH_gui.h"
//-------------------------------------

//Internal includes
#include "canvas.h"
#include "draw.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static int gui_canvas_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void canvas_draw(GUI_canvas *canvas);
//-------------------------------------

//Function implementations

GUI_canvas *gui_canvas_create(HLH_gui_element *parent, uint64_t flags, Project *project)
{
   GUI_canvas *canvas = (GUI_canvas *)HLH_gui_element_create(sizeof(*canvas),parent,flags,gui_canvas_msg);
   canvas->project = project;
   canvas->scale = 1.f;
   canvas->x = 0.f;
   canvas->y = 0.f;
   canvas->img = SDL_CreateTexture(canvas->e.window->renderer,SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STREAMING,project->width,project->height);

   return canvas;
}

void gui_canvas_update_project(GUI_canvas *canvas, Project *project)
{
   if(project==NULL||canvas==NULL)
      return;

   Image32 *img = project_to_image32(project);

   if(canvas->project->width==project->width&&canvas->project->height==project->height)
   {
      if(canvas->project!=project)
         project_free(canvas->project);
      canvas->project = project;

      void *data;
      int stride;
      SDL_LockTexture(canvas->img, NULL, &data, &stride);

      uint32_t * restrict pix = data;
      for(int i = 0; i<project->width* project->height; i++)
         pix[i] = img->data[i];

      SDL_UnlockTexture(canvas->img);

   }
   else
   {
      project_free(canvas->project);
      canvas->project = project;

      SDL_DestroyTexture(canvas->img);
      canvas->img = SDL_CreateTexture(canvas->e.window->renderer,SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STREAMING,project->width,project->height);

      void *data;
      int stride;
      SDL_LockTexture(canvas->img, NULL, &data, &stride);

      uint32_t * restrict pix = data;
      for(int i = 0; i<project->width* project->height; i++)
         pix[i] = img->data[i];

      SDL_UnlockTexture(canvas->img);
   }
}

static int gui_canvas_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   GUI_canvas *canvas = (GUI_canvas *)e;
   if(msg==HLH_GUI_MSG_DRAW)
   {
      canvas_draw(canvas);
   }
   else if(msg==HLH_GUI_MSG_MOUSE)
   {
      HLH_gui_mouse *m = dp;

      int redraw = 0;

      if(m->wheel==0)
      //if(m->button&(HLH_GUI_MOUSE_LEFT|HLH_GUI_MOUSE_RIGHT))
      {
         float mx = (float)(m->pos.x-canvas->e.bounds.minx);
         float my = (float)(m->pos.y-canvas->e.bounds.miny);
         int32_t x = 0;
         int32_t y = 0;
         x = (int32_t)(((mx-canvas->x)/canvas->scale)*256);
         y = (int32_t)(((my-canvas->y)/canvas->scale)*256);
         if(draw_event(canvas->project,x,y,m->button))
         {
            gui_canvas_update_project(canvas,canvas->project);
            HLH_gui_element_redraw(&canvas->e);
         }
      }

      if(m->wheel>0&&canvas->scale<=64.f)
      {
         float mx = (float)(m->pos.x-canvas->e.bounds.minx);
         float my = (float)(m->pos.y-canvas->e.bounds.miny);
         float x = (mx-canvas->x)/canvas->scale;
         float y = (my-canvas->y)/canvas->scale;
         float scale_change = -canvas->scale*0.15f;
         canvas->x+=x*scale_change;
         canvas->y+=y*scale_change;
         canvas->scale+=canvas->scale*0.15f;
         redraw = 1;
      }
      else if(m->wheel<0&&canvas->scale>=0.1f)
      {
         float mx = (float)(m->pos.x-canvas->e.bounds.minx);
         float my = (float)(m->pos.y-canvas->e.bounds.miny);
         float x = (mx-canvas->x)/canvas->scale;
         float y = (my-canvas->y)/canvas->scale;
         float scale_change = canvas->scale*0.15f;
         canvas->x+=x*scale_change;
         canvas->y+=y*scale_change;
         canvas->scale-=canvas->scale*0.15f;
         redraw = 1;
      }

      if(m->button&HLH_GUI_MOUSE_MIDDLE&&(m->rel.x!=0||m->rel.y!=0))
      {
         canvas->x+=(float)m->rel.x;
         canvas->y+=(float)m->rel.y;
         redraw = 1;
      }

      if(redraw)
         HLH_gui_element_redraw(&canvas->e);
   }

   return 0;
}

static void canvas_draw(GUI_canvas *canvas)
{
   HLH_gui_draw_rectangle_fill(&canvas->e, canvas->e.bounds, 0xff5a5a5a);

   SDL_Rect clip;
   clip.x = canvas->e.bounds.minx;
   clip.y = canvas->e.bounds.miny;
   clip.w = canvas->e.bounds.maxx-canvas->e.bounds.minx;
   clip.h = canvas->e.bounds.maxy-canvas->e.bounds.miny;
   SDL_RenderSetClipRect(canvas->e.window->renderer,&clip);
   SDL_Rect dst = {0};
   dst.x = (int)((float)canvas->e.bounds.minx+canvas->x);
   dst.y = (int)((float)canvas->e.bounds.miny+canvas->y);
   dst.w = (int)((float)canvas->project->width*canvas->scale);
   dst.h = (int)((float)canvas->project->height*canvas->scale);
   SDL_RenderCopy(canvas->e.window->renderer, canvas->img, NULL, &dst);
   SDL_RenderSetClipRect(canvas->e.window->renderer,NULL);
}
//-------------------------------------
