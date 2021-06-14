#pragma once

#include "ast.h"

typedef struct {
   ASTNode node;
   char *message; // owner
} ASTError;

ASTError *ast_error_create(const char*, ASTNode *replace);
ASTNode *ast_error_create_node(const char*, ASTNode *replace);
