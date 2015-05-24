#pragma once

#include <SWI-Prolog.h>

class PrologLifetime {
public:
  static void begin(int argc, char *argv[]);
  static void end();
};

