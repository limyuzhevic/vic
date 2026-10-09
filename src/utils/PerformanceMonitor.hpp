#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <functional>
#include <variant>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Performance monitor state for tracking system performance
struct PerformanceState {
    // System metrics
    double cpuUsage{0.0};                    // Current CPU usage (0-1)
    double memoryUsage{0.0};                // Current memory usage (MB)
    double memoryAvailable{0.0};            // Available memory (MB)
    double diskReadRate{0.0};               // Disk read rate (MB/s)
    double diskWriteRate{0.0};              // Disk write rate (MB/s)
    double networkReceiveRate{0.0};        // Network receive rate (Mbps)
    double networkTransmitRate{0.0};       // Network transmit rate (Mbps)
    
    // Thread metrics
    size_t activeThreads{0};               // Number of active threads
    size_t totalThreads{0};                 // Total number of threads
    double averageThreadCpu{0.0};          // Average CPU per thread
    double maxThreadCpu{0.0};              // Maximum CPU usage by any thread
    
    // Process metrics
    size_t processId{0};                   // Process ID
    double processCpuTime{0.0};            // Process CPU time (seconds)
    double processUserTime{0.0};           // Process user time (seconds)
    double processSystemTime{0.0};         // Process system time (seconds)
    size_t processMemory{0};               // Process memory usage (bytes)
    size_t processPeakMemory{0};           // Peak process memory usage (bytes)
    
    // I/O metrics
    uint64_t ioReads{0};                   // Total I/O read operations
    uint64_t ioWrites{0};                  // Total I/O write operations
    uint64_t ioReadBytes{0};               // Total I/O read bytes
    uint64_t ioWriteBytes{0};              // Total I/O write bytes
    
    // Network metrics
    uint64_t networkPacketsReceived{0};    // Total network packets received
    uint64_t networkPacketsSent{0};        // Total network packets sent
    uint64_t networkBytesReceived{0};      // Total network bytes received
    uint64_t networkBytesSent{0};          // Total network bytes sent
    
    // Timestamp
    std::chrono::steady_clock::time_point timestamp;
};

// Performance alert levels
enum class AlertLevel {
    Info = 0,
    Warning = 1,
    Critical = 2,
    Severe = 3
};

// Performance alert
struct PerformanceAlert {
    AlertLevel level{AlertLevel::Info};
    std::string message;
    std::string metricName;
    double value{0.0};
    double threshold{0.0};
    std::chrono::steady_clock::time_point timestamp;
    bool acknowledged{false};
};

// Performance metrics history for trend analysis
struct PerformanceHistory {
    std::deque<PerformanceState> samples;
    std::chrono::steady_clock::time_point startTime;
    size_t maxHistorySize{1000};
    
    // Calculate rolling averages for metrics
    double getRollingAverageCpu(size_t window) const {
        if (samples.size() < window) return 0.0;
        double sum = 0.0;
        for (size_t i = 0; i < window && i < samples.size(); ++i) {
            sum += samples[samples.size() - 1 - i].cpuUsage;
        }
        return sum / window;
    }
    
    double getRollingAverageMemory(size_t window) const {
        if (samples.size() < window) return 0.0;
        double sum = 0.0;
        for (size_t i = 0; i < window && i < samples.size(); ++i) {
            sum += samples[samples.size() - 1 - i].memoryUsage;
        }
        return sum / window;
    }
    
    // Detect performance trends
    std::string analyzeTrend(const std::string& metric, size_t window = 10) const {
        if (samples.size() < window + 1) return "Insufficient data";
        
        double current = 0.0;
        double previous = 0.0;
        
        if (metric == "cpu") {
            current = samples.back().cpuUsage;
            previous = samples[samples.size() - 1 - window].cpuUsage;
        } else if (metric == "memory") {
            current = samples.back().memoryUsage;
            previous = samples[samples.size() - 1 - window].memoryUsage;
        }
        
        if (current > previous * 1.1) return "Increasing";
        else if (current < previous * 0.9) return "Decreasing";
        else return "Stable";
    }
    
