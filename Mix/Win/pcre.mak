# Makefile for pcre

!include <win32.mak>

CC=cl
LD=link
CFLAGS=-O -nologo
LDFLAGS=
O=.obj

# variables
OBJ1 = maketables$(O) get$(O) study$(O) pcre$(O)

all:  dftables.exe chartables.c pcre.lib

maketables.obj: maketables.c internal.h pcre.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

get.obj: get.c internal.h pcre.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

study.obj: study.c internal.h pcre.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

pcre.obj: pcre.c internal.h pcre.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

dftables.obj: dftables.c maketables.c
	$(CC) -c $(CFLAGS) $*.c

chartables.c: dftables.exe
	dftables > chartables.c

pcre.lib: $(OBJ1)
	lib  -out:$@ $(OBJ1)
	copy pcre.lib ..

dftables.exe: dftables.obj
	$(LD) $(LDFLAGS) dftables.obj


clean:
	del *.obj
	del *.exe
	del *.lib
