#!/bin/sh

gcc -c HLH_gui_all.c -Wall -Wextra -Wshadow -Wconversion -Wno-sign-conversion -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -std=c99 -pedantic -lSDL2 -ggdb

gcc test.c HLH_gui_all.o -lSDL2 -o test
