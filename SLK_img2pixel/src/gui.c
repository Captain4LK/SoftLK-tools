/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <SLK/SLK.h>
#include <SLK/SLK_gui.h>
#include "../../external/tinyfiledialogs.h"
#include "../../external/UtilityLK/include/ULK_json.h"
#include "../../external/cute_files.h"
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
static SLK_RGB_sprite *sprite_out = NULL;
static int palette_selected = 0;

static SLK_gui_window *preview = NULL;
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
   SLK_gui_element *general_width_plus;
   SLK_gui_element *general_width_minus;
   SLK_gui_element *general_height_plus;
   SLK_gui_element *general_height_minus;
   SLK_gui_element *general_dither_left;
   SLK_gui_element *general_dither_right;
   SLK_gui_element *general_sample_left;
   SLK_gui_element *general_sample_right;
   SLK_gui_element *general_bar_width;
   SLK_gui_element *general_bar_height;
   SLK_gui_element *general_bar_dither;
   SLK_gui_element *general_label_width;
   SLK_gui_element *general_label_height;
   SLK_gui_element *general_label_dither;
   SLK_gui_element *general_label_sample;

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
};
static struct Elements elements = {0};

//Preview window
static SLK_gui_element *image_in = NULL;
static SLK_gui_element *image_out = NULL;

static const char *text_dither[] = 
{
   "No dithering",
   "Ordered",
   "FloydSteinberg 1",
   "FloydSteinberg 2",
};

static const char *text_sample[] = 
{
   "Round",
   "Floor",
   "Ceil",
   "Linear",
};

static const char *text_tab_image[] = 
{
   "Input",
   "Output",
};

static const char *text_tab_settings[] = 
{
   "Save/Load",
   "Palette",
   "General",
   "Process",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
   "Reserved",
};

static SLK_Palette *palette = NULL;
static int gui_out_width = 128;
static int gui_out_height = 128;
static int pixel_sample_mode = 0;
static int pixel_process_mode = 1;
//-------------------------------------

//Function prototypes
static void gui_buttons();
static void gui_draw();
static void update_output();
static void palette_draw();
static void palette_labels();
static void preset_save(const char *path);
static void preset_load(const char *path);
//-------------------------------------

//Function implementations

