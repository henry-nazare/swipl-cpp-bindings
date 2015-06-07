#pragma once

#include "PrologInterface.h"

class LazyTermBuilder {
public:
  virtual PrologTerm build() const =0;

  static LazyTermBuilder *from(term_t term);
};

class LazyAtomBuilder : public LazyTermBuilder {
public:
  LazyAtomBuilder(std::string name);

  virtual PrologTerm build() const;

  static LazyAtomBuilder *from(term_t term);

private:
  std::string name_;
};

class LazyStringBuilder : public LazyTermBuilder {
public:
  LazyStringBuilder(std::string string);

  virtual PrologTerm build() const;

  static LazyStringBuilder *from(term_t term);

private:
  std::string string_;
};

class LazyListBuilder : public LazyTermBuilder {
public:
  LazyListBuilder(std::vector<LazyTermBuilder*> elements);

  virtual PrologTerm build() const;

  static LazyListBuilder *from(term_t term);

private:
  std::vector<LazyTermBuilder*> elements_;
};

class LazyFunctorBuilder : public LazyTermBuilder {
public:
  LazyFunctorBuilder(std::string name, std::vector<LazyTermBuilder*> args);

  virtual PrologTerm build() const;

  static LazyFunctorBuilder *from(term_t term);

private:
  std::string name_;
  std::vector<LazyTermBuilder*> args_;
};

