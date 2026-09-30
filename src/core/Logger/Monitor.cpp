#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <ctime>
#include <numeric>

namespace nlm {

// System resource metrics
struct SystemResourceMetrics {
    double cpuUsage;  // percentage
    double memoryUsage;  // percentage
    double memoryAvailable;  // in bytes
    double memoryTotal;  // in bytes
    double diskUsage;  // percentage for primary disk
    double diskAvailable;  // in bytes for primary disk
    double networkSent;  // bytes per second
    double networkReceived;  // bytes per second
    std::chrono::system_clock::time_point timestamp;
    std::string hostname;
    std::string osVersion;
};

// Simulation-specific metrics
struct SimulationMetrics {
    std::string simulationId;
    int stepNumber;
    double simulationTime;  // in seconds
    int totalNeurons;
    int activeNeurons;
    int totalSynapses;
    double synapticStrength;  // average
    double networkActivity;  // 0-1 scale
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, double> customMetrics;
    std::vector<double> neuronActivities;
    std::vector<double> synapticWeights;
};

// Performance metrics aggregation
struct PerformanceMetrics {
    std::string metricName;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> tags;
    double trend;  // percentage change over time
    double average;  // average value over time window
    double min;  // minimum value
    double max;  // maximum value
    size_t count;  // number of samples
};

// Real-time monitoring system
class RealTimeMonitor {
public:
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    // Start/stop monitoring
    void startMonitoring(double sampleInterval = 1.0);  // in seconds
    void stopMonitoring();
    bool isMonitoring() const;
    
    // System resource monitoring
    void addSystemResourceMonitor(const std::function<SystemResourceMetrics()>& monitor);
    SystemResourceMetrics getCurrentSystemResources() const;
    std::vector<SystemResourceMetrics> getSystemResourceHistory(size_t limit = 1000) const;
    
    // Simulation monitoring
    void addSimulationMonitor(const std::function<SimulationMetrics()>& monitor);
    SimulationMetrics getCurrentSimulationMetrics(const std::string& simulationId = "") const;
    std::vector<SimulationMetrics> getSimulationHistory(size_t limit = 1000) const;
    
    // Performance metrics monitoring
    void addPerformanceMetric(const PerformanceMetric& metric);
    PerformanceMetrics getCurrentPerformance(const std::string& name) const;
    std::vector<PerformanceMetrics> getPerformanceHistory(const std::string& name, size_t limit = 1000) const;
    
    // Threshold monitoring and alerting
    void addThresholdAlert(const std::string& metricName, double threshold,
                          const std::string& severity = "warning",
                          const std::function<void(const std::string& metric, double value)>& action = nullptr);
    void checkThresholds();
    
    // Performance analysis
    void analyzePerformanceTrends();
    std::vector<std::string> identifyPerformanceDeviations(double deviationThreshold = 0.2) const;
    double calculatePerformanceScore(const std::string& simulationId = "") const;
    
    // Export and reporting
    void exportSystemReport(const std::string& filename) const;
    void exportSimulationReport(const std::string& simulationId, const std::string& filename) const;
    void exportPerformanceReport(const std::string& filename) const;
    
    // Data management
    void clearData();
    void setDataRetention(size_t systemResourceLimit, size_t simulationLimit, size_t performanceLimit);
    
    // Benchmarking
    void startBenchmark(const std::string& benchmarkName, const std::unordered_map<std::string, std::string>& metadata = {});
    void endBenchmark(const std::string& benchmarkName, const std::unordered_map<std::string, std::string>& metadata = {});
    std::vector<std::pair<std::string, double>> getBenchmarkResults() const;
    
    // Visualization data generation
    std::string generateSystemResourceChart(const std::string& title = "System Resources") const;
    std::string generateSimulationChart(const std::string& simulationId, const std::string& title = "Simulation Metrics") const;
    std::string generatePerformanceChart(const std::string& metricName, const std::string& title = "Performance Metrics") const;
    
    // Singleton pattern
    static RealTimeMonitor& getInstance();
    static void setGlobalMonitor(std::shared_ptr<RealTimeMonitor> monitor);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void monitoringThreadFunction();
    SystemResourceMetrics collectSystemResources() const;
    SimulationMetrics collectSimulationMetrics(const std::string& simulationId = "") const;
    PerformanceMetric collectPerformanceMetric(const std::string& name) const;
    std::string formatChartData(const std::vector<SystemResourceMetrics>& data, const std::string& title) const;
    std::string formatSimulationChartData(const std::vector<SimulationMetrics>& data, const std::string& title) const;
    std::string formatPerformanceChartData(const std::vector<PerformanceMetrics>& data, const std::string& title) const;
};

// Health monitoring system
class HealthMonitor {
public:
    HealthMonitor();
    ~HealthMonitor();
    
    // Component health monitoring
    enum class ComponentHealth {
        Healthy = 0,
        Warning = 1,
        Critical = 2,
        Unknown = 3
    };
    
    struct ComponentHealthStatus {
        std::string componentName;
        ComponentHealth health;
        std::chrono::system_clock::time_point lastCheck;
        std::string lastError;
        std::unordered_map<std::string, std::string> details;
    };
    
    // Health check registration
    void registerHealthCheck(const std::string& componentName,
                           const std::function<ComponentHealthStatus()>& checkFunction,
                           double checkInterval = 5.0);  // in seconds
    
    // Health status retrieval
    ComponentHealthStatus getComponentHealth(const std::string& componentName) const;
    std::vector<ComponentHealthStatus> getAllComponentHealth() const;
    std::vector<ComponentHealthStatus> getUnhealthyComponents() const;
    
