#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace benchmarking {

// Performance counter for nanosecond precision
class PerformanceCounter {
public:
    static int64_t getNanoseconds() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
    
    static int64_t getMicroseconds() {
        return getNanoseconds() / 1000;
    }
    
    static int64_t getMilliseconds() {
        return getNanoseconds() / 1000000;
    }
    
    static double getSeconds() {
        return static_cast<double>(getNanoseconds()) / 1e9;
    }
    
    struct TimePoint {
        int64_t ns;
        explicit TimePoint(int64_t nanoseconds) : ns(nanoseconds) {}
        int64_t asNanoseconds() const { return ns; }
        int64_t asMicroseconds() const { return ns / 1000; }
        int64_t asMilliseconds() const { return ns / 1000000; }
        double asSeconds() const { return static_cast<double>(ns) / 1e9; }
        
        static TimePoint now() { return TimePoint(getNanoseconds()); }
        int64_t elapsedSince(const TimePoint& other) const { return ns - other.ns; }
    };
};

// Memory usage tracking
struct MemoryUsage {
    uint64_t totalBytes;
    uint64_t usedBytes;
    uint64_t availableBytes;
    double percentageUsed;
    
    MemoryUsage() : totalBytes(0), usedBytes(0), availableBytes(0), percentageUsed(0.0) {}
    
    std::string toString() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Total: " << (totalBytes / (1024*1024)) << "MB, "
           << "Used: " << (usedBytes / (1024*1024)) << "MB, "
           << "Available: " << (availableBytes / (1024*1024)) << "MB "
           << "(" << percentageUsed << "%)";
        return ss.str();
    }
};

// CPU usage tracking
struct CPUMetrics {
    double usagePercent;  // CPU usage percentage (0-100)
    int64_t userTime;     // CPU time in user mode (nanoseconds)
    int64_t systemTime;   // CPU time in system mode (nanoseconds)
    int64_t idleTime;     // CPU time in idle mode (nanoseconds)
    int64_t totalTime;    // Total CPU time (nanoseconds)
    
    CPUMetrics() : usagePercent(0.0), userTime(0), systemTime(0), idleTime(0), totalTime(0) {}
    
    std::string toString() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "CPU: " << usagePercent << "%, "
           << "User: " << (userTime / 1000000) << "ms, "
           << "System: " << (systemTime / 1000000) << "ms, "
           << "Idle: " << (idleTime / 1000000) << "ms";
        return ss.str();
    }
};

// Neural activity metrics
struct NeuralMetrics {
    size_t spikeCount;
    size_t firingNeuronCount;
    size_t activeNeuronCount;
    double avgFiringRate;
    double excitationInhibitionRatio;
    size_t memoryAccesses;
    size_t plasticityOperations;
    
    NeuralMetrics() : spikeCount(0), firingNeuronCount(0), activeNeuronCount(0),
                     avgFiringRate(0.0), excitationInhibitionRatio(0.0),
                     memoryAccesses(0), plasticityOperations(0) {}
};

// Complete benchmark result structure
struct BenchmarkResult {
    // Configuration
    std::string benchmarkName;
    size_t neuronCount;
    size_t synapseCount;
    size_t steps;
    size_t regions;
    
    // Timing (nanosecond precision)
    int64_t totalTimeNs;
    int64_t cpuTimeNs;
    int64_t wallClockTimeNs;
    double stepsPerSecond;
    
    // Resource usage
    MemoryUsage memoryUsage;
    CPUMetrics cpuMetrics;
    NeuralMetrics neuralMetrics;
    
    // Detailed timing breakdown
    std::map<std::string, int64_t> phaseTimes;  // e.g., "setup", "simulation", "teardown"
    
    // Statistical data
    std::vector<double> throughputHistory;
    std::vector<double> memoryHistory;
    std::vector<double> cpuHistory;
    
    // Custom metrics
    std::map<std::string, double> customMetrics;
    
