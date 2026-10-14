#pragma once

#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <chrono>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>

namespace nlm {

// Forward declarations
class SystemMonitor;
class NeuralPerformanceTracker;
class LogAggregator;
class StructuredLogOutput;
class PerformanceMetricsLogger;

// Performance metrics for simulation tracking
struct PerformanceMetrics {
    // Timing metrics
    double simulationTime;                // Total simulation time (seconds)
    double stepTime;                      // Average step time (seconds)
    double peakStepTime;                  // Maximum step time (seconds)
    double memoryUsage;                   // Current memory usage (MB)
    double peakMemoryUsage;               // Peak memory usage (MB)
    
    // Neural activity metrics
    double averageFiringRate;             // Average firing rate across all neurons (Hz)
    double peakFiringRate;                // Maximum firing rate observed (Hz)
    uint64_t totalSpikes;                 // Total number of spikes
    uint64_t synapticOperations;           // Total synaptic operations
    
    // Resource metrics
    double cpuUsage;                      // Current CPU usage (percentage)
    double memoryBandwidth;               // Memory bandwidth usage (GB/s)
    uint64_t diskReadOperations;           // Disk read operations
    uint64_t diskWriteOperations;          // Disk write operations
    
    // System health metrics
    bool simulationHealthy;               // Overall simulation health flag
    uint64_t checkpointCount;              // Number of completed checkpoints
    uint64_t completedSteps;               // Steps completed since last checkpoint
    
    PerformanceMetrics() 
        : simulationTime(0.0)
        , stepTime(0.0)
        , peakStepTime(0.0)
        , memoryUsage(0.0)
        , peakMemoryUsage(0.0)
        , averageFiringRate(0.0)
        , peakFiringRate(0.0)
        , totalSpikes(0)
        , synapticOperations(0)
        , cpuUsage(0.0)
        , memoryBandwidth(0.0)
        , diskReadOperations(0)
        , diskWriteOperations(0)
        , simulationHealthy(true)
        , checkpointCount(0)
        , completedSteps(0) {}
};

// Context for correlation and tracking
struct LoggingContext {
    std::string correlationId;            // Unique ID for tracking related events
    std::string componentId;              // Component/system identifier
    std::string simulationId;             // Simulation identifier
    uint64_t stepNumber;                  // Current simulation step
    Timestamp simulationTime;             // Current simulation time
    std::unordered_map<std::string, std::string> customFields; // Custom context data
    
    LoggingContext()
        : stepNumber(0)
        , simulationTime(0.0) {}
    
    std::string getContextKey() const {
        std::ostringstream oss;
        oss << "step:" << stepNumber 
            << ":time:" << simulationTime
            << ":corr:" << correlationId;
        return oss.str();
    }
};

// Performance Metrics Logger for simulation tracking
class PerformanceMetricsLogger : public ILogger {
public:
    PerformanceMetricsLogger();
    explicit PerformanceMetricsLogger(LogLevel level);
    ~PerformanceMetricsLogger() override;
    
    // ILogger interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Performance tracking
    void recordStepStart(const LoggingContext& context);
    void recordStepEnd(const LoggingContext& context);
    void recordSpike(const LoggingContext& context, double firingRate);
    void recordSynapticOperation(const LoggingContext& context);
    void recordCheckpoint(const LoggingContext& context);
    
    // Neural activity tracking
    void recordNeuronFiring(const LoggingContext& context, NeuronId neuronId, 
                           double firingRate, NeuronType type);
    void recordSynapticWeight(const LoggingContext& context, SynapseId synapseId,
                            SynapticWeight oldWeight, SynapticWeight newWeight);
    
    // Resource tracking
    void recordMemoryUsage(const LoggingContext& context, double memoryMB);
    void recordCpuUsage(const LoggingContext& context, double cpuPercentage);
    
    // Performance analysis
    PerformanceMetrics getCurrentMetrics() const;
    std::string generatePerformanceReport() const;
    std::string generateNeuralAnalysisReport() const;
    
    // Context management
    void setCurrentContext(const LoggingContext& context);
    LoggingContext getCurrentContext() const;
    
    // Benchmarking
    void startBenchmark(const std::string& benchmarkName);
    void endBenchmark(const std::string& benchmarkName);
    double getBenchmarkTime(const std::string& benchmarkName) const;
    
    // Export metrics
    std::string exportMetricsJson() const;
    std::string exportMetricsCsv() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Console Logger with advanced features
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    // ILogger interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Advanced filtering
    void setFilter(const std::string& moduleFilter);
    void clearFilter();
    bool shouldLog(const std::string& module, LogLevel level) const;
    
    // Structured output formats
    void setOutputFormat(const std::string& format); // json, csv, human, dashboard
    std::string formatEntry(const LogEntry& entry, const LoggingContext& context) const;
    
    // Log rotation and management
    void setLogRotation(bool enable, size_t maxFileSize = 10 * 1024 * 1024, 
                       size_t maxFiles = 5);
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Progress indicators and dashboards
    void showProgressBar(double progress, const std::string& message = "");
    void updateDashboard(const std::unordered_map<std::string, std::string>& metrics);
    void hideDashboard();
    
    // Colors and formatting
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setTimeFormat(const std::string& format);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// System Monitor for resource tracking
class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();
    
    // Resource monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // CPU and memory monitoring
    double getCpuUsage() const;            // CPU usage percentage
    double getMemoryUsage() const;         // Memory usage percentage
    double getMemoryAvailable() const;     // Available memory (MB)
    double getMemoryTotal() const;         // Total memory (MB)
    
