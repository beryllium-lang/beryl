#include "parse_json_params.hpp"
#include "error.hpp"
#include "third-party/json.hpp"
#include <fstream>

namespace beryl {
  std::vector<std::string> get_includes(std::string_view path) {
    using json = nlohmann::json;
    std::ifstream file((std::string(path)));
    if (!file.is_open()) throw_arg_read_error("Could not open manifest: " + std::string(path));
    json data = json::parse(file);
    if (!data.contains("includes")) throw_arg_read_error("Configuration error: 'includes' key missing in " + std::string(path));
    auto& includes_node = data["includes"];
    if (!includes_node.is_array()) throw_arg_read_error("Type error: 'includes' must be an array in " + std::string(path));
    return includes_node.get<std::vector<std::string>>();
    return {};
  }
} // namespace beryl