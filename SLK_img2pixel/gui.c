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
#include <time.h>

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
#define SLIDER(root_group,type,type_enum) \
   group = HLH_gui_group_create(root_group,HLH_GUI_FILL_X); \
   slider = HLH_gui_slider_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL|HLH_GUI_FIXED_X,0); \
   slider->e.fixed_size.x = 196*HLH_gui_get_scale(); \
   HLH_gui_slider_set(slider,-1,1,0,0); \
   slider->e.msg_usr = slider_msg; \
   slider->e.usr = SLIDER_##type_enum; \
   gui.slider_##type = slider; \
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x11",NULL); \
   b->e.msg_usr = button_sub_msg; \
   b->e.usr = BUTTON_##type_enum; \
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x10",NULL); \
   b->e.msg_usr = button_add_msg; \
   b->e.usr = BUTTON_##type_enum; \
   gui.entry_##type = HLH_gui_entry_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,5); \
   gui.entry_##type->e.msg_usr = entry_msg; \
   gui.entry_##type->e.usr = ENTRY_##type_enum;
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
   SLIDER_TINT_RED,
   SLIDER_TINT_GREEN,
   SLIDER_TINT_BLUE,
}Slider_id;

typedef enum
{
   BUTTON_BLUR,
   BUTTON_X_OFF,
   BUTTON_Y_OFF,
   BUTTON_WIDTH,
   BUTTON_HEIGHT,
   BUTTON_SCALE_X,
   BUTTON_SCALE_Y,
   BUTTON_SHARP,
   BUTTON_ALPHA_THRESHOLD,
   BUTTON_DITHER_AMOUNT,
   BUTTON_PALETTE_WEIGHT,
   BUTTON_COLOR_COUNT,
   BUTTON_COLOR_RED,
   BUTTON_COLOR_GREEN,
   BUTTON_COLOR_BLUE,
   BUTTON_TINT_RED,
   BUTTON_TINT_GREEN,
   BUTTON_TINT_BLUE,
   BUTTON_BRIGHTNESS,
   BUTTON_CONTRAST,
   BUTTON_SATURATION,
   BUTTON_HUE,
   BUTTON_GAMMA,
}Button_id;

typedef enum
{
   ENTRY_BLUR,
   ENTRY_X_OFF,
   ENTRY_Y_OFF,
   ENTRY_WIDTH,
   ENTRY_HEIGHT,
   ENTRY_SCALE_X,
   ENTRY_SCALE_Y,
   ENTRY_SHARP,
   ENTRY_ALPHA_THRESHOLD,
   ENTRY_DITHER_AMOUNT,
   ENTRY_PALETTE_WEIGHT,
   ENTRY_COLOR_COUNT,
   ENTRY_COLOR_RED,
   ENTRY_COLOR_GREEN,
   ENTRY_COLOR_BLUE,
   ENTRY_TINT_RED,
   ENTRY_TINT_GREEN,
   ENTRY_TINT_BLUE,
   ENTRY_BRIGHTNESS,
   ENTRY_CONTRAST,
   ENTRY_SATURATION,
   ENTRY_HUE,
   ENTRY_GAMMA,
}Entry_id;

typedef enum
{
   CHECKBUTTON_KMEANS,
   CHECKBUTTON_KMEANSPP,
}Checkbutton_id;
//-------------------------------------

//Variables
static HLH_gui_imgcmp *gui_imgcmp;

static HLH_gui_group *gui_groups_left[4];

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
   HLH_gui_slider *slider_tint_red;
   HLH_gui_slider *slider_tint_green;
   HLH_gui_slider *slider_tint_blue;

   HLH_gui_entry *entry_blur;
   HLH_gui_entry *entry_x_off;
   HLH_gui_entry *entry_y_off;
   HLH_gui_entry *entry_width;
   HLH_gui_entry *entry_height;
   HLH_gui_entry *entry_scale_x;
   HLH_gui_entry *entry_scale_y;
   HLH_gui_entry *entry_sharp;
   HLH_gui_entry *entry_alpha_threshold;
   HLH_gui_entry *entry_dither_amount;
   HLH_gui_entry *entry_palette_weight;
   HLH_gui_entry *entry_color_count;
   HLH_gui_entry *entry_color_red;
   HLH_gui_entry *entry_color_green;
   HLH_gui_entry *entry_color_blue;
   HLH_gui_entry *entry_tint_red;
   HLH_gui_entry *entry_tint_green;
   HLH_gui_entry *entry_tint_blue;
   HLH_gui_entry *entry_brightness;
   HLH_gui_entry *entry_contrast;
   HLH_gui_entry *entry_saturation;
   HLH_gui_entry *entry_hue;
   HLH_gui_entry *entry_gamma;

   HLH_gui_group *group_palette;

   HLH_gui_radiobutton *sample_sample_mode[5];
   HLH_gui_radiobutton *sample_scale_mode[2];
   HLH_gui_radiobutton *dither_dither_mode[8];
   HLH_gui_radiobutton *dither_color_dist[6];
   HLH_gui_radiobutton *palette_colors[256];
   HLH_gui_radiobutton *postprocess_colors[256];

   HLH_gui_checkbutton *dither_kmeans;
   HLH_gui_checkbutton *palette_kmeanspp;
}gui;

//img2pixel
static int block_process = 0;
static int color_selected = 0;
static float blur_amount = 0;
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
static uint8_t tint_red = 255;
static uint8_t tint_green = 255;
static uint8_t tint_blue = 255;

//cached
static SLK_image64 *cache_sample;
static SLK_image64 *cache_sharp;
static SLK_image64 *cache_tint;
//-------------------------------------

//Function prototypes
static int rb_radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_sample_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_scale_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_palette_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int entry_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_add_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_sub_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_load_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_save_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_help_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int checkbutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_dither_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_distance_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_palette_gen_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void radiobutton_palette_draw(HLH_gui_radiobutton *r);

