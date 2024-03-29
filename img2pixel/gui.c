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
typedef enum
{
   SLIDER_BLUR,
}Slider_id;
//-------------------------------------

//Variables
static HLH_gui_imgcmp *gui_imgcmp;

static HLH_gui_group *gui_groups_left[6];

static SLK_image32 *gui_input = NULL;

static int blur_amount = 0;
//-------------------------------------

//Function prototypes
static int rb_radiobutton_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_load_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_save_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_help_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);

static void gui_process();
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

   //Sample
   //-------------------------------------
   {
      gui_groups_left[0] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"                                ");
      HLH_gui_group *group = HLH_gui_group_create(&gui_groups_left[0]->e,0);
      HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_WEST,"Absolute",NULL);
      HLH_gui_radiobutton_create(&group->e,HLH_GUI_PACK_WEST,"Relative",NULL);

      HLH_gui_slider *slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);
      slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample mode");

      HLH_gui_group *group_sample = HLH_gui_group_create(&gui_groups_left[0]->e.window->e,HLH_GUI_NO_PARENT|HLH_GUI_STYLE_01);
      HLH_gui_radiobutton *r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"  Round ",NULL);
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"  Floor ",NULL);
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"  Ceil  ",NULL);
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bilinear",NULL);
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bicubic ",NULL);
      r = HLH_gui_radiobutton_create(&group_sample->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Lanczos ",NULL);
      const char *bar_sample[1] = {"Round"};
      HLH_gui_menubar_create(&gui_groups_left[0]->e,0,HLH_GUI_PACK_WEST,bar_sample,(HLH_gui_element **)&group_sample,1,NULL);

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample x offset");
      slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);
      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Sample y offset");
      slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);

      HLH_gui_label_create(&gui_groups_left[0]->e,0,"Blur amount");
      slider = HLH_gui_slider_create(&gui_groups_left[0]->e,HLH_GUI_FILL_X,0);
      HLH_gui_slider_set(slider,1,512,0,0);
      slider->e.msg_usr = slider_msg;
   }
   //-------------------------------------

   gui_groups_left[1] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
   HLH_gui_label_create(&gui_groups_left[1]->e,0,"                                ");
   HLH_gui_button_create(&gui_groups_left[1]->e,HLH_GUI_PACK_NORTH,"colors",NULL);
   gui_groups_left[2] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
   HLH_gui_button_create(&gui_groups_left[2]->e,HLH_GUI_PACK_NORTH,"test 2",NULL);
   HLH_gui_label_create(&gui_groups_left[2]->e,0,"                                ");
   gui_groups_left[3] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
   HLH_gui_button_create(&gui_groups_left[3]->e,HLH_GUI_PACK_NORTH,"test 3",NULL);
   HLH_gui_label_create(&gui_groups_left[3]->e,0,"                                ");
   gui_groups_left[4] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
   HLH_gui_button_create(&gui_groups_left[4]->e,HLH_GUI_PACK_NORTH,"test 4",NULL);
   HLH_gui_label_create(&gui_groups_left[4]->e,0,"                                ");
   gui_groups_left[5] = HLH_gui_group_create(&group_left->e,HLH_GUI_EXPAND);
   HLH_gui_button_create(&gui_groups_left[5]->e,HLH_GUI_PACK_NORTH,"test 5",NULL);
   HLH_gui_label_create(&gui_groups_left[5]->e,0,"                                ");
   HLH_gui_element_ignore(&gui_groups_left[0]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[1]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[2]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[3]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[4]->e,1);
   HLH_gui_element_ignore(&gui_groups_left[5]->e,1);

   //Right bar: settings tabs
   HLH_gui_radiobutton *rb = NULL;
   HLH_gui_radiobutton *sample = NULL;
   sample = rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Sample",NULL);
   rb->e.usr = 0;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Dither",NULL);
   rb->e.usr = 1;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Test 2",NULL);
   rb->e.usr = 2;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Test 3",NULL);
   rb->e.usr = 3;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Test 4",NULL);
   rb->e.usr = 4;
   rb->e.msg_usr = rb_radiobutton_msg;
   rb = HLH_gui_radiobutton_create(&group_right->e,HLH_GUI_PACK_NORTH|HLH_GUI_STYLE_02|HLH_GUI_MAX_X,"Test 5",NULL);
   rb->e.usr = 5;
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
   }

   return 0;
}

static void gui_process()
{
   if(gui_input==NULL)
      return;

   SLK_image64 *img = SLK_image64_dup32(gui_input);
   SLK_image64_blur(img,blur_amount/16.f);
   SLK_image64 *sampled = SLK_image64_sample(img,img->w/8,img->h/8,0,0.f,0.f);
   SLK_image64_hscb(sampled,0.f,1.f,1.f,1.f);
   SLK_image64_gamma(sampled,1.f);
   SLK_image32 *dithered = SLK_image64_dither(sampled,0,0,0);
   free(img);
   free(sampled);
//SLK_image64 *SLK_image64_sample(const SLK_image64 *img, int width, int height, int sample_mode, float x_off, float y_off)
//void SLK_image64_hscb(SLK_image64 *img, float hue, float saturation, float contrast, float brightness);

   //SLK_image32 *out = SLK_image32_dup64(sampled);

   HLH_gui_imgcmp_update1(gui_imgcmp,dithered->data,dithered->w,dithered->h,1);
   free(dithered);
}
//-------------------------------------
