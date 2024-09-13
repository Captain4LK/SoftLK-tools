/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include "HLH.h"
#include "HLH_gui.h"
//-------------------------------------

//Internal includes
#include "brush.h"
#include "undo.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static SDL_Texture *brush_icons = NULL;
//-------------------------------------

//Function prototypes
static int gui_brush_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void brush_draw(GUI_brush *brush);
//-------------------------------------

//Function implementations

int brush_place(Project *project, const Settings *settings, const Brush *brush, int x, int y, int layer, uint8_t color)
{
   //Center brush
   x-=brush->width/2;
   y-=brush->height/2;
   
   if(layer!=project->num_layers-1)
   {
      for(int by = y/16;by<=(y+brush->height)/16;by++)
      {
         if(by<0||by>=(project->height+15)/16)
            continue;

         for(int bx = x/16;bx<=(x+brush->width)/16;bx++)
         {
            if(bx<0||bx>=(project->width+15)/16)
               continue;

            if(HLH_bitmap_check(project->undo_map,by*((project->width+15)/16)+bx))
               continue;

            HLH_bitmap_set(project->undo_map,by*((project->width+15)/16)+bx);
            undo_track_layer_chunk(project,bx,by,layer);
         }
      }
   }

   //Clip source texture
   int draw_start_y = 0;
   int draw_start_x = 0;
   int draw_end_x = brush->width;
   int draw_end_y = brush->height;
   if(x<0)
      draw_start_x = -x;
   if(y<0)
      draw_start_y = -y;
   if(x + draw_end_x>project->width)
      draw_end_x = brush->width + (project->width - x - draw_end_x);
   if(y + draw_end_y>project->height)
      draw_end_y = brush->height + (project->height - y - draw_end_y);

   //Clip dst sprite
   x = x<0?0:x;
   y = y<0?0:y;

   const uint8_t *src = &brush->data[draw_start_x + draw_start_y * brush->width];
   int src_step = -(draw_end_x - draw_start_x) + brush->width;

   for(int y1 = draw_start_y; y1<draw_end_y; y1++, src += src_step)
   {
      for(int x1 = draw_start_x; x1<draw_end_x; x1++, src++)
      {
         project->layers[layer]->data[(y+y1)*project->width+x+x1] = color;
         project_update(project,x+x1,y+y1,settings);
      }
   }

   return draw_start_y<draw_end_y&&draw_start_x<draw_end_x;
}

GUI_brush *gui_brush_create(HLH_gui_element *parent, uint64_t flags, Project *project, Settings *settings, int brush_num)
{
   GUI_brush *brush = (GUI_brush *)HLH_gui_element_create(sizeof(*brush),parent,flags,gui_brush_msg);
   brush->project = project;
   brush->settings = settings;
   brush->brush_num = brush_num;
   brush->icon_bounds.minx = brush_num*16;
   brush->icon_bounds.miny = 0;
   brush->icon_bounds.maxx = (brush_num+1)*16;
   brush->icon_bounds.maxy = 16;

   return brush;
}

void gui_brushes_update(HLH_gui_window *window, const Settings *settings)
{
   //NOTE(Captain4LK): this breaks when multiple windows include brush buttons
   if(brush_icons==NULL)
   {
      brush_icons = SDL_CreateTexture(window->renderer,SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STATIC,16*60,16);
      SDL_SetTextureBlendMode(brush_icons,SDL_BLENDMODE_BLEND);
   }

   uint32_t *data = calloc(16*60*16,sizeof(*data));
   for(int i = 0;i<60;i++)
   {
      if(settings->brushes[i]==NULL)
         continue;

      int width = HLH_min(16,settings->brushes[i]->width);
      int height = HLH_min(16,settings->brushes[i]->height);
      for(int y = 0;y<height;y++)
      {
         for(int x = 0;x<width;x++)
         {
            uint32_t color = 0;
            if(settings->brushes[i]->data[y*settings->brushes[i]->width+x])
               color = 0xff000000;
            data[(y+(16-height)/2)*16*60+x+(16-width)/2+i*16] = color;
         }
      }
   }

   SDL_UpdateTexture(brush_icons,NULL,data,16*60*4);
   free(data);
}

static int gui_brush_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   GUI_brush *brush = (GUI_brush *)e;
   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return (16*2+6)*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return (16*2+6)*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      brush_draw(brush);
   }
   else if(msg==HLH_GUI_MSG_GET_CHILD_SPACE)
   {}
   else if(msg==HLH_GUI_MSG_MOUSE_LEAVE)
   {
      int state_old = brush->state;
      brush->state = 0;
      if(state_old!=brush->state)
         HLH_gui_element_redraw(e);
   }
   else if(msg==HLH_GUI_MSG_MOUSE)
   {
      HLH_gui_mouse *m = dp;

      int click = 0;
      int state_old = brush->state;
      if(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
      {
         brush->state = 1;
      }
      else
      {
         click = brush->state==1;
         brush->state = 0;
      }

      if(click||state_old!=brush->state)
         HLH_gui_element_redraw(e);

      if(click)
      {
         HLH_gui_element_msg(e, HLH_GUI_MSG_CLICK, 0, NULL);
         brush->state = 0;
      }
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      if(brush_icons!=NULL)
      {
         SDL_DestroyTexture(brush_icons);
         brush_icons = NULL;
      }
   }

   return 0;
}

static void brush_draw(GUI_brush *brush)
{
   int scale = HLH_gui_get_scale();
   HLH_gui_rect bounds = brush->e.bounds;

   //Infill
   HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.minx + scale, bounds.miny + scale, bounds.maxx - scale, bounds.maxy - scale), 0xff5a5a5a);

   //Outline
   HLH_gui_draw_rectangle(&brush->e, bounds, 0xff000000);

   //Border
   if(brush->state)
   {
      HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff000000);
      HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff000000);

      HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xff323232);
      HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xff323232);
   }
   else
   {
      HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff323232);
      HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff323232);

      HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xffc8c8c8);
      HLH_gui_draw_rectangle_fill(&brush->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xffc8c8c8);
   }

   if(brush_icons!=NULL)
   {
      int width = brush->icon_bounds.maxx - brush->icon_bounds.minx;
      int height = brush->icon_bounds.maxy - brush->icon_bounds.miny;
      SDL_Rect src = {.x = brush->icon_bounds.minx, .y = brush->icon_bounds.miny, .w = width, .h = height};
      //printf("%d %d %d %d\n",src.x,src.y,src.w,src.h);
      SDL_Rect dst = {.x = bounds.minx + 3 * scale, .y = bounds.miny + 3 * scale, .w = width*2, .h = height*2};
      SDL_RenderCopy(brush->e.window->renderer, brush_icons, &src, &dst);
   }
}
//-------------------------------------
