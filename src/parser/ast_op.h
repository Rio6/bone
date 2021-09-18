#pragma once

typedef struct ASTOp {
   ASTNode node;
   TokenType op_type;
} ASTOp;

ASTOp *ast_op_create(TokenType);
ASTNode *op_parser(ASTNode*);
