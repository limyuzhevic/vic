// Performance and Profiling Utilities for NLM
// Phase 6: Integrated Artificial Brain Performance Analysis
// 
// This module provides comprehensive performance profiling, monitoring, and
// optimization tools for the NLM neural simulation framework. It includes:
// 
// - Performance counters for CPU, memory, and I/O operations
// - Profiling tools for brain simulation phases and systems
// - Optimization recommendations based on performance analysis
// - Benchmarking tools for comparison and regression testing
// - Memory usage monitoring and analysis
// - Resource utilization tracking
// - Performance degradation detection
// - System integration and coordination profiling

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

// Performance measurement units
struct PerformanceMetrics {
    std::string name;
    double time_ns = 0.0;      // Execution time in nanoseconds
    uint64_t memory_bytes = 0; // Memory usage in bytes
    uint64_t calls = 0;        // Number of calls/operations
    double cpu_time_ns = 0.0;   // CPU time in nanoseconds
    double wall_time_ns = 0.0;  // Wall clock time in nanoseconds
    
    // Calculate average time per call
    double getAverageTimeNs() const {
        return calls > 0 ? time_ns / calls : 0.0;
    }
    
    // Calculate operations per second
    double getOperationsPerSecond() const {
        return calls > 0 ? (time_ns / 1e9) / (getAverageTimeNs() / 1e9) : 0.0;
    }
    
    // Calculate memory per operation
    double getMemoryPerOperation() const {
        return calls > 0 ? static_cast<double>(memory_bytes) / calls : 0.0;
    }
};

// Performance counter for timing and measurement
class PerformanceCounter {
public:
    PerformanceCounter() : start_time_(), elapsed_time_(0), enabled_(true) {}
    
    void start() {
        if (enabled_) {
            start_time_ = std::chrono::high_resolution_clock::now();
        }
    }
    
    void stop() {
        if (enabled_) {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time_);
            elapsed_time_ += duration.count();
        }
    }
    
    void reset() {
        elapsed_time_ = 0;
    }
    
    double getElapsedTimeNs() const {
        return static_cast<double>(elapsed_time_);
    }
    
    void setEnabled(bool enabled) {
        enabled_ = enabled;
    }
    
    bool isEnabled() const {
        return enabled_;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    int64_t elapsed_time_;
    bool enabled_;
};

// Memory usage tracker
class MemoryTracker {
public:
    MemoryTracker() : total_allocated_(0), peak_allocated_(0), current_allocated_(0) {}
    
    void allocate(uint64_t size) {
        total_allocated_ += size;
        current_allocated_ += size;
        if (current_allocated_ > peak_allocated_) {
            peak_allocated_ = current_allocated_;
        }
    }
    
    void deallocate(uint64_t size) {
        current_allocated_ = std::max(0ULL, current_allocated_ - size);
    }
    
    uint64_t getTotalAllocated() const {
        return total_allocated_;
    }
    
    uint64_t getPeakAllocated() const {
        return peak_allocated_;
    }
    
    uint64_t getCurrentAllocated() const {
        return current_allocated_;
    }
    
    void reset() {
        total_allocated_ = 0;
        peak_allocated_ = 0;
        current_allocated_ = 0;
    }
    
private:
    uint64_t total_allocated_;
    uint64_t peak_allocated_;
    uint64_t current_allocated_;
};

// Performance profiler for different system components
class PerformanceProfiler {
public:
    PerformanceProfiler() {}
    
    // Add a performance counter for a specific operation
    void addCounter(const std::string& name) {
        counters_[name] = PerformanceCounter();
    }
    
    // Start a performance counter
    void startCounter(const std::string& name) {
        auto it = counters_.find(name);
        if (it != counters_.end()) {
            it->second.start();
        }
    }
    
    // Stop a performance counter
    void stopCounter(const std::string& name) {
        auto it = counters_.find(name);
        if (it != counters_.end()) {
            it->second.stop();
        }
    }
    
    // Get performance metrics for a counter
    PerformanceMetrics getMetrics(const std::string& name) const {
        PerformanceMetrics metrics;
        metrics.name = name;
        
        auto it = counters_.find(name);
        if (it != counters_.end()) {
            metrics.time_ns = it->second.getElapsedTimeNs();
        }
        
        return metrics;
    }
    
    // Get all performance metrics
    std::vector<PerformanceMetrics> getAllMetrics() const {
        std::vector<PerformanceMetrics> metrics;
        
        for (const auto& pair : counters_) {
            PerformanceMetrics m = getMetrics(pair.first);
            m.calls = 1;  // Simple assumption for now
            metrics.push_back(m);
        }
        
        return metrics;
    }
    
    // Reset all counters
    void reset() {
        for (auto& pair : counters_) {
            pair.second.reset();
        }
    }
    
