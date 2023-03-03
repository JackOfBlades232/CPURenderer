SRCMODULES = src/camera.c src/debug.c src/geom.c src/image.c \
			 src/raytracer.c src/mathd.c src/png_save.c src/scene.c \
			 src/file_reader.c
OBJMODULES = $(SRCMODULES:.c=.o)
CC = gcc
CFLAGS = -g -Wall -I ~/Utils/headers/
LFLAGS = -lm -lpng -l:libtokeniz.a -L ~/Utils/lib/

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
