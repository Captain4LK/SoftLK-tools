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

static void imgcmp_update_view(HLH_gui_imgcmp *img, int reset);
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

   imgcmp_update_view(img,1);

   return img;
}
void HLH_gui_imgcmp_update0(HLH_gui_imgcmp *img, uint32_t *pix, int width, int height, int redraw)
{
   if(img->img0!=NULL)
      SDL_DestroyTexture(img->img0);

   img->width0 = width;
   img->height0 = height;
   img->img0 = HLH_gui_texture_from_data(img->e.window, pix, width, height);

   imgcmp_update_view(img,1);

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

   imgcmp_update_view(img,0);

   if(redraw)
      HLH_gui_element_redraw(&img->e);
}

static void imgcmp_update_view(HLH_gui_imgcmp *img, int reset)
{
   HLH_gui_rect bounds = img->e.bounds;
   int scale = HLH_gui_get_scale();
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

   view_x = (width - view_width) / 2;
   view_y = (height - view_height) / 2;

   if(reset)
   {
      img->x = (float)view_x;
      img->y = (float)view_y;
      img->scale = (float)view_width/(float)img->width0;
   }

   //Img1
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

   float sc0 = (float)view_width/(float)img->width0;
   img->scale1 = ((float)view_width/(float)img->width1)/sc0;
   img->ox = (float)(width - view_width)/2 - (float)view_x;
   img->oy = (float)(height - view_height)/2 - (float)view_y;
   img->ox /= sc0;
   img->oy /= sc0;
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
      int redraw = 0;

      if(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT))
      {
         HLH_gui_rect bounds = img->e.bounds;
         int scale = HLH_gui_get_scale();

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

      if(m->wheel>0&&img->scale<=64.f)
      {
         float mx = (float)(m->pos.x-img->e.bounds.minx);
         float my = (float)(m->pos.y-img->e.bounds.miny);
         float x = (mx-img->x)/img->scale;
         float y = (my-img->y)/img->scale;
         float scale_change = -img->scale*0.15f;
         img->x+=x*scale_change;
         img->y+=y*scale_change;
         img->scale+=img->scale*0.15f;
         redraw = 1;
      }
      else if(m->wheel<0&&img->scale>=0.1f)
      {
         float mx = (float)(m->pos.x-img->e.bounds.minx);
         float my = (float)(m->pos.y-img->e.bounds.miny);
         float x = (mx-img->x)/img->scale;
         float y = (my-img->y)/img->scale;
         float scale_change = img->scale*0.15f;
         img->x+=x*scale_change;
         img->y+=y*scale_change;
         img->scale-=img->scale*0.15f;
         redraw = 1;
      }

      if(m->button&HLH_GUI_MOUSE_MIDDLE&&(m->rel.x!=0||m->rel.y!=0))
      {
         img->x+=(float)m->rel.x;
         img->y+=(float)m->rel.y;
         HLH_gui_element_redraw(&img->e);

         return 1;
      }

      if(redraw)
      {
         HLH_gui_element_redraw(&img->e);
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
   else if(msg==HLH_GUI_MSG_BUTTON_DOWN)
   {
      int scancode = di;

      if(scancode==SDL_SCANCODE_BACKSPACE)
      {
         imgcmp_update_view(img,1);
         HLH_gui_element_redraw(&img->e);
      }
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

   int middle = ((bounds.maxx-bounds.minx-6*scale)* img->slider) / 2048 + bounds.minx+3*scale;
   SDL_Rect clip = {0};
   SDL_Rect dst = {0};
   dst.x = (int)img->x + bounds.minx;
   dst.y = (int)img->y + bounds.miny;
   dst.w = (int)((float)img->width0 * img->scale);
   dst.h = (int)((float)img->height0 * img->scale);
   clip.x = bounds.minx + 3 * scale;
   clip.y = bounds.miny + 3 * scale;
   clip.w = middle-(bounds.minx + 6 * scale);
   if(clip.w<=0) clip.w = 1;
   clip.h = bounds.maxy - bounds.miny - 6 * scale;
   SDL_RenderSetClipRect(img->e.window->renderer, &clip);
   SDL_RenderCopy(img->e.window->renderer, img->img0, NULL, &dst);
   SDL_RenderSetClipRect(img->e.window->renderer, NULL);

   clip.x = middle;
   clip.y = bounds.miny + 3 * scale;
   clip.w = bounds.maxx- middle - 3 * scale;
   if(clip.w<=0)
      clip.w = 1;
   clip.h = bounds.maxy - bounds.miny - 6 * scale;
   dst.x = (int)(img->x + img->ox * img->scale) + bounds.minx;
   dst.y = (int)(img->y + img->oy * img->scale) + bounds.miny;
   dst.w = (int)((float)img->width1 * img->scale1 * img->scale);
   dst.h = (int)((float)img->height1 * img->scale1 * img->scale);
   SDL_RenderSetClipRect(img->e.window->renderer, &clip);
   SDL_RenderCopy(img->e.window->renderer, img->img1, NULL, &dst);
   SDL_RenderSetClipRect(img->e.window->renderer, NULL);

   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(middle - scale, bounds.miny + 3 * scale, middle + scale, bounds.maxy - 3 * scale), 0xff5a5a5a);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(middle - 2 * scale, bounds.miny + 3 * scale, middle - scale, bounds.maxy - 3 * scale), 0xff323232);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(middle + scale, bounds.miny + 3 * scale, middle + 2 * scale, bounds.maxy - 3 * scale), 0xffc8c8c8);
}
//-------------------------------------
