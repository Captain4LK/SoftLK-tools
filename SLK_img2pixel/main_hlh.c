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
#define MIN(a,b) ((a)>(b)?(b):(a))
//-------------------------------------

//Typedefs
typedef struct
{
   HLH_gui_element e;
   SDL_Texture *palette;
   int selected;
}Palette_selection;
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
               Palette_selection *pal2;
            HLH_gui_panel *panel21;
               HLH_gui_panel *panel22;
                  HLH_gui_panel *panel220;
                     HLH_gui_label *label221;
                  HLH_gui_panel *panel221;
                     HLH_gui_button *red_minus;
                     HLH_gui_slider *red;
                     HLH_gui_button *red_plus;
                  HLH_gui_panel *panel222;
                     HLH_gui_label *label222;
               HLH_gui_panel *panel23;
                  HLH_gui_panel *panel230;
                     HLH_gui_label *label231;
                  HLH_gui_panel *panel231;
                     HLH_gui_button *green_minus;
                     HLH_gui_slider *green;
                     HLH_gui_button *green_plus;
                  HLH_gui_panel *panel232;
                     HLH_gui_label *label232;
               HLH_gui_panel *panel24;
                  HLH_gui_panel *panel240;
                     HLH_gui_label *label241;
                  HLH_gui_panel *panel241;
                     HLH_gui_button *blue_minus;
                     HLH_gui_slider *blue;
                     HLH_gui_button *blue_plus;
                  HLH_gui_panel *panel242;
                     HLH_gui_label *label242;
               HLH_gui_panel *panel25;
                  HLH_gui_panel *panel250;
                     HLH_gui_label *label251;
                  HLH_gui_panel *panel251;
                     HLH_gui_button *colors_minus;
                     HLH_gui_slider *colors;
                     HLH_gui_button *colors_plus;
                  HLH_gui_panel *panel252;
                     HLH_gui_label *label252;
            HLH_gui_panel *panel26;
               HLH_gui_button *load_pal;
               HLH_gui_button *save_pal;
               HLH_gui_button *gen_pal;

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
            HLH_gui_panel *panel36;
                  HLH_gui_panel *panel360;
                     HLH_gui_label *label361;
                  HLH_gui_panel *panel361;
                     HLH_gui_button *sample_left;
                     HLH_gui_label *sample;
                     HLH_gui_button *sample_right;
            HLH_gui_panel *panel37;
                  HLH_gui_panel *panel370;
                     HLH_gui_label *label371;
                  HLH_gui_panel *panel371;
                     HLH_gui_button *xoff_minus;
                     HLH_gui_slider *xoff;
                     HLH_gui_button *xoff_plus;
                  HLH_gui_panel *panel372;
                     HLH_gui_label *label372;
            HLH_gui_panel *panel38;
                  HLH_gui_panel *panel380;
                     HLH_gui_label *label381;
                  HLH_gui_panel *panel381;
                     HLH_gui_button *yoff_minus;
                     HLH_gui_slider *yoff;
                     HLH_gui_button *yoff_plus;
                  HLH_gui_panel *panel382;
                     HLH_gui_label *label382;
            HLH_gui_panel *panel39;
                  HLH_gui_panel *panel390;
                     HLH_gui_label *label391;
                  HLH_gui_panel *panel391;
                     HLH_gui_button *gauss_minus;
                     HLH_gui_slider *gauss;
                     HLH_gui_button *gauss_plus;
                  HLH_gui_panel *panel392;
                     HLH_gui_label *label392;

         HLH_gui_panel *tab4;
            HLH_gui_panel *panel40;
               Palette_selection *pal4;
            HLH_gui_panel *panel41;
               HLH_gui_panel *panel42;
                  HLH_gui_panel *panel420;
                     HLH_gui_label *label421;
                  HLH_gui_panel *panel421;
                     HLH_gui_button *dither_left;
                     HLH_gui_label *dither;
                     HLH_gui_button *dither_right;
               HLH_gui_panel *panel43;
                  HLH_gui_panel *panel430;
                     HLH_gui_label *label431;
                  HLH_gui_panel *panel431;
                     HLH_gui_button *amount_minus;
                     HLH_gui_slider *amount;
                     HLH_gui_button *amount_plus;
                  HLH_gui_panel *panel432;
                     HLH_gui_label *label432;
               HLH_gui_panel *panel44;
                  HLH_gui_panel *panel440;
                     HLH_gui_label *label441;
                  HLH_gui_panel *panel441;
                     HLH_gui_button *alpha_minus;
                     HLH_gui_slider *alpha;
                     HLH_gui_button *alpha_plus;
                  HLH_gui_panel *panel442;
                     HLH_gui_label *label442;
               HLH_gui_panel *panel45;
                  HLH_gui_panel *panel450;
                     HLH_gui_label *label451;
                  HLH_gui_panel *panel451;
                     HLH_gui_button *dist_left;
                     HLH_gui_label *dist;
                     HLH_gui_button *dist_right;
               HLH_gui_panel *panel48;
                  HLH_gui_panel *panel480;
                     HLH_gui_label *label481;
                  HLH_gui_panel *panel481;
                     HLH_gui_button *weight_minus;
                     HLH_gui_slider *weight;
                     HLH_gui_button *weight_plus;
                  HLH_gui_panel *panel482;
                     HLH_gui_label *label482;
            HLH_gui_panel *panel46;
               HLH_gui_label *label460;
               HLH_gui_button *tick_inline;
               HLH_gui_label *label461;
               HLH_gui_button *tick_outline;
            HLH_gui_panel *panel47;
               HLH_gui_button *inline_set;
               HLH_gui_button *outline_set;

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
static int htab_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void update_output();
static void preset_load(FILE *f);
static void palette_draw();

