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
#include "HLH.h"
//-------------------------------------

//Internal includes
#include "img2pixel.h"
#include "util.h"
#include "gui.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
typedef enum
{
   SLIDER_BLUR,
   SLIDER_X_OFF,
   SLIDER_Y_OFF,
   SLIDER_WIDTH,
   SLIDER_HEIGHT,
   SLIDER_SCALE_X,
   SLIDER_SCALE_Y,
   SLIDER_ALPHA_THRESHOLD,
   SLIDER_DITHER_AMOUNT,
   SLIDER_PALETTE_WEIGHT,
}Slider_id;
//-------------------------------------

//Variables
static HLH_gui_imgcmp *gui_imgcmp;

static HLH_gui_group *gui_groups_left[5];

static SLK_image32 *gui_input = NULL;

static HLH_gui_group *gui_bar_sample;
static HLH_gui_group *gui_groups_sample[2];
static HLH_gui_group *gui_bar_dither;
static HLH_gui_group *gui_bar_distance;

//img2pixel
static int blur_amount = 0;
static int sample_mode = 0;
static float x_offset = 0.f;
static float y_offset = 0.f;
static int scale_relative = 0;
static int size_relative_x = 1;
static int size_relative_y = 1;
static int size_absolute_x = 64;
static int size_absolute_y = 64;
SLK_dither_config dither_config = {.alpha_threshold = 128};
//-------------------------------------

//Function prototypes
static int rb_radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_sample_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_scale_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_load_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_save_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_help_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);

static void gui_process();
//-------------------------------------

//Function implementations

