#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

static void verifyUnboundVariable(PrologTermHolder holder) {
  std::stringstream ss;
  ss << holder.asVariable();
  // Check that the term prints as an unbound variable.
  ASSERT_EQ(ss.str().substr(0, 2), "_L");
}

namespace {

class PrologTermVectorTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologTermVectorTest, UninitializedVector) {
  PrologTermVector vector(2);
  verifyUnboundVariable(vector.at(0));
  verifyUnboundVariable(vector.at(1));
}

TEST_F(PrologTermVectorTest, InitializedVector) {
  PrologTermVector vector(
      {PrologAtom::fromString("atom"), PrologString("string")});
  ASSERT_EQ(vector.size(), 2);
  std::stringstream ss;
  ss << vector.at(0).asAtom() << ", " << vector.at(1).asString();
  ASSERT_EQ(ss.str(), "atom, \"string\"");
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

