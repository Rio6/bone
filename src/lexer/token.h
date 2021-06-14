#pragma once

typedef enum {
   TOKEN_ERROR,
   IDENT,
   KEYWORD,
   INT,
   FLOAT,
   CHAR,
   STRING,
   COMMENT,
   BLOCK_L,
   BLOCK_R,
   PAREN_L,
   PAREN_R,
   LIST_L,
   LIST_R,
   DOT,
   COMMA,
   SEMICOLON,
   COLON,
   OP,
   REF,
   DEREF,
   EOT,
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
