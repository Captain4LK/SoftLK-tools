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
#include "draw.h"
#include "brush.h"
#include "undo.h"
#include "layer.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void draw_line(Project *project, const Settings *settings, const Brush *brush, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int layer, uint8_t color);
//int brush_place(Project *project, const Settings *settings, const Brush *brush, int x, int y, int layer,uint8_t color);

static uint8_t draw_clip_outcode(int32_t l, int32_t u, int32_t r, int32_t d, int32_t x, int32_t y);
static int draw_clip_line(int32_t l, int32_t u, int32_t r, int32_t d, int32_t *x0, int32_t *y0, int32_t *x1, int32_t *y1);

static int draw_event_pen_continuous(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);
static int draw_event_pen_discontinuous(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);
static int draw_event_pen_single(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);
static int draw_event_pen_contour_fill(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);

static int draw_event_line_single(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);
static int draw_event_line_connect(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);
static int draw_event_line_fan(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);

static int draw_event_flood_fill(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings);
static int draw_event_flood_swap(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings);

static int draw_event_rect_outline(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);
static int draw_event_rect_fill(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end);
//-------------------------------------

//Function implementations

int draw_event(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings)
{
   switch(project->tools.selected)
   {
   case TOOL_PEN:
      if(project->tools.pen.place_mode==0)
         return draw_event_pen_continuous(project,mx,my,button,settings,0);
      else if(project->tools.pen.place_mode==1)
         return draw_event_pen_discontinuous(project,mx,my,button,settings,0);
      else if(project->tools.pen.place_mode==2)
         return draw_event_pen_single(project,mx,my,button,settings,0);
      else if(project->tools.pen.place_mode==3)
         return draw_event_pen_contour_fill(project,mx,my,button,settings,0);
      break;
   case TOOL_LINE:
      if(project->tools.line.mode==0)
         return draw_event_line_single(project,mx,my,button,settings,0);
      else if(project->tools.line.mode==1)
         return draw_event_line_connect(project,mx,my,button,settings,0);
      else if(project->tools.line.mode==2)
         return draw_event_line_fan(project,mx,my,button,settings,0);
      break;
   case TOOL_FLOOD:
      if(project->tools.flood.mode==0)
         return draw_event_flood_fill(project,mx,my,button,settings);
      else if(project->tools.flood.mode==1)
         return draw_event_flood_swap(project,mx,my,button,settings);
      break;
   case TOOL_RECT_OUTLINE: return draw_event_rect_outline(project,mx,my,button,settings,0);
   case TOOL_RECT_FILL: return draw_event_rect_fill(project,mx,my,button,settings,0);
   //case TOOL_GRADIENT: return draw_event_gradient(project,mx,my,button,settings);
   //case TOOL_SPLINE: return draw_event_spline(project,mx,my,button,settings);
   }

   return 0;
}

int draw_event_end(Project *project, const Settings *settings)
{
   switch(project->tools.selected)
   {
   case TOOL_PEN:
      if(project->tools.pen.place_mode==0)
      {
         draw_event_pen_continuous(project,-1,-1,0,settings,0);
         draw_event_pen_continuous(project,-1,-1,0,settings,1);
         return 1;
      }
      else if(project->tools.pen.place_mode==1)
      {
         draw_event_pen_discontinuous(project,-1,-1,0,settings,0);
         draw_event_pen_discontinuous(project,-1,-1,0,settings,1);
         return 1;
      }
      else if(project->tools.pen.place_mode==2)
      {
         draw_event_pen_single(project,-1,-1,0,settings,0);
         draw_event_pen_single(project,-1,-1,0,settings,1);
         return 1;
      }
      else if(project->tools.pen.place_mode==3)
      {
         draw_event_pen_contour_fill(project,-1,-1,0,settings,0);
         draw_event_pen_contour_fill(project,-1,-1,0,settings,1);
         return 1;
      }
      break;
   case TOOL_LINE:
      if(project->tools.line.mode==0)
      {
         draw_event_line_single(project,-1,-1,0,settings,0);
         draw_event_line_single(project,-1,-1,0,settings,1);
         return 1;
      }
      else if(project->tools.line.mode==1)
      {
         draw_event_line_connect(project,-1,-1,0,settings,0);
         draw_event_line_connect(project,-1,-1,HLH_GUI_MOUSE_RIGHT,settings,0);
         draw_event_line_connect(project,-1,-1,0,settings,1);
         return 1;
      }
      else if(project->tools.line.mode==2)
      {
         draw_event_line_fan(project,-1,-1,0,settings,0);
         draw_event_line_fan(project,-1,-1,0,settings,1);
         return 1;
      }
      break;
   case TOOL_FLOOD: return 1; //Nothing needed here
   case TOOL_RECT_OUTLINE:
      draw_event_rect_outline(project,-1,-1,0,settings,0);
      draw_event_rect_outline(project,-1,-1,0,settings,1);
      return 1;
   //case TOOL_RECT_FILL: return draw_event_rect_fill(project,mx,my,button,settings);
   //case TOOL_GRADIENT: return draw_event_gradient(project,mx,my,button,settings);
   //case TOOL_SPLINE: return draw_event_spline(project,mx,my,button,settings);
   }

   return 0;
}

