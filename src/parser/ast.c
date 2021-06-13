#include "ast.h"

#include <stdio.h>

const char *ASTType_names[] = {
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

void ast_replace(ASTNode *node, ASTNode *prev, ASTNode *next) {
   if(prev) prev->next = node;
   if(next) next->prev = node;
   node->prev = prev;
   node->next = next;
}

ASTNode *ast_root(ASTNode *node) {
   while(node->up) node = node->up;
   return node;
}
