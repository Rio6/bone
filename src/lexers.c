#include "lexer.h"
#include "stream.h"

#include <stdlib.h>
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
      case '@':
         return token_create(DEREF, NULL);
      case '#':
         return token_create(REF, NULL);
      default:
         stream_ungetc(stream, 1);
         return NULL;
   }
}

Token *number_lexer(Stream *stream) {
   stream_begins(stream);

   TokenType type = INT;
   int digit_count = 0;

   while(isdigit(stream_getc(stream))) {
      digit_count++;
   }

   stream_ungetc(stream, 1);

   if(stream_getc(stream) == '.') {
      type = FLOAT;
      while(isdigit(stream_getc(stream))) {
         digit_count++;
      };
   }

   stream_ungetc(stream, 1);

   char c = stream_getc(stream);
   if(c == 'e' || c == 'E') {
      type = FLOAT;
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

   if(stream_peak(stream) != '\'') {
      return token_create(ERROR, "character literal has more than one character");
   }

   char buff[3];
   stream_ends(stream, buff, sizeof(buff));
   stream_getc(stream);

   return token_create(CHAR, buff);
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
            continue;
         }
         if(c == '"') {
            finished = 1;
            break;
         }
      }

      stream_ends(stream, mbuff + mbuff_size - batch_size - 1, batch_size + 1);

      if(finished) {
         break;
      }
   }

   // Remove trailing " and shrink the buffer
   mbuff_size = strlen(mbuff);
   mbuff[mbuff_size - 1] = '\0';
   mbuff = realloc(mbuff, mbuff_size);

   return token_create_raw(STRING, mbuff);
}

Token *comment_lexer(Stream *stream) {
   stream_begins(stream);

   if(stream_getc(stream) == '/') {
      char c = stream_getc(stream);
      if(c == '/') {
         while(c = stream_getc(stream), c != '\n' && c != EOF);
         return token_create(COMMENT, NULL);
      } else if(c == '*') {
         while(1) {
            while(c = stream_getc(stream), c != '*' && c != EOF);
            if(stream_getc(stream) == '/') {
               return token_create(COMMENT, NULL);
            }
         }
      }
   }

   stream_rollback(stream);
   return NULL;
}

Token *assign_lexer(Stream *stream) {
   stream_begins(stream);

   static const char not_assign[] = { '<', '>', '!' }; // <=, >=, != are not assignments. == is handled later
   char c = stream_getc(stream);
   for(size_t i = 0; i < sizeof(not_assign); i++) {
      if(not_assign[i] == c) {
         stream_rollback(stream);
         return NULL;
      }
   }

   if(c == '=' && stream_peak(stream) != '=') {
      return token_create(ASSIGN, "=");
   }

   stream_rollback(stream);
   return NULL;
}

Token *operator_lexer(Stream *stream) {
   static const char *one_symbs[] = { "+", "-", "*", "/", "%", "<", ">", "&", "|", "^" , "~", NULL };
   static const char *two_symbs[] = { "<<", ">>", "==", "!=", "<=", ">=", NULL };
   static const char **symbs[] = { one_symbs, two_symbs };

   const char *value = NULL;

   stream_begins(stream);

   char buff[3];
   for(size_t i = 0; i < sizeof(symbs) / sizeof(symbs[0]); i++) {
      stream_getc(stream);
      for(const char **symb = symbs[i]; *symb != NULL; symb++) {
         stream_ends(stream, buff, i+2);
         if(strcmp(*symb, buff) == 0) {
            value = *symb;
         }
      }
   }

   if(value != NULL) {
      return token_create(OP, value);
   }

   return NULL;
}
