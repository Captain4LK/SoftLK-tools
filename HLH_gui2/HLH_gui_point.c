/*
HLH_gui - gui framework

Written in 2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
//-------------------------------------

//Function implementations

HLH_gui_point HLH_gui_point_make(int x, int y)
{
   return (HLH_gui_point){.x = x, .y = y};
}

HLH_gui_point HLH_gui_point_add(HLH_gui_point a, HLH_gui_point b)
{
   return HLH_gui_point_make(a.x+b.x,a.y+b.y);
}
HLH_gui_point HLH_gui_point_sub(HLH_gui_point a, HLH_gui_point b)
{
   return HLH_gui_point_make(a.x-b.x,a.y-b.y);
}
//-------------------------------------
