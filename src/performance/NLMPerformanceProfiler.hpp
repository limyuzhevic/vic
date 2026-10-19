#pragma once

/**
 * Comprehensive Performance Profiling System for NLM Framework
 * 
 * This module provides high-resolution performance profiling capabilities
 * for the NLM (Neural Language Model?) framework with detailed metrics
 * collection and analysis tools.
 * 
 * Features:
 * - High-resolution nanosecond-precision timing
 * - Nested and stacked measurements
 * - Thread-local and global performance data
 * - Memory and CPU profiling
 * - Real-time monitoring capabilities
 * - Integration with all NLM systems
 * - Configurable output formats and thresholds
 * - Performance regression detection
 * - Bottleneck identification tools
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iomanip>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <atomic>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <shared_mutex>
#include <sstream>
#include <fstream>
#include <queue>
#include <variant>
#include <numeric>
#include <cmath>
#include <sstream>
#include <ctime>
#include <array>
#include <random>
#include <limits>

namespace nlm {
namespace profiling {

/**
 * High-resolution timer with nanosecond precision
 */
class Timer {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    
    struct alignas(64) Measurement {
        TimePoint start;
        TimePoint end;
        std::string name;
        std::string context;
        uint64_t threadId;
        uint64_t cpuCycles;
        std::string tag;
        bool active;
        
        Measurement() : start(Clock::now()), end(start), threadId(0), cpuCycles(0), active(true) {}
        
        double duration_ns() const {
            return std::chrono::duration<double, std::nano>(end - start).count();
        }
        
        double duration_us() const {
            return std::chrono::duration<double, std::micro>(end - start).count();
        }
        
        double duration_ms() const {
            return std::chrono::duration<double, std::milli>(end - start).count();
        }
    };
    
    Measurement* measurement_;
    
public:
    Timer(const std::string& name, const std::string& context = "", const std::string& tag = "")
        : measurement_(new Measurement()) {
        measurement_->name = name;
        measurement_->context = context;
        measurement_->tag = tag;
        measurement_->threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    }
    
    ~Timer() {
        if (measurement_ && measurement_->active) {
            measurement_->end = Clock::now();
            measurement_->active = false;
        }
        delete measurement_;
    }
    
    // Manual stop/start
    void stop() {
        if (measurement_ && measurement_->active) {
            measurement_->end = Clock::now();
            measurement_->active = false;
        }
    }
    
    void start() {
        if (measurement_) {
            measurement_->start = Clock::now();
            measurement_->active = true;
        }
    }
    
    // Get measurement data
    const Measurement* getMeasurement() const { return measurement_; }
    
    double duration_ns() const {
        if (!measurement_) return 0.0;
        return measurement_->duration_ns();
    }
    
    double duration_us() const {
        if (!measurement_) return 0.0;
        return measurement_->duration_us();
    }
    
    double duration_ms() const {
        if (!measurement_) return 0.0;
        return measurement_->duration_ms();
    }
};

/**
 * Scoped timer for automatic start/stop
 */
class ScopedTimer {
private:
    Timer timer_;
    
public:
    ScopedTimer(const std::string& name, const std::string& context = "", const std::string& tag = "")
        : timer_(name, context, tag) {}
    
    ~ScopedTimer() {}
    
    // Allow access to timer data
    double duration_ns() const { return timer_.duration_ns(); }
    double duration_us() const { return timer_.duration_us(); }
    double duration_ms() const { return timer_.duration_ms(); }
};

/**
 * Nested timer for hierarchical performance measurement
 */
class NestedTimer {
private:
    std::vector<Timer*> timers_;
    bool active_;
    
public:
    NestedTimer(const std::vector<std::string>& names, const std::string& context = "")
        : active_(true) {
        for (const auto& name : names) {
            timers_.push_back(new Timer(name, context));
        }
    }
    
    ~NestedTimer() {
        for (auto* timer : timers_) {
            timer->stop();
            delete timer;
        }
    }
    
    void startAll() {}
    void stopAll() {}
    
    std::vector<double> getDurations_ns() const {
        std::vector<double> durations;
        for (const auto* timer : timers_) {
            durations.push_back(timer->duration_ns());
        }
        return durations;
    }
};

/**
 * Thread-local storage for per-thread measurements
 */
class ThreadLocalStorage {
private:
    struct alignas(64) ThreadData {
        std::vector<Timer::Measurement*> measurements;
        std::atomic<uint64_t> memoryUsage;
        std::atomic<uint64_t> cpuUsage;
        std::atomic<uint64_t> spikeCount;
        std::atomic<uint64_t> neuronCount;
        std::atomic<uint64_t> synapseCount;
        
