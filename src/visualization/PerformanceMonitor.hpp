#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "core/Types/Types.hpp"
#include "visualization/VisualizationInterface.hpp"

namespace nlm {

// System resource usage information
struct ResourceUsage {
    uint64_t memoryUsage;           // Memory usage in bytes
    uint64_t memoryAllocated;       // Memory allocated in bytes
    uint64_t memoryPeak;            // Peak memory usage
    float cpuUsage;                 // CPU usage percentage (0.0-100.0)
    float cpuUser;                  // User CPU time percentage
    float cpuSystem;                // System CPU time percentage
    size_t threadCount;             // Number of active threads
    size_t openFileCount;           // Number of open file handles
    std::chrono::microseconds lastSampleTime;  // Time of last sample
    
    ResourceUsage() : memoryUsage(0), memoryAllocated(0), memoryPeak(0),
                     cpuUsage(0.0f), cpuUser(0.0f), cpuSystem(0.0f),
                     threadCount(0), openFileCount(0) {
        lastSampleTime = std::chrono::high_resolution_clock::now();
    }
};

// Performance metrics for system components
struct ComponentMetrics {
    std::string name;
    SimulationStep step;
    std::chrono::microseconds updateTime;    // Time spent in update
    std::chrono::microseconds renderTime;   // Time spent in render
    std::chrono::microseconds totalTime;    // Total time
    size_t memoryUsage;                     // Memory used by component
    uint32_t drawCalls;                     // Draw calls issued
    uint32_t vertexCount;                   // Vertices rendered
    float cpuUtilization;                   // CPU utilization
    float gpuUtilization;                    // GPU utilization
    std::map<std::string, double> customMetrics;  // Custom metrics
    uint32_t errorCount;                     // Number of errors
    uint32_t warningCount;                   // Number of warnings
    
    ComponentMetrics(const std::string& componentName)
        : name(componentName), step(0), updateTime(0), renderTime(0), totalTime(0),
          memoryUsage(0), drawCalls(0), vertexCount(0), cpuUtilization(0.0f),
          gpuUtilization(0.0f), errorCount(0), warningCount(0) {}
};

// System-wide performance summary
struct SystemPerformanceSummary {
    SimulationStep currentStep;
    Timestamp currentTime;
    std::chrono::system_clock::time_point sessionStartTime;
    double sessionDuration;                 // Session duration in seconds
    
    // Resource usage
    ResourceUsage currentResources;
    ResourceUsage peakResources;
    float averageCpuUsage;                   // Average CPU usage
    float averageMemoryUsage;                // Average memory usage
    
    // Performance metrics
    uint64_t totalUpdates;                   // Total update calls
    uint64_t totalRenders;                   // Total render calls
    double averageUpdateTime;                // Average update time
    double averageRenderTime;                // Average render time
    double averageTotalTime;                 // Average total time
    double targetFPS;                        // Target frames per second
    double actualFPS;                        // Actual frames per second
    
    // System statistics
    uint64_t totalSpikes;                    // Total spikes generated
    float averageFiringRate;                 // Average firing rate
    uint32_t activeNeurons;                  // Number of active neurons
    uint32_t totalNeurons;                   // Total number of neurons
    uint32_t activeConnections;              // Number of active connections
    uint32_t totalConnections;               // Total number of connections
    
    // Bottleneck analysis
    std::vector<std::string> identifiedBottlenecks;
    std::map<std::string, double> bottleneckScores;  // Bottleneck severity (0.0-1.0)
    
    // Resource trends
    std::vector<float> memoryTrend;          // Memory usage trend
    std::vector<float> cpuTrend;             // CPU usage trend
    std::vector<double> fpsTrend;            // FPS trend
    
    SystemPerformanceSummary() : currentStep(0), currentTime(0.0), sessionDuration(0.0),
                                 averageCpuUsage(0.0f), averageMemoryUsage(0.0f),
                                 totalUpdates(0), totalRenders(0), averageUpdateTime(0.0),
                                 averageRenderTime(0.0), averageTotalTime(0.0), targetFPS(60.0),
                                 actualFPS(0.0), totalSpikes(0), averageFiringRate(0.0f),
                                 activeNeurons(0), totalNeurons(0), activeConnections(0),
                                 totalConnections(0), sessionStartTime(std::chrono::system_clock::now()) {}
};

// Performance alert
struct PerformanceAlert {
    std::string id;
    std::string message;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::string severity;  // "INFO", "WARNING", "ERROR", "CRITICAL"
    std::string source;    // Component or system that generated the alert
    std::map<std::string, std::string> context;  // Additional context
    bool isActive;         // Whether alert is still active
    std::chrono::system_clock::time_point firstOccurrence;
    uint32_t occurrenceCount;  // How many times this alert has occurred
    
