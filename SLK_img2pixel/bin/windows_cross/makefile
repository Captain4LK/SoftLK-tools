CC = i686-w64-mingw32-gcc -flto -O3 -s
CFLAGS+=-L../../../../SoftLK-lib/lib/windows_cross/ -I ../../../../SoftLK-lib/include/ -lcomdlg32 -lole32 -lSLK -lmingw32 -lSDL2main -lSDL2 -mwindows -Wall -Wno-sign-compare -Wno-unused-parameter
DEPENDENCIES = assets.o gui.o process.o sample.o utility_win.o palette.o quantization.o image2pixel.o tinyfiledialogs.o gifenc.o gifdec.o

SLK_img2pix: main.o $(DEPENDENCIES)
	$(CC) -o SLK_img2pix $^ $(CFLAGS)
	mv SLK_img2pix.exe ../../../bin/

SLK_img2pix_cmd: main_cmd.o $(DEPENDENCIES)
	$(CC) -o SLK_img2pix_cmd $^ $(CFLAGS)
	mv SLK_img2pix_cmd.exe ../../../bin/

main.o: ../../src/main.c ../../src/gui.h
	$(CC) -c $< $(CFLAGS) 

assets.o: ../../src/assets.c ../../src/assets.h
	$(CC) -c $< $(CFLAGS) 

gui.o: ../../src/gui.c ../../src/gui.h ../../src/assets.h ../../src/image2pixel.h ../../src/utility.h
	$(CC) -c $< $(CFLAGS) 

process.o: ../../src/process.c ../../src/process.h ../../src/sample.h ../../src/image2pixel.h ../../src/palette.h
	$(CC) -c $< $(CFLAGS) 

sample.o: ../../src/sample.c ../../src/sample.h ../../src/process.h
	$(CC) -c $< $(CFLAGS) 

utility_win.o: ../../src/utility_win.c ../../src/utility.h ../../src/process.h
	$(CC) -c $< $(CFLAGS) 

palette.o: ../../src/palette.c ../../src/palette.h ../../src/process.h
	$(CC) -c $< $(CFLAGS) 

quantization.o: ../../src/quantization.c ../../src/quantization.h
	$(CC) -c $< $(CFLAGS) 

image2pixel.o:  ../../src/image2pixel.c ../../src/image2pixel.h ../../src/quantization.h ../../src/process.h ../../src/sample.h ../../src/utility.h
	$(CC) -c $< $(CFLAGS) 

tinyfiledialogs.o: ../../../external/tinyfiledialogs.c
	$(CC) -c $< $(CFLAGS) 

gifenc.o: ../../../external/gifenc.c
	$(CC) -c $< $(CFLAGS) 

gifdec.o: ../../../external/gifdec.c
	$(CC) -c $< $(CFLAGS) 

clean:
	rm *.o ../../../bin/SLK_img2pix.exe
