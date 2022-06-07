#!/bin/sh

sources="utility.c assets.c gui.c image2pixel.c ../external/tinyfiledialogs.c"

if [ $# -lt 1 ]; then
   echo "unspecified target, need either gui,cmd or video"
   exit 0 
fi

if [ $1 = "gui" ]; then
   sources="$sources main.c"

   gcc -o ../bin/SLK_img2pix $sources -Wall -Wextra -lm -lSDL2 -Og -g

elif [ $1 = "cmd" ]; then
   sources="$sources main_cmd.c"
   echo $sources

   gcc -o ../bin/SLK_img2pix_cmd $sources -Wall -Wextra -lm -lSDL2 -O3 -s -flto=auto

elif [ $1 = "video" ]; then
   sources="$sources main_video.c"
   echo "${sources}"

   gcc -o ../bin/SLK_img2pix_video $sources -Wall -Wextra -lm -lSDL2 -Og -g

else

   echo "unknown target, need either gui, cmd or video"

fi