void gui_init()
{
   //Load entry palette 
   palette = SLK_palette_load("palette/aurora.pal");

   //Clear layer
   SLK_layer_set_current(0);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(20,20,20,255));
   SLK_draw_rgb_clear();

   //Setup windows
   //Preview window
   SLK_gui_element *label;
   SLK_gui_set_colors(SLK_color_create(90,90,90,255),SLK_color_create(200,200,200,255),SLK_color_create(100,100,100,255),SLK_color_create(50,50,50,255),SLK_color_create(0,0,0,255));
   SLK_gui_set_font(font);
   SLK_gui_set_font_color(SLK_color_create(0,0,0,255));
   preview = SLK_gui_window_create(384,100,260,286);
   SLK_gui_window_set_title(preview,"Preview");
   SLK_gui_window_set_moveable(preview,1);
   SLK_gui_element *tabbar = SLK_gui_tabbar_create(2,14,256,14,2,text_tab_image);
   SLK_RGB_sprite *tmp = SLK_rgb_sprite_create(1,1);
   image_in = SLK_gui_image_create(2,28,256,256,tmp,(SLK_gui_rectangle){0,0,1,1});
   SLK_gui_tabbar_add_element(tabbar,0,image_in);
   image_out = SLK_gui_image_create(2,28,256,256,tmp,(SLK_gui_rectangle){0,0,1,1});
   SLK_gui_tabbar_add_element(tabbar,1,image_out);
   SLK_gui_window_add_element(preview,tabbar);
   
   //Gui window
   settings = SLK_gui_window_create(10,10,384,294);
   SLK_gui_window_set_title(settings,"Settings");
   SLK_gui_window_set_moveable(settings,1);
   settings_tabs = SLK_gui_vtabbar_create(2,14,96,20,text_tab_settings);
   SLK_gui_window_add_element(settings,settings_tabs);
   //Save/Load tab
   elements.save_load = SLK_gui_button_create(158,64,164,14,"Load image");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load);
   elements.save_save = SLK_gui_button_create(158,198,164,14,"Save image");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save);
   elements.save_load_preset = SLK_gui_button_create(158,92,164,14,"Load preset");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load_preset);
   elements.save_save_preset = SLK_gui_button_create(158,226,164,14,"Save preset");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save_preset);
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
   elements.general_width_plus = SLK_gui_button_create(344,21,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_width_plus);
   elements.general_width_minus = SLK_gui_button_create(160,21,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_width_minus);
   elements.general_height_plus = SLK_gui_button_create(344,53,14,14,"+");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_height_plus);
   elements.general_height_minus = SLK_gui_button_create(160,53,14,14,"-");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_height_minus);
   label = SLK_gui_label_create(104,24,48,12,"Width");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   label = SLK_gui_label_create(104,56,56,12,"Height");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_bar_width = SLK_gui_slider_create(174,21,170,14,0,256);
   elements.general_bar_width->slider.value = 128;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_bar_width);
   elements.general_bar_height = SLK_gui_slider_create(174,53,170,14,0,256);;
   elements.general_bar_height->slider.value = 128;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_bar_height);
   elements.general_label_width = SLK_gui_label_create(354,24,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_label_width);
   elements.general_label_height = SLK_gui_label_create(354,56,32,12,"128");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_label_height);
   elements.general_dither_left = SLK_gui_button_create(160,125,14,14,"<");;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_dither_left);
   elements.general_dither_right = SLK_gui_button_create(344,125,14,14,">");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_dither_right);
   label = SLK_gui_label_create(104,128,56,12,"Dither");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_label_dither = SLK_gui_label_create(174,128,170,12,text_dither[1]);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_label_dither);
   label = SLK_gui_label_create(104,160,56,12,"Amount");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_bar_dither = SLK_gui_slider_create(160,157,198,14,0,1000);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_bar_dither);
   elements.general_bar_dither->slider.value = 250;
   label = SLK_gui_label_create(104,232,56,12,"Sample");
   SLK_gui_vtabbar_add_element(settings_tabs,2,label);
   elements.general_sample_left = SLK_gui_button_create(160,229,14,14,"<");;
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_sample_left);
   elements.general_sample_right = SLK_gui_button_create(344,229,14,14,">");
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_sample_right);
   elements.general_label_sample = SLK_gui_label_create(174,232,170,12,text_sample[0]);
   SLK_gui_vtabbar_add_element(settings_tabs,2,elements.general_label_sample);
   //Process tab
   label = SLK_gui_label_create(104,24,56,12,"Bright");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   label = SLK_gui_label_create(104,56,56,12,"Contra");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   label = SLK_gui_label_create(104,88,56,12,"Satura");
   SLK_gui_vtabbar_add_element(settings_tabs,3,label);
   label = SLK_gui_label_create(104,120,56,12,"Gamma");
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
}

