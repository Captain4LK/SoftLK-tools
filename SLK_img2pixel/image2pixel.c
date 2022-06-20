/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//Quantization algorithm based on: https://github.com/ogus/kmeans-quantizer (wtfpl)

//External includes
#include <math.h>
#include "../headers/libSLK.h"

#define HLH_JSON_IMPLEMENTATION
#include "../external/HLH_json.h"
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
   double c0;
   double c1;
   double c2;
}Color_d3;
//-------------------------------------

//Variables
static int brightness = 0;
static int contrast = 0;
static int img_gamma = 100;
static int saturation = 100;
static int dither_amount = 64;
static int alpha_threshold = 128;
static int sharpen = 0;
static int hue = 0;
static int gauss = 80;
static int offset_x = 0;
static int offset_y = 0;
static int image_outline = -1;
static int image_inline = -1;
static int pixel_scale_mode = 0;
static int pixel_sample_mode = 0;
static int pixel_process_mode = 1;
static int pixel_distance_mode = 0;
static int image_out_width = 128;
static int image_out_height = 128;
static int image_out_swidth = 2;
static int image_out_sheight = 2;
static int palette_weight = 2;
static SLK_Palette *palette = NULL;

static dyn_array *quant_cluster_list = NULL;
static SLK_Color *quant_centroid_list = NULL;
static int *quant_assignment = NULL;
static int quant_k = 16;

static const float dither_threshold_bayer8x8[64] = 
{
    0.0f/64.0f,32.0f/64.0f, 8.0f/64.0f,40.0f/64.0f, 2.0f/64.0f,34.0f/64.0f,10.0f/64.0f,42.0f/64.0f,
   48.0f/64.0f,16.0f/64.0f,56.0f/64.0f,24.0f/64.0f,50.0f/64.0f,18.0f/64.0f,58.0f/64.0f,26.0f/64.0f,
   12.0f/64.0f,44.0f/64.0f, 4.0f/64.0f,36.0f/64.0f,14.0f/64.0f,46.0f/64.0f, 6.0f/64.0f,38.0f/64.0f,
   60.0f/64.0f,28.0f/64.0f,52.0f/64.0f,20.0f/64.0f,62.0f/64.0f,30.0f/64.0f,54.0f/64.0f,22.0f/64.0f,
    3.0f/64.0f,35.0f/64.0f,11.0f/64.0f,43.0f/64.0f, 1.0f/64.0f,33.0f/64.0f, 9.0f/64.0f,41.0f/64.0f,
   51.0f/64.0f,19.0f/64.0f,59.0f/64.0f,27.0f/64.0f,49.0f/64.0f,17.0f/64.0f,57.0f/64.0f,25.0f/64.0f,
   15.0f/64.0f,47.0f/64.0f, 7.0f/64.0f,39.0f/64.0f,13.0f/64.0f,45.0f/64.0f, 5.0f/64.0f,37.0f/64.0f,
   63.0f/64.0f,31.0f/64.0f,55.0f/64.0f,23.0f/64.0f,61.0f/64.0f,29.0f/64.0f,53.0f/64.0f,21.0f/64.0f,
};
static const float dither_threshold_bayer4x4[16] = 
{
   0.0f/16.0f,8.0f/16.0f,2.0f/16.0f,10.0f/16.0f,
   12.0f/16.0f,4.0f/16.0f,14.0f/16.0f,6.0f/16.0f,
   3.0f/16.0f,11.0f/16.0f,1.0f/16.0f,9.0f/16.0f,
   15.0f/16.0f,7.0f/16.0f,13.0f/16.0f,5.0f/16.0f,
};
static const float dither_threshold_bayer2x2[4] = 
{
   0.0f/4.0f,2.0f/4.0f,
   3.0f/4.0f,1.0f/4.0f
};
static const float dither_threshold_cluster8x8[64] = 
{
   24.0f/64.0f,10.0f/64.0f,12.0f/64.0f,26.0f/64.0f,35.0f/64.0f,47.0f/64.0f,49.0f/64.0f,37.0f/64.0f,
   8.0f/64.0f,0.0f/64.0f,2.0f/64.0f,14.0f/64.0f,45.0f/64.0f,59.0f/64.0f,61.0f/64.0f,51.0f/64.0f,
   22.0f/64.0f,6.0f/64.0f,4.0f/64.0f,16.0f/64.0f,43.0f/64.0f,57.0f/64.0f,63.0f/64.0f,53.0f/64.0f,
   30.0f/64.0f,20.0f/64.0f,18.0f/64.0f,28.0f/64.0f,33.0f/64.0f,41.0f/64.0f,55.0f/64.0f,39.0f/64.0f,
   34.0f/64.0f,46.0f/64.0f,48.0f/64.0f,36.0f/64.0f,25.0f/64.0f,11.0f/64.0f,13.0f/64.0f,27.0f/64.0f,
   44.0f/64.0f,58.0f/64.0f,60.0f/64.0f,50.0f/64.0f,9.0f/64.0f,1.0f/64.0f,3.0f/64.0f,15.0f/64.0f,
   42.0f/64.0f,56.0f/64.0f,62.0f/64.0f,52.0f/64.0f,23.0f/64.0f,7.0f/64.0f,5.0f/64.0f,17.0f/64.0f,
   32.0f/64.0f,40.0f/64.0f,54.0f/64.0f,38.0f/64.0f,31.0f/64.0f,21.0f/64.0f,19.0f/64.0f,29.0f/64.0f,
};
static const float dither_threshold_cluster4x4[16] = 
{
   12.0f/16.0f,5.0f/16.0f,6.0f/16.0f,13.0f/16.0f,
   4.0f/16.0f,0.0f/16.0f,1.0f/16.0f,7.0f/16.0f,
   11.0f/16.0f,3.0f/16.0f,2.0f/16.0f,8.0f/16.0f,
   15.0f/16.0f,10.0f/16.0f,9.0f/16.0f,14.0f/16.0f,
};
//-------------------------------------

//Function prototypes

static void dither_image          (SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, int process_mode, int distance_mode);
static void dither_threshold      (SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, Color_d3 *pal_d3, int distance_mode, const float *threshold, uint8_t dim);
static void dither_threshold_apply(SLK_Color *in, SLK_Color *out, int width, int height, const float *threshold, uint8_t dim);
static void dither_none           (SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, Color_d3 *pal_d3, int distance_mode);
static void dither_none_apply     (SLK_Color *in, SLK_Color *out, int width, int height);
static void dither_floyd          (SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, Color_d3 *pal_d3, int distance_mode);
static void dither_floyd2         (SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, Color_d3 *pal_d3, int distance_mode);
static void floyd_apply_error     (SLK_Color *d, double error_r, double error_g, double error_b, int x, int y, int width, int height);

static double gauss_calc(double x, double y, double sigma);
static SLK_Color kernel_data_get(int x, int y, int width, int height, const SLK_RGB_sprite *data);

static Color_d3  color_to_rgb(SLK_Color c);
static Color_d3  color_to_lab(SLK_Color c);
static Color_d3  color_to_xyz(SLK_Color c);
static Color_d3  color_to_ycc(SLK_Color c);
static Color_d3  color_to_yiq(SLK_Color c);
static Color_d3  color_to_yuv(SLK_Color c);
static Color_d3  color_to_hsv(SLK_Color c);
static SLK_Color hsv_to_color(Color_d3 hsv);

