// PerformanceProfiler implementation
// This is the core implementation of the performance profiling framework

#include "PerformanceProfiler.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

namespace nlm {

PerformanceProfiler::~PerformanceProfiler() = default;

} // namespace nlm