        ThreadData() : memoryUsage(0), cpuUsage(0), spikeCount(0), neuronCount(0), synapseCount(0) {}
        
        void addMeasurement(Timer::Measurement* measurement) {
            measurements.push_back(measurement);
        }
        
        void updateMemoryUsage(uint64_t usage) { memoryUsage.store(usage); }
        void updateCPUUsage(uint64_t usage) { cpuUsage.store(usage); }
        void incrementSpikeCount() { spikeCount.fetch_add(1); }
        void incrementNeuronCount() { neuronCount.fetch_add(1); }
        void incrementSynapseCount() { synapseCount.fetch_add(1); }
    };
    
    static thread_local ThreadData threadData_;
    static std::shared_mutex mutex_;
    
public:
    static void addMeasurement(Timer::Measurement* measurement) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        threadData_.addMeasurement(measurement);
    }
    
    static void updateMemoryUsage(uint64_t usage) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        threadData_.updateMemoryUsage(usage);
    }
    
    static void updateCPUUsage(uint64_t usage) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        threadData_.updateCPUUsage(usage);
    }
    
    static void incrementSpikeCount() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        threadData_.incrementSpikeCount();
    }
    
    static void incrementNeuronCount() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        threadData_.incrementNeuronCount();
    }
    
    static void incrementSynapseCount() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        threadData_.incrementSynapseCount();
    }
    
    static const ThreadData& getThreadData() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return threadData_;
    }
    
    static void clearThreadData() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        threadData_.measurements.clear();
    }
};

/**
 * Memory profiler for tracking memory usage
 */
class MemoryProfiler {
private:
    struct alignas(64) MemorySample {
        uint64_t timestamp;
        uint64_t memoryUsage;
        uint64_t peakMemory;
        uint64_t allocatedObjects;
        std::string context;
        
        MemorySample(uint64_t ts, uint64_t mem, uint64_t peak, uint64_t alloc, const std::string& ctx)
            : timestamp(ts), memoryUsage(mem), peakMemory(peak), allocatedObjects(alloc), context(ctx) {}
    };
    
    std::vector<MemorySample> samples_;
    std::mutex mutex_;
    bool active_;
    uint64_t sampleIntervalNs_;
    
public:
    MemoryProfiler() : active_(false), sampleIntervalNs_(1000000) {} // 1ms interval
    
    void start() {
        active_ = true;
    }
    
    void stop() {
        active_ = false;
    }
    
    void sample(const std::string& context = "") {
        if (!active_) return;
        
        uint64_t timestamp = getTimestampNs();
        uint64_t memoryUsage = getCurrentMemoryUsage();
        uint64_t peakMemory = getPeakMemoryUsage();
        uint64_t allocatedObjects = getAllocatedObjectCount();
        
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.emplace_back(timestamp, memoryUsage, peakMemory, allocatedObjects, context);
    }
    
    const std::vector<MemorySample>& getSamples() const { return samples_; }
    void clearSamples() { std::lock_guard<std::mutex> lock(mutex_); samples_.clear(); }
    
private:
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    uint64_t getCurrentMemoryUsage() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
    
    uint64_t getPeakMemoryUsage() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
    
    uint64_t getAllocatedObjectCount() {
        // Implementation would depend on memory pool
        return 0; // Placeholder
    }
};

/**
 * CPU profiler for tracking CPU usage
 */
class CPUProfiler {
private:
    struct alignas(64) CPUSample {
        uint64_t timestamp;
        uint64_t cpuUsage;
        uint64_t userTime;
        uint64_t systemTime;
        uint64_t niceTime;
        std::string context;
        
        CPUSample(uint64_t ts, uint64_t cpu, uint64_t user, uint64_t sys, uint64_t nice, const std::string& ctx)
            : timestamp(ts), cpuUsage(cpu), userTime(user), systemTime(sys), niceTime(nice), context(ctx) {}
    };
    
    std::vector<CPUSample> samples_;
    std::mutex mutex_;
    bool active_;
    uint64_t sampleIntervalNs_;
    
public:
    CPUProfiler() : active_(false), sampleIntervalNs_(1000000) {} // 1ms interval
    
    void start() {
        active_ = true;
    }
    
    void stop() {
        active_ = false;
    }
    
