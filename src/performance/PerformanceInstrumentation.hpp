#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>

namespace nlm {

class PerformanceMonitor;

class PerformanceInstrumentation {
public:
    // Instrument brain operations for performance monitoring
    template<typename T, typename... Args>
    static T timeOperation(const std::string& operationName, const std::string& component,
                           std::function<T(Args...)> func, Args&&... args);
    
    // Instrument memory operations
    static void trackMemoryAllocation(const std::string& context, size_t bytes, bool isAllocation);
    
    // Instrument I/O operations
    static void trackIOOperation(const std::string& operation, double durationMs, size_t bytes);
    
    // Get current performance monitor instance
    static PerformanceMonitor& getMonitor();
};

} // namespace nlm