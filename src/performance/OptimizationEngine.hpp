#pragma once

/**
 * Optimization Engine Module
 * 
 * Main optimization engine that coordinates all performance optimization modules for neural network simulation with:
 * - Integration with existing optimization systems
 * - Support for automated optimization
 * - Performance analysis
 * - Optimization recommendations
 * 
 * Features:
 * - Coordinated optimization across all modules
 * - Automated optimization triggers
 * - Comprehensive performance analysis
 * - Integration with neural network systems
 * - Optimization validation
 */

#ifndef NLM_OPTIMIZATION_ENGINE_HPP
#define NLM_OPTIMIZATION_ENGINE_HPP

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
#include <numeric>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "Performance.hpp"
#include "MemoryOptimizer.hpp"
#include "ProcessingOptimizer.hpp"
#include "ConfigOptimizer.hpp"
#include "SystemOptimizer.hpp"
#include "PerfMonitor.hpp"

namespace nlm {

/**
 * Optimization system state
 */
enum class OptimizationState {
    DISABLED,
    INITIALIZING,
    OPTIMIZING,
    VALIDATING,
    APPLIED,
    MAINTAINING,
    ERROR
};

/**
 * Overall system performance metrics
 */
struct SystemPerformanceMetrics {
    double overallScore;
    double memoryEfficiency;
    double processingEfficiency;
    double configEfficiency;
    double systemEfficiency;
    double trendScore;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<double> componentScores;
    std::vector<std::string> topOptimizations;
};

/**
 * Automated optimization trigger
 */
struct OptimizationTrigger {
    std::string triggerType;
    double threshold;
    std::chrono::steady_clock::time_point lastTriggered;
    size_t triggerCount;
};

/**
 * Optimization engine implementing:
 * - Integration with existing optimization systems
 * - Support for automated optimization
 * - Performance analysis
 * - Optimization recommendations
 */
class OptimizationEngine {
public:
    /**
     * Construct optimization engine with configuration
     * @param enableAutomated Enable automated optimization
     * @param optimizationInterval Optimization interval in milliseconds
     * @param validationInterval Validation interval in milliseconds
     */
    explicit OptimizationEngine(bool enableAutomated = true,
                               uint64_t optimizationInterval = 30000,
                               uint64_t validationInterval = 60000);
    
    ~OptimizationEngine();
    
    // Disable copying and moving
    OptimizationEngine(const OptimizationEngine&) = delete;
    OptimizationEngine& operator=(const OptimizationEngine&) = delete;
    OptimizationEngine(OptimizationEngine&&) = delete;
    OptimizationEngine& operator=(OptimizationEngine&&) = delete;
    
    /**
     * Initialize optimization engine
     * @return true if initialization succeeded
     */
    bool initialize();
    
    /**
     * Finalize optimization engine
     */
    void finalize();
    
    /**
     * Run optimization cycle
     * @return true if optimization was performed
     */
    bool runOptimizationCycle();
    
    /**
     * Get optimization system state
     * @return Current optimization state
     */
    OptimizationState getOptimizationState() const;
    
    /**
     * Set optimization system state
     * @param state New optimization state
     */
    void setOptimizationState(OptimizationState state);
    
    /**
     * Get system performance metrics
     * @return System performance metrics
     */
    SystemPerformanceMetrics getSystemPerformanceMetrics() const;
    
    /**
     * Get optimization recommendations
     * @return Optimization recommendations
     */
    std::vector<OptimizationRecommendation> getOptimizationRecommendations() const;
    
    /**
     * Apply optimization recommendation
     * @param recommendation Recommendation to apply
     * @return true if recommendation was applied successfully
     */
    bool applyOptimizationRecommendation(const OptimizationRecommendation& recommendation);
    
    /**
     * Validate current system state
     * @return true if system is in valid state
     */
    bool validateSystemState() const;
    
    /**
     * Get optimization report
     * @return Formatted optimization report
     */
    std::string getOptimizationReport() const;
    
    /**
     * Set monitoring system reference
     * @param monitor Performance monitor reference
     */
    void setPerformanceMonitor(std::shared_ptr<PerfMonitor> monitor);
    
    /**
     * Set memory optimizer reference
     * @param optimizer Memory optimizer reference
     */
    void setMemoryOptimizer(std::shared_ptr<MemoryOptimizer> optimizer);
    
