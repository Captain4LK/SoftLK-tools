# SoftLK-tools

This repo hosts the source code for different (currently only one) tools that were made to be used with SoftLK-lib.

## SLK_img2pixel (v1.0-dev)

A tool for transforming images into pixel art.

![SLK_img2pixel_preview](screenshots/SLK_img2pixel.png)

# Building from source

* First you need to have [SoftLK-lib](https://codeberg.org/Captain4LK/SoftLK-lib) installed on your system, more instructions can be found in its repo.
* Then, clone this repository (note: the --recursive): ``git clone https://codeberg.org/Captain4LK/SoftLK-tools.git --recursive``
* cd into the directory of the tool you want to build
* cd into its bin/unix directory
* ...and build it: ``make``
* The binary can be found at the top level of the repo in the bin directory

# Download 

* SLK_img2pixel: [itch.io](https://captain4lk.itch.io/slk-img2pixel)

# License

All code in this repository (except the 'external' directory containing source code not by me) is released into the public domain (CC0), see COPYING for more info.

# Planned tools / ideas

* tile map editor
* pixel art editor

# Third party libraries

* [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/), zlib
* [cute_files](https://github.com/RandyGaul/cute_headers), zlib/unlicense
* [stb_image](https://github.com/nothings/stb), mit/unlicense
* [gifenc](https://github.com/lecram/gifenc), public domain
* [gifdec](https://github.com/lecram/gifdec), public domain
* [fopen_utf8](https://github.com/Photosounder/fopen_utf8/), only for windows builds, public domain
