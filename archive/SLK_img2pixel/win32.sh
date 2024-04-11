#!/bin/sh

sources="utility_win.c assets.c image2pixel.c"
object_files="utility_win.o assets.o image2pixel.o nfd_win.o"
options="-O3 -flto -s"

if [ $# -lt 1 ]; then
   echo "unspecified target, need either gui, gui_win, gui_hlh, cmd, cmd_win or video"
   exit 0
fi

if [ $1 = "gui" ]; then
   sources="$sources gui.c main.c"
   object_files="$object_files gui.o main.o"

   i686-w64-mingw32-g++ -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-gcc -c $sources -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-g++ -o ../bin/SLK_img2pix $object_files -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options

elif [ $1 = "gui_hlh" ]; then

   sources="$sources main_hlh.c ../HLH_gui/HLH_gui_all.c"
   object_files="$object_files main_hlh.o HLH_gui_all.o"

   i686-w64-mingw32-g++ -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-gcc -c $sources -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-g++ -o ../bin/SLK_img2pix $object_files -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options

elif [ $1 = "gui_win" ]; then

   sources="$sources gui.c main.c"
   object_files="$object_files gui.o main.o"
   options="$options -DLIBSLK_BACKEND=1 -luser32 -lgdi32 -lopengl32 -lgdiplus"

   i686-w64-mingw32-g++ -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-gcc -c $sources -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-g++ -o ../bin/SLK_img2pix $object_files -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options

elif [ $1 = "cmd" ]; then
   sources="$sources main_cmd.c"
   object_files="$object_files main_cmd.o"

   i686-w64-mingw32-g++ -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-gcc -c $sources -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-g++ -o ../bin/SLK_img2pix_cmd $object_files -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options

elif [ $1 = "cmd_win" ]; then
   sources="$sources main_cmd.c"
   object_files="$object_files main_cmd.o"
   options="$options -DLIBSLK_BACKEND=1 -luser32 -lgdi32 -lopengl32 -lgdiplus"

   i686-w64-mingw32-g++ -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-gcc -c $sources -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-g++ -o ../bin/SLK_img2pix_cmd $object_files -luuid -lcomdlg32 -lole32 -lmingw32 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options

elif [ $1 = "video" ]; then
   sources="$sources main_video.c"
   object_files="$object_files main_video.o"

   i686-w64-mingw32-g++ -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-gcc -c $sources -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options
   i686-w64-mingw32-g++ -o ../bin/SLK_img2pix_video $object_files -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ $options

else

   echo "unknown target, need either gui, cmd or video"

fi
