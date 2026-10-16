#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <atomic>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <deque>
#include <optional>
#include <thread>
#include <condition_variable>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Performance monitoring data structures

// Performance category enumeration
enum class PerformanceCategory {
    General,
    Neural,
    Memory,
    CPU,
    I/O,
    Network,
    Plasticity,
    Simulation,
    Visualization,
    Optimization,
    Custom
};

// Performance alert levels
enum class PerformanceAlertLevel {
    Info,
    Warning,
    Critical,
    Error
};

// Performance metric type
struct PerformanceMetric {
    std::string name;
    double value;
    std::string unit;
    PerformanceCategory category;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    PerformanceMetric() : value(0.0), category(PerformanceCategory::General) {}
    
    PerformanceMetric(const std::string& n, double v, const std::string& u, 
                      PerformanceCategory c = PerformanceCategory::General)
        : name(n), value(v), unit(u), category(c), 
          timestamp(std::chrono::system_clock::now()) {}
};

// Performance alert structure
struct PerformanceAlert {
    PerformanceAlertLevel level;
    std::string message;
    std::string metricName;
    double threshold;
    double actualValue;
    std::chrono::system_clock::time_point timestamp;
    std::string operation;
    
    PerformanceAlert(PerformanceAlertLevel l, const std::string& msg, 
                    const std::string& metric, double thresh, double actual)
        : level(l), message(msg), metricName(metric), threshold(thresh), 
          actualValue(actual), timestamp(std::chrono::system_clock::now()) {}
};

// Performance report structure
struct PerformanceReport {
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double duration;
    size_t totalSteps;
    size_t totalSpikes;
    std::map<std::string, double> metrics;
    std::vector<PerformanceAlert> alerts;
    std::vector<PerformanceMetric> metricHistory;
    std::string summary;
    std::map<std::string, std::string> additionalData;
    
    std::string toJSON() const;
    std::string toCSV() const;
    std::string toText() const;
};

// Performance threshold configuration
struct PerformanceThreshold {
    std::string metricName;
    double warningLevel;
    double criticalLevel;
    PerformanceCategory category;
    bool enabled;
    std::function<bool(double)> condition;
    
    PerformanceThreshold(const std::string& name, double warning, double critical, 
                        PerformanceCategory cat = PerformanceCategory::General)
        : metricName(name), warningLevel(warning), criticalLevel(critical), 
          category(cat), enabled(true) {
        condition = [this](double value) {
            return value >= warningLevel || value <= criticalLevel;
        };
    }
};

// Core performance monitoring class
class PerformanceMonitor {
public:
    using AlertCallback = std::function<void(const PerformanceAlert&)>
    using MetricCallback = std::function<void(const PerformanceMetric&)>
    using ReportCallback = std::function<void(const PerformanceReport&)>
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    // Singleton pattern
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    // Initialization and configuration
    void initialize(const std::string& configFile = "");
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // Performance metrics recording
    void recordMetric(const PerformanceMetric& metric);
    void recordMetric(const std::string& name, double value, 
                     const std::string& unit = "",
                     PerformanceCategory category = PerformanceCategory::General,
                     const std::map<std::string, std::string>& metadata = {});
    
    // Neural network performance
    void recordSpikeCount(size_t spikeCount, double time);
    void recordFiringRate(double rate, const std::string& population = "");
    void recordActiveNeurons(size_t activeCount, const std::string& population = "");
    void recordExcitationInhibitionRatio(double ratio, const std::string& population = "");
    void recordConnectionDensity(double density, const std::string& population = "");
    
    // Simulation performance
    void recordStepPerformance(double duration, size_t step,
                               double time, size_t spikeCount);
    void recordOperationPerformance(const std::string& operation,
                                    double duration,
                                    PerformanceCategory category = PerformanceCategory::General,
                                    uint64_t memoryUsage = 0);
    
    // Resource performance
    void recordCPUUsage(double usage);
    void recordMemoryUsage(uint64_t memoryBytes);
    void recordIOThroughput(double readMBps, double writeMBps);
    void recordNetworkThroughput(double bytesPerSecond);
    
    // Performance alerts
    void addThreshold(const PerformanceThreshold& threshold);
    void removeThreshold(const std::string& metricName);
    void setAlertCallback(AlertCallback callback) { alertCallback_ = callback; }
    void setMetricCallback(MetricCallback callback) { metricCallback_ = callback; }
    void setReportCallback(ReportCallback callback) { reportCallback_ = callback; }
    
