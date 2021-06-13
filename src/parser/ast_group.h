#pragma once

#include "ast.h"

typedef struct {
   ASTNode node;
   ASTNode *content; // owner
} ASTGroup;

ASTGroup *ast_group_create(ASTNode*);
void group_parser(ASTNode*);
