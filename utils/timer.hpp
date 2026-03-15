#pragma once
#include <chrono>
#include <string_view>

namespace beryl {
  class Timer {
  private:
    std::chrono::time_point<std::chrono::steady_clock> start;
    std::string_view name;
    bool stopped = false;

  public:
    Timer(std::string_view name_) : start(std::chrono::steady_clock::now()), name(name_) {}
    ~Timer() { stop(); }

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

    void restart() {
      stopped = false;
      start = std::chrono::steady_clock::now();
    }

    void stop();
  };
} // namespace beryl