static void gui_process(int from);
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
   HLH_gui_element *menus[2];
   menus[0] = (HLH_gui_element *)HLH_gui_menu_create(&win->e,HLH_GUI_STYLE_01|HLH_GUI_NO_PARENT,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,menu0,3,menu_load_msg);
   menus[1] = (HLH_gui_element *)HLH_gui_menu_create(&win->e,HLH_GUI_STYLE_01|HLH_GUI_NO_PARENT,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,menu1,3,menu_save_msg);

   const char *menubar[] = 
   {
      "Load",
      "Save",
   };

   HLH_gui_group *root_group = HLH_gui_group_create(&win->e,HLH_GUI_FILL);
   HLH_gui_menubar_create(&root_group->e,HLH_GUI_FILL_X,HLH_GUI_LAYOUT_HORIZONTAL|HLH_GUI_STYLE_01,menubar,menus,2,NULL);
   HLH_gui_separator_create(&root_group->e,HLH_GUI_FILL_X,0);
   //-------------------------------------

   HLH_gui_group *group_left = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_Y|HLH_GUI_LAYOUT_HORIZONTAL);
   HLH_gui_group *group_middle = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL|HLH_GUI_LAYOUT_HORIZONTAL);
   HLH_gui_group *group_right = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_Y|HLH_GUI_LAYOUT_HORIZONTAL);

   //Left bar: settings
   //n subgroups --> marked as ignored by tabs on right

   //Sample
   //-------------------------------------
   {
      HLH_gui_button *b = NULL;
      HLH_gui_slider *slider = NULL;
      HLH_gui_group *group = NULL;

      gui_groups_left[0] = HLH_gui_group_create(&group_left->e,HLH_GUI_FILL);
      HLH_gui_group *group_relative = HLH_gui_group_create(&gui_groups_left[0]->e,0);

      gui_groups_sample[0] = HLH_gui_group_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X);
      HLH_gui_label_create(&gui_groups_sample[0]->e,0,"Width");
      SLIDER(&gui_groups_sample[0]->e,width,WIDTH)
      HLH_gui_label_create(&gui_groups_sample[0]->e,0,"Height");
      SLIDER(&gui_groups_sample[0]->e,height,HEIGHT)


      gui_groups_sample[1] = HLH_gui_group_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X);
      HLH_gui_label_create(&gui_groups_sample[1]->e,0,"Scale X");
      SLIDER(&gui_groups_sample[1]->e,scale_x,SCALE_X);
      HLH_gui_label_create(&gui_groups_sample[1]->e,0,"Scale Y");
      SLIDER(&gui_groups_sample[1]->e,scale_y,SCALE_Y);

      HLH_gui_radiobutton *r = HLH_gui_radiobutton_create(&group_relative->e,HLH_GUI_LAYOUT_HORIZONTAL,"Absolute",NULL);
      r->e.usr = 0;
      r->e.msg_usr = radiobutton_scale_msg;
      gui.sample_scale_mode[0] = r;
      HLH_gui_radiobutton *r_first = r;
      r = HLH_gui_radiobutton_create(&group_relative->e,HLH_GUI_LAYOUT_HORIZONTAL,"Relative",NULL);
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
      const char *bar_sample[1] = {"Nearest  \x1f"};
      gui_bar_sample = HLH_gui_menubar_create(&gui_groups_left[0]->e,0,HLH_GUI_LAYOUT_HORIZONTAL,bar_sample,(HLH_gui_element **)&group_sample,1,NULL);

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample x offset");
      SLIDER(&gui_groups_left[0]->e,x_off,X_OFF)
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample y offset");
      SLIDER(&gui_groups_left[0]->e,y_off,Y_OFF)

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Blur amount");
      SLIDER(&gui_groups_left[0]->e,blur,BLUR)

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sharpen amount");
      SLIDER(&gui_groups_left[0]->e,sharp,SHARP)
   }
   //-------------------------------------

   //Dither
   //-------------------------------------
   {
      HLH_gui_button *b = NULL;
      HLH_gui_slider *slider = NULL;
      HLH_gui_group *group = NULL;
      gui_groups_left[1] = HLH_gui_group_create(&group_left->e,HLH_GUI_FILL);

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Alpha threshold");
      SLIDER(&gui_groups_left[1]->e,alpha_threshold,ALPHA_THRESHOLD)

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
      const char *bar_dither[1] = {"Bayer 4x4         \x1f"};
      gui_bar_dither = HLH_gui_menubar_create(&gui_groups_left[1]->e,0,HLH_GUI_LAYOUT_HORIZONTAL,bar_dither,(HLH_gui_element **)&group_dither,1,NULL);

      HLH_gui_label_create(&gui_groups_left[1]->e,0,"Dither amount");
      SLIDER(&gui_groups_left[1]->e,dither_amount,DITHER_AMOUNT)

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
      const char *bar_distance[1] = {"RGB Euclidian \x1f"};
      gui_bar_distance = HLH_gui_menubar_create(&gui_groups_left[1]->e,0,HLH_GUI_LAYOUT_HORIZONTAL,bar_distance,(HLH_gui_element **)&group_distance,1,NULL);

      HLH_gui_checkbutton *c = HLH_gui_checkbutton_create(&gui_groups_left[1]->e,0,"k-means",NULL);
      c->e.usr = CHECKBUTTON_KMEANS;
      c->e.msg_usr = checkbutton_msg;
      gui.dither_kmeans = c;
      gui_group_kmeans = HLH_gui_group_create(&gui_groups_left[1]->e,HLH_GUI_FILL_X);
      HLH_gui_label_create(&gui_group_kmeans->e,0,"Palette weight");
      SLIDER(&gui_group_kmeans->e,palette_weight,PALETTE_WEIGHT)
      HLH_gui_element_ignore(&gui_group_kmeans->e,1);
   }
   //-------------------------------------

   //Palette
   //-------------------------------------
   {
      HLH_gui_button *b = NULL;
      HLH_gui_slider *slider = NULL;
      HLH_gui_group *group = NULL;
      gui_groups_left[2] = HLH_gui_group_create(&group_left->e,HLH_GUI_FILL);

      //Palette buttons
      HLH_gui_group *group_pal = HLH_gui_group_create(&gui_groups_left[2]->e,0);
      gui.group_palette = group_pal;
      int color = 0;
      for(int i = 0;i<16;i++)
      {
         HLH_gui_radiobutton *r = NULL;
         for(int j = 0;j<15;j++)
         {
            r = HLH_gui_radiobutton_create(&group_pal->e,HLH_GUI_LAYOUT_HORIZONTAL|HLH_GUI_NO_CENTER_X|HLH_GUI_NO_CENTER_Y,"",NULL);
            gui.palette_colors[color] = r;
            r->e.usr_ptr = &dither_config.palette[color];
            r->e.usr = color++;
            r->e.msg_usr = radiobutton_palette_msg;
         }
         r = HLH_gui_radiobutton_create(&group_pal->e,HLH_GUI_NO_CENTER_X|HLH_GUI_NO_CENTER_Y,"",NULL);
         gui.palette_colors[color] = r;
         r->e.usr_ptr = &dither_config.palette[color];
         r->e.usr = color++;
         r->e.msg_usr = radiobutton_palette_msg;
      }

      HLH_gui_label_create(&gui_groups_left[2]->e,0,"Red");
      SLIDER(&gui_groups_left[2]->e,color_red,COLOR_RED);
      HLH_gui_label_create(&gui_groups_left[2]->e,0,"Green");
      SLIDER(&gui_groups_left[2]->e,color_green,COLOR_GREEN);
      HLH_gui_label_create(&gui_groups_left[2]->e,0,"Blue");
      SLIDER(&gui_groups_left[2]->e,color_blue,COLOR_BLUE);

      HLH_gui_label_create(&gui_groups_left[2]->e,0,"Color count");
      SLIDER(&gui_groups_left[2]->e,color_count,COLOR_COUNT);

      HLH_gui_label_create(&gui_groups_left[2]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[2]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[2]->e,0,"                                ");

      b = HLH_gui_button_create(&gui_groups_left[2]->e,0,"Generate palette",NULL);
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
      HLH_gui_button *b = NULL;
      HLH_gui_slider *slider = NULL;
      HLH_gui_group *group = NULL;
      gui_groups_left[3] = HLH_gui_group_create(&group_left->e,HLH_GUI_FILL);

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Brightness");
      SLIDER(&gui_groups_left[3]->e,brightness,BRIGHTNESS)

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Contrast");
      SLIDER(&gui_groups_left[3]->e,contrast,CONTRAST)

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Saturation");
      SLIDER(&gui_groups_left[3]->e,saturation,SATURATION)

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Hue");
      SLIDER(&gui_groups_left[3]->e,hue,HUE)

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Gamma");
      SLIDER(&gui_groups_left[3]->e,gamma,GAMMA)

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"                                ");
      HLH_gui_separator_create(&gui_groups_left[3]->e,HLH_GUI_FILL_X,0);
      HLH_gui_label_create(&gui_groups_left[3]->e,0,"                                ");

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Tint red");
      SLIDER(&gui_groups_left[3]->e,tint_red,TINT_RED)

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Tint green");
      SLIDER(&gui_groups_left[3]->e,tint_green,TINT_GREEN)

      HLH_gui_label_create(&gui_groups_left[3]->e,0,"Tint blue");
      SLIDER(&gui_groups_left[3]->e,tint_blue,TINT_BLUE)
   }
   //-------------------------------------

   //Post process
   //-------------------------------------
   {
      //gui_groups_left[4] = HLH_gui_group_create(&group_left->e,HLH_GUI_FILL);
      //HLH_gui_label_create(&gui_groups_left[4]->e,0,"                                ");
   }
   //-------------------------------------

   HLH_gui_element_ignore(&gui_groups_left[0]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[1]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[2]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[3]->e,1);
   //HLH_gui_element_ignore(&gui_groups_left[4]->e,1);

   //Right bar: settings tabs
   HLH_gui_radiobutton *rb = NULL;
   HLH_gui_radiobutton *sample = NULL;
   sample = rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_STYLE_02|HLH_GUI_FILL_X,"Sample",NULL);
   rb->e.usr = 0;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_STYLE_02|HLH_GUI_FILL_X,"Dither",NULL);
   rb->e.usr = 1;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_STYLE_02|HLH_GUI_FILL_X,"Palette",NULL);
   rb->e.usr = 2;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_STYLE_02|HLH_GUI_FILL_X,"Colors",NULL);
   rb->e.usr = 3;
   rb->e.msg_usr = rb_radiobutton_msg;

   HLH_gui_radiobutton_set(sample,1,1);

   //Middle: preview
   uint32_t pix = 0;
   HLH_gui_imgcmp *imgcmp = HLH_gui_imgcmp_create(&group_middle->e,HLH_GUI_FILL,&pix,1,1,&pix,1,1);
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
         HLH_gui_element_layout(&e->window->e, e->window->e.bounds);
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
         snprintf(tmp,256,"%s \x1f",r->text);
         HLH_gui_menubar_label_set(gui_bar_sample,tmp,0);

         gui_process(0);
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
         HLH_gui_element_layout(&e->window->e, e->window->e.bounds);
         HLH_gui_element_redraw(&e->window->e);
         scale_relative = e->usr;
         gui_process(0);
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
         SLK_image32 *img = NULL;
         FILE *f = image_load_select();
         if(f!=NULL)
         {
            int width,height;
            uint32_t *data = HLH_gui_image_load(f,&width,&height);
            if(data!=NULL&&width>0&&height>0)
            {
               img = malloc(sizeof(*img)+sizeof(*img->data)*width*height);
               img->w = width;
               img->h = height;
               memcpy(img->data,data,sizeof(*img->data)*width*height);
            }
            HLH_gui_image_free(data);
            fclose(f);
         }

         if(img!=NULL)
         {
            HLH_gui_imgcmp_update0(gui_imgcmp,img->data,img->w,img->h,1);
            if(gui_input!=NULL)
            {
               free(gui_input);
               gui_input = NULL;
            }
            gui_input = SLK_image32_dup(img);
            free(img);

            gui_process(0);
         }
      }
      //Preset
      else if(m->index==1)
      {
         FILE *f = preset_load_select();
         if(f!=NULL)
         {
            gui_load_preset(f);
            fclose(f);
         }
      }
      //Palette
      else if(m->index==2)
      {
         char ext[512] = {0};
         FILE *f = palette_load_select(ext);
         if(f!=NULL)
         {
            SLK_palette_load(f,dither_config.palette,&dither_config.palette_colors,ext);
            fclose(f);
            block_process = 1;
            HLH_gui_slider_set(gui.slider_color_count,dither_config.palette_colors-1,255,1,1);
            HLH_gui_slider_set(gui.slider_color_red,SLK_color32_r(dither_config.palette[color_selected]),255,1,1);
            HLH_gui_slider_set(gui.slider_color_green,SLK_color32_g(dither_config.palette[color_selected]),255,1,1);
            HLH_gui_slider_set(gui.slider_color_blue,SLK_color32_b(dither_config.palette[color_selected]),255,1,1);
            block_process = 0;
            gui_process(3);
         }
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
         if(gui_output==NULL)
            return 0;

         char ext[512] = {0};
         FILE *f = image_save_select(ext);
         HLH_gui_image_save(f,gui_output->data,gui_output->w,gui_output->h,ext);
         if(f!=NULL)
            fclose(f);

         //const char *image = image_save_select();
         //HLH_gui_image_save(image,gui_output->data,gui_output->w,gui_output->h);
      }
      //Preset
      else if(m->index==1)
      {
         FILE *f = preset_save_select();
         //const char *preset = preset_save_select();
         //FILE *f = fopen(preset,"w");
         //if(f==NULL)
            //return 0;

         HLH_json5_root *root = HLH_json_create_root();
         HLH_json_object_add_real(&root->root,"blur_amount",blur_amount);
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
         HLH_json_object_add_integer(&root->root,"tint_red",tint_red);
         HLH_json_object_add_integer(&root->root,"tint_green",tint_green);
         HLH_json_object_add_integer(&root->root,"tint_blue",tint_blue);
         HLH_json_object_add_integer(&root->root,"dither_palette_colors",dither_config.palette_colors);
         HLH_json5 array = HLH_json_create_array();
         for(int i = 0;i<256;i++)
            HLH_json_array_add_integer(&array,dither_config.palette[i]);
         HLH_json_object_add_array(&root->root,"dither_palette",array);

         HLH_json_write_file(f,&root->root);
         HLH_json_free(root);

         if(f!=NULL)
            fclose(f);
      }
      //Palette
      else if(m->index==2)
      {
         char ext[512] = {0};
         FILE *f = palette_save_select(ext);
         //const char *palette = palette_save_select();

         SLK_palette_save(f,dither_config.palette,dither_config.palette_colors,ext);
         if(f!=NULL)
            fclose(f);
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
      char buffer[512];
      if(s->e.usr==SLIDER_BLUR)
      {
         blur_amount = s->value/16.f;
         snprintf(buffer,512,"%.2f",blur_amount);
         HLH_gui_entry_set(gui.entry_blur,buffer);
         gui_process(0);
      }
      else if(s->e.usr==SLIDER_X_OFF)
      {
         x_offset = (float)s->value/500.f;
         snprintf(buffer,512,"%.2f",x_offset);
         HLH_gui_entry_set(gui.entry_x_off,buffer);
         gui_process(0);
      }
      else if(s->e.usr==SLIDER_Y_OFF)
      {
         y_offset = (float)s->value/500.f;
         snprintf(buffer,512,"%.2f",y_offset);
         HLH_gui_entry_set(gui.entry_y_off,buffer);
         gui_process(0);
      }
      else if(s->e.usr==SLIDER_WIDTH)
      {
         size_absolute_x = s->value+1;
         snprintf(buffer,512,"%d",size_absolute_x);
         HLH_gui_entry_set(gui.entry_width,buffer);
         gui_process(0);
      }
      else if(s->e.usr==SLIDER_HEIGHT)
      {
         size_absolute_y = s->value+1;
         snprintf(buffer,512,"%d",size_absolute_y);
         HLH_gui_entry_set(gui.entry_height,buffer);
         gui_process(0);
      }
      else if(s->e.usr==SLIDER_SCALE_X)
      {
         size_relative_x = s->value+1;
         snprintf(buffer,512,"%d",size_relative_x);
         HLH_gui_entry_set(gui.entry_scale_x,buffer);
         gui_process(0);
      }
      else if(s->e.usr==SLIDER_SCALE_Y)
      {
         size_relative_y = s->value+1;
         snprintf(buffer,512,"%d",size_relative_y);
         HLH_gui_entry_set(gui.entry_scale_y,buffer);
         gui_process(0);
      }
      else if(s->e.usr==SLIDER_ALPHA_THRESHOLD)
      {
         dither_config.alpha_threshold = s->value;
         snprintf(buffer,512,"%d",dither_config.alpha_threshold);
         HLH_gui_entry_set(gui.entry_alpha_threshold,buffer);
         gui_process(3);
      }
      else if(s->e.usr==SLIDER_DITHER_AMOUNT)
      {
         dither_config.dither_amount = s->value/500.f;
         snprintf(buffer,512,"%.2f",dither_config.dither_amount);
         HLH_gui_entry_set(gui.entry_dither_amount,buffer);
         gui_process(3);
      }
      else if(s->e.usr==SLIDER_PALETTE_WEIGHT)
      {
         dither_config.palette_weight = s->value;
         snprintf(buffer,512,"%d",dither_config.palette_weight);
         HLH_gui_entry_set(gui.entry_palette_weight,buffer);
         gui_process(3);
      }
      else if(s->e.usr==SLIDER_SHARP)
      {
         sharp_amount = (float)s->value/500.f;
         snprintf(buffer,512,"%.2f",sharp_amount);
         HLH_gui_entry_set(gui.entry_sharp,buffer);
         gui_process(1);
      }
      else if(s->e.usr==SLIDER_BRIGHTNESS)
      {
         brightness = (float)(s->value-250)/250.f;
         snprintf(buffer,512,"%.2f",brightness);
         HLH_gui_entry_set(gui.entry_brightness,buffer);
         gui_process(2);
      }
      else if(s->e.usr==SLIDER_CONTRAST)
      {
         contrast = (float)s->value/100.f;
         snprintf(buffer,512,"%.2f",contrast);
         HLH_gui_entry_set(gui.entry_contrast,buffer);
         gui_process(2);
      }
      else if(s->e.usr==SLIDER_SATURATION)
      {
         saturation = (float)s->value/100.f;
         snprintf(buffer,512,"%.2f",saturation);
         HLH_gui_entry_set(gui.entry_saturation,buffer);
         gui_process(2);
      }
      else if(s->e.usr==SLIDER_HUE)
      {
         hue = (float)s->value-180.f;
         snprintf(buffer,512,"%.0f",hue);
         HLH_gui_entry_set(gui.entry_hue,buffer);
         gui_process(2);
      }
      else if(s->e.usr==SLIDER_GAMMA)
      {
         gamma = (float)s->value/100.f;
         snprintf(buffer,512,"%.2f",gamma);
         HLH_gui_entry_set(gui.entry_gamma,buffer);
         gui_process(2);
      }
      else if(s->e.usr==SLIDER_COLOR_COUNT)
      {
         dither_config.palette_colors = s->value+1;
         snprintf(buffer,512,"%d",dither_config.palette_colors);
         HLH_gui_entry_set(gui.entry_color_count,buffer);
         HLH_gui_element_redraw(&gui.group_palette->e);
         gui_process(3);
      }
      else if(s->e.usr==SLIDER_COLOR_RED)
      {
         uint32_t c = dither_config.palette[color_selected];
         dither_config.palette[color_selected] = ((uint32_t)s->value)|(SLK_color32_g(c)<<8)|(SLK_color32_b(c)<<16)|(SLK_color32_a(c)<<24);
         snprintf(buffer,512,"%d",s->value);
         HLH_gui_entry_set(gui.entry_color_red,buffer);

         if(c!=dither_config.palette[color_selected])
         {
            HLH_gui_element_redraw(&gui.group_palette->e);
            gui_process(3);
         }
      }
      else if(s->e.usr==SLIDER_COLOR_GREEN)
      {
         uint32_t c = dither_config.palette[color_selected];
         dither_config.palette[color_selected] = (SLK_color32_r(c))|((uint32_t)s->value<<8)|(SLK_color32_b(c)<<16)|(SLK_color32_a(c)<<24);
         snprintf(buffer,512,"%d",s->value);
         HLH_gui_entry_set(gui.entry_color_green,buffer);

         if(c!=dither_config.palette[color_selected])
         {
            HLH_gui_element_redraw(&gui.group_palette->e);
            gui_process(3);
         }
      }
      else if(s->e.usr==SLIDER_COLOR_BLUE)
      {
         uint32_t c = dither_config.palette[color_selected];
         dither_config.palette[color_selected] = (SLK_color32_r(c))|(SLK_color32_g(c)<<8)|((uint32_t)s->value<<16)|(SLK_color32_a(c)<<24);
         snprintf(buffer,512,"%d",s->value);
         HLH_gui_entry_set(gui.entry_color_blue,buffer);

         if(c!=dither_config.palette[color_selected])
         {
            HLH_gui_element_redraw(&gui.group_palette->e);
            gui_process(3);
         }
      }
      else if(s->e.usr==SLIDER_TINT_RED)
      {
         tint_red = s->value;
         snprintf(buffer,512,"%d",s->value);
         HLH_gui_entry_set(gui.entry_tint_red,buffer);
         gui_process(2);
      }
      else if(s->e.usr==SLIDER_TINT_GREEN)
      {
         tint_green = s->value;
         snprintf(buffer,512,"%d",s->value);
         HLH_gui_entry_set(gui.entry_tint_green,buffer);
         gui_process(2);
      }
      else if(s->e.usr==SLIDER_TINT_BLUE)
      {
         tint_blue = s->value;
         snprintf(buffer,512,"%d",s->value);
         HLH_gui_entry_set(gui.entry_tint_blue,buffer);
         gui_process(2);
      }
   }

   return 0;
}

