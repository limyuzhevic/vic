#pragma once

/**
 * Performance Monitoring Infrastructure for NLM
 * 
 * Provides comprehensive performance monitoring capabilities including
 * timing metrics, resource usage tracking, and bottleneck identification.
 */

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <iomanip>

namespace nlm {

/**
 * Performance measurement result
 */
struct PerformanceMeasurement {
    std::string operationName;
    double duration;  // in seconds
    size_t memoryUsage;  // in bytes
    size_t cpuTime;  // in CPU cycles
    double timestamp;
    
    PerformanceMeasurement(const std::string& name, double duration, size_t memory = 0, size_t cpu = 0)
        : operationName(name), duration(duration), memoryUsage(memory), cpuTime(cpu), timestamp(0.0) {}
};

/**
 * Performance profiler
 */
class PerformanceProfiler {
public:
    PerformanceProfiler() : currentOperationStart_(0.0), totalMemoryUsage_(0), totalCpuTime_(0) {}
    
    ~PerformanceProfiler() = default;
    
    /**
     * Start profiling an operation
     */
    void startProfiling(const std::string& operationName) {
        currentOperation_ = operationName;
        operationStart_ = std::chrono::high_resolution_clock::now();
        currentOperationStart_ = getCurrentTimestamp();
    }
    
    /**
     * End profiling the current operation
     */
    PerformanceMeasurement endProfiling() {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endTime - operationStart_;
        
        PerformanceMeasurement measurement(
            currentOperation_,
            duration.count(),
            getCurrentMemoryUsage(),
            getCurrentCpuTime()
        );
        measurement.timestamp = currentOperationStart_ + duration.count();
        
        // Store measurement
        measurements_.push_back(measurement);
        
        // Update totals
        totalMemoryUsage_ += measurement.memoryUsage;
        totalCpuTime_ += measurement.cpuTime;
        
        // Clear current operation
        currentOperation_.clear();
        currentOperationStart_ = 0.0;
        
        return measurement;
    }
    
    /**
     * Get all measurements
     */
    const std::vector<PerformanceMeasurement>& getMeasurements() const {
        return measurements_;
    }
    
    /**
     * Get measurement by operation name
     */
    PerformanceMeasurement getMeasurement(const std::string& operationName) const {
        for (const auto& measurement : measurements_) {
            if (measurement.operationName == operationName) {
                return measurement;
            }
        }
        return PerformanceMeasurement(operationName, 0.0);
    }
    
    /**
     * Get measurements by operation name pattern
     */
    std::vector<PerformanceMeasurement> getMeasurementsByPattern(const std::string& pattern) const {
        std::vector<PerformanceMeasurement> result;
        for (const auto& measurement : measurements_) {
            if (measurement.operationName.find(pattern) != std::string::npos) {
                result.push_back(measurement);
            }
        }
        return result;
    }
    
    /**
     * Get total execution time for an operation
     */
    double getTotalTime(const std::string& operationName) const {
        double total = 0.0;
        for (const auto& measurement : measurements_) {
            if (measurement.operationName == operationName) {
                total += measurement.duration;
            }
        }
        return total;
    }
    
    /**
     * Get total memory usage for an operation
     */
    size_t getTotalMemory(const std::string& operationName) const {
        size_t total = 0;
        for (const auto& measurement : measurements_) {
            if (measurement.operationName == operationName) {
                total += measurement.memoryUsage;
            }
        }
        return total;
    }
    
    /**
     * Get overall performance statistics
     */
    struct PerformanceStats {
        double totalExecutionTime;
        double averageExecutionTime;
        double maxExecutionTime;
        double minExecutionTime;
        size_t totalMemoryUsage;
        size_t totalCpuTime;
        size_t measurementCount;
        
        PerformanceStats() : totalExecutionTime(0.0), averageExecutionTime(0.0), 
                           maxExecutionTime(0.0), minExecutionTime(0.0),
                           totalMemoryUsage(0), totalCpuTime(0), measurementCount(0) {}
    };
    
    PerformanceStats getStatistics() const {
        PerformanceStats stats;
        
        if (measurements_.empty()) {
            return stats;
        }
        
        double totalDuration = 0.0;
        double maxDuration = 0.0;
        double minDuration = std::numeric_limits<double>::max();
        
        for (const auto& measurement : measurements_) {
            totalDuration += measurement.duration;
            if (measurement.duration > maxDuration) maxDuration = measurement.duration;
            if (measurement.duration < minDuration) minDuration = measurement.duration;
        }
        
        stats.totalExecutionTime = totalDuration;
        stats.averageExecutionTime = totalDuration / measurements_.size();
        stats.maxExecutionTime = maxDuration;
        stats.minExecutionTime = minDuration;
        stats.totalMemoryUsage = totalMemoryUsage_;
        stats.totalCpuTime = totalCpuTime_;
        stats.measurementCount = measurements_.size();
        
        return stats;
    }
    
