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
#include "color.h"
#include "util.h"
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

static struct
{
   HLH_gui_entry *entry_img_width;
   HLH_gui_entry *entry_img_height;

   HLH_gui_radiobutton *palette_colors[256];

   GUI_canvas *canvas;
}gui;
//-------------------------------------

//Variables
static HLH_gui_window *window_root;
//-------------------------------------

//Function prototypes
static void gui_project_update(Project *p);

static void ui_construct_ask_new();
static void ui_construct_image_new();
static void ui_construct_ask_load();

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
//-------------------------------------

//Function implementations

void gui_construct(void)
{
   HLH_gui_window *win = HLH_gui_window_create("pixtexed",1000,650,NULL);
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
   GUI_canvas *canvas = gui_canvas_create(&group_middle->e,HLH_GUI_FILL|HLH_GUI_LAYOUT_HORIZONTAL,project_new(64,64));
   gui.canvas = canvas;
   gui_canvas_update_project(gui.canvas,canvas->project);
   //-------------------------------------

   //Right -- palette, brushes
   //-------------------------------------
   HLH_gui_separator_create(&group_middle->e,HLH_GUI_FILL_Y|HLH_GUI_LAYOUT_HORIZONTAL,1);
   HLH_gui_group *group_right = HLH_gui_group_create(&group_middle->e,HLH_GUI_FILL_Y|HLH_GUI_LAYOUT_HORIZONTAL);
   HLH_gui_group *group_pal = HLH_gui_group_create(&group_right->e,0);
   //gui.group_palette = group_pal;
   int color = 0;
   for(int i = 0;i<32;i++)
   {
      HLH_gui_radiobutton *r = NULL;
      for(int j = 0;j<7;j++)
      {
         r = HLH_gui_radiobutton_create(&group_pal->e,HLH_GUI_LAYOUT_HORIZONTAL|HLH_GUI_NO_CENTER_X|HLH_GUI_NO_CENTER_Y,"",NULL);
         gui.palette_colors[color] = r;
         r->e.usr_ptr = &gui.canvas->project->palette[color];
         r->e.usr = color++;
         r->e.msg_usr = radiobutton_palette_msg;
      }
      r = HLH_gui_radiobutton_create(&group_pal->e,HLH_GUI_NO_CENTER_X|HLH_GUI_NO_CENTER_Y,"",NULL);
      gui.palette_colors[color] = r;
      r->e.usr_ptr = &gui.canvas->project->palette[color];
      r->e.usr = color++;
      r->e.msg_usr = radiobutton_palette_msg;
   }
   //-------------------------------------

   //Toolbar
   //-------------------------------------
   HLH_gui_group *group_tools = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_X|HLH_GUI_LAYOUT_VERTICAL);
   HLH_gui_separator_create(&group_tools->e,HLH_GUI_FILL_X,0);
   HLH_gui_button_create(&group_tools->e,HLH_GUI_LAYOUT_HORIZONTAL," ",NULL);
   {
      HLH_gui_radiobutton *rfirst = NULL;
      HLH_gui_group *group = HLH_gui_group_create(&group_tools->e,HLH_GUI_LAYOUT_HORIZONTAL);
      HLH_gui_radiobutton *r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      rfirst = r;
      r->e.usr = TOOL_PEN;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_LINE;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_FLOOD;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_RECT_OUTLINE;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_RECT_FILL;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_GRADIENT;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_SPLINE;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_SPRAY;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_POLYLINE;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_POLYFORM;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_CIRCLE_OUTLINE;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_CIRCLE_FILL;
      r->e.msg_usr = radiobutton_toolbox_msg;
      r = HLH_gui_radiobutton_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"",NULL);
      r->e.usr = TOOL_SPHERE_GRAD;
      r->e.msg_usr = radiobutton_toolbox_msg;

      HLH_gui_radiobutton_set(rfirst,1,1);
   }
   //-------------------------------------

   //Layers
   //-------------------------------------
   HLH_gui_group *group_status = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_X|HLH_GUI_LAYOUT_VERTICAL);
   //HLH_gui_label_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"TEST");

   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"1",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"2",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"3",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"4",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"5",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"6",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"7",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"8",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"9",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"10",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"11",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"12",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"13",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"14",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"15",NULL);
   HLH_gui_radiobutton_create(&group_status->e,HLH_GUI_LAYOUT_HORIZONTAL,"16",NULL);
   //-------------------------------------

   //Animation + Status
   //-------------------------------------
   HLH_gui_group *group_anim = HLH_gui_group_create(&root_group->e,HLH_GUI_FILL_X|HLH_GUI_LAYOUT_VERTICAL);
   HLH_gui_radiobutton_create(&group_anim->e,HLH_GUI_LAYOUT_HORIZONTAL,"a",NULL);
   //-------------------------------------
}

