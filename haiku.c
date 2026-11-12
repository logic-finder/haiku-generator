#include "haiku.h"
#include "haiku.internals.h"

extern bool haiku(const char *input, const char **reason, bool show) {
   ctx_t *ctx = NULL;

   // Init ctx
   if (!(ctx = malloc(sizeof *ctx))) {
      *reason = "failed to malloc ctx";
      return false;
   }

   ctx->reason = NULL;
   ctx->words = words;
   ctx->nwords = nwords;
   ctx->input = input;
   ctx->rollback_count = 0;
   ctx->try_count = 0;

   #if DEBUG
   show_data_info(ctx);
   #endif

   // Preprocess data
   if (0 == (ctx->longest_nsyll = find_longest_nsyll(ctx))) {
      ctx->reason = "invalid data. nsyll > 0 required";
      goto err;
   }

   if (!init_lists(ctx)) {
      goto err;
   }

   if (!fill_lists(ctx)) {
      goto err;
   }

   #if DEBUG
   debug_showlist(ctx);
   #endif

   if (!try(ctx)) {
      goto err;
   }

   if (show) {
      render(ctx);
   }

   // Clean up
   destroy_lists(ctx);
   free(ctx);

   return true;

err:
   *reason = ctx->reason;
   return false;
}

static void show_data_info(ctx_t *ctx) {
   (void) printf(
      "haiku: total %zu entries\n",
      ctx->nwords
   );
}

static size_t find_longest_nsyll(ctx_t *ctx) {
   size_t longest = 0;

   for (size_t i = 0; i < ctx->nwords; i++) {
      if (ctx->words[i].nsyll > longest) {
         longest = ctx->words[i].nsyll;
      }
   }

   return longest;
}

static bool init_lists(ctx_t *ctx) {
   ctx->lists = malloc(sizeof ctx->lists[0] * (ctx->longest_nsyll + 1));
   if (!ctx->lists) {
      ctx->reason = "failed to init lists";
      goto err;
   }

   // Init to NULL
   for (size_t nsyll = 0; nsyll <= ctx->longest_nsyll; nsyll++) {
      ctx->lists[nsyll] = NULL;
   }

   for (size_t nsyll = 1; nsyll <= ctx->longest_nsyll; nsyll++) {
      if (!(ctx->lists[nsyll] = hwlist_create(nsyll))) {
         ctx->reason = "failed to create hwlist";
         goto err;
      }
   }

   return true;

err:
   for (size_t nsyll = 0; nsyll <= ctx->longest_nsyll; nsyll++) {
      free(ctx->lists[nsyll]);
   }
   free(ctx->lists);
   return false;
}

static bool fill_lists(ctx_t *ctx) {
   const haiku_word_t *word;

   for (size_t i = 0; i < ctx->nwords; i++) {
      word = &ctx->words[i];
      if (!hwlist_append(ctx->lists[word->nsyll], word)) {
         ctx->reason = "failed to copy word";
         goto err;
      }
   }

   return true;

err:
   return false;
}

static void destroy_lists(ctx_t *ctx) {
   for (size_t nsyll = 1; nsyll <= ctx->longest_nsyll; nsyll++) {
      hwlist_destroy(ctx->lists[nsyll]);
   }
   free(ctx->lists);
}

static haiku_word_t *find_firstword(ctx_t *ctx) {
   size_t list_siz;
   hwlist_t *list;
   haiku_word_t *word, *ret = NULL;

   for (size_t nsyll = 1; nsyll <= ctx->longest_nsyll; nsyll++) {
      list = ctx->lists[nsyll];
      list_siz = hwlist_size(list);

      for (size_t i = 0; i < list_siz; i++) {
         if (!(word = hwlist_peek(list, i))) {
            ctx->reason = "failed to peek";
            goto err;
         }
         if (!strcmp(ctx->input, word->word)) {
            // Make `firstword` be the 0th element
            if (!hwlist_swap(list, i, 0)) {
               ctx->reason = "failed to swap";
               goto err;
            }
            ret = hwlist_peek(list, 0);  // can't fail
            goto outside;
         }
      }
   }

outside:
   if (!ret) {
      // error path: couldn't find firstword
      ctx->reason = "invalid first word(argv[1])";
      goto err;
   }

   return ret;

err:
   return NULL;
}

