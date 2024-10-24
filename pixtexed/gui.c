/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include "HLH_gui.h"
//-------------------------------------

//Internal includes
#include "gui.h"
#include "canvas.h"
#include "project.h"
#include "shared/color.h"
#include "util.h"
#include "settings.h"
#include "brush.h"
#include "layer.h"
#include "tool.h"
#include "undo.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
typedef enum
{
   BUTTON_IMG_WIDTH,
   BUTTON_IMG_HEIGHT,
}Button_id;

typedef enum
{
   ENTRY_IMG_WIDTH,
   ENTRY_IMG_HEIGHT,
}Entry_id;

/*static struct
{
   HLH_gui_entry *entry_img_width;
   HLH_gui_entry *entry_img_height;

   HLH_gui_radiobutton *palette_colors[256];
   HLH_gui_radiobutton *layers[16];

   GUI_canvas *canvas;
}gui;*/
//-------------------------------------

//Variables
static HLH_gui_window *window_root;
GUI_state gui_state;
//-------------------------------------

//Function prototypes
static void ui_replace_project(Project *project);

static void ui_construct_ask_new();
static void ui_construct_image_new();
static void ui_construct_ask_load();
static void ui_construct_brushes();
static void ui_construct_layer_settings(int layer);

static int menu_file_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int menu_help_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int ask_new_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int ask_load_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_add_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_sub_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int entry_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_img_new_ok(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_img_new_cancel(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_palette_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int radiobutton_toolbox_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_brushes_open(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_brushes_select(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int button_layer_control(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

void gui_construct(void)
{
   HLH_gui_window *win = HLH_gui_window_create("pixtexed",1000,650,"pixtexed_icons.png");
   window_root = win;

   //Menubar
   //-------------------------------------
   const char *menu0[] = 
   {
      "New",
      "Open",
      "Save",
      "Save as",
   };
   const char *menu1[] = 
   {
      "Image",
      "Preset",
      "Palette",
   };
   HLH_gui_element *menus[2];
   menus[0] = (HLH_gui_element *)HLH_gui_menu_create(&win->e,HLH_GUI_STYLE_01|HLH_GUI_NO_PARENT,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,menu0,4,menu_file_msg);
   menus[1] = (HLH_gui_element *)HLH_gui_menu_create(&win->e,HLH_GUI_STYLE_01|HLH_GUI_NO_PARENT,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,menu1,3,menu_help_msg);

   const char *menubar[] = 
   {
      "Image",
      "Help",
   };

   HLH_gui_group *root_group = HLH_gui_group_create(&win->e,HLH_GUI_FILL);
   HLH_gui_menubar_create(&root_group->e,HLH_GUI_FILL_X,HLH_GUI_LAYOUT_HORIZONTAL|HLH_GUI_STYLE_01,menubar,menus,2,NULL);
   HLH_gui_separator_create(&root_group->e,HLH_GUI_FILL_X,0);
   //-------------------------------------

   HLH_gui_group *group_middle = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL|HLH_GUI_LAYOUT_VERTICAL);

   //Middle -- Canvas
   //-------------------------------------
   Settings *settings = settings_init();
   settings_build_lut(settings);
   GUI_canvas *canvas = gui_canvas_create(&group_middle->e,HLH_GUI_FILL|HLH_GUI_LAYOUT_HORIZONTAL,project_new(64,64,settings),settings,&gui_state);
   gui_state.canvas = canvas;
   //-------------------------------------

   //Right -- palette, brushes
   //-------------------------------------
   HLH_gui_separator_create(&group_middle->e,HLH_GUI_FILL_Y|HLH_GUI_LAYOUT_HORIZONTAL,1);
   HLH_gui_group *group_right = HLH_gui_group_create(&group_middle->e,HLH_GUI_FILL_Y|HLH_GUI_LAYOUT_HORIZONTAL);
   HLH_gui_group *group_pal = HLH_gui_group_create(&group_right->e,0);
   //gui_state.group_palette = group_pal;
   {
      int color = 0;
      HLH_gui_radiobutton *rfirst;
      for(int i = 0;i<32;i++)
      {
         HLH_gui_radiobutton *r = NULL;
         for(int j = 0;j<7;j++)
         {
            r = HLH_gui_radiobutton_create(&group_pal->e,HLH_GUI_LAYOUT_HORIZONTAL|HLH_GUI_NO_CENTER_X|HLH_GUI_NO_CENTER_Y,"",NULL);
            if(i==0&&j==0)
               rfirst = r;
            gui_state.palette_colors[color] = r;
            r->e.usr_ptr = &gui_state.canvas->settings->palette[color];
            r->e.usr = color++;
            r->e.msg_usr = radiobutton_palette_msg;
         }
         r = HLH_gui_radiobutton_create(&group_pal->e,HLH_GUI_NO_CENTER_X|HLH_GUI_NO_CENTER_Y,"",NULL);
         gui_state.palette_colors[color] = r;
         r->e.usr_ptr = &gui_state.canvas->settings->palette[color];
         r->e.usr = color++;
         r->e.msg_usr = radiobutton_palette_msg;
      }

      HLH_gui_radiobutton_set(rfirst,1,1);
   }
   //-------------------------------------

   //Toolbar
   //-------------------------------------
   HLH_gui_group *group_tools = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_X|HLH_GUI_LAYOUT_VERTICAL);
   HLH_gui_separator_create(&group_tools->e,HLH_GUI_FILL_X,0);
   HLH_gui_button *button = HLH_gui_button_create(&group_tools->e,HLH_GUI_LAYOUT_HORIZONTAL," ",NULL);
   button->e.msg_usr = button_brushes_open;
   {
      HLH_gui_radiobutton *rfirst = NULL;
      HLH_gui_group *group = HLH_gui_group_create(&group_tools->e,HLH_GUI_LAYOUT_HORIZONTAL);

      GUI_tool *tfirst = NULL;
      gui_state.tools[0] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_PEN);
      tfirst = gui_state.tools[0];
      gui_state.tools[1] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_LINE);
      gui_state.tools[2] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_FLOOD);
      gui_state.tools[3] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_RECT_OUTLINE);
      gui_state.tools[4] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_RECT_FILL);
      gui_state.tools[5] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_GRADIENT);
      gui_state.tools[6] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_SPLINE);
      gui_state.tools[7] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_SPRAY);
      gui_state.tools[8] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_POLYLINE);
      gui_state.tools[9] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_POLYFORM);
      gui_state.tools[10] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_CIRCLE_OUTLINE);
      gui_state.tools[11] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_CIRCLE_FILL);
      gui_state.tools[12] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_SELECT_RECT);
      gui_state.tools[13] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_SELECT_LASSO);
      gui_state.tools[14] = gui_tool_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas,TOOL_PIPETTE);
      gui_tool_set(tfirst,HLH_GUI_MOUSE_LEFT);
   }
   //-------------------------------------

   //Layers
   //-------------------------------------
   HLH_gui_group *group_status = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_X|HLH_GUI_LAYOUT_VERTICAL);
   //HLH_gui_label_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"TEST");

   button = HLH_gui_button_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x0f",NULL);
   button->e.msg_usr = button_layer_control;
   button->e.usr = 5;
   button = HLH_gui_button_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"+",NULL);
   button->e.msg_usr = button_layer_control;
   button->e.usr = 0;
   button = HLH_gui_button_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"-",NULL);
   button->e.msg_usr = button_layer_control;
   button->e.usr = 1;
   button = HLH_gui_button_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x1b",NULL);
   button->e.msg_usr = button_layer_control;
   button->e.usr = 2;
   button = HLH_gui_button_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x1a",NULL);
   button->e.msg_usr = button_layer_control;
   button->e.usr = 3;
   button = HLH_gui_button_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x1f",NULL);
   button->e.msg_usr = button_layer_control;
   button->e.usr = 4;

   for(int i = 0;i<16;i++)
   {
      char tmp[128];
      snprintf(tmp,128,"%d",i+1);
      GUI_layer *l = gui_layer_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,canvas,i);
      gui_state.layers[i] = l;
      l->e.usr = i;
      if(i>0)
         HLH_gui_element_ignore(&l->e,1);
   }
   //-------------------------------------

   //Animation + Status
   //-------------------------------------
   HLH_gui_group *group_anim = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_X|HLH_GUI_LAYOUT_VERTICAL);
   HLH_gui_radiobutton_create(&group_anim->e,HLH_GUI_LAYOUT_HORIZONTAL,"a",NULL);
   //-------------------------------------

   ui_replace_project(canvas->project);
}

