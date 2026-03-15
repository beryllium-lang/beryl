#pragma once
#include <vector>
#include <string>

namespace beryl {
  struct CompileArgs {
    int argc;
    char** argv;
  };

  struct Version {
    unsigned int major;
    unsigned int minor;

    bool operator<(Version other) const {
      if (major < other.major) return true;
      else if (major > other.major) return false;
      else if (minor < other.minor) return true;
      else return false;
    }

    bool operator>(Version other) const {
      if (major > other.major) return true;
      else if (major < other.major) return false;
      else if (minor > other.minor) return true;
      else return false;
    }

    bool operator==(Version other) const { return major == other.major && minor == other.minor; }
    bool operator!=(Version other) const { return !operator==(other); }
    bool operator<=(Version other) const { return !operator>(other); }
    bool operator>=(Version other) const { return !operator<(other); }
  };

  enum class OptLevel : unsigned char { NONE, ONE, TWO, THREE };

  struct BuildArgs {
    bool link = true;
    bool force_module_recompile = false;
    OptLevel opt_level = OptLevel::NONE;
    std::vector<std::string> filepath_prefixes;
    std::vector<std::string> input_files;
    std::string output_path;
    Version std_ver;
  };

  void create_venv(bool install_stdlib = true);
  void destroy_venv(bool force = false);
  bool ask();
  void build(BuildArgs args);
  void help();
  void version();
  void run(int argc, char** argv);
} // namespace beryl