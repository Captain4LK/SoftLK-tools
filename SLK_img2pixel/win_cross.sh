#!/bin/sh

flags="-I../external/ -Wall -Wextra -Wno-unused -Wshadow"
sources="color.c tint.c blur.c kmeans.c image.c sharp.c sample.c hscb.c pcx.c gamma.c postprocess.c dither.c palette.c"
object_files="color.o tint.o blur.o kmeans.o image.o sharp.o sample.o hscb.o pcx.o gamma.o postprocess.o dither.o palette.o"
options="-O3 -flto -fopenmp -s"

CPP="/mnt/sdb1/apps/llvm-mingw-20240619-ucrt-ubuntu-20.04-x86_64/bin/x86_64-w64-mingw32-g++"
CC="/mnt/sdb1/apps/llvm-mingw-20240619-ucrt-ubuntu-20.04-x86_64/bin/x86_64-w64-mingw32-gcc"

if [ $# -lt 1 ]; then
   echo "unspecified target, need either gui, gui_hlh, cmd or video"
   exit 0 
fi

if [ $1 = "gui" ]; then
   sources="$sources main.c gui.c ../HLH_gui/HLH_gui_all.c win/util_win.c"
   object_files="$object_files main.o gui.o HLH_gui_all.o util_win.o nfd_win.o"

   rm *.a
   #ln -s `x86_64-w64-mingw32-g++ --print-file-name=libgomp.a`
   $CPP -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ -L. $options $flags
   $CC -c $sources $flags -lSDL2 -I../HLH_gui -static-libgcc -static-libstdc++ -L. $options
   $CPP -o ../bin/SLK_img2pix $object_files -static-libgcc -static-libstdc++ -L. -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -Wall -Wno-sign-compare -Wno-unused-parameter -mwindows $options $flags


elif [ $1 = "cmd" ]; then
   sources="$sources main_cmd.c ../HLH_gui/HLH_gui_all.c"

   rm *.a
   #ln -s `$CPP --print-file-name=libgomp.a`
   #x86_64-w64-mingw32-gcc -o ../bin/SLK_img2pix_cmd $sources $flags -I../HLH_gui -static-libgcc -static-libstdc++ -L. $options
   $CC -o ../bin/SLK_img2pix_cmd $sources $flags -std=c99 -Wall -Wextra -lm -O3 -s -static-libgcc -static-libstdc++ -flto=auto -lmingw32 -fopenmp -lSDL2main -lSDL2 -L. -I../HLH_gui

else

   echo "unknown target, need either gui, cmd or video"

fi
