#pragma once

#include "ast.h"

typedef struct ASTUnary {
   ASTNode node;
   ASTNode *op; // owner
   ASTNode *opand; // owner
} ASTUnary;

ASTUnary *ast_unary_create(ASTType, ASTNode *op, ASTNode *opand);
ASTNode *prefix_parser(ASTNode*);
ASTNode *postfix_parser(ASTNode*);
