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
//-------------------------------------

//Internal includes
#include "assets.h"
#include "gui.h"
#include "process.h"
#include "sample.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//Lots of globals here, I need to find a way to manage this better
static int gui_pos_x = 0;
static int gui_pos_y = 0;
static SLK_RGB_sprite *sprite_in = NULL;
static SLK_RGB_sprite *sprite_out = NULL;
static SLK_Palette *palette = NULL;
static int gui_out_width = 128;
static int gui_out_height = 128;
static int pixel_sample_mode = 0;
static int pixel_process_mode = 1;

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

   //General tab
   SLK_gui_element *general_width_plus;
   SLK_gui_element *general_width_minus;
   SLK_gui_element *general_height_plus;
   SLK_gui_element *general_height_minus;
   SLK_gui_element *general_dither_left;
   SLK_gui_element *general_dither_right;
   SLK_gui_element *general_bar_width;
   SLK_gui_element *general_bar_height;
   SLK_gui_element *general_bar_dither;
   SLK_gui_element *general_label_width;
   SLK_gui_element *general_label_height;
   SLK_gui_element *general_label_dither;
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
   "Reserved",
};
//-------------------------------------

//Function prototypes
static void gui_buttons();
static void gui_draw();
static void update_output();
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
   SLK_rgb_sprite_destroy(tmp);
   SLK_gui_window_add_element(preview,tabbar);
   
   //Gui window
   settings = SLK_gui_window_create(10,10,384,294);
   SLK_gui_window_set_title(settings,"Settings");
   SLK_gui_window_set_moveable(settings,1);
   settings_tabs = SLK_gui_vtabbar_create(2,14,96,20,text_tab_settings);
   SLK_gui_window_add_element(settings,settings_tabs);
   //Save/Load tab
   elements.save_load = SLK_gui_button_create(158,64,164,14,"Load");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load);
   elements.save_save = SLK_gui_button_create(158,198,164,14,"Save");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save);
   elements.save_load_preset = SLK_gui_button_create(158,92,164,14,"Load preset");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_load_preset);
   elements.save_save_preset = SLK_gui_button_create(158,226,164,14,"Save preset");
   SLK_gui_vtabbar_add_element(settings_tabs,0,elements.save_save_preset);
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
   elements.general_bar_width = SLK_gui_slider_create(174,21,170,14,0,256);;
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
   int update = 0;
   switch(settings_tabs->vtabbar.current_tab)
   {
   case 0: //Save/Load tab
      if(elements.save_load->button.state.released)
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_openFileDialog("Select a png file","",1,filter_patterns,NULL,0);
         SLK_rgb_sprite_destroy(sprite_in);
         sprite_in = SLK_rgb_sprite_load(file_path);
         if(sprite_in)
            SLK_gui_image_update(image_in,sprite_in,(SLK_gui_rectangle){0,0,sprite_in->width,sprite_in->height});
         update = 1;
      }
      else if(elements.save_save->button.state.released)
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_saveFileDialog("Save image","",1,filter_patterns,NULL);
         SLK_rgb_sprite_save(file_path,sprite_out);
         elements.save_save->button.state.released = 0;
      }
      else if(elements.save_load_preset->button.state.released)
      {
         const char *filter_patterns[2] = {"*.json"};
         const char *file_path = tinyfd_openFileDialog("Select a preset","",1,filter_patterns,NULL,0);
      }
      else if(elements.save_save_preset->button.state.released)
      {
         const char *filter_patterns[2] = {"*.json"};
         const char *file_path = tinyfd_saveFileDialog("Save preset","",1,filter_patterns,NULL);
      }
      break;
   case 1: //Palette tab

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
      break;
   }

   if(update)
      update_output(); 
   /*int update = 0;
   int x,y;
   SLK_mouse_get_layer_pos(0,&x,&y);

   if(SLK_mouse_pressed(SLK_BUTTON_LEFT))
   {
      if(x>3&&x<64&&y>372&&y<395) //Load button
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_openFileDialog("Select a png file","",1,filter_patterns,NULL,0);
         SLK_rgb_sprite_destroy(sprite_in);
         sprite_in = SLK_rgb_sprite_load(file_path);
         if(sprite_in)
         {
            SLK_layer_set_size(1,sprite_in->width,sprite_in->height);
            SLK_layer_set_current(1);
            SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
            SLK_draw_rgb_clear();
            SLK_draw_rgb_sprite(sprite_in,0,0);
            SLK_draw_rgb_set_changed(1);
            float scale;
            if(sprite_in->width>sprite_in->height)
               scale = 256.0f/sprite_in->width;
            else 
               scale = 256.0f/sprite_in->height;
            SLK_layer_set_scale(1,scale);
            update_output();
         }
      }
      else if(x>81&&x<144&&y>373&&y<395) //Save button
      {
         const char *filter_patterns[2] = {"*.png"};
         const char *file_path = tinyfd_saveFileDialog("Save image","",1,filter_patterns,NULL);
         SLK_rgb_sprite_save(file_path,sprite_out);
         puts(file_path);
      }
      else if(x>3&&x<144&&y>332&&y<355) //Palette load button
      {
         const char *filter_patterns[2] = {"*.pal"};
         const char *file_path = tinyfd_openFileDialog("Load a palette","",1,filter_patterns,NULL,0);
         if(palette)
            free(palette);
         palette = SLK_palette_load(file_path);
         update = 1;
      }
      else if(x>3&&x<144&&y>293&&y<315) //Process image button
      {
         update = 1;
      }
      else if(x>0&&x<9&&y>19&&y<28) //Dither left
      {
         pixel_process_mode--;
         if(pixel_process_mode<0)
            pixel_process_mode = 5;
         update = 1;
      }
      else if(x>142&&x<149&&y>19&&y<28) //Dither right
      {
         pixel_process_mode++;
         if(pixel_process_mode>5)
            pixel_process_mode = 0;
         update = 1;
      }
      else if(x>0&&x<9&&y>89&&y<98) //Output width left
      {
         if(gui_out_width>0)
            gui_out_width-=8;
         update = 1;
      }
      else if(x>142&&x<149&&y>89&&y<98) //Output width right
      {
         if(gui_out_width<256)
            gui_out_width+=8;
         update = 1;
      }
      else if(x>0&&x<9&&y>123&&y<132) //Output height left
      {
         if(gui_out_height>0)
            gui_out_height-=8;
         update = 1;
      }
      else if(x>142&&x<149&&y>123&&y<132) //Output height right
      {
         if(gui_out_height<256)
            gui_out_height+=8;
         update = 1;
      }
   }

   if(update)
      update_output();*/
}

