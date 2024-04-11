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
static void ui_construct(void);
static int button_quit_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

int main(int argc, char **argv)
{
   HLH_gui_init();

   ui_construct();

   return HLH_gui_message_loop();
}

static void ui_construct(void)
{
   HLH_gui_window *win = HLH_gui_window_create("Test", 800, 600, NULL);

   const char *menu0[] =
   {
      "Test 1",
      "Test 2",
      "Test 3",
   };
   const char *menu1[] =
   {
      "Test 4",
      "Test 5",
      "Test 6",
   };
   const char *menu2[] =
   {
      "Test 6",
      "Test 8",
      "Test 9",
      "Test 10",
      "Test 11",
   };

   HLH_gui_element *menus[3];
   menus[0] = (HLH_gui_element *)HLH_gui_menu_create(&win->e, HLH_GUI_STYLE_01 | HLH_GUI_NO_PARENT, HLH_GUI_FILL_X | HLH_GUI_STYLE_01, menu0, 3, NULL);
   menus[1] = (HLH_gui_element *)HLH_gui_menu_create(&win->e, HLH_GUI_STYLE_01 | HLH_GUI_NO_PARENT, HLH_GUI_FILL_X | HLH_GUI_STYLE_01, menu1, 3, NULL);
   menus[2] = (HLH_gui_element *)HLH_gui_menu_create(&win->e, HLH_GUI_STYLE_01 | HLH_GUI_NO_PARENT, HLH_GUI_FILL_X | HLH_GUI_STYLE_01, menu2, 5, NULL);
   HLH_gui_separator_create(menus[1], HLH_GUI_FILL_X, 0);
   HLH_gui_checkbutton_create(menus[1], HLH_GUI_FILL_X | HLH_GUI_STYLE_01, "check", NULL);

   const char *menubar[] =
   {
      "Menu 0",
      "Menu 1",
      "Menu 2",
   };

   HLH_gui_group *group2 = HLH_gui_group_create(&win->e, HLH_GUI_EXPAND);
   HLH_gui_menubar_create(&group2->e, HLH_GUI_FILL_X, HLH_GUI_PACK_WEST | HLH_GUI_STYLE_01, menubar, menus, 3, NULL);
   HLH_gui_separator_create(&group2->e, HLH_GUI_FILL_X, 0);
   //HLH_gui_group *HLH_gui_menubar_create(HLH_gui_element *parent, uint64_t flags, uint64_t cflags, const char **labels, HLH_gui_element **panels, int child_count, HLH_gui_msg_handler msg_usr);

   //HLH_gui_menu_create(&group2->e,0,HLH_GUI_FILL_X|HLH_GUI_PACK_WEST,labels,4,NULL);

   HLH_gui_group *group = HLH_gui_group_create(&group2->e, HLH_GUI_STYLE_01);
   group->e.pad_out = HLH_gui_point_make(0, 10);
   //HLH_gui_label *label = HLH_gui_label_create(&group->e,0,"Hello, World!");
   //HLH_gui_button *text = HLH_gui_button_create(&group->e,0,"Test",NULL);


   HLH_gui_button_create(&group->e, HLH_GUI_PACK_WEST | HLH_GUI_MAX_X, "test 1", NULL);
   HLH_gui_button_create(&group->e, HLH_GUI_PACK_WEST | HLH_GUI_MAX_X, "test 2", NULL);
   HLH_gui_button_create(&group->e, HLH_GUI_PACK_WEST | HLH_GUI_MAX_X, "test 3", NULL);
   HLH_gui_checkbutton_create(&group->e, HLH_GUI_PACK_WEST | HLH_GUI_MAX_X, "check button", NULL);
   HLH_gui_radiobutton_create(&group->e, HLH_GUI_PACK_NORTH, "button 1", NULL);
   HLH_gui_radiobutton_create(&group->e, HLH_GUI_PACK_NORTH, "button 2", NULL);
   HLH_gui_radiobutton_create(&group->e, HLH_GUI_PACK_NORTH, "button 3", NULL);
   HLH_gui_radiobutton_create(&group->e, HLH_GUI_PACK_NORTH, "button 4", NULL);
   HLH_gui_radiobutton_create(&group->e, HLH_GUI_PACK_NORTH, "button 5", NULL);
   //HLH_gui_element_timer(&group->e,1000);

   group = HLH_gui_group_create(&group2->e, HLH_GUI_STYLE_01 | HLH_GUI_EXPAND);
   HLH_gui_slider_set(HLH_gui_slider_create(&group->e, HLH_GUI_FILL_X, 0), 20, 200, 0, 0);
   HLH_gui_slider_set(HLH_gui_slider_create(&group->e, HLH_GUI_FILL_Y | HLH_GUI_PACK_EAST, 1), 20, 200, 0, 0);
   HLH_gui_radiobutton_create(&group->e, HLH_GUI_PACK_EAST, "button 5", NULL);

   int w0 = 0;
   int h0 = 0;
   int w1 = 0;
   int h1 = 0;
   uint32_t *pix0 = HLH_gui_image_load("logo0.png", &w0, &h0);
   uint32_t *pix1 = HLH_gui_image_load("logo1.png", &w1, &h1);
   HLH_gui_imgcmp_create(&group->e, HLH_GUI_EXPAND | HLH_GUI_PACK_EAST, pix0, w0, h0, pix1, w1, h1);
   HLH_gui_image_free(pix0);
   HLH_gui_image_free(pix1);
   //HLH_gui_img_create_path(&group->e,HLH_GUI_EXPAND|HLH_GUI_PACK_EAST,"/home/lukash/Pictures/logo3d.png");

   win = HLH_gui_window_create("Test 2", 128, 64, "icons.png");
   group = HLH_gui_group_create(&win->e, HLH_GUI_EXPAND);
   HLH_gui_rect icon = HLH_gui_rect_make(0, 0, 8, 8);
   HLH_gui_button_create(&group->e, 0, NULL, &icon);
   HLH_gui_button_create(&group->e, 0, "Quit", NULL)->e.msg_usr = button_quit_msg;
}

static int button_quit_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      puts("QUIT");
      HLH_gui_window_close(e->window);
   }

   return 0;
}
//-------------------------------------
