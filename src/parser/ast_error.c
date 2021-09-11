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

static ASTNode *parse(ASTNode *node, ASTParserFn*) {
   return node;
}

static void delete(ASTNode *node) {
   if(!node) return;
   ASTError *error = (ASTError*) node;
   free(error->message);
   free(error);
}

ASTError *ast_error_create(const char *message, ASTNode *replace) {
   ASTError *error = calloc(1, sizeof(ASTError));

   ast_init(&error->node, AST_ERROR, parse, dump, delete);

   if(message) {
      size_t len = strlen(message) + 1;
      error->message = malloc(len);
      strcpy(error->message, message);
   }

   if(replace) {
      if(replace->prev) replace->prev->next = &error->node;
      CALL_METHOD(delete, replace);
   }

   return error;
}
