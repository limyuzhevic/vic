#pragma once

/**
 * Performance Monitoring and Profiling System for NLM Framework
 * 
 * This system provides comprehensive performance monitoring capabilities:
 * - Real-time performance metrics collection
 * - Multi-level profiling (call stack, memory, event-driven)
 * - Performance bottleneck detection and hot path identification
 * - Flame graph generation and performance analysis
 * - Integration with existing simulation timing
 * - Configurable monitoring levels and output formats
 * - Performance impact assessment and optimization recommendations
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <thread>
#include <optional>
#include <functional>
#include <map>
#include <variant>
#include <span>
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

/**
 * Performance monitoring levels
 */
enum class MonitoringLevel {
    Silent = 0,      // No performance monitoring
    Basic = 1,        // Essential metrics only
    Detailed = 2,     // Detailed profiling
    Comprehensive = 3  // Full profiling with analysis
};

/**
 * Performance categories for monitoring
 */
enum class PerformanceCategory {
    NeuralComputation = 0,    // Neuron step time, spike propagation, synaptic updates
    MemoryManagement = 1,      // Allocation, deallocation, memory fragmentation
    Connectivity = 2,         // Synapse creation, plasticity updates, region management
    Integration = 3,           // Inter-system communication, state synchronization
    Development = 4,          // Developmental changes, adaptation, plasticity
    Overall = 5              // Total simulation performance, efficiency metrics
};

/**
 * Performance threshold alert types
 */
enum class AlertType {
    Critical = 0,      // Immediate action required
    Warning = 1,       // Attention needed
    Info = 2          // Informational
};

/**
 * Output format types
 */
enum class OutputFormat {
    Text = 0,
    JSON = 1,
    CSV = 2,
    HTML = 3
};

/**
 * Performance metric data structure
 */
struct PerformanceMetric {
    std::string name;
    PerformanceCategory category;
    double value;
    double baseline;  // Reference value for comparison
    std::chrono::microseconds timestamp;
    std::string unit;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * Performance alert structure
 */
struct PerformanceAlert {
    std::string id;
    AlertType type;
    PerformanceCategory category;
    std::string message;
    double currentValue;
    double thresholdValue;
    std::chrono::microseconds timestamp;
    bool acknowledged;
    std::string recommendation;
};

/**
 * Flame graph entry for profiling
 */
struct FlameGraphEntry {
    std::string functionName;
    std::string filePath;
    int lineNumber;
    uint64_t samples;
    double percentage;
    std::vector<std::unique_ptr<FlameGraphEntry>> children;
};

/**
 * Performance profile snapshot
 */
struct PerformanceProfile {
    std::chrono::microseconds timestamp;
    uint64_t stepNumber;
    double simulationTime;
    double realTime;
    std::vector<PerformanceMetric> metrics;
    std::vector<PerformanceAlert> alerts;
    std::unique_ptr<FlameGraphEntry> flameGraph;
    std::unordered_map<std::string, uint64_t> callStackCounts;
    std::unordered_map<std::string, uint64_t> memoryAllocationCounts;
    std::unordered_map<std::string, uint64_t> eventCounts;
};

/**
 * Performance configuration
 */
struct PerformanceConfig {
    MonitoringLevel monitoringLevel;
    bool enableRealTimeMonitoring;
    bool enableCallStackProfiling;
    bool enableMemoryProfiling;
    bool enableEventProfiling;
    bool enableBottleneckDetection;
    double alertThresholdMultiplier;
    size_t maxProfileHistory;
    std::chrono::milliseconds updateInterval;
    std::vector<std::string> monitoredFunctions;
    std::vector<PerformanceCategory> monitoredCategories;
    OutputFormat outputFormat;
    std::string outputFile;
    bool generateFlameGraphs;
    bool enableRegressionDetection;
    std::chrono::milliseconds regressionDetectionInterval;
    std::string baselineFile;
    std::unordered_map<std::string, double> customThresholds;
    bool enableOptimizationRecommendations;
    bool enablePerformanceDashboard;
};

/**
 * Core performance monitoring infrastructure
 */
class PerformanceMonitor {
public:
    PerformanceMonitor(std::shared_ptr<Config> config);
    ~PerformanceMonitor();
    
    // Initialize performance monitoring
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Update monitoring data
    void updateMetrics();
    void recordMetric(const std::string& name, PerformanceCategory category, 
                     double value, const std::string& unit = "");
    void recordMemoryUsage(size_t bytes, size_t allocated, size_t deallocated);
    void recordCPUTime(std::chrono::microseconds time);
    void recordIOOperation(size_t bytes, std::chrono::microseconds duration);
    
    // Profiling
    void startProfile(const std::string& functionName, const std::string& filePath = "", 
                     int lineNumber = 0);
    void stopProfile(const std::string& functionName);
    void recordAllocation(const std::string& type, size_t size);
    void recordDeallocation(const std::string& type, size_t size);
    void recordEvent(const std::string& eventName, uint64_t count = 1);
    
