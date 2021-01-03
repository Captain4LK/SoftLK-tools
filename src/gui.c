/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <SLK/SLK.h>
#include "../external/tinyfiledialogs.h"
//-------------------------------------

//Internal includes
#include "assets.h"
#include "gui.h"
#include "process.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static int gui_pos_x = 0;
static int gui_pos_y = 0;
static SLK_RGB_sprite *sprite_in = NULL;
static SLK_RGB_sprite *sprite_out = NULL;
static SLK_Palette *palette = NULL;
static int gui_out_width = 64;
static int gui_out_height = 64;
static int pixel_sample_mode = 0;
static int pixel_process_mode = 0;
//-------------------------------------

//Function prototypes
static void gui_buttons();
static void gui_draw();
static void update_output();
static SLK_Color sample_pixel(float x, float y);
//-------------------------------------

//Function implementations

void gui_init()
{
   //Force background redraw by changing size
   SLK_layer_set_size(5,800,600);

   SLK_layer_set_size(0,150,400);
}

void gui_update()
{
   gui_buttons();

   gui_draw();
}

static void gui_buttons()
{
   int x,y;
   SLK_mouse_get_layer_pos(0,&x,&y);

   if(SLK_mouse_pressed(SLK_BUTTON_LEFT))
   {
      if(x>3&&x<64&&y>373&&y<394) //Load button
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_openFileDialog("Select a png file","",1,filter_patterns,NULL,0);
         SLK_rgb_sprite_destroy(sprite_in);
         sprite_in = SLK_rgb_sprite_load(file_path);
         update_output();
      }
      else if(x>81&&x<144&&y>373&&y<394) //Save button
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_saveFileDialog("Save image","",1,filter_patterns,NULL);
         SLK_rgb_sprite_save(file_path,sprite_out);
      }
      else if(x>3&&x<144&&y>335&&y<356) //Palette load button
      {
         const char *filter_patterns[2] = {"*.pal"};
         const char *file_path = tinyfd_openFileDialog("Load a palette","",1,filter_patterns,NULL,0);
         if(palette)
            free(palette);
         palette = SLK_palette_load(file_path);
      }
   }
}

static void gui_draw()
{
   if(SLK_layer_get_resized(3))
   {
      SLK_layer_set_current(3);
      SLK_draw_rgb_set_changed(1);
      SLK_draw_rgb_set_clear_color(SLK_color_create(201,212,253,255));
      SLK_draw_rgb_clear();
      int w,h;
      SLK_layer_get_size(3,&w,&h);
      for(int i = 0;i<(w/196)+1;i++)
         SLK_draw_rgb_sprite(sprite_backdrop_0,i*196,h-27);

      //Update layer pos
      gui_pos_x = w/16;
      gui_pos_y = (h-400)/2;
      SLK_layer_set_pos(0,gui_pos_x,gui_pos_y);
   }

   SLK_layer_set_current(0);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,212,253,255));
   SLK_draw_rgb_clear();
   SLK_draw_rgb_sprite(sprite_gui_0,0,0);
}

static void update_output()
{
   if(sprite_out==NULL||sprite_out->width!=gui_out_width||sprite_out->height!=gui_out_height)
   {
      SLK_rgb_sprite_destroy(sprite_out);
      sprite_out = SLK_rgb_sprite_create(gui_out_width,gui_out_height);
   }

   if(sprite_in==NULL)
      return;

   //Process image
   int out_width = sprite_out->width;
   int out_height = sprite_out->height;
   for(int y = 0;y<gui_out_height;y++)
   {
      for(int x = 0;x<gui_out_width;x++)
      {
         SLK_Color in = sample_pixel((float)x/(float)out_width,(float)y/(float)out_height);          
         SLK_Color out = process_pixel(x,y,pixel_process_mode,in,palette);
         SLK_rgb_sprite_set_pixel(sprite_out,x,y,out);
      }
   }
}

static SLK_Color sample_pixel(float x, float y)
{
   SLK_Color out = SLK_color_create(0,0,0,255);
   switch(pixel_sample_mode)
   {
   case 0: //default
      {
         double sx = x*sprite_in->width;   
         double sy = y*sprite_in->height;   
         out = SLK_rgb_sprite_get_pixel(sprite_in,round(sx),round(sy));
      }
      break;
   }

   return out;
}
//-------------------------------------
