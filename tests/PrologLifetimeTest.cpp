#include "gtest/gtest.h"

#include "PrologInterface.h"
#include <iostream>

namespace {

static int glob_argc;
static char **glob_argv;

class PrologLifetimeTest : public ::testing::Test {
};

TEST_F(PrologLifetimeTest, BeginInitializesProlog) {
  PrologLifetime::begin(glob_argc, glob_argv);
  ASSERT_TRUE(PL_is_initialised(&glob_argc, &glob_argv));
  PrologLifetime::end();
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

