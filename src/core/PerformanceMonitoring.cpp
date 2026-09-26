// Performance Monitoring and Benchmarking Tools
// This file provides comprehensive performance monitoring capabilities for NLM

#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <memory>
#include <mutex>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        std::string operation;
        double totalTimeMs;
        double averageTimeMs;
        double minTimeMs;
        double maxTimeMs;
        uint64_t callCount;
        uint64_t maxMemoryUsage;
        std::vector<double> recentTimes;
        
        PerformanceMetrics() : totalTimeMs(0.0), averageTimeMs(0.0), 
                              minTimeMs(std::numeric_limits<double>::max()),
                              maxTimeMs(0.0), callCount(0), maxMemoryUsage(0) {}
    };
    
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    void startMeasure(const std::string& operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        operationTimers_[operation] = std::chrono::high_resolution_clock::now();
    }
    
    void stopMeasure(const std::string& operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = operationTimers_.find(operation);
        if (it != operationTimers_.end()) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second);
            
            double timeMs = static_cast<double>(duration.count()) / 1000.0;
            
            if (metrics_.find(operation) == metrics_.end()) {
                metrics_[operation] = PerformanceMetrics();
            }
            
            PerformanceMetrics& metric = metrics_[operation];
            metric.operation = operation;
            metric.totalTimeMs += timeMs;
            metric.callCount++;
            
            if (timeMs < metric.minTimeMs) {
                metric.minTimeMs = timeMs;
            }
            if (timeMs > metric.maxTimeMs) {
                metric.maxTimeMs = timeMs;
            }
            
            metric.recentTimes.push_back(timeMs);
            if (metric.recentTimes.size() > 100) {
                metric.recentTimes.erase(metric.recentTimes.begin());
            }
            
            // Calculate average
            if (metric.callCount > 0) {
                metric.averageTimeMs = metric.totalTimeMs / static_cast<double>(metric.callCount);
            }
            
            operationTimers_.erase(it);
        }
    }
    
    const PerformanceMetrics& getMetrics(const std::string& operation) const {
        static PerformanceMetrics emptyMetrics;
        auto it = metrics_.find(operation);
        if (it != metrics_.end()) {
            return it->second;
        }
        return emptyMetrics;
    }
    
    std::vector<std::string> getAllOperations() const {
        std::vector<std::string> operations;
        operations.reserve(metrics_.size());
        for (const auto& pair : metrics_) {
            operations.push_back(pair.first);
        }
        return operations;
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== NLM Performance Report ===" << std::endl;
        oss << "Generated at: " << getCurrentTimestamp() << std::endl;
        oss << std::endl;
        
        if (metrics_.empty()) {
            oss << "No performance metrics collected." << std::endl;
            return oss.str();
        }
        
        oss << std::left << std::setw(25) << "Operation" 
            << std::right << std::setw(12) << "Calls" 
            << std::right << std::setw(15) << "Total (ms)" 
            << std::right << std::setw(15) << "Avg (ms)" 
            << std::right << std::setw(15) << "Min (ms)" 
            << std::right << std::setw(15) << "Max (ms)" 
            << std::endl;
        oss << std::string(95, '-') << std::endl;
        
        for (const auto& pair : metrics_) {
            const auto& metric = pair.second;
            oss << std::left << std::setw(25) << metric.operation
                << std::right << std::setw(12) << metric.callCount
                << std::right << std::setw(15) << std::fixed << std::setprecision(3) << metric.totalTimeMs
                << std::right << std::setw(15) << std::fixed << std::setprecision(3) << metric.averageTimeMs
                << std::right << std::setw(15) << std::fixed << std::setprecision(3) << metric.minTimeMs
                << std::right << std::setw(15) << std::fixed << std::setprecision(3) << metric.maxTimeMs
                << std::endl;
        }
        
        oss << std::endl;
        oss << "=== Performance Recommendations ===" << std::endl;
        
        // Generate recommendations based on performance
        for (const auto& pair : metrics_) {
            const auto& metric = pair.second;
            if (metric.callCount > 0 && metric.maxTimeMs > 100.0) {  // 100ms threshold
                oss << "- Operation '" << metric.operation << "' has high latency (max: " 
                    << metric.maxTimeMs << "ms). Consider optimization." << std::endl;
            }
            if (metric.callCount > 1000 && metric.averageTimeMs > 10.0) {
                oss << "- Operation '" << metric.operation << "' is called frequently but has high average latency. "
                    << "Potential performance bottleneck identified." << std::endl;
            }
        }
        
        return oss.str();
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.clear();
        operationTimers_.clear();
        NLM_LOG_INFO("Performance monitor reset");
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
private:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> operationTimers_;
    std::unordered_map<std::string, PerformanceMetrics> metrics_;
};

