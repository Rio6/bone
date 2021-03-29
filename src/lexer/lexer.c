#include "lexer.h"
#include "utils/stream.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *TokenType_names[] = {
   "ERROR",
   "IDENT",
   "KEYWORD",
   "INT",
   "FLOAT",
   "CHAR",
   "STRING",
   "COMMENT",
   "BLOCK_L",
   "BLOCK_R",
   "PAREN_L",
   "PAREN_R",
   "LIST_L",
   "LIST_R",
   "DOT",
   "COMMA",
   "SEMICOLON",
   "COLON",
   "ASSIGN",
   "OP",
   "REF",
   "DEREF",
   "EOT",
};

Token *lex(Stream *stream) {
   Token head_token = {0};
   Token *last_token = &head_token;

   while(1) {
      int has_token = 0;
      char last_c = '\0';

      for(Lexer **lexer = lexers; *lexer != NULL; lexer++) {

         // advance until not a white space
         while(isspace(stream_getc(stream)));
         last_c = stream_ungetc(stream, 1);

         Token *token = (*lexer)(stream);

         if(token) {
            last_token->next = token;
            last_token = token;
            has_token = 1;
            break;
         }
      }

      if(!has_token) {
         char buff[128];
         snprintf(buff, sizeof(buff), "Unexpected character: %1$c (%1$X)\n", last_c);
         last_token->next = token_create(ERROR, buff);
         last_token = last_token->next;
      }

      if(last_token->type == ERROR || last_token->type == EOT) {
         break;
      }
   }

   return head_token.next;
}

Token *token_create(TokenType type, const char *value) {
   char *copied_value = NULL;
   if(value) {
      size_t len = strlen(value) + 1;
      copied_value = malloc(len);
      memcpy(copied_value, value, len);
   }

   return token_create_raw(type, copied_value);
}

Token *token_create_raw(TokenType type, char *value) {
   Token *token = calloc(1, sizeof(Token));
   token->type = type;
   token->value = value;
   return token;
}

void token_delete(Token *token) {
   if(!token) return;
   token_delete(token->next);
   free(token->value);
   free(token);
}

void token_dump(Token *token) {
   if(!token) return;
   printf("type: %s, value: %s\n", TokenType_names[token->type], token->value);
   token_dump(token->next);
}
