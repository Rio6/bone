#pragma once

#include "ast.h"

typedef struct ASTFuncType {
   ASTNode node;
   ASTNode *returns;
   ASTNode *param; // nullable
} ASTFuncType;

ASTFuncType *ast_func_type_create(ASTNode *returns, ASTNode *param);
ASTNode *func_type_parser(ASTNode*);
