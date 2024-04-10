#!/bin/sh

flags="-I../external/ -Wall -Wextra -Wno-unused -Wshadow -std=c99"
sources="color.c tint.c blur.c kmeans.c image.c sharp.c sample.c hscb.c gamma.c postprocess.c dither.c palette.c win/util_win.c"

if [ $# -lt 1 ]; then
   echo "unspecified target, need either gui, gui_hlh, cmd or video"
   exit 0 
fi

if [ $1 = "gui" ]; then
   sources="$sources main.c gui.c ../HLH_gui2/HLH_gui_all.c"

   x86_64-w64-mingw32-g++ -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   x86_64-w64-mingw32-gcc -o ../bin/SLK_img2pix $sources $flags -lm -lSDL2 -O3 -g -fopenmp -I../HLH_gui2 -static-libgcc
   x86_64-w64-mingw32-g++ -o ../bin/SLK_img2pix $object_files -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options


elif [ $1 = "cmd" ]; then
   sources="$sources main_cmd.c gui.c"
   echo $sources

   gcc -o ../bin/SLK_img2pix_cmd $sources -Wall -Wextra -lm -O3 -s -flto=auto -Wstrict-aliasing=3 

else

   echo "unknown target, need either gui, cmd or video"

fi
