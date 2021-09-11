#pragma once

#include "parser.h"

typedef enum {
   AST_ERROR,
   AST_ATOM,
   AST_PREFIX,
   AST_POSTFIX,
   AST_BINARY,
   AST_TENARY,
   AST_GROUP,
   AST_TOKEN,
} ASTType;
extern const char *ASTType_names[];

typedef ASTNode *(*ASTParseFn)(ASTNode*, ASTParserFn*);
typedef void (*ASTDumpFn)(ASTNode*, unsigned indent);
typedef void (*ASTDeleteFn)(ASTNode*);

typedef struct ASTNode ASTNode;
struct ASTNode {
   ASTType type;
   ASTNode *up;
   ASTNode *prev;
   ASTNode *next;

   // Apply null-terminated ASTParserFn to its children and also the rest of node behind it.
   // Stop at first non-null result, returns a new node that replaces the current node
   ASTParseFn parse;
   ASTDumpFn dump;
   ASTDeleteFn delete;
};

void print_indent(unsigned);

void ast_init(ASTNode*, ASTType, ASTParseFn, ASTDumpFn, ASTDeleteFn);
ASTNode *ast_root(ASTNode*);
ASTNode *ast_replace(ASTNode *old, ASTNode *new);
// Chop the head, return the new head.
ASTNode *ast_chop(ASTNode*);
void ast_remove(ASTNode*);