    BenchmarkResult() : totalTimeNs(0), cpuTimeNs(0), wallClockTimeNs(0),
                       stepsPerSecond(0.0), regions(0) {}
    
    // Helper methods
    double getTotalTimeSeconds() const { return static_cast<double>(totalTimeNs) / 1e9; }
    double getCPUTimeSeconds() const { return static_cast<double>(cpuTimeNs) / 1e9; }
    double getWallClockTimeSeconds() const { return static_cast<double>(wallClockTimeNs) / 1e9; }
    
    std::string toCSV() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(6);
        ss << benchmarkName << ",";
        ss << neuronCount << ",";
        ss << synapseCount << ",";
        ss << stepsPerSecond << ",";
        ss << getTotalTimeSeconds() << ",";
        ss << memoryUsage.percentageUsed << ",";
        ss << cpuMetrics.usagePercent << ",";
        ss << neuralMetrics.spikeCount << ",";
        ss << neuralMetrics.firingNeuronCount << ",";
        return ss.str();
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"benchmark\": \"" << benchmarkName << "\",\"neurons\": " << neuronCount
           << ",\"synapses\": " << synapseCount << ",\"steps\": " << steps
           << ",\"stepsPerSecond\": " << stepsPerSecond
           << ",\"totalTimeSeconds\": " << getTotalTimeSeconds()
           << ",\"memoryUsagePercent\": " << memoryUsage.percentageUsed
           << ",\"cpuUsagePercent\": " << cpuMetrics.usagePercent
           << ",\"spikeCount\": " << neuralMetrics.spikeCount << "}";
        return ss.str();
    }
};

// Benchmark status for real-time monitoring
enum class BenchmarkStatus {
    IDLE,
    RUNNING,
    PAUSED,
    COMPLETED,
    CANCELLED,
    ERROR
};

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;
    size_t neuronCount;
    size_t steps;
    size_t regions;
    std::string mode;  // benchmark type
    double durationLimit;  // seconds
    bool enableMemoryTracking;
    bool enableCPUMonitoring;
    bool enableRealTimeReporting;
    std::string outputFormat;  // console, json, csv, html
    std::string outputFile;
    std::map<std::string, double> thresholds;  // performance thresholds
    
    BenchmarkConfig() : neuronCount(500), steps(1000), regions(1),
                       mode("standard"), durationLimit(0.0),
                       enableMemoryTracking(true), enableCPUMonitoring(true),
                       enableRealTimeReporting(false), outputFormat("console") {}
};

// Advanced Benchmark Types
enum class BenchmarkType {
    STANDARD,
    MEMORY,
    PLASTICITY,
    SPIKE,
    INTEGRATION,
    SCALABILITY
};

// Real-time monitoring interface
class RealTimeMonitor {
public:
    virtual ~RealTimeMonitor() = default;
    
    virtual void onBenchmarkStart(const BenchmarkConfig& config) = 0;
    virtual void onBenchmarkProgress(double progress, const BenchmarkResult& current) = 0;
    virtual void onBenchmarkComplete(const BenchmarkResult& result) = 0;
    virtual void onBenchmarkError(const std::string& error) = 0;
    virtual void onPause() = 0;
    virtual void onResume() = 0;
    virtual void onInterrupt() = 0;
    
    virtual BenchmarkStatus getStatus() const = 0;
    virtual double getProgress() const = 0;
    virtual std::string getCurrentOperation() const = 0;
};

// Enhanced Benchmark Base Class
class BenchmarkBase {
public:
    virtual ~BenchmarkBase() = default;
    
    virtual BenchmarkType getType() const = 0;
    virtual std::string getName() const = 0;
    virtual BenchmarkResult run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) = 0;
    virtual bool validateConfig(const BenchmarkConfig& config) const = 0;
    virtual std::vector<std::string> getRequiredSystemFeatures() const = 0;
    
    virtual void setMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
        monitor_ = monitor;
    }
    