void gui_construct(void)
{
   HLH_gui_window *win = HLH_gui_window_create("SLK_img2pixel",1000,600,NULL);

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

   //Sample
   //-------------------------------------
   {
      gui_groups_left[0] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
      HLH_gui_group *group = HLH_gui_group_create(&gui_groups_left[0]->e,0);

      gui_groups_sample[0] = HLH_gui_group_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X);
      HLH_gui_label_create(&gui_groups_sample[0]->e,0,"Width");
      HLH_gui_slider *slider = HLH_gui_slider_create(&gui_groups_sample[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_WIDTH;
      HLH_gui_label_create(&gui_groups_sample[0]->e,0,"Height");
      slider = HLH_gui_slider_create(&gui_groups_sample[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_HEIGHT;

      gui_groups_sample[1] = HLH_gui_group_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X);
      HLH_gui_label_create(&gui_groups_sample[1]->e,0,"Scale X");
      slider = HLH_gui_slider_create(&gui_groups_sample[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,16,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_SCALE_X;
      HLH_gui_label_create(&gui_groups_sample[1]->e,0,"Scale Y");
      slider = HLH_gui_slider_create(&gui_groups_sample[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,16,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_SCALE_Y;
      HLH_gui_element_ignore(&gui_groups_sample[0]->e,1);
      HLH_gui_element_ignore(&gui_groups_sample[1]->e,1);

      HLH_gui_radiobutton *r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_WEST,"Absolute",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_scale_msg;
      HLH_gui_radiobutton *r_first = r;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_WEST,"Relative",NULL);
      r->e.usr = 1;
      r->e.msg_usr = radiobutton_scale_msg;
      HLH_gui_radiobutton_set(r_first,1,1);

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample mode");

      HLH_gui_group *group_sample = HLH_gui_group_create(&gui_groups_left[0]->e.window->e,HLH_GUI_NO_PARENT|HLH_GUI_STYLE_01);
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Round   ",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_sample_msg;
      HLH_gui_radiobutton *first = r;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Floor   ",NULL);
      r->e.usr = 1;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Ceil    ",NULL);
      r->e.usr = 2;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bilinear",NULL);
      r->e.usr = 3;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bicubic ",NULL);
      r->e.usr = 4;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Lanczos ",NULL);
      r->e.usr = 5;
      r->e.msg_usr = radiobutton_sample_msg;
      HLH_gui_radiobutton_set(first,1,1);
      const char *bar_sample[1] = {"Round    >"};
      gui_bar_sample = HLH_gui_menubar_create(&gui_groups_left[0]->e,0,HLH_GUI_PACK_WEST,bar_sample,(HLH_gui_element **)&group_sample,1,NULL);

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample x offset");
      slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_X_OFF;
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample y offset");
      slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_Y_OFF;

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Blur amount");
      group = HLH_gui_group_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X);
      slider = HLH_gui_slider_create(&group->e,HLH_GUI_PACK_WEST|HLH_GUI_FIXED_X,0);
      slider->e.fixed_size.x = 196;
      HLH_gui_slider_set(slider,1,512,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_BLUR;
      HLH_gui_button_create(&group->e,HLH_GUI_PACK_WEST,"<",NULL);
      HLH_gui_button_create(&group->e,HLH_GUI_PACK_WEST,">",NULL);
      HLH_gui_label_create(&group->e,HLH_GUI_PACK_WEST,"01.00");
   }
   //-------------------------------------

   //Dither
   //-------------------------------------
   {
      gui_groups_left[1] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Alpha threshold");
      HLH_gui_slider *slider = HLH_gui_slider_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,128,256,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_ALPHA_THRESHOLD;

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[1]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Dither mode");

      HLH_gui_group *group_dither = HLH_gui_group_create(&gui_groups_left[1]->e.window->e,HLH_GUI_NO_PARENT|HLH_GUI_STYLE_01);
      HLH_gui_radiobutton *r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"None             ",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_sample_msg;
      HLH_gui_radiobutton *first = r;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bayer 8x8        ",NULL);
      r->e.usr = 1;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bayer 4x4        ",NULL);
      r->e.usr = 2;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bayer 2x2        ",NULL);
      r->e.usr = 3;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Cluster 8x8      ",NULL);
      r->e.usr = 4;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Cluster 4x4      ",NULL);
      r->e.usr = 5;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Floyd-Steinberg  ",NULL);
      r->e.usr = 6;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Floyd-Steinberg 2",NULL);
      r->e.usr = 7;
      r->e.msg_usr = radiobutton_sample_msg;
      HLH_gui_radiobutton_set(first,1,1);
      const char *bar_dither[1] = {"Bayer 4x4         >"};
      gui_bar_dither = HLH_gui_menubar_create(&gui_groups_left[1]->e,0,HLH_GUI_PACK_WEST,bar_dither,(HLH_gui_element **)&group_dither,1,NULL);

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Dither amount");
      slider = HLH_gui_slider_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_DITHER_AMOUNT;

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Distance metric");

      HLH_gui_group *group_distance = HLH_gui_group_create(&gui_groups_left[1]->e.window->e,HLH_GUI_NO_PARENT|HLH_GUI_STYLE_01);
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"RGB euclidian",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_sample_msg;
      first = r;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"RGB weighted ",NULL);
      r->e.usr = 1;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"RGB redmean  ",NULL);
      r->e.usr = 2;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"CIE76        ",NULL);
      r->e.usr = 3;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"CIE94        ",NULL);
      r->e.usr = 4;
      r->e.msg_usr = radiobutton_sample_msg;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"CIEDE2000    ",NULL);
      r->e.usr = 5;
      r->e.msg_usr = radiobutton_sample_msg;
      HLH_gui_radiobutton_set(first,1,1);
      const char *bar_distance[1] = {"RGB Euclidian >"};
      gui_bar_distance = HLH_gui_menubar_create(&gui_groups_left[1]->e,0,HLH_GUI_PACK_WEST,bar_distance,(HLH_gui_element **)&group_distance,1,NULL);

      HLH_gui_checkbutton_create(&gui_groups_left[1]->e,0,"k-means",NULL);
      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Palette weight");
      slider = HLH_gui_slider_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,16,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_PALETTE_WEIGHT;
   }
   //-------------------------------------

   //Palette
   //-------------------------------------
   {
      gui_groups_left[2] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
      HLH_gui_button_create(&gui_groups_left[2]->e,HLH_GUI_PACK_NORTH,"Palette",NULL);
      HLH_gui_label_create(&gui_groups_left[2]->e,0,"                                ");
   }
   //-------------------------------------

   //Colors
   //-------------------------------------
   {
      gui_groups_left[3] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Brightness");
      HLH_gui_slider *slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Contrast");
      slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Saturation");
      slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Hue");
      slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Gamma");
      slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"                                ");
   }
   //-------------------------------------

   //Post process
   //-------------------------------------
   {
      gui_groups_left[4] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
      HLH_gui_button_create(&gui_groups_left[4]->e,HLH_GUI_PACK_NORTH,"Post process",NULL);
      HLH_gui_label_create(&gui_groups_left[4]->e,0,"                                ");
   }
   //-------------------------------------

   HLH_gui_element_ignore(&gui_groups_left[0]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[1]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[2]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[3]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[4]->e,1);

   //Right bar: settings tabs
   HLH_gui_radiobutton *rb = NULL;
   HLH_gui_radiobutton *sample = NULL;
   sample = rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Sample",NULL);
   rb->e.usr = 0;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Dither",NULL);
   rb->e.usr = 1;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Palette",NULL);
   rb->e.usr = 2;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Colors",NULL);
   rb->e.usr = 3;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Post process",NULL);
   rb->e.usr = 4;
   rb->e.msg_usr = rb_radiobutton_msg;

   HLH_gui_radiobutton_set(sample,1,0);

   //Middle: preview
   uint32_t pix = 0;
   HLH_gui_imgcmp *imgcmp = HLH_gui_imgcmp_create(&group_middle->e,HLH_GUI_EXPAND,&pix,1,1,&pix,1,1);
   gui_imgcmp = imgcmp;
}