    // Detect anomalies in metrics
    std::vector<std::string> detectAnomalies(const std::string& metric, double threshold = 2.0) const {
        std::vector<std::string> anomalies;
        
        if (samples.size() < 3) return anomalies;
        
        double mean = 0.0;
        double variance = 0.0;
        
        for (const auto& sample : samples) {
            double value = (metric == "cpu") ? sample.cpuUsage : 
                          (metric == "memory") ? sample.memoryUsage : 0.0;
            mean += value;
        }
        mean /= samples.size();
        
        for (const auto& sample : samples) {
            double value = (metric == "cpu") ? sample.cpuUsage : 
                          (metric == "memory") ? sample.memoryUsage : 0.0;
            variance += (value - mean) * (value - mean);
        }
        variance /= samples.size();
        double stdDev = std::sqrt(variance);
        
        if (stdDev == 0.0) return anomalies;
        
        for (size_t i = 0; i < samples.size(); ++i) {
            double value = (metric == "cpu") ? samples[i].cpuUsage : 
                          (metric == "memory") ? samples[i].memoryUsage : 0.0;
            if (std::abs(value - mean) > threshold * stdDev) {
                anomalies.push_back("Anomaly at sample " + std::to_string(i) + ": " + std::to_string(value));
            }
        }
        
        return anomalies;
    }
};

// Performance monitor configuration
struct PerformanceMonitorConfig {
    bool enabled{true};                     // Enable performance monitoring
    std::chrono::milliseconds samplingInterval{1000};  // Sampling interval (ms)
    size_t historySize{1000};               // Maximum history size
    bool trackThreads{true};                // Track thread performance
    bool trackIO{true};                     // Track I/O operations
    bool trackNetwork{true};                // Track network operations
    bool trackProcesses{true};              // Track process metrics
    std::string outputFile;                 // Output file for metrics
    std::string format{"json"};             // Output format (json, csv, text)
    bool continuous{false};                 // Continuous monitoring
    bool alertsEnabled{true};               // Enable performance alerts
    double cpuThreshold{0.90};              // CPU alert threshold
    double memoryThreshold{0.90};           // Memory alert threshold
    std::function<void(const PerformanceAlert&)> alertCallback;  // Alert callback
    std::unordered_map<std::string, double> customThresholds;   // Custom thresholds
    
    // Validate configuration
    bool validate() const {
        if (samplingInterval.count() < 10) return false;  // Minimum 10ms interval
        if (historySize == 0) return false;
        return true;
    }
};

// Main performance monitor class
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;
    
    // Configuration
    bool configure(const PerformanceMonitorConfig& config);
    PerformanceMonitorConfig getConfig() const { return config; }
    
    // Control
    bool startMonitoring();
    bool stopMonitoring();
    bool pauseMonitoring();
    bool resumeMonitoring();
    
    // State queries
    PerformanceState getCurrentState() const;
    PerformanceHistory getHistory() const;
    std::vector<PerformanceAlert> getAlerts() const;
    bool isMonitoring() const { return monitoring; }
    bool isPaused() const { return paused; }
    
    // Metrics access
    double getCurrentCpuUsage() const;
    double getCurrentMemoryUsage() const;
    double getCurrentMemoryAvailable() const;
    size_t getActiveThreadCount() const;
    
    // Analysis and reporting
    std::string generateReport() const;
    bool exportMetrics(const std::string& filename, const std::string& format = "text");
    bool exportHistory(const std::string& filename, const std::string& format = "csv");
    
    // Alert management
    void acknowledgeAlert(size_t index);
    void clearAlerts();
    std::vector<PerformanceAlert> getActiveAlerts() const;
    
    // System information
    std::string getSystemInfo() const;
    std::unordered_map<std::string, std::string> getSystemInfoMap() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PerformanceMonitorConfig config;
    bool monitoring;
    bool paused;
    std::thread monitorThread;
    
    // Performance monitoring thread
    void monitoringWorker();
    
    // System data collection
    void collectSystemMetrics(PerformanceState& state) const;
    void collectCpuMetrics(PerformanceState& state) const;
    void collectMemoryMetrics(PerformanceState& state) const;
    void collectDiskMetrics(PerformanceState& state) const;
    void collectNetworkMetrics(PerformanceState& state) const;
    void collectThreadMetrics(PerformanceState& state) const;
    void collectProcessMetrics(PerformanceState& state) const;
    void collectIoMetrics(PerformanceState& state) const;
    
    // Alert management
    void checkForAlerts(const PerformanceState& state);
    void triggerAlert(const std::string& metricName, double value, AlertLevel level, const std::string& message);
    
    // File I/O
    bool writeToFile(const std::string& filename, const std::string& content) const;
    bool ensureOutputDirectory(const std::string& filename) const;
    
    // Helper functions
    double calculateCpuLoad() const;
    double calculateMemoryUsage() const;
    double calculateMemoryAvailable() const;
    void updateDiskMetrics(double& readRate, double& writeRate) const;
    void updateNetworkMetrics(double& receiveRate, double& transmitRate) const;
    void updateThreadMetrics(size_t& activeThreads, size_t& totalThreads, 
                            double& avgThreadCpu, double& maxThreadCpu) const;
};

} // namespace nlm