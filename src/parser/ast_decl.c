#include "ast.h"
#include "ast_decl.h"
#include "ast_atom.h"

#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {
   ASTNode *new_node = parser_parse(node, parsers);
   if(new_node) return new_node;

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }

   return node;
}

static void delete(ASTNode *node) {
   if(!node) return;

   ASTDecl *decl = (ASTDecl*) node;

   if(node->next) {
      CALL_METHOD(delete, node->next);
   }

   CALL_METHOD(delete, decl->type);

   free(decl->name);
   free(node);
}

static void dump(ASTNode *node, unsigned indent) {
   ASTDecl *decl = (ASTDecl*) node;

   print_indent(indent);
   printf("%s: name: %s\n", ASTType_names[node->type], decl->name);

   CALL_METHOD(dump, decl->type, indent+1);

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

ASTDecl *ast_decl_create(ASTNode *type, const char *name) {
   ASTDecl *decl = calloc(1, sizeof(ASTDecl));

   ast_init(&decl->node, AST_DECL, parse, dump, delete);

   decl->type = type;

   // allocate and copy name
   size_t name_len = strnlen(name, MAX_NAME_LEN);
   decl->name = malloc(name_len);
   strncpy(decl->name, name, name_len-1);
   decl->name[name_len-1] = '0';

   return decl;
}

ASTNode *decl_parser(ASTNode *node) {
   return NULL;
}
