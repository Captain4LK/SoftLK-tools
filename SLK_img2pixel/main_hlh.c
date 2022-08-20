/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes

#define LIBSLK_IMPLEMENTATION
#include "../headers/libSLK.h"

#include "../HLH_gui/HLH_gui.h"
//-------------------------------------

//Internal includes
#include "utility.h"
#include "assets.h"
#include "image2pixel.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
struct
{
   HLH_gui_window *window;
      HLH_gui_panel *panel;
         HLH_gui_htab *htab;
         HLH_gui_panel *tab1;
            HLH_gui_image *img_in;
         HLH_gui_panel *tab2;
            HLH_gui_image *img_out;
}preview;

struct
{
   HLH_gui_window *window;
      HLH_gui_panel *panel0;
         HLH_gui_vtab *vtab;

         HLH_gui_panel *tab1;
            HLH_gui_panel *panel10;
               HLH_gui_button *load_img;
               HLH_gui_button *save_img;
            HLH_gui_panel *panel11;
               HLH_gui_button *load_pre;
               HLH_gui_button *save_pre;
            HLH_gui_panel *panel12;
               HLH_gui_button *dir_in;
               HLH_gui_button *dir_out;
            HLH_gui_panel *panel13;
               HLH_gui_panel *panel14;
                  HLH_gui_label *label10;
               HLH_gui_panel *panel15;
                  HLH_gui_button *upscale_minus;
                  HLH_gui_slider *upscale;
                  HLH_gui_button *upscale_plus;
               HLH_gui_panel *panel16;
                  HLH_gui_label *label11;

         HLH_gui_panel *tab2;
            HLH_gui_panel *panel20;
               HLH_gui_image *img_pal20;
               HLH_gui_image *img_pal21;
            HLH_gui_panel *panel21;
            HLH_gui_panel *panel22;

         HLH_gui_panel *tab3;
            HLH_gui_htab *htab;
            HLH_gui_panel *panel30;
               HLH_gui_panel *panel31;
                  HLH_gui_panel *panel310;
                     HLH_gui_label *label311;
                  HLH_gui_panel *panel311;
                     HLH_gui_button *width_minus;
                     HLH_gui_slider *width;
                     HLH_gui_button *width_plus;
                  HLH_gui_panel *panel312;
                     HLH_gui_label *label312;
               HLH_gui_panel *panel32;
                  HLH_gui_panel *panel320;
                     HLH_gui_label *label321;
                  HLH_gui_panel *panel321;
                     HLH_gui_button *height_minus;
                     HLH_gui_slider *height;
                     HLH_gui_button *height_plus;
                  HLH_gui_panel *panel322;
                     HLH_gui_label *label322;
            HLH_gui_panel *panel33;
               HLH_gui_panel *panel34;
                  HLH_gui_panel *panel340;
                     HLH_gui_label *label341;
                  HLH_gui_panel *panel341;
                     HLH_gui_button *xdiv_minus;
                     HLH_gui_slider *xdiv;
                     HLH_gui_button *xdiv_plus;
                  HLH_gui_panel *panel342;
                     HLH_gui_label *label342;
               HLH_gui_panel *panel35;
                  HLH_gui_panel *panel350;
                     HLH_gui_label *label351;
                  HLH_gui_panel *panel351;
                     HLH_gui_button *ydiv_minus;
                     HLH_gui_slider *ydiv;
                     HLH_gui_button *ydiv_plus;
                  HLH_gui_panel *panel352;
                     HLH_gui_label *label352;

         HLH_gui_panel *tab4;