static void ui_replace_project(Project *project)
{
   gui_canvas_update_project(gui_state.canvas,project);
   project->layer_selected = 0;

   for(int i = 0;i<16;i++)
   {
      gui_state.layers[i]->canvas = gui_state.canvas;
      if(i>0)
         HLH_gui_element_ignore(&gui_state.layers[i]->e,1);
   }
   for(int i = 0;i<15;i++)
   {
      gui_state.tools[i]->canvas = gui_state.canvas;
   }

   gui_layer_set(gui_state.layers[0],HLH_GUI_MOUSE_LEFT);
}

static int menu_file_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_menubutton *m = (HLH_gui_menubutton *)e;
   if(msg==HLH_GUI_MSG_CLICK_MENU)
   {
      //New
      if(m->index==0)
      {
         ui_construct_ask_new();
      }
      //Load
      else if(m->index==1)
      {
         ui_construct_ask_load();
      }
      //Save
      else if(m->index==2)
      {
         if(strlen(gui_state.canvas->project->file)>0)
         {
            Image8 *img = project_to_image8(gui_state.canvas->project,gui_state.canvas->settings);
            if(!image8_save(img,gui_state.canvas->project->file,gui_state.canvas->project->ext))
            {
               gui_state.canvas->project->file[0] = '\0';
            }
            free(img);
         }
         else
         {
            image_save_select(gui_state.canvas->project->file,gui_state.canvas->project->ext);
            Image8 *img = project_to_image8(gui_state.canvas->project,gui_state.canvas->settings);
            if(!image8_save(img,gui_state.canvas->project->file,gui_state.canvas->project->ext))
            {
               gui_state.canvas->project->file[0] = '\0';
            }
            free(img);
         }
      }
      //Save as
      else if(m->index==3)
      {
         image_save_select(gui_state.canvas->project->file,gui_state.canvas->project->ext);
         Image8 *img = project_to_image8(gui_state.canvas->project,gui_state.canvas->settings);
         if(!image8_save(img,gui_state.canvas->project->file,gui_state.canvas->project->ext))
         {
            gui_state.canvas->project->file[0] = '\0';
         }
         free(img);
      }
   }
   return 0;
}

