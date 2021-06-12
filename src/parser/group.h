#pragma once

#include "parser.h"

typedef struct {
   ASTNode node;
   ASTNode *content;
} ASTGroup;

void parser_group(ASTNode*);
ASTGroup *ast_group_create(ASTNode*);
void ast_group_delete(ASTGroup*);
