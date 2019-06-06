TARGET = common.o
CC=gcc
CFLAGS = -c -g -Wall -Wswitch-enum -ansi -pedantic -DDEBUG 
OBJS = dispose.o \
  opcode.o \
  disassemble.o \
  wchar.o

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -I../include $*.c
disassemble.o: disassemble.c ../include/debug_public.h ../include/common.h \
 ../include/povm_code.h ../include/povm.h
dispose.o: dispose.c ../include/memory_public.h ../include/debug_public.h \
 ../include/povm_code.h ../include/povm.h
opcode.o: opcode.c ../include/common.h ../include/povm_code.h \
 ../include/povm.h
wchar.o: wchar.c ../include/debug_public.h ../include/memory_public.h ../include/povm.h