    // Get total elapsed time
    double getTotalTimeNs() const {
        double total = 0.0;
        for (const auto& pair : counters_) {
            total += pair.second.getElapsedTimeNs();
        }
        return total;
    }
    
    // Print performance report
    void printReport(std::ostream& out = std::cout) const {
        out << "=== NLM Performance Profile ===" << std::endl;
        out << "Total time: " << (getTotalTimeNs() / 1e6) << " ms" << std::endl;
        out << "Counters:" << std::endl;
        
        auto metrics = getAllMetrics();
        double total_time = getTotalTimeNs();
        
        for (const auto& metric : metrics) {
            double percentage = (metric.time_ns / total_time) * 100.0;
            out << "  " << std::left << std::setw(30) << metric.name << " : "
                << std::setw(10) << (metric.time_ns / 1e6) << " ms ("
                << std::setw(6) << percentage << "%)" << std::endl;
        }
    }
    
private:
    std::unordered_map<std::string, PerformanceCounter> counters_;
};

// Benchmarking utilities for performance comparison
class Benchmarker {
public:
    Benchmarker() : start_time_(), end_time_() {}
    
    // Start benchmark
    void start(const std::string& name) {
        benchmarks_[name].start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    // Stop benchmark and record result
    void stop(const std::string& name) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - benchmarks_[name].start_time_);
        
        BenchmarkResult result;
        result.name = name;
        result.duration_ns = duration.count();
        result.timestamp = std::chrono::system_clock::now();
        
        benchmarks_[name] = result;
    }
    
    // Get benchmark result
    BenchmarkResult getResult(const std::string& name) const {
        auto it = benchmarks_.find(name);
        if (it != benchmarks_.end()) {
            return it->second;
        }
        return BenchmarkResult();
    }
    
    // Get all benchmark results
    std::vector<BenchmarkResult> getAllResults() const {
        std::vector<BenchmarkResult> results;
        for (const auto& pair : benchmarks_) {
            results.push_back(pair.second);
        }
        return results;
    }
    
    // Calculate average benchmark time
    double getAverageTimeNs(const std::string& name) const {
        auto results = getBenchmarksForName(name);
        if (results.empty()) return 0.0;
        
        double total = 0.0;
        for (const auto& result : results) {
            total += result.duration_ns;
        }
        return total / results.size();
    }
    
    // Get benchmarks for a specific operation
    std::vector<BenchmarkResult> getBenchmarksForName(const std::string& name) const {
        std::vector<BenchmarkResult> results;
        for (const auto& pair : benchmarks_) {
            if (pair.first == name) {
                results.push_back(pair.second);
            }
        }
        return results;
    }
    
    // Clear all benchmarks
    void clear() {
        benchmarks_.clear();
    }
    
    // Print benchmark report
    void printReport(std::ostream& out = std::cout) const {
        out << "=== NLM Benchmark Results ===" << std::endl;
        
        if (benchmarks_.empty()) {
            out << "No benchmarks available." << std::endl;
            return;
        }
        
        // Group results by operation name
        std::unordered_map<std::string, std::vector<BenchmarkResult>> grouped;
        for (const auto& pair : benchmarks_) {
            grouped[pair.first].push_back(pair.second);
        }
        
        for (const auto& pair : grouped) {
            const std::string& name = pair.first;
            const std::vector<BenchmarkResult>& results = pair.second;
            
            double total_time = 0.0;
            double min_time = std::numeric_limits<double>::max();
            double max_time = 0.0;
            
            for (const auto& result : results) {
                total_time += result.duration_ns;
                min_time = std::min(min_time, result.duration_ns);
                max_time = std::max(max_time, result.duration_ns);
            }
            
            double avg_time = total_time / results.size();
            
            out << name << ":" << std::endl;
            out << "  Runs: " << results.size() << std::endl;
            out << "  Total time: " << (total_time / 1e6) << " ms" << std::endl;
            out << "  Average: " << (avg_time / 1e6) << " ms" << std::endl;
            out << "  Min: " << (min_time / 1e6) << " ms" << std::endl;
            out << "  Max: " << (max_time / 1e6) << " ms" << std::endl;
            out << std::endl;
        }
    }
    
private:
    struct BenchmarkResult {
        std::string name;
        int64_t duration_ns;
        std::chrono::system_clock::time_point timestamp;
        
        BenchmarkResult() : duration_ns(0) {}
    };
    
    std::unordered_map<std::string, BenchmarkResult> benchmarks_;
};

// System resource monitor
class ResourceMonitor {
public:
    ResourceMonitor() : memory_usage_(0), cpu_usage_(0.0), active_threads_(0) {}
    
