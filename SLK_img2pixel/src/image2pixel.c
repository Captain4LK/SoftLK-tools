/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <SLK/SLK.h>
#include <SLK/SLK_gui.h>

#define HLH_JSON_IMPLEMENTATION
#include "../../external/HLH_json.h"
//-------------------------------------

//Internal includes
#include "process.h"
#include "sample.h"
#include "utility.h"
#include "quantization.h"
#include "image2pixel.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
int brightness = 0;
int contrast = 0;
int img_gamma = 100;
int saturation = 100;
int dither_amount = 250;
int alpha_threshold = 128;
int sharpen = 0;
int gauss = 80;
int pixel_scale_mode = 0;
int pixel_sample_mode = 0;
int pixel_process_mode = 1;
int pixel_distance_mode = 0;
int image_out_width = 128;
int image_out_height = 128;
int image_out_swidth = 2;
int image_out_sheight = 2;
SLK_Palette *palette = NULL;
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

void img2pixel_preset_load(FILE *f)
{
   if(!f)
      return;

   HLH_json5 fallback = {0};
   HLH_json5_root *root = HLH_json_parse_file_stream(f);
   fclose(f);

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
 
   HLH_json_write_file(f,&root->root);
   HLH_json_free(root);
   fclose(f);
}

void img2pixel_sharpen_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   sharpen_image(in,out);
}

void img2pixel_lowpass_image(SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   lowpass_image(in,out);
}

void img2pixel_quantize(int colors, SLK_RGB_sprite *in)
{
   quantize(palette,colors,in);
}

void img2pixel_process_image(const SLK_RGB_sprite *in, SLK_RGB_sprite *out)
{
   process_image(in,out,palette,pixel_sample_mode,pixel_process_mode,pixel_distance_mode);
}
//-------------------------------------