    // Performance profile management
    std::vector<PerformanceProfile> getProfileHistory() const;
    PerformanceProfile getCurrentProfile() const;
    void addProfileToHistory();
    void clearProfileHistory();
    
    // Alert management
    std::vector<PerformanceAlert> getAlerts() const;
    void acknowledgeAlert(const std::string& alertId);
    void checkThresholds();
    void addAlert(const std::string& id, AlertType type, PerformanceCategory category,
                 const std::string& message, double currentValue, double threshold,
                 const std::string& recommendation = "");
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
    // Performance analysis
    std::vector<std::string> analyzePerformance() const;
    std::vector<std::string> detectRegression() const;
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Baseline management
    bool establishBaseline(const std::string& baselineName);
    bool compareToBaseline(const std::string& baselineName);
    double getPerformanceScore() const;
    
    // Export/import
    bool exportPerformanceData(const std::string& filename, OutputFormat format = OutputFormat::JSON) const;
    bool importPerformanceData(const std::string& filename);
    
    // Flame graph generation
    bool generateFlameGraph(const std::string& filename) const;
    bool generateDetailedReport(const std::string& filename) const;
    
    // Statistics and reporting
    std::string getStatistics() const;
    std::string getPerformanceSummary() const;
    uint64_t getTotalSteps() const;
    std::chrono::microseconds getTotalSimulationTime() const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    
    // Singleton pattern
    static PerformanceMonitor& getInstance();
    static void setInstance(std::shared_ptr<PerformanceMonitor> monitor);
    static bool isInitialized();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor for singleton
    PerformanceMonitor();
    PerformanceMonitor(std::shared_ptr<Config> config, bool initialized);
};

/**
 * Advanced profiling system for detailed performance analysis
 */
class Profiler {
public:
    Profiler(std::shared_ptr<Config> config);
    ~Profiler();
    
    // Profile configuration
    void setMonitoringLevel(MonitoringLevel level);
    MonitoringLevel getMonitoringLevel() const;
    
    // Call stack profiling
    void startCallStackProfiling();
    void stopCallStackProfiling();
    void recordCallStack(const std::vector<std::string>& stack);
    std::unordered_map<std::string, uint64_t> getCallStackCounts() const;
    bool generateCallStackReport(const std::string& filename) const;
    
    // Memory allocation profiling
    void startMemoryProfiling();
    void stopMemoryProfiling();
    void recordAllocation(void* address, size_t size, const std::string& type);
    void recordDeallocation(void* address);
    std::unordered_map<std::string, uint64_t> getAllocationCounts() const;
    std::unordered_map<std::string, size_t> getAllocationSizes() const;
    bool generateMemoryReport(const std::string& filename) const;
    
    // Event-driven profiling
    void startEventProfiling();
    void stopEventProfiling();
    void recordEvent(const std::string& eventName, uint64_t timestamp, 
                    const std::unordered_map<std::string, std::string>& metadata = {});
    std::vector<std::pair<std::string, uint64_t>> getEventTimeline() const;
    bool generateEventReport(const std::string& filename) const;
    
    // Performance bottleneck detection
    void updateBottleneckData();
    std::vector<std::string> detectBottlenecks() const;
    std::vector<std::string> identifyHotPaths() const;
    bool generateBottleneckReport(const std::string& filename) const;
    
    // Flame graph generation
    void updateFlameGraphData(const std::string& functionName, const std::string& filePath,
                           int lineNumber, uint64_t samples = 1);
    bool generateFlameGraph(const std::string& filename) const;
    std::unique_ptr<FlameGraphEntry> getFlameGraph() const;
    
    // Profile history
    std::vector<PerformanceProfile> getProfileHistory() const;
    void addProfileToHistory();
    
    // Performance comparison
    bool establishBaseline(const std::string& baselineName);
    bool compareToBaseline(const std::string& baselineName, const std::string& outputFile = "") const;
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance metrics collection system
 */
class PerformanceMetrics {
public:
    PerformanceMetrics();
    ~PerformanceMetrics();
    
    // Metric collection
    void recordMetric(const std::string& name, PerformanceCategory category,
                     double value, const std::string& unit = "",
                     const std::unordered_map<std::string, std::string>& metadata = {});
    double getMetric(const std::string& name) const;
    std::vector<PerformanceMetric> getMetrics(PerformanceCategory category) const;
    std::vector<PerformanceMetric> getAllMetrics() const;
    
    // Time measurement
    void startTimer(const std::string& timerName);
    std::chrono::microseconds stopTimer(const std::string& timerName);
    std::chrono::microseconds getTimerDuration(const std::string& timerName) const;
    std::unordered_map<std::string, std::chrono::microseconds> getAllTimers() const;
    
    // Memory measurement
    void recordMemorySnapshot();
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    std::vector<size_t> getMemoryHistory() const;
    
    // CPU and I/O measurement
    void recordCPUSample(double usage);
    void recordIOSample(size_t bytes, std::chrono::microseconds duration);
    double getAverageCPUUsage() const;
    std::vector<std::pair<size_t, std::chrono::microseconds>> getIOHistory() const;
    
