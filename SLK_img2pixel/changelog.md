# Changelog


Current development version: SLK_img2pixel 0.4

----------------------------------------
Version:                SLK_img2pixel 0.4 - palette formats! (tba)
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
