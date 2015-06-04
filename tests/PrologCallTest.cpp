#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

namespace {

class PrologCallTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologCallTest, Compile) {
  PrologCall::compile("test", "test(x). test(y). test(z).");
  int solutions = 0;
  PrologQuery("test", PrologTermVector({PrologVariable()}))
      .apply([&](PrologTermVector) {
        solutions = solutions + 1;
      });
  ASSERT_EQ(solutions, 3);
}

TEST_F(PrologCallTest, CompileMultiple) {
  PrologCall::compile("test", "test(x). test(y). test(z).");
  PrologCall::compile("foo", "foo(x). foo(y).");
  int solutions = 0;
  PrologQuery("test", PrologTermVector({PrologVariable()}))
      .apply([&](PrologTermVector) {
        solutions = solutions + 1;
      });
  ASSERT_EQ(solutions, 3);
  solutions = 0;
  PrologQuery("foo", PrologTermVector({PrologVariable()}))
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

