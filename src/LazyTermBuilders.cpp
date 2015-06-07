#include "LazyTermBuilders.h"

#include <cassert>

/**
 * LazyTermBuilder
 */
LazyTermBuilder *LazyTermBuilder::from(term_t term) {
  switch (PL_term_type(term)) {
  case PL_ATOM:
    return LazyAtomBuilder::from(term);
  default:
    assert(false && "No know lazy building functionality for term");
  }
}

/**
 * LazyAtomBuilder
 */
LazyAtomBuilder::LazyAtomBuilder(std::string name)
    : name_(name) {
}

PrologTerm LazyAtomBuilder::build() const {
  return PrologAtom(name_);
}

LazyAtomBuilder *LazyAtomBuilder::from(term_t term) {
  char *s;
  assert(PL_get_chars(term, &s, CVT_ATOM));
  return new LazyAtomBuilder(s);
}

