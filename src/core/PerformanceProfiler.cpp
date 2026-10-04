#include "core/PerformanceProfiler.hpp"
#include <iomanip>
#include <limits>
#include <sstream>

namespace nlm {

// PerformanceCounter implementation
void PerformanceCounter::start() {
    if (enabled_) {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
}

void PerformanceCounter::stop() {
    if (enabled_) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time_);
        elapsed_time_ += duration.count();
    }
}

void PerformanceCounter::reset() {
    elapsed_time_ = 0;
}

} // namespace nlm