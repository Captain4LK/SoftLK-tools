/*
HLH_gui - gui framework

Written in 2023,2024,2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
static int64_t image_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp);
static void image_draw(HLH_gui_image *img);
//-------------------------------------

//Function implementations

HLH_gui_image *HLH_gui_img_create_path(HLH_gui_element *parent, HLH_gui_flags flags, const char *path)
{
   HLH_gui_image *img = (HLH_gui_image *) HLH_gui_element_create(sizeof(*img), parent, flags, image_msg);
   img->e.type = HLH_GUI_IMAGE;

   img->img = HLH_gui_texture_load(img->e.window, path, &img->width, &img->height);

   return img;
}

HLH_gui_image *HLH_gui_img_create_data(HLH_gui_element *parent, HLH_gui_flags flags, uint32_t *pix, int width, int height)
{
   HLH_gui_image *img = (HLH_gui_image *) HLH_gui_element_create(sizeof(*img), parent, flags, image_msg);
   img->e.type = HLH_GUI_IMAGE;

   img->width = width;
   img->height = height;
   img->img = HLH_gui_texture_from_data(img->e.window, pix, width, height);

   return img;
}

void HLH_gui_img_update(HLH_gui_image *img, uint32_t *pix, int width, int height, int redraw)
{
   if(img->img!=NULL)
      SDL_DestroyTexture(img->img);

   img->width = width;
   img->height = height;
   img->img = HLH_gui_texture_from_data(img->e.window, pix, width, height);

   if(redraw)
      HLH_gui_element_redraw(&img->e);
}

static int64_t image_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   HLH_gui_image *img = (HLH_gui_image *)e;

   if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return img->width + 6 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return img->height + 6 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      image_draw(img);
   }
   else if(msg==HLH_GUI_MSG_DESTROY)
   {
      if(img->img!=NULL)
         SDL_DestroyTexture(img->img);
   }

   return 0;
}

static void image_draw(HLH_gui_image *img)
{
   HLH_gui_rect bounds = img->e.bounds;
   int scale = HLH_gui_get_scale();

   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.min[1] + 2 * scale, bounds.min[0] + 2 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_dark);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(bounds.min[0] + 1 * scale, bounds.max[1] - 2 * scale, bounds.max[0] - 2 * scale, bounds.max[1] - 1 * scale), HLH_gui_theme_current.bevel_dark);

   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(bounds.max[0] - 2 * scale, bounds.min[1] + 2 * scale, bounds.max[0] - 1 * scale, bounds.max[1] - 2 * scale), HLH_gui_theme_current.bevel_light);
   HLH_gui_draw_rectangle_fill(&img->e, HLH_gui_rect_make(bounds.min[0] + 2 * scale, bounds.min[1] + 1 * scale, bounds.max[0] - 1 * scale, bounds.min[1] + 2 * scale), HLH_gui_theme_current.bevel_light);

   int view_x;
   int view_y;
   int view_width;
   int view_height;
   int width = bounds.max[0] - bounds.min[0] - scale * 6;
   int height = bounds.max[1] - bounds.min[1] - scale * 6;

   if(width * img->height>img->width * height)
   {
      view_height = height;
      view_width = (img->width * height) / img->height;
   }
   else
   {
      view_width = width;
      view_height = (img->height * width) / img->width;
   }

   view_x = (width - view_width) / 2 + bounds.min[0] + 3 * scale;
   view_y = (height - view_height) / 2 + bounds.min[1] + 3 * scale;

   SDL_FRect dst = {0};
   dst.x = view_x;
   dst.y = view_y;
   dst.w = view_width;
   dst.h = view_height;
   SDL_RenderTexture(img->e.window->sdl_renderer, img->img, NULL, &dst);
}
//-------------------------------------
