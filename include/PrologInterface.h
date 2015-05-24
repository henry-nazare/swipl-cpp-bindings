#pragma once

#include <SWI-Prolog.h>

#include <ostream>

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

  term_t getInternalTerm() const;

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
  PrologAtom(term_t term);

  static PrologAtom fromString(std::string name);
  static PrologAtom fromString(const char *name);
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

