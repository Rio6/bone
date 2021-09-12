#include "lexer.h"
#include "token.h"

#include "utils/stream.h"

#include <string.h>
#include <ctype.h>

Token *lexer_run(Stream *stream) {
   Token head_token = {0};
   Token *last_token = &head_token;

   while(1) {
      int has_token = 0;
      char last_c = '\0';

      for(Lexer **lexer = lexers; *lexer != NULL; lexer++) {

         // advance until not a white space
         while(isspace(stream_getc(stream)));
         last_c = stream_ungetc(stream, 1);

         Token *token = (*lexer)(stream);

         if(token) {
            last_token->next = token;
            last_token = token;
            has_token = 1;
            break;
         }
      }

      if(!has_token) {
         char buff[128];
         snprintf(buff, sizeof(buff), "Unexpected character: %1$c (%1$X)\n", last_c);
         last_token->next = token_create(T_ERROR, buff);
         last_token = last_token->next;
      }

      if(last_token->type == T_ERROR || last_token->type == T_EOT) {
         break;
      }
   }

   return head_token.next;
}
