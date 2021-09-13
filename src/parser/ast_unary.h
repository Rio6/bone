#pragma once

#include "ast.h"

typedef struct ASTUnary {
   ASTNode node;
   TokenType op_type;
   ASTNode *operand; // owner
} ASTUnary;

ASTUnary *ast_unary_create(ASTType, TokenType, ASTNode*);
ASTNode *prefix_parser(ASTNode*);
ASTNode *postfix_parser(ASTNode*);