static bool try(ctx_t *ctx) {
   // Init
   init_lines(ctx);
   if (!init_states(ctx)) {
      goto err;
   }
   ctx->idx = 0;

   // Make haiku
   ctx->remain = 5;
   if (!process_firstword(ctx)) {
      goto err;
   }
line1:
   if (ctx->remain > 0) {
      if (!fill_line(ctx, 0)) {
         goto resync;
      }
   }
   ctx->linelast[0] = ctx->idx - 1;

   ctx->remain = 7;
line2:
   if (!fill_line(ctx, 1)) {
      goto resync;
   }
   ctx->linelast[1] = ctx->idx - 1;

   ctx->remain = 5;
line3:
   if (!fill_line(ctx, 2)) {
      goto resync;
   }
   ctx->linelast[2] = ctx->idx - 1;

   if (!validate(ctx)) {
      ctx->idx--;
      goto resync;
   }

   // Clean up
   destroy_states(ctx);
   return true;

resync:
   ctx->rollback_count++;

   if (ctx->idx == 1) {
      (void) printf("haiku: rollback happened %zu times\n", ctx->rollback_count);
      (void) printf("haiku: validation happened %zu times\n", ctx->try_count);
      ctx->reason = "not able to make a conforming haiku";
      goto err;
   }

   restore_lists_state(ctx);
   restore_current_remain(ctx);

   if (!getword_at_most_nsyll(ctx, ctx->remain)) {
      ctx->idx--;
      goto resync;
   }

   if (ctx->idx <= ctx->linelast[0]) {
      popword(&ctx->lines[0]);
      goto line1;
   }
   if (ctx->idx <= ctx->linelast[1]) {
      popword(&ctx->lines[1]);
      goto line2;
   }
   if (ctx->idx <= ctx->linelast[2]) {
      popword(&ctx->lines[2]);
      goto line3;
   }

err:
   return false;
}

static bool init_states(ctx_t *ctx) {
   size_t *arr;

   for (size_t i = 0; i < 17; i++) {
      arr = malloc(ctx->longest_nsyll * sizeof arr[0]);
      if (!arr) {
         goto err;
      }
      ctx->states[i] = arr;
   }

   return true;

err:
   ctx->reason = "failed to init states";
   return false;
}

static void destroy_states(ctx_t *ctx) {
   for (size_t i = 0; i < 17; i++) {
      free(ctx->states[i]);
   }
}

static bool process_firstword(ctx_t *ctx) {
   if (!(ctx->firstword = find_firstword(ctx))) {
      goto err;
   }

   if (ctx->firstword->nsyll > 5) {
      ctx->reason = "first word has more than 5 syllables";
      goto err;
   }

   archive_current_remain(ctx);
   archive_lists_state(ctx);

   // can't fail
   (void) hwlist_getword(ctx->lists[ctx->firstword->nsyll]);

   putword(&ctx->lines[0], ctx->firstword);

   ctx->remain -= ctx->firstword->nsyll;
   ctx->idx++;

   return true;

err:
   return false;
}

static void archive_lists_state(ctx_t *ctx) {
   for (size_t i = 0; i < ctx->longest_nsyll; i++) {
      ctx->states[ctx->idx][i] = hwlist_get_curr(ctx->lists[i + 1]);
   }
}

static void restore_lists_state(ctx_t *ctx) {
   for (size_t i = 0; i < ctx->longest_nsyll; i++) {
      hwlist_set_curr(ctx->lists[i + 1], ctx->states[ctx->idx][i]);
   }
}

static void archive_current_remain(ctx_t *ctx) {
   ctx->remains[ctx->idx] = ctx->remain;
}

