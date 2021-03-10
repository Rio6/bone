#include "lexer.h"

Lexer *lexers[] = {
   comment_lexer,
   number_lexer,
   string_lexer,
   ident_lexer,
   sep_lexer,
   assign_lexer,
   operator_lexer,
   NULL,
};

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
   "ASSIGN",
   "OP",
   "REF",
   "DEREF",
};

