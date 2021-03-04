SRCDIR:=src
TESTDIR:=test
BUILDDIR:=build

CFLAGS:=-O2 -Wall
LIBS:=
SRCS:=$(wildcard $(SRCDIR)/*.c)
HDRS:=$(wildcard $(SRCDIR)/*.h)
OBJS:= $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))
TGT:=name
TESTS:= $(patsubst $(TESTDIR)/%.c, $(BUILDDIR)/test/%, $(wildcard $(TESTDIR)/*.c))

.PHONY: all
all: $(BUILDDIR) $(TGT)

.PHONY: test
test:
	$(MAKE) -C $(TESTDIR)

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $(TGT)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

.PHONY: clean
clean:
	$(RM) -r build
	$(MAKE) -C $(TESTDIR) clean

.PHONY: depend
depend: .depend

.depend: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -MM $^ | sed 's|[a-zA-Z0-9_-]*\.o|$(BUILDDIR)/&|' > .depend

include .depend
