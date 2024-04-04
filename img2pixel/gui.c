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

#include "HLH_gui.h"
#include "HLH.h"
#include "HLH_json.h"
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
   SLIDER_SHARP,
   SLIDER_BRIGHTNESS,
   SLIDER_CONTRAST,
   SLIDER_SATURATION,
   SLIDER_HUE,
   SLIDER_GAMMA,
   SLIDER_COLOR_COUNT,
   SLIDER_COLOR_RED,
   SLIDER_COLOR_GREEN,
   SLIDER_COLOR_BLUE,
}Slider_id;

typedef enum
{
   CHECKBUTTON_KMEANS,
   CHECKBUTTON_KMEANSPP,
}Checkbutton_id;
//-------------------------------------

//Variables
static HLH_gui_imgcmp *gui_imgcmp;

static HLH_gui_group *gui_groups_left[5];

static SLK_image32 *gui_input = NULL;
static SLK_image32 *gui_output = NULL;

static HLH_gui_group *gui_bar_sample;
static HLH_gui_group *gui_groups_sample[2];
static HLH_gui_group *gui_bar_dither;
static HLH_gui_group *gui_bar_distance;
static HLH_gui_group *gui_group_kmeans;

static struct
{
   HLH_gui_slider *slider_blur;
   HLH_gui_slider *slider_x_off;
   HLH_gui_slider *slider_y_off;
   HLH_gui_slider *slider_width;
   HLH_gui_slider *slider_height;
   HLH_gui_slider *slider_scale_x;
   HLH_gui_slider *slider_scale_y;
   HLH_gui_slider *slider_sharp;
   HLH_gui_slider *slider_brightness;
   HLH_gui_slider *slider_contrast;
   HLH_gui_slider *slider_saturation;
   HLH_gui_slider *slider_hue;
   HLH_gui_slider *slider_gamma;
   HLH_gui_slider *slider_alpha_threshold;
   HLH_gui_slider *slider_dither_amount;
   HLH_gui_slider *slider_palette_weight;
   HLH_gui_slider *slider_color_count;
   HLH_gui_slider *slider_color_red;
   HLH_gui_slider *slider_color_green;
   HLH_gui_slider *slider_color_blue;

   HLH_gui_group *group_palette;

   HLH_gui_radiobutton *sample_sample_mode[5];
   HLH_gui_radiobutton *sample_scale_mode[2];
   HLH_gui_radiobutton *dither_dither_mode[8];
   HLH_gui_radiobutton *dither_color_dist[6];
   HLH_gui_radiobutton *palette_colors[256];

   HLH_gui_checkbutton *dither_kmeans;
   HLH_gui_checkbutton *palette_kmeanspp;
}gui;

//img2pixel
static int block_process = 0;
static int color_selected = 0;
static int blur_amount = 0;
static int sample_mode = 0;
static float x_offset = 0.f;
static float y_offset = 0.f;
static int scale_relative = 0;
static int size_relative_x = 1;
static int size_relative_y = 1;
static int size_absolute_x = 64;
static int size_absolute_y = 64;
static float sharp_amount = 0.f;
static float brightness = 0.f;
static float contrast = 1.f;
static float saturation = 1.f;
static float hue = 0.f;
static float gamma = 1.f;
static int kmeanspp = 0;
SLK_dither_config dither_config = {.alpha_threshold = 128};
//-------------------------------------

