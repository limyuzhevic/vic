#include "PerformanceInstrumentation.hpp"
#include "PerformanceMonitor.hpp"
#include <chrono>
#include <stdexcept>
#include <iostream>

namespace nlm {

template<typename T, typename... Args>
T PerformanceInstrumentation::timeOperation(const std::string& operationName,
                                           const std::string& component,
                                           std::function<T(Args...)> func,
                                           Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    
    T result;
    try {
        result = func(std::forward<Args>(args)...);
    } catch (const std::exception& e) {
        std::chrono::high_resolution_clock::now(); // Record time even on exception
        throw;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    // Get performance monitor instance
    auto& monitor = PerformanceMonitor::getInstance();
    
    // Track the operation
    std::string fullOperation = component + "::" + operationName;
    monitor.trackGenericOperation(fullOperation, duration.count(), component);
    
    return result;
}

void PerformanceInstrumentation::trackMemoryAllocation(const std::string& context,
                                                      size_t bytes,
                                                      bool isAllocation) {
    auto& monitor = PerformanceMonitor::getInstance();
    monitor.trackMemoryUsage(bytes, isAllocation, context);
}

void PerformanceInstrumentation::trackIOOperation(const std::string& operation,
                                                 double durationMs,
                                                 size_t bytes) {
    auto& monitor = PerformanceMonitor::getInstance();
    monitor.trackIO(operation, durationMs, bytes);
}

PerformanceMonitor& PerformanceInstrumentation::getMonitor() {
    return PerformanceMonitor::getInstance();
}

} // namespace nlm