#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>
#include <iostream>

static int glob_argc;
static char **glob_argv;

static PrologFunctor test(PrologTerm first, PrologTerm second) {
  return PrologFunctor("test", PrologTermVector({first, second}));
}

static PrologFunctor foo(PrologTerm first, PrologTerm second) {
  return PrologFunctor("foo", PrologTermVector({first, second}));
}

namespace {

class PrologConjunctionTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologConjunctionTest, AddAndRetrieveFacts) {
  PrologAtom a("a"), b("b"), c("c"), d("d");
  PrologCall::fact(test(a, b));
  PrologCall::fact(test(a, c));
  PrologCall::fact(test(a, d));
  PrologCall::fact(foo(a, a));
  PrologCall::fact(foo(a, b));
  PrologCall::fact(foo(a, c));

  PrologVariable x, y;
  int solutions = 0;
  PrologQuery(PrologConjunction(PrologList({test(x, y), foo(x, y)})))
      .apply([&](PrologTermVector) {
        solutions = solutions + 1;
      });
  ASSERT_EQ(solutions, 2);
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

