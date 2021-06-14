#include "ast_error.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void dump(ASTNode *node, unsigned indent) {
   print_indent(indent);
   ASTError *error = (ASTError*) node;
   printf("%s: %s\n", ASTType_names[node->type], error->message);
}

static ASTNode *parse(ASTNode *node, ASTParser**) {
   return node;
}

static void delete(ASTNode *node) {
   if(!node) return;
   ASTError *error = (ASTError*) node;
   free(error->message);
   free(error);
}

ASTError *ast_error_create(const char *message, ASTNode *replace) {
   CALL_METHOD(delete, replace);

   ASTError *error = calloc(1, sizeof(ASTError));

   error->node.type = AST_ERROR;
   error->node.dump = dump;
   error->node.parse = parse;
   error->node.delete = delete;

   if(message) {
      size_t len = strlen(message) + 1;
      error->message = malloc(len);
      strcpy(error->message, message);
   }

   return error;
}

ASTNode *ast_error_create_node(const char *message, ASTNode *replace) {
   return &ast_error_create(message, replace)->node;
}
