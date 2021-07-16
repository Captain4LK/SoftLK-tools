# Changelog

Current development version: SLK_img2pixel 1.2-dev

----------------------------------------
Version:                SLK_img2pixel 1.2 (tba)
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
