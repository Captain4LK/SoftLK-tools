#!/bin/sh

flags="-I../external/ -Wall -Wextra -Wno-unused -std=c99 -Wshadow"
sources="color.c blur.c kmeans.c image.c sharp.c sample.c hscb.c gamma.c postprocess.c dither.c palette.c unix/util_unix.c ../external/tinyfiledialogs.c"

if [ $# -lt 1 ]; then
   echo "unspecified target, need either gui, gui_hlh, cmd or video"
   exit 0 
fi

if [ $1 = "gui" ]; then
   sources="$sources main.c gui.c ../HLH_gui2/HLH_gui_all.c"

   gcc -o ../bin/SLK_img2pix $sources $flags -lm -lSDL2 -Og -g -Wstrict-aliasing=3 -fopenmp -I../HLH_gui2 -fsanitize=undefined,address


elif [ $1 = "cmd" ]; then
   sources="$sources main_cmd.c gui.c"
   echo $sources

   gcc -o ../bin/SLK_img2pix_cmd $sources -Wall -Wextra -lm -O3 -s -flto=auto -Wstrict-aliasing=3 

else

   echo "unknown target, need either gui, cmd or video"

fi
