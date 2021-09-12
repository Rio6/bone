#include "lexer/lexer.h"
#include "utils/stream.h"

#include <stdio.h>

#define TEST_FILE "../idea.bone"

int main(void) {
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
   token_dump(tokens);
   token_delete(tokens);

   return 0;
}
