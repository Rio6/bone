#pragma once

#include "lexer/token.h"

// Forward typedef
typedef struct ASTNode ASTNode;

typedef ASTNode *(*ASTParserFn)(ASTNode*);

extern ASTParserFn *parsers[];

ASTNode *parser_run(Token*);
ASTNode *parser_parse(ASTNode*, ASTParserFn*);

ASTNode *comment_parser(ASTNode*);
