#include "LazyTermBuilders.h"

#include <cassert>

/**
 * LazyTermBuilder
 */
LazyTermBuilder *LazyTermBuilder::from(term_t term) {
  switch (PL_term_type(term)) {
  case PL_ATOM:
    return LazyAtomBuilder::from(term);
  case PL_STRING:
    return LazyStringBuilder::from(term);
  case PL_TERM:
    if (!PL_is_list(term)) {
      return LazyFunctorBuilder::from(term);
    }
    return LazyListBuilder::from(term);
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

/**
 * LazyStringBuilder
 */
LazyStringBuilder::LazyStringBuilder(std::string string)
    : string_(string) {
}

PrologTerm LazyStringBuilder::build() const {
  return PrologString(string_);
}

LazyStringBuilder *LazyStringBuilder::from(term_t term) {
  char *s;
  assert(PL_get_chars(term, &s, CVT_STRING));
  return new LazyStringBuilder(s);
}

/**
 * LazyListBuilder
 */
LazyListBuilder::LazyListBuilder(std::vector<LazyTermBuilder*> elements)
    : elements_(elements) {
}

PrologTerm LazyListBuilder::build() const {
  std::vector<PrologTerm> elements;
  for (auto &element : elements_) {
    elements.push_back(element->build());
  }
  return PrologList(elements);
}

LazyListBuilder *LazyListBuilder::from(term_t term) {
  term_t list = PL_copy_term_ref(term);
  term_t head = PL_new_term_ref();
  std::vector<LazyTermBuilder*> elements;
  while (!PL_unify_nil(list) && PL_unify_list(list, head, list)) {
    elements.push_back(LazyTermBuilder::from(head));
  }
  return new LazyListBuilder(elements);
}

/**
 * LazyFunctorBuilder
 */
LazyFunctorBuilder::LazyFunctorBuilder(
    std::string name, std::vector<LazyTermBuilder*> args)
    : name_(name), args_(args) {
}

PrologTerm LazyFunctorBuilder::build() const {
  std::vector<PrologTerm> args;
  for (auto &element : args_) {
    args.push_back(element->build());
  }
  return PrologFunctor(name_, PrologTermVector(args));
}

LazyFunctorBuilder *LazyFunctorBuilder::from(term_t term) {
  term_t tmp = PL_new_term_ref();
  atom_t name;
  int arity;
  assert(PL_get_name_arity(term, &name, &arity));
  std::vector<LazyTermBuilder*> args;
  for (unsigned i = 1; i <= arity; ++i) {
    assert(PL_get_arg(i, term, tmp));
    args.push_back(LazyTermBuilder::from(tmp));
  }
  return new LazyFunctorBuilder(PL_atom_chars(name), args);
}

