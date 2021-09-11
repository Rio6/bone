#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *TokenType_names[] = {
   "T_ERROR",
   "T_IDENT",
   "T_KEYWORD",
   "T_INT",
   "T_FLOAT",
   "T_CHAR",
   "T_STRING",
   "T_COMMENT",
   "T_BRACE_L",
   "T_BRACE_R",
   "T_PAREN_L",
   "T_PAREN_R",
   "T_BRACK_L",
   "T_BRACK_R",
   "T_DOT",
   "T_COMMA",
   "T_SEMICOLON",
   "T_COLON",
   "T_OP",
   "T_REF",
   "T_DEREF",
   "T_EOT",
};

Token *token_create(TokenType type, const char *value) {
   char *copied_value = NULL;
   if(value) {
      size_t len = strlen(value) + 1;
      copied_value = malloc(len);
      strcpy(copied_value, value);
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
   token_print(token);
   token_dump(token->next);
}

void token_print(Token *token) {
   printf("type: %s, value: %s\n", TokenType_names[token->type], token->value);
}
