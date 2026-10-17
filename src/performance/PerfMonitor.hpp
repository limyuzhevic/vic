#pragma once

/**
 * Performance Monitoring Module
 * 
 * Comprehensive performance monitoring and analysis for neural network simulation with:
 * - Performance profiling
 * - Bottleneck identification
 * - Performance trending
 * - Performance prediction
 * - Optimization recommendations
 * 
 * Features:
 * - Real-time performance profiling
 * - Automatic bottleneck detection
 * - Performance trend analysis
 * - Predictive performance optimization
 * - Automated optimization recommendations
 */

#ifndef NLM_PERF_MONITOR_HPP
#define NLM_PERF_MONITOR_HPP

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <optional>
#include <functional>

#include "Performance.hpp"
#include "MemoryOptimizer.hpp"
#include "ProcessingOptimizer.hpp"
#include "ConfigOptimizer.hpp"
#include "SystemOptimizer.hpp"

namespace nlm {

/**
 * Performance profile data point
 */
struct PerformanceProfile {
    SimulationStep step;
    std::chrono::steady_clock::time_point timestamp;
    double cpuUsage;
    double memoryUsage;
    double diskIO;
    double networkIO;
    double spikeProcessingTime;
    double neuralComputationTime;
    double configLoadingTime;
    double iowait;
    size_t activeSpikes;
    size_t totalSpikesProcessed;
    size_t memoryAllocations;
    size_t configLoads;
    size_t networkPackets;
};

/**
 * Bottleneck detection result
 */
struct BottleneckInfo {
    std::string component;
    std::string bottleneckType;
    double impactScore;
    std::chrono::steady_clock::time_point detectedTime;
    std::string recommendation;
    std::chrono::steady_clock::time_point nextCheckTime;
    bool resolved;
};

/**
 * Performance trend data point
 */
struct PerformanceTrend {
    SimulationStep step;
    double averageSpikeProcessingTime;
    double averageNeuralComputationTime;
    double memoryUsageMB;
    double cpuUtilizationPercent;
    double configLoadingTimeMs;
    double overallPerformanceScore;
    std::chrono::steady_clock::time_point timestamp;
};

/**
 * Performance prediction model
 */
struct PerformancePrediction {
    SimulationStep step;
    double predictedSpikeProcessingTime;
    double predictedNeuralComputationTime;
    double predictedMemoryUsage;
    double predictedCPUUsage;
    double confidenceInterval;
    std::chrono::steady_clock::time_point predictionTime;
};

/**
 * Optimization recommendation
 */
struct OptimizationRecommendation {
    std::string component;
    std::string recommendationType;
    std::string description;
    double expectedImprovement;
    std::chrono::steady_clock::time_point recommendedTime;
    bool applied;
    std::string result;
};

/**
 * Performance monitoring data
 */
struct MonitoringData {
    SimulationStep currentStep;
    std::vector<PerformanceProfile> profiles;
    std::vector<BottleneckInfo> bottlenecks;
    std::vector<PerformanceTrend> trends;
    std::vector<PerformancePrediction> predictions;
    std::vector<OptimizationRecommendation> recommendations;
    std::chrono::steady_clock::time_point lastUpdate;
};

/**
 * Performance monitoring system implementing:
 * - Performance profiling
 * - Bottleneck identification
 * - Performance trending
 * - Performance prediction
 * - Optimization recommendations
 */
class PerfMonitor {
public:
    /**
     * Construct performance monitor with configuration
     * @param enableProfiling Enable performance profiling
     * @param enableBottleneckDetection Enable bottleneck detection
     * @param enableTrendAnalysis Enable trend analysis
     * @param enablePrediction Enable performance prediction
     * @param monitoringInterval Monitoring interval in milliseconds
     */
    explicit PerfMonitor(bool enableProfiling = true,
                        bool enableBottleneckDetection = true,
                        bool enableTrendAnalysis = true,
                        bool enablePrediction = true,
                        uint64_t monitoringInterval = 1000);
    
    ~PerfMonitor();
    
    // Disable copying and moving
    PerfMonitor(const PerfMonitor&) = delete;
    PerfMonitor& operator=(const PerfMonitor&) = delete;
    PerfMonitor(PerfMonitor&&) = delete;
    PerfMonitor& operator=(PerfMonitor&&) = delete;
    
    /**
     * Initialize performance monitoring system
     * @return true if initialization succeeded
     */
    bool initialize();
    
    /**
     * Finalize performance monitoring system
     */
    void finalize();
    
    /**
     * Record performance profile
     * @param profile Performance profile to record
     */
    void recordProfile(const PerformanceProfile& profile);
    
    /**
     * Start performance profiling
     */
    void startProfiling();
    
    /**
     * Stop performance profiling
     */
    void stopProfiling();
    
    /**
     * Get current performance profile
     * @return Current performance profile
     */
    PerformanceProfile getCurrentProfile() const;
    
    /**
     * Get performance history
     * @param limit Maximum number of profiles to return
     * @return Vector of performance profiles
     */
    std::vector<PerformanceProfile> getPerformanceHistory(size_t limit = 100) const;
    
