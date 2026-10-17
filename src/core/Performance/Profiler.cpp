#include "Profiler.hpp"
#include <numeric>
#include <algorithm>
#include <limits>

namespace nlm {

// Implementation of PerformanceProfiler methods

PerformanceProfiler::PerformanceProfiler() : currentOperationStart_(0.0), totalMemoryUsage_(0), totalCpuTime_(0) {}

void PerformanceProfiler::startProfiling(const std::string& operationName) {
    currentOperation_ = operationName;
    operationStart_ = std::chrono::high_resolution_clock::now();
    currentOperationStart_ = getCurrentTimestamp();
}

PerformanceMeasurement PerformanceProfiler::endProfiling() {
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - operationStart_;
    
    PerformanceMeasurement measurement(
        currentOperation_,
        duration.count(),
        getCurrentMemoryUsage(),
        getCurrentCpuTime()
    );
    measurement.timestamp = currentOperationStart_ + duration.count();
    
    measurements_.push_back(measurement);
    totalMemoryUsage_ += measurement.memoryUsage;
    totalCpuTime_ += measurement.cpuTime;
    
    currentOperation_.clear();
    currentOperationStart_ = 0.0;
    
    return measurement;
}

void PerformanceProfiler::printReport() const {
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
}

std::string PerformanceProfiler::getMostTimeConsumingOperation() const {
    if (measurements_.empty()) return "N/A";
    
    auto maxIt = std::max_element(measurements_.begin(), measurements_.end(),
        [](const PerformanceMeasurement& a, const PerformanceMeasurement& b) {
            return a.duration < b.duration;
        });
    
    return maxIt->operationName;
}

std::string PerformanceProfiler::getHighestMemoryOperation() const {
    if (measurements_.empty()) return "N/A";
    
    auto maxIt = std::max_element(measurements_.begin(), measurements_.end(),
        [](const PerformanceMeasurement& a, const PerformanceMeasurement& b) {
            return a.memoryUsage < b.memoryUsage;
        });
    
    return maxIt->operationName;
}

bool PerformanceProfiler::saveToFile(const std::string& filepath) const {
    // Implementation for saving measurements to JSON or CSV format
    // For now, just a stub
    std::cout << "Performance report would be saved to: " << filepath << std::endl;
    return true;
}

bool PerformanceProfiler::loadFromFile(const std::string& filepath) {
    // Implementation for loading measurements from JSON or CSV format
    // For now, just a stub
    std::cout << "Loading performance measurements from: " << filepath << std::endl;
    return true;
}

double PerformanceProfiler::getCurrentTimestamp() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return static_cast<double>(millis) / 1000.0;
}

size_t PerformanceProfiler::getCurrentMemoryUsage() {
    // Placeholder - implement actual memory measurement
    return 0;
}

size_t PerformanceProfiler::getCurrentCpuTime() {
    // Placeholder - implement actual CPU time measurement
    return 0;
}

} // namespace nlm