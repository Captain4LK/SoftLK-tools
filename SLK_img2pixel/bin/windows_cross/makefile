CC = i686-w64-mingw32-gcc -O3 -s -flto=auto
CXX = i686-w64-mingw32-g++ -O3 -s -flto=auto
CFLAGS+=-L../../../../SoftLK-lib/lib/windows_cross/ -I ../../../../SoftLK-lib/include/ -luuid -lcomdlg32 -lole32 -lSLK -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++
CFLAGS_CMD+=-L../../../../SoftLK-lib/lib/windows_cross/ -I ../../../../SoftLK-lib/include/ -luuid -lcomdlg32 -lole32 -lSLK -lmingw32 -lSDL2main -lSDL2 -Wall -Wno-sign-compare -Wno-unused-parameter -static-libgcc -static-libstdc++
DEPENDENCIES = assets.o gui.o utility_win.o image2pixel.o nfd_win.o gifenc.o gifdec.o

SLK_img2pix: main.o $(DEPENDENCIES)
	$(CXX) -o SLK_img2pix $^ $(CFLAGS)
	mv SLK_img2pix.exe ../../../bin/

SLK_img2pix_cmd: main_cmd.o $(DEPENDENCIES)
	$(CC) -o SLK_img2pix_cmd $^ $(CFLAGS_CMD)
	mv SLK_img2pix_cmd.exe ../../../bin/

main.o: ../../src/main.c ../../src/gui.h ../../src/utility.h
	$(CC) -c $< $(CFLAGS) 

main_cmd.o: ../../src/main_cmd.c ../../src/image2pixel.h ../../src/assets.h
	$(CC) -c $< $(CFLAGS_CMD) 

assets.o: ../../src/assets.c ../../src/assets.h
	$(CC) -c $< $(CFLAGS) 

gui.o: ../../src/gui.c ../../src/gui.h ../../src/assets.h ../../src/image2pixel.h ../../src/utility.h
	$(CC) -c $< $(CFLAGS) 

utility_win.o: ../../src/utility_win.c ../../src/utility.h ../../src/image2pixel.h
	$(CC) -c $< $(CFLAGS) 

image2pixel.o:  ../../src/image2pixel.c ../../src/image2pixel.h ../../src/utility.h
	$(CC) -c $< $(CFLAGS) 

#tinyfiledialogs.o: ../../../external/tinyfiledialogs.c
	#$(CC) -c $< $(CFLAGS) 

nfd_win.o: ../../src/nfd_win.cpp
	$(CXX) -c $< $(CFLAGS) 

gifenc.o: ../../../external/gifenc.c
	$(CC) -c $< $(CFLAGS) 

gifdec.o: ../../../external/gifdec.c
	$(CC) -c $< $(CFLAGS) 

clean:
	rm *.o ../../../bin/SLK_img2pix.exe