static void draw_line(Project *project, const Settings *settings, const Brush *brush, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int layer, uint8_t color)
{
   if(!draw_clip_line(0, 0, project->width* 256 - 1, project->height* 256 - 1, &x0, &y0, &x1, &y1))
      return;

   //This is basically bresenham's line algorithm,
   //with added fixed point precision
   int32_t frac_x = x0&255;
   int32_t frac_y = y0&255;
   int32_t dx = x1 - x0;
   int32_t dy = y1 - y0;
   int32_t step_x = 1;
   int32_t step_y = 1;

   if(dx<0)
   {
      frac_x = 255-(x0&255);
      step_x = -1;
   }
   if(dy<0)
   {
      frac_y = 255-(y0&255);
      step_y = -1;
   }

   dx = abs(dx);
   dy = abs(dy);

   int bx = x0/256;
   int by = y0/256;
   if(dx<dy)
   {
      int32_t dist = (int32_t)(((int64_t)(frac_x - 128) * dy - (int64_t)(frac_y - 128) * dx) / 256);
      int32_t left = abs(y1/256-y0/256)+1;

      while(left--)
      {
         if(dist>dy / 2)
         {
            bx+=step_x;
            dist -= dy;
         }

         brush_place(project,settings,brush,bx,by,layer,color);
         by+=step_y;
         dist += dx;
      }
   }
   else
   {
      int32_t dist = (int32_t)(((int64_t)(frac_y - 128) * dx - (int64_t)(frac_x - 128) * dy) / 256);
      int32_t left = abs(x1/256-x0/256)+1;

      while(left--)
      {
         if(dist>dx / 2)
         {
            by+=step_y;
            dist -= dx;
         }

         brush_place(project,settings,brush,bx,by,layer,color);
         bx+=step_x;
         dist += dy;
      }
   }
}


static uint8_t draw_clip_outcode(int32_t l, int32_t u, int32_t r, int32_t d, int32_t x, int32_t y)
{
   uint8_t code = 0;

   if(x<l)
      code |= 1;
   else if(x>r)
      code |= 2;

   if(y<u)
      code |= 4;
   if(y>d)
      code |= 8;

   return code;
}

static int draw_clip_line(int32_t l, int32_t u, int32_t r, int32_t d, int32_t *x0, int32_t *y0, int32_t *x1, int32_t *y1)
{
   uint8_t code0 = draw_clip_outcode(l, u, r, d, *x0, *y0);
   uint8_t code1 = draw_clip_outcode(l, u, r, d, *x1, *y1);

   for(;;)
   {
      if(code0==0&&code1==0)
         return 1;

      if(code0 & code1)
         return 0;

      uint8_t code_out = code1>code0?code1:code0;
      int32_t x, y;
      int32_t dx = *x1 - *x0;
      int32_t dy = *y1 - *y0;

      if(code_out & 8)
      {
         x = (int32_t)(*x0 + ((int64_t)dx * (d - *y0)) / dy);
         y = d;
      }
      else if(code_out & 4)
      {
         x = (int32_t)(*x0 + ((int64_t)dx * (u - *y0)) / dy);
         y = u;
      }
      else if(code_out & 2)
      {
         x = r;
         y = (int32_t)(*y0 + ((int64_t)dy * (r - *x0)) / dx);
      }
      else
      {
         x = l;
         y = (int32_t)(*y0 + ((int64_t)dy * (l - *x0)) / dx);
      }

      if(code_out==code0)
      {
         *x0 = x;
         *y0 = y;
         code0 = draw_clip_outcode(l, u, r, d, *x0, *y0);
      }
      else
      {
         *x1 = x;
         *y1 = y;
         code1 = draw_clip_outcode(l, u, r, d, *x1, *y1);
      }
   }

   return 0;
}