    // Statistics
    size_t getMetricCount() const;
    size_t getTimerCount() const;
    std::chrono::microseconds getTotalMeasurementTime() const;
    
    // Export/import
    bool exportMetrics(const std::string& filename, OutputFormat format = OutputFormat::JSON) const;
    bool importMetrics(const std::string& filename);
    
    // Clear/reset
    void clearMetrics();
    void resetTimers();
    void clearMemoryHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance analysis and recommendation system
 */
class PerformanceAnalysis {
public:
    PerformanceAnalysis(std::shared_ptr<PerformanceMetrics> metrics);
    ~PerformanceAnalysis();
    
    // Performance analysis
    std::vector<std::string> analyzePerformance() const;
    std::vector<std::string> detectAnomalies() const;
    std::vector<std::string> detectRegression(const std::string& baselineName) const;
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Bottleneck analysis
    std::vector<std::string> identifyBottlenecks() const;
    std::vector<std::string> identifyHotPaths() const;
    std::string getBottleneckDetails() const;
    std::string getHotPathDetails() const;
    
    // Performance scoring
    double calculatePerformanceScore() const;
    std::unordered_map<std::string, double> getCategoryScores() const;
    std::string getOverallPerformanceGrade() const;
    
    // Comparison analysis
    bool establishBaseline(const std::string& baselineName);
    bool compareToBaseline(const std::string& baselineName) const;
    std::string getComparisonReport() const;
    
    // Recommendations
    std::vector<std::string> generateRecommendations(const std::string& problem) const;
    std::string getRecommendationDetails(const std::string& recommendationId) const;
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Real-time performance dashboard
 */
class PerformanceDashboard {
public:
    PerformanceDashboard(std::shared_ptr<PerformanceMonitor> monitor,
                        std::shared_ptr<PerformanceMetrics> metrics);
    ~PerformanceDashboard();
    
    // Dashboard update
    void update();
    void startDashboard();
    void stopDashboard();
    bool isDashboardRunning() const;
    
    // Display
    std::string getDashboardDisplay() const;
    void display() const;
    bool exportDashboard(const std::string& filename, OutputFormat format = OutputFormat::HTML) const;
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
    // Interactive features
    void setRefreshInterval(std::chrono::milliseconds interval);
    std::chrono::milliseconds getRefreshInterval() const;
    void setMaxDisplayItems(size_t count);
    size_t getMaxDisplayItems() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance reporting system
 */
class PerformanceReporter {
public:
    PerformanceReporter(std::shared_ptr<PerformanceMonitor> monitor,
                        std::shared_ptr<PerformanceMetrics> metrics,
                        std::shared_ptr<PerformanceAnalysis> analysis);
    ~PerformanceReporter();
    
    // Report generation
    bool generateReport(const std::string& filename, OutputFormat format = OutputFormat::JSON,
                       const std::string& baseline = "") const;
    bool generateSummaryReport(const std::string& filename, OutputFormat format = OutputFormat::Text) const;
    bool generateDetailedReport(const std::string& filename, OutputFormat format = OutputFormat::HTML) const;
    bool generateComparisonReport(const std::string& filename, const std::string& baseline,
                                 OutputFormat format = OutputFormat::Text) const;
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
    // Report configuration
    void setReportTitle(const std::string& title);
    void setReportDescription(const std::string& description);
    void setIncludePerformanceMetrics(bool include);
    void setIncludePerformanceProfile(bool include);
    void setIncludeAlerts(bool include);
    void setIncludeRecommendations(bool include);
    void setIncludeComparison(bool include);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

// Convenience macros for performance monitoring
#define NLM_PERF_RECORD(name, category, value, unit) \
    nlm::PerformanceMonitor::getInstance().recordMetric(name, category, value, unit)

#define NLM_PERF_START_TIMER(name) \
    nlm::PerformanceMetrics::getInstance().startTimer(name)

#define NLM_PERF_STOP_TIMER(name) \
    nlm::PerformanceMetrics::getInstance().stopTimer(name)

#define NLM_PERF_START_PROFILE(name, file, line) \
    nlm::PerformanceMonitor::getInstance().startProfile(name, file, line)

#define NLM_PERF_STOP_PROFILE(name) \
    nlm::PerformanceMonitor::getInstance().stopProfile(name)

#define NLM_PERF_RECORD_ALLOCATION(type, size) \
    nlm::PerformanceMonitor::getInstance().recordAllocation(type, size)

#define NLM_PERF_RECORD_DEALLOCATION(type, size) \
    nlm::PerformanceMonitor::getInstance().recordDeallocation(type, size)

#define NLM_PERF_RECORD_EVENT(event, count) \
    nlm::PerformanceMonitor::getInstance().recordEvent(event, count)

// Global performance metrics instance for convenience
nlm::PerformanceMetrics& getGlobalPerformanceMetrics();
nlm::PerformanceMonitor& getGlobalPerformanceMonitor();