//Function prototypes
static int rb_radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_sample_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_scale_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_palette_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_load_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_save_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_help_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int checkbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_dither_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_distance_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_palette_gen_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);

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
      gui.slider_width = slider;
      HLH_gui_label_create(&gui_groups_sample[0]->e,0,"Height");
      slider = HLH_gui_slider_create(&gui_groups_sample[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_HEIGHT;
      gui.slider_height = slider;

      gui_groups_sample[1] = HLH_gui_group_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X);
      HLH_gui_label_create(&gui_groups_sample[1]->e,0,"Scale X");
      slider = HLH_gui_slider_create(&gui_groups_sample[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,16,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_SCALE_X;
      gui.slider_scale_x = slider;
      HLH_gui_label_create(&gui_groups_sample[1]->e,0,"Scale Y");
      slider = HLH_gui_slider_create(&gui_groups_sample[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,16,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_SCALE_Y;
      gui.slider_scale_y = slider;
      HLH_gui_element_ignore(&gui_groups_sample[0]->e,1);
      HLH_gui_element_ignore(&gui_groups_sample[1]->e,1);

      HLH_gui_radiobutton *r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_WEST,"Absolute",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_scale_msg;
      gui.sample_scale_mode[0] = r;
      HLH_gui_radiobutton *r_first = r;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_WEST,"Relative",NULL);
      r->e.usr = 1;
      r->e.msg_usr = radiobutton_scale_msg;
      gui.sample_scale_mode[1] = r;
      HLH_gui_radiobutton_set(r_first,1,1);

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample mode");

      HLH_gui_group *group_sample = HLH_gui_group_create(&gui_groups_left[0]->e.window->e,HLH_GUI_NO_PARENT|HLH_GUI_STYLE_01);
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Nearest ",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_sample_msg;
      gui.sample_sample_mode[0] = r;
      HLH_gui_radiobutton *first = r;
      //r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Floor   ",NULL);
      //r->e.usr = 1;
      //r->e.msg_usr = radiobutton_sample_msg;
      //gui.sample_sample_mode[1] = r;
      //r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Ceil    ",NULL);
      //r->e.usr = 2;
      //r->e.msg_usr = radiobutton_sample_msg;
      //gui.sample_sample_mode[2] = r;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bilinear",NULL);
      r->e.usr = 1;
      r->e.msg_usr = radiobutton_sample_msg;
      gui.sample_sample_mode[1] = r;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bicubic ",NULL);
      r->e.usr = 2;
      r->e.msg_usr = radiobutton_sample_msg;
      gui.sample_sample_mode[2] = r;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Lanczos ",NULL);
      r->e.usr = 3;
      r->e.msg_usr = radiobutton_sample_msg;
      gui.sample_sample_mode[3] = r;
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Cluster",NULL);
      r->e.usr = 4;
      r->e.msg_usr = radiobutton_sample_msg;
      gui.sample_sample_mode[4] = r;
      HLH_gui_radiobutton_set(first,1,1);
      const char *bar_sample[1] = {"Nearest  >"};
      gui_bar_sample = HLH_gui_menubar_create(&gui_groups_left[0]->e,0,HLH_GUI_PACK_WEST,bar_sample,(HLH_gui_element **)&group_sample,1,NULL);

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample x offset");
      slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_X_OFF;
      gui.slider_x_off = slider;
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample y offset");
      slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_Y_OFF;
      gui.slider_y_off = slider;

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Blur amount");
      group = HLH_gui_group_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X);
      slider = HLH_gui_slider_create(&group->e,HLH_GUI_PACK_WEST|HLH_GUI_FIXED_X,0);
      slider->e.fixed_size.x = 196;
      HLH_gui_slider_set(slider,0,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_BLUR;
      gui.slider_blur = slider;
      HLH_gui_button_create(&group->e,HLH_GUI_PACK_WEST,"<",NULL);
      HLH_gui_button_create(&group->e,HLH_GUI_PACK_WEST,">",NULL);
      HLH_gui_label_create(&group->e,HLH_GUI_PACK_WEST,"01.00");

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sharpen amount");
      group = HLH_gui_group_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X);
      slider = HLH_gui_slider_create(&group->e,HLH_GUI_PACK_WEST|HLH_GUI_FIXED_X,0);
      slider->e.fixed_size.x = 196;
      HLH_gui_slider_set(slider,1,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_SHARP;
      gui.slider_sharp = slider;
   }
   //-------------------------------------

   //Dither
   //-------------------------------------
   {
      gui_groups_left[1] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Alpha threshold");
      HLH_gui_slider *slider = HLH_gui_slider_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,128,255,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_ALPHA_THRESHOLD;
      gui.slider_alpha_threshold = slider;

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[1]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Dither mode");

      HLH_gui_group *group_dither = HLH_gui_group_create(&gui_groups_left[1]->e.window->e,HLH_GUI_NO_PARENT|HLH_GUI_STYLE_01);
      HLH_gui_radiobutton *r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"None             ",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_dither_msg;
      HLH_gui_radiobutton *first = r;
      gui.dither_dither_mode[0] = r;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bayer 8x8        ",NULL);
      r->e.usr = 1;
      r->e.msg_usr = radiobutton_dither_msg;
      gui.dither_dither_mode[1] = r;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bayer 4x4        ",NULL);
      r->e.usr = 2;
      r->e.msg_usr = radiobutton_dither_msg;
      gui.dither_dither_mode[2] = r;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bayer 2x2        ",NULL);
      r->e.usr = 3;
      r->e.msg_usr = radiobutton_dither_msg;
      gui.dither_dither_mode[3] = r;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Cluster 8x8      ",NULL);
      r->e.usr = 4;
      r->e.msg_usr = radiobutton_dither_msg;
      gui.dither_dither_mode[4] = r;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Cluster 4x4      ",NULL);
      r->e.usr = 5;
      r->e.msg_usr = radiobutton_dither_msg;
      gui.dither_dither_mode[5] = r;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Floyd-Steinberg  ",NULL);
      r->e.usr = 6;
      r->e.msg_usr = radiobutton_dither_msg;
      gui.dither_dither_mode[6] = r;
      r = HLH_gui_radiobutton_create(&group_dither->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Floyd-Steinberg 2",NULL);
      r->e.usr = 7;
      r->e.msg_usr = radiobutton_dither_msg;
      gui.dither_dither_mode[7] = r;
      HLH_gui_radiobutton_set(first,1,1);
      const char *bar_dither[1] = {"Bayer 4x4         >"};
      gui_bar_dither = HLH_gui_menubar_create(&gui_groups_left[1]->e,0,HLH_GUI_PACK_WEST,bar_dither,(HLH_gui_element **)&group_dither,1,NULL);

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Dither amount");
      slider = HLH_gui_slider_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,250,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_DITHER_AMOUNT;
      gui.slider_dither_amount = slider;

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Distance metric");

      HLH_gui_group *group_distance = HLH_gui_group_create(&gui_groups_left[1]->e.window->e,HLH_GUI_NO_PARENT|HLH_GUI_STYLE_01);
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"RGB euclidian",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_distance_msg;
      first = r;
      gui.dither_color_dist[0] = r;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"RGB weighted ",NULL);
      r->e.usr = 1;
      r->e.msg_usr = radiobutton_distance_msg;
      gui.dither_color_dist[1] = r;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"RGB redmean  ",NULL);
      r->e.usr = 2;
      r->e.msg_usr = radiobutton_distance_msg;
      gui.dither_color_dist[2] = r;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"CIE76        ",NULL);
      r->e.usr = 3;
      r->e.msg_usr = radiobutton_distance_msg;
      gui.dither_color_dist[3] = r;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"CIE94        ",NULL);
      r->e.usr = 4;
      r->e.msg_usr = radiobutton_distance_msg;
      gui.dither_color_dist[4] = r;
      r = HLH_gui_radiobutton_create(&group_distance->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"CIEDE2000    ",NULL);
      r->e.usr = 5;
      r->e.msg_usr = radiobutton_distance_msg;
      gui.dither_color_dist[5] = r;
      HLH_gui_radiobutton_set(first,1,1);
      const char *bar_distance[1] = {"RGB Euclidian >"};
      gui_bar_distance = HLH_gui_menubar_create(&gui_groups_left[1]->e,0,HLH_GUI_PACK_WEST,bar_distance,(HLH_gui_element **)&group_distance,1,NULL);

      HLH_gui_checkbutton *c = HLH_gui_checkbutton_create(&gui_groups_left[1]->e,0,"k-means",NULL);
      c->e.usr = CHECKBUTTON_KMEANS;
      c->e.msg_usr = checkbutton_msg;
      gui.dither_kmeans = c;
      gui_group_kmeans = HLH_gui_group_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X);
      HLH_gui_label_create(&gui_group_kmeans->e,0,"Palette weight");
      slider = HLH_gui_slider_create(&gui_group_kmeans->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,16,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_PALETTE_WEIGHT;
      gui.slider_palette_weight = slider;
      HLH_gui_element_ignore(&gui_group_kmeans->e,1);
   }
   //-------------------------------------

   //Palette
   //-------------------------------------
   {
      gui_groups_left[2] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);

      //Palette buttons
      HLH_gui_group *group_pal = HLH_gui_group_create(&gui_groups_left[2]->e,0);
      gui.group_palette = group_pal;
      int color = 0;
      for(int i = 0;i<16;i++)
      {
         HLH_gui_radiobutton *r = NULL;
         for(int j = 0;j<15;j++)
         {
            r = HLH_gui_radiobutton_create(&group_pal->e,HLH_GUI_PACK_WEST|HLH_GUI_PLACE_NW,"",NULL);
            gui.palette_colors[color] = r;
            r->e.usr_ptr = &dither_config.palette[color];
            r->e.usr = color++;
            r->e.msg_usr = radiobutton_palette_msg;
         }
         r = HLH_gui_radiobutton_create(&group_pal->e,HLH_GUI_PACK_NORTH|HLH_GUI_PLACE_WEST,"",NULL);
         gui.palette_colors[color] = r;
         r->e.usr_ptr = &dither_config.palette[color];
         r->e.usr = color++;
         r->e.msg_usr = radiobutton_palette_msg;
      }

      HLH_gui_label_create(&gui_groups_left[2]->e,0,"Red");
      HLH_gui_slider *slider = HLH_gui_slider_create(&gui_groups_left[2]->e,HLH_GUI_FILL_X,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_COLOR_RED;
      HLH_gui_slider_set(slider,1,256,0,0);
      gui.slider_color_red = slider;
      HLH_gui_label_create(&gui_groups_left[2]->e,0,"Green");
      slider = HLH_gui_slider_create(&gui_groups_left[2]->e,HLH_GUI_FILL_X,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_COLOR_GREEN;
      HLH_gui_slider_set(slider,1,256,0,0);
      gui.slider_color_green = slider;
      HLH_gui_label_create(&gui_groups_left[2]->e,0,"Blue");
      slider = HLH_gui_slider_create(&gui_groups_left[2]->e,HLH_GUI_FILL_X,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_COLOR_BLUE;
      HLH_gui_slider_set(slider,1,256,0,0);
      gui.slider_color_blue = slider;

      HLH_gui_label_create(&gui_groups_left[2]->e,0,"Color count");
      slider = HLH_gui_slider_create(&gui_groups_left[2]->e,HLH_GUI_FILL_X,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_COLOR_COUNT;
      HLH_gui_slider_set(slider,1,255,0,0);
      gui.slider_color_count = slider;

      HLH_gui_label_create(&gui_groups_left[2]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[2]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[2]->e,0,"                                ");

      HLH_gui_button *b = HLH_gui_button_create(&gui_groups_left[2]->e,0,"Generate palette",NULL);
      b->e.msg_usr = button_palette_gen_msg;
      HLH_gui_checkbutton *c = HLH_gui_checkbutton_create(&gui_groups_left[2]->e,0,"k-means++",NULL);
      c->e.usr = CHECKBUTTON_KMEANSPP;
      c->e.msg_usr = checkbutton_msg;
      gui.palette_kmeanspp = c;
   }
   //-------------------------------------

   //Colors
   //-------------------------------------
   {
      gui_groups_left[3] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Brightness");
      HLH_gui_slider *slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,250,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_BRIGHTNESS;
      gui.slider_brightness = slider;

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Contrast");
      slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,100,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_CONTRAST;
      gui.slider_contrast = slider;

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Saturation");
      slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,100,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_SATURATION;
      gui.slider_saturation = slider;

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Hue");
      slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,180,180*2,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_HUE;
      gui.slider_hue = slider;

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Gamma");
      slider = HLH_gui_slider_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,100,500,0,0);
      slider->e.msg_usr = slider_msg;
      slider->e.usr = SLIDER_GAMMA;
      gui.slider_gamma = slider;

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"                                ");
   }
   //-------------------------------------

   //Post process
   //-------------------------------------
   {
      gui_groups_left[4] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
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
      if(di)
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
         const char *preset = preset_load_select();
         gui_load_preset(preset);
      }
      //Palette
      else if(m->index==2)
      {
         const char *palette = palette_load_select();
         SLK_palette_load(palette,dither_config.palette,&dither_config.palette_colors);
         HLH_gui_slider_set(gui.slider_color_count,dither_config.palette_colors-1,255,1,1);
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
         const char *image = image_save_select();
         HLH_gui_image_save(image,gui_output->data,gui_output->w,gui_output->h);
      }
      //Preset
      else if(m->index==1)
      {
         const char *preset = preset_save_select();
         FILE *f = fopen(preset,"w");
         if(f==NULL)
            return 0;

         HLH_json5_root *root = HLH_json_create_root();
         HLH_json_object_add_integer(&root->root,"blur_amount",blur_amount);
         HLH_json_object_add_integer(&root->root,"sample_mode",sample_mode);
         HLH_json_object_add_real(&root->root,"x_offset",x_offset);
         HLH_json_object_add_real(&root->root,"y_offset",y_offset);
         HLH_json_object_add_boolean(&root->root,"scale_relative",scale_relative);
         HLH_json_object_add_integer(&root->root,"size_relative_x",size_relative_x);
         HLH_json_object_add_integer(&root->root,"size_relative_y",size_relative_y);
         HLH_json_object_add_integer(&root->root,"size_absolute_x",size_absolute_x);
         HLH_json_object_add_integer(&root->root,"size_absolute_y",size_absolute_y);
         HLH_json_object_add_real(&root->root,"sharp_amount",sharp_amount);
         HLH_json_object_add_real(&root->root,"brightness",brightness);
         HLH_json_object_add_real(&root->root,"contrast",contrast);
         HLH_json_object_add_real(&root->root,"saturation",saturation);
         HLH_json_object_add_real(&root->root,"hue",hue);
         HLH_json_object_add_real(&root->root,"gamma",gamma);
         HLH_json_object_add_boolean(&root->root,"kmeanspp",kmeanspp);
         HLH_json_object_add_integer(&root->root,"dither_alpha_threshold",dither_config.alpha_threshold);
         HLH_json_object_add_real(&root->root,"dither_dither_amount",dither_config.dither_amount);
         HLH_json_object_add_integer(&root->root,"dither_palette_weight",dither_config.palette_weight);
         HLH_json_object_add_boolean(&root->root,"dither_use_kmeans",dither_config.use_kmeans);
         HLH_json_object_add_integer(&root->root,"dither_dither_mode",dither_config.dither_mode);
         HLH_json_object_add_integer(&root->root,"dither_color_dist",dither_config.color_dist);
         HLH_json_object_add_integer(&root->root,"dither_palette_colors",dither_config.palette_colors);
         HLH_json5 array = HLH_json_create_array();
         for(int i = 0;i<256;i++)
            HLH_json_array_add_integer(&array,dither_config.palette[i]);
         HLH_json_object_add_array(&root->root,"dither_palette",array);

         HLH_json_write_file(f,&root->root);
         HLH_json_free(root);

         fclose(f);
      }
      //Palette
      else if(m->index==2)
      {
         const char *palette = palette_save_select();

         SLK_palette_save(palette,dither_config.palette,dither_config.palette_colors);
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
         size_relative_x = s->value+1;
         gui_process();
      }
      else if(s->e.usr==SLIDER_SCALE_Y)
      {
         size_relative_y = s->value+1;
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
      else if(s->e.usr==SLIDER_SHARP)
      {
         sharp_amount = (float)s->value/500.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_BRIGHTNESS)
      {
         brightness = (float)(s->value-250)/250.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_CONTRAST)
      {
         contrast = (float)s->value/100.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_SATURATION)
      {
         saturation = (float)s->value/100.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_HUE)
      {
         hue = (float)s->value-180.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_GAMMA)
      {
         gamma = (float)s->value/100.f;
         gui_process();
      }
      else if(s->e.usr==SLIDER_COLOR_COUNT)
      {
         dither_config.palette_colors = s->value+1;
         HLH_gui_element_redraw(&gui.group_palette->e);
         gui_process();
      }
      else if(s->e.usr==SLIDER_COLOR_RED)
      {
         uint32_t c = dither_config.palette[color_selected];
         dither_config.palette[color_selected] = ((uint32_t)s->value)|(SLK_color32_g(c)<<8)|(SLK_color32_b(c)<<16)|(SLK_color32_a(c)<<24);
         HLH_gui_element_redraw(&gui.group_palette->e);
         gui_process();
      }
      else if(s->e.usr==SLIDER_COLOR_GREEN)
      {
         uint32_t c = dither_config.palette[color_selected];
         dither_config.palette[color_selected] = (SLK_color32_r(c))|((uint32_t)s->value<<8)|(SLK_color32_b(c)<<16)|(SLK_color32_a(c)<<24);
         HLH_gui_element_redraw(&gui.group_palette->e);
         gui_process();
      }
      else if(s->e.usr==SLIDER_COLOR_BLUE)
      {
         uint32_t c = dither_config.palette[color_selected];
         dither_config.palette[color_selected] = (SLK_color32_r(c))|(SLK_color32_g(c)<<8)|((uint32_t)s->value<<16)|(SLK_color32_a(c)<<24);
         HLH_gui_element_redraw(&gui.group_palette->e);
         gui_process();
      }
   }

   return 0;
}

