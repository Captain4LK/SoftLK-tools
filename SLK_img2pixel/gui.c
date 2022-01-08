/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <SLK/SLK.h>
#include <SLK/SLK_gui.h>
//-------------------------------------

//Internal includes
#include "assets.h"
#include "gui.h"
#include "image2pixel.h"
#include "utility.h"
//-------------------------------------

//#defines
#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))

#define BUTTON_BAR_PLUS(a,b) \
   if((a)->button.state.pressed&&(b)->slider.value<(b)->slider.max) \
      (b)->slider.value++

#define BUTTON_BAR_MINUS(a,b) \
   if((a)->button.state.pressed&&(b)->slider.value>(b)->slider.min) \
      (b)->slider.value--

#define BAR_UPDATE(get,set,bar,label) \
   if((get)()!=(bar)->slider.value) \
   { \
      (set)((bar)->slider.value); \
      char ctmp[16]; \
      sprintf(ctmp,"%d",(get)()); \
      SLK_gui_label_set_text((label),ctmp); \
      update = 1; \
   }
//-------------------------------------

//Typedefs
//Hide in struct to not pollute namespace
struct Elements
{
   //Save/Load tab
   SLK_gui_element *save_load;
   SLK_gui_element *save_save;
   SLK_gui_element *save_load_preset;
   SLK_gui_element *save_save_preset;
   SLK_gui_element *save_load_folder;
   SLK_gui_element *save_save_folder;
   SLK_gui_element *save_upscale_plus;
   SLK_gui_element *save_upscale_minus;
   SLK_gui_element *save_bar_upscale;
   SLK_gui_element *save_label_upscale;

   //Palette tab
   SLK_gui_element *palette_save;
   SLK_gui_element *palette_load;
   SLK_gui_element *palette_generate;
   SLK_gui_element *palette_palette;
   SLK_RGB_sprite *palette_sprite;
   SLK_gui_element *palette_button;
   SLK_gui_element *palette_bar_r;
   SLK_gui_element *palette_bar_g;
   SLK_gui_element *palette_bar_b;
   SLK_gui_element *palette_bar_colors;
   SLK_gui_element *palette_plus_r;
   SLK_gui_element *palette_minus_r;
   SLK_gui_element *palette_plus_g;
   SLK_gui_element *palette_minus_g;
   SLK_gui_element *palette_plus_b;
   SLK_gui_element *palette_minus_b;
   SLK_gui_element *palette_plus_colors;
   SLK_gui_element *palette_minus_colors;
   SLK_gui_element *palette_label_r;
   SLK_gui_element *palette_label_g;
   SLK_gui_element *palette_label_b;
   SLK_gui_element *palette_label_colors;

   //Sample tab
   SLK_gui_element *sample_tab_scale;
   SLK_gui_element *sample_width_plus;
   SLK_gui_element *sample_width_minus;
   SLK_gui_element *sample_height_plus;
   SLK_gui_element *sample_height_minus;
   SLK_gui_element *sample_swidth_plus;
   SLK_gui_element *sample_swidth_minus;
   SLK_gui_element *sample_sheight_plus;
   SLK_gui_element *sample_sheight_minus;
   SLK_gui_element *sample_offset_x_plus;
   SLK_gui_element *sample_offset_x_minus;
   SLK_gui_element *sample_offset_y_plus;
   SLK_gui_element *sample_offset_y_minus;
   SLK_gui_element *sample_gauss_plus;
   SLK_gui_element *sample_gauss_minus;
   SLK_gui_element *sample_sample_left;
   SLK_gui_element *sample_sample_right;
   SLK_gui_element *sample_bar_width;
   SLK_gui_element *sample_bar_height;
   SLK_gui_element *sample_bar_swidth;
   SLK_gui_element *sample_bar_sheight;
   SLK_gui_element *sample_bar_offset_x;
   SLK_gui_element *sample_bar_offset_y;
   SLK_gui_element *sample_bar_gauss;
   SLK_gui_element *sample_label_width;
   SLK_gui_element *sample_label_height;
   SLK_gui_element *sample_label_swidth;
   SLK_gui_element *sample_label_sheight;
   SLK_gui_element *sample_label_sample;
   SLK_gui_element *sample_label_offset_x;
   SLK_gui_element *sample_label_offset_y;
   SLK_gui_element *sample_label_gauss;

   //Colors tab
   SLK_gui_element *color_palette;
   SLK_gui_element *color_button;
   SLK_gui_element *color_check_inline;
   SLK_gui_element *color_check_outline;
   SLK_gui_element *color_button_inline;
   SLK_gui_element *color_button_outline;
   SLK_gui_element *color_alpha_plus;
   SLK_gui_element *color_alpha_minus;
   SLK_gui_element *color_weight_plus;
   SLK_gui_element *color_weight_minus;
   SLK_gui_element *color_dither_left;
   SLK_gui_element *color_dither_right;
   SLK_gui_element *color_dither_plus;
   SLK_gui_element *color_dither_minus;
   SLK_gui_element *color_space_left;
   SLK_gui_element *color_space_right;
   SLK_gui_element *color_bar_dither;
   SLK_gui_element *color_bar_alpha;
   SLK_gui_element *color_bar_weight;
   SLK_gui_element *color_label_dither;
   SLK_gui_element *color_label_alpha;
   SLK_gui_element *color_label_weight;
   SLK_gui_element *color_label_dither_amount;
   SLK_gui_element *color_label_space;

   //Process tab
   SLK_gui_element *process_bar_brightness;
   SLK_gui_element *process_minus_brightness;
   SLK_gui_element *process_plus_brightness;
   SLK_gui_element *process_label_brightness;
   SLK_gui_element *process_bar_contrast;
   SLK_gui_element *process_minus_contrast;
   SLK_gui_element *process_plus_contrast;
   SLK_gui_element *process_label_contrast;
   SLK_gui_element *process_bar_saturation;
   SLK_gui_element *process_minus_saturation;
   SLK_gui_element *process_plus_saturation;
   SLK_gui_element *process_label_saturation;
   SLK_gui_element *process_bar_gamma;
   SLK_gui_element *process_minus_gamma;
   SLK_gui_element *process_plus_gamma;
   SLK_gui_element *process_label_gamma;
   SLK_gui_element *process_bar_sharpen;
   SLK_gui_element *process_minus_sharpen;
   SLK_gui_element *process_plus_sharpen;
   SLK_gui_element *process_label_sharpen;
   SLK_gui_element *process_bar_hue;
   SLK_gui_element *process_minus_hue;
   SLK_gui_element *process_plus_hue;
   SLK_gui_element *process_label_hue;

   //Special tab
   SLK_gui_element *special_gif_load;
   SLK_gui_element *special_gif_save;
};
//-------------------------------------

//Variables
//Lots of globals here, I need to find a way to manage this better
static SLK_RGB_sprite *sprite_in = NULL;
static SLK_RGB_sprite *sprite_in_org = NULL;
static SLK_RGB_sprite *sprite_out = NULL;
static int palette_selected = 0;
static SLK_gui_window *preview = NULL;
static SLK_gui_element *preview_tabs = NULL;
static SLK_gui_window *settings = NULL;
static SLK_gui_element *settings_tabs = NULL;
static struct Elements elements = {0};
static int gui_in_x = 0;
static int gui_in_y = 0;
static int gui_out_x = 0;
static int gui_out_y = 0;

