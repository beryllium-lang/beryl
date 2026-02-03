#pragma once
#include <vector>
#include <utility>

namespace beryl {
  struct Field {
    size_t size;
    size_t alignment;

    inline size_t get_net_size() const {
      return size > alignment ? size : alignment;
    }
  };

  void opt_layout(std::vector<Field>& fields);
  std::pair<size_t, size_t> compute_struct_size(const std::vector<Field>& fields);
}