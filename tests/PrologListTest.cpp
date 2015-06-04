#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

namespace {

class PrologListTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologListTest, FromVector) {
  PrologList list({PrologAtom("atom"), PrologString("string")});
  std::stringstream ss;
  ss << list;
  ASSERT_EQ(ss.str(), "[atom, \"string\"]");
  ASSERT_TRUE(PL_is_list(list.term()));
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

