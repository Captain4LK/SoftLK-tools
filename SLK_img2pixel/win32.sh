#!/bin/sh

SOURCES="main.c utility_win.c assets.c gui.c image2pixel.c"

i686-w64-mingw32-g++ -c ../external/nfd_win.cpp -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++
i686-w64-mingw32-gcc -c $SOURCES -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++
i686-w64-mingw32-g++ -o ../bin/SLK_img2pix *.o -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++
