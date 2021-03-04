#include "lexer.h"
#include "stream.h"

#include <string.h>
#include <ctype.h>

static const char * const keywords[] = {
   "import", "struct", "if", "else", "switch", "case", "while", "for", "return",
};

static int isident(char c) {
   return isalnum(c) || c == '_';
}

Token *ident_lexer(Stream *stream) {
   stream_begins(stream);

   if(isdigit(stream_peak(stream))) {
      return NULL;
   }

   while(isident(stream_getc(stream)));
   stream_ungetc(stream, 1);

   char buff[sizeof(stream->buff)];
   stream_ends(stream, buff, sizeof(buff));
   if(strlen(buff) == 0) {
      return NULL;
   }

   for(size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
      if(strcmp(buff, keywords[i]) == 0) {
         return token_create(KEYWORD, buff);
      }
   }

   return token_create(IDENT, buff);
}

Token *sep_lexer(Stream *stream) {
   switch(stream_getc(stream)) {
      case '.':
         return token_create(DOT, NULL);
      case ',':
         return token_create(COMMA, NULL);
      case ';':
         return token_create(SEMICOLON, NULL);
      case '(':
         return token_create(PAREN_L, NULL);
      case ')':
         return token_create(PAREN_R, NULL);
      case '[':
         return token_create(LIST_L, NULL);
      case ']':
         return token_create(LIST_R, NULL);
      case '{':
         return token_create(BLOCK_L, NULL);
      case '}':
         return token_create(BLOCK_R, NULL);
      default:
         stream_ungetc(stream, 1);
         return NULL;
   }
}

Token *number_lexer(Stream *stream) {
   return NULL;
}

Token *string_lexer(Stream *stream) {
   return NULL;
}

Token *comment_lexer(Stream *stream) {
   stream_begins(stream);

   if(stream_getc(stream) == '/') {
      if(stream_getc(stream) == '/') {
         char c;
         while(c = stream_getc(stream), c != '\n' && c != EOF);
         stream_ungetc(stream, 1);
         return token_create(COMMENT, NULL);
      } else if(stream_getc(stream) == '*') {
      }
   }

   stream_rollback(stream);
   return NULL;
}

Token *assign_lexer(Stream *stream) {
   stream_begins(stream);

   static const char not_assign[] = { '!', '<', '>' };

   char c = stream_getc(stream);
   for(size_t i = 0; i < sizeof(not_assign); i++) {
      if(not_assign[i] == c) {
         goto assign_lexer_end;
      }
   }

   if(c == '=') {
      if(stream_getc(stream) == '=') {
         char buff[sizeof(stream->buff)];
         return token_create(ASSIGN, stream_ends(stream, buff, sizeof(buff)));
      } else {
         stream_ungetc(stream, 1);
         return token_create(ASSIGN, "=");
      }
   }

assign_lexer_end:
   stream_rollback(stream);
   return NULL;
}