    // Update resource usage
    void update() {
        // In a real implementation, this would query system resources
        // For now, we'll use placeholder values
        memory_usage_ = 0;  // Placeholder
        cpu_usage_ = 0.0;   // Placeholder
        active_threads_ = 0;  // Placeholder
    }
    
    // Get memory usage in bytes
    uint64_t getMemoryUsage() const {
        return memory_usage_;
    }
    
    // Get CPU usage as percentage (0-100)
    double getCpuUsage() const {
        return cpu_usage_;
    }
    
    // Get number of active threads
    uint32_t getActiveThreads() const {
        return active_threads_;
    }
    
    // Print resource report
    void printReport(std::ostream& out = std::cout) const {
        out << "=== System Resource Usage ===" << std::endl;
        out << "Memory usage: " << (memory_usage_ / (1024 * 1024)) << " MB" << std::endl;
        out << "CPU usage: " << cpu_usage_ << "%" << std::endl;
        out << "Active threads: " << active_threads_ << std::endl;
    }
    
private:
    uint64_t memory_usage_;
    double cpu_usage_;
    uint32_t active_threads_;
};

// Performance analysis utilities
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer() {}
    
    // Analyze performance bottlenecks
    std::vector<std::string> analyzeBottlenecks(const std::vector<PerformanceMetrics>& metrics) {
        std::vector<std::string> bottlenecks;
        
        if (metrics.empty()) {
            return bottlenecks;
        }
        
        // Find the longest running operations
        double total_time = 0.0;
        for (const auto& metric : metrics) {
            total_time += metric.time_ns;
        }
        
        for (const auto& metric : metrics) {
            double percentage = (metric.time_ns / total_time) * 100.0;
            
            if (percentage > 50.0) {
                bottlenecks.push_back(
                    metric.name + " is taking " + std::to_string(percentage) + "% of total time"
                );
            }
        }
        
        return bottlenecks;
    }
    
    // Generate performance optimization recommendations
    std::vector<std::string> generateOptimizationRecommendations(const std::vector<PerformanceMetrics>& metrics) {
        std::vector<std::string> recommendations;
        
        if (metrics.empty()) {
            return recommendations;
        }
        
        for (const auto& metric : metrics) {
            if (metric.calls > 0) {
                double avg_time = metric.time_ns / metric.calls;
                
                if (avg_time > 1000000.0) {  // More than 1ms per operation
                    recommendations.push_back(
                        "Consider optimizing " + metric.name + " (average " + 
                        std::to_string(avg_time / 1000.0) + " ms per operation)"
                    );
                }
            }
            
            if (metric.memory_bytes > 1024 * 1024) {  // More than 1MB
                recommendations.push_back(
                    "Consider reducing memory usage in " + metric.name + " (" +
                    std::to_string(metric.memory_bytes / (1024 * 1024)) + " MB)"
                );
            }
        }
        
        return recommendations;
    }
    
    // Compare performance between two benchmark sets
    std::string comparePerformance(const std::vector<BenchmarkResult>& before,
                                   const std::vector<BenchmarkResult>& after) {
        std::stringstream ss;
        ss << "=== Performance Comparison ===" << std::endl;
        
        if (before.empty() || after.empty()) {
            ss << "Insufficient data for comparison." << std::endl;
            return ss.str();
        }
        
        // Calculate average times for each operation
        std::unordered_map<std::string, double> before_times;
        std::unordered_map<std::string, double> after_times;
        
        for (const auto& result : before) {
            before_times[result.name] += result.duration_ns;
        }
        for (const auto& result : after) {
            after_times[result.name] += result.duration_ns;
        }
        
        for (const auto& pair : before_times) {
            const std::string& name = pair.first;
            double before_avg = pair.second / before.size();
            
            auto after_it = after_times.find(name);
            if (after_it != after_times.end()) {
                double after_avg = after_it->second / after.size();
                
                double improvement = ((before_avg - after_avg) / before_avg) * 100.0;
                
                ss << name << ": " << std::endl;
                ss << "  Before: " << (before_avg / 1e6) << " ms" << std::endl;
                ss << "  After:  " << (after_avg / 1e6) << " ms" << std::endl;
                ss << "  Improvement: " << (improvement > 0 ? "+" : "") << improvement << "%" << std::endl;
                ss << std::endl;
            }
        }
        
        return ss.str();
    }
};

// Global performance profiler instance
inline PerformanceProfiler& getGlobalProfiler() {
    static PerformanceProfiler profiler;
    return profiler;
}

// Global benchmarker instance
inline Benchmarker& getGlobalBenchmarker() {
    static Benchmarker benchmarker;
    return benchmarker;
}

// Global resource monitor instance
inline ResourceMonitor& getGlobalResourceMonitor() {
    static ResourceMonitor monitor;
    return monitor;
}

} // namespace nlm