static int menu_help_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   return 0;
}

static void ui_construct_ask_new()
{
   HLH_gui_window *win = HLH_gui_window_create("Create new image", 300, 100, NULL);
   HLH_gui_window_block(window_root, win);
   HLH_gui_group *group = HLH_gui_group_create(&win->e, HLH_GUI_FILL);

   HLH_gui_label_create(&group->e, 0, "Are you sure you want");
   HLH_gui_label_create(&group->e, 0, "to start a new image?");
   group = HLH_gui_group_create(&group->e, 0);
   HLH_gui_button *button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Cancel", NULL);
   button->e.msg_usr = ask_new_msg;
   button->e.usr = 0;
   button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Save", NULL);
   button->e.msg_usr = ask_new_msg;
   button->e.usr = 1;
   button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Confirm", NULL);
   button->e.msg_usr = ask_new_msg;
   button->e.usr = 2;
}

static void ui_construct_ask_load()
{
   HLH_gui_window *win = HLH_gui_window_create("Load image", 300, 100, NULL);
   HLH_gui_window_block(window_root, win);
   HLH_gui_group *group = HLH_gui_group_create(&win->e, HLH_GUI_FILL);

   HLH_gui_label_create(&group->e, 0, "Are you sure you want");
   HLH_gui_label_create(&group->e, 0, "to start load an image?");
   group = HLH_gui_group_create(&group->e, 0);
   HLH_gui_button *button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Cancel", NULL);
   button->e.msg_usr = ask_load_msg;
   button->e.usr = 0;
   button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Save", NULL);
   button->e.msg_usr = ask_load_msg;
   button->e.usr = 1;
   button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Confirm", NULL);
   button->e.msg_usr = ask_load_msg;
   button->e.usr = 2;
}

