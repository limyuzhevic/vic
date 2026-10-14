#include "Profiler.hpp"

namespace nlm {

PerformanceProfiler& PerformanceProfiler::getInstance() {
    static BasicPerformanceProfiler instance;
    return instance;
}

} // namespace nlm
