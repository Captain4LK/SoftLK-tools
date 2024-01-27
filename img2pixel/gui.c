/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
static HLH_gui_imgcmp *gui_imgcmp;
//-------------------------------------

//Function prototypes
static int radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_load_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_save_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_help_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

void gui_construct(void)
{
   HLH_gui_window *win = HLH_gui_window_create("SLK_img2pixel",800,600,NULL);

   //Menubar
   //-------------------------------------
   const char *menu0[] = 
   {
      "Image",
      "Preset",
      "Palette",
   };
   const char *menu1[] = 
   {
      "Image",
      "Preset",
      "Palette",
   };
   const char *menu2[] = 
   {
      "About",
      "Test 8",
      "Test 9",
   };
   HLH_gui_element *menus[3];
   menus[0] = (HLH_gui_element *)HLH_gui_menu_create(&win->e,HLH_GUI_STYLE_01|HLH_GUI_NO_PARENT,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,menu0,3,menu_load_msg);
   menus[1] = (HLH_gui_element *)HLH_gui_menu_create(&win->e,HLH_GUI_STYLE_01|HLH_GUI_NO_PARENT,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,menu1,3,menu_save_msg);
   menus[2] = (HLH_gui_element *)HLH_gui_menu_create(&win->e,HLH_GUI_STYLE_01|HLH_GUI_NO_PARENT,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,menu2,3,menu_help_msg);

   const char *menubar[] = 
   {
      "Load",
      "Save",
      "Help",
   };

   HLH_gui_group *root_group = HLH_gui_group_create(&win->e,HLH_GUI_EXPAND);
   HLH_gui_menubar_create(&root_group->e,HLH_GUI_FILL_X,HLH_GUI_PACK_WEST|HLH_GUI_STYLE_01,menubar,menus,3,NULL);
   HLH_gui_separator_create(&root_group->e,HLH_GUI_FILL_X,0);
   //-------------------------------------

   HLH_gui_group *group_left = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_Y|HLH_GUI_PACK_WEST);
   HLH_gui_group *group_middle = HLH_gui_group_create(&root_group->e,HLH_GUI_EXPAND|HLH_GUI_PACK_WEST);
   HLH_gui_group *group_right = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_Y|HLH_GUI_PACK_WEST);

   //Left bar: settings
   //n subgroups --> marked as ignored by tabs on right
   HLH_gui_group *group = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
   HLH_gui_button_create(&group->e,HLH_GUI_PACK_NORTH,"test",NULL);

   //Right bar: settings tabs
   HLH_gui_radiobutton *rb = NULL;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Scale",NULL);
   rb->e.usr = 0;
   rb->e.msg_usr = radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Dither",NULL);
   rb->e.usr = 1;
   rb->e.msg_usr = radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Test 2",NULL);
   rb->e.usr = 2;
   rb->e.msg_usr = radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Test 3",NULL);
   rb->e.usr = 3;
   rb->e.msg_usr = radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Test 4",NULL);
   rb->e.usr = 4;
   rb->e.msg_usr = radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Test 5",NULL);
   rb->e.usr = 5;
   rb->e.msg_usr = radiobutton_msg;

   //Middle: preview
   uint32_t pix = 0;
   HLH_gui_imgcmp *imgcmp = HLH_gui_imgcmp_create(&group_middle->e,HLH_GUI_EXPAND,&pix,1,1,&pix,1,1);
   gui_imgcmp = imgcmp;
}

static int radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      //Uncheck
      if(di==0)
      {
      }
      //Check
      else if(di==1)
      {
      }
   }

   return 0;
}

static int menu_load_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_menubutton *m = (HLH_gui_menubutton *)e;

   if(msg==HLH_GUI_MSG_CLICK_MENU)
   {
      //Image
      if(m->index==0)
      {
         Image32 *img = image_select();
         if(img!=NULL)
         {
            HLH_gui_imgcmp_update0(,img->data,img->width,img->height,1);
         }
      }
      //Preset
      else if(m->index==1)
      {
      }
      //Palette
      else if(m->index==2)
      {
      }
   }

   return 0;
}

static int menu_save_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_menubutton *m = (HLH_gui_menubutton *)e;

   if(msg==HLH_GUI_MSG_CLICK_MENU)
   {
      //Image
      if(m->index==0)
      {
      }
      //Preset
      else if(m->index==1)
      {
      }
      //Palette
      else if(m->index==2)
      {
      }
   }

   return 0;
}

static int menu_help_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   return 0;
}
//-------------------------------------
