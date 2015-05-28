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

static PrologTerm::PrintFnTy GetFunctorPrintFn() {
  return [](std::ostream &os, term_t term) {
    term_t tmp = PL_new_term_ref();

    atom_t name;
    int arity;
    assert(PL_get_name_arity(term, &name, &arity));
    os << PL_atom_chars(name) << "(";

    for (unsigned i = 1; i <= arity; ++i) {
      assert(PL_get_arg(i, term, tmp));
      if (i > 1) {
        os << ", ";
      }
      os << PrologTerm::from(tmp);
    }
    os << ")";
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
  case PL_TERM:
    if (!PL_is_list(term)) {
      return PrologFunctor(term);
    }
    // Fall through.
  default:
    assert(false && "No know conversion from term type");
  }
}

/**
 * PrologTermHolder
 */
PrologTermHolder::PrologTermHolder(term_t term) : term_(term) {
}

PrologAtom PrologTermHolder::asAtom() const {
  return PrologAtom(term_);
}

PrologString PrologTermHolder::asString() const {
  return PrologString(term_);
}

PrologVariable PrologTermHolder::asVariable() const {
  return PrologVariable(term_);
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
PrologAtom::PrologAtom(std::string name)
    : PrologAtom(name.c_str()) {
}

PrologAtom::PrologAtom(const char *name)
    : PrologTerm(GetAtomPrintFn()) {
  assert(PL_put_atom(getInternalTerm(), PL_new_atom(name)));
}

PrologAtom::PrologAtom(term_t term)
    : PrologTerm(term, GetAtomPrintFn()) {
}

PrologAtom::PrologAtom()
    : PrologTerm(GetAtomPrintFn()) {
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

PrologTermHolder PrologTermVector::at(size_t idx) const {
  assert(idx < size());
  return PrologTermHolder(getInternalTerm() + idx);
}

/**
 * PrologFunctor
 */
PrologFunctor::PrologFunctor(std::string name, PrologTermVector args)
    : PrologTerm(GetFunctorPrintFn()) {
  assert(PL_cons_functor_v(
      getInternalTerm(),
      PL_new_functor(PL_new_atom(name.c_str()), args.size()),
      args.getInternalTerm()));
}

PrologFunctor::PrologFunctor(term_t term)
    : PrologTerm(term, GetFunctorPrintFn()) {
}

std::string PrologFunctor::name() const {
  atom_t name;
  int arity;
  assert(PL_get_name_arity(getInternalTerm(), &name, &arity));
  return PL_atom_chars(name);
}

size_t PrologFunctor::arity() const {
  atom_t name;
  int arity;
  assert(PL_get_name_arity(getInternalTerm(), &name, &arity));
  return arity;
}

PrologTermVector PrologFunctor::args() const {
  size_t len = arity();
  std::vector<PrologTerm> args;
  for (unsigned i = 1; i <= len; ++i) {
    term_t tmp = PL_new_term_ref();
    assert(PL_get_arg(i, getInternalTerm(), tmp));
    args.push_back(PrologTerm::from(tmp));
  }
  return PrologTermVector(args);
}

/**
 * PrologQuery
 */
PrologQuery::PrologQuery(std::string predicate, PrologTermVector terms)
    : PrologQuery(predicate.c_str(), terms) {
}

PrologQuery::PrologQuery(const char *predicate, PrologTermVector terms)
    : terms_(terms) {
  predicate_t pred = PL_predicate(predicate, terms.size(), "user");
  qid_ =
    PL_open_query((module_t) 0, PL_Q_NORMAL, pred, terms.getInternalTerm());
}

PrologQuery::PrologQuery(PrologFunctor functor)
    : PrologQuery(functor.name(), functor.args()) {
}

void PrologQuery::apply(std::function<void (PrologTermVector term)> function) {
  while (PL_next_solution(qid_)) {
    function(terms_);
  }
  PL_cut_query(qid_);
}

/**
 * PrologCall
 */
void PrologCall::run(PrologFunctor functor) {
  PrologQuery query(functor);
  query.apply([](PrologTermVector) {});
}

void PrologCall::fact(PrologTerm term) {
  run(PrologFunctor("assert", PrologTermVector({term})));
}

void PrologCall::consult(const char *filename) {
  PrologQuery query(
      "consult", PrologTermVector({PrologAtom(filename)}));
  query.apply([](PrologTermVector) {});
}