static int rb_radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      //Uncheck
      if(di==0)
      {
         HLH_gui_element_ignore(&gui_groups_left[e->usr]->e,1);
      }
      //Check
      else if(di==1)
      {
         HLH_gui_element_ignore(&gui_groups_left[e->usr]->e,0);
         HLH_gui_element_pack(&e->window->e, e->window->e.bounds);
         HLH_gui_element_redraw(&e->window->e);
      }
   }

   return 0;
}

static int radiobutton_sample_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_radiobutton *r = (HLH_gui_radiobutton *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      if(di==1&&r->e.usr!=sample_mode)
      {
         sample_mode = r->e.usr;
         char tmp[256];
         snprintf(tmp,256,"%s >",r->text);
         HLH_gui_menubar_label_set(gui_bar_sample,tmp,0);

         gui_process();
      }
   }
   return 0;
}

static int radiobutton_scale_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      //Uncheck
      if(di==0)
      {
         HLH_gui_element_ignore(&gui_groups_sample[e->usr]->e,1);
      }
      //Check
      else if(di==1)
      {
         HLH_gui_element_ignore(&gui_groups_sample[e->usr]->e,0);
         HLH_gui_element_pack(&e->window->e, e->window->e.bounds);
         HLH_gui_element_redraw(&e->window->e);
         scale_relative = e->usr;
         gui_process();
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
         SLK_image32 *img = image_select();
         if(img!=NULL)
         {
            HLH_gui_imgcmp_update0(gui_imgcmp,img->data,img->w,img->h,1);
            if(gui_input!=NULL)
               free(gui_input);
            gui_input = SLK_image32_dup(img);
            free(img);

            gui_process();
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

static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_slider *s = (HLH_gui_slider *)e;
   if(msg==HLH_GUI_MSG_SLIDER_VALUE_CHANGED)
   {
      if(s->e.usr==SLIDER_BLUR)
      {
         blur_amount = s->value;
         gui_process();
      }
      else if(s->e.usr==SLIDER_X_OFF)
      {
         x_offset = (float)s->value/512.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_Y_OFF)
      {
         y_offset = (float)s->value/512.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_WIDTH)
      {
         size_absolute_x = s->value;
         gui_process();
      }
      else if(s->e.usr==SLIDER_HEIGHT)
      {
         size_absolute_y = s->value;
         gui_process();
      }
      else if(s->e.usr==SLIDER_SCALE_X)
      {
         size_relative_x = s->value;
         gui_process();
      }
      else if(s->e.usr==SLIDER_SCALE_Y)
      {
         size_relative_y = s->value;
         gui_process();
      }
      else if(s->e.usr==SLIDER_ALPHA_THRESHOLD)
      {
         dither_config.alpha_threshold = s->value;
         gui_process();
      }
      else if(s->e.usr==SLIDER_DITHER_AMOUNT)
      {
         dither_config.dither_amount = s->value/500.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_PALETTE_WEIGHT)
      {
         dither_config.palette_weight = s->value;
         gui_process();
      }
   }

   return 0;
}

static void gui_process()
{
   if(gui_input==NULL)
      return;

   SLK_image64 *img = SLK_image64_dup32(gui_input);
   SLK_image64_blur(img,blur_amount/16.f);

   int width = 0;
   int height = 0;
   if(scale_relative)
   {
      width = img->w/HLH_non_zero(size_relative_x);
      height = img->h/HLH_non_zero(size_relative_y);
   }
   else
   {
      width = size_absolute_x;
      height = size_absolute_y;
   }
   SLK_image64 *sampled = SLK_image64_sample(img,width,height,sample_mode,x_offset,y_offset);

   SLK_image64_hscb(sampled,0.f,1.f,1.f,0.f);
   SLK_image64_gamma(sampled,1.f);
   SLK_dither_config dither = {0};
   dither.dither_mode = SLK_DITHER_BAYER8X8;
   dither.dither_amount = 0.5f;
   dither.alpha_threshold = 128;
   dither.palette[0] = 0xffff0000;
   dither.palette[1] = 0xff00ff00;
   dither.palette[2] = 0xff0000ff;
   dither.palette[3] = 0xff000000;
   dither.palette_colors = 4;
   SLK_image32 *dithered = SLK_image64_dither(sampled,&dither);
   free(img);
   free(sampled);

   HLH_gui_imgcmp_update1(gui_imgcmp,dithered->data,dithered->w,dithered->h,1);
   free(dithered);
}
//-------------------------------------
