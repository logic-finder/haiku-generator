#ifndef HWLIST_ADT_INTERNALS_H
#define HWLIST_ADT_INTERNALS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hwlist.adt.h"

#define HWLIST_MAXSIZ 256

struct hwlist {
   haiku_word_t arr[HWLIST_MAXSIZ];
   size_t max;
   size_t siz;
   size_t curr;
   size_t nsyll;
};

#endif
