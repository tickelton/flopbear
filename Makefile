CFLAGS_DEBUG = -g -Wall -O0
CFLAGS_RELEASE = -Wall -O3

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
