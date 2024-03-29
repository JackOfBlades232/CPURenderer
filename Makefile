SRCMODULES = $(shell find src -type f -name '*.c')
OBJMODULES = $(SRCMODULES:.c=.o)
CC = gcc
CFLAGS = -g -Wall -I ~/Utils/headers/ -O3
LFLAGS = -lm -lpng -l:libtokeniz.a -L ~/Utils/lib/

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

test: test.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

ifneq (clean, $(MAKECMDGOALS))
ifneq (cl_meta, $(MAKECMDGOALS))
-include deps.mk
endif
endif

deps.mk: $(SRCMODULES)
	$(CC) -MM $^ > $@

clean:
	rm -f *.o src/*.o test prog
