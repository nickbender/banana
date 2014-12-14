
CC=gcc
INSTALL=.
INCLUDE=$(INSTALL)/include -I.
LIBS=$(INSTALL)/lib -lreadline -lncurses
LFLAGS=-L$(LIBS) -Wl,-rpath=$(LIBS) -lmylib
CFLAGS=-Wall -g  -I$(INCLUDE) $(GITDEV)
PROGS=library loop mydash

all: $(PROGS) dox

loop: loop.o
	$(CC) $(CFLAGS) -o $@  $<

mydash: mydash.o error.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

library:
	cd libsrc; make install; cd ..
	cd buddyzorz; make install; cd ..

dox:
	echo "Generating documentation using doxygen..."
	doxygen doxygen-config > doxygen.log
	echo "Use konqueror docs/html/index.html to see docs (or any other browser)"

remake: clean all

clean:
	cd libsrc; make clean; cd ..
	cd buddyzorz; make clean; cd ..
	/bin/rm -f *.o $(PROGS) a.out core  *.log
	/bin/rm -fr docs
	cd include; rm -fr *.h
	cd lib; rm -rf libmylib.* libbuddy.*
