#!/bin/sh
set -e

# -isystem to not list all RvR headers
CFLAGS="-Wall -Wextra -Wshadow -std=c99 -Wno-sign-compare -Wconversion -Wno-sign-conversion -Wno-unused -O3 -g -fno-omit-frame-pointer -I../HLH_gui/ -lm"
printf "
.POSIX:
CC      = gcc
CFLAGS  = %s
LDFLAGS =
LDLIBS  = 
all: pixtexed
" "$CFLAGS"

obj=""

for src in $(find ./ -name "*.c"); do
   gcc -MM -MT "${src%%.c}.o" "$src" $CFLAGS
   obj="$obj ${src%%.c}.o"
done

gcc -MM -MT "../HLH_gui/HLH_gui_all.o" "../HLH_gui/HLH_gui_all.c" $CFLAGS
obj="$obj ../HLH_gui/HLH_gui_all.o"

echo "obj= $obj"

printf "pixtexed: \$(obj)\n\tgcc -o ../bin/pixtexed $^ $CFLAGS -lSDL2\nclean:\n\trm -f \$(obj) ../bin/port\n"

