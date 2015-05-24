#include "PrologInterface.h"

void PrologLifetime::begin(int argc, char *argv[]) {
  if (!PL_initialise(argc, argv)) {
    PL_halt(EXIT_FAILURE);
  }
}

void PrologLifetime::end() {
  PL_halt(EXIT_SUCCESS);
}

