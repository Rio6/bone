#include "ast.h"
#include "ast_unary.h"
#include "ast_token.h"
#include "ast_group.h"

#include "utils/array.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static TokenType prefix_tokens[] = {
   T_REF, T_DEREF, T_PLUS, T_MINUS, T_NOT,
};

static TokenType postfix_tokens[] = {
   T_REF, T_DEREF,
};

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {
   ASTNode *new_node = parser_parse(node, parsers);
   if(new_node) return new_node;

   ASTUnary *unary = (ASTUnary*) node;

   unary->opand = CALL_METHOD(parse, unary->opand, parsers);

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }

   return node;
}

static void dump(ASTNode *node, unsigned indent) {
   ASTUnary *unary = (ASTUnary*) node;

   print_indent(indent);
   printf("%s: type: %s\n", ASTType_names[node->type], TokenType_names[unary->op_type]);
   CALL_METHOD(dump, unary->opand, indent+1);

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

static void delete(ASTNode *node) {
   if(!node) return;

   ASTUnary *unary = (ASTUnary*) node;

   if(node->next) {
      CALL_METHOD(delete, node->next);
   }

   CALL_METHOD(delete, unary->opand);

   free(node);
}

ASTUnary *ast_unary_create(ASTType type, TokenType op_type, ASTNode *opand) {
   ASTUnary *unary = calloc(1, sizeof(ASTUnary));

   ast_init(&unary->node, type, parse, dump, delete);
   unary->op_type = op_type;
   unary->opand = opand;

   return unary;
}

ASTNode *prefix_parser(ASTNode *node) {
   if(!node->next || node->type != AST_TOKEN) return NULL;

   TokenType op_type = T_ERROR;
   for(size_t i = 0; i < LEN(prefix_tokens); i++) {
      TokenType type = ((ASTToken*) node)->token->type;
      if(type == prefix_tokens[i]) {
         op_type = type;
         break;
      }
   }

   if(op_type == T_ERROR) return NULL;

   ASTUnary *unary = ast_unary_create(AST_PREFIX, op_type, node->next);
   ast_add_next(&unary->node, node->next->next);
   node->next = node->next->prev = NULL;
   CALL_METHOD(delete, node);

   return &unary->node;
}

ASTNode *postfix_parser(ASTNode *node) {
   if(!node->next || node->next->type != AST_TOKEN) return NULL;

   TokenType op_type = T_ERROR;
   for(size_t i = 0; i < LEN(postfix_tokens); i++) {
      TokenType type = ((ASTToken*) node->next)->token->type;
      if(type == postfix_tokens[i]) {
         op_type = type;
         break;
      }
   }

   if(op_type == T_ERROR) return NULL;

   ASTUnary *unary = ast_unary_create(AST_POSTFIX, op_type, node);
   ast_add_next(&unary->node, node->next->next);
   node->next->next = NULL;
   CALL_METHOD(delete, node->next);
   node->next = node->prev = NULL;

   return &unary->node;
}
