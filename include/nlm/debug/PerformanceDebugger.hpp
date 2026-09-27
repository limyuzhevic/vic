// ==============================================================================
// Performance Debugger
// ==============================================================================
#ifndef PERFORMANCEDEBUGGER_HPP
#define PERFORMANCEDEBUGGER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include "../performance/PerformanceProfiler.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"

namespace nlm {
namespace debug {

/**
 * @class PerformanceDebugger
 * @brief Performance debugging tool for NLM
 * 
 * Provides tools for performance debugging including bottleneck identification,
 * performance analysis, and optimization recommendations.
 */
class PerformanceDebugger {
public:
    /**
     * @enum PerformanceIssueType
     * @brief Type of performance issue
     */
    enum class PerformanceIssueType {
        CPU_BOTTLECK,
        MEMORY_BOTTLECK,
        SYNAPTIC_BOTTLECK,
        NEURAL_ACTIVITY_BOTTLECK,
        MEMORY_ACCESS_BOTTLECK,
        NEUROMODULATION_BOTTLECK,
        IO_BOTTLECK,
        ALGORITHM_BOTTLECK,
        THREAD_BOTTLECK,
        CACHE_BOTTLECK
    };

    /**
     * @enum OptimizationLevel
     * @brief Optimization level
     */
    enum class OptimizationLevel {
        BASIC,
        INTERMEDIATE,
        ADVANCED,
        AGGRESSIVE
    };

    /**
     * @struct PerformanceIssue
     * @brief Performance issue
     */
    struct PerformanceIssue {
        PerformanceIssueType type;
        std::string description;
        double severity;
        uint64_t impact;
        std::chrono::steady_clock::time_point detectedTime;
        std::vector<std::string> suggestions;
        double estimatedPerformanceGain;
        std::map<std::string, double> metrics;
    };

    /**
     * @struct PerformanceProfile
     * @brief Performance profile
     */
    struct PerformanceProfile {
        std::chrono::steady_clock::time_point timestamp;
        double cpuUsage;
        double memoryUsage;
        double neuralActivity;
        double synapticActivity;
        double memoryAccess;
        double neuromodulationActivity;
        double throughput;
        double latency;
        double efficiency;
    };

    PerformanceDebugger();
    ~PerformanceDebugger();

    /**
     * @brief Start performance debugging
     * @param profiler Performance profiler reference
     */
    void startDebugging(PerformanceProfiler& profiler);

    /**
     * @brief Stop performance debugging
     */
    void stopDebugging();

    /**
     * @brief Analyze performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     */
    void analyze(const Brain& brain, const NeuralWorkingMemory& memory);

    /**
     * @brief Get performance issues
     * @return Vector of performance issues
     */
    std::vector<PerformanceIssue> getPerformanceIssues() const;

    /**
     * @brief Get performance profile
     * @return Performance profile
     */
    PerformanceProfile getPerformanceProfile() const;

    /**
     * @brief Get performance trends
     * @param timeWindow Time window in seconds
     * @return Performance trends
     */
    std::vector<PerformanceProfile> getPerformanceTrends(double timeWindow = 60.0) const;

    /**
     * @brief Identify performance bottlenecks
     * @return Vector of performance issues
     */
    std::vector<PerformanceIssue> identifyBottlenecks();

    /**
     * @brief Get optimization recommendations
     * @param level Optimization level
     * @return Optimization recommendations
     */
    std::vector<std::string> getOptimizationRecommendations(OptimizationLevel level = OptimizationLevel::INTERMEDIATE);

    /**
     * @brief Get performance statistics
     * @return Performance statistics
     */
    struct PerformanceStatistics {
        double averageCPUUsage;
        double averageMemoryUsage;
        double averageNeuralActivity;
        double averageSynapticActivity;
        double averageMemoryAccess;
        double averageNeuromodulationActivity;
        double averageThroughput;
        double averageLatency;
        double averageEfficiency;
        double performanceTrend;
        double stabilityScore;
        uint64_t samplesCollected;
    };

    PerformanceStatistics getPerformanceStatistics() const;

    /**
     * @brief Get performance alerts
     * @return Vector of performance alerts
     */
    std::vector<std::string> getPerformanceAlerts() const;

    /**
     * @brief Generate performance report
     * @param filename Output file path
     * @return true if report was generated
     */
    bool generatePerformanceReport(const std::string& filename);

    /**
     * @brief Export performance data
     * @param filename Output file path
     * @return true if export was successful
     */
    bool exportPerformanceData(const std::string& filename);

    /**
     * @brief Import performance data
     * @param filename Input file path
     * @return true if import was successful
     */
    bool importPerformanceData(const std::string& filename);

    /**
     * @brief Clear performance data
     */
    void clearPerformanceData();

    /**
     * @brief Set performance threshold
     * @param metric Metric type
     * @param threshold Threshold value
     */
    void setPerformanceThreshold(const std::string& metric, double threshold);

    /**
     * @brief Get performance threshold
     * @param metric Metric type
     * @return Threshold value
     */
    double getPerformanceThreshold(const std::string& metric) const;

    /**
     * @brief Start real-time monitoring
     */
    void startRealTimeMonitoring();

    /**
     * @brief Stop real-time monitoring
     */
    void stopRealTimeMonitoring();

    /**
     * @brief Is real-time monitoring active?
     * @return true if real-time monitoring is active
     */
    bool isRealTimeMonitoringActive() const;

    /**
     * @brief Generate performance debugging visualization
     * @param filename Output file path
     */
    void generatePerformanceDebuggingVisualization(const std::string& filename);

    /**
     * @brief Generate bottleneck visualization
     * @param filename Output file path
     */
    void generateBottleneckVisualization(const std::string& filename);

    /**
     * @brief Generate optimization visualization
     * @param filename Output file path
     */
    void generateOptimizationVisualization(const std::string& filename);

    /**
     * @brief Get available metrics
     * @return Vector of metric names
     */
    std::vector<std::string> getAvailableMetrics() const;

    /**
     * @brief Get optimization levels
     * @return Vector of optimization levels
     */
    std::vector<OptimizationLevel> getAvailableOptimizationLevels() const;

protected:
    /**
     * @brief Update performance data
     */
    void updatePerformanceData();

    /**
     * @brief Analyze performance data
     */
    void analyzePerformanceData();

    /**
     * @brief Identify bottlenecks
     */
    void identifyBottlenecksInternal();

    /**
     * @brief Generate performance report
     */
    void generatePerformanceReportInternal();

    /**
     * @brief Create visualization
     * @param filename Output file path
     */
    void createVisualization(const std::string& filename);

    /**
     * @brief Create bottleneck visualization
     * @param filename Output file path
     */
    void createBottleneckVisualization(const std::string& filename);

    /**
     * @brief Create optimization visualization
     * @param filename Output file path
     */
    void createOptimizationVisualization(const std::string& filename);

    /**
     * @brief Calculate performance score
     * @return Performance score
     */
    double calculatePerformanceScore() const;

    /**
     * @brief Calculate efficiency score
     * @return Efficiency score
     */
    double calculateEfficiencyScore() const;

    /**
     * @brief Calculate stability score
     * @return Stability score
     */
    double calculateStabilityScore() const;

    /**
     * @brief Generate alert
     * @param message Alert message
     */
    void generateAlert(const std::string& message);

    /**
     * @brief Check thresholds
     */
    void checkThresholds();

    /**
     * @brief Generate optimization recommendations
     */
    void generateOptimizationRecommendationsInternal();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // PERFORMANCEDEBUGGER_HPP
