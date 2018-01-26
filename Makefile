CFLAGS_COMMON = -std=c99 -pedantic -Wall 
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -O0
CFLAGS_RELEASE = $(CFLAGS_COMMON)  -O3

OBJS = flopbear
CC = gcc

SRCDIR = src
BUILDDIR = build

all: release

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(OBJS)

release: CFLAGS = $(CFLAGS_RELEASE)
release: $(OBJS)

$(BUILDDIR)/main.o: src/main.c src/flopbear.h
	@[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $(BUILDDIR)/main.o $(SRCDIR)/main.c

$(BUILDDIR)/libflopbear.o: src/libflopbear.c src/flopbear.h
	@[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $(BUILDDIR)/libflopbear.o $(SRCDIR)/libflopbear.c

flopbear: $(BUILDDIR)/libflopbear.o $(BUILDDIR)/main.o
	@[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/flopbear $(BUILDDIR)/libflopbear.o $(BUILDDIR)/main.o

clean:
	rm -rf $(BUILDDIR)/*

indent:
	@indent -di8 $(FILE)