static Palette_selection *palette_selection_create(HLH_gui_element *parent, uint32_t flags);
static void palette_selection_update(Palette_selection *s, uint32_t *data);
static int palsel_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
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
   settings.tab2 = HLH_gui_panel_create(&settings.panel0->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL|HLH_GUI_V_FILL);
   settings.tab2->gap = 40;
   HLH_gui_vtab_set(settings.vtab,1,"Palette");
   settings.panel20 = HLH_gui_panel_create(&settings.tab2->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL);
   settings.panel20->gap = 20;
   settings.panel20->border = HLH_gui_rect_make(4,4,4,4);
   settings.panel21 = HLH_gui_panel_create(&settings.tab2->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL);
   settings.panel21->gap = 16;
   settings.panel26 = HLH_gui_panel_create(&settings.tab2->e,HLH_GUI_PANEL_LIGHT);
   settings.panel26->gap = 16;
   settings.pal2 = palette_selection_create(&settings.panel20->e,HLH_GUI_H_FILL);

   settings.panel22 = HLH_gui_panel_create(&settings.panel21->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel220 = HLH_gui_panel_create(&settings.panel22->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel220->border = HLH_gui_rect_make(8,8,0,0);
   settings.label221 = HLH_gui_label_create(&settings.panel220->e,0,"Red   ",-1);
   settings.panel221 = HLH_gui_panel_create(&settings.panel22->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel221->border = HLH_gui_rect_make(8,8,0,0);
   settings.red_minus = HLH_gui_button_create(&settings.panel221->e,0,"-",-1);
   settings.red_minus->e.msg_usr = button_msg;
   settings.red = HLH_gui_slider_create(&settings.panel221->e, HLH_GUI_H_FILL,0,0,255);
   settings.red->e.msg_usr = slider_msg;
   settings.red_plus = HLH_gui_button_create(&settings.panel221->e,0,"+",-1);
   settings.red_plus->e.msg_usr = button_msg;
   settings.panel222 = HLH_gui_panel_create(&settings.panel22->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel222->border = HLH_gui_rect_make(8,8,0,0);
   settings.label222 = HLH_gui_label_create(&settings.panel222->e,0," 128",-1);

   settings.panel23 = HLH_gui_panel_create(&settings.panel21->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel230 = HLH_gui_panel_create(&settings.panel23->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel230->border = HLH_gui_rect_make(8,8,0,0);
   settings.label231 = HLH_gui_label_create(&settings.panel230->e,0,"Green ",-1);
   settings.panel231 = HLH_gui_panel_create(&settings.panel23->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel231->border = HLH_gui_rect_make(8,8,0,0);
   settings.green_minus = HLH_gui_button_create(&settings.panel231->e,0,"-",-1);
   settings.green_minus->e.msg_usr = button_msg;
   settings.green = HLH_gui_slider_create(&settings.panel231->e, HLH_GUI_H_FILL,0,0,255);
   settings.green->e.msg_usr = slider_msg;
   settings.green_plus = HLH_gui_button_create(&settings.panel231->e,0,"+",-1);
   settings.green_plus->e.msg_usr = button_msg;
   settings.panel232 = HLH_gui_panel_create(&settings.panel23->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel232->border = HLH_gui_rect_make(8,8,0,0);
   settings.label232 = HLH_gui_label_create(&settings.panel232->e,0," 128",-1);

   settings.panel24 = HLH_gui_panel_create(&settings.panel21->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel240 = HLH_gui_panel_create(&settings.panel24->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel240->border = HLH_gui_rect_make(8,8,0,0);
   settings.label241 = HLH_gui_label_create(&settings.panel240->e,0,"Blue  ",-1);
   settings.panel241 = HLH_gui_panel_create(&settings.panel24->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel241->border = HLH_gui_rect_make(8,8,0,0);
   settings.blue_minus = HLH_gui_button_create(&settings.panel241->e,0,"-",-1);
   settings.blue_minus->e.msg_usr = button_msg;
   settings.blue = HLH_gui_slider_create(&settings.panel241->e, HLH_GUI_H_FILL,0,0,255);
   settings.blue->e.msg_usr = slider_msg;
   settings.blue_plus = HLH_gui_button_create(&settings.panel241->e,0,"+",-1);
   settings.blue_plus->e.msg_usr = button_msg;
   settings.panel242 = HLH_gui_panel_create(&settings.panel24->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel242->border = HLH_gui_rect_make(8,8,0,0);
   settings.label242 = HLH_gui_label_create(&settings.panel242->e,0," 128",-1);

   settings.panel25 = HLH_gui_panel_create(&settings.panel21->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel250 = HLH_gui_panel_create(&settings.panel25->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel250->border = HLH_gui_rect_make(8,8,0,0);
   settings.label251 = HLH_gui_label_create(&settings.panel250->e,0,"Colors",-1);
   settings.panel251 = HLH_gui_panel_create(&settings.panel25->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel251->border = HLH_gui_rect_make(8,8,0,0);
   settings.colors_minus = HLH_gui_button_create(&settings.panel251->e,0,"-",-1);
   settings.colors_minus->e.msg_usr = button_msg;
   settings.colors = HLH_gui_slider_create(&settings.panel251->e, HLH_GUI_H_FILL,0,0,256);
   settings.colors->e.msg_usr = slider_msg;
   settings.colors_plus = HLH_gui_button_create(&settings.panel251->e,0,"+",-1);
   settings.colors_plus->e.msg_usr = button_msg;
   settings.panel252 = HLH_gui_panel_create(&settings.panel25->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel252->border = HLH_gui_rect_make(8,8,0,0);
   settings.label252 = HLH_gui_label_create(&settings.panel252->e,0," 128",-1);

   settings.load_pal = HLH_gui_button_create(&settings.panel26->e, 0, "Load palette", -1);
   settings.load_pal->e.msg_usr = button_msg;
   settings.save_pal = HLH_gui_button_create(&settings.panel26->e, 0, "Save palette", -1);
   settings.save_pal->e.msg_usr = button_msg;
   settings.gen_pal = HLH_gui_button_create(&settings.panel26->e, 0, "Generate palette", -1);
   settings.gen_pal->e.msg_usr = button_msg;

   //Tab 3 - Sample
   settings.tab3 = HLH_gui_panel_create(&settings.panel0->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL|HLH_GUI_V_FILL);
   settings.tab3->gap = 10;
   HLH_gui_vtab_set(settings.vtab,2,"Sample");
   settings.htab = HLH_gui_htab_create(&settings.tab3->e,HLH_GUI_H_FILL);
   settings.htab->e.msg_usr = htab_msg;
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
   settings.width = HLH_gui_slider_create(&settings.panel311->e, HLH_GUI_H_FILL,128,0,512);
   settings.width->e.msg_usr = slider_msg;
   settings.width_plus = HLH_gui_button_create(&settings.panel311->e,0,"+",-1);
   settings.width_plus->e.msg_usr = button_msg;
   settings.panel312 = HLH_gui_panel_create(&settings.panel31->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel312->border = HLH_gui_rect_make(8,8,0,0);
   settings.label312 = HLH_gui_label_create(&settings.panel312->e,0," 128",-1);

   settings.panel32 = HLH_gui_panel_create(&settings.panel30->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel320 = HLH_gui_panel_create(&settings.panel32->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel320->border = HLH_gui_rect_make(8,8,0,0);
   settings.label321 = HLH_gui_label_create(&settings.panel320->e,0,"Height",-1);
   settings.panel321 = HLH_gui_panel_create(&settings.panel32->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel321->border = HLH_gui_rect_make(8,8,0,0);
   settings.height_minus = HLH_gui_button_create(&settings.panel321->e,0,"-",-1);
   settings.height_minus->e.msg_usr = button_msg;
   settings.height = HLH_gui_slider_create(&settings.panel321->e, HLH_GUI_H_FILL,128,0,512);
   settings.height->e.msg_usr = slider_msg;
   settings.height_plus = HLH_gui_button_create(&settings.panel321->e,0,"+",-1);
   settings.height_plus->e.msg_usr = button_msg;
   settings.panel322 = HLH_gui_panel_create(&settings.panel32->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel322->border = HLH_gui_rect_make(8,8,0,0);
   settings.label322 = HLH_gui_label_create(&settings.panel322->e,0," 128",-1);

   settings.panel34 = HLH_gui_panel_create(&settings.panel33->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel340 = HLH_gui_panel_create(&settings.panel34->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel340->border = HLH_gui_rect_make(8,8,0,0);
   settings.label341 = HLH_gui_label_create(&settings.panel340->e,0,"x div ",-1);
   settings.panel341 = HLH_gui_panel_create(&settings.panel34->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel341->border = HLH_gui_rect_make(8,8,0,0);
   settings.xdiv_minus = HLH_gui_button_create(&settings.panel341->e,0,"-",-1);
   settings.xdiv_minus->e.msg_usr = button_msg;
   settings.xdiv = HLH_gui_slider_create(&settings.panel341->e, HLH_GUI_H_FILL,2,1,32);
   settings.xdiv->e.msg_usr = slider_msg;
   settings.xdiv_plus = HLH_gui_button_create(&settings.panel341->e,0,"+",-1);
   settings.xdiv_plus->e.msg_usr = button_msg;
   settings.panel342 = HLH_gui_panel_create(&settings.panel34->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel342->border = HLH_gui_rect_make(8,8,0,0);
   settings.label342 = HLH_gui_label_create(&settings.panel342->e,0,"   2",-1);

   settings.panel35 = HLH_gui_panel_create(&settings.panel33->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel350 = HLH_gui_panel_create(&settings.panel35->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel350->border = HLH_gui_rect_make(8,8,0,0);
   settings.label351 = HLH_gui_label_create(&settings.panel350->e,0,"y div ",-1);
   settings.panel351 = HLH_gui_panel_create(&settings.panel35->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel351->border = HLH_gui_rect_make(8,8,0,0);
   settings.ydiv_minus = HLH_gui_button_create(&settings.panel351->e,0,"-",-1);
   settings.ydiv_minus->e.msg_usr = button_msg;
   settings.ydiv = HLH_gui_slider_create(&settings.panel351->e, HLH_GUI_H_FILL,2,1,32);
   settings.ydiv->e.msg_usr = slider_msg;
   settings.ydiv_plus = HLH_gui_button_create(&settings.panel351->e,0,"+",-1);
   settings.ydiv_plus->e.msg_usr = button_msg;
   settings.panel352 = HLH_gui_panel_create(&settings.panel35->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel352->border = HLH_gui_rect_make(8,8,0,0);
   settings.label352 = HLH_gui_label_create(&settings.panel352->e,0,"   2",-1);

   settings.panel36 = HLH_gui_panel_create(&settings.tab3->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel36->border = HLH_gui_rect_make(0,0,40,0);
   settings.panel360 = HLH_gui_panel_create(&settings.panel36->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel360->border = HLH_gui_rect_make(8,8,0,0);
   settings.label361 = HLH_gui_label_create(&settings.panel360->e,0,"sample",-1);
   settings.panel361 = HLH_gui_panel_create(&settings.panel36->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel361->border = HLH_gui_rect_make(8,60,0,0);
   settings.sample_left = HLH_gui_button_create(&settings.panel361->e,0,"<",-1);
   settings.sample_left->e.msg_usr = button_msg;
   settings.sample = HLH_gui_label_create(&settings.panel361->e,HLH_GUI_H_FILL|HLH_GUI_LABEL_CENTER,"Round",-1);
   settings.sample_right = HLH_gui_button_create(&settings.panel361->e,0,">",-1);
   settings.sample_right->e.msg_usr = button_msg;

   settings.panel37 = HLH_gui_panel_create(&settings.tab3->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel370 = HLH_gui_panel_create(&settings.panel37->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel370->border = HLH_gui_rect_make(8,8,0,0);
   settings.label371 = HLH_gui_label_create(&settings.panel370->e,0,"x off ",-1);
   settings.panel371 = HLH_gui_panel_create(&settings.panel37->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel371->border = HLH_gui_rect_make(8,8,0,0);
   settings.xoff_minus = HLH_gui_button_create(&settings.panel371->e,0,"-",-1);
   settings.xoff_minus->e.msg_usr = button_msg;
   settings.xoff = HLH_gui_slider_create(&settings.panel371->e, HLH_GUI_H_FILL,0,0,100);
   settings.xoff->e.msg_usr = slider_msg;
   settings.xoff_plus = HLH_gui_button_create(&settings.panel371->e,0,"+",-1);
   settings.xoff_plus->e.msg_usr = button_msg;
   settings.panel372 = HLH_gui_panel_create(&settings.panel37->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel372->border = HLH_gui_rect_make(8,8,0,0);
   settings.label372 = HLH_gui_label_create(&settings.panel372->e,0,"   0",-1);

   settings.panel38 = HLH_gui_panel_create(&settings.tab3->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel380 = HLH_gui_panel_create(&settings.panel38->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel380->border = HLH_gui_rect_make(8,8,0,0);
   settings.label381 = HLH_gui_label_create(&settings.panel380->e,0,"y off ",-1);
   settings.panel381 = HLH_gui_panel_create(&settings.panel38->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel381->border = HLH_gui_rect_make(8,8,0,0);
   settings.yoff_minus = HLH_gui_button_create(&settings.panel381->e,0,"-",-1);
   settings.yoff_minus->e.msg_usr = button_msg;
   settings.yoff = HLH_gui_slider_create(&settings.panel381->e, HLH_GUI_H_FILL,0,0,100);
   settings.yoff->e.msg_usr = slider_msg;
   settings.yoff_plus = HLH_gui_button_create(&settings.panel381->e,0,"+",-1);
   settings.yoff_plus->e.msg_usr = button_msg;
   settings.panel382 = HLH_gui_panel_create(&settings.panel38->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel382->border = HLH_gui_rect_make(8,8,0,0);
   settings.label382 = HLH_gui_label_create(&settings.panel382->e,0,"   0",-1);

   settings.panel39 = HLH_gui_panel_create(&settings.tab3->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel390 = HLH_gui_panel_create(&settings.panel39->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel390->border = HLH_gui_rect_make(8,8,0,0);
   settings.label391 = HLH_gui_label_create(&settings.panel390->e,0,"Gauss ",-1);
   settings.panel391 = HLH_gui_panel_create(&settings.panel39->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel391->border = HLH_gui_rect_make(8,8,0,0);
   settings.gauss_minus = HLH_gui_button_create(&settings.panel391->e,0,"-",-1);
   settings.gauss_minus->e.msg_usr = button_msg;
   settings.gauss = HLH_gui_slider_create(&settings.panel391->e, HLH_GUI_H_FILL,80,0,500);
   settings.gauss->e.msg_usr = slider_msg;
   settings.gauss_plus = HLH_gui_button_create(&settings.panel391->e,0,"+",-1);
   settings.gauss_plus->e.msg_usr = button_msg;
   settings.panel392 = HLH_gui_panel_create(&settings.panel39->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel392->border = HLH_gui_rect_make(8,8,0,0);
   settings.label392 = HLH_gui_label_create(&settings.panel392->e,0,"  80",-1);

   //Tab 4 - Colors
   settings.tab4 = HLH_gui_panel_create(&settings.panel0->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL|HLH_GUI_V_FILL);
   settings.tab4->gap = 10;
   HLH_gui_vtab_set(settings.vtab,3,"Colors");
   settings.panel40 = HLH_gui_panel_create(&settings.tab4->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL);
   settings.panel40->gap = 20;
   settings.panel40->border = HLH_gui_rect_make(4,4,4,40);
   settings.panel41 = HLH_gui_panel_create(&settings.tab4->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_H_FILL);
   settings.panel41->gap = 8;
   settings.panel41->border = HLH_gui_rect_make(0,0,0,40);
   settings.panel46 = HLH_gui_panel_create(&settings.tab4->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel46->gap = 18;
   settings.panel46->border = HLH_gui_rect_make(8,60,0,0);
   settings.panel47 = HLH_gui_panel_create(&settings.tab4->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel47->gap = 18;
   settings.panel47->border = HLH_gui_rect_make(8,60,0,0);
   settings.pal4 = palette_selection_create(&settings.panel40->e,HLH_GUI_H_FILL);

   settings.panel42 = HLH_gui_panel_create(&settings.panel41->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel42->border = HLH_gui_rect_make(0,0,0,0);
   settings.panel420 = HLH_gui_panel_create(&settings.panel42->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel420->border = HLH_gui_rect_make(8,8,0,0);
   settings.label421 = HLH_gui_label_create(&settings.panel420->e,0,"dither",-1);
   settings.panel421 = HLH_gui_panel_create(&settings.panel42->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel421->border = HLH_gui_rect_make(8,60,0,0);
   settings.dither_left = HLH_gui_button_create(&settings.panel421->e,0,"<",-1);
   settings.dither_left->e.msg_usr = button_msg;
   settings.dither = HLH_gui_label_create(&settings.panel421->e,HLH_GUI_H_FILL|HLH_GUI_LABEL_CENTER,"Bayer 8x8",-1);
   settings.dither_right = HLH_gui_button_create(&settings.panel421->e,0,">",-1);
   settings.dither_right->e.msg_usr = button_msg;

   settings.panel43 = HLH_gui_panel_create(&settings.panel41->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel430 = HLH_gui_panel_create(&settings.panel43->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel430->border = HLH_gui_rect_make(8,8,0,0);
   settings.label431 = HLH_gui_label_create(&settings.panel430->e,0,"amount",-1);
   settings.panel431 = HLH_gui_panel_create(&settings.panel43->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel431->border = HLH_gui_rect_make(8,8,0,0);
   settings.amount_minus = HLH_gui_button_create(&settings.panel431->e,0,"-",-1);
   settings.amount_minus->e.msg_usr = button_msg;
   settings.amount = HLH_gui_slider_create(&settings.panel431->e, HLH_GUI_H_FILL,80,0,500);
   settings.amount->e.msg_usr = slider_msg;
   settings.amount_plus = HLH_gui_button_create(&settings.panel431->e,0,"+",-1);
   settings.amount_plus->e.msg_usr = button_msg;
   settings.panel432 = HLH_gui_panel_create(&settings.panel43->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel432->border = HLH_gui_rect_make(8,8,0,0);
   settings.label432 = HLH_gui_label_create(&settings.panel432->e,0,"  80",-1);

   settings.panel44 = HLH_gui_panel_create(&settings.panel41->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel440 = HLH_gui_panel_create(&settings.panel44->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel440->border = HLH_gui_rect_make(8,8,0,0);
   settings.label441 = HLH_gui_label_create(&settings.panel440->e,0,"alpha",-1);
   settings.panel441 = HLH_gui_panel_create(&settings.panel44->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel441->border = HLH_gui_rect_make(8,8,0,0);
   settings.alpha_minus = HLH_gui_button_create(&settings.panel441->e,0,"-",-1);
   settings.alpha_minus->e.msg_usr = button_msg;
   settings.alpha = HLH_gui_slider_create(&settings.panel441->e, HLH_GUI_H_FILL,80,0,500);
   settings.alpha->e.msg_usr = slider_msg;
   settings.alpha_plus = HLH_gui_button_create(&settings.panel441->e,0,"+",-1);
   settings.alpha_plus->e.msg_usr = button_msg;
   settings.panel442 = HLH_gui_panel_create(&settings.panel44->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel442->border = HLH_gui_rect_make(8,8,0,0);
   settings.label442 = HLH_gui_label_create(&settings.panel442->e,0,"  80",-1);

   settings.panel45 = HLH_gui_panel_create(&settings.panel41->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel45->border = HLH_gui_rect_make(0,0,0,0);
   settings.panel450 = HLH_gui_panel_create(&settings.panel45->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel450->border = HLH_gui_rect_make(8,8,0,0);
   settings.label451 = HLH_gui_label_create(&settings.panel450->e,0,"dist",-1);
   settings.panel451 = HLH_gui_panel_create(&settings.panel45->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel451->border = HLH_gui_rect_make(8,60,0,0);
   settings.dist_left = HLH_gui_button_create(&settings.panel451->e,0,"<",-1);
   settings.dist_left->e.msg_usr = button_msg;
   settings.dist = HLH_gui_label_create(&settings.panel451->e,HLH_GUI_H_FILL|HLH_GUI_LABEL_CENTER,"RGB",-1);
   settings.dist_right = HLH_gui_button_create(&settings.panel451->e,0,">",-1);
   settings.dist_right->e.msg_usr = button_msg;

   settings.panel48 = HLH_gui_panel_create(&settings.panel41->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel480 = HLH_gui_panel_create(&settings.panel48->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel480->border = HLH_gui_rect_make(8,8,0,0);
   settings.label481 = HLH_gui_label_create(&settings.panel480->e,0,"alpha",-1);
   settings.panel481 = HLH_gui_panel_create(&settings.panel48->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL|HLH_GUI_H_FILL);
   settings.panel481->border = HLH_gui_rect_make(8,8,0,0);
   settings.weight_minus = HLH_gui_button_create(&settings.panel481->e,0,"-",-1);
   settings.weight_minus->e.msg_usr = button_msg;
   settings.weight = HLH_gui_slider_create(&settings.panel481->e, HLH_GUI_H_FILL,80,0,500);
   settings.weight->e.msg_usr = slider_msg;
   settings.weight_plus = HLH_gui_button_create(&settings.panel481->e,0,"+",-1);
   settings.weight_plus->e.msg_usr = button_msg;
   settings.panel482 = HLH_gui_panel_create(&settings.panel48->e,HLH_GUI_PANEL_LIGHT|HLH_GUI_PANEL_HORIZONTAL);
   settings.panel482->border = HLH_gui_rect_make(8,8,0,0);
   settings.label482 = HLH_gui_label_create(&settings.panel482->e,0,"  80",-1);

   settings.label460 = HLH_gui_label_create(&settings.panel46->e,0,"inline ",-1);
   settings.tick_inline = HLH_gui_button_create(&settings.panel46->e,0," ",-1);
   settings.tick_inline->e.msg_usr = button_msg;
   HLH_gui_element_create(sizeof(HLH_gui_element),&settings.panel46->e,HLH_GUI_H_FILL,NULL);
   settings.label461 = HLH_gui_label_create(&settings.panel46->e,0,"outline",-1);
   settings.tick_outline = HLH_gui_button_create(&settings.panel46->e,0," ",-1);
   settings.tick_outline->e.msg_usr = button_msg;
   
   settings.inline_set = HLH_gui_button_create(&settings.panel47->e,0,"  inline  ",-1);
   HLH_gui_element_create(sizeof(HLH_gui_element),&settings.panel47->e,HLH_GUI_H_FILL,NULL);
   settings.outline_set = HLH_gui_button_create(&settings.panel47->e,0,"  outline  ",-1);

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

   palette_draw();
   HLH_gui_slider_set_value(settings.colors,img2pixel_get_palette()->used);
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

      //Palette
      else if(b==settings.red_minus) { HLH_gui_slider_set_value(settings.red,settings.red->value-1); }
      else if(b==settings.red_plus) { HLH_gui_slider_set_value(settings.red,settings.red->value+1); }
      else if(b==settings.green_minus) { HLH_gui_slider_set_value(settings.green,settings.green->value-1); }
      else if(b==settings.green_plus) { HLH_gui_slider_set_value(settings.green,settings.green->value+1); }
      else if(b==settings.blue_minus) { HLH_gui_slider_set_value(settings.blue,settings.blue->value-1); }
      else if(b==settings.blue_plus) { HLH_gui_slider_set_value(settings.blue,settings.blue->value+1); }
      else if(b==settings.colors_minus) { HLH_gui_slider_set_value(settings.colors,settings.colors->value-1); }
      else if(b==settings.colors_plus) { HLH_gui_slider_set_value(settings.colors,settings.colors->value+1); }
      else if(b==settings.load_pal)
      {
         SLK_Palette *p = palette_select();
         if(p!=NULL)
         {
            if(img2pixel_get_palette())
               free(img2pixel_get_palette());
            img2pixel_set_palette(p);
            palette_draw();
            HLH_gui_slider_set_value(settings.colors,img2pixel_get_palette()->used);

            update_output();
         }
      }
      else if(b==settings.save_pal)
      {
         palette_write(img2pixel_get_palette());
      }
      else if(b==settings.gen_pal)
      {
         img2pixel_quantize(img2pixel_get_palette()->used,sprite_in_org);
         palette_draw();

         update_output();
      }

      //Sample
      else if(b==settings.width_minus) { HLH_gui_slider_set_value(settings.width,settings.width->value-1); }
      else if(b==settings.width_plus) { HLH_gui_slider_set_value(settings.width,settings.width->value+1); }
      else if(b==settings.height_minus) { HLH_gui_slider_set_value(settings.height,settings.height->value-1); }
      else if(b==settings.height_plus) { HLH_gui_slider_set_value(settings.height,settings.height->value+1); }
      else if(b==settings.xdiv_minus) { HLH_gui_slider_set_value(settings.xdiv,settings.xdiv->value-1); }
      else if(b==settings.xdiv_plus) { HLH_gui_slider_set_value(settings.xdiv,settings.xdiv->value+1); }
      else if(b==settings.ydiv_minus) { HLH_gui_slider_set_value(settings.ydiv,settings.ydiv->value-1); }
      else if(b==settings.ydiv_plus) { HLH_gui_slider_set_value(settings.ydiv,settings.ydiv->value+1); }
      else if(b==settings.xoff_minus) { HLH_gui_slider_set_value(settings.xoff,settings.xoff->value-1); }
      else if(b==settings.xoff_plus) { HLH_gui_slider_set_value(settings.xoff,settings.xoff->value+1); }
      else if(b==settings.yoff_minus) { HLH_gui_slider_set_value(settings.yoff,settings.yoff->value-1); }
      else if(b==settings.yoff_plus) { HLH_gui_slider_set_value(settings.yoff,settings.yoff->value+1); }
      else if(b==settings.gauss_minus) { HLH_gui_slider_set_value(settings.gauss,settings.gauss->value-1); }
      else if(b==settings.gauss_plus) { HLH_gui_slider_set_value(settings.gauss,settings.gauss->value+1); }
      else if(b==settings.sample_left)
      {
         img2pixel_set_sample_mode(img2pixel_get_sample_mode()-1);
         if(img2pixel_get_sample_mode()<0)
            img2pixel_set_sample_mode(5);
         HLH_gui_label_set_text(settings.sample,text_sample[img2pixel_get_sample_mode()],-1);
         update_output();
      }
      else if(b==settings.sample_right)
      {
         img2pixel_set_sample_mode(img2pixel_get_sample_mode()+1);
         if(img2pixel_get_sample_mode()>5)
            img2pixel_set_sample_mode(0);
         HLH_gui_label_set_text(settings.sample,text_sample[img2pixel_get_sample_mode()],-1);
         update_output();
      }

      //Colors
      else if(b==settings.amount_minus) { HLH_gui_slider_set_value(settings.amount,settings.amount->value-1); }
      else if(b==settings.amount_plus) { HLH_gui_slider_set_value(settings.amount,settings.amount->value+1); }
      else if(b==settings.alpha_minus) { HLH_gui_slider_set_value(settings.alpha,settings.alpha->value-1); }
      else if(b==settings.alpha_plus) { HLH_gui_slider_set_value(settings.alpha,settings.alpha->value+1); }
      else if(b==settings.dither_left)
      {
         img2pixel_set_process_mode(img2pixel_get_process_mode()-1);
         if(img2pixel_get_process_mode()<0)
            img2pixel_set_process_mode(7);

         //TODO
         //if(img2pixel_get_process_mode()==6||img2pixel_get_process_mode()==7||img2pixel_get_process_mode()==0)
            //color_dither_enabled(0);
         //else
            //color_dither_enabled(1);

         update_output();
         HLH_gui_label_set_text(settings.dither,text_dither[img2pixel_get_process_mode()],-1);
      }
      else if(b==settings.dither_right)
      {
         img2pixel_set_process_mode(img2pixel_get_process_mode()+1);
         if(img2pixel_get_process_mode()>7)
            img2pixel_set_process_mode(0);

         //TODO
         //if(img2pixel_get_process_mode()==6||img2pixel_get_process_mode()==7||img2pixel_get_process_mode()==0)
            //color_dither_enabled(0);
         //else
            //color_dither_enabled(1);

         update_output();
         HLH_gui_label_set_text(settings.dither,text_dither[img2pixel_get_process_mode()],-1);
      }
      else if(b==settings.dist_left)
      {
         img2pixel_set_distance_mode(img2pixel_get_distance_mode()-1);;
         if(img2pixel_get_distance_mode()<0)
            img2pixel_set_distance_mode(8);

         //TODO
         //if(img2pixel_get_distance_mode()==8)
            //color_weight_enabled(1);
         //else
            //color_weight_enabled(0);

         update_output();
         HLH_gui_label_set_text(settings.dist,text_space[img2pixel_get_distance_mode()],-1);
      }
      else if(b==settings.dist_right)
      {
         img2pixel_set_distance_mode(img2pixel_get_distance_mode()+1);;
         if(img2pixel_get_distance_mode()>8)
            img2pixel_set_distance_mode(0);

         //TODO
         //if(img2pixel_get_distance_mode()==8)
            //color_weight_enabled(1);
         //else
            //color_weight_enabled(0);

         update_output();
         HLH_gui_label_set_text(settings.dist,text_space[img2pixel_get_distance_mode()],-1);
      }
      else if(b==settings.tick_inline)
      {
         if(b->text[0]==' ')
            img2pixel_set_inline(img2pixel_get_inline()+256);
         else
            img2pixel_set_inline(img2pixel_get_inline()-256);
         b->text[0] = (b->text[0]==' ')?'X':' ';
         update_output();
         HLH_gui_element_repaint(e,NULL);
      }
      else if(b==settings.tick_outline)
      {
         if(b->text[0]==' ')
            img2pixel_set_outline(img2pixel_get_outline()+256);
         else
            img2pixel_set_outline(img2pixel_get_outline()-256);
         b->text[0] = (b->text[0]==' ')?'X':' ';
         update_output();
         HLH_gui_element_repaint(e,NULL);
      }
      else if(b==settings.inline_set)
      {
         if(img2pixel_get_inline()<0)
            img2pixel_set_inline(settings.pal4->selected-256);
         else
            img2pixel_set_inline(settings.pal4->selected);
         update_output();
      }
      else if(b==settings.outline_set)
      {
         if(img2pixel_get_outline()<0)
            img2pixel_set_outline(settings.pal4->selected-256);
         else
            img2pixel_set_outline(settings.pal4->selected);
         update_output();
      }

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

      //Palette
      else if(s==settings.red) { sprintf(tmp,"%4d",settings.red->value); HLH_gui_label_set_text(settings.label222,tmp,-1); img2pixel_get_palette()->colors[settings.pal2->selected].rgb.r = settings.red->value; palette_draw(); update_output(); }
      else if(s==settings.green) { sprintf(tmp,"%4d",settings.green->value); HLH_gui_label_set_text(settings.label232,tmp,-1); img2pixel_get_palette()->colors[settings.pal2->selected].rgb.g = settings.green->value; palette_draw(); update_output(); }
      else if(s==settings.blue) { sprintf(tmp,"%4d",settings.blue->value); HLH_gui_label_set_text(settings.label242,tmp,-1); img2pixel_get_palette()->colors[settings.pal2->selected].rgb.b = settings.blue->value; palette_draw(); update_output(); }
      else if(s==settings.colors) { sprintf(tmp,"%4d",settings.colors->value); HLH_gui_label_set_text(settings.label252,tmp,-1); img2pixel_get_palette()->used = settings.colors->value; update_output(); }

      //Sample
      else if(s==settings.width) { sprintf(tmp,"%4d",settings.width->value); HLH_gui_label_set_text(settings.label312,tmp,-1); img2pixel_set_out_width(settings.width->value); update_output(); }
      else if(s==settings.height) { sprintf(tmp,"%4d",settings.height->value); HLH_gui_label_set_text(settings.label322,tmp,-1); img2pixel_set_out_height(settings.height->value); update_output(); }
      else if(s==settings.xdiv) { sprintf(tmp,"%4d",settings.xdiv->value); HLH_gui_label_set_text(settings.label342,tmp,-1); img2pixel_set_out_swidth(settings.xdiv->value); update_output(); }
      else if(s==settings.ydiv) { sprintf(tmp,"%4d",settings.ydiv->value); HLH_gui_label_set_text(settings.label352,tmp,-1); img2pixel_set_out_sheight(settings.ydiv->value); update_output(); }
      else if(s==settings.xoff) { sprintf(tmp,"%4d",settings.xoff->value); HLH_gui_label_set_text(settings.label372,tmp,-1); img2pixel_set_offset_x(settings.xoff->value); update_output(); }
      else if(s==settings.yoff) { sprintf(tmp,"%4d",settings.yoff->value); HLH_gui_label_set_text(settings.label382,tmp,-1); img2pixel_set_offset_y(settings.yoff->value); update_output(); }
      else if(s==settings.gauss)
      {
         sprintf(tmp,"%4d",settings.gauss->value);
         HLH_gui_label_set_text(settings.label392,tmp,-1);
         img2pixel_set_gauss(settings.gauss->value);
         img2pixel_lowpass_image(sprite_in_org,sprite_in);
         img2pixel_sharpen_image(sprite_in,sprite_in);
         update_output();
      }

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

static int htab_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_htab *h = e;

   if(msg==HLH_GUI_MSG_TAB_CHANGED)
   {
      if(h==settings.htab)
      {
         img2pixel_set_scale_mode(h->tab_current);
         update_output();
      }
   }

   return 0;
}

static Palette_selection *palette_selection_create(HLH_gui_element *parent, uint32_t flags)
{
   Palette_selection *p = (Palette_selection *)HLH_gui_element_create(sizeof(*p),parent,flags,palsel_msg);
   p->palette = SDL_CreateTexture(parent->window->renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,279,81);

   return p;
}

static void palette_selection_update(Palette_selection *s, uint32_t *data)
{
   void *pixels;
   int pitch;
   SDL_LockTexture(s->palette,NULL,&pixels,&pitch);
   memcpy(pixels,data,sizeof(*data)*279*81);
   SDL_UnlockTexture(s->palette);

   //HLH_gui_element_msg(&img->e,HLH_GUI_MSG_LAYOUT,0,NULL);
   HLH_gui_element_repaint(&s->e,NULL);

   s->selected = 0;
   SLK_Color c = img2pixel_get_palette()->colors[s->selected];
   if(s==settings.pal2)
   {
      HLH_gui_slider_set_value(settings.red,c.rgb.r);
      HLH_gui_slider_set_value(settings.green,c.rgb.g);
      HLH_gui_slider_set_value(settings.blue,c.rgb.b);
   }
}

static int palsel_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   Palette_selection *pal = e;

   if(msg==HLH_GUI_MSG_DESTROY)
   {
      SDL_DestroyTexture(pal->palette);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return 279*2*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return 81*2*HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_PAINT)
   {
      HLH_gui_painter *p = dp;

      SDL_Rect clip;
      clip.x = p->clip.l;
      clip.y = p->clip.t;
      clip.w = p->clip.r-p->clip.l;
      clip.h = p->clip.b-p->clip.t;
      SDL_RenderSetClipRect(p->win->renderer,&clip);

      SDL_Rect dst;
      dst.x = e->bounds.l;
      dst.y = e->bounds.t;
      dst.w = 279*2*HLH_gui_get_scale();
      dst.h = 81*2*HLH_gui_get_scale();

      SDL_RenderCopy(p->win->renderer,pal->palette,NULL,&dst);

      HLH_gui_rect rect;
      int pos_y = pal->selected/31;
      int pos_x = pal->selected-pos_y*31;
      rect.l = e->bounds.l+pos_x*18*HLH_gui_get_scale();
      rect.t = e->bounds.t+pos_y*18*HLH_gui_get_scale();
      rect.r = rect.l+HLH_gui_get_scale()*18;
      rect.b = rect.t+HLH_gui_get_scale()*18;
      SLK_Color c = img2pixel_get_palette()->colors[pal->selected];
      uint32_t color;
      if(c.rgb.r+c.rgb.g+c.rgb.b>384)
         color = 0;
      else
         color = UINT32_MAX;
      HLH_gui_draw_rectangle(p,rect,0,color);

      SDL_RenderSetClipRect(p->win->renderer,NULL);
   }
   else if(msg==HLH_GUI_MSG_CLICK)
   {
      int nx = e->window->mouse_x-e->bounds.l;
      int ny = e->window->mouse_y-e->bounds.t;
      int scale = 2*HLH_gui_get_scale();
      if(nx>=379*scale||ny>=81*scale||nx<0||ny<0)
         return 0;
      int new = (ny/(9*scale))*31+nx/(9*scale);
      if(new>255)
         return 0;
      pal->selected = new;

      SLK_Color c = img2pixel_get_palette()->colors[pal->selected];
      if(pal==settings.pal2)
      {
         HLH_gui_slider_set_value(settings.red,c.rgb.r);
         HLH_gui_slider_set_value(settings.green,c.rgb.g);
         HLH_gui_slider_set_value(settings.blue,c.rgb.b);
      }

      HLH_gui_element_repaint(e,NULL);
   }

   return 0;
}

void preset_load(FILE *f)
{
   char ctmp[16];
   img2pixel_preset_load(f);

   if(img2pixel_get_process_mode()==6||img2pixel_get_process_mode()==7||img2pixel_get_process_mode()==0)
      settings.panel43->e.flags|=HLH_GUI_INVISIBLE;
   else
      settings.panel43->e.flags&=~HLH_GUI_INVISIBLE;

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

static void palette_draw()
{
   SLK_RGB_sprite *old = SLK_draw_rgb_get_target();
   SLK_RGB_sprite *target = SLK_rgb_sprite_create(279,81);
   SLK_draw_rgb_set_target(target);
   SLK_draw_rgb_set_clear_color(SLK_color_create(0,0,0,255));
   SLK_draw_rgb_clear();

   for(int y = 0;y<9;y++)
      for(int x = 0;x<31;x++)
         if(y*31+x<img2pixel_get_palette()->used)
            SLK_draw_rgb_fill_rectangle(x*9,y*9,9,9,img2pixel_get_palette()->colors[y*31+x]);

   SLK_draw_rgb_set_target(old);
   palette_selection_update(settings.pal2,(uint32_t *)target->data);
   palette_selection_update(settings.pal4,(uint32_t *)target->data);
   //SLK_gui_image_update(elements.palette_palette,elements.palette_sprite,(SLK_gui_rectangle){0,0,279,81});
   SLK_rgb_sprite_destroy(target);
}
//-------------------------------------
