/*
HLH_gui - gui framework

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
//-------------------------------------

//Function prototypes
static int img_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_image *HLH_gui_image_create(HLH_gui_element *parent, uint32_t flags, int width, int height, uint32_t *data)
{
   HLH_gui_image *img = (HLH_gui_image *) HLH_gui_element_create(sizeof(*img),parent,flags,img_msg);
   img->width = width;
   img->height = height;
   img->texture = SDL_CreateTexture(parent->window->renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,width,height);
   SDL_SetTextureBlendMode(img->texture,SDL_BLENDMODE_BLEND);
   void *pixels;
   int pitch;
   SDL_LockTexture(img->texture,NULL,&pixels,&pitch);
   memcpy(pixels,data,sizeof(*data)*width*height);
   SDL_UnlockTexture(img->texture);

   return img;
}

void HLH_gui_image_update(HLH_gui_image *img, int width, int height, uint32_t *data)
{
   if(img->width!=width||img->height!=height)
   {
      img->width = width;
      img->height = height;
      SDL_DestroyTexture(img->texture);
      img->texture = SDL_CreateTexture(img->e.parent->window->renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,width,height);
      SDL_SetTextureBlendMode(img->texture,SDL_BLENDMODE_BLEND);
   }

   void *pixels;
   int pitch;
   SDL_LockTexture(img->texture,NULL,&pixels,&pitch);
   memcpy(pixels,data,sizeof(*data)*width*height);
   SDL_UnlockTexture(img->texture);

   HLH_gui_element_msg(&img->e,HLH_GUI_MSG_LAYOUT,0,NULL);
   HLH_gui_element_repaint(&img->e,NULL);
}

static int img_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_image *img = (HLH_gui_image *) e;
   HLH_gui_painter *p = dp;
   float aspect = (float)img->width/(float)img->height;

   if(msg==HLH_GUI_MSG_DESTROY)
   {
      SDL_DestroyTexture(img->texture);
      //free(img->data);
   }
   else if(msg==HLH_GUI_MSG_PAINT)
   {
      int view_x,view_y;
      int view_width,view_height;
      int width = e->bounds.r-e->bounds.l;
      int height = e->bounds.b-e->bounds.t;

      if(width*img->height>img->width*height)
      {
         view_height = height;
         view_width = (img->width*height)/img->height;
      }
      else
      {
         view_width = width;
         view_height = (img->height*width)/img->width;
      }

      view_x = (width-view_width)/2+e->bounds.l;
      view_y = (height-view_height)/2+e->bounds.t;

      SDL_Rect clip;
      clip.x = p->clip.l;
      clip.y = p->clip.t;
      clip.w = p->clip.r-p->clip.l;
      clip.h = p->clip.b-p->clip.t;
      SDL_RenderSetClipRect(p->win->renderer,&clip);

      SDL_Rect dst;
      dst.x = view_x;
      dst.y = view_y;
      dst.w = view_width;
      dst.h = view_height;
      SDL_RenderCopy(p->win->renderer,img->texture,NULL,&dst);

      SDL_RenderSetClipRect(p->win->renderer,NULL);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      if(di)
         return (int)((float)di/aspect);
      return img->width;
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      if(di)
         return (int)((float)di*aspect);
      return img->height;
   }

   return 0;
}
//-------------------------------------
