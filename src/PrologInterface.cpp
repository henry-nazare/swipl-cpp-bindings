#include "PrologInterface.h"

#include <cassert>

std::ostream &operator<<(std::ostream &os, const PrologTerm &term) {
  term.print(os);
  return os;
}

static const char *GetPrologTermAtomStr(term_t term) {
  char *s;
  assert(PL_get_chars(term, &s, CVT_ATOM));
  return s;
}

static const char *GetPrologTermStringStr(term_t term) {
  char *s;
  assert(PL_get_chars(term, &s, CVT_STRING));
  return s;
}

static PrologTerm::PrintFnTy GetAtomPrintFn() {
  return [](std::ostream &os, term_t term) {
    os << GetPrologTermAtomStr(term);
  };
}

static PrologTerm::PrintFnTy GetStringPrintFn() {
  return [](std::ostream &os, term_t term) {
    os << '"' << GetPrologTermStringStr(term) << '"';
  };
}

static PrologTerm::PrintFnTy GetVariablePrintFn() {
  return [](std::ostream &os, term_t term) {
    char *s;
    assert(PL_get_chars(term, &s, CVT_VARIABLE));
    os << s;
  };
}

static PrologTerm::PrintFnTy GetTermPrintFn(size_t size) {
  return [size](std::ostream &os, term_t term) {
    for (unsigned i = 0; i < size; ++i) {
      if (i > 0) {
        os << ", ";
      }
      os << PrologTerm::from(term + i);
    }
  };
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
 * PrologTerm
 */
PrologTerm PrologTerm::from(term_t term) {
  switch (PL_term_type(term)) {
  case PL_ATOM:
    return PrologAtom(term);
  case PL_VARIABLE:
   return PrologVariable(term);
  case PL_STRING:
    return PrologString(term);
  default:
    assert(false && "No know conversion from term type");
  }
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

/**
 * PrologAtom
 */
PrologAtom::PrologAtom(term_t term)
    : PrologTerm(term, GetAtomPrintFn()) {
}

PrologAtom::PrologAtom()
    : PrologTerm(GetAtomPrintFn()) {
}

PrologAtom PrologAtom::fromString(std::string name) {
  return fromString(name.c_str());
}

PrologAtom PrologAtom::fromString(const char *name) {
  return fromPrologAtom(PL_new_atom(name));
}

PrologAtom PrologAtom::fromPrologAtom(atom_t atom) {
  PrologAtom patom;
  assert(PL_put_atom(patom.getInternalTerm(), atom));
  return patom;
}

std::string PrologAtom::getAtomStr() const {
  return GetPrologTermAtomStr(getInternalTerm());
}

/**
 * PrologString
 */
PrologString::PrologString(std::string string)
    : PrologString(string.c_str()) {
}

PrologString::PrologString(const char *string)
    : PrologTerm(GetStringPrintFn()) {
  assert(PL_put_string_chars(getInternalTerm(), string));
}

PrologString::PrologString(term_t term)
    : PrologTerm(term, GetStringPrintFn()) {
}

std::string PrologString::getStr() const {
  return GetPrologTermStringStr(getInternalTerm());
}

/**
 * PrologVariable
 */
PrologVariable::PrologVariable()
    : PrologTerm(GetVariablePrintFn()) {
  PL_put_variable(getInternalTerm());
}

PrologVariable::PrologVariable(term_t term)
    : PrologTerm(term, GetVariablePrintFn()) {
}

/**
 * PrologTermVector
 */
PrologTermVector::PrologTermVector(size_t size)
    : PrologTerm(PL_new_term_refs(size), GetTermPrintFn(size)), size_(size) {
  term_t terms = getInternalTerm();
  for (unsigned i = 0; i < size_; ++i) {
    assert(PL_put_variable(terms + i));
  }
}

PrologTermVector::PrologTermVector(std::initializer_list<PrologTerm> args)
    : PrologTermVector(std::vector<PrologTerm>(args)) {
}

PrologTermVector::PrologTermVector(std::vector<PrologTerm> args)
    : PrologTermVector(args.size()) {
  term_t terms = getInternalTerm();
  for (unsigned i = 0; i < size_; ++i) {
    assert(PL_put_term(terms + i, args[i].getInternalTerm()));
  }
}

PrologTermVector::PrologTermVector(term_t term, size_t size)
    : PrologTerm(term, GetTermPrintFn(size)), size_(size) {
}

size_t PrologTermVector::size() const {
  return size_;
}

PrologTermHolder PrologTermVector::getAt(size_t idx) const {
  assert(idx < size());
  return PrologTermHolder(getInternalTerm() + idx);
}

