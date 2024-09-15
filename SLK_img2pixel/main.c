/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define HLH_IMPLEMENTATION
#include "HLH.h"
#define HLH_RW_IMPLEMENTATION
#include "HLH_rw.h"
#define HLH_JSON_IMPLEMENTATION
#include "HLH_json.h"

#include "HLH_gui.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
#include "util.h"
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
   HLH_gui_init();

   settings_load("settings.json");
   atexit(settings_save);

   gui_construct();
   gui_load_preset(NULL);

   return HLH_gui_message_loop();
}
//-------------------------------------
