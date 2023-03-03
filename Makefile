SRCMODULES = src/camera.c src/debug.c src/geom.c src/image.c \
<<<<<<< HEAD
			 src/raytracer.c src/mathd.c src/png_save.c src/scene.c \
			 src/file_reader.c
OBJMODULES = $(SRCMODULES:.c=.o)
CC = gcc
CFLAGS = -g -Wall -I ~/Utils/headers/
LFLAGS = -lm -lpng -l:libtokeniz.a -L ~/Utils/lib/
=======
			 src/raytracer.c src/mathd.c src/png_save.c src/scene.c
OBJMODULES = $(SRCMODULES:.c=.o)
CC = gcc
CFLAGS = -g -Wall -I/opt/homebrew/include
LFLAGS = -L/opt/homebrew/lib -lm -lpng
>>>>>>> 084bf20e7328227c2ffd581d2c2decfb74081cfa

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

test: src/test/tests.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

deps.mk: $(SRCMODULES)
	$(CC) -MM $^ > $@

clean:
	rm -f *.o src/*.o src/test/*.o test* prog*