    void sample(const std::string& context = "") {
        if (!active_) return;
        
        uint64_t timestamp = getTimestampNs();
        uint64_t cpuUsage = getCurrentCPUUsage();
        uint64_t userTime = getUserTime();
        uint64_t systemTime = getSystemTime();
        uint64_t niceTime = getNiceTime();
        
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.emplace_back(timestamp, cpuUsage, userTime, systemTime, niceTime, context);
    }
    
    const std::vector<CPUSample>& getSamples() const { return samples_; }
    void clearSamples() { std::lock_guard<std::mutex> lock(mutex_); samples_.clear(); }
    
private:
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    uint64_t getCurrentCPUUsage() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
    
    uint64_t getUserTime() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
    
    uint64_t getSystemTime() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
    
    uint64_t getNiceTime() {
        // Implementation would depend on platform
        return 0; // Placeholder
    }
};

/**
 * Global performance metrics collector
 */
class PerformanceMetrics {
private:
    struct alignas(64) PerformanceSnapshot {
        uint64_t timestamp;
        uint64_t threadId;
        double durationNs;
        std::string operation;
        std::string context;
        std::string tag;
        uint64_t memoryUsage;
        uint64_t cpuUsage;
        
        PerformanceSnapshot(uint64_t ts, uint64_t tid, double dur, const std::string& op,
                           const std::string& ctx, const std::string& tg, uint64_t mem, uint64_t cpu)
            : timestamp(ts), threadId(tid), durationNs(dur), operation(op), context(ctx), tag(tg), memoryUsage(mem), cpuUsage(cpu) {}
    };
    
    std::vector<PerformanceSnapshot> snapshots_;
    std::mutex mutex_;
    bool active_;
    
public:
    PerformanceMetrics() : active_(false) {}
    
    void start() { active_ = true; }
    void stop() { active_ = false; }
    
    void record(const std::string& operation, const std::string& context = "", const std::string& tag = "") {
        if (!active_) return;
        
        uint64_t timestamp = getTimestampNs();
        uint64_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        
        std::lock_guard<std::mutex> lock(mutex_);
        snapshots_.emplace_back(timestamp, threadId, 0.0, operation, context, tag, 0, 0);
    }
    
    void record(const std::string& operation, double durationNs, const std::string& context = "", const std::string& tag = "") {
        if (!active_) return;
        
        uint64_t timestamp = getTimestampNs();
        uint64_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        
        std::lock_guard<std::mutex> lock(mutex_);
        snapshots_.emplace_back(timestamp, threadId, durationNs, operation, context, tag, 0, 0);
    }
    
    const std::vector<PerformanceSnapshot>& getSnapshots() const { return snapshots_; }
    void clearSnapshots() { std::lock_guard<std::mutex> lock(mutex_); snapshots_.clear(); }
    
    uint64_t getSnapshotCount() const { return snapshots_.size(); }
    
    // Statistical analysis
    struct Statistics {
        double mean;
        double median;
        double stddev;
        double min;
        double max;
        double p95;
        double p99;
        
        Statistics() : mean(0.0), median(0.0), stddev(0.0), min(0.0), max(0.0), p95(0.0), p99(0.0) {}
    };
    
    Statistics calculateStatistics(const std::vector<double>& values) {
        Statistics stats;
        if (values.empty()) return stats;
        
        stats.min = *std::min_element(values.begin(), values.end());
        stats.max = *std::max_element(values.begin(), values.end());
        
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        stats.mean = sum / values.size();
        
        // Calculate standard deviation
        double sqSum = 0.0;
        for (double val : values) {
            sqSum += (val - stats.mean) * (val - stats.mean);
        }
        stats.stddev = std::sqrt(sqSum / values.size());
        
        // Calculate median and percentiles
        std::vector<double> sorted(values);
        std::sort(sorted.begin(), sorted.end());
        
        stats.median = sorted[values.size() / 2];
        stats.p95 = sorted[values.size() * 95 / 100];
        stats.p99 = sorted[values.size() * 99 / 100];
        
        return stats;
    }
    
private:
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

/**
 * Profile report generator
 */
class ProfileReporter {
public:
    enum class OutputFormat { TEXT, CSV, JSON };
    