static SLK_Color palette_find_closest(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c, int distance_mode);
static SLK_Color palette_find_closest_rgb      (SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c);
static SLK_Color palette_find_closest_cie76    (SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c);
static SLK_Color palette_find_closest_cie94    (SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c);
static SLK_Color palette_find_closest_ciede2000(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c);
static SLK_Color palette_find_closest_xyz      (SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c);
static SLK_Color palette_find_closest_ycc      (SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c);
static SLK_Color palette_find_closest_yiq      (SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c);
static SLK_Color palette_find_closest_yuv      (SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c);

static double cie94_color_dist2(Color_d3 c0, Color_d3 c1);
static double ciede2000_color_dist2(Color_d3 c0, Color_d3 c1);
static double color_dist2(Color_d3 a, Color_d3 b);

//Functions needed for downsampling image
static void sample_image(const SLK_RGB_sprite *in, SLK_Color *out, int sample_mode, int width, int height);
static void sample_round(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_floor(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_ceil(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_linear(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static void sample_bicubic(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static float cubic_hermite (float a, float b, float c, float d, float t);
static void sample_lanczos(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height);
static double lanczos(double v);

//Functions needed for color quantization
static void quant_cluster_list_init();
static void quant_cluster_list_free();
static void quant_compute_kmeans(SLK_RGB_sprite *data, int pal_in);
static void quant_get_cluster_centroid(SLK_RGB_sprite *data, int pal_in, int weight_pal);
static SLK_Color quant_colors_mean(dyn_array *color_list,SLK_Color color, int weight_color);
static SLK_Color quant_pick_random_color(SLK_RGB_sprite *data);
static int quant_nearest_color_idx(SLK_Color color, SLK_Color *color_list);
static double quant_distance(SLK_Color color0, SLK_Color color1);
static double quant_colors_variance(dyn_array *color_list);

//Post processing
static void post_process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out);
//-------------------------------------

//Function implementations

//loads the palette and all configuration values
//from an external json file.
void img2pixel_preset_load(FILE *f)
{
   if(f==NULL)
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
   dither_amount = HLH_json_get_object_integer(&root->root,"dither_amount",64);
   pixel_sample_mode = HLH_json_get_object_integer(&root->root,"sample_mode",0);
   alpha_threshold = HLH_json_get_object_integer(&root->root,"alpha_threshold",128);
   upscale = HLH_json_get_object_integer(&root->root,"upscale",1);
   gauss = HLH_json_get_object_integer(&root->root,"gaussian_blur",128);
   offset_x = HLH_json_get_object_integer(&root->root,"offset_x",0);
   offset_y = HLH_json_get_object_integer(&root->root,"offset_y",0);
   image_outline = HLH_json_get_object_integer(&root->root,"outline",-1);
   image_inline = HLH_json_get_object_integer(&root->root,"inline",-1);
   brightness = HLH_json_get_object_integer(&root->root,"brightness",0);
   contrast = HLH_json_get_object_integer(&root->root,"contrast",0);
   saturation = HLH_json_get_object_integer(&root->root,"saturation",0);
   img_gamma = HLH_json_get_object_integer(&root->root,"gamma",0);
   sharpen = HLH_json_get_object_integer(&root->root,"sharpness",0);
   hue = HLH_json_get_object_integer(&root->root,"hue",0);

   HLH_json_free(root);
}

//Saves the current configuration and palette to a
//json file.
void img2pixel_preset_save(FILE *f)
{
   if(f==NULL)
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
   HLH_json_object_add_integer(&root->root,"offset_x",offset_x);
   HLH_json_object_add_integer(&root->root,"offset_y",offset_y);
   HLH_json_object_add_integer(&root->root,"outline",image_outline);
   HLH_json_object_add_integer(&root->root,"inline",image_inline);
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
}

//Sharpens an image, input and output dimensions must be equal
void img2pixel_sharpen_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   if(in==NULL||out==NULL||in->width!=out->width||in->height!=out->height)
      return;

   if(sharpen==0)
   {
      SLK_rgb_sprite_copy(out,in);
      return;
   }

   SLK_RGB_sprite *tmp_data2 = SLK_rgb_sprite_create(out->width,out->height);
   if(tmp_data2==NULL)
      return;
   
   SLK_rgb_sprite_copy(tmp_data2,in);

   //Setup sharpening kernel
   const float sharpen_factor = (float)sharpen/100.0f;
   const float sharpen_kernel[3][3] =
   {
      {0.0f,-1.0f*sharpen_factor,0.0f},
      {-1.0f*sharpen_factor,4.0f*sharpen_factor+1.0f,-1.0f*sharpen_factor},
      {0.0f,-1.0f*sharpen_factor,0.0f},
   };

   #pragma omp parallel for schedule(dynamic, 1)
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

               r+=sharpen_kernel[yk+1][xk+1]*(float)c.rgb.r;
               g+=sharpen_kernel[yk+1][xk+1]*(float)c.rgb.g;
               b+=sharpen_kernel[yk+1][xk+1]*(float)c.rgb.b;
            }
         }

         out->data[y*out->width+x].rgb.r = MAX(0x0,MIN(0xff,(int)r));
         out->data[y*out->width+x].rgb.g = MAX(0x0,MIN(0xff,(int)g));
         out->data[y*out->width+x].rgb.b = MAX(0x0,MIN(0xff,(int)b));
         out->data[y*out->width+x].rgb.a = in->data[y*out->width+x].rgb.a;
      }
   }

   //Cleanup
   SLK_rgb_sprite_destroy(tmp_data2);
}

