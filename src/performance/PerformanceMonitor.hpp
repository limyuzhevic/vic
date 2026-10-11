// Performance monitoring and benchmarking infrastructure
#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <iomanip>
#include <thread>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

// Performance metrics collection
struct PerformanceMetrics {
    // Time measurements
    double simulationTime;      // Total simulation time (seconds)
    double stepTime;           // Average time per step (seconds)
    double neuronUpdateTime;   // Time spent updating neurons
    double spikeDeliveryTime;  // Time spent delivering spikes
    double plasticityTime;     // Time spent on plasticity calculations
    double memoryTime;         // Time spent on memory operations
    
    // Count measurements
    size_t totalSpikes;        // Total spikes generated
    size_t activeNeurons;      // Average number of active neurons
    size_t firingNeurons;       // Number of firing neurons this step
    size_t totalSteps;         // Total simulation steps completed
    size_t neuronsProcessed;    // Total neuron updates
    size_t spikesProcessed;    // Total spikes processed
    
    // Memory measurements
    size_t memoryUsage;        // Current memory usage (bytes)
    size_t peakMemoryUsage;    // Peak memory usage (bytes)
    size_t allocationCount;    // Number of memory allocations
    size_t deallocationCount;  // Number of memory deallocations
    
    // System measurements
    double cpuUsage;          // CPU usage percentage
    double memoryUsagePercent; // Memory usage percentage
    uint64_t timestep;        // Current simulation timestep
    uint64_t currentStep;     // Current simulation step
    
    // Network statistics
    size_t totalNeuronCount;   // Total neurons in network
    size_t totalSynapseCount;  // Total synapses in network
    float excitationInhibitionRatio; // E/I balance ratio
    float averageFiringRate;  // Average firing rate across network
    float connectivityDensity; // Network connectivity density
    
    // Learning statistics
    size_t learningEvents;    // Number of learning events
    float plasticityTotal;   // Total weight change from plasticity
    size_t stdpEvents;        // Number of STDP events
    size_t hebbianEvents;     // Number of Hebbian events
    
    // Development statistics
    float developmentProgress; // Developmental stage progress (0-1)
    size_t developmentEvents;   // Number of developmental events
    
    // Timestamp for metrics collection
    std::chrono::high_resolution_clock::time_point timestamp;
    
    PerformanceMetrics() : simulationTime(0.0), stepTime(0.0), neuronUpdateTime(0.0),
                          spikeDeliveryTime(0.0), plasticityTime(0.0), memoryTime(0.0),
                          totalSpikes(0), activeNeurons(0), firingNeurons(0),
                          totalSteps(0), neuronsProcessed(0), spikesProcessed(0),
                          memoryUsage(0), peakMemoryUsage(0), allocationCount(0),
                          deallocationCount(0), cpuUsage(0.0), memoryUsagePercent(0.0),
                          timestep(0), currentStep(0), totalNeuronCount(0),
                          totalSynapseCount(0), excitationInhibitionRatio(0.0f),
                          averageFiringRate(0.0f), connectivityDensity(0.0f),
                          learningEvents(0), plasticityTotal(0.0f), stdpEvents(0),
                          hebbianEvents(0), developmentProgress(0.0f),
                          developmentEvents(0), timestamp(std::chrono::high_resolution_clock::now()) {}
};

// Performance monitoring configuration
struct PerformanceMonitorConfig {
    bool enableTiming;           // Enable time measurements
    bool enableMemoryTracking;   // Enable memory tracking
    bool enableNetworkStats;     // Enable network statistics
    bool enableLearningStats;    // Enable learning statistics
    bool enableDevelopmentStats; // Enable development statistics
    bool enableCPUMonitoring;    // Enable CPU monitoring
    bool enableMemoryMonitoring; // Enable memory monitoring
    
    // Sampling frequency
    double metricsInterval;      // Interval between metric collections (steps)
    double samplingFrequency;    // Sampling frequency for detailed metrics
    
