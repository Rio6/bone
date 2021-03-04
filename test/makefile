TESTSRCS:=$(wildcard *.c)
TESTBINS:=$(patsubst %.c, %.bin, $(TESTSRCS))
TESTS:=$(patsubst %.c, %.test, $(TESTSRCS))

SRCDIR=$(realpath ../src)
SRCS:=$(filter-out ../src/main.c, $(wildcard $(SRCDIR)/**.c))

CFLAGS:=-O2 -Wall

.PHONY: all
all: $(TESTS)

.SILENT: $(TESTS)
%.test: %.bin
	echo
	echo " ==== $< ===="
	./$<
	echo

.SECONDARY: $(TESTBINS)
%.bin: %.c $(SRCS)
	$(CC) $(CFLAGS) -I$(SRCDIR) $(SRCS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(TESTBINS)
