TARGET = povm.o
CC=gcc
CFLAGS = -c -g -Wall -Wswitch-enum -ansi -pedantic -DDEBUG
OBJS = execute.o \
  heap.o \
  native.o \
  wchar.o \
  util.o \
  error.o \
  error_message.o

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -I../include $*.c
error.o: error.c ../include/memory_public.h ../include/debug_public.h povm_pri.h \
 ../include/povm_code.h ../include/povm.h ../include/povm_dev.h \
 ../include/common.h
error_message.o: error_message.c povm_pri.h ../include/povm_code.h \
 ../include/povm.h ../include/povm_dev.h ../include/common.h
execute.o: execute.c ../include/memory_public.h ../include/debug_public.h povm_pri.h \
 ../include/povm_code.h ../include/povm.h ../include/povm_dev.h \
 ../include/common.h
heap.o: heap.c ../include/memory_public.h ../include/debug_public.h povm_pri.h \
 ../include/povm_code.h ../include/povm.h ../include/povm_dev.h \
 ../include/common.h
native.o: native.c ../include/memory_public.h ../include/debug_public.h povm_pri.h \
 ../include/povm_code.h ../include/povm.h ../include/povm_dev.h \
 ../include/common.h
util.o: util.c ../include/memory_public.h ../include/debug_public.h povm_pri.h \
 ../include/povm_code.h ../include/povm.h ../include/povm_dev.h \
 ../include/common.h
wchar.o: wchar.c ../include/debug_public.h ../include/memory_public.h povm_pri.h \
 ../include/povm_code.h ../include/povm.h ../include/povm_dev.h \
 ../include/common.h
