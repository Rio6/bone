#include "ast.h"
#include "utils/oop.h"

#include <stdio.h>

const char *ASTType_names[] = {
   "AST_ERROR",
   "AST_ATOM",
   "AST_PREFIX",
   "AST_POSTFIX",
   "AST_BINARY",
   "AST_TENARY",
   "AST_GROUP",
   "AST_TOKEN",
};

void print_indent(unsigned indent) {
   for(unsigned i = 0; i < indent; i++) {
      fputs("   ", stdout);
   }
}

void ast_init(ASTNode *node, ASTType type, ASTParseFn parse, ASTDumpFn dump, ASTDeleteFn delete) {
   node->type = type;
   node->parse = parse;
   node->dump = dump;
   node->delete = delete;
}

ASTNode *ast_root(ASTNode *node) {
   while(node->up) node = node->up;
   return node;
}
