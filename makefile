SRCDIR:=src
TESTDIR:=test
BUILDDIR:=build

CFLAGS+=-O2 -Wall -Wstrict-prototypes
INCLUDES=-I $(SRCDIR)
LIBS:=

SRCS:=$(shell find $(SRCDIR) -name '*.c')
HDRS:=$(shell find $(SRCDIR) -name '*.h')
OBJS:= $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))
TGT:=$(BUILDDIR)/bone

.PHONY: all
all: $(TGT)

.PHONY: test
test:
	$(MAKE) -C $(TESTDIR)

.PHONY: print-test
print-test:
	$(MAKE) -C $(TESTDIR) print

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) $(OBJS) -o $(TGT)

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
