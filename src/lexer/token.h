#pragma once

typedef enum {
   T_ERROR,
   T_IDENT,
   T_KEYWORD,
   T_INT,
   T_FLOAT,
   T_CHAR,
   T_STRING,
   T_COMMENT,
   T_BRACE_L,
   T_BRACE_R,
   T_PAREN_L,
   T_PAREN_R,
   T_BRACK_L,
   T_BRACK_R,
   T_DOT,
   T_COMMA,
   T_SEMICOLON,
   T_COLON,
   T_OP,
   T_REF,
   T_DEREF,
   T_EOT,
} TokenType;
extern const char *TokenType_names[];

typedef struct Token {
   TokenType type;
   char *value; // owner
   struct Token *next; // owner
} Token;

Token *token_create(TokenType type, const char *value);
Token *token_create_raw(TokenType type, char *value);
void token_delete(Token*);
void token_dump(Token*);
void token_print(Token*);
