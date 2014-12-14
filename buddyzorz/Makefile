INSTALL=..
CC=gcc
CFLAGS=-I. -Wall -O3
#CFLAGS=-I. -Wall -g
LIBFLAGS=-I. -shared -fPIC
LIBS=-L. -lbuddy
LIB=$(INSTALL)/lib
INCLUDE=$(INSTALL)/include
LIBOBJS=buddy.o

all: libbuddy.so libbuddy.a buddy-test malloc-test buddy-unit-test

libs: libbuddy.so libbuddy.a

test:
	$(CC) buddy.c test.c -o $@

buddy.o: buddy.c
	$(CC) $(CFLAGS) -shared -fPIC -c -o $@ $?

libbuddy.so: $(LIBOBJS)
	$(LD) $(LIBFLAGS) -o $@ $?

libbuddy.a: $(LIBOBJS)
	$(AR)  rcv $@ $(LIBOBJS)
	ranlib $@

buddy-unit-test: buddy-unit-test.o
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

buddy-test: buddy-test.o
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

malloc-test: malloc-test.o
	$(CC) $(CFLAGS) -o $@ $?

install: libs
	install --mode=644  buddy.h $(INCLUDE)/
	install --mode=755 libbuddy.so $(LIB)/libbuddy.so
	install --mode=755 libbuddy.a $(LIB)/libbuddy.a

clean:
	/bin/rm -f *.o a.out buddy-test malloc-test libbuddy.* buddy-unit-test test
