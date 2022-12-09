/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/


#ifndef _IMAGE2PIXEL_H_

#define _IMAGE2PIXEL_H_
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct dyn_array dyn_array;
typedef struct
{
   int brightness;
   int contrast;
   int img_gamma;
   int saturation;
   int dither_amount;
   int alpha_threshold;
   int sharpen;
   int hue;
   int gauss;
   int offset_x;
   int offset_y;
   int image_outline;
   int image_inline;
   int pixel_scale_mode;
   int pixel_sample_mode;
   int pixel_process_mode;
   int pixel_distance_mode;
   int image_out_width;
   int image_out_height;
   int image_out_swidth;
   int image_out_sheight;
   int palette_weight;
   SLK_Palette *palette;

   dyn_array *quant_cluster_list;
   SLK_Color *quant_centroid_list;
   int *quant_assignment;
   int quant_k;
}I2P_state;

void img2pixel_state_init(I2P_state *s);
void img2pixel_state_free(I2P_state *s);
void img2pixel_preset_load(I2P_state *s, FILE *f);
void img2pixel_preset_save(I2P_state *s, FILE *f);

void         img2pixel_sharpen_image(I2P_state *s, SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void         img2pixel_lowpass_image(I2P_state *s, SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void         img2pixel_quantize(I2P_state *s, int colors, SLK_RGB_sprite *in);
void         img2pixel_process_image(I2P_state *s, const SLK_RGB_sprite *in, SLK_RGB_sprite *out);
void         img2pixel_reset_to_defaults(I2P_state *s);

int          img2pixel_get_brightness(const I2P_state *s);
void         img2pixel_set_brightness(I2P_state *s, int nbrightness);
int          img2pixel_get_contrast(const I2P_state *s);
void         img2pixel_set_contrast(I2P_state *s, int ncontrast);
int          img2pixel_get_gamma(const I2P_state *s);
void         img2pixel_set_gamma(I2P_state *s, int ngamma);
int          img2pixel_get_saturation(const I2P_state *s);
void         img2pixel_set_saturation(I2P_state *s, int nsaturation);
int          img2pixel_get_sharpen(const I2P_state *s);
void         img2pixel_set_sharpen(I2P_state *s, int nsharpen);
int          img2pixel_get_hue(const I2P_state *s);
void         img2pixel_set_hue(I2P_state *s, int nhue);
int          img2pixel_get_dither_amount(const I2P_state *s);
void         img2pixel_set_dither_amount(I2P_state *s, int namount);
int          img2pixel_get_alpha_threshold(const I2P_state *s);
void         img2pixel_set_alpha_threshold(I2P_state *s, int nthreshold);
int          img2pixel_get_gauss(const I2P_state *s);
void         img2pixel_set_gauss(I2P_state *s, int ngauss);
SLK_Palette *img2pixel_get_palette(I2P_state *s);
void         img2pixel_set_palette(I2P_state *s, SLK_Palette *npalette); //freeing is supposed to be handled by user!
int          img2pixel_get_palette_weight(const I2P_state *s);
void         img2pixel_set_palette_weight(I2P_state *s, int weight);
int          img2pixel_get_scale_mode(const I2P_state *s);
void         img2pixel_set_scale_mode(I2P_state *s, int nmode);
int          img2pixel_get_sample_mode(const I2P_state *s);
void         img2pixel_set_sample_mode(I2P_state *s, int nmode);
void         img2pixel_set_offset_x(I2P_state *s, int noffset);
int          img2pixel_get_offset_x(const I2P_state *s);
void         img2pixel_set_offset_y(I2P_state *s, int noffset);
int          img2pixel_get_offset_y(const I2P_state *s);
int          img2pixel_get_outline(const I2P_state *s);
void         img2pixel_set_outline(I2P_state *s, int nline);
int          img2pixel_get_inline(const I2P_state *s);
void         img2pixel_set_inline(I2P_state *s, int nline);
int          img2pixel_get_process_mode(const I2P_state *s);
void         img2pixel_set_process_mode(I2P_state *s, int nmode);
int          img2pixel_get_distance_mode(const I2P_state *s);
void         img2pixel_set_distance_mode(I2P_state *s, int nmode);
int          img2pixel_get_out_width(const I2P_state *s);
void         img2pixel_set_out_width(I2P_state *s, int nwidth);
int          img2pixel_get_out_height(const I2P_state *s);
void         img2pixel_set_out_height(I2P_state *s, int nheight);
int          img2pixel_get_out_swidth(const I2P_state *s);
void         img2pixel_set_out_swidth(I2P_state *s, int nwidth);
int          img2pixel_get_out_sheight(const I2P_state *s);
void         img2pixel_set_out_sheight(I2P_state *s, int nheight);
#ifdef __cplusplus
}
#endif
#endif
