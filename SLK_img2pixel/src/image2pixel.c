/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//Quantization algorithm based on: https://github.com/ogus/kmeans-quantizer (wtfpl)

//External includes
#include <math.h>
#include <SLK/SLK.h>
#include <SLK/SLK_gui.h>

#define HLH_JSON_IMPLEMENTATION
#include "../../external/HLH_json.h"
//-------------------------------------

//Internal includes
#include "utility.h"
#include "image2pixel.h"
//-------------------------------------

//#defines
#define dyn_array_init(type, array, space) \
   do { ((dyn_array *)(array))->size = (space); ((dyn_array *)(array))->used = 0; ((dyn_array *)(array))->data = malloc(sizeof(type)*(((dyn_array *)(array))->size)); } while(0)

#define dyn_array_free(type, array) \
   do { if(((dyn_array *)(array))->data) { free(((dyn_array *)(array))->data); ((dyn_array *)(array))->data = NULL; ((dyn_array *)(array))->used = 0; ((dyn_array *)(array))->size = 0; }} while(0)

#define dyn_array_add(type, array, grow, element) \
   do { ((type *)((dyn_array *)(array)->data))[((dyn_array *)(array))->used] = (element); ((dyn_array *)(array))->used++; if(((dyn_array *)(array))->used==((dyn_array *)(array))->size) { ((dyn_array *)(array))->size+=grow; ((dyn_array *)(array))->data = realloc(((dyn_array *)(array))->data,sizeof(type)*(((dyn_array *)(array))->size)); } } while(0)

#define dyn_array_element(type, array, index) \
   (((type *)((dyn_array *)(array)->data))[index])

#define MIN(a,b) \
   ((a)<(b)?(a):(b))
 
#define MAX(a,b) \
   ((a)>(b)?(a):(b))

#define DEG2RAD(a) \
   ((a)*M_PI/180.0f)
//-------------------------------------

//Typedefs
typedef struct
{
   uint32_t used;
   uint32_t size;
   void *data;
}dyn_array;

typedef struct
{
   double l;
   double a;
   double b;
}Color_lab;

typedef struct
{
   double x;
   double y;
   double z;
}Color_xyz;

typedef struct
{
   double y;
   double cb;
   double cr;
}Color_ycc;

typedef struct
{
   double y;
   double i;
   double q;
}Color_yiq;

typedef struct
{
   double y;
   double u;
   double v;
}Color_yuv;

typedef struct
{
   double h;
   double s;
   double v;
}Color_hsv;

enum
{
   HIST_SIZE = 0x10000,
   MAX_COLORS = 256,
};

typedef struct
{
   uint16_t reference[HIST_SIZE];
   uint32_t counts[HIST_SIZE];
   uint32_t tcolors;              //total original colors in the histogram [length of reference]
   uint32_t total_pixels;     
}Histogramm;

typedef struct
{
   uint32_t index;
   uint32_t colors;
   uint32_t sum;
}Box;
//-------------------------------------

//Variables
static const int16_t dither_threshold_normal[64] = 
{
    0,32, 8,40, 2,34,10,42,
   48,16,56,24,50,18,58,26,
   12,44, 4,36,14,46, 6,38,
   60,28,52,20,62,30,54,22,
    3,35,11,43, 1,33, 9,41,
   51,19,59,27,49,17,57,25,
   15,47, 7,39,13,45, 5,37,
   63,31,55,23,61,29,53,21,
};
static const int16_t dither_threshold_none[64] = {0};

static SLK_Color palette_rgb[256];
static Color_lab palette_lab[256];
static Color_xyz palette_xyz[256];
static Color_ycc palette_ycc[256];
static Color_yiq palette_yiq[256];
static Color_yuv palette_yuv[256];

static int brightness = 0;
static int contrast = 0;
static int img_gamma = 100;
static int saturation = 100;
static int dither_amount = 250;
static int alpha_threshold = 128;
static int sharpen = 0;
static int hue = 0;
static int gauss = 80;
static int pixel_scale_mode = 0;
static int pixel_sample_mode = 0;
static int pixel_process_mode = 1;
static int pixel_distance_mode = 0;
static int image_out_width = 128;
static int image_out_height = 128;
static int image_out_swidth = 2;
static int image_out_sheight = 2;
static SLK_Palette *palette = NULL;

static dyn_array *quant_cluster_list = NULL;
static SLK_Color *quant_centroid_list = NULL;
static int *quant_assignment = NULL;
static int quant_k = 16;
//-------------------------------------

//Function prototypes
//Functions needed for dithering
static void orderd_dither(SLK_Color *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, const int16_t *dither_threshold, int distance_mode);
static void floyd_dither(SLK_Color *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, int distance_mode);
static void floyd2_dither(SLK_Color *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, int distance_mode);
static void floyd_apply_error(SLK_Color *d, double error_r, double error_g, double error_b, int x, int y, int width, int height);
static void dither_image(SLK_Color *d, SLK_RGB_sprite *out, SLK_Palette *palette, int process_mode, int width, int height, int distance_mode);
static double gauss_calc(double x, double y, double sigma);
static SLK_Color kernel_data_get(int x, int y, int width, int height, const SLK_RGB_sprite *data);

