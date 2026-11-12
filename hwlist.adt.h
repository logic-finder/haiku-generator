#ifndef HWLISTADT_ADT_H
#define HWLISTADT_ADT_H

#include <stddef.h>
#include <stdbool.h>

#include "words.h"

typedef struct hwlist hwlist_t;

hwlist_t *hwlist_create(size_t nsyll);
void hwlist_destroy(hwlist_t *hwlist);
haiku_word_t *hwlist_append(hwlist_t *hwlist, const haiku_word_t *src);
haiku_word_t *hwlist_getword(hwlist_t *hwlist);
void hwlist_reset(hwlist_t *hwlist);
size_t hwlist_size(hwlist_t *hwlist);
haiku_word_t *hwlist_peek(hwlist_t *hwlist, size_t idx);
void hwlist_set(hwlist_t *hwlist, haiku_word_t *src, size_t idx);
bool hwlist_swap(hwlist_t *hwlist, size_t idx1, size_t idx2);
size_t hwlist_get_curr(hwlist_t *hwlist);
void hwlist_set_curr(hwlist_t *hwlist, size_t new_curr);

#endif