static int checkbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      if(e->usr==CHECKBUTTON_KMEANS)
      {
         if(di)
         {
            HLH_gui_element_ignore(&gui_group_kmeans->e,0);
            dither_config.use_kmeans = 1;
            gui_process();
         }
         else
         {
            HLH_gui_element_ignore(&gui_group_kmeans->e,1);
            dither_config.use_kmeans = 0;
            gui_process();
         }

         HLH_gui_element_pack(&e->window->e, e->window->e.bounds);
         HLH_gui_element_redraw(&e->window->e);
      }
      else if(e->usr==CHECKBUTTON_KMEANSPP)
      {
         kmeanspp = di;
      }
   }

   return 0;
}

static int radiobutton_dither_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_radiobutton *r = (HLH_gui_radiobutton *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      if(di)
      {
         dither_config.dither_mode = r->e.usr;
         char tmp[256];
         snprintf(tmp,256,"%s >",r->text);
         HLH_gui_menubar_label_set(gui_bar_dither,tmp,0);

         gui_process();
      }
   }

   return 0;
}

static int radiobutton_distance_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_radiobutton *r = (HLH_gui_radiobutton *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      if(di==1)
      {
         dither_config.color_dist = r->e.usr;
         char tmp[256];
         snprintf(tmp,256,"%s >",r->text);
         HLH_gui_menubar_label_set(gui_bar_distance,tmp,0);

         gui_process();
      }
   }

   return 0;
}