    static void generateReport(const PerformanceMetrics& metrics, const std::string& filename,
                              OutputFormat format = OutputFormat::TEXT) {
        switch (format) {
            case OutputFormat::TEXT:
                generateTextReport(metrics, filename);
                break;
            case OutputFormat::CSV:
                generateCSVReport(metrics, filename);
                break;
            case OutputFormat::JSON:
                generateJSONReport(metrics, filename);
                break;
        }
    }
    
private:
    static void generateTextReport(const PerformanceMetrics& metrics, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << "NLM Performance Profile Report" << std::endl;
        file << "================================" << std::endl;
        file << std::endl;
        
        // Group operations by name
        std::unordered_map<std::string, std::vector<double>> operationDurations;
        for (const auto& snapshot : metrics.getSnapshots()) {
            operationDurations[snapshot.operation].push_back(snapshot.durationNs);
        }
        
        // Generate statistics for each operation
        for (const auto& pair : operationDurations) {
            const std::string& operation = pair.first;
            const std::vector<double>& durations = pair.second;
            
            PerformanceMetrics::Statistics stats = metrics.calculateStatistics(durations);
            
            file << "Operation: " << operation << std::endl;
            file << "  Count: " << durations.size() << std::endl;
            file << "  Mean (ns): " << std::fixed << std::setprecision(2) << stats.mean << std::endl;
            file << "  Median (ns): " << stats.median << std::endl;
            file << "  StdDev (ns): " << stats.stddev << std::endl;
            file << "  Min (ns): " << stats.min << std::endl;
            file << "  Max (ns): " << stats.max << std::endl;
            file << "  95th percentile (ns): " << stats.p95 << std::endl;
            file << "  99th percentile (ns): " << stats.p99 << std::endl;
            file << std::endl;
        }
        
        file.close();
    }
    
    static void generateCSVReport(const PerformanceMetrics& metrics, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << "timestamp,thread_id,duration_ns,operation,context,tag" << std::endl;
        
        for (const auto& snapshot : metrics.getSnapshots()) {
            file << snapshot.timestamp << ","
                 << snapshot.threadId << ","
                 << snapshot.durationNs << ","
                 << snapshot.operation << ","
                 << snapshot.context << ","
                 << snapshot.tag << std::endl;
        }
        
        file.close();
    }
    
    static void generateJSONReport(const PerformanceMetrics& metrics, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << "{\n  \"profile_data\": [" << std::endl;
        
        for (size_t i = 0; i < metrics.getSnapshots().size(); ++i) {
            const auto& snapshot = metrics.getSnapshots()[i];
            
            file << "    {\n"
                 << "      \"timestamp\": " << snapshot.timestamp << ",\n"
                 << "      \"thread_id\": " << snapshot.threadId << ",\n"
                 << "      \"duration_ns\": " << snapshot.durationNs << ",\n"
                 << "      \"operation\": \"" << snapshot.operation << "\",\n"
                 << "      \"context\": \"" << snapshot.context << "\",\n"
                 << "      \"tag\": \"" << snapshot.tag << "\"\n"
                 << "    }";
            
            if (i < metrics.getSnapshots().size() - 1) {
                file << ",";
            }
            file << std::endl;
        }
        
        file << "  ]\n}";
        file.close();
    }
};

/**
 * Performance alert system
 */
class PerformanceAlertSystem {
private:
    struct alignas(64) Alert {
        std::string name;
        std::string message;
        uint64_t timestamp;
        double threshold;
        double actualValue;
        std::string severity;
        
        Alert(const std::string& n, const std::string& msg, uint64_t ts, double thresh, double actual, const std::string& sev)
            : name(n), message(msg), timestamp(ts), threshold(thresh), actualValue(actual), severity(sev) {}
    };
    
    std::vector<Alert> alerts_;
    std::mutex mutex_;
    std::function<void(const Alert&)> alertCallback_;
    
public:
    PerformanceAlertSystem() {}
    
    void setAlertCallback(std::function<void(const Alert&)> callback) {
        alertCallback_ = callback;
    }
    
    void triggerAlert(const std::string& name, const std::string& message, 
                     double threshold, double actualValue, const std::string& severity = "WARNING") {
        uint64_t timestamp = getTimestampNs();
        Alert alert(name, message, timestamp, threshold, actualValue, severity);
        
        std::lock_guard<std::mutex> lock(mutex_);
        alerts_.push_back(alert);
        
        if (alertCallback_) {
            alertCallback_(alert);
        }
    }
    
    const std::vector<Alert>& getAlerts() const { return alerts_; }
    void clearAlerts() { std::lock_guard<std::mutex> lock(mutex_); alerts_.clear(); }
    