static int entry_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_entry *entry = (HLH_gui_entry *)e;
   if(msg==HLH_GUI_MSG_TEXTINPUT_END)
   {
      if(entry->e.usr==ENTRY_BLUR)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_blur,(int)(value*16.f),512,1,1);
      }
      else if(entry->e.usr==ENTRY_X_OFF)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_x_off,(int)(value*500.f),500,1,1);
      }
      else if(entry->e.usr==ENTRY_Y_OFF)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_y_off,(int)(value*500.f),500,1,1);
      }
      else if(entry->e.usr==ENTRY_WIDTH)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_width,value-1,511,1,1);
      }
      else if(entry->e.usr==ENTRY_HEIGHT)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_height,value-1,511,1,1);
      }
      else if(entry->e.usr==ENTRY_SCALE_X)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_scale_x,value-1,31,1,1);
      }
      else if(entry->e.usr==ENTRY_SCALE_Y)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_scale_y,value-1,31,1,1);
      }
      else if(entry->e.usr==ENTRY_SHARP)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_sharp,(int)(value*500.f),500,1,1);
      }
      else if(entry->e.usr==ENTRY_ALPHA_THRESHOLD)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_alpha_threshold,value,255,1,1);
      }
      else if(entry->e.usr==ENTRY_DITHER_AMOUNT)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_dither_amount,(int)(value*500.f),500,1,1);
      }
      else if(entry->e.usr==ENTRY_PALETTE_WEIGHT)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_palette_weight,value,16,1,1);
      }
      else if(entry->e.usr==ENTRY_COLOR_COUNT)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_color_count,value-1,255,1,1);
      }
      else if(entry->e.usr==ENTRY_COLOR_RED)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_color_red,value,255,1,1);
      }
      else if(entry->e.usr==ENTRY_COLOR_GREEN)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_color_green,value,255,1,1);
      }
      else if(entry->e.usr==ENTRY_COLOR_BLUE)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_color_blue,value,255,1,1);
      }
      else if(entry->e.usr==ENTRY_TINT_RED)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_tint_red,value,255,1,1);
      }
      else if(entry->e.usr==ENTRY_TINT_GREEN)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_tint_green,value,255,1,1);
      }
      else if(entry->e.usr==ENTRY_TINT_BLUE)
      {
         int value = strtol(entry->entry,NULL,10);
         HLH_gui_slider_set(gui.slider_tint_blue,value,255,1,1);
      }
      else if(entry->e.usr==ENTRY_BRIGHTNESS)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_brightness,(int)(value*250.f+250.f),500,1,1);
      }
      else if(entry->e.usr==ENTRY_CONTRAST)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_contrast,(int)(value*100.f),500,1,1);
      }
      else if(entry->e.usr==ENTRY_SATURATION)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_saturation,(int)(value*100.f),500,1,1);
      }
      else if(entry->e.usr==ENTRY_HUE)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_hue,(int)(value+180.f),360,1,1);
      }
      else if(entry->e.usr==ENTRY_GAMMA)
      {
         float value = strtof(entry->entry,NULL);
         HLH_gui_slider_set(gui.slider_gamma,(int)(value*100.f),500,1,1);
      }
   }

   return 0;
}