static int radiobutton_palette_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_radiobutton *r = (HLH_gui_radiobutton *)e;

   if(msg==HLH_GUI_MSG_DRAW)
   {
      if(r->e.usr>=dither_config.palette_colors)
         return 1;

      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      uint32_t color = *((uint32_t *)r->e.usr_ptr);

      //Infill
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx, bounds.miny, bounds.maxx, bounds.maxy),color);

      //Outline
      //HLH_gui_draw_rectangle(&r->e, bounds, 0xff000000);

      //Border
      if(r->state||r->checked)
      {
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx + 0 * scale, bounds.miny + 1 * scale, bounds.minx + 1 * scale, bounds.maxy - 1 * scale), 0xff000000);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx + 0 * scale, bounds.maxy - 1 * scale, bounds.maxx - 1 * scale, bounds.maxy - 0 * scale), 0xff000000);

         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.maxx - 1 * scale, bounds.miny + 1 * scale, bounds.maxx - 0 * scale, bounds.maxy - 1 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 0 * scale, bounds.maxx - 0 * scale, bounds.miny + 1 * scale), 0xff323232);
      }
      else
      {
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx + 0 * scale, bounds.miny + 1 * scale, bounds.minx + 1 * scale, bounds.maxy - 0 * scale), 0xff323232);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx + 0 * scale, bounds.maxy - 1 * scale, bounds.maxx - 1 * scale, bounds.maxy - 0 * scale), 0xff323232);

         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.maxx - 1 * scale, bounds.miny + 1 * scale, bounds.maxx - 0 * scale, bounds.maxy - 1 * scale), 0xffc8c8c8);
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 0 * scale, bounds.maxx - 0 * scale, bounds.miny + 1 * scale), 0xffc8c8c8);
      }

      int height = (bounds.maxy - bounds.miny);
      int dim = (HLH_GUI_GLYPH_HEIGHT)*HLH_gui_get_scale();
      int offset = (height - dim) / 2;
      if(r->checked)
      {
         uint32_t box_color = 0xff000000;
         if(SLK_color32_r(color)<128&&SLK_color32_g(color)<128&&SLK_color32_b(color)<128)
            box_color = 0xffffffff;
         HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx + offset + 5 * scale, bounds.miny + offset + 4 * scale, bounds.minx + dim + offset - 4 * scale, bounds.miny + offset - 5 * scale + dim), box_color);
      }

      return 1;
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return 16*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return 16*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_CLICK)
   {
      if(di)
      {
         color_selected = r->e.usr;
         uint32_t c = dither_config.palette[color_selected];
         HLH_gui_slider_set(gui.slider_color_red,SLK_color32_r(c),255,1,1);
         HLH_gui_slider_set(gui.slider_color_green,SLK_color32_g(c),255,1,1);
         HLH_gui_slider_set(gui.slider_color_blue,SLK_color32_b(c),255,1,1);
      }
   }

   return 0;
}

