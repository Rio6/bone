#include "ast.h"

#include <stdio.h>

const char *ASTType_names[] = {
   "AST_ERROR",
   "ATOM",
   "UNARY",
   "BINARY",
   "TENARY",
   "GROUP",
   "TOKEN",
};

void print_indent(unsigned indent) {
   for(unsigned i = 0; i < indent; i++) {
      fputs("  ", stdout);
   }
}

ASTNode *ast_root(ASTNode *node) {
   while(node->up) node = node->up;
   return node;
}
