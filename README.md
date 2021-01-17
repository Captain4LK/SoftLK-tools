# SoftLK-tools

This repo hosts the source code for different (currently only one) tools that were made to be used with SoftLK-lib.

## SLK_img2pixel

A tool for transforming images into pixel art.

![SLK_img2pixel_preview](screenshots/SLK_img2pixel.png)

# Building

* First you need to have [SoftLK-lib](https://codeberg.org/Captain4LK/SoftLK-lib) installed on your system, more instructions can be found in its repo.
* Then, clone this repository (note: the --recursive): ``git clone https://codeberg.org/Captain4LK/SoftLK-tools.git --recursive``
* cd into the directory of the tool you want to build
* cd into its bin/unix directory
* ...and build it: ``make``
* The binary can be found at the top level of the repo in the bin directory

# License

All code in this repository (except the 'external' directory containing source code not be my) is released into the public domain (CC0), see LICENSE for more info.

# Third party libraries

* [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/), zlib
