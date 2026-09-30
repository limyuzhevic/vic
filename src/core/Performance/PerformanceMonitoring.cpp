#include "PerformanceMonitoring.hpp"

namespace nlm {

PerformanceStats& getPerformanceStats() {
    return PerformanceStats::getInstance();
}

} // namespace nlm
