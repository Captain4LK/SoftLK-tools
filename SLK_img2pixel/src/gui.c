/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <SLK/SLK.h>
#include <SLK/SLK_gui.h>
#include "../../external/UtilityLK/include/ULK_json.h"
//-------------------------------------

//Internal includes
#include "assets.h"
#include "gui.h"
#include "process.h"
#include "sample.h"
#include "utility.h"
//-------------------------------------

//#defines
#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))
//-------------------------------------

//Typedefs
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

   //Palette tab
   SLK_gui_element *palette_save;
   SLK_gui_element *palette_load;
   SLK_gui_element *palette_palette;
   SLK_RGB_sprite *palette_sprite;
   SLK_gui_element *palette_button;
   SLK_gui_element *palette_bar_r;
   SLK_gui_element *palette_bar_g;
   SLK_gui_element *palette_bar_b;
   SLK_gui_element *palette_plus_r;
   SLK_gui_element *palette_minus_r;
   SLK_gui_element *palette_plus_g;
   SLK_gui_element *palette_minus_g;
   SLK_gui_element *palette_plus_b;
   SLK_gui_element *palette_minus_b;
   SLK_gui_element *palette_label_r;
   SLK_gui_element *palette_label_g;
   SLK_gui_element *palette_label_b;

   //General tab
   SLK_gui_element *general_tab_scale;
   SLK_gui_element *general_width_plus;
   SLK_gui_element *general_width_minus;
   SLK_gui_element *general_height_plus;
   SLK_gui_element *general_height_minus;
   SLK_gui_element *general_swidth_plus;
   SLK_gui_element *general_swidth_minus;
   SLK_gui_element *general_sheight_plus;
   SLK_gui_element *general_sheight_minus;
   SLK_gui_element *general_alpha_plus;
   SLK_gui_element *general_alpha_minus;
   SLK_gui_element *general_gauss_plus;
   SLK_gui_element *general_gauss_minus;
   SLK_gui_element *general_dither_left;
   SLK_gui_element *general_dither_right;
   SLK_gui_element *general_sample_left;
   SLK_gui_element *general_sample_right;
   SLK_gui_element *general_bar_width;
   SLK_gui_element *general_bar_height;
   SLK_gui_element *general_bar_swidth;
   SLK_gui_element *general_bar_sheight;
   SLK_gui_element *general_bar_dither;
   SLK_gui_element *general_bar_gauss;
   SLK_gui_element *general_bar_alpha;
   SLK_gui_element *general_label_width;
   SLK_gui_element *general_label_height;
   SLK_gui_element *general_label_swidth;
   SLK_gui_element *general_label_sheight;
   SLK_gui_element *general_label_dither;
   SLK_gui_element *general_label_sample;
   SLK_gui_element *general_label_gauss;
   SLK_gui_element *general_label_alpha;

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

   //Special tab
   SLK_gui_element *special_gif_load;
   SLK_gui_element *special_gif_save;
};
static struct Elements elements = {0};

static const char *text_dither[] = 
{
   "No dithering",
   "Ordered 1",
   "Ordered 2",
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
   "Box sampling",
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
   "General",
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
   "",
   "Special",
};

static SLK_Palette *palette = NULL;
static int gui_out_width = 128;
static int gui_out_height = 128;
static int gui_out_swidth = 2;
static int gui_out_sheight = 2;
static int gui_in_x = 0;
static int gui_in_y = 0;
static int gui_out_x = 0;
static int gui_out_y = 0;
static int pixel_scale_mode = 0;
static int pixel_sample_mode = 0;
static int pixel_process_mode = 1;
//-------------------------------------

//Function prototypes
static void gui_buttons();
static void gui_draw();
static void update_output();
static void palette_draw();
static void palette_labels();
static void preset_save(FILE *f);
static void preset_load(FILE *f);
//-------------------------------------

//Function implementations

