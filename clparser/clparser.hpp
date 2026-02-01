#pragma once

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

  enum class OptLevel { NONE, ONE, TWO, THREE };

  void create_venv(CompileArgs args);
  void destroy_venv(CompileArgs args);
  bool ask();
  void build(CompileArgs args);
  void help();
  void version();
  void run(int argc, char** argv);
}