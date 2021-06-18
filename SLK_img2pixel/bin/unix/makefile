CC = gcc -flto -O3 -s
CFLAGS+=-lm -Wall

SLK_img2pix: main.o assets.o gui.o process.o sample.o utility.o palette.o quantization.o tinyfiledialogs.o ULK_json.o gifenc.o gifdec.o
	$(CC) -o SLK_img2pix $^ -lSLK -lm -lSDL2 -Wall -lGL $(CFLAGS)
	chmod +x SLK_img2pix 
	mv SLK_img2pix ../../../bin/

main.o: ../../src/main.c ../../src/gui.h
	$(CC) -c $< $(CFLAGS) 

assets.o: ../../src/assets.c ../../src/assets.h
	$(CC) -c $< $(CFLAGS) 

gui.o: ../../src/gui.c ../../src/gui.h ../../src/quantization.h ../../src/assets.h ../../src/process.h ../../src/sample.h ../../src/utility.h
	$(CC) -c $< $(CFLAGS) 

process.o: ../../src/process.c ../../src/process.h ../../src/sample.h ../../src/palette.h
	$(CC) -c $< $(CFLAGS) 

sample.o: ../../src/sample.c ../../src/sample.h ../../src/process.h
	$(CC) -c $< $(CFLAGS) 

utility.o: ../../src/utility.c ../../src/utility.h ../../src/process.h
	$(CC) -c $< $(CFLAGS) 

palette.o: ../../src/palette.c ../../src/palette.h ../../src/process.h
	$(CC) -c $< $(CFLAGS) 

quantization.o: ../../src/quantization.c ../../src/quantization.h
	$(CC) -c $< $(CFLAGS) 

tinyfiledialogs.o: ../../../external/tinyfiledialogs.c
	$(CC) -c $< $(CFLAGS) 

gifenc.o: ../../../external/gifenc.c
	$(CC) -c $< $(CFLAGS) 

gifdec.o: ../../../external/gifdec.c
	$(CC) -c $< $(CFLAGS) 

ULK_json.o: ../../../external/UtilityLK/src/ULK_json.c
	$(CC) -c $< $(CFLAGS) 

clean:
	rm *.o ../../../bin/SLK_img2pix