    /**
     * Run bottleneck detection
     * @param[out] bottlenecks Detected bottlenecks
     * @return Number of bottlenecks detected
     */
    size_t runBottleneckDetection(std::vector<BottleneckInfo>& bottlenecks);
    
    /**
     * Get bottleneck analysis
     * @param[out] bottlenecks Bottleneck analysis
     */
    void getBottleneckAnalysis(std::vector<BottleneckInfo>& bottlenecks);
    
    /**
     * Run performance trend analysis
     * @param[out] trends Performance trends
     */
    void runTrendAnalysis(std::vector<PerformanceTrend>& trends);
    
    /**
     * Get performance trends
     * @param[out] trends Performance trends
     */
    void getPerformanceTrends(std::vector<PerformanceTrend>& trends);
    
    /**
     * Run performance prediction
     * @param steps Ahead steps to predict
     * @param[out] predictions Performance predictions
     */
    void runPerformancePrediction(size_t steps, std::vector<PerformancePrediction>& predictions);
    
    /**
     * Get performance predictions
     * @param[out] predictions Performance predictions
     */
    void getPerformancePredictions(std::vector<PerformancePrediction>& predictions);
    
    /**
     * Generate optimization recommendations
     * @param[out] recommendations Optimization recommendations
     */
    void generateOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations);
    
    /**
     * Get optimization recommendations
     * @param[out] recommendations Optimization recommendations
     */
    void getOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations);
    
    /**
     * Apply optimization recommendation
     * @param recommendation Recommendation to apply
     * @return true if recommendation was applied successfully
     */
    bool applyOptimizationRecommendation(const OptimizationRecommendation& recommendation);
    
    /**
     * Get overall performance score
     * @return Performance score (0-100)
     */
    double getOverallPerformanceScore() const;
    
    /**
     * Check if performance is degrading
     * @param window Size of time window to analyze
     * @return true if performance is degrading
     */
    bool isPerformanceDegrading(size_t window = 10) const;
    
    /**
     * Generate performance monitoring report
     * @return Formatted monitoring report
     */
    std::string generateReport() const;
    
    /**
     * Get monitoring data
     * @return Monitoring data
     */
    MonitoringData getMonitoringData() const;

private:
    // Internal helper classes and methods
    struct ProfileMetrics {
        double cpuUsage;
        double memoryUsage;
        double diskIO;
        double networkIO;
        double spikeProcessingTime;
        double neuralComputationTime;
        double configLoadingTime;
        double iowait;
        size_t activeSpikes;
        size_t totalSpikesProcessed;
        size_t memoryAllocations;
        size_t configLoads;
        size_t networkPackets;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    // Performance profiling
    ProfileMetrics collectProfileMetrics() const;
    PerformanceProfile createProfileFromMetrics(const ProfileMetrics& metrics) const;
    
    // Bottleneck detection algorithms
    void detectMemoryBottlenecks(std::vector<BottleneckInfo>& bottlenecks);
    void detectProcessingBottlenecks(std::vector<BottleneckInfo>& bottlenecks);
    void detectIOBottlenecks(std::vector<BottleneckInfo>& bottlenecks);
    void detectConfigBottlenecks(std::vector<BottleneckInfo>& bottlenecks);
    
    // Performance trend analysis
    void calculateTrendScores(std::vector<PerformanceTrend>& trends) const;
    double calculatePerformanceScore(const PerformanceProfile& profile) const;
    
    // Performance prediction
    void predictSpikeProcessing(std::vector<PerformancePrediction>& predictions) const;
    void predictNeuralComputation(std::vector<PerformancePrediction>& predictions) const;
    void predictMemoryUsage(std::vector<PerformancePrediction>& predictions) const;
    void predictCPUUsage(std::vector<PerformancePrediction>& predictions) const;
    
    // Optimization recommendation generation
    void generateMemoryOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations);
    void generateProcessingOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations);
    void generateConfigOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations);
    void generateSystemOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations);
    
    // Performance monitoring thread
    void monitoringThreadFunc();
    
    // Bottleneck resolution
    void resolveBottlenecks();
    
    // State variables
    bool initialized_;
    bool profilingEnabled_;
    bool bottleneckDetectionEnabled_;
    bool trendAnalysisEnabled_;
    bool predictionEnabled_;
    uint64_t monitoringInterval_;
    
    // Monitoring data
    mutable std::mutex dataMutex_;
    MonitoringData monitoringData_;
    
    // Performance history
    std::deque<PerformanceProfile> performanceHistory_;
    
    // Bottleneck tracking
    std::unordered_map<std::string, BottleneckInfo> activeBottlenecks_;
    
    // Thread for monitoring
    std::thread monitoringThread_;
    std::atomic<bool> monitoringActive_;
    
    // Thresholds for bottleneck detection
    static constexpr double MEMORY_THRESHOLD = 0.8; // 80% memory usage
    static constexpr double CPU_THRESHOLD = 0.9; // 90% CPU usage
    static constexpr double IO_THRESHOLD = 0.7; // 70% I/O utilization
    static constexpr double SPIKE_PROCESSING_THRESHOLD = 10.0; // ms
    static constexpr double NEURAL_COMPUTATION_THRESHOLD = 20.0; // ms
};

} // namespace nlm

#endif // NLM_PERF_MONITOR_HPP