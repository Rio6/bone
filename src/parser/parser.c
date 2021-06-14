#include "parser.h"
#include "ast.h"

#include "ast_token.h"
#include "ast_atom.h"
#include "ast_group.h"

#include "utils/oop.h"

#include <stddef.h>

ASTParser **parsers[] = {
   (ASTParser*[]) { atom_parser, NULL},
   (ASTParser*[]) { group_parser, NULL },
   NULL,
};

ASTNode *parser_run(Token *tokens) {
   ASTNode *ast = (ASTNode*) ast_token_from_tokens(tokens);

   for(ASTParser ***parser = parsers; *parser != NULL; parser++) {
      ast = ast_root(CALL_METHOD(parse, ast, *parser));
   }

   return ast;
}

ASTNode *comment_parser(ASTToken *token) {
   if(token->token->type != COMMENT) return NULL;
   return ast_chop(&token->node, NULL);
}