    // Output configuration
    std::string outputFile;      // File for writing metrics
    std::string format;          // Output format (json, csv, text)
    bool realTimeOutput;         // Enable real-time output
    
    // Thresholds for alerts
    double maxStepTime;          // Maximum allowed step time (seconds)
    double maxMemoryUsage;       // Maximum allowed memory usage (MB)
    double maxCPUUsage;          // Maximum allowed CPU usage (%)
    
    PerformanceMonitorConfig() : enableTiming(true), enableMemoryTracking(true),
                                  enableNetworkStats(true), enableLearningStats(true),
                                  enableDevelopmentStats(true), enableCPUMonitoring(true),
                                  enableMemoryMonitoring(true), metricsInterval(1.0),
                                  samplingFrequency(10.0), outputFile(""), format("json"),
                                  realTimeOutput(false), maxStepTime(1.0),
                                  maxMemoryUsage(1024.0), maxCPUUsage(100.0) {}
};

// Event-driven performance monitoring
class PerformanceEvent {
public:
    enum class Type {
        STEP_COMPLETE,
        SPIKE_GENERATED,
        PLASTICITY_UPDATE,
        MEMORY_ALLOCATED,
        MEMORY_DEALLOCATED,
        NETWORK_STATISTICS,
        LEARNING_EVENT,
        DEVELOPMENT_EVENT,
        SYSTEM_STATUS
    };
    
    Type type;
    double timestamp;
    uint64_t step;
    std::unordered_map<std::string, double> data;
    std::string description;
    
    PerformanceEvent(Type t, double ts, uint64_t s, const std::string& desc = "")
        : type(t), timestamp(ts), step(s), description(desc) {}
    
    template<typename T>
    void setData(const std::string& key, T value) {
        data[key] = static_cast<double>(value);
    }
    
    template<typename T>
    T getData(const std::string& key, T defaultValue = T{}) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return static_cast<T>(it->second);
        }
        return defaultValue;
    }
};

// Performance monitoring and benchmarking system
class PerformanceMonitor {
public:
    PerformanceMonitor(const PerformanceMonitorConfig& config = PerformanceMonitorConfig());
    ~PerformanceMonitor();
    
    // Event processing
    void recordEvent(const PerformanceEvent& event);
    void processEvents();
    std::vector<PerformanceEvent> getEvents() const { return events_; }
    
    // Metrics management
    void updateMetrics(const PerformanceMetrics& metrics);
    PerformanceMetrics getCurrentMetrics() const { return currentMetrics_; }
    std::vector<PerformanceMetrics> getHistoricalMetrics() const { return historicalMetrics_; }
    
    // Configuration
    void setConfig(const PerformanceMonitorConfig& config) { config_ = config; }
    PerformanceMonitorConfig getConfig() const { return config_; }
    
    // Output and reporting
    bool saveMetrics(const std::string& filepath) const;
    std::string generateReport() const;
    std::string generateJSONReport() const;
    std::string generateCSVReport() const;
    std::string generateTextReport() const;
    
    // Performance analysis
    double calculateAverageStepTime() const;
    double calculateStepTimeStdDev() const;
    double calculatePerformanceScore() const;
    std::string getPerformanceGrade() const;
    
    // Event filtering and querying
    std::vector<PerformanceEvent> getEventsByType(PerformanceEvent::Type type) const;
    std::vector<PerformanceEvent> getEventsInTimeRange(double startTime, double endTime) const;
    std::vector<PerformanceEvent> getEventsByStep(uint64_t step) const;
    
    // Configuration file output
    bool generateConfigurationReport(const std::string& filepath) const;
    bool generatePerformanceProfile(const std::string& filepath) const;
    
    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isMonitoringRealTime() const { return realTimeMonitoring_; }
    
