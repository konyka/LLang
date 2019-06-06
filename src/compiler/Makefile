TARGET = po
CC=gcc
OBJS = \
  lex.yy.o\
  y.tab.o\
  main.o\
  interface.o\
  create_parse_tree.o\
  fix_parse_tree.o\
  generate_code.o\
  string.o\
  wchar.o\
  util.o\
  error.o\
  error_message.o\
  ../povm/povm.o\
  ../common/common.o\
  ../memory/mem.o\
  ../debug/dbg.o

CFLAGS = -c -g -Wall -Wswitch-enum -ansi -pedantic -DDEBUG -DSHIFT_JIS_SOURCE

INCLUDES = \
  -I/usr/local/include\
  -I../include

$(TARGET):$(OBJS)
	cd ../povm; $(MAKE);
	cd ../common; $(MAKE);
	cd ../memory; $(MAKE);
	cd ../debug; $(MAKE);
	$(CC) $(OBJS) -o $@ -lm

clean:
	rm -f *.o lex.yy.c y.tab.c y.tab.h *~
y.tab.h : po.y
	bison --yacc -dv po.y
y.tab.c : po.y
	bison --yacc -dv po.y
lex.yy.c : po.l po.y y.tab.h
	flex po.l
y.tab.o: y.tab.c poc.h ../include/memory_public.h
	$(CC) -c -g $*.c $(INCLUDES)
lex.yy.o: lex.yy.c poc.h ../include/memory_public.h
	$(CC) -c -g $*.c $(INCLUDES)
.c.o:
	$(CC) $(CFLAGS) $*.c $(INCLUDES)
../memory/memory.o:
	cd ../memory; $(MAKE);
../debug/debug.o:
	cd ../debug; $(MAKE);
../common/common.o:
	cd ../common; $(MAKE);
../povm/povm.o:
	cd ../povm; $(MAKE);
############################################################
create_parse_tree.o: create_parse_tree.c ../include/memory_public.h ../include/debug_public.h poc.h \
 ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/common.h
error.o: error.c ../include/memory_public.h ../include/debug_public.h poc.h \
 ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/common.h
error_message.o: error_message.c poc.h ../include/memory_public.h \
 ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/common.h
fix_parse_tree.o: fix_parse_tree.c ../include/memory_public.h ../include/debug_public.h poc.h \
 ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/common.h
generate_code.o: generate_code.c ../include/memory_public.h ../include/debug_public.h poc.h \
 ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/common.h
interface.o: interface.c ../include/memory_public.h ../include/debug_public.h poc.h \
 ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/common.h
main.o: main.c ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/memory_public.h
string.o: string.c ../include/memory_public.h poc.h ../include/poc_public.h \
 ../include/povm_code.h ../include/povm.h ../include/common.h
util.o: util.c ../include/memory_public.h ../include/debug_public.h poc.h \
 ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/common.h
wchar.o: wchar.c ../include/debug_public.h poc.h ../include/memory_public.h \
 ../include/poc_public.h ../include/povm_code.h ../include/povm.h \
 ../include/common.h
