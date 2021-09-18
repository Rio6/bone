#include "ast_atom.h"
#include "ast_token.h"
#include "utils/array.h"

#include <stddef.h>

static TokenType atom_tokens[] = {
   T_KEYWORD,
   T_IDENT,
   T_INT,
   T_FLOAT,
   T_CHAR,
   T_STRING,
};

ASTNode *atom_parser(ASTNode *node) {
   if(node->type != AST_TOKEN) return NULL;
   ASTToken *token = (ASTToken*) node;

   for(size_t i = 0; i < LEN(atom_tokens); i++) {
      if(token->token->type == atom_tokens[i]) {
         node->type = AST_ATOM;
         return node;
      }
   }
   return NULL;
}
