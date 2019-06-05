TARGET = dbg.o
CC=gcc
CFLAGS = -c -g -Wall -DDEBUG_NO_DEBUG
OBJS = debug.o
INCLUDES = -I../include

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $*.c
debug.o: debug.c ../include/memory_public.h debug.h ../include/debug_public.h

clean:
	rm *.o


