/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include "HLH.h"
#include "HLH_gui.h"
//-------------------------------------

//Internal includes
#include "brush.h"
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

int brush_place(Project *project, int x, int y, int layer)
{
   //HLH_error_check(project!=NULL,"brush_place","project must be non-NULL\n");
   //HLH_error_check(layer>=0,"brush_place","invalid layer '%d'\n",layer);
   //HLH_error_check(layer<project->num_layers,"brush_place","invalid layer '%d'\n",layer);

   if(x<0||y<0||x>=project->width||y>=project->height)
      return 0;

   uint32_t index = y*project->width+x;
   if(project->bitmap[index/32]&(1<<(index-(index/32)*32)))
      return 0;

   project->layers[layer]->data[index] = 1;
   project->bitmap[index/32]|=1<<(index-(index/32)*32);

   //TODO(Captain4LK): update composite image

   return 1;

HLH_err:
   return 0;
}
//-------------------------------------
