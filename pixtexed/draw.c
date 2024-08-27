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
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void draw_line(Project *project, int layer, int32_t x0, int32_t y0, int32_t x1, int32_t y1);

static uint8_t draw_clip_outcode(int32_t l, int32_t u, int32_t r, int32_t d, int32_t x, int32_t y);
static int draw_clip_line(int32_t l, int32_t u, int32_t r, int32_t d, int32_t *x0, int32_t *y0, int32_t *x1, int32_t *y1);
//-------------------------------------

//Function implementations

int draw_event(Project *project, int32_t mx, int32_t my, uint8_t button)
{
   uint8_t old_state = project->state.button;
   project->state.button = button;

   //Start new drawing operation
   if((button&HLH_GUI_MOUSE_LEFT)&&!(old_state&HLH_GUI_MOUSE_LEFT))
   {
      //Prepare undo
      HLH_bitmap_clear(project->undo_map);
      undo_begin_layer_chunks(project);

      layer_copy(project->old,project->layers[0],sizeof(*project->old->data)*project->width*project->height);

      brush_place(project,mx/256,my/256,0);
      project->state.last_x = mx;
      project->state.last_y = my;

      return 1;
   }
   else if(button&HLH_GUI_MOUSE_LEFT)
   {
      if(mx/256==project->state.last_x/256&&my/256==project->state.last_y/256)
         return 0;

      //draw_line(project,0,project->state.last_x,project->state.last_y,mx,my);
      draw_line(project,0,mx,my,project->state.last_x,project->state.last_y);
      brush_place(project,mx/256,my/256,0);
      brush_place(project,project->state.last_x/256,project->state.last_y/256,0);
      //project->layers[0]->data[(my/256)*project->width+mx/256] = 1;
      project->state.last_x = mx;
      project->state.last_y = my;

      return 1;
   }
   else if(!(button&HLH_GUI_MOUSE_LEFT)&&(old_state&HLH_GUI_MOUSE_LEFT))
   {
      undo_end_layer_chunks(project);
   }

   return 0;
}

static void draw_line(Project *project, int layer, int32_t x0, int32_t y0, int32_t x1, int32_t y1)
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
      int32_t left = abs(y1/256-y0/256);

      while(left--)
      {
         if(dist>dy / 2)
         {
            bx+=step_x;
            dist -= dy;
         }

         brush_place(project,bx,by,0);
         by+=step_y;
         dist += dx;
      }
   }
   else
   {
      int32_t dist = (int32_t)(((int64_t)(frac_y - 128) * dx - (int64_t)(frac_x - 128) * dy) / 256);
      int32_t left = abs(x1/256-x0/256);

      while(left--)
      {
         if(dist>dx / 2)
         {
            by+=step_y;
            dist -= dx;
         }

         brush_place(project,bx,by,0);
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
//-------------------------------------
