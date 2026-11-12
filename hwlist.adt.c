#include "hwlist.adt.h"
#include "hwlist.adt.internals.h"

extern hwlist_t *hwlist_create(size_t nsyll) {
   hwlist_t *hwlist = NULL;

   if (!(hwlist = malloc(sizeof *hwlist))) {
      goto err;
   }

   hwlist->max = HWLIST_MAXSIZ;
   hwlist->curr = 0;
   hwlist->siz = 0;
   hwlist->nsyll = nsyll;

   return hwlist;

err:
   return NULL;
}

extern void hwlist_destroy(hwlist_t *hwlist) {
   free(hwlist);
}

extern haiku_word_t *hwlist_append(hwlist_t *hwlist, const haiku_word_t *src) {
   haiku_word_t *dest;

   if (hwlist->siz == hwlist->max) {
      goto err;
   }

   dest = &hwlist->arr[hwlist->siz];
   memcpy(dest, src, sizeof *src);
   hwlist->siz++;

   return dest;

err:
   return NULL;
}

extern haiku_word_t *hwlist_getword(hwlist_t *hwlist) {
   haiku_word_t *word;

   if (hwlist->curr == hwlist->siz) {
      goto err;
   }

   word = &hwlist->arr[hwlist->curr];
   hwlist->curr++;

   return word;

err:
   return NULL;
}

extern void hwlist_reset(hwlist_t *hwlist) {
   hwlist->curr = 0;
}

extern size_t hwlist_size(hwlist_t *hwlist) {
   return hwlist->siz;
}

extern haiku_word_t *hwlist_peek(hwlist_t *hwlist, size_t idx) {
   if (idx >= hwlist->siz) {
      goto err;
   }

   return &hwlist->arr[idx];

err:
   return NULL;
}

extern void hwlist_set(hwlist_t *hwlist, haiku_word_t *src, size_t idx) {
   hwlist->arr[idx] = *src;
}

extern bool hwlist_swap(hwlist_t *hwlist, size_t idx1, size_t idx2) {
   haiku_word_t *w1, *w2, temp;

   // No need to bound-check since it happens in hwlist_peek

   // Write w1 to temp
   if (!(w1 = hwlist_peek(hwlist, idx1))) {
      goto err;
   }
   temp = *w1;

   // Write w2 to w1
   if (!(w2 = hwlist_peek(hwlist, idx2))) {
      goto err;
   }
   hwlist_set(hwlist, w2, idx1);

   // Write temp to w2
   hwlist_set(hwlist, &temp, idx2);

   return true;

err:
   return false;
}

extern size_t hwlist_get_curr(hwlist_t *hwlist) {
   return hwlist->curr;
}

extern void hwlist_set_curr(hwlist_t *hwlist, size_t new_curr) {
   hwlist->curr = new_curr;
}
