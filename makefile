SRCDIR:=src
TESTDIR:=test
BUILDDIR:=build

CFLAGS:=-O2 -Wall
LIBS:=
SRCS:=$(shell find $(SRCDIR) -name '*.c')
HDRS:=$(wildcard find $(SRCIR) -name '*.h')
OBJS:= $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))
TGT:=$(BUILDDIR)/bone
TESTS:= $(patsubst $(TESTDIR)/%.c, $(BUILDDIR)/test/%, $(wildcard $(TESTDIR)/*.c))

.PHONY: all
all: $(TGT)

.PHONY: test
test:
	$(MAKE) -C $(TESTDIR)

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $(TGT)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(SRCDIR) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r build
	$(MAKE) -C $(TESTDIR) clean

.PHONY: depend
depend: .depend

.depend: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -I$(SRCDIR) -MM $^ | sed 's|[a-zA-Z0-9_-]*\.o|$(BUILDDIR)/&|' > .depend

include .depend