static int button_add_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      if(button->e.usr==BUTTON_BLUR)
         HLH_gui_slider_set(gui.slider_blur,(int)((blur_amount+0.25f)*16.f),512,1,1);
      else if(button->e.usr==BUTTON_X_OFF)
         HLH_gui_slider_set(gui.slider_x_off,(int)((x_offset+0.1f)*500.f),500,1,1);
      else if(button->e.usr==BUTTON_Y_OFF)
         HLH_gui_slider_set(gui.slider_y_off,(int)((y_offset+0.1f)*500.f),500,1,1);
      else if(button->e.usr==BUTTON_WIDTH)
         HLH_gui_slider_set(gui.slider_width,size_absolute_x+8-1,511,1,1);
      else if(button->e.usr==BUTTON_HEIGHT)
         HLH_gui_slider_set(gui.slider_height,size_absolute_y+8-1,511,1,1);
      else if(button->e.usr==BUTTON_SCALE_X)
         HLH_gui_slider_set(gui.slider_scale_x,size_relative_x+1-1,31,1,1);
      else if(button->e.usr==BUTTON_SCALE_Y)
         HLH_gui_slider_set(gui.slider_scale_y,size_relative_y+1-1,31,1,1);
      else if(button->e.usr==BUTTON_SHARP)
         HLH_gui_slider_set(gui.slider_sharp,(int)((sharp_amount+0.1f)*500.f),500,1,1);
      else if(button->e.usr==BUTTON_ALPHA_THRESHOLD)
         HLH_gui_slider_set(gui.slider_alpha_threshold,dither_config.alpha_threshold+8,255,1,1);
      else if(button->e.usr==BUTTON_DITHER_AMOUNT)
         HLH_gui_slider_set(gui.slider_dither_amount,(int)((dither_config.dither_amount+0.1f)*500.f),500,1,1);
      else if(button->e.usr==BUTTON_PALETTE_WEIGHT)
         HLH_gui_slider_set(gui.slider_palette_weight,dither_config.palette_weight+1,16,1,1);
      else if(button->e.usr==BUTTON_COLOR_COUNT)
         HLH_gui_slider_set(gui.slider_color_count,dither_config.palette_colors+4-1,255,1,1);
      else if(button->e.usr==BUTTON_COLOR_RED)
         HLH_gui_slider_set(gui.slider_color_red,SLK_color32_r(dither_config.palette[color_selected])+4,255,1,1);
      else if(button->e.usr==BUTTON_COLOR_GREEN)
         HLH_gui_slider_set(gui.slider_color_green,SLK_color32_g(dither_config.palette[color_selected])+4,255,1,1);
      else if(button->e.usr==BUTTON_COLOR_BLUE)
         HLH_gui_slider_set(gui.slider_color_blue,SLK_color32_b(dither_config.palette[color_selected])+4,255,1,1);
      else if(button->e.usr==BUTTON_TINT_RED)
         HLH_gui_slider_set(gui.slider_tint_red,tint_red+4,255,1,1);
      else if(button->e.usr==BUTTON_TINT_GREEN)
         HLH_gui_slider_set(gui.slider_tint_green,tint_green+4,255,1,1);
      else if(button->e.usr==BUTTON_TINT_BLUE)
         HLH_gui_slider_set(gui.slider_tint_blue,tint_blue+4,255,1,1);
      else if(button->e.usr==BUTTON_BRIGHTNESS)
         HLH_gui_slider_set(gui.slider_brightness,(int)((brightness+0.1f)*250.f+250.f),500,1,1);
      else if(button->e.usr==BUTTON_CONTRAST)
         HLH_gui_slider_set(gui.slider_contrast,(int)((contrast+0.1f)*100.f),500,1,1);
      else if(button->e.usr==BUTTON_SATURATION)
         HLH_gui_slider_set(gui.slider_saturation,(int)((saturation+0.1f)*100.f),500,1,1);
      else if(button->e.usr==BUTTON_HUE)
         HLH_gui_slider_set(gui.slider_hue,(int)((hue+10.f)+180.f),360,1,1);
      else if(button->e.usr==BUTTON_GAMMA)
         HLH_gui_slider_set(gui.slider_gamma,(int)((gamma+0.1f)*100.f),500,1,1);
   }

   return 0;
}

