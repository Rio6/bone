#include "parser.h"
#include "ast.h"

#include "ast_token.h"
#include "ast_group.h"

#include "utils/oop.h"

#include <stddef.h>

ASTParser *parsers[] = {
   group_parser,
   NULL,
};

ASTNode *parser_run(Token *tokens) {
   ASTNode *ast = (ASTNode*) ast_token_from_tokens(tokens);

   for(ASTParser **parser = parsers; *parser != NULL; parser++) {
      CALL_METHOD(parse, ast, *parser);
      ast = ast_root(ast);
   }

   return ast;
}
