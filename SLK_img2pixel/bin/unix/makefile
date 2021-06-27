CC = gcc -flto -O3 -s
CFLAGS+=-lm -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -lSLK -lm -lSDL2 -Wall -lGL
DEPENDENCIES = assets.o gui.o utility.o image2pixel.o tinyfiledialogs.o gifenc.o gifdec.o

SLK_img2pix: main.o $(DEPENDENCIES)
	$(CC) -o SLK_img2pix $^ $(CFLAGS)
	chmod +x SLK_img2pix 
	mv SLK_img2pix ../../../bin/

SLK_img2pix_cmd: main_cmd.o $(DEPENDENCIES)
	$(CC) -o SLK_img2pix_cmd $^ $(CFLAGS)
	chmod +x SLK_img2pix_cmd 
	mv SLK_img2pix_cmd ../../../bin/

main.o: ../../src/main.c ../../src/gui.h
	$(CC) -c $< $(CFLAGS) 

assets.o: ../../src/assets.c ../../src/assets.h
	$(CC) -c $< $(CFLAGS) 

gui.o: ../../src/gui.c ../../src/gui.h ../../src/assets.h ../../src/image2pixel.h ../../src/utility.h
	$(CC) -c $< $(CFLAGS) 

utility.o: ../../src/utility.c ../../src/utility.h ../../src/image2pixel.h
	$(CC) -c $< $(CFLAGS) 

image2pixel.o:  ../../src/image2pixel.c ../../src/image2pixel.h ../../src/utility.h
	$(CC) -c $< $(CFLAGS) 

tinyfiledialogs.o: ../../../external/tinyfiledialogs.c
	$(CC) -c $< $(CFLAGS) 

gifenc.o: ../../../external/gifenc.c
	$(CC) -c $< $(CFLAGS) 

gifdec.o: ../../../external/gifdec.c
	$(CC) -c $< $(CFLAGS) 

clean:
	rm *.o ../../../bin/SLK_img2pix
