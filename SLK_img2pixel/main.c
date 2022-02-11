/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes

#define LIBSLK_IMPLEMENTATION
#include "../headers/libSLK.h"

#define LIBSLK_GUI_IMPLEMENTATION
#include "../headers/libSLK_gui.h"
//-------------------------------------

//Internal includes
#include "gui.h"
#include "utility.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

int main(int argc, char **argv)
{
   //Use 800x500 resolution to force 2x pixel scale on 1080p resolution.
   SLK_setup(800,500,4,"SLK_img2pixel",0,SLK_WINDOW_MAX,1);
   SLK_timer_set_fps(30);

   //Layer for input preview
   SLK_layer_create(0,SLK_LAYER_RGB);
   SLK_layer_activate(0,1);
   SLK_layer_set_dynamic(0,0);
   SLK_layer_set_current(0);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
   SLK_draw_rgb_clear();

   //Layer for output preview
   SLK_layer_create(1,SLK_LAYER_RGB);
   SLK_layer_activate(1,0);
   SLK_layer_set_dynamic(1,0);
   SLK_layer_set_current(1);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
   SLK_draw_rgb_clear();

   //Layer for GUI
   SLK_layer_create(2,SLK_LAYER_RGB);
   SLK_layer_activate(2,1);
   SLK_layer_set_dynamic(2,1);
   SLK_layer_set_current(2);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(20,20,20,255));
   SLK_draw_rgb_clear();

   utility_init();
   gui_init();

   while(SLK_core_running())
   {
      SLK_update();

      gui_update();

      SLK_render_update();
   }

   utility_exit();

   return 0;
}
//-------------------------------------
