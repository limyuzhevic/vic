#pragma once

/**
 * Performance Monitor - Real-time performance monitoring for NLM
 * 
 * Provides comprehensive performance monitoring including CPU usage,
 * memory usage, execution time, and system resource tracking.
 * 
 * Features:
 * - Real-time performance metrics
 * - Resource usage tracking (CPU, memory, I/O)
 * - Performance benchmarking
 * - Progress tracking
 * - Performance alerts and thresholds
 * - Detailed performance statistics
 * - Performance trend analysis
 */

#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct PerformanceMetrics {
    std::chrono::system_clock::time_point timestamp;
    double cpuUsage;           // Percentage (0-100)
    double memoryUsage;        // In MB
    double memoryPercent;      // Percentage (0-100)
    double diskIO;             // MB/s
    double networkIO;          // MB/s
    size_t activeNeurons;
    size_t totalNeurons;
    size_t firingNeurons;
    size_t totalSpikes;
    float averageFiringRate;
    double simulationTime;
    size_t simulationSteps;
    size_t checkpointerSize;
    double temperature;        // CPU temperature if available
    
    PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), memoryPercent(0.0),
                           diskIO(0.0), networkIO(0.0), activeNeurons(0), totalNeurons(0),
                           firingNeurons(0), totalSpikes(0), averageFiringRate(0.0),
                           simulationTime(0.0), simulationSteps(0), checkpointerSize(0),
                           temperature(0.0) {}
    
    std::string toString() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Metrics: CPU=" << cpuUsage << "%, Mem=" << memoryUsage << "MB (" << memoryPercent << "%), "
           << "Neurons=" << activeNeurons << "/" << totalNeurons << ", "
           << "Firing=" << firingNeurons << ", Spikes=" << totalSpikes << ", "
           << "Rate=" << averageFiringRate << "/s, Time=" << simulationTime << "s, Steps=" << simulationSteps;
        return ss.str();
    }
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring(bool pause);
    bool isMonitoring() const { return monitoring_; }
    
    // Record current performance metrics
    void recordMetrics(size_t neurons, size_t totalNeurons, size_t firingNeurons,
                      size_t totalSpikes, float avgFiringRate, double simTime,
                      size_t steps, double cpuUsage, double memoryUsage);
    
    // Set performance thresholds
    void setMemoryThreshold(double thresholdMB) { memoryThresholdMB_ = thresholdMB; }
    void setCPUTreshold(double thresholdPercent) { cpuThresholdPercent_ = thresholdPercent; }
    void setTimeThreshold(double thresholdSeconds) { timeThresholdSeconds_ = thresholdSeconds; }
    
    // Get current metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get performance statistics
    struct PerformanceStats {
        double totalTime;
        size_t totalSteps;
        double avgCpuUsage;
        double avgMemoryUsage;
        double maxMemoryUsage;
        double avgFiringRate;
        double totalSpikesPerSecond;
        size_t peakNeurons;
        size_t peakFiringNeurons;
        size_t performanceViolations;
    };
    
    PerformanceStats getPerformanceStats() const { return currentStats_; }
    
    // Performance alerts
    using AlertCallback = std::function<void(const std::string& message, double severity)>;
    
    void addAlertCallback(AlertCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        alertCallbacks_.push_back(std::move(callback));
    }
    
    void removeAlertCallback(AlertCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        alertCallbacks_.erase(std::remove(alertCallbacks_.begin(), alertCallbacks_.end(), callback), alertCallbacks_.end());
    }
    
    // Progress tracking
    void startProgress(size_t totalSteps, const std::string& description = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        progress_.totalSteps = totalSteps;
        progress_.completedSteps = 0;
        progress_.description = description;
        progress_.startTime = std::chrono::system_clock::now();
        progress_.pausedTime = 0.0;
        progress_.active = true;
    }
    
    void incrementProgress(size_t steps = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (progress_.active) {
            progress_.completedSteps = std::min(progress_.completedSteps + steps, progress_.totalSteps);
            auto now = std::chrono::system_clock::now();
            progress_.elapsedTime = std::chrono::duration<double>(now - progress_.startTime).count();
            if (!progress_.paused) {
                progress_.totalElapsedTime = progress_.elapsedTime;
            }
        }
    }
    
    void pauseProgress(bool pause) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (progress_.active) {
            if (progress_.paused && !pause) {
                // Resume
                progress_.startTime = std::chrono::system_clock::now();
            } else if (!progress_.paused && pause) {
                // Pause
                progress_.pausedTime = progress_.elapsedTime;
            }
            progress_.paused = pause;
        }
    }
    
    ProgressInfo getProgress() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return progress_;
    }
    
    // Performance benchmarking
    class Benchmark {
    public:
        Benchmark(const std::string& name) : name_(name), startTime_(std::chrono::system_clock::now()) {}
        ~Benchmark() {
            auto endTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime_).count();
            PerformanceMonitor::getInstance().recordBenchmark(name_, duration);
        }
        
        void reset() {
            startTime_ = std::chrono::system_clock::now();
        }
        
        double getElapsedTime() const {
            auto endTime = std::chrono::system_clock::now();
            return std::chrono::duration<double>(endTime - startTime_).count();
        }
        
    private:
        std::string name_;
        std::chrono::system_clock::time_point startTime_;
    };
    
    Benchmark createBenchmark(const std::string& name) {
        return Benchmark(name);
    }
    
    // Benchmark results
    struct BenchmarkResult {
        std::string name;
        double duration;
        size_t iterations;
        double averageTimePerIteration;
        double minTime;
        double maxTime;
        double standardDeviation;
    };
    
    std::vector<BenchmarkResult> getBenchmarkResults() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return benchmarkResults_;
    }
    
    // Performance reports
    std::string generatePerformanceReport() const;
    std::string generateBenchmarkReport() const;
    
    // File output
    void enableFileOutput(const std::string& filepath, bool append = false);
    void disableFileOutput();
    bool isFileOutputEnabled() const { return fileOutput_.is_open(); }
    
    // Resource monitoring
    void enableSystemMonitoring(bool enable) { systemMonitoringEnabled_ = enable; }
    bool isSystemMonitoringEnabled() const { return systemMonitoringEnabled_; }
    
    // Performance analysis
    std::map<std::string, double> analyzePerformanceTrends(size_t windowSize = 100) const;
    
