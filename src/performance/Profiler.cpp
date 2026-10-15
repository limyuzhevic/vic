// Performance profiling utilities implementation
#include "Profiler.hpp"
#include <thread>
#include <fstream>
#include <numeric>
#include <algorithm>

namespace nlm {

Profiler gProfiler;

Profiler& getGlobalProfiler() {
    return gProfiler;
}

} // namespace nlm