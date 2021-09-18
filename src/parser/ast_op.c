#include "ast.h"
#include "ast_op.h"
#include "ast_token.h"

#include "utils/array.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static TokenType op_tokens[] = {
   T_COLON,
   T_REF,
   T_DEREF,
   T_PLUS,
   T_MINUS,
   T_MULT,
   T_DIV,
   T_MOD,
   T_LT,
   T_GT,
   T_AND,
   T_OR,
   T_XOR,
   T_NOT,
   T_ASSIGN,
   T_LSHIFT,
   T_RSHIFT,
   T_EQ,
   T_NE,
   T_LE,
   T_GE,
   T_ASPLUS,
   T_ASMINUS,
   T_ASMULT,
   T_ASDIV,
   T_ASMOD,
   T_ASAND,
   T_ASOR,
   T_ASXOR,
   T_ASLSHIFT,
   T_ASRSHIFT,
};

static void dump(ASTNode *node, unsigned indent) {
   ASTOp *op = (ASTOp*) node;

   print_indent(indent);
   printf("%s: type: %s\n", ASTType_names[node->type], TokenType_names[op->op_type]);

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

ASTOp *ast_op_create(TokenType type) {
   ASTOp *op = calloc(1, sizeof(ASTOp));
   ast_init(&op->node, AST_OP, NULL, &dump, NULL);
   op->op_type = type;
   return op;
}

ASTNode *op_parser(ASTNode *node) {
   if(node->type != AST_TOKEN) return NULL;
   ASTToken *token = (ASTToken*) node;

   for(size_t i = 0; i < LEN(op_tokens); i++) {
      if(token->token->type == op_tokens[i]) {
         ASTOp *op = ast_op_create(op_tokens[i]);
         ast_add_next(&op->node, node->next);
         CALL_METHOD(delete, node);
         return &op->node;
      }
   }

   return NULL;
}
