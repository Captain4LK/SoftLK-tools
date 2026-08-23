#!/bin/sh
set -e

# Assumes libomp + SDL2 are installed via Homebrew. If installed elsewhere, export CPPFLAGS/LDFLAGS to add custom paths.
CC="${CC:-cc}"
CPPFLAGS="-I/opt/homebrew/opt/libomp/include -I/opt/homebrew/include -D_THREAD_SAFE ${CPPFLAGS:-}"
LDFLAGS="-L/opt/homebrew/opt/libomp/lib -L/opt/homebrew/lib ${LDFLAGS:-}"
CFLAGS="-Wall -Wextra -Wshadow -std=c99 -Wno-sign-compare -Wconversion -Wno-sign-conversion -Wno-unused -O3 -g -fno-omit-frame-pointer -DNFD_MACOS_ALLOWEDCONTENTTYPES=0 -I../HLH_gui/ -I../3rd/ -I../ -lm -I. -Xpreprocessor -fopenmp -DLUA_USE_POSIX $CPPFLAGS"
LDLIBS="-lomp -lSDL2 -framework AppKit -framework Foundation"
printf "
.POSIX:
CC      = $CC
CFLAGS  = %s
LDFLAGS = $LDFLAGS
LDLIBS  = $LDLIBS
all: img2pix img2pix_cmd app
" "$CFLAGS"

obj=""

function add_file
{
	$CC -MM -MT "${1%%.c}.o" "$1" $CFLAGS
	obj="$obj ${1%%.c}.o"
}

function add_file_objc
{
	$CC -MM -MT "${1%%.m}.o" "$1" $CFLAGS
	obj="$obj ${1%%.m}.o"
}

for src in $(find ./ -maxdepth 1 -name "*.c"); do
	add_file "$src"
done

for src in $(find ../shared -name "*.c"); do
	add_file "$src"
done

for src in $(find ../external/lua -maxdepth 1 -name "*.c"); do
	add_file "$src"
done

add_file_objc "../external/nfd_cocoa.m"
add_file "../HLH_gui/HLH_gui_all.c"
add_file "mac/util_mac.c"
add_file "mac/sdl_env.c"

echo "obj= $obj"

$CC -MM -MT "main/main.o" "main/main.c" $CFLAGS
$CC -MM -MT "main/main_cmd.o" "main/main_cmd.c" $CFLAGS

printf "img2pix_cmd: \$(obj) main/main_cmd.o\n\t$CC -o ../bin/SLK_img2pix_cmd $^ $CFLAGS $LDFLAGS $LDLIBS\n"
printf "img2pix: \$(obj) main/main.o\n\t$CC -o ../bin/SLK_img2pix $^ $CFLAGS $LDFLAGS $LDLIBS\n"
printf "app: img2pix\n\tmkdir -p ../bin/SLK_img2pix.app/Contents/MacOS ../bin/SLK_img2pix.app/Contents/Resources\n\tcp ../bin/SLK_img2pix ../bin/SLK_img2pix.app/Contents/MacOS/SLK_img2pix\n\tchmod +x ../bin/SLK_img2pix.app/Contents/MacOS/SLK_img2pix\n\tcp mac/Info.plist ../bin/SLK_img2pix.app/Contents/Info.plist\n\tcp mac/icon.icns ../bin/SLK_img2pix.app/Contents/Resources/SLK_img2pix.icns\n"
printf "clean:\n\trm -f \$(obj) main/*.o ../bin/SLK_img2pix ../bin/SLK_img2pix_cmd\n\trm -rf ../bin/SLK_img2pix.app\n"
