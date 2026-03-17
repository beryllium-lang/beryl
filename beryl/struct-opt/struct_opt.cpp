#include "struct_opt.hpp"
#include <algorithm>

namespace beryl {
  static void move_el(std::vector<Field>& fields, size_t from, size_t to) {
    if (from == to) return;
    Field temp = fields[from];
    if (from < to) {
      for (size_t i = from; i < to; i++) { fields[i] = fields[i + 1]; }
    } else {
      for (size_t i = from; i > to; i--) { fields[i] = fields[i - 1]; }
    }
    fields[to] = temp;
  }

  void opt_layout(std::vector<Field>& fields) {
    std::stable_sort(fields.begin(), fields.end(), [](const Field& a, const Field& b) { return a.get_net_size() > b.get_net_size(); });

    size_t current_offset = 0;
    for (size_t i = 0; i < fields.size(); ++i) {
      current_offset = (current_offset + fields[i].alignment - 1) & ~(fields[i].alignment - 1);
      current_offset += fields[i].size;
      if (i + 1 < fields.size()) {
        size_t gap = (current_offset + fields[i + 1].alignment - 1) & ~(fields[i + 1].alignment - 1) - current_offset;
        if (gap > 0) {
          for (size_t j = i + 1; j < fields.size(); ++j) {
            if (fields[j].size <= gap) {
              move_el(fields, j, i + 1);
            } else
              break;
          }
        }
      }
    }
  }

  std::pair<size_t, size_t> compute_struct_size(const std::vector<Field>& fields) {
    size_t offset = 0;
    size_t struct_alignment = 1;

    for (const auto& field : fields) {
      struct_alignment = std::max(struct_alignment, field.alignment);
      offset = (offset + field.alignment - 1) & ~(field.alignment - 1);
      offset += field.size;
    }

    offset = (offset + struct_alignment - 1) & ~(struct_alignment - 1);
    return {offset, struct_alignment};
  }
} // namespace beryl