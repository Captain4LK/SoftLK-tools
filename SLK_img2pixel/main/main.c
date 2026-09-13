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
#define HLH_PATH_IMPLEMENTATION
#include "HLH_path.h"
#define HLH_RW_IMPLEMENTATION
#include "HLH_rw.h"
#define FOPEN_UTF8_IMPLEMENTATION
#include "../../external/fopen_utf8.h"
#define CUTE_FILES_IMPLEMENTATION
#include "cute_files.h"
#include <stdbool.h>

#include "HLH/HLH_base.h"
#include "HLH_gui.h"
#include "HLH/HLH_path.h"
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

   HLH_string dir;
   HLH_string filename;
   HLH_path_split(HLH_string_lit("a.out"), &dir, &filename);
   HLH_path_split(HLH_string_lit("./a.out"), &dir, &filename);
   HLH_path_split(HLH_string_lit("/a.out"), &dir, &filename);
   HLH_path_split(HLH_string_lit("/tmp/a.out"), &dir, &filename);
   HLH_path_split(HLH_string_lit("/tmp/"), &dir, &filename);
   HLH_path_split(HLH_string_lit("/"), &dir, &filename);
   HLH_path_split(HLH_string_lit(""), &dir, &filename);
   HLH_string ext;
   ext = HLH_path_ext(HLH_string_lit("/tmp/a.out"));
   ext = HLH_path_ext(HLH_string_lit("a.out"));
   ext = HLH_path_ext(HLH_string_lit("/tmp/.out"));
   ext = HLH_path_ext(HLH_string_lit(".out"));
   ext = HLH_path_ext(HLH_string_lit("out"));

   HLH_string stem;
   stem = HLH_path_stem(HLH_string_lit("/tmp/a.out"));
   stem = HLH_path_stem(HLH_string_lit("ab.out"));
   stem = HLH_path_stem(HLH_string_lit("/tmp/.out"));
   stem = HLH_path_stem(HLH_string_lit(".out"));
   stem = HLH_path_stem(HLH_string_lit("out"));

   settings_load("settings.json");
   atexit(settings_save);

   gui_construct();

   FILE *f = fopen("default.json","r");
   gui_load_preset(f);
   if(f!=NULL)
      fclose(f);

   return HLH_gui_message_loop();
}
//-------------------------------------
