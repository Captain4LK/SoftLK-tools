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
#include "sample.h"
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
static int gui_out_width = 128;
static int gui_out_height = 128;
static int pixel_sample_mode = 0;
static int pixel_process_mode = 1;
static const char *text_dither[] = 
{
   "No dithering",
   "Ordered 1",
   "Ordered 2",
   "Ordered 3",
   "FloydSteinberg 1",
   "FloydSteinberg 2",
};
//-------------------------------------

//Function prototypes
static void gui_buttons();
static void gui_draw();
static void update_output();
//-------------------------------------

//Function implementations

void gui_init()
{
   //Force background redraw by changing size
   SLK_layer_set_size(5,800,600);

   SLK_layer_set_size(0,150,400);

   //Load entry palette 
   palette = SLK_palette_load("palette/3-3-2.pal");
}

void gui_update()
{
   gui_buttons();

   if(SLK_key_down(SLK_KEY_LEFT))
      if(gui_out_width>0)
         gui_out_width-=8;
   if(SLK_key_down(SLK_KEY_RIGHT))
      if(gui_out_width<256)
         gui_out_width+=8;
   if(SLK_key_down(SLK_KEY_DOWN))
      if(gui_out_height>0)
         gui_out_height-=8;
   if(SLK_key_down(SLK_KEY_UP))
      if(gui_out_height<256)
         gui_out_height+=8;

   gui_draw();
}

static void gui_buttons()
{
   int x,y;
   SLK_mouse_get_layer_pos(0,&x,&y);

   if(SLK_mouse_pressed(SLK_BUTTON_LEFT))
   {
      if(x>3&&x<64&&y>372&&y<395) //Load button
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_openFileDialog("Select a png file","",1,filter_patterns,NULL,0);
         SLK_rgb_sprite_destroy(sprite_in);
         sprite_in = SLK_rgb_sprite_load(file_path);
         if(sprite_in)
         {
            SLK_layer_set_size(1,sprite_in->width,sprite_in->height);
            SLK_layer_set_current(1);
            SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
            SLK_draw_rgb_clear();
            SLK_draw_rgb_sprite(sprite_in,0,0);
            SLK_draw_rgb_set_changed(1);
            float scale;
            if(sprite_in->width>sprite_in->height)
               scale = 256.0f/sprite_in->width;
            else 
               scale = 256.0f/sprite_in->height;
            SLK_layer_set_scale(1,scale);
            update_output();
         }
      }
      else if(x>81&&x<144&&y>373&&y<395) //Save button
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_saveFileDialog("Save image","",1,filter_patterns,NULL);
         SLK_rgb_sprite_save(file_path,sprite_out);
         puts(file_path);
      }
      else if(x>3&&x<144&&y>332&&y<355) //Palette load button
      {
         const char *filter_patterns[2] = {"*.pal"};
         const char *file_path = tinyfd_openFileDialog("Load a palette","",1,filter_patterns,NULL,0);
         if(palette)
            free(palette);
         palette = SLK_palette_load(file_path);
      }
      else if(x>3&&x<144&&y>293&&y<315) //Process image button
      {
         update_output();
      }
      else if(x>0&&x<9&&y>19&&y<28) //Dither left
      {
         pixel_process_mode--;
         if(pixel_process_mode<0)
            pixel_process_mode = 5;
      }
      else if(x>142&&x<149&&y>19&&y<28) //Dither right
      {
         pixel_process_mode++;
         if(pixel_process_mode>5)
            pixel_process_mode = 0;
      }
      else if(x>0&&x<9&&y>89&&y<98) //Output width left
      {
         if(gui_out_width>0)
            gui_out_width-=8;
      }
      else if(x>142&&x<149&&y>89&&y<98) //Output width right
      {
         if(gui_out_width<256)
            gui_out_width+=8;
      }
      else if(x>0&&x<9&&y>123&&y<132) //Output height left
      {
         if(gui_out_height>0)
            gui_out_height-=8;
      }
      else if(x>142&&x<149&&y>123&&y<132) //Output height right
      {
         if(gui_out_height<256)
            gui_out_height+=8;
      }
   }
}

static void gui_draw()
{
   if(SLK_layer_get_resized(3))
   {
      SLK_layer_set_current(3);
      SLK_draw_rgb_set_changed(1);
      SLK_draw_rgb_set_clear_color(SLK_color_create(20,20,20,255));
      SLK_draw_rgb_clear();
      int w,h;
      SLK_layer_get_size(3,&w,&h);

      //Update layer pos
      gui_pos_x = w/32;
      gui_pos_y = (h-400)/2;
      SLK_layer_set_pos(0,gui_pos_x,gui_pos_y);

      int y = h/16;
      int x = gui_pos_x+150+w/32;
      SLK_layer_set_pos(1,x,y);
      SLK_draw_rgb_sprite(sprite_backdrop_1,x-4,y-4);
      SLK_draw_rgb_string(x+88,y+264,2,"Input",SLK_color_create(255,255,255,255));

      y = h-256-h/16;
      x = w-256-w/32;
      SLK_layer_set_pos(2,x,y);
      SLK_draw_rgb_sprite(sprite_backdrop_1,x-4,y-4);
      SLK_draw_rgb_string(x+80,y-24,2,"Output",SLK_color_create(255,255,255,255));
   }

   SLK_layer_set_current(0);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
   //SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,255));
   SLK_draw_rgb_clear();
   SLK_draw_rgb_sprite(sprite_gui_0,0,0);

   //Adjust text to middle
   const char *text = text_dither[pixel_process_mode];
   int len = strlen(text)*8;
   int pos_x = 8+(134-len)/2;
   SLK_draw_rgb_string(pos_x,20,1,text_dither[pixel_process_mode],SLK_color_create(255,255,255,255));
   char tmp_text[256];
   sprintf(tmp_text,"%d",gui_out_width);
   len = strlen(tmp_text)*8;
   pos_x = 8+(134-len)/2;
   SLK_draw_rgb_string(pos_x,90,1,tmp_text,SLK_color_create(255,255,255,255));
   sprintf(tmp_text,"%d",gui_out_height);
   len = strlen(tmp_text)*8;
   pos_x = 8+(134-len)/2;
   SLK_draw_rgb_string(pos_x,124,1,tmp_text,SLK_color_create(255,255,255,255));
}

static void update_output()
{
   if(sprite_out==NULL||sprite_out->width!=gui_out_width||sprite_out->height!=gui_out_height)
   {
      SLK_rgb_sprite_destroy(sprite_out);
      sprite_out = SLK_rgb_sprite_create(gui_out_width,gui_out_height);
      SLK_layer_set_size(2,gui_out_width,gui_out_height);
      float scale;
      if(sprite_out->width>sprite_out->height)
         scale = 256.0f/sprite_out->width;
      else 
         scale = 256.0f/sprite_out->height;
      SLK_layer_set_scale(2,scale);
   }

   if(sprite_in==NULL)
      return;

   process_image(sprite_in,sprite_out,palette,pixel_sample_mode,pixel_process_mode);

   SLK_layer_set_current(2);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
   SLK_draw_rgb_clear();
   SLK_draw_rgb_sprite(sprite_out,0,0);
}