void gui_init()
{
   //Load entry palette 
   palette = SLK_palette_load("palette/aurora.pal");

   //Clear layer
   SLK_layer_set_current(0);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
   SLK_draw_rgb_clear();
   SLK_layer_set_current(1);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
   SLK_draw_rgb_clear();
   SLK_layer_set_current(2);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(20,20,20,255));
   SLK_draw_rgb_clear();

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
   elements.save_load_preset = SLK_gui_button_create(158,128,164,14,"Load preset");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load_preset);
   elements.save_save_preset = SLK_gui_button_create(158,160,164,14,"Save preset");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save_preset);
   elements.save_load_folder = SLK_gui_button_create(158,224,164,14,"Select input dir");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load_folder);
   elements.save_save_folder = SLK_gui_button_create(158,256,164,14,"Select output dir");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save_folder);

   //Palette tab
   elements.palette_load = SLK_gui_button_create(158,218,164,14,"Load palette");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_load);
   elements.palette_save = SLK_gui_button_create(158,246,164,14,"Save palette");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_save);
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
   elements.palette_minus_g = SLK_gui_button_create(160,141,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_minus_g);
   elements.palette_plus_g = SLK_gui_button_create(344,141,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_plus_g);
   elements.palette_minus_b = SLK_gui_button_create(160,173,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_minus_b);
   elements.palette_plus_b = SLK_gui_button_create(344,173,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_plus_b);
   elements.palette_bar_r = SLK_gui_slider_create(174,109,170,14,0,255);
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_bar_r);
   elements.palette_bar_g = SLK_gui_slider_create(174,141,170,14,0,255);
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_bar_g);
   elements.palette_bar_b = SLK_gui_slider_create(174,173,170,14,0,255);
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_bar_b);
   label = SLK_gui_label_create(104,112,48,12,"red");
   SLK_gui_vtabbar_add_element(settings_tabs,1,label);
   label = SLK_gui_label_create(104,144,48,12,"green");
   SLK_gui_vtabbar_add_element(settings_tabs,1,label);
   label = SLK_gui_label_create(104,176,48,12,"blue");
   SLK_gui_vtabbar_add_element(settings_tabs,1,label);
   elements.palette_label_r = SLK_gui_label_create(354,112,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_label_r);
   elements.palette_label_g = SLK_gui_label_create(354,144,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_label_g);
   elements.palette_label_b = SLK_gui_label_create(354,176,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,1,elements.palette_label_b);
   palette_labels();

   //General tab
   elements.general_tab_scale = SLK_gui_tabbar_create(99,14,283,14,2,text_tab_scale);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_tab_scale);
   elements.general_width_plus = SLK_gui_button_create(344,35,14,14,"+");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,elements.general_width_plus);
   elements.general_width_minus = SLK_gui_button_create(160,35,14,14,"-");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,elements.general_width_minus);
   elements.general_height_plus = SLK_gui_button_create(344,67,14,14,"+");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,elements.general_height_plus);
   elements.general_height_minus = SLK_gui_button_create(160,67,14,14,"-");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,elements.general_height_minus);
   label = SLK_gui_label_create(104,38,48,12,"Width");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,label);
   label = SLK_gui_label_create(104,70,56,12,"Height");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,label);
   elements.general_bar_width = SLK_gui_slider_create(174,35,170,14,0,256);
   elements.general_bar_width->slider.value = 128;
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,elements.general_bar_width);
   elements.general_bar_height = SLK_gui_slider_create(174,67,170,14,0,256);;
   elements.general_bar_height->slider.value = 128;
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,elements.general_bar_height);
   elements.general_label_width = SLK_gui_label_create(354,38,32,12,"128");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,elements.general_label_width);
   elements.general_label_height = SLK_gui_label_create(354,70,32,12,"128");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,0,elements.general_label_height);
   elements.general_swidth_plus = SLK_gui_button_create(344,35,14,14,"+");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,elements.general_swidth_plus);
   elements.general_swidth_minus = SLK_gui_button_create(160,35,14,14,"-");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,elements.general_swidth_minus);
   elements.general_sheight_plus = SLK_gui_button_create(344,67,14,14,"+");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,elements.general_sheight_plus);
   elements.general_sheight_minus = SLK_gui_button_create(160,67,14,14,"-");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,elements.general_sheight_minus);
   label = SLK_gui_label_create(104,38,48,12,"x pix");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,label);
   label = SLK_gui_label_create(104,70,48,12,"y pix");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,label);
   elements.general_bar_swidth = SLK_gui_slider_create(174,35,170,14,1,32);
   elements.general_bar_swidth->slider.value = 2;
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,elements.general_bar_swidth);
   elements.general_bar_sheight = SLK_gui_slider_create(174,67,170,14,1,32);;
   elements.general_bar_sheight->slider.value = 2;
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,elements.general_bar_sheight);
   elements.general_label_swidth = SLK_gui_label_create(354,38,32,12,"2");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,elements.general_label_swidth);
   elements.general_label_sheight = SLK_gui_label_create(354,70,32,12,"2");
   SLK_gui_tabbar_add_element(elements.general_tab_scale,1,elements.general_label_sheight);
   elements.general_dither_left = SLK_gui_button_create(160,115,14,14,"<");;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_dither_left);
   elements.general_dither_right = SLK_gui_button_create(344,115,14,14,">");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_dither_right);
   label = SLK_gui_label_create(104,118,56,12,"Dither");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_label_dither = SLK_gui_label_create(174,118,170,12,text_dither[1]);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_label_dither);
   label = SLK_gui_label_create(104,150,56,12,"Amount");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_bar_dither = SLK_gui_slider_create(160,147,198,14,0,1000);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_bar_dither);
   elements.general_bar_dither->slider.value = 250;
   label = SLK_gui_label_create(104,198,56,12,"Sample");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_sample_left = SLK_gui_button_create(160,195,14,14,"<");;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_sample_left);
   elements.general_sample_right = SLK_gui_button_create(344,195,14,14,">");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_sample_right);
   elements.general_label_sample = SLK_gui_label_create(174,198,170,12,text_sample[0]);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_label_sample);
   label = SLK_gui_label_create(104,230,56,12,"Gauss");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_gauss_plus = SLK_gui_button_create(344,227,14,14,"+");
   SLK_gui_tabbar_add_element(settings_tabs,2,elements.general_gauss_plus);
   elements.general_gauss_minus = SLK_gui_button_create(160,227,14,14,"-");
   SLK_gui_tabbar_add_element(settings_tabs,2,elements.general_gauss_minus);
   elements.general_bar_gauss = SLK_gui_slider_create(174,227,170,14,1,500);
   elements.general_bar_gauss->slider.value = 80;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_bar_gauss);
   elements.general_label_gauss = SLK_gui_label_create(354,230,32,12,"80");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_label_gauss);
   label = SLK_gui_label_create(104,278,56,12,"Alpha");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_alpha_plus = SLK_gui_button_create(344,275,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_alpha_plus);
   elements.general_alpha_minus = SLK_gui_button_create(160,275,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_alpha_minus);
   elements.general_bar_alpha = SLK_gui_slider_create(174,275,170,14,0,255);
   elements.general_bar_alpha->slider.value = 128;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_bar_alpha);
   elements.general_label_alpha = SLK_gui_label_create(354,278,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_label_alpha);

   //Process tab
   label = SLK_gui_label_create(104,24,56,12,"Bright");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   label = SLK_gui_label_create(104,56,56,12,"Contra");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   label = SLK_gui_label_create(104,88,56,12,"Satura");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   label = SLK_gui_label_create(104,120,56,12,"Gamma");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   label = SLK_gui_label_create(104,152,56,12,"Sharp");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   elements.process_bar_brightness = SLK_gui_slider_create(174,21,162,14,-255,255);
   elements.process_bar_brightness->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_bar_brightness);
   elements.process_label_brightness = SLK_gui_label_create(346,24,40,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_label_brightness);
   elements.process_plus_brightness = SLK_gui_button_create(336,21,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_plus_brightness);
   elements.process_minus_brightness = SLK_gui_button_create(160,21,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_minus_brightness);
   elements.process_bar_contrast = SLK_gui_slider_create(174,53,162,14,-255,255);
   elements.process_bar_contrast->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_bar_contrast);
   elements.process_label_contrast = SLK_gui_label_create(346,56,40,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_label_contrast);
   elements.process_plus_contrast = SLK_gui_button_create(336,53,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_plus_contrast);
   elements.process_minus_contrast = SLK_gui_button_create(160,53,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_minus_contrast);
   elements.process_bar_saturation = SLK_gui_slider_create(174,85,162,14,1,600);
   elements.process_bar_saturation->slider.value = 100;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_bar_saturation);
   elements.process_label_saturation = SLK_gui_label_create(346,88,40,12,"100");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_label_saturation);
   elements.process_plus_saturation = SLK_gui_button_create(336,85,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_plus_saturation);
   elements.process_minus_saturation = SLK_gui_button_create(160,85,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_minus_saturation);
   elements.process_bar_gamma = SLK_gui_slider_create(174,118,162,14,1,800);
   elements.process_bar_gamma->slider.value = 100;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_bar_gamma);
   elements.process_label_gamma = SLK_gui_label_create(346,120,40,12,"100");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_label_gamma);
   elements.process_plus_gamma = SLK_gui_button_create(336,118,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_plus_gamma);
   elements.process_minus_gamma = SLK_gui_button_create(160,118,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_minus_gamma);
   elements.process_bar_sharpen = SLK_gui_slider_create(174,150,162,14,0,100);
   elements.process_bar_sharpen->slider.value = 0;
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_bar_sharpen);
   elements.process_label_sharpen = SLK_gui_label_create(346,152,40,12,"0");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_label_sharpen);
   elements.process_plus_sharpen = SLK_gui_button_create(336,150,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_plus_sharpen);
   elements.process_minus_sharpen = SLK_gui_button_create(160,150,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,3,elements.process_minus_sharpen);

   //Special tab
   elements.special_gif_load = SLK_gui_button_create(158,32,164,14,"Load gif");
   SLK_gui_vtabbar_add_element(settings_tabs,19,elements.special_gif_load);
   elements.special_gif_save = SLK_gui_button_create(158,64,164,14,"Save gif");
   SLK_gui_vtabbar_add_element(settings_tabs,19,elements.special_gif_save);
   //-------------------------------------
}

