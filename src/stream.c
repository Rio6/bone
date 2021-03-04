#include "stream.h"

#include <string.h>

char stream_getc(Stream *s) {
   if(s->ahead > 0) {
      char c = s->buff[s->pos++];
      s->pos %= sizeof(s->buff);
      s->ahead--;
      return c;
   } else {
      char c = fgetc(s->stream);
      s->buff[s->pos++] = c;
      s->pos %= sizeof(s->buff);
      return c;
   }
}

char stream_peak(Stream *s) {
   if(s->ahead > 0) {
      return s->buff[s->pos];
   } else {
      char c = fgetc(s->stream);
      ungetc(c, s->stream);
      return c;
   }
}

char stream_ungetc(Stream *s, size_t count) {
   s->pos = (s->pos - count + sizeof(s->buff)) % sizeof(s->buff); // s->pos - count, but with modular math
   s->ahead += count;
   return s->buff[s->pos];
}

void stream_begins(Stream *s) {
   s->begin = s->pos;
}

char *stream_ends(Stream *s, char *buff, size_t size) {
   size_t count = 0;

   for(size_t i = s->begin; i != s->pos && count < size; i = (i+1) % sizeof(s->buff), count++) {
      buff[count] = s->buff[i];
   }

   if(count >= size) {
      buff[size-1] = '\0';
   } else {
      buff[count] = '\0';
   }

   return buff;
}

void stream_rollback(Stream *s) {
   ssize_t rollback_size = s->pos - s->begin;

   if(rollback_size > 0) {
      s->ahead += rollback_size;
   } else {
      s->ahead += sizeof(s->buff) + rollback_size;
   }

   s->pos = s->begin;
}
