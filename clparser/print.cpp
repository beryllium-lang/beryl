#include "clparser.hpp"
#include <iostream>

namespace beryl {
  bool ask() {
    std::cout << "Are you sure? Y/n: ";
    char c;
    std::cin >> c;
    return c == 'Y' || c == 'y';
  }

  void version() {
    std::cout << "Beryl 1.0.0" << std::endl;
  }

  void help() {
    std::cout << "Beryl help page\n"
      "beryl create: creates a virtual environment\n"
      "    --no-stdlib: does not install the standard library\n"
      "beryl destroy: removes the virtual environment\n"
      "    --force: removes the prompt\n"
      "beryl build: builds a file\n"
      "    --no-link: does not link the object file\n"
      "    --force-module-recompile: recompiles all modules\n"
      "    -O0: no optimization\n"
      "    -O1: one level optimization\n"
      "    -O2: two levels of optimization\n"
      "    -O3: three levels of optimization\n"
      "    -includes= : include paths provided in a JSON file\n"
      "    -out= : output file, with no linking, object file, with linking, executable\n"
      "    -std= : Beryllium standard to use. Valid values are be1. Defaults to latest version, currently be1\n"
      "help: displays this help page\n"
      "version/--version/-v: displays the version information\n";
  }
}