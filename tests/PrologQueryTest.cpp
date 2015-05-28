#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

static void addTestFacts() {
  PrologQuery
      ("assert", {PrologFunctor("test", {PrologAtom("a")})})
      .apply([](PrologTermVector){});
  PrologQuery
      ("assert", {PrologFunctor("test", {PrologAtom("b")})})
      .apply([](PrologTermVector){});
}

static int getNumFactsForAtom(const char *name) {
  int solutions = 0;
  PrologQuery("test", {PrologAtom(name)})
      .apply([&](PrologTermVector) {
        solutions = solutions + 1;
      });
  return solutions;
}

namespace {

class PrologQueryTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologQueryTest, AddFacts) {
  addTestFacts();
}

TEST_F(PrologQueryTest, AddAndRetrieveFacts) {
  addTestFacts();
  PrologVariable x;
  int solutions = 0;
  PrologQuery("test", {x})
      .apply([&](PrologTermVector) {
        ASSERT_TRUE((x.asAtom().getAtomStr() == "a")
            || (x.asAtom().getAtomStr() == "b"));
        solutions = solutions + 1;
      });
  ASSERT_EQ(solutions, 2);
}

TEST_F(PrologQueryTest, AddAndVerifyFacts) {
  addTestFacts();
  ASSERT_EQ(getNumFactsForAtom("a"), 1);
  ASSERT_EQ(getNumFactsForAtom("b"), 1);
  ASSERT_EQ(getNumFactsForAtom("c"), 0);
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