void gui_update()
{
   int mx,my;
   SLK_mouse_get_layer_pos(0,&mx,&my);
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

static void gui_buttons()
{
   int mx,my;
   SLK_mouse_get_layer_pos(0,&mx,&my);
   int update = 0;
   switch(settings_tabs->vtabbar.current_tab)
   {
   case 0: //Save/Load tab
      if(elements.save_load->button.state.released)
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_openFileDialog("Select a png file","",0,filter_patterns,NULL,0);
         SLK_rgb_sprite_destroy(sprite_in);
         sprite_in = image_load(file_path);
         if(sprite_in)
            SLK_gui_image_update(image_in,sprite_in,(SLK_gui_rectangle){0,0,sprite_in->width,sprite_in->height});
         update = 1;
         elements.save_load->button.state.released = 0;
      }
      else if(elements.save_save->button.state.released)
      {
         const char *filter_patterns[2] = {"*.png","*.slk"};
         const char *file_path = tinyfd_saveFileDialog("Save image","",2,filter_patterns,NULL);
         image_save(file_path,sprite_out,palette);
         elements.save_save->button.state.released = 0;
      }
      else if(elements.save_load_preset->button.state.released)
      {
         const char *filter_patterns[2] = {"*.json"};
         const char *file_path = tinyfd_openFileDialog("Select a preset","",1,filter_patterns,NULL,0);
         preset_load(file_path);
         elements.save_load_preset->button.state.released = 0;
      }
      else if(elements.save_save_preset->button.state.released)
      {
         const char *filter_patterns[2] = {"*.json"};
         const char *file_path = tinyfd_saveFileDialog("Save preset","",1,filter_patterns,NULL);
         preset_save(file_path);
         elements.save_save_preset->button.state.released = 0;
      }
      break;
   case 1: //Palette tab
      if(elements.palette_load->button.state.released)
      {
         const char *filter_patterns[2] = {"*.pal"};
         const char *file_path = tinyfd_openFileDialog("Load a palette","",1,filter_patterns,NULL,0);
         if(file_path!=NULL)
         {
            if(palette)
               free(palette);
            palette = SLK_palette_load(file_path);
            update = 1;
            palette_draw();
            palette_labels();
         }
         elements.palette_load->button.state.released = 0;
      }
      else if(elements.palette_save->button.state.released)
      {
         const char *filter_patterns[2] = {"*.pal"};
         const char *file_path = tinyfd_saveFileDialog("Save palette","",1,filter_patterns,NULL);
         SLK_palette_save(file_path,palette);
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
      if(elements.general_dither_left->button.state.pressed)
      {
         pixel_process_mode--;
         if(pixel_process_mode<0)
            pixel_process_mode = 3;
         update = 1;
         SLK_gui_label_set_text(elements.general_label_dither,text_dither[pixel_process_mode]);
      }
      else if(elements.general_dither_right->button.state.pressed)
      {
         pixel_process_mode++;
         if(pixel_process_mode>3)
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
      if(elements.general_bar_dither->slider.value!=dither_amount)
      {
         dither_amount = elements.general_bar_dither->slider.value;
         update = 1;
      }
      if(elements.general_sample_left->button.state.pressed)
      {
         pixel_sample_mode--;
         if(pixel_sample_mode<0)
            pixel_sample_mode = 2;
         update = 1;
         SLK_gui_label_set_text(elements.general_label_sample,text_sample[pixel_sample_mode]);
      }
      else if(elements.general_sample_right->button.state.pressed)
      {
         pixel_sample_mode++;
         if(pixel_sample_mode>2)
            pixel_sample_mode = 0;
         update = 1;
         SLK_gui_label_set_text(elements.general_label_sample,text_sample[pixel_sample_mode]);
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
      break;
   }

   if(update)
      update_output(); 
}

static void gui_draw()
{
   SLK_layer_set_current(0);
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
   SLK_gui_window_draw(preview);
}

static void update_output()
{
   if(sprite_out==NULL||sprite_out->width!=gui_out_width||sprite_out->height!=gui_out_height)
   {
      SLK_rgb_sprite_destroy(sprite_out);
      sprite_out = SLK_rgb_sprite_create(gui_out_width,gui_out_height);
   }

   if(sprite_in==NULL)
      return;

   process_image(sprite_in,sprite_out,palette,pixel_sample_mode,pixel_process_mode);
   SLK_gui_image_update(image_out,sprite_out,(SLK_gui_rectangle){0,0,sprite_out->width,sprite_out->height});
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

void preset_save(const char *path)
{
   if(path==NULL)
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
   ULK_json_object_add_integer(&root->root,"dither_mode",pixel_process_mode);
   ULK_json_object_add_integer(&root->root,"dither_amount",dither_amount);
   ULK_json_object_add_integer(&root->root,"sample_mode",pixel_sample_mode);
   ULK_json_object_add_integer(&root->root,"brightness",brightness);
   ULK_json_object_add_integer(&root->root,"contrast",contrast);
   ULK_json_object_add_integer(&root->root,"gamma",img_gamma);
   ULK_json_object_add_integer(&root->root,"saturation",saturation);
 
   FILE *f = fopen(path,"w");
   ULK_json_write_file(f,&root->root);
   ULK_json_free(root);
   fclose(f);
}

void preset_load(const char *path)
{
   if(path==NULL)
      return;

   ULK_json5 fallback = {0};
   ULK_json5_root *root = ULK_json_parse_file(path);
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
   elements.general_bar_height->slider.value = ULK_json_get_object_integer(&root->root,"height",1);
      gui_out_height = elements.general_bar_height->slider.value;
   pixel_process_mode = ULK_json_get_object_integer(&root->root,"dither_mode",0);
   SLK_gui_label_set_text(elements.general_label_dither,text_dither[pixel_process_mode]);
   elements.general_bar_dither->slider.value = ULK_json_get_object_integer(&root->root,"dither_amount",1);
   pixel_sample_mode = ULK_json_get_object_integer(&root->root,"sample_mode",0);
      SLK_gui_label_set_text(elements.general_label_sample,text_sample[pixel_sample_mode]);
   elements.process_bar_brightness->slider.value = ULK_json_get_object_integer(&root->root,"brightness",0);
      brightness = elements.process_bar_brightness->slider.value;
      char ctmp[16];
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
   update_output();
   ULK_json_free(root);
}
//-------------------------------------
