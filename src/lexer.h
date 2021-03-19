#pragma once

#include "stream.h"

typedef enum {
   ERROR,
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
   ASSIGN,
   OP,
   REF,
   DEREF,
} TokenType;

typedef struct Token {
   TokenType type;
   char *value; // owner
   struct Token *next; // owner
} Token;

typedef Token *(Lexer)(Stream *stream);

extern Lexer *lexers[];
extern const char *TokenType_names[];

Token *lex(Stream *stream);

Token *token_create(TokenType type, const char *value);
Token *token_create_raw(TokenType type, char *value);
void token_delete(Token*);
void token_dump(Token*);

char *token_type_name(TokenType);

Token *ident_lexer(Stream*);
Token *sep_lexer(Stream*);
Token *number_lexer(Stream*);
Token *char_lexer(Stream*);
Token *string_lexer(Stream*);
Token *comment_lexer(Stream*);
Token *assign_lexer(Stream*);
Token *operator_lexer(Stream*);