static int draw_event_pen_continuous(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);
      return 1;
   }

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Clear preview
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);

      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->layer_selected,settings->palette_selected);
      project->state.x0 = mx;
      project->state.y0 = my;

      return 1;
   }
   else if(button&HLH_GUI_MOUSE_LEFT)
   {
      if(mx/256==project->state.x0/256&&my/256==project->state.y0/256)
         return 0;

      draw_line(project,settings,settings->brushes[settings->brush_selected],mx,my,project->state.x0,project->state.y0,project->layer_selected,settings->palette_selected);
      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->layer_selected,settings->palette_selected);
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->layer_selected,settings->palette_selected);
      project->state.x0 = mx;
      project->state.y0 = my;

      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT))
   {
      undo_end_layer_chunks(project);
   }
   else
   {
      if(mx/256==project->state.x0/256&&my/256==project->state.y0/256)
         return 0;
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->num_layers-1,settings->palette_selected);
      project->state.x0 = mx;
      project->state.y0 = my;
      return 1;
   }

   return 0;
}

static int draw_event_pen_discontinuous(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);
      return 1;
   }

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Clear preview
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);

      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->layer_selected,settings->palette_selected);
      project->state.x0 = mx;
      project->state.y0 = my;

      return 1;
   }
   else if(button&HLH_GUI_MOUSE_LEFT)
   {
      if(mx/256==project->state.x0/256&&my/256==project->state.y0/256)
         return 0;

      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->layer_selected,settings->palette_selected);
      project->state.x0 = mx;
      project->state.y0 = my;

      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT))
   {
      undo_end_layer_chunks(project);
   }
   else
   {
      if(mx/256==project->state.x0/256&&my/256==project->state.y0/256)
         return 0;
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->num_layers-1,settings->palette_selected);
      project->state.x0 = mx;
      project->state.y0 = my;
      return 1;
   }

   return 0;
}

static int draw_event_pen_single(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);
      return 1;
   }

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Clear preview
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);

      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->layer_selected,settings->palette_selected);
      project->state.x0 = mx;
      project->state.y0 = my;
      undo_end_layer_chunks(project);

      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT))
   {
      if(mx/256==project->state.x0/256&&my/256==project->state.y0/256)
         return 0;
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->num_layers-1,settings->palette_selected);
      project->state.x0 = mx;
      project->state.y0 = my;
      return 1;
   }

   return 0;
}

