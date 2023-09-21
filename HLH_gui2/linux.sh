#!/bin/sh

cflags="-Wall -Wextra -Wshadow -Wconversion -Wno-sign-conversion -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -std=c99 -pedantic -ggdb -fsanitize=undefined"
gcc -c HLH_gui_all.c $cflags

gcc test.c HLH_gui_all.o -o test $cflags -lSDL2