static const char *text_space[] = 
{
   "RGB",
   "CIE76",
   "CIE94",
   "CIEDE2000",
   "XYZ",
   "YCC",
   "YIQ",
   "YUV",
   "K-Means",
};

static const char *text_dither[] = 
{
   "No dithering",
   "Bayer 8x8",
   "Bayer 4x4",
   "Bayer 2x2",
   "Cluster 8x8",
   "Cluster 4x4",
   "FloydSteinberg 1",
   "FloydSteinberg 2",
};

static const char *text_sample[] = 
{
   "Round",
   "Floor",
   "Ceil",
   "Bilinear",
   "Bicubic",
   "Lanczos",
};

static const char *text_tab_image[] = 
{
   "Input",
   "Output",
};

static const char *text_tab_scale[] = 
{
   "Absoulte",
   "Relative",
};

static const char *text_tab_settings[] = 
{
   "Save/Load",
   "Palette",
   "Sample",
   "Colors",
   "Process",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "Special",
};
//-------------------------------------

//Function prototypes
static void gui_buttons();
static void gui_draw();
static void update_output();
static void palette_draw();
static void palette_labels();
static void preset_load(FILE *f);
//-------------------------------------

//Function implementations

void gui_init()
{
   //Load entry palette and font
   assets_init();
   img2pixel_set_palette(assets_load_pal_default());

   //Setup windows
   //Preview window
   SLK_gui_element *label;
   SLK_gui_set_colors(SLK_color_create(90,90,90,255),SLK_color_create(200,200,200,255),SLK_color_create(100,100,100,255),SLK_color_create(50,50,50,255),SLK_color_create(0,0,0,255));
   SLK_gui_set_font(font);
   SLK_gui_set_font_color(SLK_color_create(0,0,0,255));
   preview = SLK_gui_window_create(400,100,260,286);
   SLK_gui_window_set_title(preview,"Preview");
   SLK_gui_window_set_moveable(preview,1);
   preview_tabs = SLK_gui_tabbar_create(2,14,256,14,2,text_tab_image);
   SLK_gui_window_add_element(preview,preview_tabs);
   //-------------------------------------
   
   //Gui window
   SLK_RGB_sprite *tmp = SLK_rgb_sprite_create(1,1);
   settings = SLK_gui_window_create(10,10,384,296);
   SLK_gui_window_set_title(settings,"Settings");
   SLK_gui_window_set_moveable(settings,1);
   settings_tabs = SLK_gui_vtabbar_create(2,14,96,20,text_tab_settings);
   SLK_gui_window_add_element(settings,settings_tabs);

   //Save/Load tab
   elements.save_load = SLK_gui_button_create(158,32,164,14,"Load image");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load);
   elements.save_save = SLK_gui_button_create(158,64,164,14,"Save image");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save);
   elements.save_load_preset = SLK_gui_button_create(158,112,164,14,"Load preset");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load_preset);
   elements.save_save_preset = SLK_gui_button_create(158,144,164,14,"Save preset");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save_preset);
   elements.save_load_folder = SLK_gui_button_create(158,192,164,14,"Select input dir");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load_folder);
   elements.save_save_folder = SLK_gui_button_create(158,224,164,14,"Select output dir");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save_folder);
   label = SLK_gui_label_create(100,278,64,12,"Scale");
   SLK_gui_vtabbar_add_element(settings_tabs,0,label);
   elements.save_upscale_plus = SLK_gui_button_create(344,275,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_upscale_plus);
   elements.save_upscale_minus = SLK_gui_button_create(160,275,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_upscale_minus);
   elements.save_bar_upscale = SLK_gui_slider_create(174,275,170,14,1,16);
   elements.save_bar_upscale->slider.value = 1;
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_bar_upscale);
   elements.save_label_upscale = SLK_gui_label_create(354,278,32,12,"1");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_label_upscale);

   //Palette tab
   elements.palette_load = SLK_gui_button_create(158,221,164,14,"Load palette");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_load);
   elements.palette_save = SLK_gui_button_create(158,245,164,14,"Save palette");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_save);
   elements.palette_generate = SLK_gui_button_create(158,269,164,14,"Generate palette");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_generate);
   elements.palette_sprite = SLK_rgb_sprite_create(279,81);
   elements.palette_palette = SLK_gui_image_create(100,15,279,81,elements.palette_sprite,(SLK_gui_rectangle){0,0,279,81});
   palette_draw();
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_palette);
   elements.palette_button = SLK_gui_icon_create(100,15,279,81,tmp,(SLK_gui_rectangle){0,0,1,1},(SLK_gui_rectangle){0,0,1,1});
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_button);
   elements.palette_minus_r = SLK_gui_button_create(160,109,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_minus_r);
   elements.palette_plus_r = SLK_gui_button_create(344,109,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_plus_r);
   elements.palette_minus_g = SLK_gui_button_create(160,125,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_minus_g);
   elements.palette_plus_g = SLK_gui_button_create(344,125,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_plus_g);
   elements.palette_minus_b = SLK_gui_button_create(160,141,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_minus_b);
   elements.palette_plus_b = SLK_gui_button_create(344,141,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_plus_b);
   elements.palette_minus_colors = SLK_gui_button_create(160,157,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_minus_colors);
   elements.palette_plus_colors = SLK_gui_button_create(344,157,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_plus_colors);
   elements.palette_bar_r = SLK_gui_slider_create(174,109,170,14,0,255);
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_bar_r);
   elements.palette_bar_g = SLK_gui_slider_create(174,125,170,14,0,255);
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_bar_g);
   elements.palette_bar_b = SLK_gui_slider_create(174,141,170,14,0,255);
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_bar_b);
   elements.palette_bar_colors = SLK_gui_slider_create(174,157,170,14,1,256);
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_bar_colors);
   label = SLK_gui_label_create(104,112,48,12,"red");
   SLK_gui_vtabbar_add_element(settings_tabs,1,label);
   label = SLK_gui_label_create(104,128,48,12,"green");
   SLK_gui_vtabbar_add_element(settings_tabs,1,label);
   label = SLK_gui_label_create(104,144,48,12,"blue");
   SLK_gui_vtabbar_add_element(settings_tabs,1,label);
   label = SLK_gui_label_create(100,160,56,12,"colors");
   SLK_gui_vtabbar_add_element(settings_tabs,1,label);
   elements.palette_label_r = SLK_gui_label_create(354,112,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_label_r);
   elements.palette_label_g = SLK_gui_label_create(354,128,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_label_g);
   elements.palette_label_b = SLK_gui_label_create(354,144,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_label_b);
   elements.palette_label_colors = SLK_gui_label_create(354,160,32,12,"256");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_label_colors);
   palette_labels();

   //Sample tab
   elements.sample_tab_scale = SLK_gui_tabbar_create(99,14,283,14,2,text_tab_scale);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_tab_scale);
   elements.sample_width_plus = SLK_gui_button_create(344,35,14,14,"+");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,elements.sample_width_plus);
   elements.sample_width_minus = SLK_gui_button_create(160,35,14,14,"-");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,elements.sample_width_minus);
   elements.sample_height_plus = SLK_gui_button_create(344,67,14,14,"+");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,elements.sample_height_plus);
   elements.sample_height_minus = SLK_gui_button_create(160,67,14,14,"-");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,elements.sample_height_minus);
   label = SLK_gui_label_create(104,38,48,12,"Width");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,label);
   label = SLK_gui_label_create(104,70,56,12,"Height");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,label);
   elements.sample_bar_width = SLK_gui_slider_create(174,35,170,14,0,512);
   elements.sample_bar_width->slider.value = 128;
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,elements.sample_bar_width);
   elements.sample_bar_height = SLK_gui_slider_create(174,67,170,14,0,512);;
   elements.sample_bar_height->slider.value = 128;
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,elements.sample_bar_height);
   elements.sample_label_width = SLK_gui_label_create(354,38,32,12,"128");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,elements.sample_label_width);
   elements.sample_label_height = SLK_gui_label_create(354,70,32,12,"128");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,0,elements.sample_label_height);
   elements.sample_swidth_plus = SLK_gui_button_create(344,35,14,14,"+");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,elements.sample_swidth_plus);
   elements.sample_swidth_minus = SLK_gui_button_create(160,35,14,14,"-");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,elements.sample_swidth_minus);
   elements.sample_sheight_plus = SLK_gui_button_create(344,67,14,14,"+");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,elements.sample_sheight_plus);
   elements.sample_sheight_minus = SLK_gui_button_create(160,67,14,14,"-");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,elements.sample_sheight_minus);
   label = SLK_gui_label_create(104,38,48,12,"x pix");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,label);
   label = SLK_gui_label_create(104,70,48,12,"y pix");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,label);
   elements.sample_bar_swidth = SLK_gui_slider_create(174,35,170,14,1,32);
   elements.sample_bar_swidth->slider.value = 2;
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,elements.sample_bar_swidth);
   elements.sample_bar_sheight = SLK_gui_slider_create(174,67,170,14,1,32);;
   elements.sample_bar_sheight->slider.value = 2;
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,elements.sample_bar_sheight);
   elements.sample_label_swidth = SLK_gui_label_create(354,38,32,12,"2");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,elements.sample_label_swidth);
   elements.sample_label_sheight = SLK_gui_label_create(354,70,32,12,"2");
   SLK_gui_tabbar_add_element(elements.sample_tab_scale,1,elements.sample_label_sheight);

   label = SLK_gui_label_create(104,118,56,12,"Sample");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.sample_sample_left = SLK_gui_button_create(160,115,14,14,"<");;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_sample_left);
   elements.sample_sample_right = SLK_gui_button_create(344,115,14,14,">");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_sample_right);
   elements.sample_label_sample = SLK_gui_label_create(174,118,170,12,text_sample[0]);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_label_sample);

   label = SLK_gui_label_create(104,150,56,12,"x off");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.sample_bar_offset_x = SLK_gui_slider_create(174,147,170,14,0,100);
   elements.sample_bar_offset_x->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_bar_offset_x);
   elements.sample_label_offset_x = SLK_gui_label_create(354,150,32,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_label_offset_x);
   elements.sample_offset_x_plus = SLK_gui_button_create(344,147,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_offset_x_plus);
   elements.sample_offset_x_minus = SLK_gui_button_create(160,147,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_offset_x_minus);

   label = SLK_gui_label_create(104,182,56,12,"y off");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.sample_bar_offset_y = SLK_gui_slider_create(174,179,170,14,0,100);
   elements.sample_bar_offset_y->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_bar_offset_y);
   elements.sample_label_offset_y = SLK_gui_label_create(354,182,32,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_label_offset_y);
   elements.sample_offset_y_plus = SLK_gui_button_create(344,179,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_offset_y_plus);
   elements.sample_offset_y_minus = SLK_gui_button_create(160,179,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_offset_y_minus);

   label = SLK_gui_label_create(104,214,56,12,"Gauss");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.sample_gauss_plus = SLK_gui_button_create(344,211,14,14,"+");
   SLK_gui_tabbar_add_element(settings_tabs,2,elements.sample_gauss_plus);
   elements.sample_gauss_minus = SLK_gui_button_create(160,211,14,14,"-");
   SLK_gui_tabbar_add_element(settings_tabs,2,elements.sample_gauss_minus);
   elements.sample_bar_gauss = SLK_gui_slider_create(174,211,170,14,0,500);
   elements.sample_bar_gauss->slider.value = 80;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_bar_gauss);
   elements.sample_label_gauss = SLK_gui_label_create(354,214,32,12,"80");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.sample_label_gauss);

   //Colors tab
   elements.color_dither_left = SLK_gui_button_create(160,109,14,14,"<");;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_dither_left);
   elements.color_dither_right = SLK_gui_button_create(344,109,14,14,">");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_dither_right);
   label = SLK_gui_label_create(104,112,56,12,"Dither");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   elements.color_label_dither = SLK_gui_label_create(174,112,170,12,text_dither[1]);
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_label_dither);

   label = SLK_gui_label_create(104,144,56,12,"Amount");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   elements.color_bar_dither = SLK_gui_slider_create(174,141,170,14,0,999);
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_bar_dither);
   elements.color_bar_dither->slider.value = 64;
   elements.color_dither_plus = SLK_gui_button_create(344,141,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_dither_plus);
   elements.color_dither_minus = SLK_gui_button_create(160,141,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_dither_minus);
   elements.color_label_dither_amount = SLK_gui_label_create(354,144,32,12,"64");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_label_dither_amount);

   label = SLK_gui_label_create(104,176,56,12,"Alpha");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   elements.color_alpha_plus = SLK_gui_button_create(344,173,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_alpha_plus);
   elements.color_alpha_minus = SLK_gui_button_create(160,173,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_alpha_minus);
   elements.color_bar_alpha = SLK_gui_slider_create(174,173,170,14,0,255);
   elements.color_bar_alpha->slider.value = 128;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_bar_alpha);
   elements.color_label_alpha = SLK_gui_label_create(354,176,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_label_alpha);

   label = SLK_gui_label_create(104,208,48,12,"dist");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   elements.color_label_space = SLK_gui_label_create(174,208,170,12,text_space[0]);
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_label_space);
   elements.color_space_left = SLK_gui_button_create(160,205,14,14,"<");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_space_left);
   elements.color_space_right = SLK_gui_button_create(344,205,14,14,">");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_space_right);

   label = SLK_gui_label_create(104,230,56,12,"Weight");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   elements.color_weight_plus = SLK_gui_button_create(344,227,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_weight_plus);
   elements.color_weight_minus = SLK_gui_button_create(160,227,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_weight_minus);
   elements.color_bar_weight = SLK_gui_slider_create(174,227,170,14,0,16);
   elements.color_bar_weight->slider.value = 2;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_bar_weight);
   elements.color_label_weight = SLK_gui_label_create(354,230,32,12,"2");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_label_weight);

   elements.color_palette = SLK_gui_image_create(100,15,279,81,elements.palette_sprite,(SLK_gui_rectangle){0,0,279,81});
   //This could break, I hope I don't forget about this...
   //I don't want to manage multiple sprites that have the same content, so I
   //override this elements sprite by the one of the other element.
   SLK_rgb_sprite_destroy(elements.color_palette->image.sprite);
   elements.color_palette->image.sprite = elements.palette_palette->image.sprite;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_palette);
   elements.color_button = SLK_gui_icon_create(100,15,279,81,tmp,(SLK_gui_rectangle){0,0,1,1},(SLK_gui_rectangle){0,0,1,1});
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_button);

   label = SLK_gui_label_create(104,256,56,12,"inline");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   label = SLK_gui_label_create(264,256,64,12,"outline");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   elements.color_check_inline = SLK_gui_button_create(184,253,14,14,img2pixel_get_inline()<0?" ":"x");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_check_inline);
   elements.color_button_inline = SLK_gui_button_create(104,272,94,14,"set color");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_button_inline);
   elements.color_check_outline = SLK_gui_button_create(344,253,14,14,img2pixel_get_outline()<0?" ":"x");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_check_outline);
   elements.color_button_outline = SLK_gui_button_create(264,272,94,14,"set color");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.color_button_outline);

   //Process tab
   label = SLK_gui_label_create(104,24,56,12,"Bright");
   SLK_gui_vtabbar_add_element(settings_tabs,4,label);
   label = SLK_gui_label_create(104,56,56,12,"Contra");
   SLK_gui_vtabbar_add_element(settings_tabs,4,label);
   label = SLK_gui_label_create(104,88,56,12,"Satura");
   SLK_gui_vtabbar_add_element(settings_tabs,4,label);
   label = SLK_gui_label_create(104,120,56,12,"Gamma");
   SLK_gui_vtabbar_add_element(settings_tabs,4,label);
   label = SLK_gui_label_create(104,152,56,12,"Sharp");
   SLK_gui_vtabbar_add_element(settings_tabs,4,label);
   label = SLK_gui_label_create(104,184,56,12,"Hue");
   SLK_gui_vtabbar_add_element(settings_tabs,4,label);
   elements.process_bar_brightness = SLK_gui_slider_create(174,21,162,14,-255,255);
   elements.process_bar_brightness->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_bar_brightness);
   elements.process_label_brightness = SLK_gui_label_create(346,24,40,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_label_brightness);
   elements.process_plus_brightness = SLK_gui_button_create(336,21,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_plus_brightness);
   elements.process_minus_brightness = SLK_gui_button_create(160,21,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_minus_brightness);
   elements.process_bar_contrast = SLK_gui_slider_create(174,53,162,14,-255,255);
   elements.process_bar_contrast->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_bar_contrast);
   elements.process_label_contrast = SLK_gui_label_create(346,56,40,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_label_contrast);
   elements.process_plus_contrast = SLK_gui_button_create(336,53,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_plus_contrast);
   elements.process_minus_contrast = SLK_gui_button_create(160,53,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_minus_contrast);
   elements.process_bar_saturation = SLK_gui_slider_create(174,85,162,14,1,600);
   elements.process_bar_saturation->slider.value = 100;
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_bar_saturation);
   elements.process_label_saturation = SLK_gui_label_create(346,88,40,12,"100");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_label_saturation);
   elements.process_plus_saturation = SLK_gui_button_create(336,85,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_plus_saturation);
   elements.process_minus_saturation = SLK_gui_button_create(160,85,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_minus_saturation);
   elements.process_bar_gamma = SLK_gui_slider_create(174,118,162,14,1,800);
   elements.process_bar_gamma->slider.value = 100;
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_bar_gamma);
   elements.process_label_gamma = SLK_gui_label_create(346,120,40,12,"100");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_label_gamma);
   elements.process_plus_gamma = SLK_gui_button_create(336,118,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_plus_gamma);
   elements.process_minus_gamma = SLK_gui_button_create(160,118,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_minus_gamma);
   elements.process_bar_sharpen = SLK_gui_slider_create(174,150,162,14,0,100);
   elements.process_bar_sharpen->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_bar_sharpen);
   elements.process_label_sharpen = SLK_gui_label_create(346,152,40,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_label_sharpen);
   elements.process_plus_sharpen = SLK_gui_button_create(336,150,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_plus_sharpen);
   elements.process_minus_sharpen = SLK_gui_button_create(160,150,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_minus_sharpen);
   elements.process_bar_hue = SLK_gui_slider_create(174,182,162,14,-360,360);
   elements.process_bar_hue->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_bar_hue);
   elements.process_label_hue = SLK_gui_label_create(346,184,40,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_label_hue);
   elements.process_plus_hue = SLK_gui_button_create(336,182,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_plus_hue);
   elements.process_minus_hue = SLK_gui_button_create(160,182,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,4,elements.process_minus_hue);

   //Special tab
   elements.special_gif_load = SLK_gui_button_create(158,32,164,14,"Load gif");
   SLK_gui_vtabbar_add_element(settings_tabs,19,elements.special_gif_load);
   elements.special_gif_save = SLK_gui_button_create(158,64,164,14,"Save gif");
   SLK_gui_vtabbar_add_element(settings_tabs,19,elements.special_gif_save);
   //-------------------------------------

   //Load default json
   const char *env_def = getenv("IMG2PIXEL_CONF");
   if(!env_def)
      env_def = "./default.json";
   FILE *f = fopen(env_def,"r");
   preset_load(f);
   if(f!=NULL)
      fclose(f);

   elements.palette_bar_colors->slider.value = img2pixel_get_palette()->used;
   char ctmp[16];
   sprintf(ctmp,"%d",img2pixel_get_palette()->used);
   SLK_gui_label_set_text(elements.palette_label_colors,ctmp);
}

