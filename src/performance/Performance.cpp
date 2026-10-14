// Stub implementation for performance classes
// These are placeholders to satisfy CMake build requirements
// Phase 2 implementations will replace these stubs

#include "Performance.hpp"
#include <iostream>

namespace nlm {

// PerformanceManager implementation (stub)
PerformanceManager::PerformanceManager() {
    std::cout << "PerformanceManager initialized (stub)" << std::endl;
}

PerformanceManager::~PerformanceManager() = default;

void PerformanceManager::startMonitoring() {
    std::cout << "PerformanceManager::startMonitoring called (stub)" << std::endl;
}

void PerformanceManager::stopMonitoring() {
    std::cout << "PerformanceManager::stopMonitoring called (stub)" << std::endl;
}

PerformanceMetrics PerformanceManager::getMetrics() const {
    PerformanceMetrics metrics;
    metrics.totalMemoryUsage = 0;
    metrics.totalCPUUsage = 0.0f;
    metrics.frameTime = 0.0f;
    metrics.fps = 0.0f;
    return metrics;
}

} // namespace nlm