#include <stdio.h>

#include "../haiku.h"
#include "inputs.h"

int main(int argc, char *argv[]) {
   printf("===== Test: %s =====\n", argv[0]);

   bool ret;
   const char *err;

   size_t s, f;

   (void) argc;

   printf(
      "============================\n"
      "> Test for ok_inputs\n"
      "============================\n"
      "\n"
   );
   s = f = 0;
   for (size_t i = 0; i < oksiz; i++) {
      ret = haiku(ok_inputs[i], &err, false);
      printf(
         "testcase: %s | expected: true | actual: %s\n",
         ok_inputs[i],
         ret ? "true" : "false"
      );
      if (ret) s++;
      else f++;
   }
   printf(
      "\n"
      "> Test result:\n"
      "\tSuccess %zu | Failure %zu | Total %zu\n"
      "\n",
      s, f, s + f
   );

   printf(
      "============================\n"
      "> Test for bad_inputs\n"
      "============================\n"
      "\n"
   );
   s = f = 0;
   for (size_t i = 0; i < badsiz; i++) {
      ret = haiku(bad_inputs[i], &err, false);
      printf(
         "testcase: %s | expected: false | actual: %s\n",
         ok_inputs[i],
         ret ? "true" : "false"
      );
      if (ret) f++;
      else s++;
   }
   printf(
      "\n"
      "> Test result:\n"
      "\tSuccess %zu | Failure %zu | Total %zu\n"
      "\n",
      s, f, s + f
   );

   return 0;
}
