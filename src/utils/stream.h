#pragma once

#include <stdio.h>

#define STREAM_BUFF 1024

typedef struct {
   FILE *stream;
   char buff[STREAM_BUFF];
   size_t pos, ahead, begin;
} Stream;

char stream_getc(Stream*);
char stream_peek(Stream*);
char stream_ungetc(Stream*, size_t);
void stream_begins(Stream*);
char *stream_ends(Stream*, char*, size_t);
void stream_rollback(Stream*);
