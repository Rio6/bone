#pragma once

#include "lexer/token.h"

// Forward typedef
typedef struct ASTToken ASTToken;
typedef struct ASTNode ASTNode;

typedef ASTNode *(*ASTParserFn)(ASTToken*);

extern ASTParserFn *parsers[];

ASTNode *parser_run(Token*);

ASTNode *comment_parser(ASTToken*);
