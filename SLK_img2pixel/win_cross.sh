
#!/bin/sh
set -e

CXX="/mnt/sdb1/apps/llvm-mingw-20240619-ucrt-ubuntu-20.04-x86_64/bin/x86_64-w64-mingw32-g++"
CC="/mnt/sdb1/apps/llvm-mingw-20240619-ucrt-ubuntu-20.04-x86_64/bin/x86_64-w64-mingw32-gcc"
CFLAGS="-Wall -Wextra -Wshadow -std=c99 -Wno-sign-compare -Wconversion -Wno-sign-conversion -Wno-unused -O3 -g -fno-omit-frame-pointer -I../HLH_gui/ -I../3rd/ -I../ -lm -fopenmp -I. -L."
CPPFLAGS="-Wall -Wextra -Wshadow -Wno-sign-compare -Wconversion -Wno-sign-conversion -Wno-unused -O3 -g -fno-omit-frame-pointer -I../HLH_gui/ -I../3rd/ -I../ -lm -fopenmp -I. -luuid -lcomdlg32 -lole32 -lmingw32 -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++ -L. -lSDL2main -lSDL2 -mwindows"
LDFLAGS="-static-libgcc -static-libstdc++ -L. -luuid -lcomdlg32 -lole32 -lmingw32 -lSDL2main -lSDL2 -Wall -Wno-sign-compare -Wno-unused-parameter -mwindows"
printf "
.POSIX:
CC      = $CC
CXX     = $CXX
CFLAGS  = %s
CPPFLAGS = $CPPFLAGS
LDFLAGS = $LDFLAGS
LDLIBS  = 
all: img2pix img2pix_cmd
" "$CFLAGS"

obj=""

function add_file
{
   $CC -MM -MT "${1%%.c}.o" "$1" $CFLAGS
   obj="$obj ${1%%.c}.o"
}

function add_file_cpp
{
   $CXX -MM -MT "${1%%.c}.o" "$1" $CPPFLAGS
   obj="$obj ${1%%.cpp}.o"
}

for src in $(find ./ -maxdepth 1 -name "*.c"); do
   add_file "$src"
done

for src in $(find ../shared -name "*.c"); do
   add_file "$src"
done

#add_file "../external/tinyfiledialogs.c"
add_file "../HLH_gui/HLH_gui_all.c"
add_file "win/util_win.c"
add_file_cpp "../external/nfd_win.cpp"

echo "obj= $obj"

$CC -MM -MT "main/main.o" "main/main.c" $CFLAGS
$CC -MM -MT "main/main_cmd.o" "main/main_cmd.c" $CFLAGS

printf "img2pix_cmd: \$(obj) main/main_cmd.o\n\t$CXX -o ../bin/SLK_img2pix_cmd $^ $CFLAGS $LDFLAGS\n"
printf "img2pix: \$(obj) main/main.o\n\t$CXX -o ../bin/SLK_img2pix $^ $CPPFLAGS $LDFLAGS\nclean:\n\trm -f \$(obj) main/*.o ../bin/SLK_img2pix.exe ../bin/SLK_img2pix_cmd.exe \n"