void gui_update()
{
   int mx = 0,my = 0;
   SLK_mouse_get_layer_pos(2,&mx,&my);
   if(settings->moveable!=2)
      SLK_gui_window_update_input(preview,SLK_mouse_get_state(SLK_BUTTON_LEFT),SLK_mouse_get_state(SLK_BUTTON_RIGHT),mx,my);
   if(preview->moveable!=2)
      SLK_gui_window_update_input(settings,SLK_mouse_get_state(SLK_BUTTON_LEFT),SLK_mouse_get_state(SLK_BUTTON_RIGHT),mx,my);
   gui_buttons();

   gui_draw();
}

//Big boy function
static void gui_buttons()
{
   int mx = 0,my = 0;
   int update = 0;
   SLK_mouse_get_layer_pos(2,&mx,&my);

   switch(settings_tabs->vtabbar.current_tab)
   {
   case 0: //Save/Load tab

      //upscale bar and buttons
      BUTTON_BAR_PLUS(elements.save_upscale_plus,elements.save_bar_upscale);
      BUTTON_BAR_MINUS(elements.save_upscale_minus,elements.save_bar_upscale);
      if(elements.save_bar_upscale->slider.value!=upscale)
      {
         upscale = elements.save_bar_upscale->slider.value;
         char tmp[16];
         sprintf(tmp,"%d",upscale);
         SLK_gui_label_set_text(elements.save_label_upscale,tmp);
      }
   
      //Load image button
      if(elements.save_load->button.state.released)
      {
         SLK_RGB_sprite *sprite_new = image_select();

         if(sprite_new!=NULL)
         {
            SLK_rgb_sprite_destroy(sprite_in);
            SLK_rgb_sprite_destroy(sprite_in_org);
   
            sprite_in = sprite_new;
            sprite_in_org = SLK_rgb_sprite_create(sprite_in->width,sprite_in->height);
            SLK_rgb_sprite_copy(sprite_in_org,sprite_in);
            img2pixel_lowpass_image(sprite_in_org,sprite_in);
            img2pixel_sharpen_image(sprite_in,sprite_in);

            //Readjust input preview
            SLK_layer_set_size(0,sprite_in->width,sprite_in->height);
            SLK_layer_set_current(0);
            SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
            SLK_draw_rgb_clear();
            SLK_draw_rgb_sprite(sprite_in_org,0,0);
            SLK_draw_rgb_set_changed(1);
            float scale;
            if(sprite_in->width>sprite_in->height)
               scale = 256.0f/sprite_in->width;
            else 
               scale = 256.0f/sprite_in->height;
            SLK_layer_set_scale(0,scale);
            int fwidth = (int)((float)sprite_in->width*scale);
            int fheight = (int)((float)sprite_in->height*scale);
            gui_in_x = (256-fwidth)/2;
            gui_in_y = (256-fheight)/2;
         }

         update = 1;
         elements.save_load->button.state.released = 0;
      }

      //Save image button
      if(elements.save_save->button.state.released)
      {
         image_write(sprite_out,img2pixel_get_palette());
         elements.save_save->button.state.released = 0;
      }

      //Load preset button
      if(elements.save_load_preset->button.state.released)
      {
         FILE *f = json_select();
         preset_load(f);
         if(f!=NULL)
            fclose(f);
         elements.save_load_preset->button.state.released = 0;
      }

      //Save preset button
      if(elements.save_save_preset->button.state.released)
      {
         FILE *f = json_write();

         if(f!=NULL)
         {
            img2pixel_preset_save(f);
            fclose(f);
         }

         elements.save_save_preset->button.state.released = 0;
      }
   
      //Select input dir button
      if(elements.save_save_folder->button.state.released)
      {
         if(img2pixel_get_scale_mode()==0)
            dir_output_select(img2pixel_get_process_mode(),img2pixel_get_sample_mode(),img2pixel_get_distance_mode(),img2pixel_get_scale_mode(),img2pixel_get_out_width(),img2pixel_get_out_height(),img2pixel_get_palette());
         else
            dir_output_select(img2pixel_get_process_mode(),img2pixel_get_sample_mode(),img2pixel_get_distance_mode(),img2pixel_get_scale_mode(),img2pixel_get_out_swidth(),img2pixel_get_out_sheight(),img2pixel_get_palette());
         elements.save_save_folder->button.state.released = 0;
      }

      //Select output dir button
      if(elements.save_load_folder->button.state.released)
      {
         dir_input_select();
         elements.save_load_folder->button.state.released = 0;
      }

      break;
   case 1: //Palette tab
      if(elements.palette_load->button.state.released)
      {
         SLK_Palette *p = palette_select();
         if(p!=NULL)
         {
            if(img2pixel_get_palette())
               free(img2pixel_get_palette());
            img2pixel_set_palette(p);
            update = 1;
            elements.palette_bar_r->slider.value = p->colors[palette_selected].r;
            elements.palette_bar_g->slider.value = p->colors[palette_selected].g;
            elements.palette_bar_b->slider.value = p->colors[palette_selected].b;
            palette_draw();
            palette_labels();
            elements.palette_bar_colors->slider.value = img2pixel_get_palette()->used;
            char ctmp[16];
            sprintf(ctmp,"%d",img2pixel_get_palette()->used);
            SLK_gui_label_set_text(elements.palette_label_colors,ctmp);
         }
         elements.palette_load->button.state.released = 0;
      }
      else if(elements.palette_save->button.state.released)
      {
         palette_write(img2pixel_get_palette());
         elements.palette_save->button.state.released = 0;
      }
      else if(elements.palette_generate->button.state.released)
      {
         elements.palette_save->button.state.released = 0;

         img2pixel_quantize(img2pixel_get_palette()->used,sprite_in_org);

         update = 1;
         elements.palette_bar_r->slider.value = img2pixel_get_palette()->colors[palette_selected].r;
         elements.palette_bar_g->slider.value = img2pixel_get_palette()->colors[palette_selected].g;
         elements.palette_bar_b->slider.value = img2pixel_get_palette()->colors[palette_selected].b;
         palette_draw();
         palette_labels();
         elements.palette_bar_colors->slider.value = img2pixel_get_palette()->used;
         char ctmp[16];
         sprintf(ctmp,"%d",img2pixel_get_palette()->used);
         SLK_gui_label_set_text(elements.palette_label_colors,ctmp);
      }
      else if(elements.palette_button->icon.state.pressed)
      {
         int nx = mx-settings->pos.x-elements.palette_button->icon.pos.x;
         int ny = my-settings->pos.y-elements.palette_button->icon.pos.y;
         palette_selected = MIN(255,(ny/9)*31+nx/9);
         palette_labels();
      }

      BUTTON_BAR_PLUS(elements.palette_plus_r,elements.palette_bar_r);
      BUTTON_BAR_MINUS(elements.palette_minus_r,elements.palette_bar_r);

      BUTTON_BAR_PLUS(elements.palette_plus_g,elements.palette_bar_g);
      BUTTON_BAR_MINUS(elements.palette_minus_g,elements.palette_bar_g);

      BUTTON_BAR_PLUS(elements.palette_plus_b,elements.palette_bar_b);
      BUTTON_BAR_MINUS(elements.palette_minus_b,elements.palette_bar_b);

      BUTTON_BAR_PLUS(elements.palette_plus_colors,elements.palette_bar_colors);
      BUTTON_BAR_MINUS(elements.palette_minus_colors,elements.palette_bar_colors);

      if(elements.palette_bar_r->slider.value!=img2pixel_get_palette()->colors[palette_selected].r||elements.palette_bar_g->slider.value!=img2pixel_get_palette()->colors[palette_selected].g||elements.palette_bar_b->slider.value!=img2pixel_get_palette()->colors[palette_selected].b)
      {
         img2pixel_get_palette()->colors[palette_selected].r = elements.palette_bar_r->slider.value;
         img2pixel_get_palette()->colors[palette_selected].g = elements.palette_bar_g->slider.value;
         img2pixel_get_palette()->colors[palette_selected].b = elements.palette_bar_b->slider.value;
         if(palette_selected!=0)
            img2pixel_get_palette()->colors[palette_selected].a = 255;
         img2pixel_get_palette()->used = MAX(img2pixel_get_palette()->used,palette_selected+1);
         palette_draw();
         palette_labels();
         update = 1;
      }

      if(elements.palette_bar_colors->slider.value!=img2pixel_get_palette()->used)
      {
         img2pixel_get_palette()->used = elements.palette_bar_colors->slider.value;
         char tmp[16];
         sprintf(tmp,"%d",img2pixel_get_palette()->used);
         SLK_gui_label_set_text(elements.palette_label_colors,tmp);
         update = 1;
      }
      break;
   case 2: //Sample tab
      BUTTON_BAR_PLUS(elements.sample_width_plus,elements.sample_bar_width);
      BUTTON_BAR_MINUS(elements.sample_width_minus,elements.sample_bar_width);

      BUTTON_BAR_PLUS(elements.sample_height_plus,elements.sample_bar_height);
      BUTTON_BAR_MINUS(elements.sample_height_minus,elements.sample_bar_height);

      BUTTON_BAR_PLUS(elements.sample_swidth_plus,elements.sample_bar_swidth);
      BUTTON_BAR_MINUS(elements.sample_swidth_minus,elements.sample_bar_swidth);

      BUTTON_BAR_PLUS(elements.sample_sheight_plus,elements.sample_bar_sheight);
      BUTTON_BAR_MINUS(elements.sample_sheight_minus,elements.sample_bar_sheight);

      BUTTON_BAR_PLUS(elements.sample_offset_x_plus,elements.sample_bar_offset_x);
      BUTTON_BAR_MINUS(elements.sample_offset_x_minus,elements.sample_bar_offset_x);

      BUTTON_BAR_PLUS(elements.sample_offset_y_plus,elements.sample_bar_offset_y);
      BUTTON_BAR_MINUS(elements.sample_offset_y_minus,elements.sample_bar_offset_y);

      BUTTON_BAR_PLUS(elements.sample_gauss_plus,elements.sample_bar_gauss);
      BUTTON_BAR_MINUS(elements.sample_gauss_minus,elements.sample_bar_gauss);

      BAR_UPDATE(img2pixel_get_out_width,img2pixel_set_out_width,elements.sample_bar_width,elements.sample_label_width);
      BAR_UPDATE(img2pixel_get_out_height,img2pixel_set_out_height,elements.sample_bar_height,elements.sample_label_height);
      BAR_UPDATE(img2pixel_get_out_swidth,img2pixel_set_out_swidth,elements.sample_bar_swidth,elements.sample_label_swidth);
      BAR_UPDATE(img2pixel_get_out_sheight,img2pixel_set_out_sheight,elements.sample_bar_sheight,elements.sample_label_sheight);
      BAR_UPDATE(img2pixel_get_offset_x,img2pixel_set_offset_x,elements.sample_bar_offset_x,elements.sample_label_offset_x);
      BAR_UPDATE(img2pixel_get_offset_y,img2pixel_set_offset_y,elements.sample_bar_offset_y,elements.sample_label_offset_y);

      if(elements.sample_tab_scale->tabbar.current_tab!=img2pixel_get_scale_mode())
      {
         img2pixel_set_scale_mode(elements.sample_tab_scale->tabbar.current_tab);
         update = 1;
      }

      if(elements.sample_sample_left->button.state.pressed)
      {
         img2pixel_set_sample_mode(img2pixel_get_sample_mode()-1);
         if(img2pixel_get_sample_mode()<0)
            img2pixel_set_sample_mode(5);
         update = 1;
         SLK_gui_label_set_text(elements.sample_label_sample,text_sample[img2pixel_get_sample_mode()]);
      }
      else if(elements.sample_sample_right->button.state.pressed)
      {
         img2pixel_set_sample_mode(img2pixel_get_sample_mode()+1);
         if(img2pixel_get_sample_mode()>5)
            img2pixel_set_sample_mode(0);
         update = 1;
         SLK_gui_label_set_text(elements.sample_label_sample,text_sample[img2pixel_get_sample_mode()]);
      }
      if(elements.sample_bar_gauss->slider.value!=img2pixel_get_gauss())
      {
         img2pixel_set_gauss(elements.sample_bar_gauss->slider.value);
         char tmp[16];
         sprintf(tmp,"%d",img2pixel_get_gauss());
         SLK_gui_label_set_text(elements.sample_label_gauss,tmp);
         img2pixel_lowpass_image(sprite_in_org,sprite_in);
         img2pixel_sharpen_image(sprite_in,sprite_in);
         update = 1;
      }

      break;
   case 3: //Color tab
      BUTTON_BAR_PLUS(elements.color_dither_plus,elements.color_bar_dither);
      BUTTON_BAR_MINUS(elements.color_dither_minus,elements.color_bar_dither);

      BUTTON_BAR_PLUS(elements.color_alpha_plus,elements.color_bar_alpha);
      BUTTON_BAR_MINUS(elements.color_alpha_minus,elements.color_bar_alpha);

      BUTTON_BAR_PLUS(elements.color_weight_plus,elements.color_bar_weight);
      BUTTON_BAR_MINUS(elements.color_weight_minus,elements.color_bar_weight);

      if(elements.color_dither_left->button.state.pressed)
      {
         img2pixel_set_process_mode(img2pixel_get_process_mode()-1);
         if(img2pixel_get_process_mode()<0)
            img2pixel_set_process_mode(7);
         update = 1;
         SLK_gui_label_set_text(elements.color_label_dither,text_dither[img2pixel_get_process_mode()]);
      }
      else if(elements.color_dither_right->button.state.pressed)
      {
         img2pixel_set_process_mode(img2pixel_get_process_mode()+1);
         if(img2pixel_get_process_mode()>7)
            img2pixel_set_process_mode(0);
         update = 1;
         SLK_gui_label_set_text(elements.color_label_dither,text_dither[img2pixel_get_process_mode()]);
      }

      BAR_UPDATE(img2pixel_get_alpha_threshold,img2pixel_set_alpha_threshold,elements.color_bar_alpha,elements.color_label_alpha);
      BAR_UPDATE(img2pixel_get_dither_amount,img2pixel_set_dither_amount,elements.color_bar_dither,elements.color_label_dither_amount);
      BAR_UPDATE(img2pixel_get_palette_weight,img2pixel_set_palette_weight,elements.color_bar_weight,elements.color_label_weight);

      if(elements.color_bar_dither->slider.value!=img2pixel_get_dither_amount())
      {
         img2pixel_set_dither_amount(elements.color_bar_dither->slider.value);
         update = 1;
      }

      if(elements.color_space_left->button.state.pressed)
      {
         img2pixel_set_distance_mode(img2pixel_get_distance_mode()-1);;
         if(img2pixel_get_distance_mode()<0)
            img2pixel_set_distance_mode(8);
         update = 1;
         SLK_gui_label_set_text(elements.color_label_space,text_space[img2pixel_get_distance_mode()]);
      }
      if(elements.color_space_right->button.state.pressed)
      {
         img2pixel_set_distance_mode(img2pixel_get_distance_mode()+1);;
         if(img2pixel_get_distance_mode()>8)
            img2pixel_set_distance_mode(0);
         update = 1;
         SLK_gui_label_set_text(elements.color_label_space,text_space[img2pixel_get_distance_mode()]);
      }
      if(elements.color_button->icon.state.pressed)
      {
         int nx = mx-settings->pos.x-elements.palette_button->icon.pos.x;
         int ny = my-settings->pos.y-elements.palette_button->icon.pos.y;
         palette_selected = MIN(255,(ny/9)*31+nx/9);
         palette_labels();
      }

      if(elements.color_check_inline->button.state.pressed)
      {
         if(elements.color_check_inline->button.text[0]=='x')
            img2pixel_set_inline(img2pixel_get_inline()-256);
         else
            img2pixel_set_inline(img2pixel_get_inline()+256);
         elements.color_check_inline->button.text[0] = elements.color_check_inline->button.text[0]=='x'?' ':'x';
         update = 1;
      }
      if(elements.color_check_outline->button.state.pressed)
      {
         if(elements.color_check_outline->button.text[0]=='x')
            img2pixel_set_outline(img2pixel_get_outline()-256);
         else
            img2pixel_set_outline(img2pixel_get_outline()+256);
         elements.color_check_outline->button.text[0] = elements.color_check_outline->button.text[0]=='x'?' ':'x';
         update = 1;
      }

      if(elements.color_button_inline->button.state.pressed)
      {
         if(img2pixel_get_inline()<0)
            img2pixel_set_inline(palette_selected-256);
         else
            img2pixel_set_inline(palette_selected);
         update = 1;
      }
      if(elements.color_button_outline->button.state.pressed)
      {
         if(img2pixel_get_outline()<0)
            img2pixel_set_outline(palette_selected-256);
         else
            img2pixel_set_outline(palette_selected);
         update = 1;
      }

      break;
   case 4: //Process tab
      BUTTON_BAR_PLUS(elements.process_plus_brightness,elements.process_bar_brightness);
      BUTTON_BAR_MINUS(elements.process_minus_brightness,elements.process_bar_brightness);

      BUTTON_BAR_PLUS(elements.process_plus_contrast,elements.process_bar_contrast);
      BUTTON_BAR_MINUS(elements.process_minus_contrast,elements.process_bar_contrast);

      BUTTON_BAR_PLUS(elements.process_plus_saturation,elements.process_bar_saturation);
      BUTTON_BAR_MINUS(elements.process_minus_saturation,elements.process_bar_saturation);

      BUTTON_BAR_PLUS(elements.process_plus_gamma,elements.process_bar_gamma);
      BUTTON_BAR_MINUS(elements.process_minus_gamma,elements.process_bar_gamma);

      BUTTON_BAR_PLUS(elements.process_plus_sharpen,elements.process_bar_sharpen);
      BUTTON_BAR_MINUS(elements.process_minus_sharpen,elements.process_bar_sharpen);

      BUTTON_BAR_PLUS(elements.process_plus_hue,elements.process_bar_hue);
      BUTTON_BAR_MINUS(elements.process_minus_hue,elements.process_bar_hue);

      BAR_UPDATE(img2pixel_get_brightness,img2pixel_set_brightness,elements.process_bar_brightness,elements.process_label_brightness);
      BAR_UPDATE(img2pixel_get_contrast,img2pixel_set_contrast,elements.process_bar_contrast,elements.process_label_contrast);
      BAR_UPDATE(img2pixel_get_saturation,img2pixel_set_saturation,elements.process_bar_saturation,elements.process_label_saturation);
      BAR_UPDATE(img2pixel_get_gamma,img2pixel_set_gamma,elements.process_bar_gamma,elements.process_label_gamma);
      BAR_UPDATE(img2pixel_get_hue,img2pixel_set_hue,elements.process_bar_hue,elements.process_label_hue);

      if(img2pixel_get_sharpen()!=elements.process_bar_sharpen->slider.value)
      {
         img2pixel_set_sharpen(elements.process_bar_sharpen->slider.value);
         char ctmp[16];
         sprintf(ctmp,"%d",img2pixel_get_sharpen());
         SLK_gui_label_set_text(elements.process_label_sharpen,ctmp);
         img2pixel_lowpass_image(sprite_in_org,sprite_in);
         img2pixel_sharpen_image(sprite_in,sprite_in);
         update = 1;
      }
      break;
   case 19: //Special tab
      if(elements.special_gif_save->button.state.released)
      {
         if(img2pixel_get_scale_mode()==0)
            gif_output_select(img2pixel_get_process_mode(),img2pixel_get_sample_mode(),img2pixel_get_distance_mode(),img2pixel_get_scale_mode(),img2pixel_get_out_width(),img2pixel_get_out_height(),img2pixel_get_palette());
         else
            gif_output_select(img2pixel_get_process_mode(),img2pixel_get_sample_mode(),img2pixel_get_distance_mode(),img2pixel_get_scale_mode(),img2pixel_get_out_swidth(),img2pixel_get_out_sheight(),img2pixel_get_palette());
         elements.special_gif_save->button.state.released = 0;
      }
      else if(elements.special_gif_load->button.state.released)
      {
         gif_input_select();
         elements.special_gif_load->button.state.released = 0;
      }
      break;
   }

   if(update)
      update_output(); 
}