static int button_sub_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      if(button->e.usr==BUTTON_BLUR)
         HLH_gui_slider_set(gui.slider_blur,(int)((blur_amount-0.25f)*16.f),512,1,1);
      else if(button->e.usr==BUTTON_X_OFF)
         HLH_gui_slider_set(gui.slider_x_off,(int)((x_offset-0.1f)*500.f),500,1,1);
      else if(button->e.usr==BUTTON_Y_OFF)
         HLH_gui_slider_set(gui.slider_y_off,(int)((y_offset-0.1f)*500.f),500,1,1);
      else if(button->e.usr==BUTTON_WIDTH)
         HLH_gui_slider_set(gui.slider_width,size_absolute_x-8-1,511,1,1);
      else if(button->e.usr==BUTTON_HEIGHT)
         HLH_gui_slider_set(gui.slider_height,size_absolute_y-8-1,511,1,1);
      else if(button->e.usr==BUTTON_SCALE_X)
         HLH_gui_slider_set(gui.slider_scale_x,size_relative_x-1-1,31,1,1);
      else if(button->e.usr==BUTTON_SCALE_Y)
         HLH_gui_slider_set(gui.slider_scale_y,size_relative_y-1-1,31,1,1);
      else if(button->e.usr==BUTTON_SHARP)
         HLH_gui_slider_set(gui.slider_sharp,(int)((sharp_amount-0.1f)*500.f),500,1,1);
      else if(button->e.usr==BUTTON_ALPHA_THRESHOLD)
         HLH_gui_slider_set(gui.slider_alpha_threshold,dither_config.alpha_threshold-8,255,1,1);
      else if(button->e.usr==BUTTON_DITHER_AMOUNT)
         HLH_gui_slider_set(gui.slider_dither_amount,(int)((dither_config.dither_amount-0.1f)*500.f),500,1,1);
      else if(button->e.usr==BUTTON_PALETTE_WEIGHT)
         HLH_gui_slider_set(gui.slider_palette_weight,dither_config.palette_weight-1,16,1,1);
      else if(button->e.usr==BUTTON_COLOR_COUNT)
         HLH_gui_slider_set(gui.slider_color_count,dither_config.palette_colors-4-1,255,1,1);
      else if(button->e.usr==BUTTON_COLOR_RED)
         HLH_gui_slider_set(gui.slider_color_red,SLK_color32_r(dither_config.palette[color_selected])-4,255,1,1);
      else if(button->e.usr==BUTTON_COLOR_GREEN)
         HLH_gui_slider_set(gui.slider_color_green,SLK_color32_g(dither_config.palette[color_selected])-4,255,1,1);
      else if(button->e.usr==BUTTON_COLOR_BLUE)
         HLH_gui_slider_set(gui.slider_color_blue,SLK_color32_b(dither_config.palette[color_selected])-4,255,1,1);
      else if(button->e.usr==BUTTON_TINT_RED)
         HLH_gui_slider_set(gui.slider_tint_red,tint_red-4,255,1,1);
      else if(button->e.usr==BUTTON_TINT_GREEN)
         HLH_gui_slider_set(gui.slider_tint_green,tint_green-4,255,1,1);
      else if(button->e.usr==BUTTON_TINT_BLUE)
         HLH_gui_slider_set(gui.slider_tint_blue,tint_blue-4,255,1,1);
      else if(button->e.usr==BUTTON_BRIGHTNESS)
         HLH_gui_slider_set(gui.slider_brightness,(int)((brightness-0.1f)*250.f+250.f),500,1,1);
      else if(button->e.usr==BUTTON_CONTRAST)
         HLH_gui_slider_set(gui.slider_contrast,(int)((contrast-0.1f)*100.f),500,1,1);
      else if(button->e.usr==BUTTON_SATURATION)
         HLH_gui_slider_set(gui.slider_saturation,(int)((saturation-0.1f)*100.f),500,1,1);
      else if(button->e.usr==BUTTON_HUE)
         HLH_gui_slider_set(gui.slider_hue,(int)((hue-10.f)+180.f),360,1,1);
      else if(button->e.usr==BUTTON_GAMMA)
         HLH_gui_slider_set(gui.slider_gamma,(int)((gamma-0.1f)*100.f),500,1,1);
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
            gui_process(3);
         }
         else
         {
            HLH_gui_element_ignore(&gui_group_kmeans->e,1);
            dither_config.use_kmeans = 0;
            gui_process(3);
         }

         HLH_gui_element_layout(&e->window->e, e->window->e.bounds);
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
         snprintf(tmp,256,"%s \x1f",r->text);
         HLH_gui_menubar_label_set(gui_bar_dither,tmp,0);

         gui_process(3);
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
         snprintf(tmp,256,"%s \x1f",r->text);
         HLH_gui_menubar_label_set(gui_bar_distance,tmp,0);

         gui_process(3);
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
      SLK_image32_kmeans(gui_input,dither_config.palette,dither_config.palette_colors,time(NULL),kmeanspp);
      HLH_gui_element_redraw(&gui.group_palette->e);
      gui_process(3);
   }

   return 0;
}

