#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "PrologInterface.h"

#include "TestCommon.h"

#include <sstream>

using namespace testing;
using namespace testcommon;

static int glob_argc;
static char **glob_argv;

namespace {

class PrologQueryTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologQueryTest, CountSolutionsIsAlpha) {
  addTestFacts();
  ASSERT_EQ(numSolutionsFor(is_alpha_pred(PrologAtom("a"))), 1);
  ASSERT_EQ(numSolutionsFor(is_alpha_pred(PrologAtom("b"))), 1);
  ASSERT_EQ(numSolutionsFor(is_alpha_pred(PrologAtom("c"))), 1);
  ASSERT_EQ(numSolutionsFor(is_alpha_pred(PrologAtom("d"))), 1);
  ASSERT_EQ(numSolutionsFor(is_alpha_pred(PrologAtom("e"))), 1);
  ASSERT_EQ(numSolutionsFor(is_alpha_pred(PrologAtom("f"))), 0);
  ASSERT_EQ(numSolutionsFor(is_alpha_pred(PrologVariable())), 5);
}

TEST_F(PrologQueryTest, CountSolutionsNextAlpha) {
  addTestFacts();
  ASSERT_EQ(
      numSolutionsFor(next_alpha_pred(PrologAtom("a"), PrologVariable())), 1);
  ASSERT_EQ(
      numSolutionsFor(next_alpha_pred(PrologAtom("a"), PrologAtom("b"))), 1);
  ASSERT_EQ(
      numSolutionsFor(next_alpha_pred(PrologAtom("a"), PrologAtom("c"))), 0);
  ASSERT_EQ(
      numSolutionsFor(next_alpha_pred(PrologVariable(), PrologVariable())), 4);
}

TEST_F(PrologQueryTest, VerifySolutionsIsAlpha) {
  addTestFacts();
  PrologVariable x;
  std::vector<PrologSolution> solutions =
      PrologQuery(is_alpha_pred(x)).solutions();
  std::vector<std::string> alphas(solutions.size());
  std::transform(
      solutions.begin(), solutions.end(), alphas.begin(),
      [&](PrologSolution solution) {
        return solution.get(x).asAtom().str();
      });
  ASSERT_THAT(alphas, ElementsAre("a", "b", "c", "d", "e"));
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

