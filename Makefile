progs =	finger-auto-test hex-test

ldflags=-lowfat
cc=gcc
ggdb=-ggdb
cflags=-O3 -Wall -pedantic --std=gnu11 -fomit-frame-pointer $(ldflags)

targets=finger-auto-test hex-test

all: ${progs}

hex-test: hex-test.c sha1.o tools.o
	$(cc) $(ggdb) $(cflags) -o hex-test sha1.o tools.o hex-test.c

finger-auto-test: libchord.a
	$(cc) $(ggdb) $(cflags) -o finger-auto-test finger-auto-test.c libchord.a -lgmp

libchord.a: chord.o sha1.o tools.o finger.o
	ar cr libchord.a chord.o sha1.o tools.o finger.o
	ranlib libchord.a

sha1.o: sha1.c sha1.h
	$(cc) $(ggdb) $(cflags) -c sha1.c -o sha1.o 

chord.o: chord.c chord.h
	$(cc) $(ggdb) $(cflags) -c chord.c -o chord.o

finger.o: finger.c chord.h
	$(cc) $(ggdb) $(cflags) -c finger.c -o finger.o -lgmp

tools.o: tools.c chord.h
	$(cc) $(ggdb) $(cflags) -c -o tools.o tools.c

clean:
	rm -fr `cat TARGETS` *~
