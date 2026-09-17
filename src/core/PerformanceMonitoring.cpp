// Performance monitoring implementation
// Provides concrete implementations for performance tracking

#include "PerformanceMonitoring.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

// Implementation of PerformanceMonitor methods

PerformanceMetrics PerformanceMonitor::getMetrics(const std::string& component) const {
    PerformanceMetrics metrics;
    auto it = component_metrics.find(component);
    if (it != component_metrics.end()) {
        metrics = it->second;
    }
    metrics.calculateStatistics();
    return metrics;
}

PerformanceMetrics PerformanceMonitor::getGlobalMetrics() const {
    PerformanceMetrics metrics = global_metrics;
    metrics.calculateStatistics();
    return metrics;
}

// Implementation of Timer methods (inline in header)

// Performance monitor singleton instance
PerformanceMonitor& getPerformanceMonitor() {
    static PerformanceMonitor monitor;
    return monitor;
}

} // namespace nlm