static int draw_event_pen_contour_fill(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x0/256,project->state.y0/256,project->num_layers-1,0);
      return 1;
   }

   //TODO(Captain4LK): this needs to be reworked
   //Store all lines drawn by mousemovement and rasterize based on that?
   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Clear preview
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x1/256,project->state.y1/256,project->num_layers-1,0);

      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      mx = HLH_min(project->width*256,HLH_max(mx,0));
      my = HLH_min(project->height*256,HLH_max(my,0));

      brush_place(project,settings,settings->brushes[0],mx/256,my/256,project->num_layers-1,1);
      project->state.x0 = mx;
      project->state.y0 = my;
      project->state.x1 = mx;
      project->state.y1 = my;
      project->state.minx = INT32_MAX;
      project->state.miny = INT32_MAX;
      project->state.maxx = INT32_MIN;
      project->state.maxy = INT32_MIN;

      project->state.minx = HLH_min(project->state.minx,mx/256);
      project->state.miny = HLH_min(project->state.miny,my/256);
      project->state.maxx = HLH_max(project->state.maxx,mx/256);
      project->state.maxy = HLH_max(project->state.maxy,my/256);

      return 1;
   }
   else if(button&HLH_GUI_MOUSE_LEFT)
   {
      if(mx/256==project->state.x1/256&&my/256==project->state.y1/256)
         return 0;

      mx = HLH_min(project->width*256-1,HLH_max(mx,0));
      my = HLH_min(project->height*256-1,HLH_max(my,0));

      draw_line(project,settings,settings->brushes[0],mx,my,project->state.x1,project->state.y1,project->num_layers-1,1);
      brush_place(project,settings,settings->brushes[0],mx/256,my/256,project->num_layers-1,1);
      brush_place(project,settings,settings->brushes[0],project->state.x1/256,project->state.y1/256,project->num_layers-1,1);
      project->state.x1 = mx;
      project->state.y1 = my;
      project->state.minx = HLH_min(project->state.minx,mx/256);
      project->state.miny = HLH_min(project->state.miny,my/256);
      project->state.maxx = HLH_max(project->state.maxx,mx/256);
      project->state.maxy = HLH_max(project->state.maxy,my/256);

      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT))
   {
      mx = HLH_min(project->width*256-1,HLH_max(mx,0));
      my = HLH_min(project->height*256-1,HLH_max(my,0));

      draw_line(project,settings,settings->brushes[0],mx,my,project->state.x0,project->state.y0,project->num_layers-1,1);
      brush_place(project,settings,settings->brushes[0],mx/256,my/256,project->num_layers-1,1);
      brush_place(project,settings,settings->brushes[0],project->state.x0/256,project->state.y0/256,project->num_layers-1,1);
      project->state.minx = HLH_min(project->state.minx,mx/256);
      project->state.miny = HLH_min(project->state.miny,my/256);
      project->state.maxx = HLH_max(project->state.maxx,mx/256);
      project->state.maxy = HLH_max(project->state.maxy,my/256);

      //Scan
      for(int y = project->state.miny;y<=project->state.maxy;y++)
      {
         if(y==project->state.miny||y==project->state.maxy)
         {
            for(int x = project->state.minx;x<=project->state.maxx;x++)
            {
               if(project->layers[project->num_layers-1]->data[y*project->width+x])
                  brush_place(project,settings,settings->brushes[0],x,y,project->layer_selected,settings->palette_selected);
            }

            continue;
         }

         int on = 0;
         int last = 0;
         for(int x = project->state.minx;x<=project->state.maxx;x++)
         {
            if(project->layers[project->num_layers-1]->data[y*project->width+x])
            {
               brush_place(project,settings,settings->brushes[0],x,y,project->layer_selected,settings->palette_selected);
               last = 1;
            }
            else
            {
               if(last)
               {
                  if(project->layers[project->num_layers-1]->data[(y-1)*project->width+x])
                     on = !on;
                  else
                     on = 0;
               }

               if(on)
               {
                  brush_place(project,settings,settings->brushes[0],x,y,project->layer_selected,settings->palette_selected);
                  brush_place(project,settings,settings->brushes[0],x,y,project->num_layers-1,1);
               }

               last = 0;
               //if(last)
                  //on = !on;
            }
         }
         /*int count = 0;
         for(int x = project->state.minx;x<=project->state.maxx;x++)
         {
            if(project->layers[project->num_layers-1]->data[y*project->width+x])
               count++;
         }

         int on = 0;
         int last = 0;
         for(int x = project->state.minx;x<=project->state.maxx;x++)
         {
            if(project->layers[project->num_layers-1]->data[y*project->width+x])
            {
               brush_place(project,settings,settings->brushes[0],x,y,project->layer_selected,settings->palette_selected);
               brush_place(project,settings,settings->brushes[0],x,y,project->num_layers-1,0);
               //on = 1;
               //if(!last)
                  //on = !on;
               last = 1;
               count--;
            }
            else
            {
               if(last)
                  on = !on;
               if(on)
                  brush_place(project,settings,settings->brushes[0],x,y,project->layer_selected,settings->palette_selected);
               last = 0;
               //if(on&&count>0)
                  //brush_place(project,settings,settings->brushes[0],x,y,project->layer_selected,settings->palette_selected);
               //else
                  //on = 0;
               //last = 0;
            }
         }*/
      }

      //Clear preview

      undo_end_layer_chunks(project);

      for(int y = project->state.miny;y<=project->state.maxy;y++)
      {
         for(int x = project->state.minx;x<=project->state.maxx;x++)
         {
            if(project->layers[project->num_layers-1]->data[y*project->width+x])
               brush_place(project,settings,settings->brushes[0],x,y,project->num_layers-1,0);
         }
      }

      return 1;
   }
   else
   {
      if(mx/256==project->state.x1/256&&my/256==project->state.y1/256)
         return 0;
      brush_place(project,settings,settings->brushes[settings->brush_selected],project->state.x1/256,project->state.y1/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[settings->brush_selected],mx/256,my/256,project->num_layers-1,settings->palette_selected);
      project->state.x1 = mx;
      project->state.y1 = my;
      return 1;
   }

   return 0;
}