static void gui_draw()
{
   SLK_layer_set_current(0);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(20,20,20,255));
   SLK_draw_rgb_clear();

   SLK_gui_window_draw(settings);
   SLK_gui_window_draw(preview);
   /*if(SLK_layer_get_resized(3))
   {
      SLK_layer_set_current(3);
      SLK_draw_rgb_set_changed(1);
      SLK_draw_rgb_set_clear_color(SLK_color_create(20,20,20,255));
      SLK_draw_rgb_clear();
      int w,h;
      SLK_layer_get_size(3,&w,&h);

      //Update layer pos
      gui_pos_x = w/32;
      gui_pos_y = (h-400)/2;
      SLK_layer_set_pos(0,gui_pos_x,gui_pos_y);

      int y = h/16;
      int x = gui_pos_x+150+w/32;
      SLK_layer_set_pos(1,x,y);
      SLK_draw_rgb_sprite(sprite_backdrop_1,x-4,y-4);
      SLK_draw_rgb_string(x+88,y+264,2,"Input",SLK_color_create(255,255,255,255));

      y = h-256-h/16;
      x = w-256-w/32;
      SLK_layer_set_pos(2,x,y);
      SLK_draw_rgb_sprite(sprite_backdrop_1,x-4,y-4);
      SLK_draw_rgb_string(x+80,y-24,2,"Output",SLK_color_create(255,255,255,255));
   }

   SLK_layer_set_current(0);
   SLK_draw_rgb_set_changed(1);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,0));
   //SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,255));
   SLK_draw_rgb_clear();
   SLK_draw_rgb_sprite(sprite_gui_0,0,0);

   //Adjust text to middle
   const char *text = text_dither[pixel_process_mode];
   int len = strlen(text)*8;
   int pos_x = 8+(134-len)/2;
   SLK_draw_rgb_string(pos_x,20,1,text_dither[pixel_process_mode],SLK_color_create(255,255,255,255));
   char tmp_text[256];
   sprintf(tmp_text,"%d",gui_out_width);
   len = strlen(tmp_text)*8;
   pos_x = 8+(134-len)/2;
   SLK_draw_rgb_string(pos_x,90,1,tmp_text,SLK_color_create(255,255,255,255));
   sprintf(tmp_text,"%d",gui_out_height);
   len = strlen(tmp_text)*8;
   pos_x = 8+(134-len)/2;
   SLK_draw_rgb_string(pos_x,124,1,tmp_text,SLK_color_create(255,255,255,255));*/
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