void img2pixel_lowpass_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   if(in==NULL||out==NULL||in->width!=out->width||in->height!=out->height)
      return;

   if(gauss==0)
   {
      SLK_rgb_sprite_copy(out,in);
      return;
   }

   SLK_RGB_sprite *tmp_data2 = SLK_rgb_sprite_create(out->width,out->height);
   if(tmp_data2==NULL)
      return;
   
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

   #pragma omp parallel for schedule(dynamic, 1)
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

               r+=lowpass_kernel[xk+3][yk+3]*(double)c.rgb.r;
               g+=lowpass_kernel[xk+3][yk+3]*(double)c.rgb.g;
               b+=lowpass_kernel[xk+3][yk+3]*(double)c.rgb.b;
            }
         }

         out->data[y*out->width+x].rgb.r = MAX(0x0,MIN(0xff,(int)r));
         out->data[y*out->width+x].rgb.g = MAX(0x0,MIN(0xff,(int)g));
         out->data[y*out->width+x].rgb.b = MAX(0x0,MIN(0xff,(int)b));
         out->data[y*out->width+x].rgb.a = in->data[y*out->width+x].rgb.a;
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
   quant_compute_kmeans(tmp,0);
   palette->used = colors;
   for(int i = 0;i<colors;i++)
   {
      palette->colors[i] = quant_centroid_list[i];
      palette->colors[i].rgb.a = 255;
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

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<out->height;y++)
   {
      for(int x = 0;x<out->width;x++)
      {
         SLK_Color in = tmp_data[y*out->width+x];
         uint8_t a = in.rgb.a;

         //Hue
         //Only ajust if not the default value --> better performance
         if(hue!=0)
         {
            float huef = (float)hue;
            Color_d3 hsv = color_to_hsv(in);
            hsv.c0+=huef;
            in = hsv_to_color(hsv);
         }

         //Saturation, brightness and contrast
         float r = (float)in.rgb.r;
         float g = (float)in.rgb.g;
         float b = (float)in.rgb.b;
         in.rgb.r = MAX(0x0,MIN(0xff,(int)(rr*r)+(gr*g)+(br*b)+wr));
         in.rgb.g = MAX(0x0,MIN(0xff,(int)(rg*r)+(gg*g)+(bg*b)+wg));
         in.rgb.b = MAX(0x0,MIN(0xff,(int)(rb*r)+(gb*g)+(bb*b)+wb));

         //Gamma
         //Only ajust if not the default value --> better performance
         if(img_gamma!=100)
         {
            in.rgb.r = MAX(0x0,MIN(0xff,(int)(255.0f*pow((float)in.rgb.r/255.0f,gamma_factor))));
            in.rgb.g = MAX(0x0,MIN(0xff,(int)(255.0f*pow((float)in.rgb.g/255.0f,gamma_factor))));
            in.rgb.b = MAX(0x0,MIN(0xff,(int)(255.0f*pow((float)in.rgb.b/255.0f,gamma_factor))));
         }

         in.rgb.a = a;
         tmp_data[y*out->width+x] = in;
      }
   }

   //Dithering is done after all image processing
   //If it was done at any other time, it would
   //resoult in different colors than the palette
   dither_image(tmp_data,out->data,out->width,out->height,palette,pixel_process_mode,pixel_distance_mode);

   //Clean up
   free(tmp_data);

   //Post process
   post_process_image(out,out);
}

