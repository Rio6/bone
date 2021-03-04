#include "lexer.h"
#include "stream.h"

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

   Token *tokens = lex(&stream);
   token_dump(tokens);
   fclose(file);

   return 0;
}