    /**
     * Set processing optimizer reference
     * @param optimizer Processing optimizer reference
     */
    void setProcessingOptimizer(std::shared_ptr<ProcessingOptimizer> optimizer);
    
    /**
     * Set config optimizer reference
     * @param optimizer Config optimizer reference
     */
    void setConfigOptimizer(std::shared_ptr<ConfigOptimizer> optimizer);
    
    /**
     * Set system optimizer reference
     * @param optimizer System optimizer reference
     */
    void setSystemOptimizer(std::shared_ptr<SystemOptimizer> optimizer);
    
    /**
     * Get current optimization score
     * @return Optimization score (0-100)
     */
    double getOptimizationScore() const;
    
    /**
     * Check if optimization is needed
     * @return true if optimization is needed
     */
    bool isOptimizationNeeded() const;

private:
    // Internal helper classes and methods
    struct OptimizationCycle {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        bool successful;
        std::vector<OptimizationRecommendation> appliedRecommendations;
        double performanceScoreBefore;
        double performanceScoreAfter;
        std::string status;
    };
    
    // Optimization coordination
    void coordinateMemoryOptimization();
    void coordinateProcessingOptimization();
    void coordinateConfigOptimization();
    void coordinateSystemOptimization();
    
    // Automated optimization triggers
    void checkAutomatedTriggers();
    bool checkPerformanceDegradationTrigger() const;
    bool checkResourceExhaustionTrigger() const;
    bool checkMemoryPressureTrigger() const;
    bool checkThresholdTrigger(const OptimizationTrigger& trigger) const;
    
    // Optimization validation
    bool validateMemoryOptimization() const;
    bool validateProcessingOptimization() const;
    bool validateConfigOptimization() const;
    bool validateSystemOptimization() const;
    
    // Performance analysis
    void analyzePerformanceTrends();
    void analyzeBottleneckPatterns();
    void predictOptimizationImpact(std::vector<OptimizationRecommendation>& recommendations);
    
    // Optimization recommendation aggregation
    void aggregateOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations);
    double calculateRecommendationPriority(const OptimizationRecommendation& recommendation) const;
    bool isRecommendationDuplicate(const OptimizationRecommendation& recommendation,
                                 const std::vector<OptimizationRecommendation>& existing) const;
    
    // Optimization cycle management
    OptimizationCycle runOptimizationCycleImpl();
    bool executeOptimizationRecommendations(const std::vector<OptimizationRecommendation>& recommendations);
    double calculatePerformanceImprovement() const;
    
    // System integration
    void integrateWithNeuralNetworkSystems();
    void updateOptimizationSystem();
    
    // State variables
    bool initialized_;
    bool automatedOptimizationEnabled_;
    OptimizationState currentState_;
    
    // Optimization components
    std::shared_ptr<PerfMonitor> perfMonitor_;
    std::shared_ptr<MemoryOptimizer> memoryOptimizer_;
    std::shared_ptr<ProcessingOptimizer> processingOptimizer_;
    std::shared_ptr<ConfigOptimizer> configOptimizer_;
    std::shared_ptr<SystemOptimizer> systemOptimizer_;
    
    // Automated optimization triggers
    std::vector<OptimizationTrigger> optimizationTriggers_;
    
    // Performance metrics
    mutable std::mutex metricsMutex_;
    SystemPerformanceMetrics performanceMetrics_;
    
    // Optimization history
    std::vector<OptimizationCycle> optimizationHistory_;
    
    // Thread for automated optimization
    std::thread optimizationThread_;
    std::atomic<bool> optimizationActive_;
    
    // Timing
    std::chrono::steady_clock::time_point lastOptimizationTime_;
    std::chrono::steady_clock::time_point lastValidationTime_;
    
    // Thresholds for optimization triggers
    static constexpr double PERFORMANCE_DEGRADATION_THRESHOLD = 0.15; // 15% degradation
    static constexpr double MEMORY_PRESSURE_THRESHOLD = 0.85; // 85% memory usage
    static constexpr double RESOURCE_EXHAUSTION_THRESHOLD = 0.95; // 95% resource usage
};

} // namespace nlm

#endif // NLM_OPTIMIZATION_ENGINE_HPP