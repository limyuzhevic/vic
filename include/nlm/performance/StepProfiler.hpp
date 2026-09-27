#pragma once

/**
 * StepProfiler - Step-by-step performance analysis
 * 
 * Provides detailed timing analysis of simulation steps, bottleneck
 * identification, and performance trend analysis for neural simulations.
 * 
 * Features:
 * - High-precision step timing measurements
 * - Step-by-step performance breakdown
 * - Bottleneck identification and reporting
 * - Performance trend analysis
 * - Comparison with baseline performance
 * - Automatic performance regression detection
 * 
 * Biological Motivation:
 * Neural systems optimize their processing through feedback control
 * and adaptive plasticity. Step profiling provides similar insights
 * for computational performance optimization.
 */

#include <chrono>
#include <memory>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include <deque>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * Step timing record
 */
struct StepRecord {
    size_t stepNumber{0};
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::chrono::nanoseconds stepDuration;
    size_t spikesCount{0};
    size_t neuronsUpdated{0};
    size_t synapticOperations{0};
    double stepEfficiency{0.0};
    std::unordered_map<std::string, double> operationTimes;
    std::string stepDescription;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * Performance bottleneck information
 */
struct BottleneckInfo {
    std::string name;
    double impactScore; // 0.0 to 1.0
    std::chrono::nanoseconds duration;
    std::string location;
    std::string description;
    std::vector<std::string> suggestedFixes;
    std::chrono::high_resolution_clock::time_point firstDetected;
    size_t detectionCount{0};
    std::string severity; // "low", "medium", "high", "critical"
};

/**
 * Performance trend information
 */
struct PerformanceTrend {
    enum class TrendType { IMPROVING, DEGRADING, STABLE, FLUCTUATING };
    
    TrendType type;
    double trendValue; // Rate of change per step
    double confidence; // 0.0 to 1.0
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::string metricName;
    std::vector<std::pair<std::chrono::high_resolution_clock::time_point, double>> dataPoints;
};

/**
 * Performance regression information
 */
struct PerformanceRegression {
    std::string metricName;
    double regressionValue; // Negative means regression
    double regressionPercentage;
    std::chrono::high_resolution_clock::time_point detectionTime;
    std::chrono::high_resolution_clock::time_point regressionStartTime;
    std::string regressionCause;
    std::string suggestedFix;
    double confidence{0.0};
};

/**
 * Step profiler alert
 */
struct ProfilerAlert {
    enum class AlertType { PERFORMANCE_REGRESSION, BOTTLENECK_DETECTED, SLOW_STEP, EFFICIENCY_DROP };
    
    AlertType type;
    std::string description;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> details;
    double severity; // 0.0 to 1.0
};

/**
 * StepProfiler - Step-by-step performance analysis
 */
class StepProfiler {
public:
    using StepCallback = std::function<void(const StepRecord&)>
;    using BottleneckCallback = std::function<void(const BottleneckInfo&)>
;    using TrendCallback = std::function<void(const PerformanceTrend&)>
;    using RegressionCallback = std::function<void(const PerformanceRegression&)>
;    using AlertCallback = std::function<void(const ProfilerAlert&)>
;    
    /**
     * Create a step profiler
     * @param enableRealTimeAnalysis Enable real-time performance analysis
     * @param analysisIntervalMs Interval for performance analysis (ms)
     * @param enableRegressionDetection Enable regression detection
     */
    StepProfiler(
        bool enableRealTimeAnalysis = true,
        std::chrono::milliseconds analysisInterval = std::chrono::milliseconds(1000),
        bool enableRegressionDetection = true);
    
    /**
     * Start profiling
     * @param enableNeuralMetrics Enable neural-specific metrics collection
     */
    void start(bool enableNeuralMetrics = true);
    
    /**
     * Stop profiling
     */
    void stop();
    
    /**
     * Check if profiler is running
     */
    bool isRunning() const { return running_.load(std::memory_order_relaxed); }
    
    /**
     * Start timing a simulation step
     * @param stepNumber Current step number
     * @param description Description of the step
     */
    void startStep(size_t stepNumber, const std::string& description = "");
    
    /**
     * End timing a simulation step
     * @param spikesCount Number of spikes in this step
     * @param neuronsUpdated Number of neurons updated in this step
     * @param synapticOperations Number of synaptic operations performed
     */
    void endStep(size_t spikesCount, size_t neuronsUpdated, size_t synapticOperations);
    
    /**
     * Record operation timing within a step
     * @param operationName Name of the operation
     * @param duration Duration of the operation
     * @param stepNumber Step number (if not current step, specify)
     */
    void recordOperation(const std::string& operationName,
                        std::chrono::nanoseconds duration,
                        std::optional<size_t> stepNumber = std::nullopt);
    
    /**
     * Get current step record
     * @return Current step record (if step is active)
     */
    std::optional<StepRecord> getCurrentStep() const;
    
    /**
     * Get step records for a range of steps
     * @param startStep Start step number (inclusive)
     * @param endStep End step number (inclusive, 0 = all)
     */
    std::vector<StepRecord> getStepRecords(size_t startStep = 0, size_t endStep = 0) const;
    
