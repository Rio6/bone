#pragma once

#include "lexer/token.h"

// Forward typedef
typedef struct ASTNode ASTNode;

typedef ASTNode *(*ASTParserFn)(ASTNode*);

extern ASTParserFn *parsers[];

// Parse tokens into AST
ASTNode *parser_run(Token*);

// Apply parser to a node, returns NULL when no parser is triggered.
ASTNode *parser_parse(ASTNode*, ASTParserFn*);

ASTNode *comment_parser(ASTNode*);
