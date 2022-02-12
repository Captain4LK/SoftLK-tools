#!/bin/sh

SOURCES="main.c utility.c assets.c gui.c image2pixel.c ../external/tinyfiledialogs.c"

gcc -o ../bin/SLK_img2pix $SOURCES -Wall -Wextra -lm -lSDL2 -O3 -s -flto=auto
