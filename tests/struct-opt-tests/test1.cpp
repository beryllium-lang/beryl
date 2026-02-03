#include "struct-opt/struct_opt.hpp"
#include <iostream>

int main() {
  std::vector<beryl::Field> fields = {
    {4, 4},
    {3, 1},
    {3, 1},
    {1, 1},
    {1, 1},
    {1, 1},
    {1, 1},
    {1, 1}
  };

  beryl::opt_layout(fields);

  std::cout << "Optimized field order:\n";
  for (const auto& field : fields) {
    std::cout << "Size: " << field.size << ", Alignment: " << field.alignment << "\n";
  }

  auto [total_size, struct_alignment] = beryl::compute_struct_size(fields);
  std::cout << "Total struct size: " << total_size << ", Struct alignment: " << struct_alignment << "\n";

  return 0;
}