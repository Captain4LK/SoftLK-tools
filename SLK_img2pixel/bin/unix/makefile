CC = gcc
CFLAGS+=-lm -Wall

SLK_img2pix: main.o assets.o gui.o process.o sample.o utility.o tinyfiledialogs.o ULK_json.o
	gcc -o SLK_img2pix $^ -lSLK -lSLK_gui -lm -lSDL2 -Wall
	chmod +x SLK_img2pix 
	mv SLK_img2pix ../../../bin/

main.o: ../../src/main.c ../../src/assets.h ../../src/gui.h
	$(CC) -O3 -c $< $(CFLAGS) 

assets.o: ../../src/assets.c ../../src/assets.h
	$(CC) -O3 -c $< $(CFLAGS) 

gui.o: ../../src/gui.c ../../src/gui.h ../../src/assets.h ../../src/process.h ../../src/sample.h ../../src/utility.h
	$(CC) -O3 -c $< $(CFLAGS) 

process.o: ../../src/process.c ../../src/process.h ../../src/sample.h
	$(CC) -O3 -c $< $(CFLAGS) 

sample.o: ../../src/sample.c ../../src/sample.h ../../src/process.h
	$(CC) -O3 -c $< $(CFLAGS) 

utility.o: ../../src/utility.c ../../src/utility.h ../../src/process.h
	$(CC) -O3 -c $< $(CFLAGS) 

tinyfiledialogs.o: ../../../external/tinyfiledialogs.c
	$(CC) -O3 -c $< $(CFLAGS) 

ULK_json.o: ../../../external/UtilityLK/src/ULK_json.c
	$(CC) -O3 -c $< $(CFLAGS) 

clean:
	rm *.o ../../../bin/SLK_img2pix