protected:
    std::shared_ptr<RealTimeMonitor> monitor_;
    
    BenchmarkResult createBaseResult(const std::string& name, size_t neuronCount, 
                                   size_t steps, size_t regions) {
        BenchmarkResult result;
        result.benchmarkName = name;
        result.neuronCount = neuronCount;
        result.steps = steps;
        result.regions = regions;
        return result;
    }
    
    void notifyProgress(double progress, const BenchmarkResult& current) {
        if (monitor_) {
            monitor_->onBenchmarkProgress(progress, current);
        }
    }
    
    void notifyError(const std::string& error) {
        if (monitor_) {
            monitor_->onBenchmarkError(error);
        }
    }
};

// Enhanced Benchmark Manager
class EnhancedBenchmarkManager {
public:
    EnhancedBenchmarkManager(std::shared_ptr<Logger> logger);
    ~EnhancedBenchmarkManager();
    
    // Configuration management
    void setConfig(const BenchmarkConfig& config) { config_ = config; }
    const BenchmarkConfig& getConfig() const { return config_; }
    
    // Benchmark management
    bool registerBenchmark(std::shared_ptr<BenchmarkBase> benchmark);
    bool runBenchmark(const std::string& name, std::shared_ptr<Brain> brain);
    void cancelCurrentBenchmark();
    void pauseBenchmark();
    void resumeBenchmark();
    
    // Real-time monitoring
    void attachMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    BenchmarkStatus getStatus() const { return status_; }
    double getOverallProgress() const { return overallProgress_; }
    BenchmarkResult getCurrentResult() const { return currentResult_; }
    
    // Result management
    void saveResult(const std::string& filename, const BenchmarkResult& result);
    std::vector<BenchmarkResult> getAllResults() const { return results_; }
    
    // Analysis and reporting
    std::string generateReport(const std::vector<BenchmarkResult>& results, 
                              const std::string& format = "text") const;
    std::string compareResults(const BenchmarkResult& result1, const BenchmarkResult& result2) const;
    bool detectRegression(const BenchmarkResult& baseline, const BenchmarkResult& current,
                        const std::map<std::string, double>& thresholds) const;
    
    // System resource monitoring
    MemoryUsage getCurrentMemoryUsage() const;
    CPUMetrics getCurrentCPUMetrics() const;
    NeuralMetrics getCurrentNeuralMetrics(std::shared_ptr<Brain> brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    BenchmarkConfig config_;
    BenchmarkStatus status_;
    double overallProgress_;
    BenchmarkResult currentResult_;
    std::vector<BenchmarkResult> results_;
    std::vector<std::shared_ptr<BenchmarkBase>> benchmarks_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<RealTimeMonitor> monitor_;
    std::thread monitoringThread_;
    std::mutex mutex_;
    bool monitoringEnabled_;
    
    void monitoringLoop();
    void updateResourceMetrics();
};

// Real-time monitor implementation
class ConsoleRealTimeMonitor : public RealTimeMonitor {
public:
    ConsoleRealTimeMonitor(std::shared_ptr<Logger> logger);
    
    void onBenchmarkStart(const BenchmarkConfig& config) override;
    void onBenchmarkProgress(double progress, const BenchmarkResult& current) override;
    void onBenchmarkComplete(const BenchmarkResult& result) override;
    void onBenchmarkError(const std::string& error) override;
    void onPause() override;
    void onResume() override;
    void onInterrupt() override;
    
    BenchmarkStatus getStatus() const override { return status_; }
    double getProgress() const override { return progress_; }
    std::string getCurrentOperation() const override { return currentOperation_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    BenchmarkStatus status_;
    double progress_;
    std::string currentOperation_;
    std::shared_ptr<Logger> logger_;
};

// Forward declarations for advanced benchmark implementations
class StandardBenchmark;
class MemoryBenchmark;
class PlasticityBenchmark;
class SpikeBenchmark;
class IntegrationBenchmark;
class ScalabilityBenchmark;

}}} // namespace nlm::benchmarking

