#include "lexer.h"
#include "token.h"

#include "utils/array.h"
#include "utils/stream.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char * const keywords[] = {
   "import", "export", "struct", "if", "else", "switch", "case", "while", "for", "return",
};

typedef struct {
   const char *op; // owner
   TokenType type;
} Symbol;

static const Symbol symbols[] = { // NOTE: sorted by op's length
   {".", T_DOT},
   {",", T_COMMA},
   {";", T_SEMICOLON},
   {":", T_COLON},
   {"(", T_PAREN_L},
   {")", T_PAREN_R},
   {"[", T_BRACK_L},
   {"]", T_BRACK_R},
   {"{", T_BRACE_L},
   {"}", T_BRACE_R},
   {"@", T_DEREF},
   {"#", T_REF},
   {"+", T_PLUS},
   {"-", T_MINUS},
   {"*", T_MULT},
   {"/", T_DIV},
   {"%", T_MOD},
   {"<", T_LT},
   {">", T_GT},
   {"=", T_ASSIGN},
   {"&", T_AND},
   {"|", T_OR},
   {"^", T_XOR},
   {"~", T_NOT},
   {"<<", T_LSHIFT},
   {">>", T_RSHIFT},
   {"==", T_EQ},
   {"!=", T_NE},
   {"<=", T_LE},
   {">=", T_GE},
   {"+=", T_ASPLUS},
   {"-=", T_ASMINUS},
   {"*=", T_ASMULT},
   {"/=", T_ASDIV},
   {"%=", T_ASMOD},
   {"&=", T_ASAND},
   {"|=", T_ASOR},
   {"^=", T_ASXOR},
   {"<<=", T_ASLSHIFT},
   {">>=", T_ASRSHIFT},
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

   for(size_t i = 0; i < LEN(keywords); i++) {
      if(strcmp(buff, keywords[i]) == 0) {
         return token_create(T_KEYWORD, buff);
      }
   }

   return token_create(T_IDENT, buff);
}

Token *eof_lexer(Stream *stream) {
   if(stream_getc(stream) == EOF) {
      return token_create(T_EOT, NULL);
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

Token *symbol_lexer(Stream *stream) {
   stream_begins(stream);

   TokenType type = T_ERROR;

   int symb_len = 0;
   int read = 0;

   for(size_t i = 0; i < LEN(symbols); i++) {
      size_t op_len = strlen(symbols[i].op);
      while(read < op_len) {
         stream_getc(stream);
         read++;
      }

      char buff[4];
      stream_ends(stream, buff, op_len+1);

      if(strncmp(symbols[i].op, buff, op_len) == 0) {
         type = symbols[i].type;
         symb_len = op_len;
      }
   }

   stream_ungetc(stream, read - symb_len);

   if(type != T_ERROR) {
      return token_create(type, NULL);
   }

   stream_rollback(stream);

   return NULL;
}

Lexer *lexers[] = {
   eof_lexer,
   comment_lexer,
   number_lexer,
   char_lexer,
   string_lexer,
   ident_lexer,
   symbol_lexer,
   NULL,
};
