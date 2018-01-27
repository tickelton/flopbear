TESTDIR = test
TSRCDIR = $(TESTDIR)
TBUILDDIR = $(BUILDDIR)/$(TESTDIR)
UNITYDIR = unity
USRCDIR = $(TSRCDIR)/$(UNITYDIR)
UBUILDDIR = $(TBUILDDIR)/$(UNITYDIR)

TESTSRCS = $(wildcard $(TSRCDIR)/*.c)
TESTOBJS = $(patsubst $(TSRCDIR)/%.c,$(TBUILDDIR)/%.o,$(TESTSRCS))
TESTBINS = $(patsubst $(TSRCDIR)/%.c,$(TBUILDDIR)/%,$(TESTSRCS))
UNITYOBJ = $(UBUILDDIR)/unity.o

$(UBUILDDIR):
	@[ -d $(UBUILDDIR) ] || mkdir -p $(UBUILDDIR)

$(UNITYOBJ): $(USRCDIR)/unity.c $(UBUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

tests: $(TESTBINS)

$(TBUILDDIR):
	@[ -d $(TBUILDDIR) ] || mkdir -p $(TBUILDDIR)

$(TESTOBJS): $(TBUILDDIR)/%.o : $(TSRCDIR)/%.c $(TBUILDDIR)
	$(CC) $(CFLAGS) -c -I$(USRCDIR) -I$(SRCDIR) -o $@ $<

$(TESTBINS): $(TBUILDDIR)/% : $(TBUILDDIR)/%.o $(BUILDDIR)/%.o $(UNITYOBJ)
	$(CC) $(CFLAGS) -o $@ $^