static void gui_draw()
{
   SLK_layer_set_current(2);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(20,20,20,255));
   SLK_draw_rgb_clear();

   SLK_gui_window_draw(settings);
   if(settings_tabs->vtabbar.current_tab==1||settings_tabs->vtabbar.current_tab==3)
   {
      int pos_y = palette_selected/31;
      int pos_x = palette_selected-pos_y*31;
      SLK_draw_rgb_rectangle(settings->pos.x+pos_x*9+100,settings->pos.y+pos_y*9+15,9,9,SLK_color_create(0,0,0,255));
   }
   if(preview_tabs->tabbar.current_tab==0)
   {
      SLK_layer_activate(0,1);
      SLK_layer_activate(1,0);
      SLK_layer_set_pos(0,preview->pos.x+2+gui_in_x,preview->pos.y+28+gui_in_y);
   }
   else
   {
      SLK_layer_activate(0,0);
      SLK_layer_activate(1,1);
      SLK_layer_set_pos(1,preview->pos.x+2+gui_out_x,preview->pos.y+28+gui_out_y);
   }
   SLK_gui_window_draw(preview);
}

static void update_output()
{
   if(sprite_in==NULL)
      return;
   int width;
   int height;
   if(img2pixel_get_scale_mode()==0)
   {
      width = img2pixel_get_out_width();
      height = img2pixel_get_out_height();
   }
   else
   {
      width = sprite_in->width/img2pixel_get_out_swidth();
      height = sprite_in->height/img2pixel_get_out_sheight();
   }

   if(sprite_out==NULL||sprite_out->width!=width||sprite_out->height!=height)
   {
      SLK_rgb_sprite_destroy(sprite_out);
      sprite_out = SLK_rgb_sprite_create(width,height);
      SLK_layer_set_size(1,sprite_out->width,sprite_out->height);
   }

   if(sprite_in==NULL)
      return;

   img2pixel_process_image(sprite_in,sprite_out);

   SLK_layer_set_current(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
   SLK_draw_rgb_clear();
   SLK_draw_rgb_sprite(sprite_out,0,0);
   SLK_draw_rgb_set_changed(1);
   float scale;
   if(sprite_out->width>sprite_out->height)
      scale = 256.0f/sprite_out->width;
   else 
      scale = 256.0f/sprite_out->height;
   SLK_layer_set_scale(1,scale);
   int fwidth = (int)((float)sprite_out->width*scale);
   int fheight = (int)((float)sprite_out->height*scale);
   gui_out_x = (256-fwidth)/2;
   gui_out_y = (256-fheight)/2;
}

static void palette_draw()
{
   SLK_RGB_sprite *old = SLK_draw_rgb_get_target();
   SLK_draw_rgb_set_target(elements.palette_sprite);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,255));
   SLK_draw_rgb_clear();

   for(int y = 0;y<9;y++)
      for(int x = 0;x<31;x++)
         if(y*31+x<img2pixel_get_palette()->used)
            SLK_draw_rgb_fill_rectangle(x*9,y*9,9,9,img2pixel_get_palette()->colors[y*31+x]);

   SLK_draw_rgb_set_target(old);
   SLK_gui_image_update(elements.palette_palette,elements.palette_sprite,(SLK_gui_rectangle){0,0,279,81});
}

