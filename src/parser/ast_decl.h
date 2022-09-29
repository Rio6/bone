#pragma once

#include "ast.h"
#include "ast_atom.h"

#define MAX_NAME_LEN 256

typedef struct ASTDecl {
   ASTNode node;
   ASTNode *type; // owner
   char *name; // owner
} ASTDecl;

ASTDecl *ast_decl_create(ASTNode*, const char*); // takes owner of node, copy char*
ASTNode *decl_parser(ASTNode*);
