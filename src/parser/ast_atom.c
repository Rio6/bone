#include "ast_atom.h"
#include "ast_token.h"

#include "utils/array.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static TokenType atom_tokens[] = {
   T_KEYWORD,
   T_IDENT,
   T_INT,
   T_FLOAT,
   T_CHAR,
   T_STRING,
};

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {
   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }
   return node;
}

static void dump(ASTNode *node, unsigned indent) {
   ASTAtom *ast_atom = (ASTAtom*) node;

   print_indent(indent);
   printf("%s: ", ASTType_names[node->type]);
   token_print(ast_atom->token);

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

static void delete(ASTNode *node) {
   if(!node) return;

   ASTAtom *ast_atom = (ASTAtom*) node;
   token_delete(ast_atom->token);

   if(node->next) {
      CALL_METHOD(delete, node->next);
   }

   free(ast_atom);
}

ASTAtom *ast_atom_create(Token *token) {
   ASTAtom *atom = calloc(1, sizeof(ASTAtom));
   ast_init(&atom->node, AST_ATOM, parse, dump, delete);
   atom->token = token;
   return atom;
}

ASTNode *atom_parser(ASTToken *token) {
   for(size_t i = 0; i < LEN(atom_tokens); i++) {
      if(token->token->type == atom_tokens[i]) {
         ASTAtom *atom = ast_atom_create(token->token);
         token->token = NULL;
         ast_token_replace(token, &atom->node);
         return &atom->node;
      }
   }
   return NULL;
}
