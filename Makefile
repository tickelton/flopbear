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

flopbear: src/main.c src/flopbear.h
	@[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/flopbear $(SRCDIR)/main.c

clean:
	rm -rf $(BUILDDIR)/*

indent:
	@indent -di8 $(FILE)
