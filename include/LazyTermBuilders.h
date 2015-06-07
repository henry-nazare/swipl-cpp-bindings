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

