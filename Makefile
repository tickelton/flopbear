CFLAGS_COMMON = -std=c99 -pedantic -Wall 
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -O0
CFLAGS_RELEASE = $(CFLAGS_COMMON)  -O3

OBJS = flopbear
CC = gcc

SRCDIR = src
TESTDIR = test
UNITYDIR = $(TESTDIR)/unity
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

$(BUILDDIR)/$(UNITYDIR)/unity.o: $(UNITYDIR)/unity.c $(UNITYDIR)/unity.h $(UNITYDIR)/unity_internals.h
	@[ -d $(BUILDDIR)/$(UNITYDIR) ] || mkdir -p $(BUILDDIR)/$(UNITYDIR)
	$(CC) $(CFLAGS) -c -o $(BUILDDIR)/$(UNITYDIR)/unity.o $(UNITYDIR)/unity.c


$(BUILDDIR)/$(TESTDIR)/01_libflopbear: $(BUILDDIR)/libflopbear.o $(TESTDIR)/01_libflopbear.c $(BUILDDIR)/$(UNITYDIR)/unity.o
	@[ -d $(BUILDDIR)/$(TESTDIR) ] || mkdir -p $(BUILDDIR)/$(TESTDIR)
	$(CC) $(CFLAGS) -I$(UNITYDIR) -I$(SRCDIR) -o $(BUILDDIR)/$(TESTDIR)/01_libflopbear $(TESTDIR)/01_libflopbear.c $(BUILDDIR)/libflopbear.o $(BUILDDIR)/$(UNITYDIR)/unity.o

test: $(BUILDDIR)/$(TESTDIR)/01_libflopbear
	$(BUILDDIR)/$(TESTDIR)/01_libflopbear

clean:
	rm -rf $(BUILDDIR)/*

indent:
	@indent -di8 $(FILE)
