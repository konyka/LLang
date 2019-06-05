TARGET = mem.o
CC=gcc
CFLAGS = -c -g -DDEBUG -Wall
OBJS = memory.o storage.o

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
test: $(OBJS) main.o
	$(CC) -o $@ $(OBJS) main.o
.c.o:
	$(CC) $(CFLAGS) -I../include $*.c
clean:
	rm *.o test
main.o: main.c ../include/memory_public.h
memory.o: memory.c memory.h ../include/memory_public.h
storage.o: storage.c memory.h ../include/memory_public.h
