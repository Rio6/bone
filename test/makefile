TESTSRCS:=$(wildcard *.c)
BINS:=$(patsubst %.c, %.bin, $(TESTSRCS))
OUTS:=$(patsubst %.c, %.out, $(TESTSRCS))
PRINTS:=$(patsubst %.c, %.print, $(TESTSRCS))
TESTS:=$(patsubst %.c, %, $(TESTSRCS))
RESETS:=$(patsubst %.c, %.reset, $(TESTSRCS))

SRCDIR=$(realpath ../src)
SRCS:=$(filter-out ../src/main.c, $(shell find $(SRCDIR) -name '*.c'))

CFLAGS+=-O0 -Wall -Wstrict-prototypes -g -ggdb -fsanitize=address -fsanitize=leak -fsanitize=undefined

.PHONY: all
all: $(TESTS)

.PHONY: print
print: $(PRINTS)

%: %.bin %.out
	./$< | diff -y --suppress-common-lines $*.out -

.SILENT: $(PRINTS)
%.print: %.bin
	./$<

.SECONDARY: $(OUTS)
%.out: | %.bin
	./$| > $@

.SECONDARY: $(BINS)
%.bin: %.c $(SRCS)
	$(CC) $(CFLAGS) -I$(SRCDIR) $(SRCS) $< -o $@

%.reset:
	$(RM) $*.out

.PHONY: clean
clean:
	$(RM) -r $(BINS)

.PHONY: reset
reset: $(RESETS)
