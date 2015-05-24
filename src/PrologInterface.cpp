#include "PrologInterface.h"

std::ostream &operator<<(std::ostream &os, const PrologTerm &term) {
  term.print(os);
  return os;
}

/**
 * PrologLifetime
 */
void PrologLifetime::begin(int argc, char *argv[]) {
  if (!PL_initialise(argc, argv)) {
    PL_halt(EXIT_FAILURE);
  }
}

void PrologLifetime::end() {
  PL_cleanup(EXIT_SUCCESS);
}

/**
 * PrologTermHolder
 */
PrologTermHolder::PrologTermHolder(term_t term) : term_(term) {
}

term_t PrologTermHolder::getInternalTerm() const {
  return term_;
}

/**
 * PrologTerm
 */
PrologTerm::PrologTerm(PrintFnTy printFn)
    : PrologTerm(PL_new_term_ref(), printFn) {
}

PrologTerm::PrologTerm(term_t term, PrintFnTy printFn)
    : PrologTermHolder(term), printer_(printFn) {
}

void PrologTerm::print(std::ostream &os) const {
  printer_(os, term_);
}

