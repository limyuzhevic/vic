#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <atomic>
#include <thread>
#include <optional>
#include "MetricsRecorder.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Metric definition for monitoring
struct MetricDefinition {
    std::string name;
    std::string description;
    std::string unit;
    std::function<double()> getValue; // Function to get current value
    std::chrono::milliseconds updateInterval;
    LogLevel logLevel;
    bool logToConsole;
    bool logToFile;
    bool trackHistory;
    
    MetricDefinition(const std::string& name, const std::string& desc = "",
                     const std::string& unit = "",
                     const std::function<double()>& getter = nullptr,
                     const std::chrono::milliseconds& interval = std::chrono::milliseconds(1000),
                     LogLevel level = LogLevel::Info,
                     bool console = true, bool file = false,
                     bool history = true)
        : name(name), description(desc), unit(unit), getValue(getter),
          updateInterval(interval), logLevel(level), logToConsole(console), 
          logToFile(file), trackHistory(history) {}
};

// Real-time metric value
struct RealTimeMetric {
    std::string name;
    double value;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string label;
    
    RealTimeMetric(const std::string& metricName, double val, const std::string& lbl = "")
        : name(metricName), value(val), label(lbl) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Performance monitoring metrics
struct PerformanceMetrics {
    // CPU/Memory
    double cpuUsage = 0.0;           // CPU utilization (%)
    double memoryUsageMB = 0.0;      // Memory usage (MB)
    double memoryUsagePercent = 0.0; // Memory utilization (%)
    
    // Neural system
    size_t neuronCount = 0;          // Total neurons
    size_t synapseCount = 0;         // Total synapses
    size_t activeNeurons = 0;       // Currently active neurons
    size_t firingNeurons = 0;        // Currently firing neurons
    size_t totalSpikes = 0;          // Total spikes since start
    size_t pendingSpikes = 0;        // Pending spike events
    size_t pendingDelayedSpikes = 0; // Pending delayed spike events
    
    // Network metrics
    float avgFiringRate = 0.0;       // Average firing rate (Hz)
    float e_iRatio = 0.0;            // Excitatory/Inhibitory ratio
    
    // Performance
    double stepTimeMs = 0.0;         // Average step time (ms)
    double realTimeFactor = 0.0;     // Real-time simulation factor
    double neuronsPerSecond = 0.0;   // Neurons processed per second
    double spikesPerSecond = 0.0;    // Spikes processed per second
    
    // Simulation state
    size_t currentStep = 0;          // Current simulation step
    double currentTime = 0.0;        // Current simulation time (s)
    
    // System resources
    size_t threadCount = std::thread::hardware_concurrency(); // Available threads
    float eventDrivenEnabled = 0.0;  // Event-driven processing enabled (0/1)
    float multithreadingEnabled = 0.0; // Multithreading enabled (0/1)
    float simdEnabled = 0.0;         // SIMD enabled (0/1)
    float sparseEnabled = 0.0;       // Sparse connectivity enabled (0/1)
};

// Monitoring configuration
struct MonitoringConfig {
    std::chrono::milliseconds updateInterval = std::chrono::milliseconds(100); // 10 Hz update rate
    bool enableRealTimeMetrics = true;
    bool enablePerformanceMonitoring = true;
    bool enableHistoryTracking = true;
    bool enableConsoleOutput = true;
    bool enableFileOutput = false;
    std::string outputFile;
    bool enableLogging = true;
    LogLevel minimumLogLevel = LogLevel::Info;
    bool enableNotifications = false;
    std::function<void(const std::string&)> notificationCallback;
    
    // Metric-specific settings
    struct MetricConfig {
        std::string name;
        std::chrono::milliseconds updateInterval;
        bool enableRealTimeUpdates;
        bool enableHistoricalTracking;
        LogLevel logLevel;
    };
    std::vector<MetricConfig> metricConfigs;
};

// Monitoring statistics
struct MonitoringStats {
    size_t totalMetrics = 0;
    size_t activeMetrics = 0;
    size_t realTimeMetrics = 0;
    double cpuUsageAvg = 0.0;
    double memoryUsageAvg = 0.0;
    double simulationSpeedAvg = 0.0;
    size_t totalSteps = 0;
    double totalSimulationTime = 0.0;
    size_t recordingStartTime = 0;
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Total metrics: " << totalMetrics << "\n"
            << "Active metrics: " << activeMetrics << "\n"
            << "Real-time metrics: " << realTimeMetrics << "\n"
            << "CPU usage avg: " << cpuUsageAvg << "%\n"
            << "Memory usage avg: " << memoryUsageAvg << " MB\n"
            << "Simulation speed avg: " << simulationSpeedAvg << " steps/s\n"
            << "Total steps: " << totalSteps << "\n"
            << "Total simulation time: " << totalSimulationTime << " s\n"
            << "Recording duration: " << (totalSteps > 0 ? (totalSimulationTime / totalSteps) : 0.0) << " s per step\n";
        return oss.str();
    }
};

