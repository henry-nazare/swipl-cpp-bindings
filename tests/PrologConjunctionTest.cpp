#include "gtest/gtest.h"

#include "PrologInterface.h"

#include "TestCommon.h"

#include <sstream>
#include <iostream>

using namespace testcommon;

static int glob_argc;
static char **glob_argv;

namespace {

class PrologConjunctionTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologConjunctionTest, TestConjunction) {
  addTestFacts();
  PrologVariable x, y;
  PrologFunctor query(PrologConjunction(
        PrologList({is_vowel_pred(x), next_alpha_pred(x, y)})));
  ASSERT_EQ(numSolutionsFor(query), 1);
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

