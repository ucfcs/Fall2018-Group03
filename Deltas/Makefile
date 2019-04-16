# Makefile for lzss
CC = gcc
LD = gcc
CFLAGS = -O2 -Wall -ansi -c
LDFLAGS = -O2 -o

# Treat NT and non-NT windows the same
ifeq ($(OS),Windows_NT)
	OS = Windows
endif

ifeq ($(OS),Windows)
	EXE = .exe
	DEL = del
else	#assume Linux/Unix
	EXE =
	DEL = rm
endif

all:		lzss$(EXE) lzlist$(EXE) lzhash$(EXE)

lzss$(EXE):	lzss.o getopt.o
		$(LD) $^ $(LDFLAGS) $@

lzss.o:		lzss.c getopt.h
		$(CC) $(CFLAGS) $<

lzlist$(EXE):	lzlist.o getopt.o
		$(LD) $^ $(LDFLAGS) $@

lzlist.o:	lzlist.c getopt.h
		$(CC) $(CFLAGS) $<

lzhash$(EXE):	lzhash.o getopt.o
		$(LD) $^ $(LDFLAGS) $@

lzhash.o:	lzhash.c getopt.h
		$(CC) $(CFLAGS) $<

getopt.o:	getopt.c getopt.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) lzss$(EXE)
		$(DEL) lzlist$(EXE)
		$(DEL) lzhash$(EXE)
