#pragma once

/**
 * Performance Metrics Collection System for NLM Framework
 * 
 * This system provides comprehensive performance metrics collection:
 * - Real-time performance metrics collection
 * - Performance counters and timers
 * - Memory usage tracking
 * - CPU and I/O performance monitoring
 * - Statistics and reporting
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <thread>
#include <optional>
#include <functional>
#include <map>
#include <variant>
#include "../core/Config/Config.hpp"

namespace nlm {

/**
 * Performance metric data structure
 */
struct PerformanceMetric {
    std::string name;
    PerformanceCategory category;
    double value;
    double baseline;  // Reference value for comparison
    std::chrono::microseconds timestamp;
    std::string unit;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * Performance metrics collection system
 */
class PerformanceMetrics {
public:
    PerformanceMetrics();
    ~PerformanceMetrics();
    
    // Metric collection
    void recordMetric(const std::string& name, PerformanceCategory category,
                     double value, const std::string& unit = "",
                     const std::unordered_map<std::string, std::string>& metadata = {});
    double getMetric(const std::string& name) const;
    std::vector<PerformanceMetric> getMetrics(PerformanceCategory category) const;
    std::vector<PerformanceMetric> getAllMetrics() const;
    
    // Time measurement
    void startTimer(const std::string& timerName);
    std::chrono::microseconds stopTimer(const std::string& timerName);
    std::chrono::microseconds getTimerDuration(const std::string& timerName) const;
    std::unordered_map<std::string, std::chrono::microseconds> getAllTimers() const;
    
    // Memory measurement
    void recordMemorySnapshot();
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    std::vector<size_t> getMemoryHistory() const;
    
    // CPU and I/O measurement
    void recordCPUSample(double usage);
    void recordIOSample(size_t bytes, std::chrono::microseconds duration);
    double getAverageCPUUsage() const;
    std::vector<std::pair<size_t, std::chrono::microseconds>> getIOHistory() const;
    
    // Statistics
    size_t getMetricCount() const;
    size_t getTimerCount() const;
    std::chrono::microseconds getTotalMeasurementTime() const;
    
    // Export/import
    bool exportMetrics(const std::string& filename, OutputFormat format = OutputFormat::JSON) const;
    bool importMetrics(const std::string& filename);
    
    // Clear/reset
    void clearMetrics();
    void resetTimers();
    void clearMemoryHistory();
    
    // Singleton pattern
    static PerformanceMetrics& getInstance();
    static void setInstance(std::shared_ptr<PerformanceMetrics> metrics);
    static bool isInitialized();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor for singleton
    PerformanceMetrics();
    PerformanceMetrics(bool initialized);
};

} // namespace nlm

// Global performance metrics instance for convenience
nlm::PerformanceMetrics& getGlobalPerformanceMetrics();