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

static PrologTerm::PrintFnTy GetListPrintFn() {
  return [](std::ostream &os, term_t term) {
    term_t list = PL_copy_term_ref(term);
    term_t head = PL_new_term_ref();
    os << "[";
    bool isListNil = PL_unify_nil(list);
    while (!isListNil && PL_unify_list(list, head, list)) {
      isListNil = PL_unify_nil(list);
      os <<  PrologTerm::from(head) << (isListNil ? "" : ", ");
    }
    os << "]";
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
  PrologCall::consult(SPC_PROLOG_DEFS_DIR "/lifetime_defs.pl");
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
    return PrologList(term);
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

PrologFunctor PrologTermHolder::asFunctor() const {
  return PrologFunctor(term_);
}

term_t PrologTermHolder::term() const {
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
  assert(PL_put_atom(term(), PL_new_atom(name)));
}

PrologAtom::PrologAtom(term_t term)
    : PrologTerm(term, GetAtomPrintFn()) {
}

PrologAtom::PrologAtom()
    : PrologTerm(GetAtomPrintFn()) {
}

PrologAtom PrologAtom::fromPrologAtom(atom_t atom) {
  PrologAtom patom;
  assert(PL_put_atom(patom.term(), atom));
  return patom;
}

std::string PrologAtom::str() const {
  return GetPrologTermAtomStr(term());
}

/**
 * PrologString
 */
PrologString::PrologString(std::string string)
    : PrologString(string.c_str()) {
}

PrologString::PrologString(const char *string)
    : PrologTerm(GetStringPrintFn()) {
  assert(PL_put_string_chars(term(), string));
}

PrologString::PrologString(term_t term)
    : PrologTerm(term, GetStringPrintFn()) {
}

std::string PrologString::str() const {
  return GetPrologTermStringStr(term());
}

/**
 * PrologVariable
 */
PrologVariable::PrologVariable()
    : PrologTerm(GetVariablePrintFn()) {
  PL_put_variable(term());
}

PrologVariable::PrologVariable(term_t term)
    : PrologTerm(term, GetVariablePrintFn()) {
}

/**
 * PrologList
 */
PrologList::PrologList(std::vector<PrologTerm> terms)
    : PrologTerm(GetListPrintFn()) {
  term_t head = term();
  PL_put_nil(head);
  for (auto it = terms.rbegin(), e = terms.rend(); it != e; ++it) {
    assert(PL_cons_list(head, (*it).term(), head));
  }
}

PrologList::PrologList(term_t term)
    : PrologTerm(term, GetListPrintFn()) {
}

/**
 * PrologTermVector
 */
PrologTermVector::PrologTermVector(size_t size)
    : PrologTerm(PL_new_term_refs(size), GetTermPrintFn(size)), size_(size) {
  term_t terms = term();
  for (unsigned i = 0; i < size_; ++i) {
    assert(PL_put_variable(terms + i));
  }
}

PrologTermVector::PrologTermVector(std::initializer_list<PrologTerm> args)
    : PrologTermVector(std::vector<PrologTerm>(args)) {
}

PrologTermVector::PrologTermVector(std::vector<PrologTerm> args)
    : PrologTermVector(args.size()) {
  term_t terms = term();
  for (unsigned i = 0; i < size_; ++i) {
    assert(PL_put_term(terms + i, args[i].term()));
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
  return PrologTermHolder(term() + idx);
}

/**
 * PrologFunctor
 */
PrologFunctor::PrologFunctor(std::string name, PrologTermVector args)
    : PrologTerm(GetFunctorPrintFn()) {
  assert(PL_cons_functor_v(
      term(),
      PL_new_functor(PL_new_atom(name.c_str()), args.size()),
      args.term()));
}

PrologFunctor::PrologFunctor(term_t term)
    : PrologTerm(term, GetFunctorPrintFn()) {
}

std::string PrologFunctor::name() const {
  atom_t name;
  int arity;
  assert(PL_get_name_arity(term(), &name, &arity));
  return PL_atom_chars(name);
}

size_t PrologFunctor::arity() const {
  atom_t name;
  int arity;
  assert(PL_get_name_arity(term(), &name, &arity));
  return arity;
}

PrologTermVector PrologFunctor::args() const {
  size_t len = arity();
  std::vector<PrologTerm> args;
  for (unsigned i = 1; i <= len; ++i) {
    term_t tmp = PL_new_term_ref();
    assert(PL_get_arg(i, term(), tmp));
    args.push_back(PrologTerm::from(tmp));
  }
  return PrologTermVector(args);
}

/**
 * PrologQuery
 */
PrologQuery::PrologQuery(std::string predicate, PrologTermVector terms)
    : predicate_(predicate), terms_(terms) {
}

PrologQuery::PrologQuery(PrologFunctor functor)
    : PrologQuery(functor.name(), functor.args()) {
}

void PrologQuery::apply(std::function<void (PrologTermVector term)> function) {
  predicate_t pred = PL_predicate(predicate_.c_str(), terms_.size(), "user");
  qid_t qid =
    PL_open_query((module_t) 0, PL_Q_NORMAL, pred, terms_.term());
  while (PL_next_solution(qid)) {
    function(terms_);
  }
  PL_cut_query(qid);
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

