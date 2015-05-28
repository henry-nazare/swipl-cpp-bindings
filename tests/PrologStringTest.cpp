#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

namespace {

class PrologStringTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologStringTest, FromCharString) {
  PrologString string("string");
  ASSERT_EQ(string.str(), "string");
}

TEST_F(PrologStringTest, FromStdString) {
  PrologString string(std::string("string"));
  ASSERT_EQ(string.str(), "string");
}

TEST_F(PrologStringTest, FromPrologString) {
  term_t term = PL_new_term_ref();
  ASSERT_TRUE(PL_put_string_chars(term, "string"));
  PrologString string(term);
  ASSERT_EQ(string.str(), "string");
}

TEST_F(PrologStringTest, PrintOstream) {
  std::stringstream ss;
  ss << PrologString("string");
  ASSERT_EQ(ss.str(), "\"string\"");
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

