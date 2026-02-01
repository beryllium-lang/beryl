#include "clparser.hpp"
#include "utils/error.hpp"
#include <string_view>

namespace beryl {
  void run(int argc, char* argv[]) {
    if (argc < 2) {
      beryl::help();
      return;
    }
    std::string_view command = argv[1];
    CompileArgs args{ argc, argv };
    if (command == "create") {
      create_venv(args);
    } else if (command == "destroy") {
      destroy_venv(args);
    } else if (command == "build") {
      build(args);
    } else if (command == "help") {
      help();
    } else if (command == "version" || command == "--version" || command == "-v") {
      version();
    } else {
      beryl::throw_arg_read_error("You typed an unknown command");
    }
  }
} // namespace beryl