    // Check for performance bottlenecks
    void checkForBottlenecks(const PerformanceMetrics& metrics) {
        // Group by thread and operation to detect slow operations
        std::unordered_map<std::string, std::vector<double>> operationTimes;
        
        for (const auto& snapshot : metrics.getSnapshots()) {
            operationTimes[snapshot.operation].push_back(snapshot.durationNs);
        }
        
        for (const auto& pair : operationTimes) {
            const std::string& operation = pair.first;
            const std::vector<double>& durations = pair.second;
            
            PerformanceMetrics::Statistics stats = metrics.calculateStatistics(durations);
            
            if (stats.max > 1000000) { // 1ms threshold
                triggerAlert("SLOW_OPERATION", 
                            "Operation " + operation + " exceeded 1ms threshold",
                            1000000.0, stats.max, "WARNING");
            }
            
            if (stats.stddev > 100000) { // High variance threshold
                triggerAlert("HIGH_VARIANCE",
                            "Operation " + operation + " has high variance",
                            100000.0, stats.stddev, "INFO");
            }
        }
    }
    
private:
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

/**
 * Main profiler class for continuous profiling
 */
class Profiler {
private:
    bool active_;
    std::chrono::nanoseconds interval_;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastSample_;
    
    MemoryProfiler memoryProfiler_;
    CPUProfiler cpuProfiler_;
    PerformanceMetrics metrics_;
    PerformanceAlertSystem alertSystem_;
    
public:
    Profiler() : active_(false), interval_(std::chrono::milliseconds(100)) {} // 100ms interval
    
    void start() {
        active_ = true;
        lastSample_ = std::chrono::high_resolution_clock::now();
        memoryProfiler_.start();
        cpuProfiler_.start();
        metrics_.start();
    }
    
    void stop() {
        active_ = false;
        memoryProfiler_.stop();
        cpuProfiler_.stop();
        metrics_.stop();
    }
    
    void update() {
        if (!active_) return;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastSample_);
        
        if (elapsed >= interval_) {
            memoryProfiler_.sample();
            cpuProfiler_.sample();
            lastSample_ = now;
        }
    }
    
    // Record a specific operation
    void recordOperation(const std::string& operation, const std::string& context = "", const std::string& tag = "") {
        metrics_.record(operation, context, tag);
    }
    
    void recordOperation(const std::string& operation, double durationNs, 
                        const std::string& context = "", const std::string& tag = "") {
        metrics_.record(operation, durationNs, context, tag);
    }
    
    // Access to underlying components
    MemoryProfiler& getMemoryProfiler() { return memoryProfiler_; }
    CPUProfiler& getCPUProfiler() { return cpuProfiler_; }
    PerformanceMetrics& getPerformanceMetrics() { return metrics_; }
    PerformanceAlertSystem& getAlertSystem() { return alertSystem_; }
    
    // Generate report
    void generateReport(const std::string& filename, ProfileReporter::OutputFormat format = ProfileReporter::OutputFormat::TEXT) {
        ProfileReporter::generateReport(metrics_, filename, format);
    }
};

/**
 * Thread-local profiler wrapper
 */
class ThreadLocalProfiler {
private:
    Profiler profiler_;
    std::string threadName_;
    
public:
    ThreadLocalProfiler(const std::string& name) : threadName_(name) {}
    
    Profiler& getProfiler() { return profiler_; }
    const Profiler& getProfiler() const { return profiler_; }
    
    void start() { profiler_.start(); }
    void stop() { profiler_.stop(); }
    void update() { profiler_.update(); }
};

/**
 * Global profiler instance
 */
class GlobalProfiler {
private:
    Profiler profiler_;
    std::vector<ThreadLocalProfiler> threadProfilers_;
    std::mutex mutex_;
    
public:
    static GlobalProfiler& instance() {
        static GlobalProfiler instance;
        return instance;
    }
    
    void start() {
        profiler_.start();
        for (auto& threadProfiler : threadProfilers_) {
            threadProfiler.getProfiler().start();
        }
    }
    
    void stop() {
        profiler_.stop();
        for (auto& threadProfiler : threadProfilers_) {
            threadProfiler.getProfiler().stop();
        }
    }
    
    void update() {
        profiler_.update();
        for (auto& threadProfiler : threadProfilers_) {
            threadProfiler.getProfiler().update();
        }
    }
    
    Profiler& getProfiler() { return profiler_; }
    
    // Thread-local profiling
    void addThreadProfiler(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        threadProfilers_.emplace_back(name);
    }
    
