#pragma once

#include <SWI-Prolog.h>

#include <initializer_list>
#include <ostream>
#include <vector>

class PrologAtom;
class PrologString;
class PrologVariable;

class PrologLifetime {
public:
  static void begin(int argc, char *argv[]);
  static void end();
};

class PrologTermHolder {
public:
  PrologTermHolder(term_t term);

  PrologAtom asAtom() const;
  PrologString asString() const;
  PrologVariable asVariable() const;

  term_t term() const;

protected:
   term_t term_;
};

class PrologTerm : public PrologTermHolder {
  friend std::ostream &operator<<(
      std::ostream &os, const PrologTerm &printable);

public:
  typedef std::function<void (std::ostream&, term_t)> PrintFnTy;

  static PrologTerm from(term_t term);

  void print(std::ostream &os) const;

protected:
  PrologTerm(PrintFnTy printFn);
  PrologTerm(term_t term, PrintFnTy printFn);

private:
  PrintFnTy printer_;
};

class PrologAtom : public PrologTerm {
public:
  PrologAtom(std::string name);
  PrologAtom(const char *name);
  PrologAtom(term_t term);

  static PrologAtom fromPrologAtom(atom_t atom);

  std::string getAtomStr() const;

protected:
  PrologAtom();
};

class PrologString : public PrologTerm {
public:
  PrologString(std::string string);
  PrologString(const char *string);
  PrologString(term_t term);

  std::string getStr() const;
};

class PrologVariable : public PrologTerm {
public:
  PrologVariable();
  PrologVariable(term_t term);
};

class PrologTermVector : public PrologTerm {
public:
  PrologTermVector(size_t size);
  PrologTermVector(std::initializer_list<PrologTerm> args);
  PrologTermVector(std::vector<PrologTerm> args);

  size_t size() const;

  PrologTermHolder at(size_t idx) const;

protected:
  PrologTermVector(term_t term, size_t size);

private:
  size_t size_;
};

class PrologFunctor : public PrologTerm {
public:
  PrologFunctor(std::string name, PrologTermVector args);
  PrologFunctor(term_t term);

  std::string name() const;
  size_t arity() const;
  PrologTermVector args() const;
};

class PrologQuery {
public:
  PrologQuery(std::string predicate, PrologTermVector terms);
  PrologQuery(const char *predicate, PrologTermVector terms);
  PrologQuery(PrologFunctor functor);

  void apply(std::function<void (PrologTermVector)> function);

private:
  qid_t qid_;
  PrologTermVector terms_;
};

class PrologCall {
public:
  static void run(PrologFunctor funcor);
  static void fact(PrologTerm term);
  static void consult(const char *filename);
};

