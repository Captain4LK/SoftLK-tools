# SLK_img2pixel documentation (WIP)

# Table of contents

* [User interface](#user-interface)
   * [Preview](#preview)
   * [Settings](#settings)
   * [Save/Load](#save-load)
   * [Palette](#palette)
   * [General](#general)
   * [Process](#process)
   * [Special](#special)

# User interface

The user interface is divided into two moveable windows. 

## Preview

The window titled "Preview" serves as a preview window for both the input and the output image. You can switch between the input and output image by clicking the specific tab at the top of the window.

## Settings

The other window is titled "Settings" and is were you actually interact with the program. The window is divided into five tabs, aligned vertically on the left of the window.

### Save Load

**Load image**

Load an image from disc, supported formats (same as stb_image): 

   * JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
   * PNG 1/2/4/8/16-bit-per-channel
   * TGA (not sure what subset, if a subset)
   * BMP non-1bpp, non-RLE
   * PSD (composited view only, no extra channels, 8/16 bit-per-channel)
   * GIF (*comp always reports as 4-channel)
   * HDR (radiance rgbE format)
   * PIC (Softimage PIC)
   * PNM (PPM and PGM binary only)

***Save image***

Save the created image to disk. Only png files are supported for now.

***Load preset***

Loads a preset in from a json file. Presets store the current settings and palette for later use.

***Save preset***

Save the current settings and palette to a json file. Presets can be loaded again later.

***Select input dir***

Use for selecting the input directory for batch processing an entire directory (non-recursive). After selecting an input directory the batch processing will start once you select an output directory.

***Select output dir***

Use to select the output directory for batch processing. After selecting an output directory, all files in the directory will be processed and placed into the output directory.

### Palette

This tab contains a list of all palette colors. You can click a color and edit it with the sliders.

***red/green/blue***

Change the red/green/blue color components of the currently selected color.

***dist***

Set which color distance calculation mode to use. 
To figure out which color to use for a specific pixel, SLK_img2pixel needs to find the distance between two colors. There are many different ways of calculating this distance, some better, some worse. The following modes are available:

* RGB 
* CIE76
* CIE94
* CIEDE2000
* XYZ
* YCC
* YUV
* YIQ

***Load palette***

Load a palette from disk, the following formats are supported: 

   * pal
   * hex
   * png
   * gpl

***Save palette***

Save the current palette to disk, only the pal format is supported

### General

***Absolute/Relative***

Use to select the output image dimensions. On the top you can select wether to use relative or absoulte image dimensions. If you use absolute dimensions, you can choose an image size between 1 and 256. If you use relative dimensions, you can select an image downscaling factor between 1 and 32.

***Dither***

Set the dithering mode. Availible modes:

   * No dithering
   * Ordered 1 (bias towards lighter colors)
   * Ordered 2 (no bias)
   * FloydSteinberg 1
   * FloydSteinberg 2

***Amount***

Set the dithering amount. Only affects orederd dithering modes.

***Sample***

Set the downsampling mode to use. Availible modes:

   * Round (nearest neighbour sampling, rounding coordinates)
   * Floor (nearest neighbour sampling, flooring coordinates)
   * Ceil  (nearest neighbour sampling, ceiling coordinates)
   * Bilinear
   * Bicubic
   * Box sampling (use without gaussian blur for best results)
   * Lanczos

***Gauss***

Set the gaussian blur amount. Higher means more blurring, lower means less. Set to value of 1 to disable.

***Alpha***

Set the alpha threshold. SLK_img2pixel only supports outputing alpha values of 255 and 0. All alpha values above the threshold will be set to 255, all below to 0.

### Process

***Bright***

Tweak the brightness. (-255 = -1.0; 255 = 1.0)

***Contra***

Tweak the contrast. (-255 = -1.0; 255 = 1.0)

***Satura***

Tweak the saturation. (100 = 1.0; 200 = 2.0)

***Gamma***

Tweak the gamma level (100 = 1.0; 200 = 2.0)

***Sharp***

Sharpen the image. Sharpening is done on the input image, SLK_img2pixel might slow done on large input images.

### Special

***Load gif***

Select a gif  to process, processing will start once a target file has been selcted with "Save gif"

***Save gif***

Select the output file for gif processing. If you have selected an input gif, it will be processed after selecting an output gif.
