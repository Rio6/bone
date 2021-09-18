#include "ast.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

const char * const ASTType_names[] = {
   "AST_ERROR",
   "AST_ATOM",
   "AST_OP",
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

static ASTNode *generic_parse(ASTNode *node, ASTParserFn *parsers) {
   ASTNode *new_node = parser_parse(node, parsers);
   if(new_node) return new_node;

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }

   return node;
}

static void generic_dump(ASTNode *node, unsigned indent) {
   print_indent(indent);
   printf("%s", ASTType_names[node->type]);

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

static void generic_delete(ASTNode *node) {
   if(!node) return;
   if(node->next) {
      CALL_METHOD(delete, node->next);
   }
   free(node);
}

void ast_init(ASTNode *node, ASTType type, ASTParseFn parse, ASTDumpFn dump, ASTDeleteFn delete) {
   node->type = type;
   node->parse = parse != NULL ? parse : generic_parse;
   node->dump = dump != NULL ? dump : generic_dump;
   node->delete = delete != NULL ? delete : generic_delete;
}

void ast_add_next(ASTNode *node, ASTNode *next) {
   if(node == next) return;
   if(next && next->prev) next->prev->next = NULL;
   if(node && node->next) node->next->prev = NULL;
   if(node) node->next = next;
   if(next) next->prev = node;
}
