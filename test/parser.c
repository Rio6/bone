#include "lexer/lexer.h"

#include "parser/ast.h"
#include "parser/parser.h"

#include "utils/stream.h"
#include "utils/oop.h"

#include <stdio.h>

#define TEST_FILE "../idea.bone"

int main() {
   FILE *file = fopen(TEST_FILE, "r");

   if(!file) {
      perror("error open file");
      return 1;
   }

   Stream stream = {
      .stream = file
   };

   Token *tokens = lexer_run(&stream);
   fclose(file);

   ASTNode *ast = parser_run(tokens);

   CALL_METHOD(dump, ast, 0);
   CALL_METHOD(delete, ast);

   return 0;
}