    PerformanceAlert(const std::string& alertId, const std::string& alertMsg,
                     const std::string& alertDesc, const std::string& alertSource,
                     const std::string& alertSeverity = "WARNING")
        : id(alertId), message(alertMsg), description(alertDesc), timestamp(std::chrono::system_clock::now()),
          source(alertSource), severity(alertSeverity), isActive(true),
          firstOccurrence(std::chrono::system_clock::now()), occurrenceCount(1) {}
};

// Performance monitoring configuration
struct PerformanceMonitorConfig {
    bool enabled;                    // Whether performance monitoring is enabled
    bool realTimeEnabled;            // Whether real-time monitoring is enabled
    double monitoringInterval;       // Interval between performance samples (seconds)
    size_t historySize;              // Size of performance history buffer
    bool enableAlerts;               // Whether to generate alerts for performance issues
    float cpuThreshold;              // CPU usage threshold for alerts (percentage)
    float memoryThreshold;           // Memory usage threshold for alerts (bytes)
    float fpsThreshold;              // FPS threshold for alerts (frames per second)
    float updateTimeThreshold;       // Update time threshold for alerts (microseconds)
    float renderTimeThreshold;       // Render time threshold for alerts (microseconds)
    bool trackResourceLeaks;         // Whether to track resource leaks
    bool analyzeBottlenecks;         // Whether to analyze performance bottlenecks
    bool generateReports;            // Whether to generate performance reports
    std::string reportFormat;        // Report format ("text", "json", "xml")
    std::string alertFormat;         // Alert format
    bool exportToSystemMonitor;     // Whether to export to system monitoring tools
    bool enableVisualization;        // Whether to enable visualization of performance data
    
    PerformanceMonitorConfig() : enabled(true), realTimeEnabled(true), monitoringInterval(0.1),
                                 historySize(3600), enableAlerts(true), cpuThreshold(90.0f),
                                 memoryThreshold(1024 * 1024 * 1024),  // 1GB
                                 fpsThreshold(30.0), updateTimeThreshold(5000.0),   // 5ms
                                 renderTimeThreshold(8000.0),  // 8ms
                                 trackResourceLeaks(true), analyzeBottlenecks(true),
                                 generateReports(true), reportFormat("text"),
                                 alertFormat("text"), exportToSystemMonitor(false),
                                 enableVisualization(true) {}
};

// Performance monitor class
class PerformanceMonitor : public VisualizationInterface {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // VisualizationInterface interface
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Performance monitoring methods
    void recordPerformanceSample(const PerformanceSample& sample);
    void recordComponentMetrics(const ComponentMetrics& metrics);
    void updateResourceUsage(const ResourceUsage& usage);
    
    // System-wide statistics
    SystemPerformanceSummary getSystemPerformanceSummary() const;
    ComponentMetrics getComponentMetrics(const std::string& name) const;
    std::vector<ComponentMetrics> getAllComponentMetrics() const;
    
    // Performance analysis
    void computePerformanceStatistics();
    std::string analyzePerformanceTrends() const;
    std::string detectPerformanceBottlenecks() const;
    std::string identifyResourceIssues() const;
    
    // Alert management
    void addAlert(const PerformanceAlert& alert);
    void removeAlert(const std::string& id);
    void acknowledgeAlert(const std::string& id);
    void clearAlerts();
    std::vector<PerformanceAlert> getAlerts() const;
    std::vector<PerformanceAlert> getActiveAlerts() const;
    std::vector<PerformanceAlert> getAlertsBySeverity(const std::string& severity) const;
    
    // Configuration and control
    void setConfig(const PerformanceMonitorConfig& config);
    void toggleRealTimeMonitoring(bool enabled);
    void toggleVisualization(bool enabled);
    
    // Reporting and analysis
    std::string generatePerformanceReport() const;
    void exportPerformanceData(const std::string& filename, const std::string& format = "json");
    void savePerformanceSnapshot(const std::string& filename);
    
    // Resource monitoring
    void monitorMemoryUsage();
    void monitorCpuUsage();
    void monitorFileHandles();
    void monitorThreads();
    
    // Performance regression testing
    void setPerformanceBaseline(const std::string& baselineName);
    std::string compareWithBaseline(const std::string& baselineName) const;
    bool checkPerformanceRegression(const std::string& baselineName, 
                                   const std::string& regressionThreshold = "10%") const;
    
    // Visualization and monitoring
    void updateVisualization(const VisualizationData& data);
    void updateRealTimeVisualization();
    void renderPerformanceCharts();
    void renderResourceUsageGraph();
    void renderComponentPerformanceGraph();
    void renderAlertSummary();
    
    // Statistics
    size_t getSampleCount() const;
    size_t getComponentCount() const;
    size_t getAlertCount() const;
    uint64_t getTotalUpdates() const;
    uint64_t getTotalRenders() const;
    double getAverageFPS() const;
    std::chrono::microseconds getAverageUpdateTime() const;
    std::chrono::microseconds getAverageRenderTime() const;
    
    // Status and health
    bool isMonitoring() const;
    bool isRealTimeEnabled() const;
    bool isVisualizationEnabled() const;
    std::string getSystemHealth() const;
    std::chrono::system_clock::time_point getSessionStartTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal methods
    void updateRealTimeMetrics();
    void updatePerformanceAnalysis();
    void updateResourceAnalysis();
    void generateAlerts();
    void cleanupOldData();
    void maintainHistoryBuffers();
    
    void createPerformanceCharts();
    void createResourceUsageChart();
    void createComponentUsageChart();
    void createAlertTimeline();
    
    std::string formatTime(std::chrono::microseconds time) const;
    std::string formatBytes(size_t bytes) const;
    std::string formatPercentage(float percentage) const;
    std::string analyzeMemoryLeaks() const;
    std::string analyzeFileHandleLeaks() const;
    void updateSystemNeurons(const class Brain& brain);
    void updateSystemConnections(const class Brain& brain);
};

} // namespace nlm