static int draw_event_line_single(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      return 1;
   }

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      project->state.x0 = mx;
      project->state.y0 = my;
      project->state.x1 = mx;
      project->state.y1 = my;

      return 1;
   }
   else if(button&HLH_GUI_MOUSE_LEFT)
   {
      if(mx==project->state.x0&&my==project->state.y0)
         return 0;

      //Erease old
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);

      project->state.x1 = mx;
      project->state.y1 = my;
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,settings->palette_selected);


      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT))
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[settings->brush_selected],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->layer_selected,settings->palette_selected);
      undo_end_layer_chunks(project);

      return 1;
   }

   return 0;
}

static int draw_event_line_connect(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      return 1;
   }

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT)&&!project->state.active)
   {
      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      project->state.x0 = mx;
      project->state.y0 = my;
      project->state.x1 = mx;
      project->state.y1 = my;
      project->state.active = 1;

      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT))
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[settings->brush_selected],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->layer_selected,settings->palette_selected);

      project->state.x0 = mx;
      project->state.y0 = my;

      return 1;
   }
   else if(button& HLH_GUI_MOUSE_RIGHT&&!(old_state&HLH_GUI_MOUSE_RIGHT))
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      project->state.active = 0;
      undo_end_layer_chunks(project);

      return 1;
   }
   else if(project->state.active)
   {
      if(mx==project->state.x0&&my==project->state.y0)
         return 0;

      //Erease old
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);

      project->state.x1 = mx;
      project->state.y1 = my;
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,settings->palette_selected);


      return 1;
   }

   return 0;
}

static int draw_event_line_fan(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      return 1;
   }

   if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT)&&!project->state.active)
   {
      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      project->state.x0 = mx;
      project->state.y0 = my;
      project->state.x1 = mx;
      project->state.y1 = my;
      project->state.active = 1;
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,settings->palette_selected);

      return 1;
   }
   else if(button&HLH_GUI_MOUSE_LEFT&&project->state.active)
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      project->state.x1 = mx;
      project->state.y1 = my;
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,settings->palette_selected);
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->layer_selected,settings->palette_selected);

      return 1;
   }
   else if(button& HLH_GUI_MOUSE_RIGHT&&!(old_state&HLH_GUI_MOUSE_RIGHT))
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      project->state.active = 0;
      undo_end_layer_chunks(project);

      return 1;
   }
   else if(project->state.active)
   {
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,0);
      project->state.x1 = mx;
      project->state.y1 = my;
      draw_line(project,settings,settings->brushes[0],project->state.x0,project->state.y0,project->state.x1,project->state.y1,project->num_layers-1,settings->palette_selected);

      return 1;
   }

   return 0;
}