// Benchmarking utilities
class Benchmark {
public:
    struct BenchmarkResult {
        std::string name;
        double durationMs;
        uint64_t iterations;
        double itemsPerSecond;
        std::string details;
    };
    
    Benchmark(const std::string& name) : name_(name), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~Benchmark() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        
        double durationMs = static_cast<double>(duration.count()) / 1000.0;
        
        BenchmarkResult result;
        result.name = name_;
        result.durationMs = durationMs;
        result.iterations = 1;
        result.itemsPerSecond = durationMs > 0.0 ? 1000.0 / durationMs : 0.0;
        result.details = "Single run benchmark";
        
        results_.push_back(result);
        
        NLM_LOG_INFO(std::string("Benchmark completed: ") + name_ + " in " + 
                    std::to_string(durationMs) + "ms");
    }
    
    static std::string generateBenchmarkReport() {
        std::ostringstream oss;
        oss << "=== NLM Benchmark Results ===" << std::endl;
        oss << "Generated at: " << PerformanceMonitor::getInstance().getCurrentTimestamp() << std::endl;
        oss << std::endl;
        
        if (Benchmark::results_.empty()) {
            oss << "No benchmark results collected." << std::endl;
            return oss.str();
        }
        
        oss << std::left << std::setw(25) << "Benchmark" 
            << std::right << std::setw(15) << "Duration (ms)" 
            << std::right << std::setw(20) << "Items/sec" 
            << std::left << std::setw(20) << "Details" 
            << std::endl;
        oss << std::string(80, '-') << std::endl;
        
        for (const auto& result : Benchmark::results_) {
            oss << std::left << std::setw(25) << result.name
                << std::right << std::setw(15) << std::fixed << std::setprecision(3) << result.durationMs
                << std::right << std::setw(20) << std::fixed << std::setprecision(0) << result.itemsPerSecond
                << std::left << std::setw(20) << result.details
                << std::endl;
        }
        
        return oss.str();
    }
    
    static void saveBenchmarkReport(const std::string& filepath) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << generateBenchmarkReport();
            file.close();
            NLM_LOG_INFO("Benchmark report saved to: " + filepath);
        } else {
            NLM_LOG_ERROR("Failed to save benchmark report to: " + filepath);
        }
    }
    
    static void clearResults() {
        Benchmark::results_.clear();
    }
    
    static std::vector<BenchmarkResult>& getResults() {
        return Benchmark::results_;
    }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
    static std::vector<BenchmarkResult> results_;
};

std::vector<Benchmark::BenchmarkResult> Benchmark::results_;

// Integration macros for easy performance monitoring
#define NLM_PERFORMANCE_SCOPE(operation) \
    class NLMPerformanceScope { \
    public: \
        NLMPerformanceScope(const std::string& op) : operation_(op) { \
            PerformanceMonitor::getInstance().startMeasure(operation_); \
        } \
        ~NLMPerformanceScope() { \
            PerformanceMonitor::getInstance().stopMeasure(operation_); \
        } \
    private: \
        std::string operation_; \
    } nlm_perf_scope_##__LINE__(operation)

#define NLM_BENCHMARK(operation) NLM_BENCHMARK_OBJ(operation, __LINE__)
#define NLM_BENCHMARK_OBJ(operation, line) NLM_BENCHMARK_INTERNAL(operation, line)
#define NLM_BENCHMARK_INTERNAL(operation, line) \
    class NLMBenchmark_##line { \
    public: \
        NLMBenchmark_##line() { \
            startTime_ = std::chrono::high_resolution_clock::now(); \
        } \
        ~NLMBenchmark_##line() { \
            auto endTime = std::chrono::high_resolution_clock::now(); \
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_); \
            double durationMs = static_cast<double>(duration.count()) / 1000.0; \
            NLM_LOG_INFO(std::string("Benchmark " #operation " completed in ") + std::to_string(durationMs) + "ms"); \
        } \
    private: \
        std::chrono::high_resolution_clock::time_point startTime_; \
    } nlm_benchmark_##line

