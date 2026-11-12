#include "inputs.h"

const char *ok_inputs[] = {
   // nsyll = 1
   "A",
   "And",
   "Fruit",
   "At",
   "Bird",

   // nsyll = 2
   "Always",
   "Before",
   "Beneath",
   "Courage",
   "Diamond",

   // nsyll = 3
   "Absolute",
   "Animal",
   "Beautiful",
   "Carefully",
   "Celebrate",

   // nsyll = 4
   "Accelerate",
   "Anticipate",
   "Appreciate",
   "Architecture",
   "Capacity",

   // nsyll = 5
   "Communication",
   "Consideration",
   "Curiosity",
   "Determination",
   "Electricity"
};
const size_t oksiz = sizeof ok_inputs / sizeof ok_inputs[0];

const char *bad_inputs[] = {
   // nsyll = 6
   "Incomprehensible",
   "Rehabilitation",
   "Responsibility",

   // Not exist in dataset
   "Here",
   "Challenging"
};
const size_t badsiz = sizeof bad_inputs / sizeof bad_inputs[0];
