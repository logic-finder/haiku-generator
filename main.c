#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "haiku.h"

int main(int argc, char *argv[]) {
   bool stat = false;
   const char *err = NULL;

   (void) argv;

   if (argc == 1) {
      err = "usage: haiku <word>";
      goto report_err;
   }

   if (!haiku(argv[1], &err, true)) {
      goto report_err;
   }

   stat = true;

report_err:
   if (err) {
      (void) fprintf(stderr, "panic! %s\n", err);
   }

   if (!stat) {
      goto abort;
   }

   return EXIT_SUCCESS;

abort:
   abort();
}
