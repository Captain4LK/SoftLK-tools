# SLK_img2pixel

![anim](../screenshots/crate.gif)

A tool for transforming images into pixel art.

# Download 

* SLK_img2pixel: [itch.io](https://captain4lk.itch.io/slk-img2pixel)

# Contact

Here is a link to the SLK_img2pixel Discord. Feel free to pop in and make suggestions or ask questions: https://discord.gg/Nch8hjdZ2V

# Building from source

* First, clone this repository: ``https://github.com/Captain4LK/SoftLK-tools.git``

## Dependencies

In general, I try to keep the amount of external dependencies as low as possible, but some are still needed, as listed below:

* [SDL2](https://www.libsdl.org/download-2.0.php)

## Building

* cd into SLK_img2pixels directory: ``cd SLK_img2pixel``
* run the shell script associated with your platform and supply it with the desired target, e.g: ``sh linux.sh gui_hlh``, possible targets are:
  * ``gui_hlh`` - the new gui, seen since version 1.4
  * ``gui`` - the old gui, seen on all versions prior to 1.4
  * ``cmd`` - command line version
  * ``video`` - experimental video processing version
* The binary can be found at the top level of the repo in the ``bin/`` directory

# Gallery

![SLK_img2pixel_preview](../screenshots/SLK_img2pixel.png)

# Changelog

Current development version: SLK_img2pixel 1.5-dev

----------------------------------------
Version:                SLK_img2pixel 1.5 (
----------------------------------------

CHANGES:

* optimized and corrected gaussian blur: about 5x faster and more correct for large blurring radius

----------------------------------------
Version:                SLK_img2pixel 1.4 (17.09.2022)
----------------------------------------

CHANGES:

* new gui
* drag'n drop support for images
* fixed batch processing appending png extension without removing original extension

----------------------------------------
Version:                SLK_img2pixel 1.3 (21.04.2022)
----------------------------------------

CHANGES:

* fixed multiple memory leaks
* removed supersampling sample mode (virtually the same results can be achieved using a high gauss blurring setting)
* added persistent paths for file/folder selection
* added K-means color distance mode
* made empty tabs in gui unclickable
* removed gif processing (it was basically useless anyway)
* Options in the gui are now only shown if they actually do anything
* Support for [qoi format](https://qoiformat.org/)

----------------------------------------
Version:                SLK_img2pixel 1.2 (16.08.2021)
----------------------------------------

CHANGES:

* replaced image quantization algorithm with higher quality one
* added command line version
* added hue adjustment slider
* rearranged the gui, moved all sampling options in separate tab
* added x and y offset sliders for fine-tuned sampling
* added option to add inlines/outlines
* reworked dithering, availible modes: 
   * no dithering
   * bayer 8x8
   * bayer 4x4
   * bayer 2x2
   * cluster 8x8
   * cluster 4x4
   * floyd-steinberg
* lowered memory consumption by using rgb888 pixel format while processing images

----------------------------------------
Version:                SLK_img2pixel 1.1 - color quantization (24.04.2021)
----------------------------------------

CHANGES:

* added support for changing default settings
* fixed kdialog support
* added support for exporting upscaled images
* added color count slider
* added basic color quantization

----------------------------------------
Version:                SLK_img2pixel 1.0 - initial release (14.02.2021)
----------------------------------------

CHANGES:

* improved all sampling modes
* improved gaussian blur and image sharpening
* added color distance calculation modes (CIE76, CIE94, CIEDE2000, XYZ, YCC, YIQ, YUV)

----------------------------------------
Version:                SLK_img2pixel 0.4 - palette formats! (27.01.2021)
----------------------------------------

NOTE: 

* Consider downsizing very high resolution images before using them with SLK_img2pixel. Sharpening and gaussian blur are done on the input image, so performance will suffer when loading high resolution images.

CHANGES:

* fix sharpness not changing at value of 0
* renamed supersampling to box sampling
* added gaussian blur
* added lanczos sample mode
* added support for more palette formats(.gpl,.png,.hex)

----------------------------------------
Version:                SLK_img2pixel 0.3 - supersampling is amazing (24.01.2021)
----------------------------------------

CHANGES:

* preview now drawn by the gpu
* added relative image sizes
* added image sharpening
* improved bilinear sampling
* added bicubic sampling mode
* added supersampling sampling mode (works great for portraits!)

----------------------------------------
Version:                SLK_img2pixel 0.2 - unicode hell (22.01.2021)
----------------------------------------

CHANGES:

* added bilinear sampling mode
* added alpha threshold
* added gif support
* added quake, doom palettes
* added unicode support for windows

----------------------------------------
Version:                SLK_img2pixel 0.1 - initial prerelease (20.01.2021)
----------------------------------------