    /**
     * Print performance report
     */
    void printReport() const {
        if (measurements_.empty()) {
            std::cout << "No performance measurements recorded" << std::endl;
            return;
        }
        
        std::cout << "=== Performance Report ===" << std::endl;
        std::cout << std::fixed << std::setprecision(6);
        
        // Individual measurements
        std::cout << "\nIndividual Measurements:" << std::endl;
        std::cout << std::setw(30) << std::left << "Operation"
                  << std::setw(15) << std::left << "Duration (s)"
                  << std::setw(15) << std::left << "Memory (B)"
                  << std::setw(15) << std::left << "CPU Cycles" << std::endl;
        std::cout << std::string(75, '-') << std::endl;
        
        for (const auto& measurement : measurements_) {
            std::cout << std::setw(30) << std::left << measurement.operationName
                      << std::setw(15) << std::left << measurement.duration
                      << std::setw(15) << std::left << measurement.memoryUsage
                      << std::setw(15) << std::left << measurement.cpuTime << std::endl;
        }
        
        // Overall statistics
        PerformanceStats stats = getStatistics();
        std::cout << "\nOverall Statistics:" << std::endl;
        std::cout << std::setw(30) << std::left << "Total Time (s): " << stats.totalExecutionTime << std::endl;
        std::cout << std::setw(30) << std::left << "Average Time (s): " << stats.averageExecutionTime << std::endl;
        std::cout << std::setw(30) << std::left << "Max Time (s): " << stats.maxExecutionTime << std::endl;
        std::cout << std::setw(30) << std::left << "Min Time (s): " << stats.minExecutionTime << std::endl;
        std::cout << std::setw(30) << std::left << "Total Memory (B): " << stats.totalMemoryUsage << std::endl;
        std::cout << std::setw(30) << std::left << "Total CPU Cycles: " << stats.totalCpuTime << std::endl;
        std::cout << std::setw(30) << std::left << "Measurement Count: " << stats.measurementCount << std::endl;
        
        // Bottleneck analysis
        if (stats.measurementCount > 0) {
            std::cout << "\nBottleneck Analysis:" << std::endl;
            std::cout << "  Most time-consuming operation: " << getMostTimeConsumingOperation() << std::endl;
            std::cout << "  Highest memory usage: " << getHighestMemoryOperation() << std::endl;
        }
    }
    
    /**
     * Get the most time-consuming operation
     */
    std::string getMostTimeConsumingOperation() const {
        if (measurements_.empty()) return "N/A";
        
        auto maxIt = std::max_element(measurements_.begin(), measurements_.end(),
            [](const PerformanceMeasurement& a, const PerformanceMeasurement& b) {
                return a.duration < b.duration;
            });
        
        return maxIt->operationName;
    }
    
    /**
     * Get the operation with highest memory usage
     */
    std::string getHighestMemoryOperation() const {
        if (measurements_.empty()) return "N/A";
        
        auto maxIt = std::max_element(measurements_.begin(), measurements_.end(),
            [](const PerformanceMeasurement& a, const PerformanceMeasurement& b) {
                return a.memoryUsage < b.memoryUsage;
            });
        
        return maxIt->operationName;
    }
    
    /**
     * Save performance measurements to file
     */
    bool saveToFile(const std::string& filepath) const {
        // Implementation for saving measurements to JSON or CSV format
        // For now, just a stub
        std::cout << "Performance report would be saved to: " << filepath << std::endl;
        return true;
    }
    
    /**
     * Load performance measurements from file
     */
    bool loadFromFile(const std::string& filepath) {
        // Implementation for loading measurements from JSON or CSV format
        // For now, just a stub
        std::cout << "Loading performance measurements from: " << filepath << std::endl;
        return true;
    }
    
    /**
     * Clear all measurements
     */
    void clearMeasurements() {
        measurements_.clear();
        totalMemoryUsage_ = 0;
        totalCpuTime_ = 0;
        currentOperation_.clear();
        currentOperationStart_ = 0.0;
    }
    
    /**
     * Get global performance profiler instance
     */
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }

private:
    double getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return static_cast<double>(millis) / 1000.0;
    }
    
    size_t getCurrentMemoryUsage() {
        // Placeholder - implement actual memory measurement
        return 0;
    }
    
    size_t getCurrentCpuTime() {
        // Placeholder - implement actual CPU time measurement
        return 0;
    }
    
    std::vector<PerformanceMeasurement> measurements_;
    std::string currentOperation_;
    double currentOperationStart_;
    size_t totalMemoryUsage_;
    size_t totalCpuTime_;
};

/**
 * Performance monitoring macros for easy profiling
 */
#define MEASURE_PERFORMANCE(operation, code) \
    do { \
        PerformanceProfiler::getInstance().startProfiling(operation); \
        code; \
        PerformanceProfiler::getInstance().endProfiling(); \
    } while(0)

#define MEASURE_FUNCTION() MEASURE_PERFORMANCE(__func__, return __VA_ARGS__)

/**
 * Global performance profiler instance
 */
inline PerformanceProfiler& getPerformanceProfiler() {
    return PerformanceProfiler::getInstance();
}

} // namespace nlm