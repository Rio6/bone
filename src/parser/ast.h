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
   AST_FUNC_TYPE,
   AST_TOKEN,
   AST_DECL,
} ASTType;
extern const char * const ASTType_names[];

typedef ASTNode *(*ASTParseFn)(ASTNode*, ASTParserFn*);
typedef void (*ASTDumpFn)(ASTNode*, unsigned indent);
typedef void (*ASTDeleteFn)(ASTNode*);

typedef struct ASTNode ASTNode;
struct ASTNode {
   ASTType type;
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
void ast_add_next(ASTNode *node, ASTNode *next);
