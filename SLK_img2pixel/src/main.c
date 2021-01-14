/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <SLK/SLK.h>
//-------------------------------------

//Internal includes
#include "assets.h"
#include "gui.h"
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
   SLK_setup(800,500,4,"SLK_img2pixel",0,SLK_WINDOW_MAX,1);
   SLK_timer_set_fps(30);

   SLK_layer_create(0,SLK_LAYER_RGB); //Layer for GUI

   SLK_layer_activate(0,1);
   SLK_layer_set_dynamic(0,1);

   assets_load();
   gui_init();

   while(SLK_core_running())
   {
      SLK_update();

      gui_update();

      SLK_render_update();
   }

   return 0;
}
//-------------------------------------