static int draw_event_flood_fill(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      if(mx<0||my<0)
         return 0;
      if(mx/256>=project->width||my/256>=project->height)
         return 0;

      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      HLH_bitmap_clear(project->bitmap);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      uint8_t flood = project->layers[project->layer_selected]->data[(my/256)*project->width+mx/256];
      int32_t *todo = NULL;
      HLH_array_push(todo,mx/256);
      HLH_array_push(todo,my/256);
      HLH_bitmap_set(project->bitmap,(my/256)*project->width+mx/256);
      while(HLH_array_length(todo))
      {
         int32_t x,y;
         y = todo[HLH_array_length(todo)-1];
         x = todo[HLH_array_length(todo)-2];
         HLH_array_length_set(todo,HLH_array_length(todo)-2);
         brush_place(project,settings,settings->brushes[0],x,y,project->layer_selected,settings->palette_selected);

         if(x>0&&project->layers[project->layer_selected]->data[y*project->width+x-1]==flood&&
            !HLH_bitmap_check(project->bitmap,y*project->width+x-1))
         {
            HLH_array_push(todo,x-1);
            HLH_array_push(todo,y);
            HLH_bitmap_set(project->bitmap,y*project->width+x-1);
         }
         if(y>0&&project->layers[project->layer_selected]->data[(y-1)*project->width+x]==flood&&
            !HLH_bitmap_check(project->bitmap,(y-1)*project->width+x))
         {
            HLH_array_push(todo,x);
            HLH_array_push(todo,y-1);
            HLH_bitmap_set(project->bitmap,(y-1)*project->width+x);
         }
         if(x<project->width-1&&project->layers[project->layer_selected]->data[y*project->width+x+1]==flood&&
            !HLH_bitmap_check(project->bitmap,y*project->width+x+1))
         {
            HLH_array_push(todo,x+1);
            HLH_array_push(todo,y);
            HLH_bitmap_set(project->bitmap,y*project->width+x+1);
         }
         if(y<project->height-1&&project->layers[project->layer_selected]->data[(y+1)*project->width+x]==flood&&
            !HLH_bitmap_check(project->bitmap,(y+1)*project->width+x))
         {
            HLH_array_push(todo,x);
            HLH_array_push(todo,y+1);
            HLH_bitmap_set(project->bitmap,(y+1)*project->width+x);
         }
      }
      HLH_array_free(todo);

      undo_end_layer_chunks(project);

      return 1;
   }

   return 0;
}

static int draw_event_flood_swap(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      if(mx<0||my<0)
         return 0;
      if(mx/256>=project->width||my/256>=project->height)
         return 0;

      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      //HLH_bitmap_clear(project->bitmap);
      undo_begin_layer_chunks(project);

      uint8_t selected = project->layers[project->layer_selected]->data[(my/256)*project->width+mx/256];
      for(int y = 0;y<project->height;y++)
      {
         for(int x = 0;x<project->width;x++)
         {
            if(project->layers[project->layer_selected]->data[y*project->width+x]==selected)
            {
               brush_place(project,settings,settings->brushes[0],x,y,project->layer_selected,settings->palette_selected);
            }
         }
      }

      undo_end_layer_chunks(project);

      return 1;
   }

   return 0;
}

static int draw_event_rect_outline(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      return 1;
   }

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      project->state.x0 = (mx/256)*256+128;
      project->state.y0 = (my/256)*256+128;
      project->state.x1 = (mx/256)*256+128;
      project->state.y1 = (my/256)*256+128;

      return 1;
   }
   else if(button&HLH_GUI_MOUSE_LEFT)
   {
      if(mx==project->state.x0&&my==project->state.y0)
         return 0;

      //Erease old
      int x0 = project->state.x0;
      int y0 = project->state.y0;
      int x1 = project->state.x1;
      int y1 = project->state.y1;
      draw_line(project,settings,settings->brushes[0],x0,0,x0,project->height*256+128,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],x1,0,x1,project->height*256+128,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],0,y0,project->width*256+128,y0,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],0,y1,project->width*256+128,y1,project->num_layers-1,0);

      //Preview
      project->state.x1 = (mx/256)*256+128;
      project->state.y1 = (my/256)*256+128;
      x0 = project->state.x0;
      y0 = project->state.y0;
      x1 = project->state.x1;
      y1 = project->state.y1;

      draw_line(project,settings,settings->brushes[0],x0,0,x0,project->height*256+128,project->num_layers-1,settings->color_white);
      draw_line(project,settings,settings->brushes[0],x1,0,x1,project->height*256+128,project->num_layers-1,settings->color_white);
      draw_line(project,settings,settings->brushes[0],0,y0,project->width*256+128,y0,project->num_layers-1,settings->color_white);
      draw_line(project,settings,settings->brushes[0],0,y1,project->width*256+128,y1,project->num_layers-1,settings->color_white);
      brush_place(project,settings,settings->brushes[0],x0/256,y0/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[0],x1/256,y0/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[0],x0/256,y1/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[0],x1/256,y1/256,project->num_layers-1,0);

      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Erease preview
      int x0 = project->state.x0;
      int y0 = project->state.y0;
      int x1 = project->state.x1;
      int y1 = project->state.y1;
      draw_line(project,settings,settings->brushes[0],x0,0,x0,project->height*256+128,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],x1,0,x1,project->height*256+128,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],0,y0,project->width*256+128,y0,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],0,y1,project->width*256+128,y1,project->num_layers-1,0);

      //Draw rect
      draw_line(project,settings,settings->brushes[settings->brush_selected],x0,y0,x1,y0,project->layer_selected,settings->palette_selected);
      draw_line(project,settings,settings->brushes[settings->brush_selected],x0,y1,x1,y1,project->layer_selected,settings->palette_selected);
      draw_line(project,settings,settings->brushes[settings->brush_selected],x0,y0,x0,y1,project->layer_selected,settings->palette_selected);
      draw_line(project,settings,settings->brushes[settings->brush_selected],x1,y0,x1,y1,project->layer_selected,settings->palette_selected);

      undo_end_layer_chunks(project);

      return 1;
   }

   return 0;
}