         HLH_gui_panel *tab5;
            HLH_gui_panel *panel50;
               HLH_gui_panel *panel500;
                  HLH_gui_label *label501;
               HLH_gui_panel *panel501;
                  HLH_gui_button *bright_minus;
                  HLH_gui_slider *bright;
                  HLH_gui_button *bright_plus;
               HLH_gui_panel *panel502;
                  HLH_gui_label *label502;
            HLH_gui_panel *panel51;
               HLH_gui_panel *panel510;
                  HLH_gui_label *label511;
               HLH_gui_panel *panel511;
                  HLH_gui_button *contra_minus;
                  HLH_gui_slider *contra;
                  HLH_gui_button *contra_plus;
               HLH_gui_panel *panel512;
                  HLH_gui_label *label512;
            HLH_gui_panel *panel52;
               HLH_gui_panel *panel520;
                  HLH_gui_label *label521;
               HLH_gui_panel *panel521;
                  HLH_gui_button *satura_minus;
                  HLH_gui_slider *satura;
                  HLH_gui_button *satura_plus;
               HLH_gui_panel *panel522;
                  HLH_gui_label *label522;
            HLH_gui_panel *panel53;
               HLH_gui_panel *panel530;
                  HLH_gui_label *label531;
               HLH_gui_panel *panel531;
                  HLH_gui_button *gamma_minus;
                  HLH_gui_slider *gamma;
                  HLH_gui_button *gamma_plus;
               HLH_gui_panel *panel532;
                  HLH_gui_label *label532;
            HLH_gui_panel *panel54;
               HLH_gui_panel *panel540;
                  HLH_gui_label *label541;
               HLH_gui_panel *panel541;
                  HLH_gui_button *sharp_minus;
                  HLH_gui_slider *sharp;
                  HLH_gui_button *sharp_plus;
               HLH_gui_panel *panel542;
                  HLH_gui_label *label542;
            HLH_gui_panel *panel55;
               HLH_gui_panel *panel550;
                  HLH_gui_label *label551;
               HLH_gui_panel *panel551;
                  HLH_gui_button *hue_minus;
                  HLH_gui_slider *hue;
                  HLH_gui_button *hue_plus;
               HLH_gui_panel *panel552;
                  HLH_gui_label *label552;
}settings;

static SLK_RGB_sprite *sprite_in = NULL;
static SLK_RGB_sprite *sprite_in_org = NULL;
static SLK_RGB_sprite *sprite_out = NULL;
static int palette_selected = 0;

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
//-------------------------------------

//Function prototypes
static int button_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void update_output();
static void preset_load(FILE *f);
//-------------------------------------

//Function implementations

