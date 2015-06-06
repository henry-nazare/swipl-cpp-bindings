#include "TestCommon.h"

namespace testcommon {

PrologFunctor is_alpha_pred(PrologTerm alpha) {
  return PrologFunctor ("is_alpha", PrologTermVector({alpha}));
}

PrologFunctor is_vowel_pred(PrologTerm alpha) {
  return PrologFunctor ("is_vowel", PrologTermVector({alpha}));
}

PrologFunctor next_alpha_pred(PrologTerm alpha, PrologTerm next) {
  return PrologFunctor ("next_alpha", PrologTermVector({alpha, next}));
}

void addTestFacts() {
  PrologCall::fact(is_alpha_pred(PrologAtom("a")));
  PrologCall::fact(is_alpha_pred(PrologAtom("b")));
  PrologCall::fact(is_alpha_pred(PrologAtom("c")));
  PrologCall::fact(is_alpha_pred(PrologAtom("d")));
  PrologCall::fact(is_alpha_pred(PrologAtom("e")));

  PrologCall::fact(is_vowel_pred(PrologAtom("a")));
  PrologCall::fact(is_vowel_pred(PrologAtom("e")));

  PrologCall::fact(next_alpha_pred(PrologAtom("a"), PrologAtom("b")));
  PrologCall::fact(next_alpha_pred(PrologAtom("b"), PrologAtom("c")));
  PrologCall::fact(next_alpha_pred(PrologAtom("c"), PrologAtom("d")));
  PrologCall::fact(next_alpha_pred(PrologAtom("d"), PrologAtom("e")));
}

int numSolutionsFor(PrologFunctor functor) {
  return PrologQuery(functor).solutions().size();
}

}
