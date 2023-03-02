#!/bin/sh

sources="utility.c assets.c image2pixel.c ../external/tinyfiledialogs.c"

if [ $# -lt 1 ]; then
   echo "unspecified target, need either gui, gui_hlh, cmd or video"
   exit 0 
fi

if [ $1 = "gui" ]; then
   sources="$sources main.c gui.c"

   gcc -o ../bin/SLK_img2pix $sources -Wall -Wextra -lm -lSDL2 -Og -g -Wstrict-aliasing=3 -fopenmp

elif [ $1 = "gui_hlh" ]; then
   sources="$sources main_hlh.c ../HLH_gui/HLH_gui_all.c"
   echo $sources

   gcc -o ../bin/SLK_img2pix $sources -Wall -Wextra -lm -lSDL2 -Og -g -Wstrict-aliasing=3 -fopenmp -Wno-unused

elif [ $1 = "cmd" ]; then
   sources="$sources main_cmd.c gui.c"
   echo $sources

   gcc -o ../bin/SLK_img2pix_cmd $sources -Wall -Wextra -lm -O3 -s -flto=auto -Wstrict-aliasing=3 -fopenmp

elif [ $1 = "video" ]; then
   sources="$sources main_video.c gui.c"
   echo "${sources}"

   gcc -o ../bin/SLK_img2pix_video $sources -Wall -Wextra -lm -lSDL2 -Og -g -Wstrict-aliasing=3 -fopenmp

else

   echo "unknown target, need either gui, cmd or video"

fi
