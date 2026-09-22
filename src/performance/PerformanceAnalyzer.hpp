#pragma once

#include "../core/Types/Types.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <iomanip>

namespace nlm {

// Performance monitoring system for NLM
// Provides timing, memory usage, and statistics tracking

// Performance metrics structure
struct PerformanceMetrics {
    double cpuTime;           // CPU time in seconds
    double wallClockTime;     // Wall clock time in seconds
    size_t memoryUsage;       // Memory usage in bytes
    size_t peakMemoryUsage;   // Peak memory usage in bytes
    uint64_t cpuCycles;       // CPU cycles
    uint64_t instructions;   // Instructions retired
    
    PerformanceMetrics()
        : cpuTime(0.0), wallClockTime(0.0), memoryUsage(0), 
          peakMemoryUsage(0), cpuCycles(0), instructions(0) {}
};

// Timer class for measuring execution time
class Timer {
public:
    Timer() : startTime_(std::chrono::high_resolution_clock::now()), 
              isRunning_(true) {}
    
    ~Timer() {
        if (isRunning_) {
            stop();
        }
    }
    
    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
        isRunning_ = true;
    }
    
    void stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        duration_ = endTime - startTime_;
        isRunning_ = false;
    }
    
    double elapsedSeconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        if (isRunning_) {
            return std::chrono::duration<double>(endTime - startTime_).count();
        } else {
            return std::chrono::duration<double>(duration_).count();
        }
    }
    
    double elapsedMilliseconds() const {
        return elapsedSeconds() * 1000.0;
    }
    
    double elapsedMicroseconds() const {
        return elapsedSeconds() * 1000000.0;
    }
    
    bool isRunning() const { return isRunning_; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::duration<double> duration_;
    bool isRunning_;
};

// Statistical aggregator for performance measurements
class PerformanceStats {
public:
    struct Sample {
        double value;
        double timestamp;
        
        Sample(double val, double ts) : value(val), timestamp(ts) {}
    };
    
    PerformanceStats() : count_(0), sum_(0.0), sumSquares_(0.0), min_(std::numeric_limits<double>::max()),
                         max_(std::numeric_limits<double>::lowest()), firstSampleTime_(0.0) {}
    
    void addSample(double value, double timestamp = 0.0) {
        if (count_ == 0) {
            firstSampleTime_ = timestamp;
        }
        samples_.emplace_back(value, timestamp);
        count_++;
        sum_ += value;
        sumSquares_ += value * value;
        if (value < min_) min_ = value;
        if (value > max_) max_ = value;
    }
    
    double getMean() const {
        return count_ > 0 ? sum_ / count_ : 0.0;
    }
    
    double getVariance() const {
        if (count_ <= 1) return 0.0;
        return (sumSquares_ - (sum_ * sum_) / count_) / (count_ - 1);
    }
    
    double getStdDev() const {
        return std::sqrt(getVariance());
    }
    
    double getMin() const { return min_; }
    double getMax() const { return max_; }
    size_t getCount() const { return count_; }
    double getFirstSampleTime() const { return firstSampleTime_; }
    
    double getSampleRate() const {
        if (samples_.size() < 2) return 0.0;
        double timeSpan = samples_.back().timestamp - firstSampleTime_;
        return timeSpan > 0.0 ? (samples_.size() - 1) / timeSpan : 0.0;
    }
    
    const std::vector<Sample>& getSamples() const { return samples_; }
    
    void clear() {
        count_ = 0;
        sum_ = 0.0;
        sumSquares_ = 0.0;
        min_ = std::numeric_limits<double>::max();
        max_ = std::numeric_limits<double>::lowest();
        firstSampleTime_ = 0.0;
        samples_.clear();
    }
    
private:
    size_t count_;
    double sum_;
    double sumSquares_;
    double min_;
    double max_;
    double firstSampleTime_;
    std::vector<Sample> samples_;
};

// Performance monitor for individual operations
class PerformanceMonitor {
public:
    PerformanceMonitor(const std::string& name) : name_(name) {
        timer_.start();
    }
    
    ~PerformanceMonitor() {
        timer_.stop();
        recordMeasurement(timer_.elapsedSeconds());
    }
    
    void recordMeasurement(double value) {
        auto it = stats_.find(name_);
        if (it == stats_.end()) {
            stats_[name_] = PerformanceStats();
        }
        stats_[name_].addSample(value);
    }
    
    double getMeanTime() const {
        auto it = stats_.find(name_);
        return it != stats_.end() ? it->second.getMean() : 0.0;
    }
    
    double getStdDevTime() const {
        auto it = stats_.find(name_);
        return it != stats_.end() ? it->second.getStdDev() : 0.0;
    }
    