    /**
     * Get performance bottlenecks
     * @return Vector of bottleneck information
     */
    std::vector<BottleneckInfo> getBottlenecks() const;
    
    /**
     * Get performance trends
     * @param metricName Metric name to analyze (empty for all)
     * @param timeWindow Time window to analyze (0 = all data)
     */
    std::vector<PerformanceTrend> getTrends(const std::string& metricName = "",
                                           std::chrono::high_resolution_clock::duration timeWindow = std::chrono::hours(1)) const;
    
    /**
     * Get performance regressions
     * @return Vector of performance regressions
     */
    std::vector<PerformanceRegression> getPerformanceRegressions() const;
    
    /**
     * Get recent profiler alerts
     * @param count Number of recent alerts to retrieve
     */
    std::vector<ProfilerAlert> getRecentAlerts(size_t count = 50) const;
    
    /**
     * Add step callback
     * @param callback Function to call when step is completed
     */
    void addStepCallback(const StepCallback& callback);
    
    /**
     * Add bottleneck callback
     * @param callback Function to call when bottleneck is detected
     */
    void addBottleneckCallback(const BottleneckCallback& callback);
    
    /**
     * Add trend callback
     * @param callback Function to call when trend is detected
     */
    void addTrendCallback(const TrendCallback& callback);
    
    /**
     * Add regression callback
     * @param callback Function to call when regression is detected
     */
    void addRegressionCallback(const RegressionCallback& callback);
    
    /**
     * Add alert callback
     * @param callback Function to call when profiler alert is triggered
     */
    void addAlertCallback(const AlertCallback& callback);
    
    /**
     * Get step performance statistics
     * @param stepNumber Step number (0 = current step)
     */
    struct StepPerformanceStats {
        std::chrono::nanoseconds averageStepTime{0};
        std::chrono::nanoseconds minStepTime{0};
        std::chrono::nanoseconds maxStepTime{0};
        double averageStepEfficiency{0.0};
        double averageSpikesPerStep{0.0};
        double averageNeuronsPerStep{0.0};
        size_t totalSteps{0};
    };
    
    StepPerformanceStats getStepStats(size_t stepNumber = 0) const;
    
    /**
     * Get performance optimization recommendations
     * @return Vector of optimization recommendation strings
     */
    std::vector<std::string> getOptimizationRecommendations() const;
    
    /**
     * Generate performance report
     * @param format Report format ("text", "json", "csv")
     * @param includeTrends Include trend analysis
     * @param includeRegressions Include regression analysis
     */
    std::string generateReport(const std::string& format = "text",
                              bool includeTrends = true,
                              bool includeRegressions = true) const;
    
    /**
     * Save step records to file
     * @param filename File to save records to
     */
    void saveRecords(const std::string& filename) const;
    
    /**
     * Load step records from file
     */
    void loadRecords(const std::string& filename);
    
    /**
     * Get current step number
     */
    size_t getCurrentStepNumber() const { return currentStepNumber_.load(std::memory_order_relaxed); }
    
    /**
     * Get total number of steps recorded
     */
    size_t getTotalSteps() const { return totalSteps_.load(std::memory_order_relaxed); }
    
    /**
     * Reset profiler state
     */
    void reset();

private:
    // Thread management
    void profilingThreadFunc();
    
    // Step record management
    void completeStep(size_t spikesCount, size_t neuronsUpdated, size_t synapticOperations);
    
    // Performance analysis
    void analyzePerformanceTrends();
    void detectPerformanceRegressions();
    void identifyBottlenecks();
    
    // Alert generation
    void checkPerformanceThresholds();
    
    // Statistics calculation
    double calculateStepEfficiency(const StepRecord& step) const;
    
    // Internal data structures
    mutable std::mutex stepsMutex_;
    mutable std::mutex operationsMutex_;
    mutable std::mutex analysisMutex_;
    std::atomic<bool> running_{false};
    
    bool enableRealTimeAnalysis_;
    bool enableRegressionDetection_;
    std::chrono::milliseconds analysisInterval_;
    
    // Step tracking
    std::optional<StepRecord> currentStep_;
    std::deque<StepRecord> stepRecords_;
    std::map<size_t, std::vector<std::pair<std::string, std::chrono::nanoseconds>>> operationRecords_;
    
    std::atomic<size_t> currentStepNumber_{0};
    std::atomic<size_t> totalSteps_{0};
    
    // Performance analysis
    std::vector<BottleneckInfo> bottlenecks_;
    std::vector<PerformanceTrend> trends_;
    std::vector<PerformanceRegression> regressions_;
    
    // Alert tracking
    std::vector<ProfilerAlert> alerts_;
    
    // Callbacks
    std::vector<StepCallback> stepCallbacks_;
    std::vector<BottleneckCallback> bottleneckCallbacks_;
    std::vector<TrendCallback> trendCallbacks_;
    std::vector<RegressionCallback> regressionCallbacks_;
    std::vector<AlertCallback> alertCallbacks_;
    
    // Background thread
    std::thread profilingThread_;
    std::condition_variable stopCondition_;
    std::mutex stopMutex_;
};

} // namespace nlm