// Monitoring manager for real-time metrics dashboard
class MonitoringManager {
public:
    MonitoringManager();
    ~MonitoringManager();
    
    // Disable copying
    MonitoringManager(const MonitoringManager&) = delete;
    MonitoringManager& operator=(const MonitoringManager&) = delete;
    
    // Initialize monitoring manager
    bool initialize(const MonitoringConfig& config = MonitoringConfig());
    
    // Start/stop monitoring
    void start();
    void stop();
    bool isRunning() const { return running_.load(); }
    
    // Configure monitoring
    void configure(const MonitoringConfig& config);
    MonitoringConfig getConfig() const { return config_; }
    
    // Register metrics
    void registerMetric(const MetricDefinition& metric);
    void registerNeuralMetrics(const class Brain& brain);
    void registerPerformanceMetrics(const class OptimizedBrain* optimizedBrain = nullptr);
    void registerSpikeMetrics(const std::vector<class DetailedSpikeEvent>& spikes);
    
    // Update monitoring data
    void update();
    void update(const class Brain& brain, Timestamp currentTime, SimulationStep currentStep);
    
    // Get current metrics
    const PerformanceMetrics& getPerformanceMetrics() const { return performanceMetrics_; }
    const std::vector<RealTimeMetric>& getRealTimeMetrics() const { return realTimeMetrics_; }
    
    // Get specific metric value
    std::optional<double> getMetricValue(const std::string& name) const;
    
    // Get metrics statistics
    MonitoringStats getStats() const;
    
    // Get all registered metric names
    std::vector<std::string> getMetricNames() const;
    
    // Get real-time metric history
    std::vector<RealTimeMetric> getRealTimeHistory(const std::string& name, size_t limit = 100) const;
    
    // Get formatted metrics for display
    std::string getFormattedMetrics() const;
    std::string getFormattedPerformanceMetrics() const;
    std::string getFormattedRealTimeMetrics() const;
    
    // Export functionality
    bool exportMetrics(const std::string& filename, const std::string& format = "csv");
    bool exportPerformanceMetrics(const std::string& filename, const std::string& format = "json");
    
    // History management
    void clearHistory();
    void clearOldData(const std::chrono::milliseconds& age);
    
    // Configuration management
    void setUpdateInterval(const std::chrono::milliseconds& interval);
    std::chrono::milliseconds getUpdateInterval() const { return config_.updateInterval; }
    
    // Enable/disable components
    void setRealTimeMetricsEnabled(bool enabled) { config_.enableRealTimeMetrics = enabled; }
    void setPerformanceMonitoringEnabled(bool enabled) { config_.enablePerformanceMonitoring = enabled; }
    void setHistoryTrackingEnabled(bool enabled) { config_.enableHistoryTracking = enabled; }
    
    bool isRealTimeMetricsEnabled() const { return config_.enableRealTimeMetrics; }
    bool isPerformanceMonitoringEnabled() const { return config_.enablePerformanceMonitoring; }
    bool isHistoryTrackingEnabled() const { return config_.enableHistoryTracking; }
    
    // Notifications
    void setNotificationCallback(const std::function<void(const std::string&)>& callback) {
        config_.notificationCallback = callback;
    }
    
    // Get singleton instance
    static MonitoringManager& getInstance() {
        static MonitoringManager instance;
        return instance;
    }
    
    // Global monitoring functions
    static void updateGlobal(const class Brain& brain, Timestamp currentTime, SimulationStep currentStep);
    static void recordGlobalMetric(const std::string& name, double value, const std::string& label = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::shared_mutex mutex_;
    std::atomic<bool> running_;
    MonitoringConfig config_;
    PerformanceMetrics performanceMetrics_;
    std::vector<RealTimeMetric> realTimeMetrics_;
    std::chrono::high_resolution_clock::time_point lastUpdateTime_;
    
    // Internal helper methods
    void updateMetrics(const class Brain& brain, Timestamp currentTime, SimulationStep currentStep);
    void updatePerformanceMetrics(const class OptimizedBrain* optimizedBrain = nullptr);
    void updateRealTimeMetrics();
    void processMetrics();
    void logMetrics() const;
    void sendNotifications() const;
    
    // CPU and memory monitoring helpers
    static double getCurrentCPUUsage();
    static double getCurrentMemoryUsageMB();
    static double getCurrentMemoryUsagePercent();
    
    // File output helpers
    void writeToFile(const std::string& filename, const std::string& data) const;
    
    // Export helpers
    bool exportToCSV(const std::string& filename) const;
    bool exportToJSON(const std::string& filename) const;
    bool exportToXML(const std::string& filename) const;
};

} // namespace nlm
