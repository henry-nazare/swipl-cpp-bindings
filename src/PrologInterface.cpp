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

