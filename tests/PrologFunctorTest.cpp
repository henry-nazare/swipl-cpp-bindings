#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

namespace {

class PrologFunctorTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologFunctorTest, Arity) {
  PrologFunctor functor(
      "f", PrologTermVector({PrologAtom::fromString("atom")}));
  ASSERT_EQ(functor.arity(), 1);
}

TEST_F(PrologFunctorTest, Name) {
  PrologFunctor functor(
      "f", PrologTermVector({PrologAtom::fromString("atom")}));
  ASSERT_EQ(functor.name(), "f");
}

TEST_F(PrologFunctorTest, Args) {
  PrologFunctor functor(
      "f", PrologTermVector(
          {PrologAtom::fromString("atom"), PrologString("string")}));
  PrologTermVector args = functor.args();
  ASSERT_EQ(functor.arity(), 2);
  ASSERT_EQ(args.size(), 2);
  ASSERT_EQ(args.at(0).asAtom().getAtomStr(), "atom");
  ASSERT_EQ(args.at(1).asString().getStr(), "string");
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