    // Performance alerts and thresholds
    bool checkThresholds() const;
    std::vector<std::string> getPerformanceAlerts() const { return alerts_; }
    void clearAlerts() { alerts_.clear(); }
    
    // Benchmarking utilities
    void startBenchmark(const std::string& benchmarkName);
    void endBenchmark(const std::string& benchmarkName);
    double getBenchmarkResult(const std::string& benchmarkName) const;
    std::unordered_map<std::string, double> getAllBenchmarkResults() const { return benchmarkResults_; }
    
    // Statistics and analysis
    size_t getEventCount() const { return events_.size(); }
    size_t getMetricCount() const { return historicalMetrics_.size(); }
    double getMonitoringDuration() const;
    
    // Integration with simulation components
    void recordStepComplete(double stepTime, uint64_t step);
    void recordSpikeGenerated(const PerformanceEvent& spikeEvent);
    void recordPlasticityUpdate(double updateTime, uint64_t step);
    void recordMemoryAllocation(size_t size, const std::string& context);
    void recordMemoryDeallocation(size_t size, const std::string& context);
    void recordNetworkStatistics(const PerformanceMetrics& metrics);
    void recordLearningEvent(const PerformanceEvent& learningEvent);
    void recordDevelopmentEvent(const PerformanceEvent& devEvent);
    
    // Python bindings integration
    void integrateWithPython() {
        enablePythonIntegration_ = true;
        // Integration with Python bindings for real-time monitoring
    }
    
    // External API for monitoring
    void notifyStepComplete(double stepTime, uint64_t step);
    void notifySpikeEvent(double timestamp, uint64_t step, uint64_t source, uint64_t destination);
    void notifyPlasticityUpdate(double weightChange, uint64_t step);
    void notifyMemoryOperation(size_t size, bool allocation, const std::string& context);
    void notifyNetworkChange();
    
    // Performance profiling
    void startProfiling(const std::string& profileName);
    void endProfiling(const std::string& profileName);
    void recordProfileData(const std::string& profileName, const std::string& key, double value);
    std::unordered_map<std::string, std::unordered_map<std::string, double>> getProfileData() const { return profileData_; }
    
private:
    PerformanceMonitorConfig config_;
    PerformanceMetrics currentMetrics_;
    std::vector<PerformanceMetrics> historicalMetrics_;
    std::vector<PerformanceEvent> events_;
    
    // Internal state
    bool realTimeMonitoring_;
    std::chrono::high_resolution_clock::time_point lastEventTime_;
    double accumulatedTime_;
    uint64_t stepsSinceLastMetricsUpdate_;
    
    // Benchmarking
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> benchmarkTimers_;
    std::unordered_map<std::string, double> benchmarkResults_;
    
    // Performance alerts
    std::vector<std::string> alerts_;
    
    // Profiling
    std::unordered_map<std::string, std::unordered_map<std::string, double>> profileData_;
    
    // Python integration
    bool enablePythonIntegration_;
    
    // Thread safety
    mutable std::mutex eventsMutex_;
    mutable std::mutex metricsMutex_;
    
    // Internal methods
    void updatePerformanceAlerts();
    void collectSystemMetrics();
    void processRealTimeOutput();
    void generatePerformanceWarnings();
    void updateBenchmarkResults();
    
    // Data file utilities
    bool writeMetricsToFile(const std::string& filepath, const std::vector<PerformanceMetrics>& metrics) const;
    std::string escapeCSVValue(const std::string& value) const;
    
    // Analysis utilities
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    double calculateStandardDeviation(const std::vector<double>& values) const;
    double calculateMean(const std::vector<double>& values) const;
    
    // Report generation helpers
    std::string generateHeaderSection() const;
    std::string generateMetricsSection() const;
    std::string generateEventSection() const;
    std::string generateBenchmarkSection() const;
    std::string generateAlertSection() const;
    std::string generateProfileSection() const;
};

} // namespace nlm