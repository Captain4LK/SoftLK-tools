CC = gcc
CFLAGS+=-lm -Wall

SLK_img2pix: main.o assets.o gui.o process.o tinyfiledialogs.o
	gcc -o SLK_img2pix $^ -lSLK -lm -lSDL2 -Wall
	chmod +x SLK_img2pix 
	mv SLK_img2pix ../

main.o: ../../src/main.c ../../src/assets.h ../../src/gui.h
	$(CC) -O3 -c $< $(CFLAGS) 

assets.o: ../../src/assets.c ../../src/assets.h
	$(CC) -O3 -c $< $(CFLAGS) 

gui.o: ../../src/gui.c ../../src/gui.h ../../src/assets.h ../../src/process.h
	$(CC) -O3 -c $< $(CFLAGS) 

process.o: ../../src/process.c ../../src/process.h
	$(CC) -O3 -c $< $(CFLAGS) 

tinyfiledialogs.o: ../../external/tinyfiledialogs.c
	$(CC) -O3 -c $< $(CFLAGS) 

clean:
	rm *.o ../SLK_img2pix