    // Performance queries
    double getMetric(const std::string& name) const;
    PerformanceMetric getLatestMetric(const std::string& name) const;
    std::vector<PerformanceMetric> getMetrics(const std::string& name, 
                                              size_t count = 100) const;
    std::vector<PerformanceMetric> getMetricsByCategory(PerformanceCategory category,
                                                        size_t count = 100) const;
    
    // Performance statistics
    size_t getTotalSpikes() const { return totalSpikes_; }
    size_t getTotalSteps() const { return totalSteps_; }
    double getAverageFiringRate() const { return averageFiringRate_; }
    double getAverageStepDuration() const { return averageStepDuration_; }
    
    // Performance report generation
    PerformanceReport generateReport() const;
    PerformanceReport generateReport(const std::string& startTime,
                                     const std::string& endTime) const;
    
    // Performance export/import
    bool exportMetrics(const std::string& filepath, const std::string& format = "json") const;
    bool importMetrics(const std::string& filepath);
    
    // Performance analysis
    std::string analyzeTrends(const std::string& metricName, size_t windowSize = 10) const;
    std::string identifyBottlenecks() const;
    std::string getPerformanceSummary() const;
    
    // Performance monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }
    
    // Real-time performance display
    void startRealTimeDisplay(double refreshInterval = 1.0);
    void stopRealTimeDisplay();
    bool isRealTimeDisplayActive() const { return realTimeDisplayActive_; }
    
    // Integration with existing systems
    void integrateWithLogger(std::shared_ptr<Logger> logger);
    void integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector);
    void integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    void integrateWithLogAnalyzer(std::shared_ptr<LogAnalyzer> analyzer);
    
    // Performance hooks for simulation
    void onStepStart(size_t step, double time);
    void onStepEnd(size_t step, double time, double duration, size_t spikeCount);
    void onOperationStart(const std::string& operation, PerformanceCategory category);
    void onOperationEnd(const std::string& operation, double duration, uint64_t memoryUsage);
    
    // Performance monitoring for specific modes
    void monitorBasicConnectivityMode();
    void monitorPlasticityExperimentMode();
    void monitorStdpVerificationMode();
    void monitorBenchmarkMode();
    void monitorContinuousMode();
    
    // Performance monitoring for runtime configuration
    void monitorRuntimeConfigDemoMode();
    void monitorRuntimeConfigCommandInterfaceMode();
    void monitorRuntimeConfigTestMode();
    
    // Performance monitoring for visualization
    void monitorDemoMode();
    void monitorContinuousVisualizationMode();
    
    // Performance monitoring for Python interface
    void monitorPythonInterfaceMode();
    
    // Performance monitoring for scaling benchmark
    void monitorScalingBenchmarkMode();
    
    // Performance monitoring for results saving
    void monitorResultsSaveMode(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool initialized_ = false;
    bool monitoring_ = false;
    bool realTimeDisplayActive_ = false;
    
    // Performance data storage
    mutable std::mutex dataMutex_;
    std::deque<PerformanceMetric> metricHistory_;
    std::deque<PerformanceMetric> neuralMetrics_;
    std::deque<PerformanceMetric> simulationMetrics_;
    std::deque<PerformanceMetric> resourceMetrics_;
    
    // Performance thresholds
    std::vector<PerformanceThreshold> thresholds_;
    
    // Performance callbacks
    AlertCallback alertCallback_;
    MetricCallback metricCallback_;
    ReportCallback reportCallback_;
    
    // Performance statistics
    size_t totalSpikes_ = 0;
    size_t totalSteps_ = 0;
    double totalFiringRate_ = 0.0;
    double totalStepDuration_ = 0.0;
    double averageFiringRate_ = 0.0;
    double averageStepDuration_ = 0.0;
    
    // Performance monitoring integration
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<StatisticsCollector> statisticsCollector_;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor_;
    std::shared_ptr<LogAnalyzer> logAnalyzer_;
    
    // Real-time display thread
    std::thread realTimeDisplayThread_;
    
    // Performance monitoring configuration
    double samplingInterval_ = 0.1; // seconds
    size_t maxHistorySize_ = 10000;
    bool enableRealTimeDisplay_ = true;
    bool enablePerformanceAlerts_ = true;
    
    // Performance monitoring implementation
    void updatePerformanceStatistics();
    void checkPerformanceThresholds();
    void generateRealTimeDisplay();
    void recordPerformanceMetric(const PerformanceMetric& metric);
    void notifyMetricListeners(const PerformanceMetric& metric);
    void notifyAlertListeners(const PerformanceAlert& alert);
    void notifyReportListeners(const PerformanceReport& report);
};

} // namespace nlm