static int button_palette_gen_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      SLK_image32_kmeans(gui_input,dither_config.palette,dither_config.palette_colors,0);
      HLH_gui_element_redraw(&gui.group_palette->e);
      gui_process();
   }

   return 0;
}

static void gui_process()
{
   if(gui_input==NULL||block_process)
      return;
   if(gui_output!=NULL)
   {
      free(gui_output);
      gui_output = NULL;
   }

   SLK_image64 *img = SLK_image64_dup32(gui_input);
   SLK_image64_blur(img,blur_amount/16.f);
   SLK_image64_sharpen(img,sharp_amount);

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

   SLK_image64_hscb(sampled,hue,saturation,contrast,brightness);
   SLK_image64_gamma(sampled,gamma);
   gui_output = SLK_image64_dither(sampled,&dither_config);
   free(img);
   free(sampled);

   HLH_gui_imgcmp_update1(gui_imgcmp,gui_output->data,gui_output->w,gui_output->h,1);
   //free(dithered);
}

void gui_load_preset(const char *path)
{
   block_process = 1;
   if(path!=NULL)
   {
      FILE *f = fopen(path,"r");
      if(f==NULL)
         return;

      HLH_json5 fallback = {0};
      HLH_json5_root *root = HLH_json_parse_file_stream(f);

      blur_amount = HLH_json_get_object_integer(&root->root,"blur_amount",0);
      sample_mode = HLH_json_get_object_integer(&root->root,"sample_mode",0);
      x_offset = HLH_json_get_object_real(&root->root,"x_offset",0.f);
      y_offset = HLH_json_get_object_real(&root->root,"y_offset",0.f);
      scale_relative = HLH_json_get_object_boolean(&root->root,"scale_relative",0);
      size_relative_x = HLH_json_get_object_integer(&root->root,"size_relative_x",2);
      size_relative_y = HLH_json_get_object_integer(&root->root,"size_relative_y",2);
      size_absolute_x = HLH_json_get_object_integer(&root->root,"size_absolute_x",64);
      size_absolute_y = HLH_json_get_object_integer(&root->root,"size_absolute_y",64);
      sharp_amount = HLH_json_get_object_real(&root->root,"sharp_amount",0.f);
      brightness = HLH_json_get_object_real(&root->root,"brightness",0.f);
      contrast = HLH_json_get_object_real(&root->root,"contrast",1.f);
      saturation = HLH_json_get_object_real(&root->root,"saturation",1.f);
      hue = HLH_json_get_object_real(&root->root,"hue",0.f);
      gamma = HLH_json_get_object_real(&root->root,"gamma",1.f);
      kmeanspp = HLH_json_get_object_boolean(&root->root,"kmeanspp",1);
      dither_config.alpha_threshold = HLH_json_get_object_integer(&root->root,"dither_alpha_threshold",128);
      dither_config.dither_amount = HLH_json_get_object_real(&root->root,"dither_dither_amount",0.2f);
      dither_config.palette_weight = HLH_json_get_object_integer(&root->root,"dither_palette_weight",2);
      dither_config.use_kmeans = HLH_json_get_object_boolean(&root->root,"dither_use_kmeans",0);
      dither_config.dither_mode = HLH_json_get_object_integer(&root->root,"dither_dither_mode",2);
      dither_config.color_dist = HLH_json_get_object_integer(&root->root,"dither_color_dist",2);
      dither_config.palette_colors = HLH_json_get_object_integer(&root->root,"dither_palette_colors",2);
      HLH_json5 *array = HLH_json_get_object_array(&root->root,"dither_palette",&fallback);
      for(int i = 0;i<256;i++)
         dither_config.palette[i] = HLH_json_get_array_integer(array,i,0);

      HLH_json_free(root);
      fclose(f);

      color_selected = 0;
   }
   else
   {
      blur_amount = 0;
      sample_mode = 0;
      x_offset = 0.f;
      y_offset = 0.f;
      scale_relative = 0;
      size_relative_x = 2;
      size_relative_y = 2;
      size_absolute_x = 64;
      size_absolute_y = 64;
      sharp_amount = 0.f;
      brightness = 0.f;
      contrast = 1.f;
      saturation = 1.f;
      hue = 0.f;
      gamma = 1.f;
      color_selected = 0;
      kmeanspp = 1;
      dither_config.alpha_threshold = 128;
      dither_config.dither_amount = 0.2f;
      dither_config.palette_weight = 2;
      dither_config.use_kmeans = 0;
      dither_config.dither_mode = SLK_DITHER_BAYER4X4;
      dither_config.color_dist = SLK_RGB_REDMEAN;

      //Dawnbringer-32 palette
      dither_config.palette_colors = 32;
      dither_config.palette[0] = 0xff000000;
      dither_config.palette[1] = 0xff342022;
      dither_config.palette[2] = 0xff3c2845;
      dither_config.palette[3] = 0xff313966;
      dither_config.palette[4] = 0xff3b568f;
      dither_config.palette[5] = 0xff2671e0;
      dither_config.palette[6] = 0xff66a0d9;
      dither_config.palette[7] = 0xff9ac3ef;
      dither_config.palette[8] = 0xff36f2fb;
      dither_config.palette[9] = 0xff50e599;
      dither_config.palette[10] = 0xff30be6a;
      dither_config.palette[11] = 0xff6e9437;
      dither_config.palette[12] = 0xff2f694b;
      dither_config.palette[13] = 0xff244b52;
      dither_config.palette[14] = 0xff393c32;
      dither_config.palette[15] = 0xff743f3f;
      dither_config.palette[16] = 0xff826030;
      dither_config.palette[17] = 0xffe16e5b;
      dither_config.palette[18] = 0xffff9b63;
      dither_config.palette[19] = 0xffe4cd5f;
      dither_config.palette[20] = 0xfffcdbcb;
      dither_config.palette[21] = 0xffffffff;
      dither_config.palette[22] = 0xffb7ad9b;
      dither_config.palette[23] = 0xff877e84;
      dither_config.palette[24] = 0xff6a6a69;
      dither_config.palette[25] = 0xff525659;
      dither_config.palette[26] = 0xff8a4276;
      dither_config.palette[27] = 0xff3232ac;
      dither_config.palette[28] = 0xff6357d9;
      dither_config.palette[29] = 0xffba7bd7;
      dither_config.palette[30] = 0xff4a978f;
      dither_config.palette[31] = 0xff306f8a;
   }

   HLH_gui_slider_set(gui.slider_blur,blur_amount,500,1,1);
   HLH_gui_slider_set(gui.slider_x_off,(int)(x_offset*500.f),500,1,1);
   HLH_gui_slider_set(gui.slider_y_off,(int)(y_offset*500.f),500,1,1);
   HLH_gui_slider_set(gui.slider_width,size_absolute_x,512,1,1);
   HLH_gui_slider_set(gui.slider_height,size_absolute_y,512,1,1);
   HLH_gui_slider_set(gui.slider_scale_x,size_relative_x-1,15,1,1);
   HLH_gui_slider_set(gui.slider_scale_y,size_relative_y-1,15,1,1);
   HLH_gui_slider_set(gui.slider_sharp,(int)(sharp_amount*500.f),500,1,1);
   HLH_gui_slider_set(gui.slider_brightness,(int)(brightness*250.f+250.f),500,1,1);
   HLH_gui_slider_set(gui.slider_contrast,(int)(contrast*100.f),500,1,1);
   HLH_gui_slider_set(gui.slider_saturation,(int)(saturation*100.f),500,1,1);
   HLH_gui_slider_set(gui.slider_hue,(int)(hue+180.f),360,1,1);
   HLH_gui_slider_set(gui.slider_gamma,(int)(gamma*100.f),500,1,1);
   HLH_gui_slider_set(gui.slider_alpha_threshold,dither_config.alpha_threshold,255,1,1);
   HLH_gui_slider_set(gui.slider_dither_amount,(int)(dither_config.dither_amount*500.f),250,1,1);
   HLH_gui_slider_set(gui.slider_palette_weight,dither_config.palette_weight,16,1,1);
   HLH_gui_slider_set(gui.slider_color_count,dither_config.palette_colors-1,255,1,1);

   HLH_gui_radiobutton_set(gui.sample_sample_mode[sample_mode],1,1);
   HLH_gui_radiobutton_set(gui.sample_scale_mode[scale_relative],1,1);
   HLH_gui_radiobutton_set(gui.dither_dither_mode[dither_config.dither_mode],1,1);
   HLH_gui_radiobutton_set(gui.dither_color_dist[dither_config.color_dist],1,1);
   HLH_gui_radiobutton_set(gui.palette_colors[color_selected],1,1);

   HLH_gui_checkbutton_set(gui.dither_kmeans,dither_config.use_kmeans,1,1);
   HLH_gui_checkbutton_set(gui.palette_kmeanspp,kmeanspp,1,1);

   block_process = 0;
   gui_process();
}
//-------------------------------------