static void ui_construct_layer_settings(int layer)
{
   HLH_gui_window *win = HLH_gui_window_create("Layer settings", 300, 400, NULL);
   HLH_gui_window_block(window_root, win);
   HLH_gui_group *group = HLH_gui_group_create(&win->e, HLH_GUI_FILL);

   char tmp[512];
   snprintf(tmp,512,"Layer %2d",layer+1);
   HLH_gui_label_create(&group->e, 0, tmp);

   //HLH_gui_group *group_type = HLH_gui_group_create(&group->e,HLH_GUI_FILL_X);
   HLH_gui_label_create(&group->e, 0, "Layer type");

   HLH_gui_group *group_select = HLH_gui_group_create(&group->e.window->e,HLH_GUI_NO_PARENT|HLH_GUI_STYLE_01);
   HLH_gui_radiobutton *r = HLH_gui_radiobutton_create(&group_select->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Normal",NULL);
   r = HLH_gui_radiobutton_create(&group_select->e,HLH_GUI_FILL_X|HLH_GUI_STYLE_01,"Bump",NULL);
   //HLH_gui_label_create(&group_type->e, HLH_GUI_LAYOUT_HORIZONTAL, "Layer type");
   const char *bar_select[1] = {"Normal \x1f"};
   HLH_gui_menubar_create(&group->e,0,HLH_GUI_LAYOUT_HORIZONTAL,bar_select,(HLH_gui_element **)&group_select,1,NULL);
   HLH_gui_separator_create(&group->e, HLH_GUI_FILL_X, 0);
   
   //Blend modes
   {
      gui_state.group_layer_settings[0] = HLH_gui_group_create(&group->e,HLH_GUI_FILL);

      //slider opacity
   }

   //Bump map
   {
      gui_state.group_layer_settings[1] = HLH_gui_group_create(&group->e,HLH_GUI_FILL);

      //slider x direction
      
      //slider y direction

      //slider z direction
   }
   //HLH_gui_group *group = HLH_gui_group_create(&win->e, HLH_GUI_FILL);
}

static int ask_new_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      //Cancel
      if(e->usr==0)
      {
         HLH_gui_window_close(e->window);
      }
      else if(e->usr==1)
      {
         //Save
         /*if(strlen(map_path_get())>0)
         {
            //map_save();
         }
         else
         {
            //if(util_select_map_path())
               //return 0;

            //map_save();
         }*/

         HLH_gui_window_close(e->window);

         ui_construct_image_new();
      }
      else if(e->usr==2)
      {
         HLH_gui_window_close(e->window);

         ui_construct_image_new();
      }
   }

   return 0;
}

static int ask_load_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      //Cancel
      if(e->usr==0)
      {
         HLH_gui_window_close(e->window);
      }
      else if(e->usr==1)
      {
         HLH_gui_window_close(e->window);

         //ui_construct_image_new();
      }
      else if(e->usr==2)
      {
         HLH_gui_window_close(e->window);

         image_load_select(gui_state.canvas->project->file,gui_state.canvas->project->ext);
         Image8 *img = image8_load(gui_state.canvas->project->file,gui_state.canvas->project->ext);
         Project *p = project_from_image8(gui_state.canvas->settings,img);
         free(img);
         ui_replace_project(p);
      }
   }

   return 0;
}

static void ui_construct_image_new()
{
   HLH_gui_window *win = HLH_gui_window_create("Create new image", 180, 100, NULL);
   HLH_gui_window_block(window_root, win);
   HLH_gui_group *group_root = HLH_gui_group_create(&win->e, HLH_GUI_FILL);

   HLH_gui_label_create(&group_root->e,0,"Size");
   HLH_gui_separator_create(&group_root->e,HLH_GUI_FILL_X,0);

   HLH_gui_group *group = HLH_gui_group_create(&group_root->e,0);
   HLH_gui_label_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"Width ");
   HLH_gui_button *b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x11",NULL);
   b->e.msg_usr = button_sub_msg;
   b->e.usr = BUTTON_IMG_WIDTH;
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x10",NULL);
   b->e.msg_usr = button_add_msg;
   b->e.usr = BUTTON_IMG_WIDTH;
   gui_state.entry_img_width = HLH_gui_entry_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,5);
   gui_state.entry_img_width->e.msg_usr = entry_msg;
   gui_state.entry_img_width->e.usr = ENTRY_IMG_WIDTH;

   group = HLH_gui_group_create(&group_root->e,0);
   HLH_gui_label_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"Height");
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x11",NULL);
   b->e.msg_usr = button_sub_msg;
   b->e.usr = BUTTON_IMG_HEIGHT;
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x10",NULL);
   b->e.msg_usr = button_add_msg;
   b->e.usr = BUTTON_IMG_HEIGHT;
   gui_state.entry_img_height = HLH_gui_entry_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,5);
   gui_state.entry_img_height->e.msg_usr = entry_msg;
   gui_state.entry_img_height->e.usr = ENTRY_IMG_HEIGHT;

   group = HLH_gui_group_create(&group_root->e,0);
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"  Ok  ",NULL);
   b->e.msg_usr = button_img_new_ok;
   HLH_gui_label_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"   ");
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"Cancel",NULL);
   b->e.msg_usr = button_img_new_cancel;

   HLH_gui_entry_set(gui_state.entry_img_width,"64");
   HLH_gui_entry_set(gui_state.entry_img_height,"64");
}

