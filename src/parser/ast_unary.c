#include "ast.h"
#include "ast_unary.h"
#include "ast_token.h"

#include "utils/array.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static const TokenType prefix_tokens[] = {
   T_REF, T_DEREF, T_PLUS, T_MINUS, T_NOT,
};

static const TokenType postfix_tokens[] = {
   T_REF, T_DEREF,
};

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {
   ASTNode *new_node = parser_parse(node, parsers);
   if(new_node) return new_node;

   ASTUnary *unary = (ASTUnary*) node;

   unary->operand = CALL_METHOD(parse, unary->operand, parsers);

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }

   return node;
}

static void dump(ASTNode *node, unsigned indent) {
   ASTUnary *unary = (ASTUnary*) node;

   print_indent(indent);
   printf("%s: type: %s\n", ASTType_names[node->type], TokenType_names[unary->op_type]);

   CALL_METHOD(dump, unary->operand, indent+1);

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

   CALL_METHOD(delete, unary->operand);

   free(node);
}

ASTUnary *ast_unary_create(ASTType type, TokenType op_type, ASTNode *operand) {
   ASTUnary *unary = calloc(1, sizeof(ASTUnary));

   ast_init(&unary->node, type, parse, dump, delete);
   unary->op_type = op_type;
   unary->operand = operand;

   return unary;
}

ASTNode *prefix_parser(ASTNode *node) {
   if(node->type != AST_TOKEN || !node->next || node->next->type == AST_TOKEN) {
      return NULL;
   }

   TokenType op_type = T_ERROR;

   for(size_t i = 0; i < LEN(prefix_tokens); i++) {
      if(((ASTToken*) node)->token->type == prefix_tokens[i]) {
         op_type = prefix_tokens[i];
         break;
      }
   }

   if(op_type != T_ERROR) {
      ASTNode *operand = node->next;
      ASTUnary *unary = ast_unary_create(AST_PREFIX, op_type, operand);

      if(operand->next) {
         unary->node.next = operand->next;
         if(unary->node.next) unary->node.next->prev = &unary->node;
      }

      node->next = NULL;
      CALL_METHOD(delete, node);
      operand->prev = operand->next = NULL;

      return &unary->node;
   }

   return NULL;
}

ASTNode *postfix_parser(ASTNode *node) {
   if(node->type == AST_TOKEN || !node->next || node->next->type != AST_TOKEN) {
      return NULL;
   }

   TokenType op_type = T_ERROR;

   for(size_t i = 0; i < LEN(postfix_tokens); i++) {
      if(((ASTToken*) node->next)->token->type == postfix_tokens[i]) {
         op_type = postfix_tokens[i];
         break;
      }
   }

   if(op_type != T_ERROR) {
      ASTUnary *unary = ast_unary_create(AST_POSTFIX, op_type, node);

      if(node->next->next) {
         unary->node.next = node->next->next;
         if(unary->node.next) unary->node.next->prev = &unary->node;
      }

      node->next->next = NULL;
      CALL_METHOD(delete, node->next);
      node->prev = node->next = NULL;

      return &unary->node;
   }

   return NULL;
}
