#include "timer.hpp"
#include <iostream>

namespace chrono = std::chrono;

namespace beryl {
  void Timer::stop() {
    if (!stopped) {
      stopped = true;
      auto end = chrono::steady_clock::now();
      auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
      std::cout << "Timer [" << name << "] finished in " << duration << " μs\n";
    }
  }
} // namespace beryl