#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

namespace {

class PrologTermTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologTermTest, FromPrologAtom) {
  term_t term = PL_new_term_ref();
  PL_put_atom_chars(term, "atom");

  std::stringstream ss;
  ss << PrologTerm::from(term);
  ASSERT_EQ(ss.str(), "atom");
}

TEST_F(PrologTermTest, FromPrologString) {
  term_t term = PL_new_term_ref();
  ASSERT_TRUE(PL_put_string_chars(term, "string"));

  std::stringstream ss;
  ss << PrologTerm::from(term);
  ASSERT_EQ(ss.str(), "\"string\"");
}

TEST_F(PrologTermTest, FromPrologVariable) {
  term_t term = PL_new_term_ref();
  PL_put_variable(term);

  std::stringstream ss;
  ss << PrologTerm::from(term);
  // Check that the term prints as an unbound variable.
  ASSERT_EQ(ss.str().substr(0, 2), "_L");
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

