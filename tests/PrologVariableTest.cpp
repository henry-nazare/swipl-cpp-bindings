#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

static void verifyUnboundStr(PrologVariable variable) {
  std::stringstream ss;
  ss << PrologVariable();
  // Unbound variables should start with "_L".
  ASSERT_EQ(ss.str().substr(0, 2), "_L");
}

namespace {

class PrologVariableTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologVariableTest, DefautConstructor) {
  verifyUnboundStr(PrologVariable());
}

TEST_F(PrologVariableTest, FromPrologVariable) {
  term_t term = PL_new_term_ref();
  PL_put_variable(term);
  verifyUnboundStr(PrologVariable(term));
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

