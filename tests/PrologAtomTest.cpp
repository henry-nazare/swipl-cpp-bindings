#include "gtest/gtest.h"

#include "PrologInterface.h"

#include <sstream>

static int glob_argc;
static char **glob_argv;

namespace {

class PrologAtomTest : public ::testing::Test {
  virtual void SetUp() {
    PrologLifetime::begin(glob_argc, glob_argv);
  }

  virtual void TearDown() {
    PrologLifetime::end();
  }
};

TEST_F(PrologAtomTest, FromCharString) {
  PrologAtom atom = PrologAtom::fromString("atom");
  ASSERT_EQ(atom.getAtomStr(), "atom");
}

TEST_F(PrologAtomTest, FromStdString) {
  PrologAtom atom = PrologAtom::fromString(std::string("atom"));
  ASSERT_EQ(atom.getAtomStr(), "atom");
}

TEST_F(PrologAtomTest, FromPrologAtom) {
  PrologAtom atom = PrologAtom::fromPrologAtom(PL_new_atom("atom"));
  ASSERT_EQ(atom.getAtomStr(), "atom");
}

TEST_F(PrologAtomTest, PrintOstream) {
  std::stringstream ss;
  ss << PrologAtom::fromString("atom");
  ASSERT_EQ(ss.str(), "atom");
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  glob_argc = argc;
  glob_argv = argv;
  return RUN_ALL_TESTS();
}

