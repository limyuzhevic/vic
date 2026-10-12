#include "PerformanceProfiler.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>

namespace nlm {

PerformanceProfiler::PerformanceProfiler()
    : enabled_(true)
    , isProfiling_(false)
    , currentOperationName_("") {}

void PerformanceProfiler::startProfiling(const std::string& operationName) {
    if (!enabled_) return;
    
    isProfiling_ = true;
    currentOperationName_ = operationName;
    operationMetrics_[operationName].startTime = std::chrono::steady_clock::now();
    
    if (PerformanceProfiler::getInstance().isEnabled()) {
        std::cout << "[PROFILING] Started profiling: " << operationName << std::endl;
    }
}

void PerformanceProfiler::stopProfiling(const std::string& operationName) {
    if (!isProfiling_ || currentOperationName_ != operationName) return;
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - operationMetrics_[operationName].startTime).count();
    
    OperationMetrics& metrics = operationMetrics_[operationName];
    metrics.totalTime += duration;
    metrics.minTime = std::min(metrics.minTime, duration);
    metrics.maxTime = std::max(metrics.maxTime, duration);
    metrics.callCount++;
    
    // Update detailed metrics
    if (detailedMetrics_.find(operationName) == detailedMetrics_.end()) {
        detailedMetrics_[operationName] = PerformanceMetrics{};
    }
    detailedMetrics_[operationName].stepTimeMs += duration;
    detailedMetrics_[operationName].simulationTimeMs += duration;
    
    // Update overall metrics
    overallMetrics_.stepTimeMs += duration;
    overallMetrics_.simulationTimeMs += duration;
    
    isProfiling_ = false;
    currentOperationName_ = "";
    
    if (PerformanceProfiler::getInstance().isEnabled()) {
        std::cout << "[PROFILING] Stopped profiling: " << operationName 
                  << " (duration: " << duration << "ms)" << std::endl;
    }
}

template<typename Func, typename... Args>
auto PerformanceProfiler::profile(const std::string& operationName, Func&& func, Args&&... args) {
    startProfiling(operationName);
    
    auto result = func(std::forward<Args>(args)...);
    
    stopProfiling(operationName);
    return result;
}

double PerformanceProfiler::getAverageTime(const std::string& operationName) const {
    auto it = operationMetrics_.find(operationName);
    if (it == operationMetrics_.end() || it->second.callCount == 0) return 0.0;
    return it->second.totalTime / it->second.callCount;
}

double PerformanceProfiler::getMinTime(const std::string& operationName) const {
    auto it = operationMetrics_.find(operationName);
    if (it == operationMetrics_.end() || it->second.callCount == 0) return 0.0;
    return it->second.minTime;
}

double PerformanceProfiler::getMaxTime(const std::string& operationName) const {
    auto it = operationMetrics_.find(operationName);
    if (it == operationMetrics_.end() || it->second.callCount == 0) return 0.0;
    return it->second.maxTime;
}

size_t PerformanceProfiler::getCallCount(const std::string& operationName) const {
    auto it = operationMetrics_.find(operationName);
    if (it == operationMetrics_.end()) return 0;
    return it->second.callCount;
}

void PerformanceProfiler::reset() {
    operationMetrics_.clear();
    detailedMetrics_.clear();
    overallMetrics_ = PerformanceMetrics{};
    
    if (PerformanceProfiler::getInstance().isEnabled()) {
        std::cout << "[PROFILING] Performance profiler reset" << std::endl;
    }
}

} // namespace nlm