#include "gtest/gtest.h"

#include "PrologInterface.h"

#include "TestCommon.h"

#include <sstream>

using namespace testcommon;

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
  PrologFunctor query("test", PrologTermVector({PrologVariable()}));
  ASSERT_EQ(numSolutionsFor(query), 3);
}

TEST_F(PrologCallTest, CompileMultiple) {
  PrologCall::compile("test", "test(x). test(y). test(z).");
  PrologCall::compile("foo", "foo(x). foo(y).");
  PrologFunctor testQuery("test", PrologTermVector({PrologVariable()}));
  PrologFunctor fooQuery("foo", PrologTermVector({PrologVariable()}));
  ASSERT_EQ(numSolutionsFor(testQuery), 3);
  ASSERT_EQ(numSolutionsFor(fooQuery), 2);
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