static void palette_labels()
{
   char ctmp[16];
   sprintf(ctmp,"%d",img2pixel_get_palette()->colors[palette_selected].r);
   SLK_gui_label_set_text(elements.palette_label_r,ctmp);
   elements.palette_bar_r->slider.value = img2pixel_get_palette()->colors[palette_selected].r;
   sprintf(ctmp,"%d",img2pixel_get_palette()->colors[palette_selected].g);
   SLK_gui_label_set_text(elements.palette_label_g,ctmp);
   elements.palette_bar_g->slider.value = img2pixel_get_palette()->colors[palette_selected].g;
   sprintf(ctmp,"%d",img2pixel_get_palette()->colors[palette_selected].b);
   SLK_gui_label_set_text(elements.palette_label_b,ctmp);
   elements.palette_bar_b->slider.value = img2pixel_get_palette()->colors[palette_selected].b;
}

void preset_load(FILE *f)
{
   char ctmp[16];
   img2pixel_preset_load(f);

   //The unnecessary indentations are for my own sanity
   elements.palette_bar_r->slider.value = img2pixel_get_palette()->colors[palette_selected].r;
   elements.palette_bar_g->slider.value = img2pixel_get_palette()->colors[palette_selected].g;
   elements.palette_bar_b->slider.value = img2pixel_get_palette()->colors[palette_selected].b;
   palette_draw();
   palette_labels();
   SLK_gui_label_set_text(elements.color_label_space,text_space[img2pixel_get_distance_mode()]);
   elements.sample_bar_width->slider.value = img2pixel_get_out_width();
      sprintf(ctmp,"%d",img2pixel_get_out_width());
      SLK_gui_label_set_text(elements.sample_label_width,ctmp);
   elements.sample_bar_height->slider.value = img2pixel_get_out_height();
      sprintf(ctmp,"%d",img2pixel_get_out_height());
      SLK_gui_label_set_text(elements.sample_label_height,ctmp);
   elements.sample_bar_swidth->slider.value = img2pixel_get_out_swidth();
      sprintf(ctmp,"%d",img2pixel_get_out_swidth());
      SLK_gui_label_set_text(elements.sample_label_swidth,ctmp);
   elements.sample_bar_sheight->slider.value = img2pixel_get_out_sheight();
      sprintf(ctmp,"%d",img2pixel_get_out_sheight());
      SLK_gui_label_set_text(elements.sample_label_sheight,ctmp);
   elements.sample_tab_scale->tabbar.current_tab = img2pixel_get_scale_mode();
   SLK_gui_label_set_text(elements.color_label_dither,text_dither[img2pixel_get_process_mode()]);
   elements.color_bar_dither->slider.value = img2pixel_get_dither_amount();
   SLK_gui_label_set_text(elements.sample_label_sample,text_sample[img2pixel_get_sample_mode()]);
   elements.color_bar_alpha->slider.value = img2pixel_get_alpha_threshold();
      sprintf(ctmp,"%d",img2pixel_get_alpha_threshold());
      SLK_gui_label_set_text(elements.color_label_alpha,ctmp);
   elements.save_bar_upscale->slider.value = upscale;
      sprintf(ctmp,"%d",upscale);
      SLK_gui_label_set_text(elements.save_label_upscale,ctmp);
   elements.sample_bar_gauss->slider.value = img2pixel_get_gauss();
      sprintf(ctmp,"%d",img2pixel_get_gauss());
      SLK_gui_label_set_text(elements.sample_label_gauss,ctmp);
   elements.sample_bar_offset_x->slider.value = img2pixel_get_offset_x();
      sprintf(ctmp,"%d",img2pixel_get_offset_x());
      SLK_gui_label_set_text(elements.sample_label_offset_x,ctmp);
   elements.sample_bar_offset_y->slider.value = img2pixel_get_offset_y();
      sprintf(ctmp,"%d",img2pixel_get_offset_y());
      SLK_gui_label_set_text(elements.sample_label_offset_y,ctmp);
   elements.process_bar_brightness->slider.value = img2pixel_get_brightness();
      sprintf(ctmp,"%d",img2pixel_get_brightness());
      SLK_gui_label_set_text(elements.process_label_brightness,ctmp);
   elements.process_bar_contrast->slider.value = img2pixel_get_contrast();
      sprintf(ctmp,"%d",img2pixel_get_contrast());
      SLK_gui_label_set_text(elements.process_label_contrast,ctmp);
   elements.process_bar_saturation->slider.value = img2pixel_get_saturation();
      sprintf(ctmp,"%d",img2pixel_get_saturation());
      SLK_gui_label_set_text(elements.process_label_saturation,ctmp);
   elements.process_bar_gamma->slider.value = img2pixel_get_gamma();
      sprintf(ctmp,"%d",img2pixel_get_gamma());
      SLK_gui_label_set_text(elements.process_label_gamma,ctmp);
   elements.process_bar_sharpen->slider.value = img2pixel_get_sharpen();
      sprintf(ctmp,"%d",img2pixel_get_sharpen());
      SLK_gui_label_set_text(elements.process_label_sharpen,ctmp);
   elements.process_bar_hue->slider.value = img2pixel_get_hue();
      sprintf(ctmp,"%d",img2pixel_get_hue());
      SLK_gui_label_set_text(elements.process_label_hue,ctmp);
   elements.color_check_inline->button.text[0] = img2pixel_get_inline()<0?' ':'x';
   elements.color_check_outline->button.text[0] = img2pixel_get_outline()<0?' ':'x';
   elements.palette_bar_colors->slider.value = img2pixel_get_palette()->used;
   sprintf(ctmp,"%d",img2pixel_get_palette()->used);
   SLK_gui_label_set_text(elements.palette_label_colors,ctmp);

   img2pixel_lowpass_image(sprite_in_org,sprite_in);   
   img2pixel_sharpen_image(sprite_in,sprite_in);
   update_output();
}
//-------------------------------------
