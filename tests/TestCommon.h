#pragma once

#include "PrologInterface.h"

namespace testcommon {

PrologFunctor is_alpha_pred(PrologTerm alpha);
PrologFunctor is_vowel_pred(PrologTerm alpha);
PrologFunctor next_alpha_pred(PrologTerm alpha, PrologTerm next);

void addTestFacts();

int numSolutionsFor(PrologFunctor functor);

}

