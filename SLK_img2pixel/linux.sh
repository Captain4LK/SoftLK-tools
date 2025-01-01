#!/bin/sh
set -e

CFLAGS="-Wall -Wextra -Wshadow -std=c99 -Wno-sign-compare -Wconversion -Wno-sign-conversion -Wno-unused -O3 -g -fno-omit-frame-pointer -I../HLH_gui/ -I../3rd/ -I../ -lm -fopenmp -I."
printf "
.POSIX:
CC      = gcc
CFLAGS  = %s
LDFLAGS =
LDLIBS  = 
all: img2pix img2pix_cmd
" "$CFLAGS"

obj=""

function add_file
{
   gcc -MM -MT "${1%%.c}.o" "$1" $CFLAGS
   obj="$obj ${1%%.c}.o"
}

for src in $(find ./ -maxdepth 1 -name "*.c"); do
   add_file "$src"
done

for src in $(find ../shared -name "*.c"); do
   add_file "$src"
done

add_file "../external/tinyfiledialogs.c"
add_file "../HLH_gui/HLH_gui_all.c"
add_file "unix/util_unix.c"

echo "obj= $obj"

gcc -MM -MT "main/main.o" "main/main.c" $CFLAGS
gcc -MM -MT "main/main_cmd.o" "main/main_cmd.c" $CFLAGS

printf "img2pix_cmd: \$(obj) main/main_cmd.o\n\tgcc -o ../bin/SLK_img2pix_cmd $^ $CFLAGS -lSDL2\n"
printf "img2pix: \$(obj) main/main.o\n\tgcc -o ../bin/SLK_img2pix $^ $CFLAGS -lSDL2\nclean:\n\trm -f \$(obj) main/*.o ../bin/SLK_img2pix ../bin/SLK_img2pix_cmd \n"