static void restore_current_remain(ctx_t *ctx) {
   ctx->remain = ctx->remains[ctx->idx];
}

static void init_lines(ctx_t *ctx) {
   for (size_t i = 0; i < 3; i++) {
      ctx->lines[i].siz = 0;
   }
}

static bool fill_line(ctx_t *ctx, size_t which) {
   haiku_word_t *word;

   for (;;) {
      if (ctx->remain == 0) {
         break;
      }

      archive_current_remain(ctx);
      archive_lists_state(ctx);

      word = getword_at_most_nsyll(ctx, ctx->remain);
      if (!word) {  // Comsumed all words -- need to resync
         ctx->idx--;
         goto resync;
      }

      putword(&ctx->lines[which], word);

      ctx->remain -= word->nsyll;
      ctx->idx++;
   }

   return true;

resync:
   return false;
}

static haiku_word_t *getword_at_most_nsyll(ctx_t *ctx, size_t most) {
   haiku_word_t *word;

   if (most > ctx->longest_nsyll) {
      most = ctx->longest_nsyll;
   }

   for (;;) {
      if (most == 0) {
         goto failure;
      }

      word = hwlist_getword(ctx->lists[most]);

      if (word) {
         break;
      }
      else {
         most--;
      }
   }

   return word;

failure:
   return NULL;
}

static void putword(haiku_line_t *line, const haiku_word_t *word) {
   line->line[line->siz++] = *word;
}

static void popword(haiku_line_t *line) {
   line->siz--;
}

static bool validate(ctx_t *ctx) {
   size_t adj = 0;
   size_t adv = 0;
   size_t noun = 0;
   size_t verb = 0;
   size_t etc = 0;
   haiku_word_t *word;

   for (size_t i = 0; i < 3; i++) {
      for (size_t j = 0; j < ctx->lines[i].siz; j++) {
         word = &ctx->lines[i].line[j];
         switch (word->pos) {
            case HaikuWordKindAdjective: adj++; break;
            case HaikuWordKindAdverb: adv++; break;
            case HaikuWordKindNoun: noun++; break;
            case HaikuWordKindVerb: verb++; break;
            default: etc++; break;
         }
      }
   }

   ctx->try_count++;

   #if DEBUG
   printf("adj = %zu, adv = %zu, noun = %zu, verb = %zu, etc = %zu\n", adj, adv, noun, verb, etc);
   #endif

   return (adj && adv && noun && verb)
      ? true
      : false;
}

static void render(ctx_t *ctx) {
   size_t sylcnt;

   (void) printf(
      "Input: %s (%zu)\n",
      ctx->firstword->word,
      ctx->firstword->nsyll
   );
   (void) printf(
      "\n"
      "Output:\n"
   );
   for (size_t i = 0; i < 3; i++) {
      sylcnt = 0;
      for (size_t j = 0; j < ctx->lines[i].siz; j++) {
         (void) printf("%s ", ctx->lines[i].line[j].word);
         sylcnt += ctx->lines[i].line[j].nsyll;
      }
      (void) printf("(%zu)\n", sylcnt);
   }

   #if DEBUG
   printf("haiku: rollback happened %zu times\n", ctx->rollback_count);
   printf("haiku: validation happened %zu times\n", ctx->try_count);
   #endif
}

/*** DEBUG ***/

static void debug_showlist(ctx_t *ctx) {
   size_t siz;
   haiku_word_t *word;

   for (size_t nsyll = 1; nsyll <= ctx->longest_nsyll; nsyll++) {
      printf("===== lists[%zu] =====\n", nsyll);
      siz = hwlist_size(ctx->lists[nsyll]);

      for (size_t i = 0; (i < 5) && (i < siz); i++) {
         // Assumes not to fail
         word = hwlist_peek(ctx->lists[nsyll], i);
         printf(
            "[%zu] = %s (%zu)\n",
            i,
            word->word,
            word->nsyll
         );
      }
   }
}