static int button_add_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      char buffer[512];
      if(button->e.usr==BUTTON_IMG_WIDTH)
      {
         int width = (int)strtol(gui_state.entry_img_width->entry,NULL,10);
         snprintf(buffer,512,"%d",width+1);
         HLH_gui_entry_set(gui_state.entry_img_width,buffer);
      }
      else if(button->e.usr==BUTTON_IMG_HEIGHT)
      {
         int height = (int)strtol(gui_state.entry_img_height->entry,NULL,10);
         snprintf(buffer,512,"%d",height+1);
         HLH_gui_entry_set(gui_state.entry_img_height,buffer);
      }
   }

   return 0;
}

static int button_sub_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      char buffer[512];
      if(button->e.usr==BUTTON_IMG_WIDTH)
      {
         int width = (int)strtol(gui_state.entry_img_width->entry,NULL,10);
         snprintf(buffer,512,"%d",width-1);
         HLH_gui_entry_set(gui_state.entry_img_width,buffer);
      }
      else if(button->e.usr==BUTTON_IMG_HEIGHT)
      {
         int height = (int)strtol(gui_state.entry_img_height->entry,NULL,10);
         snprintf(buffer,512,"%d",height-1);
         HLH_gui_entry_set(gui_state.entry_img_height,buffer);
      }
   }

   return 0;
}

static int entry_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   return 0;
}

static int button_img_new_ok(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      //TODO(Captain4LK): new image here
      HLH_gui_window_close(e->window);
   }

   return 0;
}

static int button_img_new_cancel(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      HLH_gui_window_close(e->window);
   }

   return 0;
}

static int radiobutton_palette_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_radiobutton *r = (HLH_gui_radiobutton *)e;

   if(msg==HLH_GUI_MSG_DRAW)
   {
      //if(r->e.usr>=gui_state.canvas->project->palette_colors)
         //return 1;

      int scale = HLH_gui_get_scale();
      HLH_gui_rect bounds = r->e.bounds;

      uint32_t color = *((uint32_t *)r->e.usr_ptr);

      //Infill
      HLH_gui_draw_rectangle_fill(&r->e, HLH_gui_rect_make(bounds.minx, bounds.miny, bounds.maxx, bounds.maxy),color);

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
         if(color32_r(color)<128&&color32_g(color)<128&&color32_b(color)<128)
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
         gui_state.canvas->settings->palette_selected = (uint8_t)r->e.usr;
         //uint32_t c = dither_config.palette[color_selected];
         //HLH_gui_slider_set(gui_state.slider_color_red,SLK_color32_r(c),255,1,1);
         //HLH_gui_slider_set(gui_state.slider_color_green,SLK_color32_g(c),255,1,1);
         //HLH_gui_slider_set(gui_state.slider_color_blue,SLK_color32_b(c),255,1,1);
      }
   }

   return 0;
}

static int radiobutton_toolbox_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
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
         gui_state.canvas->project->tools.selected = e->usr;
      }
   }

   return 0;
}

//GUI_brush *gui_brush_create(HLH_gui_element *parent, uint64_t flags, Project *project, Settings *settings, int brush_num);
//void gui_brushes_update(HLH_gui_window *window, const Settings *settings);

static int button_brushes_open(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(msg==HLH_GUI_MSG_CLICK)
   {
      ui_construct_brushes();
   }

   return 0;
}

