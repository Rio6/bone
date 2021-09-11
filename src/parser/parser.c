#include "parser.h"
#include "ast.h"

#include "ast_token.h"
#include "ast_atom.h"
#include "ast_group.h"

#include "utils/oop.h"

#include <stddef.h>

// TODO right to left and left to right parsing
ASTParserFn *parsers[] = {
   (ASTParserFn[]) { comment_parser, NULL },
   (ASTParserFn[]) { atom_parser, NULL },
   (ASTParserFn[]) { group_parser, NULL },
   NULL,
};

ASTNode *parser_run(Token *tokens) {
   ASTNode *ast = (ASTNode*) ast_token_from_tokens(tokens);

   for(ASTParserFn **parser = parsers; *parser != NULL; parser++) {
      ast = ast_root(CALL_METHOD(parse, ast, *parser));
   }

   return ast;
}

ASTNode *comment_parser(ASTToken *token) {
   if(token->token->type != T_COMMENT) return NULL;
   ASTNode *next = token->node.next;
   ast_remove(&token->node);
   return next;
}
