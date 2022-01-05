#!/bin/sh

SOURCES="main.c utility_win.c assets.c gui.c image2pixel.c ../../external/gifenc.c ../../external/gifdec.c ../../external/nfd_win.cpp"

i686-w64-mingw32-gcc -o ../../bin/SLK_img2pix $SOURCES -L../../../SoftLK-lib/lib/windows_cross/ -I ../../../SoftLK-lib/include/ -luuid -lcomdlg32 -lole32 -lSLK -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++