static void gui_project_update(Project *p)
{
   gui_canvas_update_project(gui.canvas,p);
   for(int i = 0;i<256;i++)
      gui.palette_colors[i]->e.usr_ptr = &gui.canvas->project->palette[i];
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
         if(strlen(gui.canvas->project->file)>0)
         {
            Image8 *img = project_to_image8(gui.canvas->project);
            if(!image8_save(img,gui.canvas->project->file,gui.canvas->project->ext))
            {
               gui.canvas->project->file[0] = '\0';
            }
            free(img);
         }
         else
         {
            image_save_select(gui.canvas->project->file,gui.canvas->project->ext);
            Image8 *img = project_to_image8(gui.canvas->project);
            if(!image8_save(img,gui.canvas->project->file,gui.canvas->project->ext))
            {
               gui.canvas->project->file[0] = '\0';
            }
            free(img);
         }
      }
      //Save as
      else if(m->index==3)
      {
         image_save_select(gui.canvas->project->file,gui.canvas->project->ext);
         Image8 *img = project_to_image8(gui.canvas->project);
         if(!image8_save(img,gui.canvas->project->file,gui.canvas->project->ext))
         {
            gui.canvas->project->file[0] = '\0';
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

         image_load_select(gui.canvas->project->file,gui.canvas->project->ext);
         Image8 *img = image8_load(gui.canvas->project->file,gui.canvas->project->ext);
         Project *p = project_from_image8(img);
         free(img);
         gui_project_update(p);
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
   gui.entry_img_width = HLH_gui_entry_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,5);
   gui.entry_img_width->e.msg_usr = entry_msg;
   gui.entry_img_width->e.usr = ENTRY_IMG_WIDTH;

   group = HLH_gui_group_create(&group_root->e,0);
   HLH_gui_label_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"Height");
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x11",NULL);
   b->e.msg_usr = button_sub_msg;
   b->e.usr = BUTTON_IMG_HEIGHT;
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"\x10",NULL);
   b->e.msg_usr = button_add_msg;
   b->e.usr = BUTTON_IMG_HEIGHT;
   gui.entry_img_height = HLH_gui_entry_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,5);
   gui.entry_img_height->e.msg_usr = entry_msg;
   gui.entry_img_height->e.usr = ENTRY_IMG_HEIGHT;

   group = HLH_gui_group_create(&group_root->e,0);
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"  Ok  ",NULL);
   b->e.msg_usr = button_img_new_ok;
   HLH_gui_label_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"   ");
   b = HLH_gui_button_create(&group->e,HLH_GUI_LAYOUT_HORIZONTAL,"Cancel",NULL);
   b->e.msg_usr = button_img_new_cancel;

   HLH_gui_entry_set(gui.entry_img_width,"64");
   HLH_gui_entry_set(gui.entry_img_height,"64");
}

static int button_add_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *button = (HLH_gui_button *)e;
   if(msg==HLH_GUI_MSG_CLICK)
   {
      char buffer[512];
      if(button->e.usr==BUTTON_IMG_WIDTH)
      {
         int width = (int)strtol(gui.entry_img_width->entry,NULL,10);
         snprintf(buffer,512,"%d",width+1);
         HLH_gui_entry_set(gui.entry_img_width,buffer);
      }
      else if(button->e.usr==BUTTON_IMG_HEIGHT)
      {
         int height = (int)strtol(gui.entry_img_height->entry,NULL,10);
         snprintf(buffer,512,"%d",height+1);
         HLH_gui_entry_set(gui.entry_img_height,buffer);
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
         int width = (int)strtol(gui.entry_img_width->entry,NULL,10);
         snprintf(buffer,512,"%d",width-1);
         HLH_gui_entry_set(gui.entry_img_width,buffer);
      }
      else if(button->e.usr==BUTTON_IMG_HEIGHT)
      {
         int height = (int)strtol(gui.entry_img_height->entry,NULL,10);
         snprintf(buffer,512,"%d",height-1);
         HLH_gui_entry_set(gui.entry_img_height,buffer);
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
      //if(r->e.usr>=gui.canvas->project->palette_colors)
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
         gui.canvas->project->palette_selected = (uint8_t)r->e.usr;
         //uint32_t c = dither_config.palette[color_selected];
         //HLH_gui_slider_set(gui.slider_color_red,SLK_color32_r(c),255,1,1);
         //HLH_gui_slider_set(gui.slider_color_green,SLK_color32_g(c),255,1,1);
         //HLH_gui_slider_set(gui.slider_color_blue,SLK_color32_b(c),255,1,1);
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
         //HLH_gui_element_ignore(&gui_groups_sample[e->usr]->e,1);
      }
      //Check
      else if(di==1)
      {
         gui.canvas->project->tools.selected = e->usr;
      }
   }

   return 0;
}
//-------------------------------------