void gui_update()
{
   int mx,my;
   SLK_mouse_get_layer_pos(2,&mx,&my);
   if(settings->moveable!=2)
      SLK_gui_window_update_input(preview,SLK_mouse_get_state(SLK_BUTTON_LEFT),SLK_mouse_get_state(SLK_BUTTON_RIGHT),mx,my);
   if(preview->moveable!=2)
      SLK_gui_window_update_input(settings,SLK_mouse_get_state(SLK_BUTTON_LEFT),SLK_mouse_get_state(SLK_BUTTON_RIGHT),mx,my);
   gui_buttons();

   if(SLK_key_down(SLK_KEY_LEFT))
      if(gui_out_width>0)
         gui_out_width-=8;
   if(SLK_key_down(SLK_KEY_RIGHT))
      if(gui_out_width<256)
         gui_out_width+=8;
   if(SLK_key_down(SLK_KEY_DOWN))
      if(gui_out_height>0)
         gui_out_height-=8;
   if(SLK_key_down(SLK_KEY_UP))
      if(gui_out_height<256)
         gui_out_height+=8;

   gui_draw();
}

//Big boy function
static void gui_buttons()
{
   int mx,my;
   SLK_mouse_get_layer_pos(2,&mx,&my);
   int update = 0;

   switch(settings_tabs->vtabbar.current_tab)
   {
   case 0: //Save/Load tab
      if(elements.save_load->button.state.released)
      {
         SLK_rgb_sprite_destroy(sprite_in);
         SLK_rgb_sprite_destroy(sprite_in_org);
         sprite_in = image_select();
         if(sprite_in)
         {
            sprite_in_org = SLK_rgb_sprite_create(sprite_in->width,sprite_in->height);
            SLK_rgb_sprite_copy(sprite_in_org,sprite_in);
            SLK_layer_set_size(0,sprite_in->width,sprite_in->height);
            SLK_layer_set_current(0);
            SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
            SLK_draw_rgb_clear();
            SLK_draw_rgb_sprite(sprite_in,0,0);
            SLK_draw_rgb_set_changed(1);
            lowpass_image(sprite_in_org,sprite_in);
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
      else if(elements.save_save->button.state.released)
      {
         image_write(sprite_out,palette);
         elements.save_save->button.state.released = 0;
      }
      else if(elements.save_load_preset->button.state.released)
      {
         FILE *f = json_select();
         preset_load(f);
         elements.save_load_preset->button.state.released = 0;
      }
      else if(elements.save_save_preset->button.state.released)
      {
         FILE *f = json_write();
         preset_save(f);
         elements.save_save_preset->button.state.released = 0;
      }
      else if(elements.save_save_folder->button.state.released)
      {
         if(pixel_scale_mode==0)
            dir_output_select(pixel_process_mode,pixel_sample_mode,pixel_scale_mode,gui_out_width,gui_out_height,palette);
         else
            dir_output_select(pixel_process_mode,pixel_sample_mode,pixel_scale_mode,gui_out_swidth,gui_out_sheight,palette);
         elements.save_save_folder->button.state.released = 0;
      }
      else if(elements.save_load_folder->button.state.released)
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
            if(palette)
               free(palette);
            palette = p;
            update = 1;
            elements.palette_bar_r->slider.value = p->colors[palette_selected].r;
            elements.palette_bar_g->slider.value = p->colors[palette_selected].g;
            elements.palette_bar_b->slider.value = p->colors[palette_selected].b;
            palette_draw();
            palette_labels();
         }
         elements.palette_load->button.state.released = 0;
      }
      else if(elements.palette_save->button.state.released)
      {
         palette_write(palette);
         elements.palette_save->button.state.released = 0;
      }
      else if(elements.palette_button->icon.state.pressed)
      {
         int nx = mx-settings->pos.x-elements.palette_button->icon.pos.x;
         int ny = my-settings->pos.y-elements.palette_button->icon.pos.y;
         palette_selected = MIN(255,(ny/9)*31+nx/9);
         palette_labels();
      }
      if(elements.palette_minus_r->button.state.pressed&&palette->colors[palette_selected].r>0)
         elements.palette_bar_r->slider.value--;
      else if(elements.palette_plus_r->button.state.pressed&&palette->colors[palette_selected].r<255)
         elements.palette_bar_r->slider.value++;
      else if(elements.palette_minus_g->button.state.pressed&&palette->colors[palette_selected].g>0)
         elements.palette_bar_g->slider.value--;
      else if(elements.palette_plus_g->button.state.pressed&&palette->colors[palette_selected].g<255)
         elements.palette_bar_g->slider.value++;
      else if(elements.palette_minus_b->button.state.pressed&&palette->colors[palette_selected].b>0)
         elements.palette_bar_b->slider.value--;
      else if(elements.palette_plus_b->button.state.pressed&&palette->colors[palette_selected].b<255)
         elements.palette_bar_b->slider.value++;
      if(elements.palette_bar_r->slider.value!=palette->colors[palette_selected].r||elements.palette_bar_g->slider.value!=palette->colors[palette_selected].g||elements.palette_bar_b->slider.value!=palette->colors[palette_selected].b)
      {
         palette->colors[palette_selected].r = elements.palette_bar_r->slider.value;
         palette->colors[palette_selected].g = elements.palette_bar_g->slider.value;
         palette->colors[palette_selected].b = elements.palette_bar_b->slider.value;
         if(palette_selected!=0)
            palette->colors[palette_selected].a = 255;
         palette->used = MAX(palette->used,palette_selected+1);
         palette_draw();
         palette_labels();
         update = 1;
      }
      break;
   case 2: //General tab
      if(elements.general_width_plus->button.state.pressed&&elements.general_bar_width->slider.value<elements.general_bar_width->slider.max)
         elements.general_bar_width->slider.value++;
      else if(elements.general_width_minus->button.state.pressed&&elements.general_bar_width->slider.value>elements.general_bar_width->slider.min)
         elements.general_bar_width->slider.value--;
      else if(elements.general_height_plus->button.state.pressed&&elements.general_bar_height->slider.value<elements.general_bar_height->slider.max)
         elements.general_bar_height->slider.value++;
      else if(elements.general_height_minus->button.state.pressed&&elements.general_bar_height->slider.value>elements.general_bar_height->slider.min)
         elements.general_bar_height->slider.value--;
      else if(elements.general_swidth_plus->button.state.pressed&&elements.general_bar_swidth->slider.value<elements.general_bar_swidth->slider.max)
         elements.general_bar_swidth->slider.value++;
      else if(elements.general_swidth_minus->button.state.pressed&&elements.general_bar_swidth->slider.value>elements.general_bar_swidth->slider.min)
         elements.general_bar_swidth->slider.value--;
      else if(elements.general_sheight_plus->button.state.pressed&&elements.general_bar_sheight->slider.value<elements.general_bar_sheight->slider.max)
         elements.general_bar_sheight->slider.value++;
      else if(elements.general_sheight_minus->button.state.pressed&&elements.general_bar_sheight->slider.value>elements.general_bar_sheight->slider.min)
         elements.general_bar_sheight->slider.value--;
      else if(elements.general_alpha_plus->button.state.pressed&&elements.general_bar_alpha->slider.value<elements.general_bar_alpha->slider.max)
         elements.general_bar_alpha->slider.value++;
      else if(elements.general_alpha_minus->button.state.pressed&&elements.general_bar_alpha->slider.value>elements.general_bar_alpha->slider.min)
         elements.general_bar_alpha->slider.value--;
      else if(elements.general_gauss_plus->button.state.pressed&&elements.general_bar_gauss->slider.value<elements.general_bar_gauss->slider.max)
         elements.general_bar_gauss->slider.value++;
      else if(elements.general_gauss_minus->button.state.pressed&&elements.general_bar_gauss->slider.value>elements.general_bar_gauss->slider.min)
         elements.general_bar_gauss->slider.value--;
      if(elements.general_dither_left->button.state.pressed)
      {
         pixel_process_mode--;
         if(pixel_process_mode<0)
            pixel_process_mode = 4;
         update = 1;
         SLK_gui_label_set_text(elements.general_label_dither,text_dither[pixel_process_mode]);
      }
      else if(elements.general_dither_right->button.state.pressed)
      {
         pixel_process_mode++;
         if(pixel_process_mode>4)
            pixel_process_mode = 0;
         update = 1;
         SLK_gui_label_set_text(elements.general_label_dither,text_dither[pixel_process_mode]);
      }
      if(elements.general_bar_width->slider.value!=gui_out_width)
      {
         gui_out_width = elements.general_bar_width->slider.value;
         char tmp[16];
         sprintf(tmp,"%d",gui_out_width);
         SLK_gui_label_set_text(elements.general_label_width,tmp);
         update = 1;
      }
      if(elements.general_bar_height->slider.value!=gui_out_height)
      {
         gui_out_height = elements.general_bar_height->slider.value;
         char tmp[16];
         sprintf(tmp,"%d",gui_out_height);
         SLK_gui_label_set_text(elements.general_label_height,tmp);
         update = 1;
      }
      if(elements.general_bar_swidth->slider.value!=gui_out_swidth)
      {
         gui_out_swidth = elements.general_bar_swidth->slider.value;
         char tmp[16];
         sprintf(tmp,"%d",gui_out_swidth);
         SLK_gui_label_set_text(elements.general_label_swidth,tmp);
         update = 1;
      }
      if(elements.general_bar_sheight->slider.value!=gui_out_sheight)
      {
         gui_out_sheight = elements.general_bar_sheight->slider.value;
         char tmp[16];
         sprintf(tmp,"%d",gui_out_sheight);
         SLK_gui_label_set_text(elements.general_label_sheight,tmp);
         update = 1;
      }

      if(elements.general_bar_dither->slider.value!=dither_amount)
      {
         dither_amount = elements.general_bar_dither->slider.value;
         update = 1;
      }
      if(elements.general_sample_left->button.state.pressed)
      {
         pixel_sample_mode--;
         if(pixel_sample_mode<0)
            pixel_sample_mode = 6;
         update = 1;
         SLK_gui_label_set_text(elements.general_label_sample,text_sample[pixel_sample_mode]);
      }
      else if(elements.general_sample_right->button.state.pressed)
      {
         pixel_sample_mode++;
         if(pixel_sample_mode>6)
            pixel_sample_mode = 0;
         update = 1;
         SLK_gui_label_set_text(elements.general_label_sample,text_sample[pixel_sample_mode]);
      }
      if(elements.general_bar_alpha->slider.value!=alpha_threshold)
      {
         alpha_threshold = elements.general_bar_alpha->slider.value;
         char tmp[16];
         sprintf(tmp,"%d",alpha_threshold);
         SLK_gui_label_set_text(elements.general_label_alpha,tmp);
         update = 1;
      }
      if(elements.general_bar_gauss->slider.value!=gauss)
      {
         gauss = elements.general_bar_gauss->slider.value;
         char tmp[16];
         sprintf(tmp,"%d",gauss);
         SLK_gui_label_set_text(elements.general_label_gauss,tmp);
         lowpass_image(sprite_in_org,sprite_in);
         sharpen_image(sprite_in,sprite_in);
         update = 1;
      }
      if(elements.general_tab_scale->tabbar.current_tab!=pixel_scale_mode)
      {
         pixel_scale_mode = elements.general_tab_scale->tabbar.current_tab;
         update = 1;
      }
      break;
   case 3: //Process tab
      if(elements.process_minus_brightness->button.state.pressed&&elements.process_bar_brightness->slider.value>elements.process_bar_brightness->slider.min)
         elements.process_bar_brightness->slider.value--;
      else if(elements.process_plus_brightness->button.state.pressed&&elements.process_bar_brightness->slider.value<elements.process_bar_brightness->slider.max)
         elements.process_bar_brightness->slider.value++;
      if(elements.process_minus_contrast->button.state.pressed&&elements.process_bar_contrast->slider.value>elements.process_bar_contrast->slider.min)
         elements.process_bar_contrast->slider.value--;
      else if(elements.process_plus_contrast->button.state.pressed&&elements.process_bar_contrast->slider.value<elements.process_bar_contrast->slider.max)
         elements.process_bar_contrast->slider.value++;
      if(elements.process_minus_saturation->button.state.pressed&&elements.process_bar_saturation->slider.value>elements.process_bar_saturation->slider.min)
         elements.process_bar_saturation->slider.value--;
      else if(elements.process_plus_saturation->button.state.pressed&&elements.process_bar_saturation->slider.value<elements.process_bar_saturation->slider.max)
         elements.process_bar_saturation->slider.value++;
      if(elements.process_minus_gamma->button.state.pressed&&elements.process_bar_gamma->slider.value>elements.process_bar_gamma->slider.min)
         elements.process_bar_gamma->slider.value--;
      else if(elements.process_plus_gamma->button.state.pressed&&elements.process_bar_gamma->slider.value<elements.process_bar_gamma->slider.max)
         elements.process_bar_gamma->slider.value++;
      if(elements.process_minus_sharpen->button.state.pressed&&elements.process_bar_sharpen->slider.value>elements.process_bar_sharpen->slider.min)
         elements.process_bar_sharpen->slider.value--;
      else if(elements.process_plus_sharpen->button.state.pressed&&elements.process_bar_sharpen->slider.value<elements.process_bar_sharpen->slider.max)
         elements.process_bar_sharpen->slider.value++;
      if(brightness!=elements.process_bar_brightness->slider.value)
      {
         brightness = elements.process_bar_brightness->slider.value;
         char ctmp[16];
         sprintf(ctmp,"%d",brightness);
         SLK_gui_label_set_text(elements.process_label_brightness,ctmp);
         update = 1;
      }
      if(contrast!=elements.process_bar_contrast->slider.value)
      {
         contrast = elements.process_bar_contrast->slider.value;
         char ctmp[16];
         sprintf(ctmp,"%d",contrast);
         SLK_gui_label_set_text(elements.process_label_contrast,ctmp);
         update = 1;
      }
      if(saturation!=elements.process_bar_saturation->slider.value)
      {
         saturation = elements.process_bar_saturation->slider.value;
         char ctmp[16];
         sprintf(ctmp,"%d",saturation);
         SLK_gui_label_set_text(elements.process_label_saturation,ctmp);
         update = 1;
      }
      if(img_gamma!=elements.process_bar_gamma->slider.value)
      {
         img_gamma = elements.process_bar_gamma->slider.value;
         char ctmp[16];
         sprintf(ctmp,"%d",img_gamma);
         SLK_gui_label_set_text(elements.process_label_gamma,ctmp);
         update = 1;
      }
      if(sharpen!=elements.process_bar_sharpen->slider.value)
      {
         sharpen = elements.process_bar_sharpen->slider.value;
         char ctmp[16];
         sprintf(ctmp,"%d",sharpen);
         SLK_gui_label_set_text(elements.process_label_sharpen,ctmp);
         lowpass_image(sprite_in_org,sprite_in);
         sharpen_image(sprite_in,sprite_in);
         update = 1;
      }
      break;
   case 19: //Special tab
      if(elements.special_gif_save->button.state.released)
      {
         if(pixel_scale_mode==0)
            gif_output_select(pixel_process_mode,pixel_sample_mode,pixel_scale_mode,gui_out_width,gui_out_height,palette);
         else
            gif_output_select(pixel_process_mode,pixel_sample_mode,pixel_scale_mode,gui_out_swidth,gui_out_sheight,palette);
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
   if(settings_tabs->vtabbar.current_tab==1)
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
   if(pixel_scale_mode==0)
   {
      width = gui_out_width;
      height = gui_out_height;
   }
   else
   {
      width = sprite_in->width/gui_out_swidth;
      height = sprite_in->height/gui_out_sheight;
   }

   if(sprite_out==NULL||sprite_out->width!=width||sprite_out->height!=height)
   {
      SLK_rgb_sprite_destroy(sprite_out);
      sprite_out = SLK_rgb_sprite_create(width,height);
      SLK_layer_set_size(1,sprite_out->width,sprite_out->height);
   }

   if(sprite_in==NULL)
      return;

   process_image(sprite_in,sprite_out,palette,pixel_sample_mode,pixel_process_mode);

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
         if(y*31+x<palette->used)
            SLK_draw_rgb_fill_rectangle(x*9,y*9,9,9,palette->colors[y*31+x]);

   SLK_draw_rgb_set_target(old);
   SLK_gui_image_update(elements.palette_palette,elements.palette_sprite,(SLK_gui_rectangle){0,0,279,81});
}

static void palette_labels()
{
   char ctmp[16];
   sprintf(ctmp,"%d",palette->colors[palette_selected].r);
   SLK_gui_label_set_text(elements.palette_label_r,ctmp);
   elements.palette_bar_r->slider.value = palette->colors[palette_selected].r;
   sprintf(ctmp,"%d",palette->colors[palette_selected].g);
   SLK_gui_label_set_text(elements.palette_label_g,ctmp);
   elements.palette_bar_g->slider.value = palette->colors[palette_selected].g;
   sprintf(ctmp,"%d",palette->colors[palette_selected].b);
   SLK_gui_label_set_text(elements.palette_label_b,ctmp);
   elements.palette_bar_b->slider.value = palette->colors[palette_selected].b;
}

void preset_save(FILE *f)
{
   if(!f)
      return;

   ULK_json5_root *root = ULK_json_create_root();
   ULK_json5 object = ULK_json_create_object();
   ULK_json_object_add_integer(&object,"used",palette->used);
   ULK_json5 array = ULK_json_create_array();
   for(int i = 0;i<256;i++)
      ULK_json_array_add_integer(&array,palette->colors[i].n);
   ULK_json_object_add_object(&object,"colors",array);
   ULK_json_object_add_object(&root->root,"palette",object);
   ULK_json_object_add_integer(&root->root,"width",gui_out_width);
   ULK_json_object_add_integer(&root->root,"height",gui_out_height);
   ULK_json_object_add_integer(&root->root,"swidth",gui_out_swidth);
   ULK_json_object_add_integer(&root->root,"sheight",gui_out_sheight);
   ULK_json_object_add_integer(&root->root,"scale_mode",pixel_scale_mode);
   ULK_json_object_add_integer(&root->root,"dither_mode",pixel_process_mode);
   ULK_json_object_add_integer(&root->root,"dither_amount",dither_amount);
   ULK_json_object_add_integer(&root->root,"sample_mode",pixel_sample_mode);
   ULK_json_object_add_integer(&root->root,"brightness",brightness);
   ULK_json_object_add_integer(&root->root,"contrast",contrast);
   ULK_json_object_add_integer(&root->root,"gamma",img_gamma);
   ULK_json_object_add_integer(&root->root,"saturation",saturation);
   ULK_json_object_add_integer(&root->root,"sharpness",sharpen);
 
   ULK_json_write_file(f,&root->root);
   ULK_json_free(root);
   fclose(f);
}

void preset_load(FILE *f)
{
   if(!f)
      return;

   //The unnecessary indentations are for my own sanity
   ULK_json5 fallback = {0};
   ULK_json5_root *root = ULK_json_parse_file_stream(f);
   fclose(f);
   ULK_json5 *o = ULK_json_get_object_object(&root->root,"palette",&fallback);
   palette->used = ULK_json_get_object_integer(o,"used",0);
   ULK_json5 *array = ULK_json_get_object(o,"colors");
   for(int i = 0;i<256;i++)
      palette->colors[i].n = ULK_json_get_array_integer(array,i,0);
   elements.palette_bar_r->slider.value = palette->colors[palette_selected].r;
   elements.palette_bar_g->slider.value = palette->colors[palette_selected].g;
   elements.palette_bar_b->slider.value = palette->colors[palette_selected].b;
   palette_draw();
   palette_labels();
   elements.general_bar_width->slider.value = ULK_json_get_object_integer(&root->root,"width",1);
      gui_out_width = elements.general_bar_width->slider.value;
      char ctmp[16];
      sprintf(ctmp,"%d",gui_out_width);
      SLK_gui_label_set_text(elements.general_label_width,ctmp);
   elements.general_bar_height->slider.value = ULK_json_get_object_integer(&root->root,"height",1);
      gui_out_height = elements.general_bar_height->slider.value;
      sprintf(ctmp,"%d",gui_out_height);
      SLK_gui_label_set_text(elements.general_label_height,ctmp);
   elements.general_bar_swidth->slider.value = ULK_json_get_object_integer(&root->root,"swidth",1);
      gui_out_swidth = elements.general_bar_swidth->slider.value;
      sprintf(ctmp,"%d",gui_out_swidth);
      SLK_gui_label_set_text(elements.general_label_swidth,ctmp);
   elements.general_bar_sheight->slider.value = ULK_json_get_object_integer(&root->root,"sheight",1);
      gui_out_sheight = elements.general_bar_sheight->slider.value;
      sprintf(ctmp,"%d",gui_out_sheight);
      SLK_gui_label_set_text(elements.general_label_sheight,ctmp);
   elements.general_tab_scale->tabbar.current_tab = ULK_json_get_object_integer(&root->root,"scale_mode",1);
      pixel_scale_mode = elements.general_tab_scale->tabbar.current_tab;
   pixel_process_mode = ULK_json_get_object_integer(&root->root,"dither_mode",0);
   SLK_gui_label_set_text(elements.general_label_dither,text_dither[pixel_process_mode]);
   elements.general_bar_dither->slider.value = ULK_json_get_object_integer(&root->root,"dither_amount",1);
   pixel_sample_mode = ULK_json_get_object_integer(&root->root,"sample_mode",0);
      SLK_gui_label_set_text(elements.general_label_sample,text_sample[pixel_sample_mode]);
   elements.process_bar_brightness->slider.value = ULK_json_get_object_integer(&root->root,"brightness",0);
      brightness = elements.process_bar_brightness->slider.value;
      sprintf(ctmp,"%d",brightness);
      SLK_gui_label_set_text(elements.process_label_brightness,ctmp);
   elements.process_bar_contrast->slider.value = ULK_json_get_object_integer(&root->root,"contrast",0);
      contrast = elements.process_bar_contrast->slider.value;
      sprintf(ctmp,"%d",contrast);
      SLK_gui_label_set_text(elements.process_label_contrast,ctmp);
   elements.process_bar_saturation->slider.value = ULK_json_get_object_integer(&root->root,"saturation",0);
      saturation = elements.process_bar_saturation->slider.value;
      sprintf(ctmp,"%d",saturation);
      SLK_gui_label_set_text(elements.process_label_saturation,ctmp);
   elements.process_bar_gamma->slider.value = ULK_json_get_object_integer(&root->root,"gamma",0);
      img_gamma = elements.process_bar_gamma->slider.value;
      sprintf(ctmp,"%d",img_gamma);
      SLK_gui_label_set_text(elements.process_label_gamma,ctmp);
   elements.process_bar_sharpen->slider.value = ULK_json_get_object_integer(&root->root,"sharpness",0);
      sharpen = elements.process_bar_sharpen->slider.value;
      sprintf(ctmp,"%d",sharpen);
      SLK_gui_label_set_text(elements.process_label_sharpen,ctmp);
      sharpen_image(sprite_in_org,sprite_in);
   update_output();
   ULK_json_free(root);
}
//-------------------------------------
