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
   T_REF,
   T_DEREF,
   T_PLUS,
   T_MINUS,
   T_MULT,
   T_DIV,
   T_MOD,
   T_LT,
   T_GT,
   T_AND,
   T_OR,
   T_XOR,
   T_NOT,
   T_ASSIGN,
   T_LSHIFT,
   T_RSHIFT,
   T_EQ,
   T_NE,
   T_LE,
   T_GE,
   T_ASPLUS,
   T_ASMINUS,
   T_ASMULT,
   T_ASDIV,
   T_ASMOD,
   T_ASAND,
   T_ASOR,
   T_ASXOR,
   T_ASLSHIFT,
   T_ASRSHIFT,
   T_EOT,
} TokenType;
extern const char * const TokenType_names[];

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
