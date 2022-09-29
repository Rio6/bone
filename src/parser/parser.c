#include "parser.h"
#include "ast.h"

#include "ast_token.h"
#include "ast_atom.h"
#include "ast_group.h"
#include "ast_func.h"
#include "ast_unary.h"

#include "utils/oop.h"

#include <stddef.h>

ASTParserFn *parsers[] = {
   (ASTParserFn[]) { comment_parser, NULL },
   (ASTParserFn[]) { atom_parser, NULL },
   (ASTParserFn[]) { group_parser, NULL },
   (ASTParserFn[]) { func_type_parser, NULL },
   //(ASTParserFn[]) { postfix_parser, NULL },
   //(ASTParserFn[]) { prefix_parser, NULL },
   NULL,
};

ASTNode *parser_run(Token *tokens) {
   ASTNode *ast = (ASTNode*) ast_token_from_tokens(tokens);

   for(ASTParserFn **parser = parsers; *parser != NULL; parser++) {
      ast = CALL_METHOD(parse, ast, *parser);
   }

   return ast;
}

ASTNode *parser_parse(ASTNode *node, ASTParserFn *parsers) {
   for(ASTParserFn *parser = parsers; *parser != NULL; parser++) {
      ASTNode *new_node = (*parser)(node);
      if(new_node) {
         return CALL_METHOD(parse, new_node, parsers);
      }
   }
   return NULL;
}

ASTNode *comment_parser(ASTNode *node) {
   if(node->type != AST_TOKEN || ((ASTToken*) node)->token->type != T_COMMENT) {
      return NULL;
   }

   ASTNode *next = node->next;
   if(next) next->prev = node->prev;
   node->next = NULL;
   CALL_METHOD(delete, node);
   return next;
}
