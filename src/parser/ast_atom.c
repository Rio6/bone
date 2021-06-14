#include "ast_atom.h"
#include "ast_token.h"

#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static ASTNode *parse(ASTNode *node, ASTParser **parsers) {
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

   atom->node.type = ATOM;
   atom->node.parse = parse;
   atom->node.dump = dump;
   atom->node.delete = delete;

   atom->token = token;

   return atom;
}

ASTNode *atom_parser(ASTToken *token) {
   switch(token->token->type) {
      case IDENT:
      case KEYWORD:
      case INT:
      case FLOAT:
      case CHAR:
      case STRING:
         {
            ASTAtom *atom = ast_atom_create(token->token);

            atom->node.next = token->node.next;
            if(token->node.next) token->node.next->prev = &atom->node;

            token->node.next = NULL;
            token->token = NULL;
            CALL_METHOD(delete, &token->node);

            return &atom->node;
         }
      default:
         return NULL;
   }
}
