TARGET = mem.o
CC=gcc
CFLAGS = -c -g -DDEBUG -Wall
OBJS = memory.o storage.o

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
testp : $(OBJS) main.o
	$(CC) -o $@ $(OBJS) main.o
.c.o:
	$(CC) $(CFLAGS) -I../include $*.c
clean:
	rm *.o
main.o: main.c ../include/memory_public.h
memory.o: memory.c memory.h ../include/memory_public.h
storage.o: storage.c memory.h ../include/memory_public.h
