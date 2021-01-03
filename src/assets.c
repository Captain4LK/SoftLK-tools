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
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
SLK_RGB_sprite *sprite_backdrop_0 = NULL;
SLK_RGB_sprite *sprite_gui_0 = NULL;
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

void assets_load()
{
   sprite_backdrop_0 = SLK_rgb_sprite_load("assets/ocean.png");
   sprite_gui_0 = SLK_rgb_sprite_load("assets/ui.png");
}
//-------------------------------------