static void gui_process(int from)
{
   if(gui_input==NULL||block_process)
      return;
   if(gui_output!=NULL)
   {
      free(gui_output);
      gui_output = NULL;
   }

   if(from<=0||cache_sample==NULL)
   {
      if(cache_sample!=NULL)
      {
         free(cache_sample);
         cache_sample = NULL;
      }

      SLK_image64 *img = SLK_image64_dup32(gui_input);
      SLK_image64_blur(img,blur_amount);

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
      cache_sample = SLK_image64_sample(img,width,height,sample_mode,x_offset,y_offset);
      free(img);
   }

   if(from<=1||cache_sharp==NULL)
   {
      if(cache_sharp!=NULL)
      {
         free(cache_sharp);
         cache_sharp = NULL;
      }

      cache_sharp = SLK_image64_dup(cache_sample);

      SLK_image64_sharpen(cache_sharp,sharp_amount);
   }

   if(from<=2||cache_tint==NULL)
   {
      if(cache_tint!=NULL)
      {
         free(cache_tint);
         cache_tint = NULL;
      }

      cache_tint = SLK_image64_dup(cache_sharp);
      SLK_image64_hscb(cache_tint,hue,saturation,contrast,brightness);
      SLK_image64_gamma(cache_tint,gamma);
      SLK_image64_tint(cache_tint,tint_red,tint_green,tint_blue);
   }

   SLK_image64 *dither_input = SLK_image64_dup(cache_tint);
   gui_output = SLK_image64_dither(dither_input,&dither_config);
   free(dither_input);
   HLH_gui_imgcmp_update1(gui_imgcmp,gui_output->data,gui_output->w,gui_output->h,1);
}