//Functions needed for finding closest colors
static void palette_setup(SLK_Palette *pal, int distance_mode);
static SLK_Color palette_find_closest(SLK_Palette *pal, SLK_Color c, int distance_mode);
static Color_lab color_to_lab(SLK_Color c);
static Color_xyz color_to_xyz(SLK_Color c);
static Color_ycc color_to_ycc(SLK_Color c);
static Color_yiq color_to_yiq(SLK_Color c);
static Color_yuv color_to_yuv(SLK_Color c);
static Color_hsv color_to_hsv(SLK_Color c);
static SLK_Color hsv_to_color(Color_hsv hsv);
static SLK_Color palette_find_closest_rgb(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_cie76(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_cie94(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_ciede2000(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_xyz(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_ycc(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_yiq(SLK_Palette *pal, SLK_Color c);
static SLK_Color palette_find_closest_yuv(SLK_Palette *pal, SLK_Color c);
static int64_t rgb_color_dist2(SLK_Color c0, SLK_Color c1);
static double cie94_color_dist2(Color_lab c0, Color_lab c1);
static double ciede2000_color_dist2(Color_lab c0, Color_lab c1);
static double color_dist2(double a0, double a1, double a2, double b0, double b1, double b2);

//Functions needed for downsampling image
static void sample_image(const SLK_RGB_sprite *in, SLK_Color *out, int sample_mode, int width, int height);
static void sample_round(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_floor(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_ceil(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_linear(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_bicubic(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static float cubic_hermite (float a, float b, float c, float d, float t);
static void sample_supersample(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_lanczos(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static double lanczos(double v);

//Functions needed for color quantization
static void quant_cluster_list_init();
static void quant_cluster_list_free();
static void quant_compute_kmeans(SLK_RGB_sprite *data);
static void quant_get_cluster_centroid(SLK_RGB_sprite *data);
static SLK_Color quant_colors_mean(dyn_array *color_list);
static SLK_Color quant_pick_random_color(SLK_RGB_sprite *data);
static int quant_nearest_color_idx(SLK_Color color, SLK_Color *color_list);
static float quant_distance(SLK_Color color0, SLK_Color color1);
static float quant_distancef(SLK_Color color0, SLK_Color color1);
static float quant_colors_variance(dyn_array *color_list);
//-------------------------------------

//Function implementations

void img2pixel_preset_load(FILE *f)
{
   if(!f)
      return;

   HLH_json5 fallback = {0};
   HLH_json5_root *root = HLH_json_parse_file_stream(f);

   HLH_json5 *o = HLH_json_get_object_object(&root->root,"palette",&fallback);
   palette->used = HLH_json_get_object_integer(o,"used",0);
   HLH_json5 *array = HLH_json_get_object(o,"colors");
   for(int i = 0;i<256;i++)
      palette->colors[i].n = HLH_json_get_array_integer(array,i,0);
   pixel_distance_mode = HLH_json_get_object_integer(&root->root,"distance_mode",0);
   image_out_width = HLH_json_get_object_integer(&root->root,"width",1);
   image_out_height = HLH_json_get_object_integer(&root->root,"height",1);
   image_out_swidth = HLH_json_get_object_integer(&root->root,"swidth",1);
   image_out_sheight = HLH_json_get_object_integer(&root->root,"sheight",1);
   pixel_scale_mode = HLH_json_get_object_integer(&root->root,"scale_mode",1);
   pixel_process_mode = HLH_json_get_object_integer(&root->root,"dither_mode",0);
   dither_amount = HLH_json_get_object_integer(&root->root,"dither_amount",1);
   pixel_sample_mode = HLH_json_get_object_integer(&root->root,"sample_mode",0);
   alpha_threshold = HLH_json_get_object_integer(&root->root,"alpha_threshold",128);
   upscale = HLH_json_get_object_integer(&root->root,"upscale",1);
   gauss = HLH_json_get_object_integer(&root->root,"gaussian_blur",128);
   brightness = HLH_json_get_object_integer(&root->root,"brightness",0);
   contrast = HLH_json_get_object_integer(&root->root,"contrast",0);
   saturation = HLH_json_get_object_integer(&root->root,"saturation",0);
   img_gamma = HLH_json_get_object_integer(&root->root,"gamma",0);
   sharpen = HLH_json_get_object_integer(&root->root,"sharpness",0);
   hue = HLH_json_get_object_integer(&root->root,"hue",0);

   HLH_json_free(root);
}

void img2pixel_preset_save(FILE *f)
{
   if(!f)
      return;

   HLH_json5_root *root = HLH_json_create_root();
   HLH_json5 object = HLH_json_create_object();
   HLH_json_object_add_integer(&object,"used",palette->used);
   HLH_json5 array = HLH_json_create_array();
   for(int i = 0;i<256;i++)
      HLH_json_array_add_integer(&array,palette->colors[i].n);
   HLH_json_object_add_object(&object,"colors",array);
   HLH_json_object_add_object(&root->root,"palette",object);
   HLH_json_object_add_integer(&root->root,"distance_mode",pixel_distance_mode);
   HLH_json_object_add_integer(&root->root,"width",image_out_width);
   HLH_json_object_add_integer(&root->root,"height",image_out_height);
   HLH_json_object_add_integer(&root->root,"swidth",image_out_swidth);
   HLH_json_object_add_integer(&root->root,"sheight",image_out_sheight);
   HLH_json_object_add_integer(&root->root,"scale_mode",pixel_scale_mode);
   HLH_json_object_add_integer(&root->root,"dither_mode",pixel_process_mode);
   HLH_json_object_add_integer(&root->root,"dither_amount",dither_amount);
   HLH_json_object_add_integer(&root->root,"sample_mode",pixel_sample_mode);
   HLH_json_object_add_integer(&root->root,"gaussian_blur",gauss);
   HLH_json_object_add_integer(&root->root,"alpha_threshold",alpha_threshold);
   HLH_json_object_add_integer(&root->root,"upscale",upscale);
   HLH_json_object_add_integer(&root->root,"brightness",brightness);
   HLH_json_object_add_integer(&root->root,"contrast",contrast);
   HLH_json_object_add_integer(&root->root,"gamma",img_gamma);
   HLH_json_object_add_integer(&root->root,"saturation",saturation);
   HLH_json_object_add_integer(&root->root,"sharpness",sharpen);
   HLH_json_object_add_integer(&root->root,"hue",hue);
 
   HLH_json_write_file(f,&root->root);
   HLH_json_free(root);
   fclose(f);
}

//Sharpens an image, input and output dimensions must be equal
void img2pixel_sharpen_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   if(in==NULL||out==NULL||in->width!=out->width||in->height!=out->height)
      return;

   SLK_RGB_sprite *tmp_data2 = SLK_rgb_sprite_create(out->width,out->height);
   if(tmp_data2==NULL)
      return;

   if(sharpen==0)
   {
      SLK_rgb_sprite_copy(out,in);
      return;
   }
   
   SLK_rgb_sprite_copy(tmp_data2,in);

   //Setup sharpening kernel
   float sharpen_factor = (float)sharpen/100.0f;
   float sharpen_kernel[3][3] = {
      {0.0f,-1.0f*sharpen_factor,0.0f},
      {-1.0f*sharpen_factor,4.0f*sharpen_factor+1.0f,-1.0f*sharpen_factor},
      {0.0f,-1.0f*sharpen_factor,0.0f},
   };

   for(int y = 0;y<out->height;y++)
   {
      for(int x = 0;x<out->width;x++)
      {
         float r = 0.0f;
         float g = 0.0f;
         float b = 0.0f;

         //Apply kernel
         for(int yk = -1;yk<2;yk++)
         {
            for(int xk = -1;xk<2;xk++)
            {
               SLK_Color c = kernel_data_get(x+xk,y+yk,out->width,out->height,tmp_data2);

               r+=sharpen_kernel[yk+1][xk+1]*(float)c.r;
               g+=sharpen_kernel[yk+1][xk+1]*(float)c.g;
               b+=sharpen_kernel[yk+1][xk+1]*(float)c.b;
            }
         }

         out->data[y*out->width+x].r = MAX(0,MIN(0xff,(int)r));
         out->data[y*out->width+x].g = MAX(0,MIN(0xff,(int)g));
         out->data[y*out->width+x].b = MAX(0,MIN(0xff,(int)b));
         out->data[y*out->width+x].a = in->data[y*out->width+x].a;
      }
   }

   //Cleanup
   SLK_rgb_sprite_destroy(tmp_data2);
}

void img2pixel_lowpass_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   if(in==NULL||out==NULL||in->width!=out->width||in->height!=out->height)
      return;

   SLK_RGB_sprite *tmp_data2 = SLK_rgb_sprite_create(out->width,out->height);
   if(tmp_data2==NULL)
      return;

   if(gauss==0)
   {
      SLK_rgb_sprite_copy(out,in);
      return;
   }
   
   SLK_rgb_sprite_copy(tmp_data2,in);

   //Setup lowpass kernel
   //We need to divide each cell
   //by the sum of all cells to make 
   //sure the sum of all cells will be 1
   //otherwise the blur would either darken
   //or lighten the image
   double gauss_factor = (double)gauss/100.0f;
   double lowpass_kernel[7][7];
   for(int y = 0;y<7;y++)
      for(int x = 0;x<7;x++)
         lowpass_kernel[x][y] = gauss_calc((double)x-3.0f,(double)y-3.0f,gauss_factor);
   double norm_val = 0.0f;
   for(int y = 0;y<7;y++)
      for(int x = 0;x<7;x++)
         norm_val+=lowpass_kernel[x][y];
   for(int y = 0;y<7;y++)
      for(int x = 0;x<7;x++)
         lowpass_kernel[x][y] = lowpass_kernel[x][y]/norm_val;

   for(int y = 0;y<out->height;y++)
   {
      for(int x = 0;x<out->width;x++)
      {
         double r = 0.0f;
         double g = 0.0f;
         double b = 0.0f;

         //Apply kernel
         for(int yk = -3;yk<4;yk++)
         {
            for(int xk = -3;xk<4;xk++)
            {
               SLK_Color c = kernel_data_get(x+xk,y+yk,out->width,out->height,tmp_data2);

               r+=lowpass_kernel[xk+3][yk+3]*(double)c.r;
               g+=lowpass_kernel[xk+3][yk+3]*(double)c.g;
               b+=lowpass_kernel[xk+3][yk+3]*(double)c.b;
            }
         }

         out->data[y*out->width+x].r = MAX(0,MIN(0xff,(int)r));
         out->data[y*out->width+x].g = MAX(0,MIN(0xff,(int)g));
         out->data[y*out->width+x].b = MAX(0,MIN(0xff,(int)b));
         out->data[y*out->width+x].a = in->data[y*out->width+x].a;
      }
   }

   //Cleanup
   SLK_rgb_sprite_destroy(tmp_data2);
}

void img2pixel_quantize(int colors, SLK_RGB_sprite *in)
{
   if(in==NULL||palette==NULL)
      return;

   SLK_RGB_sprite *tmp = SLK_rgb_sprite_create(512,512);
   sample_image(in,tmp->data,0,512,512);

   quant_k = colors;
   quant_compute_kmeans(tmp);
   palette->used = colors;
   for(int i = 0;i<colors;i++)
   {
      palette->colors[i] = quant_centroid_list[i];
      palette->colors[i].a = 255;
   }


   quant_cluster_list_free();
   free(quant_centroid_list);
   free(quant_assignment);
   SLK_rgb_sprite_destroy(tmp);
}

void img2pixel_process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   SLK_Color *tmp_data = malloc(sizeof(*tmp_data)*out->width*out->height);
   if(tmp_data==NULL)
      return;

   palette_setup(palette,pixel_distance_mode);

   //Downsample image before processing it. 
   //Every image operation except kernel based ones (sharpness, gaussian blur)
   //is done after downsampling.
   sample_image(in,tmp_data,pixel_sample_mode,out->width,out->height);

   //Adjust range of values
   float gamma_factor = (float)img_gamma/100.0f;
   float contrast_factor = (259.0f*(255.0f+(float)contrast))/(255.0f*(259.0f-(float)contrast));
   float saturation_factor = (float)saturation/100.0f;
   float brightness_factor = (float)brightness/255.0f;

   //Setup "matrix"
   //saturation, brighness and contrast are implemented with a color matrix
   //(with unused parts removed).
   //See here: https://docs.rainmeter.net/tips/colormatrix-guide/
   float t = (1.0f-contrast_factor)/2.0f;

   float sr = (1.0f-saturation_factor)*0.3086f;
   float sg = (1.0f-saturation_factor)*0.6094f;
   float sb = (1.0f-saturation_factor)*0.0820f;

   float rr = contrast_factor*(sr+saturation_factor);
   float rg = contrast_factor*sr;
   float rb = contrast_factor*sr;

   float gr = contrast_factor*sg;
   float gg = contrast_factor*(sg+saturation_factor);
   float gb = contrast_factor*sg;

   float br = contrast_factor*sb;
   float bg = contrast_factor*sb;
   float bb = contrast_factor*(sb+saturation_factor);

   float wr = (t+brightness_factor)*255.0f;
   float wg = (t+brightness_factor)*255.0f;
   float wb = (t+brightness_factor)*255.0f;
   //-------------------------------------

   for(int y = 0;y<out->height;y++)
   {
      for(int x = 0;x<out->width;x++)
      {
         SLK_Color in = tmp_data[y*out->width+x];

         //Hue
         //Only ajust if not the default value --> better performance
         if(hue!=0)
         {
            float huef = (float)hue;
            Color_hsv hsv = color_to_hsv(in);
            hsv.h+=huef;
            in = hsv_to_color(hsv);
         }

         //Saturation, brightness and contrast
         float r = (float)in.r;
         float g = (float)in.g;
         float b = (float)in.b;
         in.r = MAX(0,MIN(0xff,(int)(rr*r)+(gr*g)+(br*b)+wr));
         in.g = MAX(0,MIN(0xff,(int)(rg*r)+(gg*g)+(bg*b)+wg));
         in.b = MAX(0,MIN(0xff,(int)(rb*r)+(gb*g)+(bb*b)+wb));

         //Gamma
         //Only ajust if not the default value --> better performance
         if(img_gamma!=100)
         {
            in.r = MAX(0,MIN(0xff,(int)(255.0f*pow((float)in.r/255.0f,gamma_factor))));
            in.g = MAX(0,MIN(0xff,(int)(255.0f*pow((float)in.g/255.0f,gamma_factor))));
            in.b = MAX(0,MIN(0xff,(int)(255.0f*pow((float)in.b/255.0f,gamma_factor))));
         }

         tmp_data[y*out->width+x] = in;
      }
   }

   //Dithering is done after all image processing
   //If it was done at any other time, it would
   //resoult in different colors than the palette
   dither_image(tmp_data,out,palette,pixel_process_mode,out->width,out->height,pixel_distance_mode);

   //Clean up
   free(tmp_data);
}

void img2pixel_reset_to_defaults()
{
   brightness = 0;
   contrast = 0;
   img_gamma = 100;
   saturation = 100;
   dither_amount = 250;
   alpha_threshold = 128;
   sharpen = 0;
   hue = 0;
   gauss = 80;
   pixel_scale_mode = 0;
   pixel_sample_mode = 0;
   pixel_process_mode = 1;
   pixel_distance_mode = 0;
   image_out_width = 128;
   image_out_height = 128;
   image_out_swidth = 2;
   image_out_sheight = 2;
}

int img2pixel_get_brightness()
{
   return brightness;
}

void img2pixel_set_brightness(int nbrightness)
{
   brightness = nbrightness;
}

int img2pixel_get_contrast()
{
   return contrast;
}

void img2pixel_set_contrast(int ncontrast)
{
   contrast = ncontrast;
}

int img2pixel_get_gamma()
{
   return img_gamma;
}

void img2pixel_set_gamma(int ngamma)
{
   img_gamma = ngamma;
}

int img2pixel_get_saturation()
{
   return saturation;
}

void img2pixel_set_saturation(int nsaturation)
{
   saturation = nsaturation;
}

int img2pixel_get_sharpen()
{
   return sharpen;
}

void img2pixel_set_sharpen(int nsharpen)
{
   sharpen = nsharpen;
}

int img2pixel_get_hue()
{
   return hue;
}

void img2pixel_set_hue(int nhue)
{
   hue = nhue;
}

int img2pixel_get_dither_amount()
{
   return dither_amount;
}

void img2pixel_set_dither_amount(int namount)
{
   dither_amount = namount;
}

int img2pixel_get_alpha_threshold()
{
   return alpha_threshold;
}

void img2pixel_set_alpha_threshold(int nthreshold)
{
   alpha_threshold = nthreshold;
}

int img2pixel_get_gauss()
{
   return gauss;
}

void img2pixel_set_gauss(int ngauss)
{
   gauss = ngauss;
}

SLK_Palette *img2pixel_get_palette()
{
   return palette;
}

void img2pixel_set_palette(SLK_Palette *npalette)
{
   palette = npalette;
}

int img2pixel_get_scale_mode()
{
   return pixel_scale_mode;
}

void img2pixel_set_scale_mode(int nmode)
{
   pixel_scale_mode = nmode;
}

int img2pixel_get_sample_mode()
{
   return pixel_sample_mode;
}

void img2pixel_set_sample_mode(int nmode)
{
   pixel_sample_mode = nmode;
}

int img2pixel_get_process_mode()
{
   return pixel_process_mode;
}

void img2pixel_set_process_mode(int nmode)
{
   pixel_process_mode = nmode;
}

int img2pixel_get_distance_mode()
{
   return pixel_distance_mode;
}

void img2pixel_set_distance_mode(int nmode)
{
   pixel_distance_mode = nmode;
}

int img2pixel_get_out_width()
{
   return image_out_width;
}

void img2pixel_set_out_width(int nwidth)
{
   image_out_width = nwidth;
}

int img2pixel_get_out_height()
{
   return image_out_height;
}

void img2pixel_set_out_height(int nheight)
{
   image_out_height = nheight;
}

int img2pixel_get_out_swidth()
{
   return image_out_swidth;
}

void img2pixel_set_out_swidth(int nwidth)
{
   image_out_swidth = nwidth;
}

int img2pixel_get_out_sheight()
{
   return image_out_sheight;
}

void img2pixel_set_out_sheight(int nheight)
{
   image_out_sheight = nheight;
}

//Helper function for lowpass_image
static double gauss_calc(double x, double y, double sigma)
{
   double val = (1.0f/(2.0f*M_PI*sigma*sigma))*pow(M_E,-((x*x+y*y)/(2*sigma*sigma)));
   return val;
}

//Helper function for lowpass_image and shapren_image
static SLK_Color kernel_data_get(int x, int y, int width, int height, const SLK_RGB_sprite *data)
{
   if(x<0)
      return kernel_data_get(0,y,width,height,data);
   if(y<0)
      return kernel_data_get(x,0,width,height,data);
   if(x>width-1)
      return kernel_data_get(width-1,y,width,height,data);
   if(y>height-1)
      return kernel_data_get(x,height-1,width,height,data);

   return data->data[y*width+x];
}

//Dithers an image to the provided palette using the specified mode
static void dither_image(SLK_Color *d, SLK_RGB_sprite *out, SLK_Palette *palette, int process_mode, int width, int height, int distance_mode)
{
   switch(process_mode)
   {
   case 0: //No dithering
      orderd_dither(d,out,palette,width,height,dither_threshold_none,distance_mode);
      break;
   case 1: //Ordered dithering (positiv map --> bias towards lighter colors)
      {
      int16_t dither_threshold_tmp[64] = {0};
      for(int i = 0;i<64;i++)
         dither_threshold_tmp[i] = (int)((float)dither_threshold_normal[i]*((float)dither_amount/1000.0f));
      orderd_dither(d,out,palette,width,height,dither_threshold_tmp,distance_mode);
      }
      break;
   case 2: //Ordered dithering (positiv and negativ map --> no bias)
      {
      int16_t dither_threshold_tmp[64] = {0};
      for(int i = 0;i<64;i++)
         dither_threshold_tmp[i] = (int)((float)(dither_threshold_normal[i]-31)*((float)dither_amount/1000.0f));
      orderd_dither(d,out,palette,width,height,dither_threshold_tmp,distance_mode);
      }
      break;
   case 3: //Floyd-Steinberg dithering (per color component error)
      floyd_dither(d,out,palette,width,height,distance_mode);
      break;
   case 4: //Floyd-Steinberg dithering (distributed error)
      floyd2_dither(d,out,palette,width,height,distance_mode);
      break;
   }
}

//Applies ordered dithering to the input
static void orderd_dither(SLK_Color *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, const int16_t *dither_threshold, int distance_mode)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      { 
         SLK_Color in = d[y*width+x];
         if(in.a<alpha_threshold)
         {
            out->data[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }

         //Add a value to the color depending on the position,
         //this creates the dithering effect
         uint8_t tresshold_id = ((y&7)<<3)+(x&7);
         SLK_Color c;
         c.r = MAX(0,MIN(0xff,(in.r+dither_threshold[tresshold_id])));
         c.g = MAX(0,MIN(0xff,(in.g+dither_threshold[tresshold_id])));
         c.b = MAX(0,MIN(0xff,(in.b+dither_threshold[tresshold_id])));
         c.a = in.a;
         out->data[y*width+x] = palette_find_closest(pal,c,distance_mode);
         out->data[y*width+x].a = 255;
      }
   }
}

//Applies Floyd-Steinberg dithering to the input
//This version uses per color component errror values,
//this usually does not work well with most palettes
static void floyd_dither(SLK_Color *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, int distance_mode)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color in = d[y*width+x];
         if(in.a<alpha_threshold)
         {
            out->data[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         
         SLK_Color p = palette_find_closest(pal,in,distance_mode);
         double error_r = (double)in.r-(double)p.r;
         double error_g = (double)in.g-(double)p.g;
         double error_b = (double)in.b-(double)p.b;
         floyd_apply_error(d,error_r*(7.0/16.0),error_g*(7.0/16.0),error_b*(7.0/16.0),x+1,y,width,height);
         floyd_apply_error(d,error_r*(3.0/16.0),error_g*(3.0/16.0),error_b*(3.0/16.0),x-1,y+1,width,height);
         floyd_apply_error(d,error_r*(5.0/16.0),error_g*(5.0/16.0),error_b*(5.0/16.0),x,y+1,width,height);
         floyd_apply_error(d,error_r*(1.0/16.0),error_g*(1.0/16.0),error_b*(1.0/16.0),x+1,y+1,width,height);

         out->data[y*width+x] = p;
         out->data[y*width+x].a = 255;
      }
   }
}

//Applies Floyd-Steinberg dithering to the input
//This version uses distributed error values,
//this results in better results for most palettes
static void floyd2_dither(SLK_Color *d, SLK_RGB_sprite *out, SLK_Palette *pal, int width, int height, int distance_mode)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color in = d[y*width+x];
         if(in.a<alpha_threshold)
         {
            out->data[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         
         SLK_Color p = palette_find_closest(pal,in,distance_mode);
         double error = ((double)in.r-(double)p.r);
         error+=((double)in.g-(double)p.g);
         error+=((double)in.b-(double)p.b);
         error = error/3.0;
         floyd_apply_error(d,error*(7.0/16.0),error*(7.0/16.0),error*(7.0/16.0),x+1,y,width,height);
         floyd_apply_error(d,error*(3.0/16.0),error*(3.0/16.0),error*(3.0/16.0),x-1,y+1,width,height);
         floyd_apply_error(d,error*(5.0/16.0),error*(5.0/16.0),error*(5.0/16.0),x,y+1,width,height);
         floyd_apply_error(d,error*(1.0/16.0),error*(1.0/16.0),error*(1.0/16.0),x+1,y+1,width,height);

         out->data[y*width+x] = p;
         out->data[y*width+x].a = 255;
      }
   }
}

//Helper function for floyd_dither and floyd2_dither
static void floyd_apply_error(SLK_Color *d, double error_r, double error_g, double error_b, int x, int y, int width, int height)
{
   if(x>width-1||x<0||y>height-1||y<0)
      return;

   int r,g,b;
   SLK_Color *in = &d[y*width+x];
   r = in->r+error_r;
   g = in->g+error_g;
   b = in->b+error_b;

   in->r = MAX(0,MIN(r,255));
   in->g = MAX(0,MIN(g,255));
   in->b = MAX(0,MIN(b,255));
}

static void palette_setup(SLK_Palette *pal, int distance_mode)
{
   switch(distance_mode)
   {
   case 0: //RGB
      for(int i = 0;i<pal->used;i++)
         palette_rgb[i] = pal->colors[i];
      break;
   case 1:
   case 2:
   case 3: //Lab
      for(int i = 0;i<pal->used;i++)
         palette_lab[i] = color_to_lab(pal->colors[i]);
      break;
   case 4: //XYZ
      for(int i = 0;i<pal->used;i++)
         palette_xyz[i] = color_to_xyz(pal->colors[i]);
      break;
   case 5: //YCC
      for(int i = 0;i<pal->used;i++)
         palette_ycc[i] = color_to_ycc(pal->colors[i]);
      break;
   case 6: //YIQ
      for(int i = 0;i<pal->used;i++)
         palette_yiq[i] = color_to_yiq(pal->colors[i]);
      break;
   case 7: //YUV
      for(int i = 0;i<pal->used;i++)
         palette_yuv[i] = color_to_yuv(pal->colors[i]);
      break;
   }
}

static SLK_Color palette_find_closest(SLK_Palette *pal, SLK_Color c, int distance_mode)
{
   SLK_Color out = {0};

   switch(distance_mode)
   {
   case 0: //RGB
      out = palette_find_closest_rgb(pal,c);
      break;
   case 1: //CIE76
      out = palette_find_closest_cie76(pal,c);
      break;
   case 2: //CIE94
      out = palette_find_closest_cie94(pal,c);
      break;
   case 3: //CIEDE200
      out = palette_find_closest_ciede2000(pal,c);
      break;
   case 4: //XYZ
      out = palette_find_closest_xyz(pal,c);
      break;
   case 5: //YCC
      out = palette_find_closest_ycc(pal,c);
      break;
   case 6: //YIQ
      out = palette_find_closest_yiq(pal,c);
      break;
   case 7: //YUV
      out = palette_find_closest_yuv(pal,c);
      break;
   }

   return out;
}

//Convert to xyz then to lab color space
static Color_lab color_to_lab(SLK_Color c)
{
   Color_lab l;
   Color_xyz xyz = color_to_xyz(c) ;
 
   //x component
   if(xyz.x>0.008856f)
      xyz.x = pow(xyz.x,1.0f/3.0f);
   else
      xyz.x = (7.787f*xyz.x)+(16.0f/116.0f);

   //y component
   if(xyz.y>0.008856f)
      xyz.y = pow(xyz.y,1.0f/3.0f);
   else
      xyz.y = (7.787f*xyz.y)+(16.0f/116.0f);

   //z component
   if(xyz.z>0.008856f)
      xyz.z = pow(xyz.z,1.0f/3.0f);
   else
      xyz.z = (7.787f*xyz.z)+(16.0f/116.0f);

   l.l = 116.0f*xyz.y-16.0f;
   l.a = 500.0f*(xyz.x-xyz.y);
   l.b = 200.0f*(xyz.y-xyz.z);

   return l;
}

static Color_xyz color_to_xyz(SLK_Color c)
{
   double r,g,b;
   Color_xyz x;

   //red component
   r = (double)c.r/255.0f; 
   if(r>0.04045f)
      r = pow((r+0.055f)/1.055f,2.4f)*100.0f;
   else
      r = (r/12.92f)*100.0f;
  
   //green component
   g = (double)c.g/255.0f; 
   if(g>0.04045f)
      g = pow((g+0.055f)/1.055f,2.4f)*100.0f;
   else
      g = (g/12.92f)*100.0f;
  
   //blue component
   b = (double)c.b/255.0f; 
   if(b>0.04045f)
      b = pow((b+0.055f)/1.055f,2.4f)*100.0f;
   else
      b = (b/12.92f)*100.0f;

   x.x = (r*0.4124f+g*0.3576f+b*0.1805f)/95.047f;
   x.y = (r*0.2126+g*0.7152+b*0.0722)/100.0f;
   x.z = (r*0.0193+g*0.1192+b*0.9504)/108.883f;
 
   return x;
}

static Color_ycc color_to_ycc(SLK_Color c)
{
   double r = (double)c.r;
   double g = (double)c.g;
   double b = (double)c.b;
   Color_ycc y;

   y.y = 0.299f*r+0.587f*g+0.114f*b;
   y.cb = -0.16874f*r-0.33126f*g+0.5f*b;
   y.cr = 0.5f*r-0.41869f*g-0.08131f*b;

   return y;
}

static Color_yiq color_to_yiq(SLK_Color c)
{
   double r = (double)c.r;
   double g = (double)c.g;
   double b = (double)c.b;
   Color_yiq y;

   y.y = 0.2999f*r+0.587f*g+0.114f*b;
   y.i = 0.595716f*r-0.274453f*g-0.321264f*b;
   y.q = 0.211456f*r-0.522591f*g+0.31135f*b;

   return y;
}

static Color_yuv color_to_yuv(SLK_Color c)
{
   double r = (double)c.r;
   double g = (double)c.g;
   double b = (double)c.b;
   Color_yuv y;

   y.y = 0.2999f*r+0.587f*g+0.114f*b;
   y.u = 0.492f*(b-y.y);
   y.v = 0.887f*(r-y.y);

   return y;
}

static Color_hsv color_to_hsv(SLK_Color c)
{
   float r = (float)c.r/255.0f;
   float g = (float)c.g/255.0f;
   float b = (float)c.b/255.0f;
   float cmax = MAX(r,MAX(g,b));
   float cmin = MIN(r,MIN(g,b));
   float diff = cmax-cmin;
   Color_hsv hsv = {0};

   if(cmax==cmin)
      hsv.h = 0.0f;
   else if(cmax==r)
      hsv.h = fmod(((g-b)/diff),6.0f);
   else if(cmax==g)
      hsv.h = (b-r)/diff+2.0f;
   else if(cmax==b)
      hsv.h = (r-g)/diff+4.0f;

   hsv.v = cmax;
   hsv.h*=60.0f;
   hsv.s = diff/hsv.v;

   return hsv;
}

static SLK_Color hsv_to_color(Color_hsv hsv)
{
   SLK_Color rgb;
   float r = 0.0f,g = 0.0f,b = 0.0f;

   while(hsv.h<0.0f)
      hsv.h+=360.0f;
   hsv.h = fmod(hsv.h,360.0f);
   float c = hsv.v*hsv.s;
   float x = c*(1.0f-fabs(fmod(hsv.h/60.0f,2.0f)-1.0f));
   float m = hsv.v-c;

   if(hsv.h>=0.0f&&hsv.h<60.0f)
   {
      r = c+m;
      g = x+m;
      b = m;
   }
   else if(hsv.h>=60.0f&&hsv.h<120.0f)
   {
      r = x+m;
      g = c+m;
      b = m;
   }
   else if(hsv.h>=120.0f&&hsv.h<180.0f)
   {
      r = m;
      g = c+m;
      b = x+m;
   }
   else if(hsv.h>=180.0f&&hsv.h<240.0f)
   {
      r = m;
      g = x+m;
      b = c+m;
   }
   else if(hsv.h>=240.0f&&hsv.h<300.0f)
   {
      r = x+m;
      g = m;
      b = c+m;
   }
   else if(hsv.h>=300.0f&&hsv.h<360.0f)
   {
      r = c+m;
      g = m;
      b = x+m;
   }

   rgb.r = (uint8_t)(r*255.0f);
   rgb.g = (uint8_t)(g*255.0f);
   rgb.b = (uint8_t)(b*255.0f);

   return rgb;
}

static SLK_Color palette_find_closest_rgb(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   int64_t min_dist = INT64_MAX;
   int min_index = 0;

   for(int i = 0;i<pal->used;i++)
   {   
      int64_t dist = rgb_color_dist2(c,palette_rgb[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_cie76(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_lab in = color_to_lab(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.l,in.a,in.b,palette_lab[i].l,palette_lab[i].a,palette_lab[i].b);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_cie94(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_lab in = color_to_lab(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = cie94_color_dist2(in,palette_lab[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_ciede2000(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_lab in = color_to_lab(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = ciede2000_color_dist2(in,palette_lab[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_xyz(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_xyz in = color_to_xyz(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.x,in.y,in.z,palette_xyz[i].x,palette_xyz[i].y,palette_xyz[i].z);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_ycc(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_ycc in = color_to_ycc(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.y,in.cb,in.cr,palette_ycc[i].y,palette_ycc[i].cb,palette_ycc[i].cr);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_yiq(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_yiq in = color_to_yiq(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.y,in.i,in.q,palette_yiq[i].y,palette_yiq[i].i,palette_yiq[i].q);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_yuv(SLK_Palette *pal, SLK_Color c)
{
   if(c.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.r = MAX(0,MIN(0xff,c.r));
   cin.g = MAX(0,MIN(0xff,c.g));
   cin.b = MAX(0,MIN(0xff,c.b));
   Color_yuv in = color_to_yuv(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in.y,in.u,in.v,palette_yuv[i].y,palette_yuv[i].u,palette_yuv[i].v);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static int64_t rgb_color_dist2(SLK_Color c0, SLK_Color c1)
{
   int64_t diff_r = c1.r-c0.r;
   int64_t diff_g = c1.g-c0.g;
   int64_t diff_b = c1.b-c0.b;

   return (diff_r*diff_r+diff_g*diff_g+diff_b*diff_b);
}

static double cie94_color_dist2(Color_lab c0, Color_lab c1)
{
   double L = c0.l-c1.l;
   double C1 = sqrt(c0.a*c0.a+c0.b*c0.b);
   double C2 = sqrt(c1.a*c1.a+c1.b*c1.b);
   double C = C1-C2;
   double H = sqrt((c0.a-c1.a)*(c0.a-c1.a)+(c0.b-c1.b)*(c0.b-c1.b)-C*C);
   double r1 = L;
   double r2 = C/(1.0f+0.045f*C1);
   double r3 = H/(1.0f+0.015f*C1);

   return r1*r1+r2*r2+r3*r3;
}

static double ciede2000_color_dist2(Color_lab c0, Color_lab c1)
{
   double C1 = sqrt(c0.a*c0.a+c0.b*c0.b);
   double C2 = sqrt(c1.a*c1.a+c1.b*c1.b);
   double C_ = (C1+C2)/2.0f;

   double C_p2 = pow(C_,7.0f);
   double v = 0.5f*(1.0f-sqrt(C_p2/(C_p2+6103515625.0f)));
   double a1 = (1.0f+v)*c0.a;
   double a2 = (1.0f+v)*c1.a;

   double Cs1 = sqrt(a1*a1+c0.b*c0.b);
   double Cs2 = sqrt(a2*a2+c1.b*c1.b);

   double h1 = 0.0f;
   if(c0.b!=0||a1!=0)
   {
      h1 = atan2(c0.b,a1);
      if(h1<0)
         h1+=2.0f*M_PI;
   }
   double h2 = 0.0f;
   if(c1.b!=0||a2!=0)
   {
      h2 = atan2(c1.b,a2);
      if(h2<0)
         h2+=2.0f*M_PI;
   }

   double L = c1.l-c0.l;
   double Cs = Cs2-Cs1;
   double h = 0.0f;
   if(Cs1*Cs2!=0.0f)
   {
      h = h2-h1;
      if(h<-M_PI)
         h+=2*M_PI;
      else if(h>M_PI)
         h-=2*M_PI;
   }
   double H = 2.0f*sqrt(Cs1*Cs2)*sin(h/2.0f);

   double L_ = (c0.l+c1.l)/2.0f;
   double Cs_ = (Cs1+Cs2)/2.0f;
   double H_ = h1+h2;
   if(Cs1*Cs2!=0.0f)
   {
      if(fabs(h1-h2)<=M_PI)
         H_ = (h1+h2)/2.0f;
      else if(h1+h2<2*M_PI)
         H_ = (h1+h2+2*M_PI)/2.0f;
      else
         H_ = (h1+h2-2*M_PI)/2.0f;
   }

   double T = 1.0f-0.17f*cos(H_-DEG2RAD(30.0f))+0.24f*cos(2.0f*H_)+0.32f*cos(3.0f*H_+DEG2RAD(6.0f))-0.2f*cos(4.0f*H_-DEG2RAD(63.0f));
   v = DEG2RAD(60.0f)*exp(-1.0f*((H_-DEG2RAD(275.0f))/DEG2RAD(25.0f))*((H_-DEG2RAD(275.0f))/DEG2RAD(25.0f)));
   double Cs_p2 = pow(Cs_,7.0f);
   double RC = 2.0f*sqrt(Cs_p2/(Cs_p2+6103515625.0f));
   double RT = -1.0f*sin(v)*RC;
   double SL = 1.0f+(0.015f*(L_-50.0f)*(L_-50.0f))/sqrt(20.0f+(L_-50.0f)*(L_-50.0f));
   double SC = 1.0f+0.045f*Cs_;
   double SH = 1.0f+0.015f*Cs_*T;

   return (L/SL)*(L/SL)+(Cs/SC)*(Cs/SC)+(H/SH)*(H/SH)+RT*(Cs/SC)*(H_/SH);
}

static double color_dist2(double a0, double a1, double a2, double b0, double b1, double b2)
{
   double diff_0 = b0-a0;
   double diff_1 = b1-a1;
   double diff_2 = b2-a2;

   return diff_0*diff_0+diff_1*diff_1+diff_2*diff_2;
}

//Downsamples an image to the specified dimensions
static void sample_image(const SLK_RGB_sprite *in, SLK_Color *out, int sample_mode, int width, int height)
{
   switch(sample_mode)
   {
   case 0: sample_round(in,out,width,height); break;
   case 1: sample_floor(in,out,width,height); break;
   case 2: sample_ceil(in,out,width,height); break;
   case 3: sample_linear(in,out,width,height); break;
   case 4: sample_bicubic(in,out,width,height); break;
   case 5: sample_supersample(in,out,width,height); break;
   case 6: sample_lanczos(in,out,width,height); break;
   }
}

//Nearest neighbour sampling,
//rounding the position
static void sample_round(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,round((float)x*fw),round((float)y*fh));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

//Nearest neighbour sampling,
//flooring the position
static void sample_floor(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,floor((float)x*fw),floor((float)y*fh));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

//Nearest neighbour sampling,
//ceiling the position
static void sample_ceil(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,ceil((float)x*fw),ceil((float)y*fh));
         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

//Bilinear sampling
static void sample_linear(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)((float)x*fw);
         int iy = (int)((float)y*fh);
         float six = ((float)x*fw)-(float)ix;
         float siy = ((float)y*fh)-(float)iy;

         SLK_Color c;
         SLK_Color c1,c2,c3,c4;
         c1 = SLK_rgb_sprite_get_pixel(in,ix,iy);
         c2 = SLK_rgb_sprite_get_pixel(in,ix+1,iy);
         c3 = SLK_rgb_sprite_get_pixel(in,ix,iy+1);
         c4 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+1);

         //r value
         float c1t = ((1.0f-six)*(float)c1.r+six*(float)c2.r);
         float c2t = ((1.0f-six)*(float)c3.r+six*(float)c4.r);
         c.r = (int)((1.0f-siy)*c1t+siy*c2t);

         //g value
         c1t = ((1.0f-six)*(float)c1.g+six*(float)c2.g);
         c2t = ((1.0f-six)*(float)c3.g+six*(float)c4.g);
         c.g = (int)((1.0f-siy)*c1t+siy*c2t);

         //b value
         c1t = ((1.0f-six)*(float)c1.b+six*(float)c2.b);
         c2t = ((1.0f-six)*(float)c3.b+six*(float)c4.b);
         c.b = (int)((1.0f-siy)*c1t+siy*c2t);

         //a value
         c1t = ((1.0f-six)*(float)c1.a+six*(float)c2.a);
         c2t = ((1.0f-six)*(float)c3.a+six*(float)c4.a);
         c.a = (int)((1.0f-siy)*c1t+siy*c2t);

         out[y*width+x].r = c.r;
         out[y*width+x].b = c.b;
         out[y*width+x].g = c.g;
         out[y*width+x].a = c.a;
      }
   }
}

//Bicubic sampling
static void sample_bicubic(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)((float)x*fw);
         int iy = (int)((float)y*fh);
         float six = (x*fw)-(float)ix;
         float siy = (y*fh)-(float)iy;

         SLK_Color c00,c10,c20,c30;
         SLK_Color c01,c11,c21,c31;
         SLK_Color c02,c12,c22,c32;
         SLK_Color c03,c13,c23,c33;

         c00 = SLK_rgb_sprite_get_pixel(in,ix-1,iy-1);
         c10 = SLK_rgb_sprite_get_pixel(in,ix,iy-1);
         c20 = SLK_rgb_sprite_get_pixel(in,ix+1,iy-1);
         c30 = SLK_rgb_sprite_get_pixel(in,ix+2,iy-1);

         c01 = SLK_rgb_sprite_get_pixel(in,ix-1,iy);
         c11 = SLK_rgb_sprite_get_pixel(in,ix,iy);
         c21 = SLK_rgb_sprite_get_pixel(in,ix+1,iy);
         c31 = SLK_rgb_sprite_get_pixel(in,ix+2,iy);

         c02 = SLK_rgb_sprite_get_pixel(in,ix-1,iy+1);
         c12 = SLK_rgb_sprite_get_pixel(in,ix,iy+1);
         c22 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+1);
         c32 = SLK_rgb_sprite_get_pixel(in,ix+2,iy+1);

         c03 = SLK_rgb_sprite_get_pixel(in,ix-1,iy+2);
         c13 = SLK_rgb_sprite_get_pixel(in,ix,iy+2);
         c23 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+2);
         c33 = SLK_rgb_sprite_get_pixel(in,ix+2,iy+2);

         //r value
         float c0 = cubic_hermite((float)c00.r,(float)c10.r,(float)c20.r,(float)c30.r,six);
         float c1 = cubic_hermite((float)c01.r,(float)c11.r,(float)c21.r,(float)c31.r,six);
         float c2 = cubic_hermite((float)c02.r,(float)c12.r,(float)c22.r,(float)c32.r,six);
         float c3 = cubic_hermite((float)c03.r,(float)c13.r,(float)c23.r,(float)c33.r,six);
         float val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].r = MAX(0,MIN(0xff,(int)val));

         //g value
         c0 = cubic_hermite((float)c00.g,(float)c10.g,(float)c20.g,(float)c30.g,six);
         c1 = cubic_hermite((float)c01.g,(float)c11.g,(float)c21.g,(float)c31.g,six);
         c2 = cubic_hermite((float)c02.g,(float)c12.g,(float)c22.g,(float)c32.g,six);
         c3 = cubic_hermite((float)c03.g,(float)c13.g,(float)c23.g,(float)c33.g,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].g = MAX(0,MIN(0xff,(int)val));

         //b value
         c0 = cubic_hermite((float)c00.b,(float)c10.b,(float)c20.b,(float)c30.b,six);
         c1 = cubic_hermite((float)c01.b,(float)c11.b,(float)c21.b,(float)c31.b,six);
         c2 = cubic_hermite((float)c02.b,(float)c12.b,(float)c22.b,(float)c32.b,six);
         c3 = cubic_hermite((float)c03.b,(float)c13.b,(float)c23.b,(float)c33.b,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].b = MAX(0,MIN(0xff,(int)val));

         //a value
         c0 = cubic_hermite((float)c00.a,(float)c10.a,(float)c20.a,(float)c30.a,six);
         c1 = cubic_hermite((float)c01.a,(float)c11.a,(float)c21.a,(float)c31.a,six);
         c2 = cubic_hermite((float)c02.a,(float)c12.a,(float)c22.a,(float)c32.a,six);
         c3 = cubic_hermite((float)c03.a,(float)c13.a,(float)c23.a,(float)c33.a,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].a = MAX(0,MIN(0xff,(int)val));
      }
   }
}

//Helper function for sample_bicubic
static float cubic_hermite (float a, float b, float c, float d, float t)
{
   float a_ = -a/2.0f+(3.0f*b)/2.0f-(3.0f*c)/2.0f+d/2.0f;   
   float b_ = a-(5.0f*b)/2.0f+2.0f*c-d/2.0f; 
   float c_ = -a/2.0f+c/2.0f;
   float d_ = b;

   return a_*t*t*t+b_*t*t+c_*t+d_;
}

//Supersampling --> works best without gaussian blur
static void sample_supersample(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color p = {0};
         float n = 0.0f;
         float r = 0.0f;
         float g = 0.0f;
         float b = 0.0f;
         float a = 0.0f;

         for(int sy = (int)((float)y*fh);sy<(int)ceil((float)(y+1)*fh);sy++)
         {
            float wy = 1.0f;
            if(sy<(int)((float)y*fh))
               wy = (float)sy-(float)y*fh+1.0f;
            else if((float)sy+1.0f>(float)(y+1.0f)*fh)
               wy = (float)(y+1.0f)*fh-(float)sy;

            for(int sx = (int)((float)x*fw);sx<(int)ceil((float)(x+1)*fw);sx++)
            {
               float wx = 1.0f;
               if(sx<(int)((float)x*fw))
                  wx = (float)sx-(float)x*fw+1.0f;
               else if((float)sx+1.0f>(float)(x+1.0f)*fw)
                  wx = (float)(x+1.0f)*fw-(float)sx;
               
               SLK_Color c = SLK_rgb_sprite_get_pixel(in,sx,sy);
               n+=wx*wy;
               r+=wx*wy*(float)c.r;
               g+=wx*wy*(float)c.g;
               b+=wx*wy*(float)c.b;
               a+=wx*wy*(float)c.a;
            }
         }

         p.r = r/n;
         p.g = g/n;
         p.b = b/n;
         p.a = a/n;
         out[y*width+x] = p;
      }
   }
}

//Lanczos downsampling
//The only one to use doubles 
//for best possible result
static void sample_lanczos(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   double fw = (double)(in->width-1)/(double)width;
   double fh = (double)(in->height-1)/(double)height;

   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)((double)x*fw);
         int iy = (int)((double)y*fh);
         double sx = ((double)x*fw)-(double)ix;
         double sy = ((double)y*fh)-(double)iy;
         SLK_Color p = {0};

         double a0 = lanczos(sx+2.0f);
         double a1 = lanczos(sx+1.0f);
         double a2 = lanczos(sx);
         double a3 = lanczos(sx-1.0f);
         double a4 = lanczos(sx-2.0f);
         double a5 = lanczos(sx-3.0f);
         double b0 = lanczos(sy+2.0f);
         double b1 = lanczos(sy+1.0f);
         double b2 = lanczos(sy);
         double b3 = lanczos(sy-1.0f);
         double b4 = lanczos(sy-2.0f);
         double b5 = lanczos(sy-3.0f);

         double r[6];
         double g[6];
         double b[6];
         double a[6];
         for(int i = 0;i<6;i++)
         {
            SLK_Color p0 = SLK_rgb_sprite_get_pixel(in,ix-2,iy-2+i);
            SLK_Color p1 = SLK_rgb_sprite_get_pixel(in,ix-1,iy-2+i);
            SLK_Color p2 = SLK_rgb_sprite_get_pixel(in,ix,iy-2+i);
            SLK_Color p3 = SLK_rgb_sprite_get_pixel(in,ix+1,iy-2+i);
            SLK_Color p4 = SLK_rgb_sprite_get_pixel(in,ix+2,iy-2+i);
            SLK_Color p5 = SLK_rgb_sprite_get_pixel(in,ix+3,iy-2+i);

            r[i] = a0*(double)p0.r+a1*(double)p1.r+a2*(double)p2.r+a3*(double)p3.r+a4*(double)p4.r+a5*(double)p5.r;
            g[i] = a0*(double)p0.g+a1*(double)p1.g+a2*(double)p2.g+a3*(double)p3.g+a4*(double)p4.g+a5*(double)p5.g;
            b[i] = a0*(double)p0.b+a1*(double)p1.b+a2*(double)p2.b+a3*(double)p3.b+a4*(double)p4.b+a5*(double)p5.b;
            a[i] = a0*(double)p0.a+a1*(double)p1.a+a2*(double)p2.a+a3*(double)p3.a+a4*(double)p4.a+a5*(double)p5.a;
         }

         p.r = MAX(0,MIN(0xff,(int)(b0*r[0]+b1*r[1]+b2*r[2]+b3*r[3]+b4*r[4]+b5*r[5])));
         p.g = MAX(0,MIN(0xff,(int)(b0*g[0]+b1*g[1]+b2*g[2]+b3*g[3]+b4*g[4]+b5*g[5])));
         p.b = MAX(0,MIN(0xff,(int)(b0*b[0]+b1*b[1]+b2*b[2]+b3*b[3]+b4*b[4]+b5*b[5])));
         p.a = MAX(0,MIN(0xff,(int)(b0*a[0]+b1*a[1]+b2*a[2]+b3*a[3]+b4*a[4]+b5*a[5])));
         out[y*width+x] = p;
      }
   }
}

//Helper function for sample_lanczos
static double lanczos(double v)
{
   if(v==0.0f)
      return 1.0f;
   if(v>3.0f)
      return 0.0f;
   if(v<-3.0f)
      return 0.0f;

   return ((3.0f*sin(M_PI*v)*sin(M_PI*v/3.0f))/(M_PI*M_PI*v*v));
}

static void quant_cluster_list_init()
{
   quant_cluster_list_free(quant_k);
   
   quant_cluster_list = malloc(sizeof(*quant_cluster_list)*quant_k);
   for(int i = 0;i<quant_k;i++)
      dyn_array_init(SLK_Color,&quant_cluster_list[i],2);
}

static void quant_cluster_list_free()
{
   if(quant_cluster_list==NULL)
      return;

   for(int i = 0;i<quant_k;i++)
      dyn_array_free(SLK_Color,&quant_cluster_list[i]);

   free(quant_cluster_list);
   quant_cluster_list = NULL;
}

static void quant_compute_kmeans(SLK_RGB_sprite *data)
{
   quant_cluster_list_init();
   quant_centroid_list = malloc(sizeof(*quant_centroid_list)*quant_k);
   quant_assignment = malloc(sizeof(*quant_assignment)*(data->width*data->height));
   for(int i = 0;i<(data->width*data->height);i++)
      quant_assignment[i] = 0;

   int iter = 0;
   int max_iter = 16;
   float *previous_variance = malloc(sizeof(*previous_variance)*quant_k);
   float variance = 0.0f;
   float delta = 0.0f;
   float delta_max = 0.0f;
   float threshold = 0.00005f;
   for(int i = 0;i<quant_k;i++)
      previous_variance[i] = 1.0f;

   for(;;)
   {
      quant_get_cluster_centroid(data);
      quant_cluster_list_init();
      for(int i = 0;i<data->width*data->height;i++)
      {
         SLK_Color color = data->data[i];
         quant_assignment[i] = quant_nearest_color_idx(color,quant_centroid_list);
         dyn_array_add(SLK_Color,&quant_cluster_list[quant_assignment[i]],1,color);
      }

      delta_max = 0.0f;
      for(int i = 0;i<quant_k;i++)
      {
         variance = quant_colors_variance(&quant_cluster_list[i]);
         delta = fabs(previous_variance[i]-variance);
         delta_max = MAX(delta,delta_max);
         previous_variance[i] = variance;
      }

      if(delta_max<threshold||iter++>max_iter)
         break;
   }

   free(previous_variance);
}

static void quant_get_cluster_centroid(SLK_RGB_sprite *data)
{
   for(int i = 0;i<quant_k;i++)
   {
      if(quant_cluster_list[i].used>0)
         quant_centroid_list[i] = quant_colors_mean(&quant_cluster_list[i]);
      else
         quant_centroid_list[i] = quant_pick_random_color(data);
   }
}

static SLK_Color quant_colors_mean(dyn_array *color_list)
{
   int r = 0,g = 0,b = 0;
   int length = color_list->used;
   for(int i = 0;i<length;i++)
   {
      r+=dyn_array_element(SLK_Color,color_list,i).r;
      g+=dyn_array_element(SLK_Color,color_list,i).g;
      b+=dyn_array_element(SLK_Color,color_list,i).b;
   }

   r/=length;
   g/=length;
   b/=length;

   return (SLK_Color){.r = r, .g = g, .b = b};
}

static SLK_Color quant_pick_random_color(SLK_RGB_sprite *data)
{
   return data->data[(int)(((float)rand()/(float)RAND_MAX)*data->width*data->height)];
}

static int quant_nearest_color_idx(SLK_Color color, SLK_Color *color_list)
{
   float dist_min = 0xffff;
   float dist = 0.0f;
   int idx = 0;
   for(int i = 0;i<quant_k;i++)
   {
      dist = quant_distancef(color,color_list[i]);
      if(dist<dist_min)
      {
         dist_min = dist;
         idx = i;
      }
   }

   return idx;
}

static float quant_distance(SLK_Color color0, SLK_Color color1)
{
   float mr = 0.5f*(color0.r+color1.r),
      dr = color0.r-color1.r,
      dg = color0.g-color1.g,
      db = color0.b-color1.b;
   float distance = (2*dr*dr)+(4*dg*dg)+(3*db*db)+(mr*((dr*dr)-(db*db))/256.0f);
   return sqrt(distance)/(3.0f*255.0f);
}

static float quant_distancef(SLK_Color color0, SLK_Color color1)
{
   float mr = 0.5f*(color0.r+color1.r),
      dr = color0.r-color1.r,
      dg = color0.g-color1.g,
      db = color0.b-color1.b;
   float distance = (2*dr*dr)+(4*dg*dg)+(3*db*db)+(mr*((dr*dr)-(db*db))/256.0f);
   return distance/(3.0f*255.0f);
}

static float quant_colors_variance(dyn_array *color_list)
{
   int length = color_list->used;
   SLK_Color mean = quant_colors_mean(color_list);
   float dist = 0.0f;
   float dist_sum = 0.0f;
   for(int i = 0;i<length;i++)
   {
      dist = quant_distance(dyn_array_element(SLK_Color,color_list,i),mean);
      dist_sum+=dist*dist;
   }

   return dist_sum/(float)length;
}
//-------------------------------------
