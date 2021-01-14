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
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
SLK_RGB_sprite *font = NULL;
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

void assets_load()
{
   font = SLK_rgb_sprite_load("assets/cursive2.png");
   SLK_draw_rgb_set_font_sprite(font);
}
//-------------------------------------
