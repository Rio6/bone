TESTSRCS:=$(wildcard *.c)
BINS:=$(patsubst %.c, %.bin, $(TESTSRCS))
OUTS:=$(patsubst %.c, %.out, $(TESTSRCS))
TMPS:=$(patsubst %.c, %.tmp, $(TESTSRCS))
PRINTS:=$(patsubst %.c, %.print, $(TESTSRCS))
TESTS:=$(patsubst %.c, %, $(TESTSRCS))

SRCDIR=$(realpath ../src)
SRCS:=$(filter-out ../src/main.c, $(shell find $(SRCDIR) -name '*.c'))

CFLAGS+=-O0 -Wall -g -ggdb -fsanitize=address -fsanitize=leak -fsanitize=undefined

.PHONY: all
all: $(TESTS)

.PHONY: generate
generate: clean-outs $(OUTS)

.PHONY: print
print: $(PRINTS)

%: %.bin %.out
	./$< > $*.tmp
	diff $*.out $*.tmp

.SILENT: $(PRINTS)
%.print: %.bin
	./$<

.SECONDARY: $(OUTS)
%.out: | %.bin
	./$| > $@

.SECONDARY: $(BINS)
%.bin: %.c $(SRCS)
	$(CC) $(CFLAGS) -I$(SRCDIR) $(SRCS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(BINS) $(TMPS)

.PHONY: clean-outs
clean-outs:
	rm -rf $(OUTS)
