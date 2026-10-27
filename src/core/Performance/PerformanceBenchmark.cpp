#include "PerformanceBenchmark.hpp"
#include "core/Logger/Logger.hpp"
#include <iomanip>
#include <algorithm>

using namespace nlm;

PerformanceBenchmark::PerformanceBenchmark() {
    sessionStart_ = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("Performance benchmark initialized");
}

void PerformanceBenchmark::startTiming(const std::string& operationName) {
    startTimes_[operationName] = std::chrono::high_resolution_clock::now();
}

double PerformanceBenchmark::stopTiming(const std::string& operationName) {
    auto it = startTimes_.find(operationName);
    if (it == startTimes_.end()) {
        NLM_LOG_WARNING("No timing found for operation: " + operationName);
        return 0.0;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto startTime = it->second;
    
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime);
    elapsedTimes_[operationName] = duration.count() / 1000.0;  // Convert to seconds
    
    startTimes_.erase(it);
    
    NLM_LOG_INFO("Completed " + operationName + " in " + 
                std::to_string(elapsedTimes_[operationName]) + "s");
    
    return elapsedTimes_[operationName];
}

double PerformanceBenchmark::getTiming(const std::string& operationName) const {
    auto it = elapsedTimes_.find(operationName);
    if (it == elapsedTimes_.end()) {
        return 0.0;
    }
    return it->second;
}

void PerformanceBenchmark::printBenchmarkResults() const {
    NLM_LOG_INFO("=== Performance Benchmark Results ===");
    NLM_LOG_INFO("Session time: " + std::to_string(getTotalTime()) + "s");
    
    for (const auto& pair : elapsedTimes_) {
        NLM_LOG_INFO(pair.first + ": " + std::to_string(pair.second) + "s");
    }
}

double PerformanceBenchmark::getTotalTime() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - sessionStart_);
    return duration.count() / 1000.0;
}

void PerformanceBenchmark::reset() {
    startTimes_.clear();
    elapsedTimes_.clear();
    sessionStart_ = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("Performance benchmark reset");
}