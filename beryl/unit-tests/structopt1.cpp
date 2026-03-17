#include "struct-opt/struct_opt.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(StructOpt, Test1) {
  std::vector<beryl::Field> fields = {{8, 8}, {3, 4}, {3, 4}, {1, 1},
                                      {1, 1}, {4, 4}, {2, 2}};

  beryl::opt_layout(fields);

  std::cout << "Optimized field order:\n";
  for (const auto &field : fields) {
    std::cout << "Size: " << field.size << ", Alignment: " << field.alignment
              << "\n";
  }

  auto [total_size, struct_alignment] = beryl::compute_struct_size(fields);
  EXPECT_EQ(total_size, 24);
}