private:
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    void monitoringLoop();
    void updateSystemMetrics();
    void checkThresholds();
    void triggerAlerts(const std::string& message, double severity);
    
    mutable std::mutex mutex_;
    bool monitoring_;
    std::thread monitorThread_;
    bool monitoringThreadActive_;
    
    PerformanceMetrics currentMetrics_;
    PerformanceStats currentStats_;
    
    // Progress tracking
    struct ProgressInfo {
        size_t totalSteps;
        size_t completedSteps;
        std::string description;
        std::chrono::system_clock::time_point startTime;
        double elapsedTime;
        double totalElapsedTime;
        bool paused;
        bool active;
        
        ProgressInfo() : totalSteps(0), completedSteps(0), elapsedTime(0.0), 
                       totalElapsedTime(0.0), paused(false), active(false) {}
        
        double getProgressPercent() const {
            if (totalSteps == 0) return 0.0;
            return (double)completedSteps / totalSteps * 100.0;
        }
        
        double getEstimatedTimeRemaining() const {
            if (!active || completedSteps == 0) return 0.0;
            double rate = totalElapsedTime / completedSteps;
            return (totalSteps - completedSteps) * rate;
        }
    };
    
    ProgressInfo progress_;
    
    // Benchmarking
    struct BenchmarkInfo {
        std::string name;
        double duration;
        std::chrono::system_clock::time_point startTime;
    };
    
    std::vector<BenchmarkInfo> activeBenchmarks_;
    std::vector<BenchmarkResult> benchmarkResults_;
    
    // Alert system
    std::vector<AlertCallback> alertCallbacks_;
    
    // Thresholds
    double memoryThresholdMB_;
    double cpuThresholdPercent_;
    double timeThresholdSeconds_;
    
    // File output
    std::ofstream fileOutput_;
    std::string currentLogFile_;
    
    // System monitoring
    bool systemMonitoringEnabled_;
    std::chrono::system_clock::time_point lastSystemUpdate_;
    double updateInterval_;
};

// Global performance monitor instance
inline PerformanceMonitor& getPerformanceMonitor() {
    static PerformanceMonitor instance;
    return instance;
}

} // namespace nlm