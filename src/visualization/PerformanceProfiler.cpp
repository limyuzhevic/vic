#include "PerformanceProfiler.hpp"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <sstream>

namespace nlm {

struct PerformanceProfiler::Impl {
    // Performance tracking
    PerformanceMetrics currentMetrics;
    std::chrono::steady_clock::time_point sessionStartTime;
    std::chrono::duration<double> totalCpuTime;
    
    // Timing data
    std::map<std::string, std::chrono::steady_clock::time_point> activeTimers;
    std::map<std::string, std::chrono::duration<double>> accumulatedTimes;
    
    // Benchmark history
    std::vector<BenchmarkResult> benchmarkHistory;
    
    // Metric history (rolling window)
    std::unordered_map<std::string, std::deque<double>> metricHistory;
    size_t maxHistorySize = 100;
    
    // Memory tracking
    size_t currentMemoryAllocations = 0;
    size_t totalMemoryAllocations = 0;
    
    // System resources simulation
    std::chrono::steady_clock::time_point lastResourceUpdate;
    
    Impl() {
        sessionStartTime = std::chrono::steady_clock::now();
        lastResourceUpdate = sessionStartTime;
    }
};

PerformanceProfiler::PerformanceProfiler() : pImpl(new Impl) {}

PerformanceProfiler::~PerformanceProfiler() = default;

bool PerformanceProfiler::initialize() {
    return true;
}

void PerformanceProfiler::update(double deltaTime) {
    // Simulate performance metrics updates
    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - pImpl->lastResourceUpdate;
    
    // Update simulated system metrics
    pImpl->currentMetrics.wallClockTime = std::chrono::duration<double>(deltaTime);
    pImpl->currentMetrics.cpuTime = std::chrono::duration<double>(deltaTime * 0.8); // Assume 80% CPU usage
    
    // Simulate fluctuating resource usage
    std::srand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    pImpl->currentMetrics.cpuUsage = 30.0 + (static_cast<double>(std::rand()) / RAND_MAX) * 60.0; // 30-90%
    pImpl->currentMetrics.memoryUsage = 20.0 + (static_cast<double>(std::rand()) / RAND_MAX) * 30.0; // 20-50%
    pImpl->currentMetrics.gpuUsage = 10.0 + (static_cast<double>(std::rand()) / RAND_MAX) * 40.0; // 10-50%
    
    pImpl->currentMetrics.memoryAllocationCount = pImpl->totalMemoryAllocations;
    
    // Update object counts
    pImpl->currentMetrics.objectCreationCount = pImpl->totalMemoryAllocations / 3;
    pImpl->currentMetrics.objectDestructionCount = pImpl->totalMemoryAllocations / 4;
    
    pImpl->lastResourceUpdate = now;
    
    // Record system metrics
    recordMetric("cpu_usage", pImpl->currentMetrics.cpuUsage);
    recordMetric("memory_usage", pImpl->currentMetrics.memoryUsage);
    recordMetric("wall_clock_time", deltaTime);
}

void PerformanceProfiler::render() {
    std::cout << "=== Performance Profiler ===" << std::endl;
    
    std::cout << "Current Performance Metrics:" << std::endl;
    std::cout << "  CPU Usage: " << std::fixed << std::setprecision(1) << pImpl->currentMetrics.cpuUsage << "%" << std::endl;
    std::cout << "  Memory Usage: " << std::fixed << std::setprecision(1) << pImpl->currentMetrics.memoryUsage << "%" << std::endl;
    std::cout << "  GPU Usage: " << std::fixed << std::setprecision(1) << pImpl->currentMetrics.gpuUsage << "%" << std::endl;
    std::cout << "  Wall Clock Time: " << std::scientific << pImpl->currentMetrics.wallClockTime.count() << "s" << std::endl;
    std::cout << "  CPU Time: " << std::scientific << pImpl->currentMetrics.cpuTime.count() << "s" << std::endl;
    std::cout << "  Memory Allocations: " << pImpl->currentMetrics.memoryAllocationCount << std::endl;
    std::cout << "  Objects Created: " << pImpl->currentMetrics.objectCreationCount << std::endl;
    std::cout << "  Objects Destroyed: " << pImpl->currentMetrics.objectDestructionCount << std::endl;
    
    // Active timers
    std::cout << std::endl << "Active Timers:" << std::endl;
    for (const auto& pair : pImpl->activeTimers) {
        auto elapsed = std::chrono::duration<double>(now - pair.second);
        std::cout << "  " << pair.first << ": " << std::fixed << elapsed.count() << "s" << std::endl;
    }
    
    // Recent benchmarks
    if (!pImpl->benchmarkHistory.empty()) {
        std::cout << std::endl << "Recent Benchmarks:" << std::endl;
        for (const auto& bench : pImpl->benchmarkHistory) {
            std::cout << "  " << bench.testName << ": " 
                      << std::fixed << bench.executionTime.count() << "s "
                      << "Score: " << bench.performanceScore << " "
                      << "Memory: " << bench.memoryUsed << "B "
                      << "Passed: " << (bench.passed ? "Yes" : "No") << std::endl;
        }
    }
    
    std::cout << std::endl << getPerformanceSummary() << std::endl;
}

void PerformanceProfiler::close() {
    pImpl->activeTimers.clear();
    pImpl->accumulatedTimes.clear();
    pImpl->benchmarkHistory.clear();
    for (auto& pair : pImpl->metricHistory) {
        pair.second.clear();
    }
}

void PerformanceProfiler::startTiming(const std::string& sectionName) {
    auto now = std::chrono::steady_clock::now();
    pImpl->activeTimers[sectionName] = now;
}

BenchmarkResult PerformanceProfiler::stopTiming(const std::string& sectionName) {
    BenchmarkResult result;
    result.testName = sectionName;
    result.passed = false; // Default to false - caller should set
    
    auto it = pImpl->activeTimers.find(sectionName);
    if (it != pImpl->activeTimers.end()) {
        auto endTime = std::chrono::steady_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - it->second);
        
        // Add to accumulated times
        pImpl->accumulatedTimes[sectionName] += result.executionTime;
        
        // Calculate performance score (inverse of time, normalized)
        result.performanceScore = 1000.0 / (1.0 + result.executionTime.count());
        result.memoryUsed = static_cast<size_t>(result.performanceScore * 100); // Simulated memory usage
        
        pImpl->activeTimers.erase(it);
    }
    