    // System health assessment
    ComponentHealth getOverallSystemHealth() const;
    std::string assessSystemHealth() const;
    std::vector<std::string> getHealthRecommendations() const;
    
    // Incident management
    void logHealthIncident(const std::string& componentName, ComponentHealth health,
                         const std::string& errorMessage,
                         const std::unordered_map<std::string, std::string>& context = {});
    
    // Export and reporting
    void exportHealthReport(const std::string& filename) const;
    void clearHealthHistory();
    
    // Configuration
    void setHealthCheckTimeout(double timeout);  // in seconds
    void setHealthCheckRetryCount(int retryCount);
    void enableAutomaticHealing(bool enable, const std::function<bool(const std::string& component)>& healAction = nullptr);
    
    // Singleton pattern
    static HealthMonitor& getInstance();
    static void setGlobalHealthMonitor(std::shared_ptr<HealthMonitor> monitor);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void healthCheckThreadFunction();
};

// Resource monitoring system
class ResourceMonitor {
public:
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Resource tracking
    struct ResourceUsage {
        std::string resourceName;
        double currentUsage;
        double peakUsage;
        double averageUsage;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    // Resource monitoring registration
    void addResourceMonitor(const std::string& resourceName,
                           const std::function<ResourceUsage()>& monitor,
                           double sampleInterval = 1.0);
    
    // Resource status retrieval
    ResourceUsage getResourceUsage(const std::string& resourceName) const;
    std::vector<ResourceUsage> getAllResourceUsage() const;
    std::vector<ResourceUsage> getResourceHistory(const std::string& resourceName, size_t limit = 1000) const;
    
    // Resource analysis
    void analyzeResourceUsage();
    std::vector<std::string> identifyResourceLeaks() const;
    double calculateResourceEfficiency(const std::string& resourceName) const;
    
    // Export and reporting
    void exportResourceReport(const std::string& filename) const;
    void clearResourceHistory();
    
    // Configuration
    void setResourceAlertThresholds(const std::string& resourceName, double warningThreshold, double criticalThreshold);
    void enableResourceOptimization(bool enable, const std::function<void(const std::string& resource)> & optimizeAction = nullptr);
    
    // Singleton pattern
    static ResourceMonitor& getInstance();
    static void setGlobalResourceMonitor(std::shared_ptr<ResourceMonitor> monitor);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void resourceMonitoringThreadFunction();
};

// Benchmarking system
class BenchmarkSystem {
public:
    BenchmarkSystem();
    ~BenchmarkSystem();
    
    // Benchmark management
    struct BenchmarkResult {
        std::string benchmarkName;
        double executionTime;  // in seconds
        double cpuTime;  // in seconds
        double memoryUsage;  // in bytes
        int iterations;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, std::string> metadata;
        std::unordered_map<std::string, double> detailedMetrics;
    };
    
    // Benchmark execution
    void recordBenchmarkResult(const BenchmarkResult& result);
    BenchmarkResult runBenchmark(const std::string& name,
                                const std::function<void()>& benchmarkFunction,
                                const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Benchmark analysis
    std::vector<BenchmarkResult> getBenchmarkHistory(const std::string& name, size_t limit = 100) const;
    std::vector<BenchmarkResult> getAllBenchmarkResults(size_t limit = 1000) const;
    void analyzeBenchmarkTrends();
    std::vector<std::string> identifyBenchmarkAnomalies(double threshold = 0.3) const;
    
    // Export and reporting
    void exportBenchmarkReport(const std::string& filename) const;
    void clearBenchmarkHistory();
    
    // Configuration
    void setBenchmarkTimeout(double timeout);  // in seconds
    void enableBenchmarkValidation(bool enable, double expectedTimeThreshold = 0.0);
    
    // Singleton pattern
    static BenchmarkSystem& getInstance();
    static void setGlobalBenchmarkSystem(std::shared_ptr<BenchmarkSystem> benchmarkSystem);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Integration utilities
class MonitoringIntegration {
public:
    MonitoringIntegration();
    ~MonitoringIntegration();
    
    // Integration with StructuredLogger
    void integrateWithLogger(std::shared_ptr<StructuredLogger> logger);
    void integrateWithDebugUtils(std::shared_ptr<DebugUtils> debugUtils);
    
    // Data synchronization
    void syncDataToLogger();
    void syncDataToDebugUtils();
    
    // Real-time data streaming
    void startDataStream(double streamInterval = 0.1);  // in seconds
    void stopDataStream();
    std::vector<std::string> getDataStream() const;
    
    // Export to multiple formats
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    void exportToHTML(const std::string& filename) const;
    
    // Singleton pattern
    static MonitoringIntegration& getInstance();
    static void setGlobalMonitoringIntegration(std::shared_ptr<MonitoringIntegration> integration);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void dataStreamThreadFunction();
};

// Convenience functions
void updateSystemMetrics();
void updateSimulationMetrics(const std::string& simulationId);
void recordPerformanceMetric(const std::string& name, double value,
                           const std::string& unit = "ms",
                           const std::unordered_map<std::string, std::string>& tags = {});
void logSystemHealth(const SystemResourceMetrics& metrics);
void logSimulationStep(const SimulationMetrics& metrics);

// Configuration functions
void configureMonitoring(const std::unordered_map<std::string, std::string>& options);
std::unordered_map<std::string, std::string> getMonitoringConfiguration();

// Utility functions
std::string formatResourceUsage(const ResourceMonitor::ResourceUsage& usage);
std::string formatPerformanceData(const PerformanceMetrics& metric);
std::string formatHealthStatus(const HealthMonitor::ComponentHealthStatus& status);

} // namespace nlm