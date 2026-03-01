#pragma once
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <utility>
#include "utils/error.hpp"

namespace beryl {
  class Arena {
  private:
    std::byte* start;
    size_t size;
    size_t offset;

    static size_t align_up(size_t n, size_t align) noexcept {
      assert(align != 0);
      assert((align & (align - 1)) == 0);
      if (n > std::numeric_limits<size_t>::max() - (align - 1)) { return n; }
      return (n + align - 1) & ~(align - 1);
    }

  public:
    explicit Arena(size_t sz)
        : start(static_cast<std::byte*>(std::malloc(sz))), size(start ? sz : 0), offset(0) {}

    Arena(Arena&& other) noexcept : start(other.start), size(other.size), offset(other.offset) {
      other.start = nullptr;
      other.size = 0;
      other.offset = 0;
    }

    Arena& operator=(Arena&& other) noexcept {
      if (this != &other) {
        std::free(start);
        start = other.start;
        size = other.size;
        offset = other.offset;
        other.start = nullptr;
        other.size = 0;
        other.offset = 0;
      }
      return *this;
    }

    ~Arena() noexcept { std::free(start); }

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    void reset() noexcept { offset = 0; }

    size_t capacity() const noexcept { return size; }
    size_t used() const noexcept { return offset; }
    size_t remaining() const noexcept { return size - offset; }

    void* allocate_bytes(size_t bytes, size_t alignment) noexcept {
      size_t aligned = align_up(offset, alignment);
      if (aligned > size || bytes > size - aligned) throw_arg_read_error("ran out of arena space");

      void* ptr = start + aligned;
      offset = aligned + bytes;
      return ptr;
    }

    template <typename T> T* alloc(size_t count = 1) noexcept {
      if (count > 0 && count > std::numeric_limits<size_t>::max() / sizeof(T)) { return nullptr; }
      return static_cast<T*>(allocate_bytes(sizeof(T) * count, alignof(T)));
    }

    template <typename T, typename... Args> T* emplace(Args&&... args) {
      T* ptr = alloc<T>();
      if (!ptr) return nullptr;
      return new (ptr) T(std::forward<Args>(args)...);
    }
  };
} // namespace beryl