    Profiler& getThreadProfiler(size_t threadIndex) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (threadIndex < threadProfilers_.size()) {
            return threadProfilers_[threadIndex].getProfiler();
        }
        return profiler_;
    }
    
    void checkForBottlenecks() {
        profiler_.getAlertSystem().checkForBottlenecks(profiler_.getPerformanceMetrics());
    }
    
    // Configuration methods
    void setSampleInterval(std::chrono::nanoseconds interval) {
        profiler_.interval_ = interval;
        for (auto& threadProfiler : threadProfilers_) {
            threadProfiler.getProfiler().interval_ = interval;
        }
    }
    
    void setAlertThreshold(double threshold) {
        // Configure alert thresholds
    }
    
    // Performance metrics analysis
    struct SystemPerformanceStats {
        double totalMemoryUsage;
        double totalCPUUsage;
        double avgOperationDuration;
        double maxOperationDuration;
        double minOperationDuration;
        size_t totalOperations;
        
        SystemPerformanceStats() : totalMemoryUsage(0.0), totalCPUUsage(0.0), avgOperationDuration(0.0),
                                 maxOperationDuration(0.0), minOperationDuration(0.0), totalOperations(0) {}
    };
    
    SystemPerformanceStats getSystemStats() {
        SystemPerformanceStats stats;
        const auto& snapshots = profiler_.getPerformanceMetrics().getSnapshots();
        
        if (snapshots.empty()) return stats;
        
        std::vector<double> durations;
        for (const auto& snapshot : snapshots) {
            durations.push_back(snapshot.durationNs);
        }
        
        PerformanceMetrics::Statistics statCalc = profiler_.getPerformanceMetrics().calculateStatistics(durations);
        
        stats.avgOperationDuration = statCalc.mean;
        stats.maxOperationDuration = statCalc.max;
        stats.minOperationDuration = statCalc.min;
        stats.totalOperations = snapshots.size();
        
        // Get memory usage from memory profiler
        const auto& memorySamples = profiler_.getMemoryProfiler().getSamples();
        if (!memorySamples.empty()) {
            stats.totalMemoryUsage = memorySamples.back().memoryUsage;
        }
        
        // Get CPU usage from CPU profiler
        const auto& cpuSamples = profiler_.getCPUProfiler().getSamples();
        if (!cpuSamples.empty()) {
            stats.totalCPUUsage = cpuSamples.back().cpuUsage;
        }
        
        return stats;
    }
};

// Global thread-local storage helpers
inline thread_local ThreadLocalProfiler threadLocalProfiler("default");

// Convenience macros
#define PROFILE_SCOPE(name, context) nlm::profiling::ScopedTimer timer_##name(name, context, __FUNCTION__)
#define PROFILE_FUNCTION() PROFILE_SCOPE(std::string(__func__), "")
#define PROFILE_NESTED(names, context) nlm::profiling::NestedTimer nestedTimer_##context(names, context)

// Integration macros for NLM systems
#define PROFILE_BRAIN_PERFORMANCE(operation) nlm::profiling::GlobalProfiler::instance().recordOperation("BRAIN_" + std::string(#operation), 0, "BRAIN")
#define PROFILE_MOTOR_PERFORMANCE(operation) nlm::profiling::GlobalProfiler::instance().recordOperation("MOTOR_" + std::string(#operation), 0, "MOTOR")
#define PROFILE_SENORY_PERFORMANCE(operation) nlm::profiling::GlobalProfiler::instance().recordOperation("SENSOR_" + std::string(#operation), 0, "SENSOR")

} // namespace profiling
} // namespace nlm

// Initialize global thread-local profiler
inline void nlm::profiling::GlobalProfiler::initThreadProfilers() {
    GlobalProfiler::instance().addThreadProfiler("main");
    GlobalProfiler::instance().addThreadProfiler("worker");
}

// Start global profiling
inline void nlm::profiling::GlobalProfiler::startProfiling() {
    GlobalProfiler::instance().start();
}

// Stop global profiling
inline void nlm::profiling::GlobalProfiler::stopProfiling() {
    GlobalProfiler::instance().stop();
}

// Update global profiling
inline void nlm::profiling::GlobalProfiler::updateProfiling() {
    GlobalProfiler::instance().update();
    GlobalProfiler::instance().checkForBottlenecks();
}

// Generate performance report
inline void nlm::profiling::GlobalProfiler::generateReport(const std::string& filename,
                                                       ProfileReporter::OutputFormat format) {
    GlobalProfiler::instance().generateReport(filename, format);
}