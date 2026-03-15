#pragma once
#include "arena.hpp"
#include "error.hpp"
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <new>

namespace beryl {
  template <typename T> class ArenaAllocator {
  public:
    using value_type = T;

    Arena* arena;

    explicit ArenaAllocator(Arena& a) noexcept : arena(&a) {}

    template <typename U> ArenaAllocator(const ArenaAllocator<U>& other) noexcept : arena(other.arena) {}

    T* allocate(std::size_t n) {
      if (n > 0 && n > std::numeric_limits<std::size_t>::max() / sizeof(T)) { throw_arg_read_error("ArenaAllocator allocation size overflow"); }
      void* ptr = arena->allocate_bytes(n * sizeof(T), alignof(T));
      if (!ptr) throw std::bad_alloc();
      return static_cast<T*>(ptr);
    }

    void deallocate(T*, std::size_t) noexcept {}

    template <typename U> bool operator==(const ArenaAllocator<U>& other) const noexcept { return arena == other.arena; }

    template <typename U> bool operator!=(const ArenaAllocator<U>& other) const noexcept { return arena != other.arena; }
  };
} // namespace beryl