void gui_load_preset(FILE *f)
{
   block_process = 1;
   if(f!=NULL)
   {
      HLH_json5 fallback = {0};
      HLH_json5_root *root = HLH_json_parse_file_stream(f);

      blur_amount = HLH_json_get_object_real(&root->root,"blur_amount",0.);
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
      tint_red = HLH_json_get_object_integer(&root->root,"tint_red",255);
      tint_green = HLH_json_get_object_integer(&root->root,"tint_green",255);
      tint_blue = HLH_json_get_object_integer(&root->root,"tint_blue",255);
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

      color_selected = 0;
   }
   else
   {
      blur_amount = 0.;
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
      tint_red = 255;
      tint_green = 255;
      tint_blue = 255;
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

   HLH_gui_slider_set(gui.slider_blur,(int)(blur_amount*16.f),512,1,1);
   HLH_gui_slider_set(gui.slider_x_off,(int)(x_offset*500.f),500,1,1);
   HLH_gui_slider_set(gui.slider_y_off,(int)(y_offset*500.f),500,1,1);
   HLH_gui_slider_set(gui.slider_width,size_absolute_x-1,511,1,1);
   HLH_gui_slider_set(gui.slider_height,size_absolute_y-1,511,1,1);
   HLH_gui_slider_set(gui.slider_scale_x,size_relative_x-1,31,1,1);
   HLH_gui_slider_set(gui.slider_scale_y,size_relative_y-1,31,1,1);
   HLH_gui_slider_set(gui.slider_sharp,(int)(sharp_amount*500.f),500,1,1);
   HLH_gui_slider_set(gui.slider_brightness,(int)(brightness*250.f+250.f),500,1,1);
   HLH_gui_slider_set(gui.slider_contrast,(int)(contrast*100.f),500,1,1);
   HLH_gui_slider_set(gui.slider_saturation,(int)(saturation*100.f),500,1,1);
   HLH_gui_slider_set(gui.slider_hue,(int)(hue+180.f),360,1,1);
   HLH_gui_slider_set(gui.slider_gamma,(int)(gamma*100.f),500,1,1);
   HLH_gui_slider_set(gui.slider_alpha_threshold,dither_config.alpha_threshold,255,1,1);
   HLH_gui_slider_set(gui.slider_dither_amount,(int)(dither_config.dither_amount*500.f),500,1,1);
   HLH_gui_slider_set(gui.slider_palette_weight,dither_config.palette_weight,16,1,1);
   HLH_gui_slider_set(gui.slider_color_count,dither_config.palette_colors-1,255,1,1);
   HLH_gui_slider_set(gui.slider_tint_red,tint_red,255,1,1);
   HLH_gui_slider_set(gui.slider_tint_green,tint_green,255,1,1);
   HLH_gui_slider_set(gui.slider_tint_blue,tint_blue,255,1,1);

   HLH_gui_radiobutton_set(gui.sample_sample_mode[sample_mode],1,1);
   HLH_gui_radiobutton_set(gui.sample_scale_mode[scale_relative],1,1);
   HLH_gui_radiobutton_set(gui.dither_dither_mode[dither_config.dither_mode],1,1);
   HLH_gui_radiobutton_set(gui.dither_color_dist[dither_config.color_dist],1,1);
   HLH_gui_radiobutton_set(gui.palette_colors[color_selected],1,1);

   HLH_gui_checkbutton_set(gui.dither_kmeans,dither_config.use_kmeans,1,1);
   HLH_gui_checkbutton_set(gui.palette_kmeanspp,kmeanspp,1,1);

   block_process = 0;
   gui_process(0);
}
//-------------------------------------
