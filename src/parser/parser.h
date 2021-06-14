#pragma once

#include "lexer/token.h"

// Forward typedef
typedef struct ASTToken ASTToken;
typedef struct ASTNode ASTNode;

typedef ASTNode *(ASTParser)(ASTToken*);

extern ASTParser **parsers[];

ASTNode *parser_run(Token*);

ASTNode *comment_parser(ASTToken*);