    // Disk I/O monitoring
    uint64_t getDiskReadBytes() const;     // Disk read bytes
    uint64_t getDiskWriteBytes() const;    // Disk write bytes
    double getDiskReadRate() const;        // Disk read rate (MB/s)
    double getDiskWriteRate() const;       // Disk write rate (MB/s)
    
    // Network monitoring
    uint64_t getNetworkBytesSent() const;   // Network bytes sent
    uint64_t getNetworkBytesReceived() const; // Network bytes received
    
    // Performance benchmarking
    void startTimer(const std::string& timerName);
    double getElapsedTime(const std::string& timerName) const;
    void resetTimer(const std::string& timerName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool monitoring_;
    std::chrono::steady_clock::time_point startTime_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> timers_;
};

// Neural Performance Tracker for simulation metrics
class NeuralPerformanceTracker {
public:
    NeuralPerformanceTracker();
    ~NeuralPerformanceTracker();
    
    // Neuron firing patterns
    void recordNeuronFiring(const LoggingContext& context, NeuronId neuronId,
                           double firingRate, NeuronType type, Timestamp spikeTime);
    void recordRefractoryPeriod(const LoggingContext& context, NeuronId neuronId,
                               uint32_t duration);
    
    // Synaptic activity
    void recordSynapticWeightChange(const LoggingContext& context, SynapseId synapseId,
                                   SynapticWeight oldWeight, SynapticWeight newWeight,
                                   double changeMagnitude);
    void recordSynapticActivity(const LoggingContext& context, SynapseId synapseId,
                               double efficacy, uint32_t preSpikeCount, uint32_t postSpikeCount);
    
    // Network dynamics
    void recordPopulationActivity(const LoggingContext& context, PopulationId populationId,
                                 double averageFiringRate, double populationVariance);
    void recordConnectivityMetrics(const LoggingContext& context, double connectionDensity,
                                  double averagePathLength, double clusteringCoefficient);
    
    // Learning progress
    void recordLearningProgress(const LoggingContext& context, double learningRate,
                               double plasticityAmount, double weightChangeVariance);
    
    // Analysis and reporting
    struct NeuralMetrics {
        double averageFiringRate;           // Overall average firing rate
        double peakFiringRate;              // Maximum firing rate
        double meanFiringRateVariance;      // Variance in firing rates
        double averageSynapticWeight;       // Average synaptic weight magnitude
        double averageSynapticChange;       // Average weight change per step
        uint64_t totalSpikes;               // Total spikes recorded
        uint64_t totalSynapticEvents;       // Total synaptic events
        double averageRefractoryPeriod;     // Average refractory period duration
        double connectivityDensity;         // Network connectivity
    };
    
    NeuralMetrics getCurrentNeuralMetrics() const;
    std::string generateNeuralReport() const;
    
    // Export and analysis
    std::string exportNeuralMetricsJson() const;
    std::string exportNeuralMetricsCsv() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log Aggregator for cross-component analysis
class LogAggregator {
public:
    LogAggregator();
    ~LogAggregator() = default;
    
    // Log collection
    void addLogEntry(const LogEntry& entry);
    void addPerformanceMetrics(const PerformanceMetrics& metrics);
    void addNeuralMetrics(const NeuralPerformanceTracker::NeuralMetrics& metrics);
    
    // Analysis and reporting
    struct LogPattern {
        std::string component;
        LogLevel level;
        uint64_t count;
        double percentage;
        Timestamp firstOccurrence;
        Timestamp lastOccurrence;
    };
    
    std::vector<LogPattern> analyzeLogPatterns() const;
    std::string generateLogSummary() const;
    
    // Anomaly detection
    bool detectAnomalies() const;
    std::vector<std::string> getAnomalyReports() const;
    
    // Export
    std::string exportAggregatedLogsJson() const;
    std::string exportAggregatedLogsCsv() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Structured Log Output for machine parsing
class StructuredLogOutput {
public:
    StructuredLogOutput();
    ~StructuredLogOutput() = default;
    
    // Output formats
    void setJsonOutput(std::ostream& output);
    void setCsvOutput(std::ostream& output, const std::vector<std::string>& headers);
    void setHumanReadableOutput(std::ostream& output);
    
    // Logging formats
    void logEntry(const LogEntry& entry, const LoggingContext& context);
    void logPerformanceMetrics(const PerformanceMetrics& metrics, 
                              const LoggingContext& context);
    void logNeuralMetrics(const NeuralPerformanceTracker::NeuralMetrics& metrics,
                         const LoggingContext& context);
    
    // Configuration
    void setIndentation(int spaces);
    void setIncludeTimestamp(bool include);
    void setIncludeContext(bool include);
    
    // File management
    bool openFile(const std::string& filepath);
    void closeFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance Benchmarking utilities
class PerformanceBenchmark {
public:
    PerformanceBenchmark();
    ~PerformanceBenchmark() = default;
    
    // Timer utilities
    void start(const std::string& name);
    double stop(const std::string& name);
    double getDuration(const std::string& name) const;
    
    // Benchmark execution
    void runBenchmark(const std::string& name, 
                     std::function<void()> benchmarkFunction);
    
    // Reporting
    std::string generateBenchmarkReport() const;
    std::string exportBenchmarkDataJson() const;
    
    // Aggregation
    double getAverageTime(const std::string& name) const;
    double getMinTime(const std::string& name) const;
    double getMaxTime(const std::string& name) const;
    uint64_t getIterationCount(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
