#ifndef WORDS_H
#define WORDS_H

#include <stddef.h>

typedef enum {
   HaikuWordKindAdjective     = 1 << 0,
   HaikuWordKindAdverb        = 1 << 1,
   HaikuWordKindNoun          = 1 << 2,
   HaikuWordKindVerb          = 1 << 3,
   HaikuWordKindDeterminer    = 1 << 4,
   HaikuWordKindConjunction   = 1 << 5,
   HaikuWordKindPreposition   = 1 << 6,
   HaikuWordKindInterjection  = 1 << 7,
   HaikuWordKindPronoun       = 1 << 8
} haiku_word_kind_t;

typedef struct {
   size_t id;
   const char *word;
   haiku_word_kind_t pos;
   size_t nsyll;
} haiku_word_t;

extern haiku_word_t words[];
extern const size_t nwords;

#endif
