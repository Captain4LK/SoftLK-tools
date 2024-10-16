/*
HLH_gui - gui framework

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

//External includes
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
#include "HLH_gui_internal.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static int imgcmp_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void imgcmp_draw(HLH_gui_imgcmp *img);
//-------------------------------------

//Function implementations

HLH_gui_imgcmp *HLH_gui_imgcmp_create(HLH_gui_element *parent, uint64_t flags, uint32_t *pix0, int width0, int height0, uint32_t *pix1, int width1, int height1)
{
   HLH_gui_imgcmp *img = (HLH_gui_imgcmp *) HLH_gui_element_create(sizeof(*img), parent, flags, imgcmp_msg);
   img->e.type = HLH_GUI_IMGCMP;

   img->slider = 1024;
   img->width0 = width0;
   img->height0 = height0;
   img->width1 = width1;
   img->height1 = height1;
   img->img0 = HLH_gui_texture_from_data(img->e.window, pix0, width0, height0);
   img->img1 = HLH_gui_texture_from_data(img->e.window, pix1, width1, height1);

   return img;
}
void HLH_gui_imgcmp_update0(HLH_gui_imgcmp *img, uint32_t *pix, int width, int height, int redraw)
{
   if(img->img0!=NULL)
      SDL_DestroyTexture(img->img0);

   img->width0 = width;
   img->height0 = height;
   img->img0 = HLH_gui_texture_from_data(img->e.window, pix, width, height);

   if(redraw)
      HLH_gui_element_redraw(&img->e);
}

void HLH_gui_imgcmp_update1(HLH_gui_imgcmp *img, uint32_t *pix, int width, int height, int redraw)
{
   if(img->img1!=NULL)
      SDL_DestroyTexture(img->img1);

   img->width1 = width;
   img->height1 = height;
   img->img1 = HLH_gui_texture_from_data(img->e.window, pix, width, height);

   if(redraw)
      HLH_gui_element_redraw(&img->e);
}

static int imgcmp_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_imgcmp *img = (HLH_gui_imgcmp *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return img->width0 + 6 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return img->height0 + 6 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_MOUSE)
   {
      HLH_gui_mouse *m = dp;

      if(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
      {
         HLH_gui_rect bounds = img->e.bounds;
         int scale = HLH_gui_get_scale();

         /*int view_x;
         int view_width;
         int width = bounds.maxx - bounds.minx - scale * 6;
         int height = bounds.maxy - bounds.miny - scale * 6;

         if(width * img->height0>img->width0 * height)
            view_width = (img->width0 * height) / img->height0;
         else
            view_width = width;

         view_x = (width - view_width) / 2 + bounds.minx + 3 * scale;*/

         int mx = m->pos.x - (bounds.minx+3*scale);
         int value = (mx * 2048) / (bounds.maxx-bounds.minx-6*scale);
         if(value<0) value = 0;
         if(value>2048) value = 2048;

         if(img->slider!=value)
         {
            img->slider = value;
            HLH_gui_element_redraw(&img->e);
         }

         return 1;
      }
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      imgcmp_draw(img);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      if(img->img0!=NULL)
         SDL_DestroyTexture(img->img0);
      if(img->img1!=NULL)
         SDL_DestroyTexture(img->img1);
   }

   return 0;
}

static void imgcmp_draw(HLH_gui_imgcmp *img)
{
   HLH_gui_rect bounds = img->e.bounds;
   int scale = HLH_gui_get_scale();

   HLH_gui_draw_rectangle_fill(&img->e, bounds, 0xff5a5a5a);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 2 * scale), 0xff323232);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xff323232);

   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xffc8c8c8);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xffc8c8c8);

   //int clip_width = ((bounds.maxx-bounds.minx)*img->slider)/2048;
   int view_x;
   int view_y;
   int view_width;
   int view_height;
   int width = bounds.maxx - bounds.minx - scale * 6;
   int height = bounds.maxy - bounds.miny - scale * 6;

   //Img1
   if(width * img->height0>img->width0 * height)
   {
      view_height = height;
      view_width = (img->width0 * height) / img->height0;
   }
   else
   {
      view_width = width;
      view_height = (img->height0 * width) / img->width0;
   }

   int middle = ((bounds.maxx-bounds.minx-6*scale)* img->slider) / 2048 + bounds.minx+3*scale;
   SDL_Rect clip = {0};
   SDL_Rect dst = {0};
   view_x = (width - view_width) / 2 + bounds.minx + 3 * scale;
   view_y = (height - view_height) / 2 + bounds.miny + 3 * scale;
   dst.x = view_x;
   dst.y = view_y;
   dst.w = view_width;
   dst.h = view_height;
   clip.x = view_x;
   clip.y = view_y;
   clip.w = middle-view_x;
   if(clip.w<=0) clip.w = 1;
   clip.h = view_height;
   SDL_RenderSetClipRect(img->e.window->renderer, &clip);
   SDL_RenderCopy(img->e.window->renderer, img->img0, NULL, &dst);
   SDL_RenderSetClipRect(img->e.window->renderer, NULL);

   //Img0
   if(width * img->height1>img->width1 * height)
   {
      view_height = height;
      view_width = (img->width1 * height) / img->height1;
   }
   else
   {
      view_width = width;
      view_height = (img->height1 * width) / img->width1;
   }

   view_x = (width - view_width) / 2 + bounds.minx + 3 * scale;
   view_y = (height - view_height) / 2 + bounds.miny + 3 * scale;

   clip.x = middle;
   clip.y = view_y;
   clip.w = bounds.maxx-bounds.minx;
   if(clip.w<=0)
      clip.w = 1;
   clip.h = view_height;
   dst.x = view_x;
   dst.y = view_y;
   dst.w = view_width;
   dst.h = view_height;
   SDL_RenderSetClipRect(img->e.window->renderer, &clip);
   SDL_RenderCopy(img->e.window->renderer, img->img1, NULL, &dst);
   SDL_RenderSetClipRect(img->e.window->renderer, NULL);

   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(middle - scale, view_y, middle + scale, view_y + view_height), 0xff5a5a5a);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(middle - 2 * scale, view_y, middle - scale, view_y + view_height), 0xff323232);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(middle + scale, view_y, middle + 2 * scale, view_y + view_height), 0xffc8c8c8);
}
//-------------------------------------
