CFLAGS_COMMON = -std=c99 -pedantic -Wall 
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -O0
CFLAGS_RELEASE = $(CFLAGS_COMMON)  -O3
CFLAGS = $(CFLAGS_DEBUG)

CC = gcc

SRCDIR = src
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
BIN = $(BUILDDIR)/flopbear
VARIANTS = debug release

all: release

include test/test.mk

debug: CFLAGS = $(CFLAGS_DEBUG)
release: CFLAGS = $(CFLAGS_RELEASE)
$(VARIANTS): $(BIN)

$(BUILDDIR):
	@[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)

$(OBJS): $(BUILDDIR)/%.o : $(SRCDIR)/%.c $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN) $^

clean:
	rm -rf $(BUILDDIR)/*

indent:
	@indent -di8 $(FILE)