    return result;
}

void PerformanceProfiler::recordMetric(const std::string& metricName, double value) {
    pImpl->metricHistory[metricName].push_back(value);
    
    if (pImpl->metricHistory[metricName].size() > pImpl->maxHistorySize) {
        pImpl->metricHistory[metricName].pop_front();
    }
}

void PerformanceProfiler::addBenchmark(const BenchmarkResult& benchmark) {
    pImpl->benchmarkHistory.push_back(benchmark);
    
    if (pImpl->benchmarkHistory.size() > pImpl->maxHistorySize) {
        pImpl->benchmarkHistory.erase(pImpl->benchmarkHistory.begin());
    }
}

PerformanceMetrics PerformanceProfiler::getCurrentMetrics() const {
    return pImpl->currentMetrics;
}

const std::vector<BenchmarkResult>& PerformanceProfiler::getBenchmarkHistory() const {
    return pImpl->benchmarkHistory;
}

const std::unordered_map<std::string, std::deque<double>>& PerformanceProfiler::getMetricHistory() const {
    return pImpl->metricHistory;
}

std::string PerformanceProfiler::getPerformanceSummary() const {
    std::stringstream ss;
    
    ss << "Performance Summary:" << std::endl;
    
    // Average metrics
    if (!pImpl->metricHistory.empty()) {
        ss << "  Average CPU Usage: " << std::fixed << std::setprecision(1);
        double cpuSum = 0.0;
        size_t cpuCount = 0;
        for (const auto& pair : pImpl->metricHistory) {
            if (pair.first == "cpu_usage") {
                for (double val : pair.second) {
                    cpuSum += val;
                    cpuCount++;
                }
            }
        }
        if (cpuCount > 0) ss << (cpuSum / cpuCount) << "%" << std::endl;
        
        ss << "  Average Memory Usage: " << std::fixed << std::setprecision(1);
        double memSum = 0.0;
        size_t memCount = 0;
        for (const auto& pair : pImpl->metricHistory) {
            if (pair.first == "memory_usage") {
                for (double val : pair.second) {
                    memSum += val;
                    memCount++;
                }
            }
        }
        if (memCount > 0) ss << (memSum / memCount) << "%" << std::endl;
    }
    
    // Benchmark summary
    if (!pImpl->benchmarkHistory.empty()) {
        ss << "  Benchmarks Run: " << pImpl->benchmarkHistory.size() << std::endl;
        
        double totalScore = 0.0;
        size_t passed = 0;
        for (const auto& bench : pImpl->benchmarkHistory) {
            totalScore += bench.performanceScore;
            if (bench.passed) passed++;
        }
        
        ss << "  Average Score: " << std::fixed << (pImpl->benchmarkHistory.size() > 0 ? totalScore / pImpl->benchmarkHistory.size() : 0.0) << std::endl;
        ss << "  Passed: " << passed << "/" << pImpl->benchmarkHistory.size() << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