int main()
{
   utility_init();
   atexit(utility_exit);

   assets_init();
   img2pixel_set_palette(assets_load_pal_default());

   //Construct gui
   HLH_gui_init();

   //Preview window
   preview.window = HLH_gui_window_create("SLK_img2pixel - preview", 800, 600);
   preview.panel = HLH_gui_panel_create(&preview.window->e, HLH_GUI_PANEL_LIGHT);
   preview.htab = HLH_gui_htab_create(&preview.panel->e,HLH_GUI_H_FILL);

   //Tab 1 - input preview
   preview.tab1 = HLH_gui_panel_create(&preview.panel->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL|HLH_GUI_V_FILL);
   preview.tab1->gap = 40;
   preview.tab1->border = HLH_gui_rect_make(2, 2, 2, 2);
   HLH_gui_htab_set(preview.htab,0,"Input");
   uint32_t data[1] = {0x0};
   preview.img_in = HLH_gui_image_create(&preview.tab1->e,HLH_GUI_H_FILL|HLH_GUI_V_FILL,1,1,data);

   //Tab 2 - output preview
   preview.tab2 = HLH_gui_panel_create(&preview.panel->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL|HLH_GUI_V_FILL);
   preview.tab2->gap = 40;
   preview.tab2->border = HLH_gui_rect_make(2, 2, 2, 2);
   HLH_gui_htab_set(preview.htab,1,"Output");
   preview.img_out = HLH_gui_image_create(&preview.tab2->e,HLH_GUI_H_FILL|HLH_GUI_V_FILL,1,1,data);
   //-------------------------------------

   //Settings window
   settings.window = HLH_gui_window_create("HLH_gui_window 1", 800, 600);
   settings.panel0 = HLH_gui_panel_create(&settings.window->e, HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel0->gap = 0;
   settings.vtab = HLH_gui_vtab_create(&settings.panel0->e,HLH_GUI_V_FILL);

   //Tab 1 - Save/Load
   settings.tab1 = HLH_gui_panel_create(&settings.panel0->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL);
   settings.tab1->gap = 40;
   HLH_gui_vtab_set(settings.vtab,0,"Load/Save");
   settings.panel10 = HLH_gui_panel_create(&settings.tab1->e,HLH_GUI_PANEL_LIGHT);
   settings.panel10->gap = 20;
   settings.load_img = HLH_gui_button_create(&settings.panel10->e, 0, "Load image", -1);
   settings.load_img->e.msg_usr = button_msg;
   settings.save_img = HLH_gui_button_create(&settings.panel10->e, 0, "Save image", -1);
   settings.save_img->e.msg_usr = button_msg;
   settings.panel11 = HLH_gui_panel_create(&settings.tab1->e,HLH_GUI_PANEL_LIGHT);
   settings.panel11->gap = 20;
   settings.load_pre = HLH_gui_button_create(&settings.panel11->e, 0, "Load preset", -1);
   settings.load_pre->e.msg_usr = button_msg;
   settings.save_pre = HLH_gui_button_create(&settings.panel11->e, 0, "Save preset", -1);
   settings.save_pre->e.msg_usr = button_msg;
   settings.panel12 = HLH_gui_panel_create(&settings.tab1->e,HLH_GUI_PANEL_LIGHT);
   settings.panel12->gap = 20;
   settings.dir_in = HLH_gui_button_create(&settings.panel12->e, 0, "Select input dir", -1);
   settings.dir_in->e.msg_usr = button_msg;
   settings.dir_out = HLH_gui_button_create(&settings.panel12->e, 0, "Select output dir", -1);
   settings.dir_out->e.msg_usr = button_msg;
   settings.panel13 = HLH_gui_panel_create(&settings.tab1->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel14 = HLH_gui_panel_create(&settings.panel13->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel14->border = HLH_gui_rect_make(8,8,0,0);
   settings.label10 = HLH_gui_label_create(&settings.panel14->e,0,"Scale",-1);
   settings.panel15 = HLH_gui_panel_create(&settings.panel13->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel15->border = HLH_gui_rect_make(8,8,0,0);
   settings.upscale_minus = HLH_gui_button_create(&settings.panel15->e,0,"-",-1);
   settings.upscale_minus->e.msg_usr = button_msg;
   settings.upscale = HLH_gui_slider_create(&settings.panel15->e, HLH_GUI_H_FILL, 1,1,16);
   settings.upscale->e.msg_usr = slider_msg;
   settings.upscale_plus = HLH_gui_button_create(&settings.panel15->e,0,"+",-1);
   settings.upscale_plus->e.msg_usr = button_msg;
   settings.panel16 = HLH_gui_panel_create(&settings.panel13->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel16->border = HLH_gui_rect_make(8,8,0,0);
   settings.label11 = HLH_gui_label_create(&settings.panel16->e,0," 1",-1);

   //Tab 2 - Palette
   settings.tab2 = HLH_gui_panel_create(&settings.panel0->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL);
   settings.tab2->gap = 40;
   HLH_gui_vtab_set(settings.vtab,1,"Palette");
   settings.panel20 = HLH_gui_panel_create(&settings.tab2->e,HLH_GUI_PANEL_LIGHT);
   settings.panel20->gap = 20;
   settings.panel21 = HLH_gui_panel_create(&settings.tab2->e,HLH_GUI_PANEL_LIGHT);
   settings.panel21->gap = 20;
   settings.panel22 = HLH_gui_panel_create(&settings.tab2->e,HLH_GUI_PANEL_LIGHT);
   settings.panel22->gap = 20;

   //Tab 3 - Sample
   settings.tab3 = HLH_gui_panel_create(&settings.panel0->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL|HLH_GUI_V_FILL);
   settings.tab3->gap = 10;
   HLH_gui_vtab_set(settings.vtab,2,"Sample");
   settings.htab = HLH_gui_htab_create(&settings.tab3->e,HLH_GUI_H_FILL);
   settings.panel30 = HLH_gui_panel_create(&settings.tab3->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL);
   settings.panel30->gap = 8;
   HLH_gui_htab_set(settings.htab,0,"Absolute");
   settings.panel33 = HLH_gui_panel_create(&settings.tab3->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL);
   settings.panel33->gap = 8;
   HLH_gui_htab_set(settings.htab,1,"Relative");

   settings.panel31 = HLH_gui_panel_create(&settings.panel30->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel310 = HLH_gui_panel_create(&settings.panel31->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel310->border = HLH_gui_rect_make(8,8,0,0);
   settings.label311 = HLH_gui_label_create(&settings.panel310->e,0,"Width ",-1);
   settings.panel311 = HLH_gui_panel_create(&settings.panel31->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel311->border = HLH_gui_rect_make(8,8,0,0);
   settings.width_minus = HLH_gui_button_create(&settings.panel311->e,0,"-",-1);
   settings.width_minus->e.msg_usr = button_msg;
   settings.width = HLH_gui_slider_create(&settings.panel311->e, HLH_GUI_H_FILL,0,-255,255);
   settings.width->e.msg_usr = slider_msg;
   settings.width_plus = HLH_gui_button_create(&settings.panel311->e,0,"+",-1);
   settings.width_plus->e.msg_usr = button_msg;
   settings.panel312 = HLH_gui_panel_create(&settings.panel31->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel312->border = HLH_gui_rect_make(8,8,0,0);
   settings.label312 = HLH_gui_label_create(&settings.panel312->e,0,"   0",-1);

   settings.panel32 = HLH_gui_panel_create(&settings.panel30->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel320 = HLH_gui_panel_create(&settings.panel32->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel320->border = HLH_gui_rect_make(8,8,0,0);
   settings.label321 = HLH_gui_label_create(&settings.panel320->e,0,"Height",-1);
   settings.panel321 = HLH_gui_panel_create(&settings.panel32->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel321->border = HLH_gui_rect_make(8,8,0,0);
   settings.height_minus = HLH_gui_button_create(&settings.panel321->e,0,"-",-1);
   settings.height_minus->e.msg_usr = button_msg;
   settings.height = HLH_gui_slider_create(&settings.panel321->e, HLH_GUI_H_FILL,0,-255,255);
   settings.height->e.msg_usr = slider_msg;
   settings.height_plus = HLH_gui_button_create(&settings.panel321->e,0,"+",-1);
   settings.height_plus->e.msg_usr = button_msg;
   settings.panel322 = HLH_gui_panel_create(&settings.panel32->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel322->border = HLH_gui_rect_make(8,8,0,0);
   settings.label322 = HLH_gui_label_create(&settings.panel322->e,0,"   0",-1);

   settings.panel34 = HLH_gui_panel_create(&settings.panel33->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel340 = HLH_gui_panel_create(&settings.panel34->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel340->border = HLH_gui_rect_make(8,8,0,0);
   settings.label341 = HLH_gui_label_create(&settings.panel340->e,0,"x div ",-1);
   settings.panel341 = HLH_gui_panel_create(&settings.panel34->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel341->border = HLH_gui_rect_make(8,8,0,0);
   settings.xdiv_minus = HLH_gui_button_create(&settings.panel341->e,0,"-",-1);
   settings.xdiv_minus->e.msg_usr = button_msg;
   settings.xdiv = HLH_gui_slider_create(&settings.panel341->e, HLH_GUI_H_FILL,0,-255,255);
   settings.xdiv->e.msg_usr = slider_msg;
   settings.xdiv_plus = HLH_gui_button_create(&settings.panel341->e,0,"+",-1);
   settings.xdiv_plus->e.msg_usr = button_msg;
   settings.panel342 = HLH_gui_panel_create(&settings.panel34->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel342->border = HLH_gui_rect_make(8,8,0,0);
   settings.label342 = HLH_gui_label_create(&settings.panel342->e,0,"   0",-1);

   settings.panel35 = HLH_gui_panel_create(&settings.panel33->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel350 = HLH_gui_panel_create(&settings.panel35->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel350->border = HLH_gui_rect_make(8,8,0,0);
   settings.label351 = HLH_gui_label_create(&settings.panel350->e,0,"y div ",-1);
   settings.panel351 = HLH_gui_panel_create(&settings.panel35->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel351->border = HLH_gui_rect_make(8,8,0,0);
   settings.ydiv_minus = HLH_gui_button_create(&settings.panel351->e,0,"-",-1);
   settings.ydiv_minus->e.msg_usr = button_msg;
   settings.ydiv = HLH_gui_slider_create(&settings.panel351->e, HLH_GUI_H_FILL,0,-255,255);
   settings.ydiv->e.msg_usr = slider_msg;
   settings.ydiv_plus = HLH_gui_button_create(&settings.panel351->e,0,"+",-1);
   settings.ydiv_plus->e.msg_usr = button_msg;
   settings.panel352 = HLH_gui_panel_create(&settings.panel35->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel352->border = HLH_gui_rect_make(8,8,0,0);
   settings.label352 = HLH_gui_label_create(&settings.panel352->e,0,"   0",-1);

   //Tab 4 - Colors
   settings.tab4 = HLH_gui_panel_create(&settings.panel0->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL|HLH_GUI_V_FILL);
   settings.tab4->gap = 10;
   HLH_gui_vtab_set(settings.vtab,3,"Colors");

   //Tab 5 - Process
   settings.tab5 = HLH_gui_panel_create(&settings.panel0->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL|HLH_GUI_V_FILL);
   settings.tab5->gap = 10;
   settings.tab5->border = HLH_gui_rect_make(0,0,8,0);
   HLH_gui_vtab_set(settings.vtab,4,"Process");

   settings.panel50 = HLH_gui_panel_create(&settings.tab5->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel500 = HLH_gui_panel_create(&settings.panel50->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel500->border = HLH_gui_rect_make(8,8,0,0);
   settings.label501 = HLH_gui_label_create(&settings.panel500->e,0,"Brightness",-1);
   settings.panel501 = HLH_gui_panel_create(&settings.panel50->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel501->border = HLH_gui_rect_make(8,8,0,0);
   settings.bright_minus = HLH_gui_button_create(&settings.panel501->e,0,"-",-1);
   settings.bright_minus->e.msg_usr = button_msg;
   settings.bright = HLH_gui_slider_create(&settings.panel501->e, HLH_GUI_H_FILL,0,-255,255);
   settings.bright->e.msg_usr = slider_msg;
   settings.bright_plus = HLH_gui_button_create(&settings.panel501->e,0,"+",-1);
   settings.bright_plus->e.msg_usr = button_msg;
   settings.panel502 = HLH_gui_panel_create(&settings.panel50->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel502->border = HLH_gui_rect_make(8,8,0,0);
   settings.label502 = HLH_gui_label_create(&settings.panel502->e,0,"   0",-1);

   settings.panel51 = HLH_gui_panel_create(&settings.tab5->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel510 = HLH_gui_panel_create(&settings.panel51->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel510->border = HLH_gui_rect_make(8,8,0,0);
   settings.label511 = HLH_gui_label_create(&settings.panel510->e,0,"Contrast  ",-1);
   settings.panel511 = HLH_gui_panel_create(&settings.panel51->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel511->border = HLH_gui_rect_make(8,8,0,0);
   settings.contra_minus = HLH_gui_button_create(&settings.panel511->e,0,"-",-1);
   settings.contra_minus->e.msg_usr = button_msg;
   settings.contra = HLH_gui_slider_create(&settings.panel511->e, HLH_GUI_H_FILL, 0,-255,255);
   settings.contra->e.msg_usr = slider_msg;
   settings.contra_plus = HLH_gui_button_create(&settings.panel511->e,0,"+",-1);
   settings.contra_plus->e.msg_usr = button_msg;
   settings.panel512 = HLH_gui_panel_create(&settings.panel51->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel512->border = HLH_gui_rect_make(8,8,0,0);
   settings.label512 = HLH_gui_label_create(&settings.panel512->e,0,"   0",-1);

   settings.panel52 = HLH_gui_panel_create(&settings.tab5->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel520 = HLH_gui_panel_create(&settings.panel52->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel520->border = HLH_gui_rect_make(8,8,0,0);
   settings.label521 = HLH_gui_label_create(&settings.panel520->e,0,"Saturation",-1);
   settings.panel521 = HLH_gui_panel_create(&settings.panel52->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel521->border = HLH_gui_rect_make(8,8,0,0);
   settings.satura_minus = HLH_gui_button_create(&settings.panel521->e,0,"-",-1);
   settings.satura_minus->e.msg_usr = button_msg;
   settings.satura = HLH_gui_slider_create(&settings.panel521->e, HLH_GUI_H_FILL, 100,1,600);
   settings.satura->e.msg_usr = slider_msg;
   settings.satura_plus = HLH_gui_button_create(&settings.panel521->e,0,"+",-1);
   settings.satura_plus->e.msg_usr = button_msg;
   settings.panel522 = HLH_gui_panel_create(&settings.panel52->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel522->border = HLH_gui_rect_make(8,8,0,0);
   settings.label522 = HLH_gui_label_create(&settings.panel522->e,0," 100",-1);

   settings.panel53 = HLH_gui_panel_create(&settings.tab5->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel530 = HLH_gui_panel_create(&settings.panel53->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel530->border = HLH_gui_rect_make(8,8,0,0);
   settings.label531 = HLH_gui_label_create(&settings.panel530->e,0,"Gamma     ",-1);
   settings.panel531 = HLH_gui_panel_create(&settings.panel53->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel531->border = HLH_gui_rect_make(8,8,0,0);
   settings.gamma_minus = HLH_gui_button_create(&settings.panel531->e,0,"-",-1);
   settings.gamma_minus->e.msg_usr = button_msg;
   settings.gamma = HLH_gui_slider_create(&settings.panel531->e, HLH_GUI_H_FILL, 100,1,800);
   settings.gamma->e.msg_usr = slider_msg;
   settings.gamma_plus = HLH_gui_button_create(&settings.panel531->e,0,"+",-1);
   settings.gamma_plus->e.msg_usr = button_msg;
   settings.panel532 = HLH_gui_panel_create(&settings.panel53->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel532->border = HLH_gui_rect_make(8,8,0,0);
   settings.label532 = HLH_gui_label_create(&settings.panel532->e,0," 100",-1);

   settings.panel54 = HLH_gui_panel_create(&settings.tab5->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel540 = HLH_gui_panel_create(&settings.panel54->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel540->border = HLH_gui_rect_make(8,8,0,0);
   settings.label541 = HLH_gui_label_create(&settings.panel540->e,0,"Sharpness ",-1);
   settings.panel541 = HLH_gui_panel_create(&settings.panel54->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel541->border = HLH_gui_rect_make(8,8,0,0);
   settings.sharp_minus = HLH_gui_button_create(&settings.panel541->e,0,"-",-1);
   settings.sharp_minus->e.msg_usr = button_msg;
   settings.sharp = HLH_gui_slider_create(&settings.panel541->e, HLH_GUI_H_FILL, 0,0,100);
   settings.sharp->e.msg_usr = slider_msg;
   settings.sharp_plus = HLH_gui_button_create(&settings.panel541->e,0,"+",-1);
   settings.sharp_plus->e.msg_usr = button_msg;
   settings.panel542 = HLH_gui_panel_create(&settings.panel54->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel542->border = HLH_gui_rect_make(8,8,0,0);
   settings.label542 = HLH_gui_label_create(&settings.panel542->e,0,"   0",-1);

   settings.panel55 = HLH_gui_panel_create(&settings.tab5->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel550 = HLH_gui_panel_create(&settings.panel55->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel550->border = HLH_gui_rect_make(8,8,0,0);
   settings.label551 = HLH_gui_label_create(&settings.panel550->e,0,"Hue       ",-1);
   settings.panel551 = HLH_gui_panel_create(&settings.panel55->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel551->border = HLH_gui_rect_make(8,8,0,0);
   settings.hue_minus = HLH_gui_button_create(&settings.panel551->e,0,"-",-1);
   settings.hue_minus->e.msg_usr = button_msg;
   settings.hue = HLH_gui_slider_create(&settings.panel551->e, HLH_GUI_H_FILL, 0,-360,360);
   settings.hue->e.msg_usr = slider_msg;
   settings.hue_plus = HLH_gui_button_create(&settings.panel551->e,0,"+",-1);
   settings.hue_plus->e.msg_usr = button_msg;
   settings.panel552 = HLH_gui_panel_create(&settings.panel55->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel552->border = HLH_gui_rect_make(8,8,0,0);
   settings.label552 = HLH_gui_label_create(&settings.panel552->e,0,"   0",-1);
   //-------------------------------------

   //-------------------------------------

   //Load default json
   const char *env_def = getenv("IMG2PIXEL_CONF");
   if(!env_def)
      env_def = "./default.json";
   FILE *f = fopen(env_def,"r");
   preset_load(f);
   if(f!=NULL)
      fclose(f);


   return HLH_gui_message_loop();
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

   HLH_gui_image_update(preview.img_out,sprite_out->width,sprite_out->height,(uint32_t *)sprite_out->data);
}

static int button_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_button *b = (HLH_gui_button *)e;

   if(msg==HLH_GUI_MSG_CLICK)
   {
      if(b==settings.load_img)
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

            HLH_gui_image_update(preview.img_in,sprite_in_org->width,sprite_in_org->height,(uint32_t *)sprite_in_org->data);

            update_output();
         }
      }
      else if(b==settings.save_img)
      {
         image_write(sprite_out,img2pixel_get_palette());
      }
      else if(b==settings.load_pre)
      {
         FILE *f = json_select();
         preset_load(f);
         if(f!=NULL)
            fclose(f);
      }
      else if(b==settings.save_pre)
      {
         FILE *f = json_write();
         if(f!=NULL)
         {
            img2pixel_preset_save(f);
            fclose(f);
         }
      }
      else if(b==settings.dir_in)
      {
         dir_input_select();
      }
      else if(b==settings.dir_out)
      {
         if(img2pixel_get_scale_mode()==0)
            dir_output_select(img2pixel_get_process_mode(),img2pixel_get_sample_mode(),img2pixel_get_distance_mode(),img2pixel_get_scale_mode(),img2pixel_get_out_width(),img2pixel_get_out_height(),img2pixel_get_palette());
         else
            dir_output_select(img2pixel_get_process_mode(),img2pixel_get_sample_mode(),img2pixel_get_distance_mode(),img2pixel_get_scale_mode(),img2pixel_get_out_swidth(),img2pixel_get_out_sheight(),img2pixel_get_palette());
      }

      //Sliders
      //Save/Load
      else if(b==settings.upscale_minus) { HLH_gui_slider_set_value(settings.upscale,settings.upscale->value-1); }
      else if(b==settings.upscale_plus) { HLH_gui_slider_set_value(settings.upscale,settings.upscale->value+1); }

      //Process
      else if(b==settings.bright_minus) { HLH_gui_slider_set_value(settings.bright,settings.bright->value-1); }
      else if(b==settings.bright_plus) { HLH_gui_slider_set_value(settings.bright,settings.bright->value+1); }
      else if(b==settings.contra_minus) { HLH_gui_slider_set_value(settings.contra,settings.contra->value-1); }
      else if(b==settings.contra_plus) { HLH_gui_slider_set_value(settings.contra,settings.contra->value+1); }
      else if(b==settings.satura_minus) { HLH_gui_slider_set_value(settings.satura,settings.satura->value-1); }
      else if(b==settings.satura_plus) { HLH_gui_slider_set_value(settings.satura,settings.satura->value+1); }
      else if(b==settings.gamma_minus) { HLH_gui_slider_set_value(settings.gamma,settings.gamma->value-1); }
      else if(b==settings.gamma_plus) { HLH_gui_slider_set_value(settings.gamma,settings.gamma->value+1); }
      else if(b==settings.sharp_minus) { HLH_gui_slider_set_value(settings.sharp,settings.sharp->value-1); }
      else if(b==settings.sharp_plus) { HLH_gui_slider_set_value(settings.sharp,settings.sharp->value+1); }
      else if(b==settings.hue_minus) { HLH_gui_slider_set_value(settings.hue,settings.hue->value-1); }
      else if(b==settings.hue_plus) { HLH_gui_slider_set_value(settings.hue,settings.hue->value+1); }
   }

   return 0;
}

static int slider_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_slider *s = e;

   if(msg==HLH_GUI_MSG_SLIDER_CHANGED_VALUE)
   {
      char tmp[512];

      //Save/Load
      if(s==settings.upscale) { sprintf(tmp,"%2d",settings.upscale->value); HLH_gui_label_set_text(settings.label11,tmp,-1); }

      //Process
      else if(s==settings.bright) { sprintf(tmp,"%4d",settings.bright->value); HLH_gui_label_set_text(settings.label502,tmp,-1); img2pixel_set_brightness(settings.bright->value); update_output(); }
      else if(s==settings.contra) { sprintf(tmp,"%4d",settings.contra->value); HLH_gui_label_set_text(settings.label512,tmp,-1); img2pixel_set_contrast(settings.contra->value); update_output(); }
      else if(s==settings.satura) { sprintf(tmp,"%4d",settings.satura->value); HLH_gui_label_set_text(settings.label522,tmp,-1); img2pixel_set_saturation(settings.satura->value); update_output(); }
      else if(s==settings.gamma) { sprintf(tmp,"%4d",settings.gamma->value); HLH_gui_label_set_text(settings.label532,tmp,-1); img2pixel_set_gamma(settings.gamma->value); update_output(); }
      else if(s==settings.hue) { sprintf(tmp,"%4d",settings.hue->value); HLH_gui_label_set_text(settings.label552,tmp,-1); img2pixel_set_hue(settings.hue->value); update_output(); }
      else if(s==settings.sharp)
      {
         sprintf(tmp,"%4d",settings.sharp->value);
         HLH_gui_label_set_text(settings.label542,tmp,-1); 
         img2pixel_set_sharpen(settings.sharp->value);
         img2pixel_lowpass_image(sprite_in_org,sprite_in);
         img2pixel_sharpen_image(sprite_in,sprite_in);
         update_output();
      }
   }

   return 0;
}

void preset_load(FILE *f)
{
   char ctmp[16];
   img2pixel_preset_load(f);

   //The unnecessary indentations are for my own sanity
   /*elements.palette_bar_r->slider.value = img2pixel_get_palette()->colors[palette_selected].rgb.r;
   elements.palette_bar_g->slider.value = img2pixel_get_palette()->colors[palette_selected].rgb.g;
   elements.palette_bar_b->slider.value = img2pixel_get_palette()->colors[palette_selected].rgb.b;
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
   SLK_gui_label_set_text(elements.palette_label_colors,ctmp);*/

   img2pixel_lowpass_image(sprite_in_org,sprite_in);   
   img2pixel_sharpen_image(sprite_in,sprite_in);
   update_output();

   //Update gui
   /*if(img2pixel_get_process_mode()==6||img2pixel_get_process_mode()==7||img2pixel_get_process_mode()==0)
      color_dither_enabled(0);
   else
      color_dither_enabled(1);
   if(img2pixel_get_distance_mode()==8)
      color_weight_enabled(1);
   else
      color_weight_enabled(0);*/
}
//-------------------------------------