// Convenience functions for common performance measurements
void profileNeuralNetwork(const std::string& networkName, int steps) {
    PerformanceMonitor::getInstance().startMeasure(networkName + "_total");
    for (int i = 0; i < steps; ++i) {
        PerformanceMonitor::getInstance().startMeasure(networkName + "_step_" + std::to_string(i));
        // Simulate neural network work
        volatile double dummy = 0.0;
        for (int j = 0; j < 1000; ++j) {
            dummy += std::sin(j * 0.1) * std::cos(j * 0.05);
        }
        PerformanceMonitor::getInstance().stopMeasure(networkName + "_step_" + std::to_string(i));
    }
    PerformanceMonitor::getInstance().stopMeasure(networkName + "_total");
    
    NLM_LOG_INFO(std::string("Neural network profile: ") + networkName + " - " + std::to_string(steps) + " steps completed");
}

void profilePlasticityRules(int iterations) {
    PerformanceMonitor::getInstance().startMeasure("plasticity_total");
    
    for (int i = 0; i < iterations; ++i) {
        PerformanceMonitor::getInstance().startMeasure("hebbian_update");
        // Simulate Hebbian update
        volatile double dummy1 = 0.0;
        for (int j = 0; j < 500; ++j) {
            dummy1 += std::sin(j * 0.01);
        }
        PerformanceMonitor::getInstance().stopMeasure("hebbian_update");
        
        PerformanceMonitor::getInstance().startMeasure("stdp_update");
        // Simulate STDP update
        volatile double dummy2 = 0.0;
        for (int j = 0; j < 500; ++j) {
            dummy2 += std::cos(j * 0.01);
        }
        PerformanceMonitor::getInstance().stopMeasure("stdp_update");
    }
    
    PerformanceMonitor::getInstance().stopMeasure("plasticity_total");
    
    NLM_LOG_INFO(std::string("Plasticity rules profile: ") + std::to_string(iterations) + " iterations completed");
}

void generateSystemPerformanceReport() {
    std::ostringstream report;
    report << "=== NLM System Performance Report ===" << std::endl;
    report << "Generated at: " << PerformanceMonitor::getInstance().getCurrentTimestamp() << std::endl;
    report << std::endl;
    
    // Add performance metrics report
    report << PerformanceMonitor::getInstance().generateReport() << std::endl;
    
    // Add benchmark results
    report << Benchmark::generateBenchmarkReport() << std::endl;
    
    // Performance recommendations based on collected metrics
    report << "=== System Performance Analysis ===" << std::endl;
    
    const auto& monitor = PerformanceMonitor::getInstance();
    auto operations = monitor.getAllOperations();
    
    if (!operations.empty()) {
        report << "Top 5 most frequently called operations:" << std::endl;
        
        std::vector<std::pair<std::string, uint64_t>> operationCounts;
        for (const auto& op : operations) {
            const auto& metric = monitor.getMetrics(op);
            operationCounts.emplace_back(op, metric.callCount);
        }
        
        std::sort(operationCounts.begin(), operationCounts.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        size_t count = std::min<size_t>(5, operationCounts.size());
        for (size_t i = 0; i < count; ++i) {
            report << "  " << (i + 1) << ". " << operationCounts[i].first 
                   << " - called " << operationCounts[i].second << " times" << std::endl;
        }
        
        // Find operations with highest average latency
        report << std::endl << "Operations with highest average latency:" << std::endl;
        
        std::vector<std::pair<std::string, double>> latencies;
        for (const auto& op : operations) {
            const auto& metric = monitor.getMetrics(op);
            if (metric.callCount > 0) {
                latencies.emplace_back(op, metric.averageTimeMs);
            }
        }
        
        std::sort(latencies.begin(), latencies.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        size_t latencyCount = std::min<size_t>(5, latencies.size());
        for (size_t i = 0; i < latencyCount; ++i) {
            report << "  " << (i + 1) << ". " << latencies[i].first 
                   << " - avg latency: " << latencies[i].second << "ms" << std::endl;
        }
    }
    
    NLM_LOG_INFO("System performance report generated (see full report in logs)");
    
    // Save comprehensive report to file
    PerformanceMonitor::getInstance().saveReport("/tmp/nlm_performance_report.txt");
}

void PerformanceMonitor::saveReport(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << generateReport();
        file.close();
        NLM_LOG_INFO("Performance report saved to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to save performance report to: " + filepath);
    }
}

// Global function to initialize performance monitoring
void initializePerformanceMonitoring() {
    NLM_LOG_INFO("Performance monitoring system initialized");
}

// Global function to shutdown performance monitoring  
void shutdownPerformanceMonitoring() {
    NLM_LOG_INFO("Performance monitoring system shutting down");
    PerformanceMonitor::getInstance().reset();
    Benchmark::clearResults();
}

} // namespace nlm