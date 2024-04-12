#!/bin/sh

flags="-I../external/ -Wall -Wextra -Wno-unused -Wshadow -std=c99"
sources="color.c tint.c blur.c kmeans.c image.c sharp.c sample.c hscb.c gamma.c postprocess.c dither.c palette.c unix/util_unix.c ../external/tinyfiledialogs.c"

if [ $# -lt 1 ]; then
   echo "unspecified target, need either gui or cmd"
   exit 0 
fi

if [ $1 = "gui" ]; then
   sources="$sources main.c gui.c ../HLH_gui/HLH_gui_all.c"

   gcc -o ../bin/SLK_img2pix $sources $flags -lm -lSDL2 -O3 -g -fopenmp -I../HLH_gui


elif [ $1 = "cmd" ]; then
   sources="$sources main_cmd.c ../HLH_gui/HLH_gui_all.c"

   gcc -o ../bin/SLK_img2pix_cmd $sources $flags -Wall -Wextra -lm -O3 -s -flto=auto -fopenmp -I../HLH_gui

else

   echo "unknown target, need either gui or cmd"

fi
