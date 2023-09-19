/*
test program for HLH_gui

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

int main(int argc, char **argv)
{
   HLH_gui_init();
   HLH_gui_window *win = HLH_gui_window_create("Test",800,600);

   HLH_gui_group *group2 = HLH_gui_group_create(&win->e,HLH_GUI_EXPAND);
   HLH_gui_frame *group = HLH_gui_frame_create(&group2->e,0);
   //HLH_gui_label *label = HLH_gui_label_create(&group->e,0,"Hello, World!");
   //HLH_gui_textbutton *text = HLH_gui_textbutton_create(&group->e,0,"Test");

   HLH_gui_textbutton_create(&group->e,HLH_GUI_PACK_WEST|HLH_GUI_MAX_X,"test 1");
   HLH_gui_textbutton_create(&group->e,HLH_GUI_PACK_WEST|HLH_GUI_MAX_X,"test 2");
   HLH_gui_textbutton_create(&group->e,HLH_GUI_PACK_WEST|HLH_GUI_MAX_X,"test 3");
   HLH_gui_checkbutton_create(&group->e,HLH_GUI_PACK_WEST|HLH_GUI_MAX_X,"check button");
   HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_NORTH,"button 1");
   HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_NORTH,"button 2");
   HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_NORTH,"button 3");
   HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_NORTH,"button 4");
   HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_NORTH,"button 5");
   


   return HLH_gui_message_loop();
}
//-------------------------------------