static void ui_construct_brushes()
{
   HLH_gui_window *win = HLH_gui_window_create("Brushes", 500, 200, NULL);
   HLH_gui_window_block(window_root, win);
   HLH_gui_group *group = HLH_gui_group_create(&win->e, HLH_GUI_FILL);

   for(int y = 0;y<5;y++)
   {
      HLH_gui_group *row = HLH_gui_group_create(&group->e,HLH_GUI_FILL_X|HLH_GUI_LAYOUT_VERTICAL);
      for(int x = 0;x<12;x++)
      {
         GUI_brush *brush = gui_brush_create(&row->e,HLH_GUI_LAYOUT_HORIZONTAL,gui_state.canvas->project,gui_state.canvas->settings,y*12+x);
         brush->e.msg_usr = button_brushes_select;
      }
   }

   gui_brushes_update(win,gui_state.canvas->settings);

   /*HLH_gui_label_create(&group->e, 0, "Are you sure you want");
   HLH_gui_label_create(&group->e, 0, "to start a new image?");
   group = HLH_gui_group_create(&group->e, 0);
   HLH_gui_button *button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Cancel", NULL);
   button->e.msg_usr = ask_new_msg;
   button->e.usr = 0;
   button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Save", NULL);
   button->e.msg_usr = ask_new_msg;
   button->e.usr = 1;
   button = HLH_gui_button_create(&group->e, HLH_GUI_LAYOUT_HORIZONTAL | HLH_GUI_FILL_X, "Confirm", NULL);
   button->e.msg_usr = ask_new_msg;
   button->e.usr = 2;*/
}

static int button_brushes_select(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   GUI_brush *brush = (GUI_brush *)e;

   if(msg==HLH_GUI_MSG_CLICK)
   {
      brush->settings->brush_selected = (uint8_t)brush->brush_num;
      HLH_gui_window_close(brush->e.window);
   }

   return 0;
}

static int button_layer_control(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   Project *project = gui_state.canvas->project;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      if(e->usr==0) //Add layer
      {
         if(gui_state.canvas->project->num_layers<17)
         {
            undo_track_layer_add(gui_state.canvas->project);
            project_layer_add(gui_state.canvas->project,gui_state.canvas->project->num_layers-1);
            HLH_gui_element_ignore(&gui_state.layers[gui_state.canvas->project->num_layers-2]->e,0);
            HLH_gui_element_layout(&e->window->e, e->window->e.bounds);
            HLH_gui_element_redraw(&e->window->e);
         }
      }
      else if(e->usr==1) //Delete layer
      {
         if(gui_state.canvas->project->num_layers>2)
         {
            undo_track_layer_delete(gui_state.canvas->project,gui_state.canvas->project->layer_selected);
            HLH_gui_element_ignore(&gui_state.layers[gui_state.canvas->project->num_layers-2]->e,1);
            project_layer_delete(gui_state.canvas->project,gui_state.canvas->project->layer_selected);
            HLH_gui_element_layout(&e->window->e, e->window->e.bounds);
            HLH_gui_element_redraw(&e->window->e);
            if(gui_state.canvas->project->layer_selected>0)
               gui_layer_set(gui_state.layers[gui_state.canvas->project->layer_selected-1],HLH_GUI_MOUSE_LEFT);
            else
               gui_layer_set(gui_state.layers[0],HLH_GUI_MOUSE_LEFT);
         }
      }
      else if(e->usr==2) //Move layer left
      {
         if(project->layer_selected>0)
         {
            undo_track_layer_move(project,project->layer_selected,-1);
            Layer *tmp = project->layers[project->layer_selected];
            project->layers[project->layer_selected] = project->layers[project->layer_selected-1];
            project->layers[project->layer_selected-1] = tmp,

            gui_layer_set(gui_state.layers[project->layer_selected-1],HLH_GUI_MOUSE_LEFT);
         }
      }
      else if(e->usr==3) //Move layer right
      {
         if(project->layer_selected<project->num_layers-2)
         {
            undo_track_layer_move(project,project->layer_selected,1);
            Layer *tmp = project->layers[project->layer_selected];
            project->layers[project->layer_selected] = project->layers[project->layer_selected+1];
            project->layers[project->layer_selected+1] = tmp,

            gui_layer_set(gui_state.layers[project->layer_selected+1],HLH_GUI_MOUSE_LEFT);
         }
      }
      else if(e->usr==4) //Merge layer
      {
      }
      else if(e->usr==5) //Layer properties
      {
         ui_construct_layer_settings(0);
//(static void ui_construct_layer_settings(int layer)
      }
   }

   return 0;
}
//-------------------------------------
