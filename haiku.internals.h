#ifndef HAIKU_INTERNALS_H
#define HAIKU_INTERNALS_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "words.h"
#include "hwlist.adt.h"

#define DEBUG 0

typedef struct {
   haiku_word_t line[7];
   size_t siz;
} haiku_line_t;

typedef struct {
   const char *reason;
   const haiku_word_t *words;
   size_t nwords;
   size_t longest_nsyll;
   hwlist_t **lists;
   const char *input;
   const haiku_word_t *firstword;
   haiku_line_t lines[3];

   size_t *states[5+7+5];
   size_t remain, old_remain;
   size_t idx;
   size_t linelast[3];
   size_t remains[17];

   size_t rollback_count;
   size_t try_count;
} ctx_t;

static void show_data_info(ctx_t *ctx);
static size_t find_longest_nsyll(ctx_t *ctx);
static bool init_lists(ctx_t *ctx);
static bool fill_lists(ctx_t *ctx);
static void destroy_lists(ctx_t *ctx);
static haiku_word_t *find_firstword(ctx_t *ctx);
static bool try(ctx_t *ctx);
static bool process_firstword(ctx_t *ctx);
static bool init_states(ctx_t *ctx);
static void destroy_states(ctx_t *ctx);
static void archive_lists_state(ctx_t *ctx);
static void restore_lists_state(ctx_t *ctx);
static void archive_current_remain(ctx_t *ctx);
static void restore_current_remain(ctx_t *ctx);
static bool validate(ctx_t *ctx);
static void init_lines(ctx_t *ctx);
static bool fill_line(ctx_t *ctx, size_t which);
static haiku_word_t *getword_at_most_nsyll(ctx_t *ctx, size_t most);
static void putword(haiku_line_t *line, const haiku_word_t *word);
static void popword(haiku_line_t *line);
static void render(ctx_t *ctx);
static const char *getcolor(haiku_word_kind_t kind);
static void shuffle(haiku_word_t words[], size_t nwords);

[[maybe_unused]]
static void debug_showlist(ctx_t *ctx);

#endif