    static PerformanceMonitor& getInstance(const std::string& name) {
        static std::unordered_map<std::string, PerformanceMonitor> monitors;
        if (monitors.find(name) == monitors.end()) {
            monitors[name] = PerformanceMonitor(name);
        }
        return monitors[name];
    }
    
    static void reportAll(std::ostream& out = std::cout) {
        out << "=== NLM Performance Report ===\n";
        
        for (const auto& pair : getAllMonitors()) {
            const auto& name = pair.first;
            const auto& stats = pair.second;
            
            out << name << ":\n";
            out << "  Measurements: " << stats.getCount() << "\n";
            out << "  Mean: " << std::fixed << std::setprecision(6) 
                << stats.getMean() << "s\n";
            out << "  Std Dev: " << std::fixed << std::setprecision(6) 
                << stats.getStdDev() << "s\n";
            out << "  Min: " << std::fixed << std::setprecision(6) 
                << stats.getMin() << "s\n";
            out << "  Max: " << std::fixed << std::setprecision(6) 
                << stats.getMax() << "s\n";
            out << "  Sample Rate: " << std::fixed << std::setprecision(2) 
                << stats.getSampleRate() << " samples/s\n";
            out << "\n";
        }
    }
    
    static std::unordered_map<std::string, PerformanceStats>& getAllMonitors() {
        static std::unordered_map<std::string, PerformanceStats> monitors;
        return monitors;
    }
    
private:
    std::string name_;
    Timer timer_;
};

// Global performance macros for easy timing
#define NLM_PERF_SCOPE(name) nlm::PerformanceMonitor::getInstance(name)

// Macro to automatically record time of block execution
#define NLM_PROFILE(name) nlm::Timer timer_##name; \\
    [&]() { \\
        auto& monitor = nlm::PerformanceMonitor::getInstance(name); \\
        monitor.recordMeasurement(timer_##name.elapsedSeconds()); \\
    }()

// Memory tracking utilities
class MemoryTracker {
public:
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }
    
    void recordMemoryUsage(size_t bytes) {
        currentMemory_ = bytes;
        if (bytes > peakMemory_) {
            peakMemory_ = bytes;
        }
    }
    
    size_t getCurrentMemory() const { return currentMemory_; }
    size_t getPeakMemory() const { return peakMemory_; }
    
    void reset() {
        currentMemory_ = 0;
        peakMemory_ = 0;
    }
    
private:
    MemoryTracker() : currentMemory_(0), peakMemory_(0) {}
    size_t currentMemory_;
    size_t peakMemory_;
};

// Comprehensive performance analysis class
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer() = default;
    
    // Record performance metrics for a brain operation
    void recordBrainOperation(const std::string& operation, 
                             const PerformanceMetrics& metrics,
                             size_t neuronCount = 0,
                             size_t synapseCount = 0);
    
    // Get performance statistics
    PerformanceStats getOperationStats(const std::string& operation) const;
    std::vector<std::string> getAllOperations() const;
    
    // Generate performance report
    std::string generateReport() const;
    
    // Clear all statistics
    void clear();
    
    // Performance benchmarks
    void runBenchmarks();
    
    // Resource usage monitoring
    void startMemoryMonitoring() { memoryTracker_.reset(); }
    void updateMemoryUsage(size_t bytes) { memoryTracker_.recordMemoryUsage(bytes); }
    void stopMemoryMonitoring() { /* Implementation would go here */ }
    
    // Performance alerts
    void setMemoryThreshold(size_t thresholdBytes) {
        memoryThreshold_ = thresholdBytes;
        checkMemoryAlert();
    }
    
    void setPerformanceThreshold(const std::string& operation, 
                                double thresholdSeconds) {
        performanceThresholds_[operation] = thresholdSeconds;
        checkPerformanceAlert(operation);
    }
    
    // Alert callbacks
    std::function<void(size_t)> memoryAlertCallback;
    std::function<void(const std::string&, double)> performanceAlertCallback;
    
private:
    void checkMemoryAlert() {
        if (memoryAlertCallback && memoryTracker_.getCurrentMemory() > memoryThreshold_) {
            memoryAlertCallback(memoryTracker_.getCurrentMemory());
        }
    }
    
    void checkPerformanceAlert(const std::string& operation) {
        auto it = performanceThresholds_.find(operation);
        if (it != performanceThresholds_.end()) {
            auto stats = getOperationStats(operation);
            double meanTime = stats.getMean();
            if (performanceAlertCallback && meanTime > it->second) {
                performanceAlertCallback(operation, meanTime);
            }
        }
    }
    
    std::unordered_map<std::string, PerformanceStats> operationStats_;
    MemoryTracker memoryTracker_;
    size_t memoryThreshold_;
    std::unordered_map<std::string, double> performanceThresholds_;
};

} // namespace nlm