void img2pixel_reset_to_defaults()
{
   brightness = 0;
   contrast = 0;
   img_gamma = 100;
   saturation = 100;
   dither_amount = 64;
   alpha_threshold = 128;
   sharpen = 0;
   hue = 0;
   gauss = 80;
   offset_x = 0;
   offset_y = 0;
   image_outline = -1;
   image_inline = -1;
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

void img2pixel_set_offset_x(int noffset)
{
   offset_x = noffset;
}

int img2pixel_get_offset_x()
{
   return offset_x;
}

void img2pixel_set_offset_y(int noffset)
{
   offset_y = noffset;
}

int img2pixel_get_offset_y()
{
   return offset_y;
}

int img2pixel_get_outline()
{
   return image_outline;
}

void img2pixel_set_outline(int nline)
{
   image_outline = nline;
}

int img2pixel_get_inline()
{
   return image_inline;
}

void img2pixel_set_inline(int nline)
{
   image_inline = nline;
}

SLK_Palette *img2pixel_get_palette()
{
   return palette;
}

void img2pixel_set_palette(SLK_Palette *npalette)
{
   palette = npalette;
}

int img2pixel_get_palette_weight()
{
   return palette_weight;
}

void img2pixel_set_palette_weight(int weight)
{
   palette_weight = weight;
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
static void dither_image(SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, int process_mode, int distance_mode)
{
   if(distance_mode==8)
   {
      switch(process_mode)
      {
      case 1: //Bayer 8x8
         dither_threshold_apply(in,out,width,height,dither_threshold_bayer8x8,3);
         break;
      case 2: //Bayer 4x4
         dither_threshold_apply(in,out,width,height,dither_threshold_bayer4x4,2);
         break;
      case 3: //Bayer 2x2
         dither_threshold_apply(in,out,width,height,dither_threshold_bayer2x2,1);
         break;
      case 4: //Cluster 8x8
         dither_threshold_apply(in,out,width,height,dither_threshold_cluster8x8,3);
         break;
      case 5: //Cluster4x4
         dither_threshold_apply(in,out,width,height,dither_threshold_cluster4x4,2);
         break;
      case 0:
      case 6:
      case 7:
         dither_none_apply(in,out,width,height);
         break;
      }

      quant_k = palette->used;

      SLK_RGB_sprite tmp;
      tmp.width = width;
      tmp.height = height;
      tmp.data = out;
      quant_compute_kmeans(&tmp,1);

      for(int i = 0;i<width*height;i++)
      {
         if(out[i].rgb.a==0)
            out[i] = SLK_color_create(0,0,0,0);
         else
            out[i] = palette->colors[quant_assignment[i]];
      }

      quant_cluster_list_free();
      free(quant_centroid_list);
      free(quant_assignment);

      return;
   }

   //Convert palette to needed format
   //TODO: cache?
   Color_d3 palette_d3[256];
   switch(distance_mode)
   {
   case 0: //RGB
      for(int i = 0;i<pal->used;i++)
         palette_d3[i] = color_to_rgb(pal->colors[i]); 
      break;
   case 1:
   case 2:
   case 3: //Lab
      for(int i = 0;i<pal->used;i++)
         palette_d3[i] = color_to_lab(pal->colors[i]);
      break;
   case 4: //XYZ
      for(int i = 0;i<pal->used;i++)
         palette_d3[i] = color_to_xyz(pal->colors[i]);
      break;
   case 5: //YCC
      for(int i = 0;i<pal->used;i++)
         palette_d3[i] = color_to_ycc(pal->colors[i]);
      break;
   case 6: //YIQ
      for(int i = 0;i<pal->used;i++)
         palette_d3[i] = color_to_yiq(pal->colors[i]);
      break;
   case 7: //YUV
      for(int i = 0;i<pal->used;i++)
         palette_d3[i] = color_to_yuv(pal->colors[i]);
      break;
   }

   switch(process_mode)
   {
   case 0: //No dithering
      dither_none(in,out,width,height,pal,palette_d3,distance_mode);
      break;
   case 1: //Bayer 8x8
      dither_threshold(in,out,width,height,pal,palette_d3,distance_mode,dither_threshold_bayer8x8,3);
      break;
   case 2: //Bayer 4x4
      dither_threshold(in,out,width,height,pal,palette_d3,distance_mode,dither_threshold_bayer4x4,2);
      break;
   case 3: //Bayer 2x2
      dither_threshold(in,out,width,height,pal,palette_d3,distance_mode,dither_threshold_bayer2x2,1);
      break;
   case 4: //Cluster 8x8
      dither_threshold(in,out,width,height,pal,palette_d3,distance_mode,dither_threshold_cluster8x8,3);
      break;
   case 5: //Cluster4x4
      dither_threshold(in,out,width,height,pal,palette_d3,distance_mode,dither_threshold_cluster4x4,2);
      break;
   case 6: //Floyd-Steinberg dithering (per color component error)
      dither_floyd(in,out,width,height,pal,palette_d3,distance_mode);
      break;
   case 7: //Floyd-Steinberg dithering (distributed error)
      dither_floyd2(in,out,width,height,pal,palette_d3,distance_mode);
      break;
   }
}

static void dither_none(SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, Color_d3 *pal_d3, int distance_mode)
{
   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      { 
         SLK_Color cin = in[y*width+x];
         if(cin.rgb.a<alpha_threshold)
         {
            out[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }

         //Add a value to the color depending on the position,
         //this creates the dithering effect
         SLK_Color c;
         c.rgb.r = cin.rgb.r;
         c.rgb.g = cin.rgb.g;
         c.rgb.b = cin.rgb.b;
         c.rgb.a = cin.rgb.a;
         out[y*width+x] = palette_find_closest(pal,pal_d3,c,distance_mode);
         out[y*width+x].rgb.a = 255;
      }
   }
}

static void dither_none_apply(SLK_Color *in, SLK_Color *out, int width, int height)
{
   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      { 
         SLK_Color cin = in[y*width+x];
         if(cin.rgb.a<alpha_threshold)
         {
            out[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }

         //Add a value to the color depending on the position,
         //this creates the dithering effect
         SLK_Color c;
         c.rgb.r = cin.rgb.r;
         c.rgb.g = cin.rgb.g;
         c.rgb.b = cin.rgb.b;
         c.rgb.a = cin.rgb.a;
         out[y*width+x] = c;
         out[y*width+x].rgb.a = 255;
      }
   }
}

static void dither_threshold(SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, Color_d3 *pal_d3, int distance_mode, const float *threshold, uint8_t dim)
{
   float amount = (float)dither_amount/1000.0f;

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      { 
         SLK_Color cin = in[y*width+x];
         if(cin.rgb.a<alpha_threshold)
         {
            out[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }

         //Add a value to the color depending on the position,
         //this creates the dithering effect
         uint8_t mod = (1<<dim)-1;
         uint8_t tresshold_id = ((y&mod)<<dim)+(x&mod);
         SLK_Color c;
         c.rgb.r = MAX(0x0,MIN(0xff,(int)((float)cin.rgb.r+255.0f*amount*(threshold[tresshold_id]-0.5f))));
         c.rgb.g = MAX(0x0,MIN(0xff,(int)((float)cin.rgb.g+255.0f*amount*(threshold[tresshold_id]-0.5f))));
         c.rgb.b = MAX(0x0,MIN(0xff,(int)((float)cin.rgb.b+255.0f*amount*(threshold[tresshold_id]-0.5f))));
         c.rgb.a = cin.rgb.a;
         out[y*width+x] = palette_find_closest(pal,pal_d3,c,distance_mode);
         out[y*width+x].rgb.a = 255;
      }
   }
}

static void dither_threshold_apply(SLK_Color *in, SLK_Color *out, int width, int height, const float *threshold, uint8_t dim)
{
   float amount = (float)dither_amount/1000.0f;

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      { 
         SLK_Color cin = in[y*width+x];
         if(cin.rgb.a<alpha_threshold)
         {
            out[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }

         //Add a value to the color depending on the position,
         //this creates the dithering effect
         uint8_t mod = (1<<dim)-1;
         uint8_t tresshold_id = ((y&mod)<<dim)+(x&mod);
         SLK_Color c;
         c.rgb.r = MAX(0x0,MIN(0xff,(int)((float)cin.rgb.r+255.0f*amount*(threshold[tresshold_id]-0.5f))));
         c.rgb.g = MAX(0x0,MIN(0xff,(int)((float)cin.rgb.g+255.0f*amount*(threshold[tresshold_id]-0.5f))));
         c.rgb.b = MAX(0x0,MIN(0xff,(int)((float)cin.rgb.b+255.0f*amount*(threshold[tresshold_id]-0.5f))));
         c.rgb.a = cin.rgb.a;
         out[y*width+x] = c;
         out[y*width+x].rgb.a = 255;
      }
   }
}

//Applies Floyd-Steinberg dithering to the input
//This version uses per color component errror values,
//this usually does not work well with most palettes
static void dither_floyd(SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, Color_d3 *pal_d3, int distance_mode)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color cin = in[y*width+x];
         if(cin.rgb.a<alpha_threshold)
         {
            out[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         
         SLK_Color p = palette_find_closest(pal,pal_d3,cin,distance_mode);
         double error_r = (double)cin.rgb.r-(double)p.rgb.r;
         double error_g = (double)cin.rgb.g-(double)p.rgb.g;
         double error_b = (double)cin.rgb.b-(double)p.rgb.b;
         floyd_apply_error(in,error_r*(7.0/16.0),error_g*(7.0/16.0),error_b*(7.0/16.0),x+1,y,width,height);
         floyd_apply_error(in,error_r*(3.0/16.0),error_g*(3.0/16.0),error_b*(3.0/16.0),x-1,y+1,width,height);
         floyd_apply_error(in,error_r*(5.0/16.0),error_g*(5.0/16.0),error_b*(5.0/16.0),x,y+1,width,height);
         floyd_apply_error(in,error_r*(1.0/16.0),error_g*(1.0/16.0),error_b*(1.0/16.0),x+1,y+1,width,height);

         out[y*width+x] = p;
         out[y*width+x].rgb.a = 255;
      }
   }
}

//Applies Floyd-Steinberg dithering to the input
//This version uses distributed error values,
//this results in better results for most palettes
static void dither_floyd2(SLK_Color *in, SLK_Color *out, int width, int height, SLK_Palette *pal, Color_d3 *pal_d3, int distance_mode)
{
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         SLK_Color cin = in[y*width+x];
         if(cin.rgb.a<alpha_threshold)
         {
            out[y*width+x] = SLK_color_create(0,0,0,0);
            continue;
         }
         
         SLK_Color p = palette_find_closest(pal,pal_d3,cin,distance_mode);
         double error = ((double)cin.rgb.r-(double)p.rgb.r);
         error+=((double)cin.rgb.g-(double)p.rgb.g);
         error+=((double)cin.rgb.b-(double)p.rgb.b);
         error = error/3.0;
         floyd_apply_error(in,error*(7.0/16.0),error*(7.0/16.0),error*(7.0/16.0),x+1,y,width,height);
         floyd_apply_error(in,error*(3.0/16.0),error*(3.0/16.0),error*(3.0/16.0),x-1,y+1,width,height);
         floyd_apply_error(in,error*(5.0/16.0),error*(5.0/16.0),error*(5.0/16.0),x,y+1,width,height);
         floyd_apply_error(in,error*(1.0/16.0),error*(1.0/16.0),error*(1.0/16.0),x+1,y+1,width,height);

         out[y*width+x] = p;
         out[y*width+x].rgb.a = 255;
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
   r = in->rgb.r+error_r;
   g = in->rgb.g+error_g;
   b = in->rgb.b+error_b;

   in->rgb.r = MAX(0x0,MIN(r,0xff));
   in->rgb.g = MAX(0x0,MIN(g,0xff));
   in->rgb.b = MAX(0x0,MIN(b,0xff));
}

static SLK_Color palette_find_closest(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c, int distance_mode)
{
   SLK_Color out = {0};

   switch(distance_mode)
   {
   case 0: //RGB
      out = palette_find_closest_rgb(pal,pal_d3,c);
      break;
   case 1: //CIE76
      out = palette_find_closest_cie76(pal,pal_d3,c);
      break;
   case 2: //CIE94
      out = palette_find_closest_cie94(pal,pal_d3,c);
      break;
   case 3: //CIEDE200
      out = palette_find_closest_ciede2000(pal,pal_d3,c);
      break;
   case 4: //XYZ
      out = palette_find_closest_xyz(pal,pal_d3,c);
      break;
   case 5: //YCC
      out = palette_find_closest_ycc(pal,pal_d3,c);
      break;
   case 6: //YIQ
      out = palette_find_closest_yiq(pal,pal_d3,c);
      break;
   case 7: //YUV
      out = palette_find_closest_yuv(pal,pal_d3,c);
      break;
   }

   return out;
}

static Color_d3 color_to_rgb(SLK_Color c)
{
   Color_d3 d3;

   d3.c0 = (double)c.rgb.r/255.0;
   d3.c1 = (double)c.rgb.g/255.0;
   d3.c2 = (double)c.rgb.b/255.0;

   return d3;
}

//Convert to xyz then to lab color space
static Color_d3 color_to_lab(SLK_Color c)
{
   Color_d3 xyz = color_to_xyz(c) ;
   Color_d3 l;
 
   //x component
   if(xyz.c0>0.008856)
      xyz.c0 = pow(xyz.c0,1.0/3.0);
   else
      xyz.c0 = (7.787*xyz.c0)+(16.0/116.0);

   //y component
   if(xyz.c1>0.008856)
      xyz.c1 = pow(xyz.c1,1.0/3.0);
   else
      xyz.c1 = (7.787*xyz.c1)+(16.0/116.0);

   //z component
   if(xyz.c2>0.008856)
      xyz.c2 = pow(xyz.c2,1.0/3.0);
   else
      xyz.c2 = (7.787*xyz.c2)+(16.0/116.0);

   l.c0 = 116.0*xyz.c1-16.0;
   l.c1 = 500.0*(xyz.c0-xyz.c1);
   l.c2 = 200.0*(xyz.c1-xyz.c2);

   return l;
}

static Color_d3 color_to_xyz(SLK_Color c)
{
   Color_d3 in = color_to_rgb(c);
   Color_d3 x;

   //red component
   if(in.c0>0.04045)
      in.c0 = pow((in.c0+0.055)/1.055,2.4)*100.0;
   else
      in.c0 = (in.c0/12.92)*100.0;
  
   //green component
   if(in.c1>0.04045)
      in.c1 = pow((in.c1+0.055)/1.055,2.4)*100.0;
   else
      in.c1 = (in.c1/12.92)*100.0;
  
   //blue component
   if(in.c2>0.04045)
      in.c2 = pow((in.c2+0.055)/1.055,2.4)*100.0;
   else
      in.c2 = (in.c2/12.92)*100.0;

   x.c0 = (in.c0*0.4124+in.c1*0.3576+in.c2*0.1805)/95.05;
   x.c1 = (in.c0*0.2126+in.c1*0.7152+in.c2*0.0722)/100.0;
   x.c2 = (in.c0*0.0193+in.c1*0.1192+in.c2*0.9504)/108.89;
 
   return x;
}

static Color_d3 color_to_ycc(SLK_Color c)
{
   double r = (double)c.rgb.r;
   double g = (double)c.rgb.g;
   double b = (double)c.rgb.b;
   Color_d3 y;

   y.c0 = 0.299f*r+0.587f*g+0.114f*b;
   y.c1 = -0.16874f*r-0.33126f*g+0.5f*b;
   y.c2 = 0.5f*r-0.41869f*g-0.08131f*b;

   return y;
}

static Color_d3 color_to_yiq(SLK_Color c)
{
   double r = (double)c.rgb.r;
   double g = (double)c.rgb.g;
   double b = (double)c.rgb.b;
   Color_d3 y;

   y.c0 = 0.2999f*r+0.587f*g+0.114f*b;
   y.c1 = 0.595716f*r-0.274453f*g-0.321264f*b;
   y.c2 = 0.211456f*r-0.522591f*g+0.31135f*b;

   return y;
}

static Color_d3 color_to_yuv(SLK_Color c)
{
   double r = (double)c.rgb.r;
   double g = (double)c.rgb.g;
   double b = (double)c.rgb.b;
   Color_d3 y;

   y.c0 = 0.2999f*r+0.587f*g+0.114f*b;
   y.c1 = 0.492f*(b-y.c0);
   y.c2 = 0.887f*(r-y.c0);

   return y;
}

static Color_d3 color_to_hsv(SLK_Color c)
{
   float r = (float)c.rgb.r/255.0f;
   float g = (float)c.rgb.g/255.0f;
   float b = (float)c.rgb.b/255.0f;
   float cmax = MAX(r,MAX(g,b));
   float cmin = MIN(r,MIN(g,b));
   float diff = cmax-cmin;
   Color_d3 hsv = {0};

   if(cmax==cmin)
      hsv.c0 = 0.0f;
   else if(cmax==r)
      hsv.c0 = fmod(((g-b)/diff),6.0f);
   else if(cmax==g)
      hsv.c0 = (b-r)/diff+2.0f;
   else if(cmax==b)
      hsv.c0 = (r-g)/diff+4.0f;

   hsv.c2 = cmax;
   hsv.c0*=60.0f;
   hsv.c1 = diff/hsv.c2;

   return hsv;
}

static SLK_Color hsv_to_color(Color_d3 hsv)
{
   SLK_Color rgb;
   float r = 0.0f,g = 0.0f,b = 0.0f;

   while(hsv.c0<0.0f)
      hsv.c0+=360.0f;
   hsv.c0 = fmod(hsv.c0,360.0f);
   float c = hsv.c2*hsv.c1;
   float x = c*(1.0f-fabs(fmod(hsv.c0/60.0f,2.0f)-1.0f));
   float m = hsv.c2-c;

   if(hsv.c0>=0.0f&&hsv.c0<60.0f)
   {
      r = c+m;
      g = x+m;
      b = m;
   }
   else if(hsv.c0>=60.0f&&hsv.c0<120.0f)
   {
      r = x+m;
      g = c+m;
      b = m;
   }
   else if(hsv.c0>=120.0f&&hsv.c0<180.0f)
   {
      r = m;
      g = c+m;
      b = x+m;
   }
   else if(hsv.c0>=180.0f&&hsv.c0<240.0f)
   {
      r = m;
      g = x+m;
      b = c+m;
   }
   else if(hsv.c0>=240.0f&&hsv.c0<300.0f)
   {
      r = x+m;
      g = m;
      b = c+m;
   }
   else if(hsv.c0>=300.0f&&hsv.c0<360.0f)
   {
      r = c+m;
      g = m;
      b = x+m;
   }

   rgb.rgb.r = (uint8_t)(r*255.0f);
   rgb.rgb.g = (uint8_t)(g*255.0f);
   rgb.rgb.b = (uint8_t)(b*255.0f);

   return rgb;
}

static SLK_Color palette_find_closest_rgb(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c)
{
   if(c.rgb.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   Color_d3 in = color_to_rgb(c);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in,pal_d3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_cie76(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c)
{
   if(c.rgb.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.rgb.r = MAX(0x0,MIN(0xff,c.rgb.r));
   cin.rgb.g = MAX(0x0,MIN(0xff,c.rgb.g));
   cin.rgb.b = MAX(0x0,MIN(0xff,c.rgb.b));
   Color_d3 in = color_to_lab(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in,pal_d3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_cie94(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c)
{
   if(c.rgb.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.rgb.r = MAX(0x0,MIN(0xff,c.rgb.r));
   cin.rgb.g = MAX(0x0,MIN(0xff,c.rgb.g));
   cin.rgb.b = MAX(0x0,MIN(0xff,c.rgb.b));
   Color_d3 in = color_to_lab(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = cie94_color_dist2(in,pal_d3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_ciede2000(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c)
{
   if(c.rgb.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.rgb.r = MAX(0x0,MIN(0xff,c.rgb.r));
   cin.rgb.g = MAX(0x0,MIN(0xff,c.rgb.g));
   cin.rgb.b = MAX(0x0,MIN(0xff,c.rgb.b));
   Color_d3 in = color_to_lab(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = ciede2000_color_dist2(in,pal_d3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_xyz(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c)
{
   if(c.rgb.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.rgb.r = MAX(0x0,MIN(0xff,c.rgb.r));
   cin.rgb.g = MAX(0x0,MIN(0xff,c.rgb.g));
   cin.rgb.b = MAX(0x0,MIN(0xff,c.rgb.b));
   Color_d3 in = color_to_xyz(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in,pal_d3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_ycc(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c)
{
   if(c.rgb.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.rgb.r = MAX(0x0,MIN(0xff,c.rgb.r));
   cin.rgb.g = MAX(0x0,MIN(0xff,c.rgb.g));
   cin.rgb.b = MAX(0x0,MIN(0xff,c.rgb.b));
   Color_d3 in = color_to_ycc(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in,pal_d3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_yiq(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c)
{
   if(c.rgb.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.rgb.r = MAX(0x0,MIN(0xff,c.rgb.r));
   cin.rgb.g = MAX(0x0,MIN(0xff,c.rgb.g));
   cin.rgb.b = MAX(0x0,MIN(0xff,c.rgb.b));
   Color_d3 in = color_to_yiq(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in,pal_d3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static SLK_Color palette_find_closest_yuv(SLK_Palette *pal, Color_d3 *pal_d3, SLK_Color c)
{
   if(c.rgb.a==0)
      return pal->colors[0];

   double min_dist = 10000000000000.0f;
   int min_index = 0;
   SLK_Color cin;
   cin.rgb.r = MAX(0x0,MIN(0xff,c.rgb.r));
   cin.rgb.g = MAX(0x0,MIN(0xff,c.rgb.g));
   cin.rgb.b = MAX(0x0,MIN(0xff,c.rgb.b));
   Color_d3 in = color_to_yuv(cin);

   for(int i = 0;i<pal->used;i++)
   {   
      double dist = color_dist2(in,pal_d3[i]);
      if(dist<min_dist)
      {
         min_dist = dist;
         min_index = i;
      }
   }

   return pal->colors[min_index];
}

static double cie94_color_dist2(Color_d3 c0, Color_d3 c1)
{
   double L = c0.c0-c1.c0;
   double C1 = sqrt(c0.c1*c0.c1+c0.c2*c0.c2);
   double C2 = sqrt(c1.c1*c1.c1+c1.c2*c1.c2);
   double C = C1-C2;
   double H = sqrt((c0.c1-c1.c1)*(c0.c1-c1.c1)+(c0.c2-c1.c2)*(c0.c2-c1.c2)-C*C);
   double r1 = L;
   double r2 = C/(1.0f+0.045f*C1);
   double r3 = H/(1.0f+0.015f*C1);

   return r1*r1+r2*r2+r3*r3;
}

static double ciede2000_color_dist2(Color_d3 c0, Color_d3 c1)
{
   double C1 = sqrt(c0.c1*c0.c1+c0.c2*c0.c2);
   double C2 = sqrt(c1.c1*c1.c1+c1.c2*c1.c2);
   double C_ = (C1+C2)/2.0f;

   double C_p2 = pow(C_,7.0f);
   double v = 0.5f*(1.0f-sqrt(C_p2/(C_p2+6103515625.0f)));
   double a1 = (1.0f+v)*c0.c1;
   double a2 = (1.0f+v)*c1.c1;

   double Cs1 = sqrt(a1*a1+c0.c2*c0.c2);
   double Cs2 = sqrt(a2*a2+c1.c2*c1.c2);

   double h1 = 0.0f;
   if(c0.c2!=0||a1!=0)
   {
      h1 = atan2(c0.c2,a1);
      if(h1<0)
         h1+=2.0f*M_PI;
   }
   double h2 = 0.0f;
   if(c1.c2!=0||a2!=0)
   {
      h2 = atan2(c1.c2,a2);
      if(h2<0)
         h2+=2.0f*M_PI;
   }

   double L = c1.c0-c0.c0;
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

   double L_ = (c0.c0+c1.c0)/2.0f;
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

static double color_dist2(Color_d3 a, Color_d3 b)
{
   double diff_0 = b.c0-a.c0;
   double diff_1 = b.c1-a.c1;
   double diff_2 = b.c2-a.c2;

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
   case 5: sample_lanczos(in,out,width,height); break;
   }
}

//Nearest neighbour sampling,
//rounding the position
static void sample_round(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   double w = (double)(in->width-1)/(double)width;
   double h = (double)(in->height-1)/(double)height;
   double offx = (double)offset_x/100.0;
   double offy = (double)offset_y/100.0;

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double dx = (double)x+offx;
         double dy = (double)y+offy;

         out[y*width+x] = SLK_rgb_sprite_get_pixel(in,round(dx*w),round(dy*h));
      }
   }
}

//Nearest neighbour sampling,
//flooring the position
static void sample_floor(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   double w = (double)(in->width-1)/(double)width;
   double h = (double)(in->height-1)/(double)height;
   double offx = (double)offset_x/100.0;
   double offy = (double)offset_y/100.0;

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double dx = (double)x+offx;
         double dy = (double)y+offy;

         out[y*width+x] = SLK_rgb_sprite_get_pixel(in,floor(dx*w),floor(dy*h));
      }
   }
}

//Nearest neighbour sampling,
//ceiling the position
static void sample_ceil(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   double w = (double)(in->width-1)/(double)width;
   double h = (double)(in->height-1)/(double)height;
   double offx = (double)offset_x/100.0;
   double offy = (double)offset_y/100.0;

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         double dx = (double)x+offx;
         double dy = (double)y+offy;

         out[y*width+x] = SLK_rgb_sprite_get_pixel(in,ceil(dx*w),ceil(dy*h));
      }
   }
}

//Bilinear sampling
static void sample_linear(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;
   float foffx = (float)offset_x/100.0f;
   float foffy = (float)offset_y/100.0f;

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((float)x+foffx)*fw);
         int iy = (int)(((float)y+foffy)*fh);
         float six = (((float)x+foffx)*fw)-(float)ix;
         float siy = (((float)y+foffy)*fh)-(float)iy;

         SLK_Color c;
         SLK_Color c1,c2,c3,c4;
         c1 = SLK_rgb_sprite_get_pixel(in,ix,iy);
         c2 = SLK_rgb_sprite_get_pixel(in,ix+1,iy);
         c3 = SLK_rgb_sprite_get_pixel(in,ix,iy+1);
         c4 = SLK_rgb_sprite_get_pixel(in,ix+1,iy+1);

         //r value
         float c1t = ((1.0f-six)*(float)c1.rgb.r+six*(float)c2.rgb.r);
         float c2t = ((1.0f-six)*(float)c3.rgb.r+six*(float)c4.rgb.r);
         c.rgb.r = (int)((1.0f-siy)*c1t+siy*c2t);

         //g value
         c1t = ((1.0f-six)*(float)c1.rgb.g+six*(float)c2.rgb.g);
         c2t = ((1.0f-six)*(float)c3.rgb.g+six*(float)c4.rgb.g);
         c.rgb.g = (int)((1.0f-siy)*c1t+siy*c2t);

         //b value
         c1t = ((1.0f-six)*(float)c1.rgb.b+six*(float)c2.rgb.b);
         c2t = ((1.0f-six)*(float)c3.rgb.b+six*(float)c4.rgb.b);
         c.rgb.b = (int)((1.0f-siy)*c1t+siy*c2t);

         //a value
         c1t = ((1.0f-six)*(float)c1.rgb.a+six*(float)c2.rgb.a);
         c2t = ((1.0f-six)*(float)c3.rgb.a+six*(float)c4.rgb.a);
         c.rgb.a = (int)((1.0f-siy)*c1t+siy*c2t);

         out[y*width+x].rgb.r = c.rgb.r;
         out[y*width+x].rgb.b = c.rgb.b;
         out[y*width+x].rgb.g = c.rgb.g;
         out[y*width+x].rgb.a = c.rgb.a;
      }
   }
}

//Bicubic sampling
static void sample_bicubic(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   float fw = (float)(in->width-1)/(float)width;
   float fh = (float)(in->height-1)/(float)height;
   float foffx = (float)offset_x/100.0f;
   float foffy = (float)offset_y/100.0f;

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((float)x+foffx)*fw);
         int iy = (int)(((float)y+foffy)*fh);
         float six = (((float)x+foffx)*fw)-(float)ix;
         float siy = (((float)y+foffy)*fh)-(float)iy;

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
         float c0 = cubic_hermite((float)c00.rgb.r,(float)c10.rgb.r,(float)c20.rgb.r,(float)c30.rgb.r,six);
         float c1 = cubic_hermite((float)c01.rgb.r,(float)c11.rgb.r,(float)c21.rgb.r,(float)c31.rgb.r,six);
         float c2 = cubic_hermite((float)c02.rgb.r,(float)c12.rgb.r,(float)c22.rgb.r,(float)c32.rgb.r,six);
         float c3 = cubic_hermite((float)c03.rgb.r,(float)c13.rgb.r,(float)c23.rgb.r,(float)c33.rgb.r,six);
         float val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].rgb.r = MAX(0x0,MIN(0xff,(int)val));

         //g value
         c0 = cubic_hermite((float)c00.rgb.g,(float)c10.rgb.g,(float)c20.rgb.g,(float)c30.rgb.g,six);
         c1 = cubic_hermite((float)c01.rgb.g,(float)c11.rgb.g,(float)c21.rgb.g,(float)c31.rgb.g,six);
         c2 = cubic_hermite((float)c02.rgb.g,(float)c12.rgb.g,(float)c22.rgb.g,(float)c32.rgb.g,six);
         c3 = cubic_hermite((float)c03.rgb.g,(float)c13.rgb.g,(float)c23.rgb.g,(float)c33.rgb.g,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].rgb.g = MAX(0x0,MIN(0xff,(int)val));

         //b value
         c0 = cubic_hermite((float)c00.rgb.b,(float)c10.rgb.b,(float)c20.rgb.b,(float)c30.rgb.b,six);
         c1 = cubic_hermite((float)c01.rgb.b,(float)c11.rgb.b,(float)c21.rgb.b,(float)c31.rgb.b,six);
         c2 = cubic_hermite((float)c02.rgb.b,(float)c12.rgb.b,(float)c22.rgb.b,(float)c32.rgb.b,six);
         c3 = cubic_hermite((float)c03.rgb.b,(float)c13.rgb.b,(float)c23.rgb.b,(float)c33.rgb.b,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].rgb.b = MAX(0x0,MIN(0xff,(int)val));

         //a value
         c0 = cubic_hermite((float)c00.rgb.a,(float)c10.rgb.a,(float)c20.rgb.a,(float)c30.rgb.a,six);
         c1 = cubic_hermite((float)c01.rgb.a,(float)c11.rgb.a,(float)c21.rgb.a,(float)c31.rgb.a,six);
         c2 = cubic_hermite((float)c02.rgb.a,(float)c12.rgb.a,(float)c22.rgb.a,(float)c32.rgb.a,six);
         c3 = cubic_hermite((float)c03.rgb.a,(float)c13.rgb.a,(float)c23.rgb.a,(float)c33.rgb.a,six);
         val = cubic_hermite(c0,c1,c2,c3,siy);
         out[y*width+x].rgb.a = MAX(0x0,MIN(0xff,(int)val));
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

//Lanczos downsampling
static void sample_lanczos(const SLK_RGB_sprite *in, SLK_Color *out, int width, int height)
{
   double fw = (double)(in->width-1)/(double)width;
   double fh = (double)(in->height-1)/(double)height;
   float foffx = (float)offset_x/100.0f;
   float foffy = (float)offset_y/100.0f;

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<height;y++)
   {
      for(int x = 0;x<width;x++)
      {
         int ix = (int)(((double)x+foffx)*fw);
         int iy = (int)(((double)y+foffy)*fh);
         double sx = (((double)x+foffx)*fw)-(double)ix;
         double sy = (((double)y+foffy)*fh)-(double)iy;
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

            r[i] = a0*(double)p0.rgb.r+a1*(double)p1.rgb.r+a2*(double)p2.rgb.r+a3*(double)p3.rgb.r+a4*(double)p4.rgb.r+a5*(double)p5.rgb.r;
            g[i] = a0*(double)p0.rgb.g+a1*(double)p1.rgb.g+a2*(double)p2.rgb.g+a3*(double)p3.rgb.g+a4*(double)p4.rgb.g+a5*(double)p5.rgb.g;
            b[i] = a0*(double)p0.rgb.b+a1*(double)p1.rgb.b+a2*(double)p2.rgb.b+a3*(double)p3.rgb.b+a4*(double)p4.rgb.b+a5*(double)p5.rgb.b;
            a[i] = a0*(double)p0.rgb.a+a1*(double)p1.rgb.a+a2*(double)p2.rgb.a+a3*(double)p3.rgb.a+a4*(double)p4.rgb.a+a5*(double)p5.rgb.a;
         }

         p.rgb.r = MAX(0x0,MIN(0xff,(int)(b0*r[0]+b1*r[1]+b2*r[2]+b3*r[3]+b4*r[4]+b5*r[5])));
         p.rgb.g = MAX(0x0,MIN(0xff,(int)(b0*g[0]+b1*g[1]+b2*g[2]+b3*g[3]+b4*g[4]+b5*g[5])));
         p.rgb.b = MAX(0x0,MIN(0xff,(int)(b0*b[0]+b1*b[1]+b2*b[2]+b3*b[3]+b4*b[4]+b5*b[5])));
         p.rgb.a = MAX(0x0,MIN(0xff,(int)(b0*a[0]+b1*a[1]+b2*a[2]+b3*a[3]+b4*a[4]+b5*a[5])));
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

static void quant_compute_kmeans(SLK_RGB_sprite *data, int pal_in)
{
   quant_cluster_list_init();
   quant_centroid_list = malloc(sizeof(*quant_centroid_list)*quant_k);
   quant_assignment = malloc(sizeof(*quant_assignment)*(data->width*data->height));
   memset(quant_assignment,0,sizeof(*quant_assignment)*data->width*data->height);

   int iter = 0;
   int max_iter = 16;
   //if(pal_in)
      //max_iter = 2;
   double *previous_variance = malloc(sizeof(*previous_variance)*quant_k);
   double variance = 0.0;
   double delta = 0.0;
   double delta_max = 0.0;
   double threshold = 0.00005;
   for(int i = 0;i<quant_k;i++)
      previous_variance[i] = 1.0;

   for(;;)
   {
      quant_get_cluster_centroid(data,pal_in,1<<palette_weight);
      quant_cluster_list_init();
      for(int i = 0;i<data->width*data->height;i++)
      {
         SLK_Color color = data->data[i];
         quant_assignment[i] = quant_nearest_color_idx(color,quant_centroid_list);
         dyn_array_add(SLK_Color,&quant_cluster_list[quant_assignment[i]],1,color);
      }

      delta_max = 0.0;
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

static void quant_get_cluster_centroid(SLK_RGB_sprite *data, int pal_in, int weight_pal)
{
   for(int i = 0;i<quant_k;i++)
   {
      if(quant_cluster_list[i].used>0)
      {
         if(pal_in)
            quant_centroid_list[i] = quant_colors_mean(&quant_cluster_list[i],palette->colors[i],weight_pal);
         else
            quant_centroid_list[i] = quant_colors_mean(&quant_cluster_list[i],SLK_color_create(0,0,0,0),0);
      }
      else
      {
         if(pal_in)
            quant_centroid_list[i] = palette->colors[i];
         else
            quant_centroid_list[i] = quant_pick_random_color(data);
      }
   }
}

static SLK_Color quant_colors_mean(dyn_array *color_list,SLK_Color color, int weight_color)
{
   int r = 0,g = 0,b = 0;
   int length = color_list->used;
   for(int i = 0;i<length;i++)
   {
      r+=dyn_array_element(SLK_Color,color_list,i).rgb.r;
      g+=dyn_array_element(SLK_Color,color_list,i).rgb.g;
      b+=dyn_array_element(SLK_Color,color_list,i).rgb.b;
   }

   if(weight_color!=0)
      weight_color = length/weight_color;
   length+=weight_color;
   r+=color.rgb.r*weight_color;
   g+=color.rgb.g*weight_color;
   b+=color.rgb.b*weight_color;

   if(length!=0)
   {
      r/=length;
      g/=length;
      b/=length;
   }

   return (SLK_Color){.rgb.r = r, .rgb.g = g, .rgb.b = b};
}

static SLK_Color quant_pick_random_color(SLK_RGB_sprite *data)
{
   return data->data[(int)(((double)rand()/(double)RAND_MAX)*data->width*data->height)];
}

static int quant_nearest_color_idx(SLK_Color color, SLK_Color *color_list)
{
   double dist_min = 0xfff;
   double dist = 0.0;
   int idx = 0;
   for(int i = 0;i<quant_k;i++)
   {
      dist = quant_distance(color,color_list[i]);
      if(dist<dist_min)
      {
         dist_min = dist;
         idx = i;
      }
   }

   return idx;
}

static double quant_distance(SLK_Color color0, SLK_Color color1)
{
   /*double dr =  (color0.r-color1.r)/255.0;
   double dg =  (color0.g-color1.g)/255.0;
   double db =  (color0.b-color1.b)/255.0;
   return sqrt(dr*dr+dg*dg+db*db);*/
   double mr = 0.5*(color0.rgb.r+color1.rgb.r),
      dr = color0.rgb.r-color1.rgb.r,
      dg = color0.rgb.g-color1.rgb.g,
      db = color0.rgb.b-color1.rgb.b;
   double distance = (2.0*dr*dr)+(4.0*dg*dg)+(3.0*db*db)+(mr*((dr*dr)-(db*db))/256.0);
   return sqrt(distance)/(3.0*255.0);
}

static double quant_colors_variance(dyn_array *color_list)
{
   int length = color_list->used;
   SLK_Color mean = quant_colors_mean(color_list,SLK_color_create(0,0,0,0),0);
   double dist = 0.0;
   double dist_sum = 0.0;
   for(int i = 0;i<length;i++)
   {
      dist = quant_distance(dyn_array_element(SLK_Color,color_list,i),mean);
      dist_sum+=dist*dist;
   }

   return dist_sum/(double)length;
}

static void post_process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   SLK_RGB_sprite *tmp = SLK_rgb_sprite_create(in->width,in->height);
   SLK_rgb_sprite_copy(tmp,in);

   #pragma omp parallel for schedule(dynamic, 1)
   for(int y = 0;y<in->height;y++)
   {
      for(int x = 0;x<in->width;x++)
      {
         int empty = 0;

         //Inline
         if(image_inline>=0&&SLK_rgb_sprite_get_pixel(tmp,x,y).rgb.a!=0)
         {
            if(SLK_rgb_sprite_get_pixel(tmp,x,y-1).rgb.a==0) empty++;
            if(SLK_rgb_sprite_get_pixel(tmp,x-1,y).rgb.a==0) empty++;
            if(SLK_rgb_sprite_get_pixel(tmp,x+1,y).rgb.a==0) empty++;
            if(SLK_rgb_sprite_get_pixel(tmp,x,y+1).rgb.a==0) empty++;

            if(empty!=0)
               SLK_rgb_sprite_set_pixel(out,x,y,palette->colors[image_inline]);
         }
         
         //Outline
         if(image_outline>=0&&SLK_rgb_sprite_get_pixel(tmp,x,y).rgb.a==0)
         {
            if(SLK_rgb_sprite_get_pixel(tmp,x,y-1).rgb.a!=0) empty++;
            if(SLK_rgb_sprite_get_pixel(tmp,x-1,y).rgb.a!=0) empty++;
            if(SLK_rgb_sprite_get_pixel(tmp,x+1,y).rgb.a!=0) empty++;
            if(SLK_rgb_sprite_get_pixel(tmp,x,y+1).rgb.a!=0) empty++;

            if(empty!=0)
               SLK_rgb_sprite_set_pixel(out,x,y,palette->colors[image_outline]);
         }
      }
   }

   //Cleanup
   SLK_rgb_sprite_destroy(tmp);
}
//-------------------------------------
