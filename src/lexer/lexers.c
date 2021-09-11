#include "lexer.h"
#include "token.h"
#include "utils/stream.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char * const keywords[] = {
   "import", "export", "struct", "if", "else", "switch", "case", "while", "for", "return",
};

static const char * const operators[] = { // NOTE: sorted by length
   "+", "-", "*", "/", "%", "<", ">", "&", "|", "^" , "~", "=",
   "<<", ">>", "==", "!=", "<=", ">=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
   "<<=", ">>=",
};


static int isident(char c) {
   return isalnum(c) || c == '_';
}

Token *ident_lexer(Stream *stream) {
   stream_begins(stream);

   if(isdigit(stream_peek(stream))) {
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
         return token_create(T_KEYWORD, buff);
      }
   }

   return token_create(T_IDENT, buff);
}

Token *sep_lexer(Stream *stream) {
   switch(stream_getc(stream)) {
      case EOF:
         return token_create(T_EOT, NULL);
      case '.':
         return token_create(T_DOT, NULL);
      case ',':
         return token_create(T_COMMA, NULL);
      case ';':
         return token_create(T_SEMICOLON, NULL);
      case ':':
         return token_create(T_COLON, NULL);
      case '(':
         return token_create(T_PAREN_L, NULL);
      case ')':
         return token_create(T_PAREN_R, NULL);
      case '[':
         return token_create(T_BRACK_L, NULL);
      case ']':
         return token_create(T_BRACK_R, NULL);
      case '{':
         return token_create(T_BRACE_L, NULL);
      case '}':
         return token_create(T_BRACE_R, NULL);
      case '@':
         return token_create(T_DEREF, NULL);
      case '#':
         return token_create(T_REF, NULL);
      default:
         break;
   }

   stream_ungetc(stream, 1);
   return NULL;
}

Token *number_lexer(Stream *stream) {
   stream_begins(stream);

   TokenType type = T_INT;
   int digit_count = 0;

   while(isdigit(stream_getc(stream))) {
      digit_count++;
   }

   stream_ungetc(stream, 1);

   if(stream_getc(stream) == '.') {
      type = T_FLOAT;
      while(isdigit(stream_getc(stream))) {
         digit_count++;
      };
   }

   stream_ungetc(stream, 1);

   char c = stream_getc(stream);
   if(c == 'e' || c == 'E') {
      type = T_FLOAT;
      while(isdigit(stream_getc(stream)));
   }

   stream_ungetc(stream, 1);

   if(digit_count > 0) {
      char buff[sizeof(stream->buff)];
      stream_ends(stream, buff, sizeof(buff));
      return token_create(type, buff);
   }

   stream_rollback(stream);
   return NULL;
}

Token *char_lexer(Stream *stream) {
   if(stream_getc(stream) != '\'') {
      stream_ungetc(stream, 1);
      return NULL;
   }

   stream_begins(stream);

   char c = stream_getc(stream);
   if(c == '\\') {
      stream_getc(stream);
   }

   if(stream_peek(stream) != '\'') {
      return token_create(T_ERROR, "character literal can only have one character");
   }

   char buff[3];
   stream_ends(stream, buff, sizeof(buff));
   stream_getc(stream);

   return token_create(T_CHAR, buff);
}

Token *string_lexer(Stream *stream) {
   if(stream_getc(stream) != '"') {
      stream_ungetc(stream, 1);
      return NULL;
   }

   size_t batch_size = sizeof(stream->buff) - 1;
   size_t mbuff_size = 1; // set to 1 so all the allocations will have one extra byte for \0
   char *mbuff = NULL;

   for(size_t i = 0;; i++) {
      stream_begins(stream);

      mbuff_size += batch_size;
      mbuff = realloc(mbuff, mbuff_size);

      int finished = 0;
      for(int j = 0; j < batch_size; j++) {
         char c = stream_getc(stream);
         if(c == '\\') {
            stream_getc(stream); // TODO escape
         } else if(c == '"') {
            finished = 1;
            break;
         }
      }

      // Append to buff, shift left by one to overwrite previous \0 (thus have extra 1 byte for next \0)
      stream_ends(stream, mbuff + mbuff_size - batch_size - 1, batch_size + 1);

      if(finished) {
         while(isspace(stream_getc(stream)));
         if(stream_lastc(stream) == '"') {
            // Consecutive string literals, continuing
            mbuff_size = strlen(mbuff);
         } else {
            stream_ungetc(stream, 1);
            break;
         }
      }
   }

   // Remove trailing " and shrink the buffer
   mbuff_size = strlen(mbuff);
   mbuff[mbuff_size - 1] = '\0';
   mbuff = realloc(mbuff, mbuff_size);

   return token_create_raw(T_STRING, mbuff);
}

Token *comment_lexer(Stream *stream) {
   stream_begins(stream);

   if(stream_getc(stream) == '/') {
      char c = stream_getc(stream);
      if(c == '/') {
         while(c = stream_getc(stream), c != '\n' && c != EOF);
         return token_create(T_COMMENT, NULL);
      } else if(c == '*') {
         while(1) {
            while(c = stream_getc(stream), c != '*' && c != EOF);
            if(stream_getc(stream) == '/') {
               return token_create(T_COMMENT, NULL);
            }
         }
      }
   }

   stream_rollback(stream);
   return NULL;
}

Token *operator_lexer(Stream *stream) {
   stream_begins(stream);

   const char *value = NULL;
   int read = 0;

   for(size_t i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
      size_t op_len = strlen(operators[i]);
      while(read < op_len) {
         stream_getc(stream);
         read++;
      }

      char buff[4];
      stream_ends(stream, buff, i+2);

      if(strncmp(operators[i], buff, op_len) == 0) {
         if(op_len == read) {
            value = operators[i];
         }
      }
   }

   stream_ungetc(stream, read - strlen(value));

   if(value != NULL) {
      return token_create(T_OP, value);
   }

   return NULL;
}

Lexer *lexers[] = {
   comment_lexer,
   number_lexer,
   char_lexer,
   string_lexer,
   ident_lexer,
   sep_lexer,
   operator_lexer,
   NULL,
};