static int draw_event_rect_fill(Project *project, int32_t mx, int32_t my, uint8_t button, const Settings *settings, int end)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   if(end)
   {
      return 1;
   }

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[project->layer_selected],sizeof(*project->old->data)*project->width*project->height);

      project->state.x0 = (mx/256)*256+128;
      project->state.y0 = (my/256)*256+128;
      project->state.x1 = (mx/256)*256+128;
      project->state.y1 = (my/256)*256+128;

      return 1;
   }
   else if(button&HLH_GUI_MOUSE_LEFT)
   {
      if(mx==project->state.x0&&my==project->state.y0)
         return 0;

      //Erease old
      int x0 = project->state.x0;
      int y0 = project->state.y0;
      int x1 = project->state.x1;
      int y1 = project->state.y1;
      draw_line(project,settings,settings->brushes[0],x0,0,x0,project->height*256+128,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],x1,0,x1,project->height*256+128,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],0,y0,project->width*256+128,y0,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],0,y1,project->width*256+128,y1,project->num_layers-1,0);

      //Preview
      project->state.x1 = (mx/256)*256+128;
      project->state.y1 = (my/256)*256+128;
      x0 = project->state.x0;
      y0 = project->state.y0;
      x1 = project->state.x1;
      y1 = project->state.y1;

      draw_line(project,settings,settings->brushes[0],x0,0,x0,project->height*256+128,project->num_layers-1,settings->color_white);
      draw_line(project,settings,settings->brushes[0],x1,0,x1,project->height*256+128,project->num_layers-1,settings->color_white);
      draw_line(project,settings,settings->brushes[0],0,y0,project->width*256+128,y0,project->num_layers-1,settings->color_white);
      draw_line(project,settings,settings->brushes[0],0,y1,project->width*256+128,y1,project->num_layers-1,settings->color_white);
      brush_place(project,settings,settings->brushes[0],x0/256,y0/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[0],x1/256,y0/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[0],x0/256,y1/256,project->num_layers-1,0);
      brush_place(project,settings,settings->brushes[0],x1/256,y1/256,project->num_layers-1,0);

      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Erease preview
      int x0 = project->state.x0;
      int y0 = project->state.y0;
      int x1 = project->state.x1;
      int y1 = project->state.y1;
      draw_line(project,settings,settings->brushes[0],x0,0,x0,project->height*256+128,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],x1,0,x1,project->height*256+128,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],0,y0,project->width*256+128,y0,project->num_layers-1,0);
      draw_line(project,settings,settings->brushes[0],0,y1,project->width*256+128,y1,project->num_layers-1,0);

      //Fill rect
      int width = HLH_abs(x1/256-x0/256)+1;
      int height = HLH_abs(y1/256-y0/256)+1;
      Brush *brush = malloc(sizeof(*brush)+width*height);
      brush->width = width;
      brush->height = height;
      memset(brush->data,1,brush->width*brush->height);
      brush_place(project,settings,brush,HLH_min(x1,x0)/256+brush->width/2,HLH_min(y1,y0)/256+brush->height/2,project->layer_selected,settings->palette_selected);
      free(brush);

      undo_end_layer_chunks(project);

      return 1;
   }

   return 0;
}
//-------------------------------------
