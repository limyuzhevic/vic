#pragma once

/**
 * PerformanceMonitor - Tracks simulation performance metrics
 * 
 * Provides comprehensive performance monitoring for neural simulations,
 * including firing rate statistics, memory usage, learning progress,
 * and system performance metrics.
 * 
 * Features:
 * - Real-time firing rate tracking
 * - Memory usage monitoring
 * - Learning progress tracking
 * - System performance metrics
 * - Detailed statistics export
 */

#include <chrono>
#include <deque>
#include <map>
#include <string>
#include <vector>

namespace nlm {

/**
 * Performance metrics for a simulation step
 */
struct PerformanceMetrics {
    // Timestamps
    double wallClockTime;           // Wall clock time for this step
    double simulationTime;          // Simulation time for this step
    
    // Neural metrics
    float firingRate;               // Average firing rate for this step
    size_t firingNeuronCount;       // Number of neurons that fired
    size_t totalSpikeCount;         // Total spikes generated
    
    // Memory metrics
    size_t workingMemorySize;       // Working memory size
    size_t episodicMemoryCount;     // Number of episodic memories stored
    size_t associativeMemorySize;   // Associative memory size
    
    // Neuromodulation metrics
    float dopamineLevel;            // Current dopamine level
    float noveltyLevel;             // Current novelty level
    float curiosityLevel;           // Current curiosity level
    
    // Learning metrics
    float predictionError;          // Current prediction error
    float learningRate;             // Learning rate applied
    float plasticityChange;         // Total plasticity change this step
    
    // Behavior metrics
    float reward;                   // Reward received this step
    float energy;                   // Current energy level
    uint64_t actionCount;           // Number of actions taken
    
    PerformanceMetrics()
        : wallClockTime(0.0)
        , simulationTime(0.0)
        , firingRate(0.0f)
        , firingNeuronCount(0)
        , totalSpikeCount(0)
        , workingMemorySize(0)
        , episodicMemoryCount(0)
        , associativeMemorySize(0)
        , dopamineLevel(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , predictionError(0.0f)
        , learningRate(0.0f)
        , plasticityChange(0.0f)
        , reward(0.0f)
        , energy(0.0f)
        , actionCount(0) {}
};

/**
 * Comprehensive performance monitor
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Start performance monitoring
     */
    void startMonitoring();
    
    /**
     * Stop performance monitoring
     */
    void stopMonitoring();
    
    /**
     * Record performance metrics for a simulation step
     */
    void recordStep(const PerformanceMetrics& metrics);
    
    /**
     * Get the current performance metrics
     */
    const PerformanceMetrics& getCurrentMetrics() const { return currentMetrics_; }
    
    /**
     * Get the average firing rate over the last N steps
     */
    float getAverageFiringRate(size_t steps = 100) const;
    
    /**
     * Get the average reward over the last N steps
     */
    float getAverageReward(size_t steps = 100) const;
    
    /**
     * Get the maximum firing rate observed
     */
    float getMaxFiringRate() const { return maxFiringRate_; }
    
    /**
     * Get the minimum firing rate observed
     */
    float getMinFiringRate() const { return minFiringRate_; }
    
    /**
     * Get the standard deviation of firing rates
     */
    float getFiringRateStdDev() const { return firingRateStdDev_; }
    
    /**
     * Get the total number of steps recorded
     */
    size_t getStepCount() const { return stepCount_; }
    
    /**
     * Get the total simulation time
     */
    double getTotalSimulationTime() const { return totalSimulationTime_; }
    
    /**
     * Get the total wall clock time
     */
    double getTotalWallClockTime() const { return totalWallClockTime_; }
    
    /**
     * Get the performance summary as a formatted string
     */
    std::string getSummary() const;
    
    /**
     * Get detailed performance statistics
     */
    std::string getDetailedStats() const;
    
    /**
     * Reset all statistics
     */
    void reset();
    
    /**
     * Check if monitoring is currently active
     */
    bool isMonitoring() const { return isMonitoring_; }
    
    /**
     * Get the performance metrics over the last N steps
     */
    std::vector<PerformanceMetrics> getRecentMetrics(size_t steps) const;
    
    /**
     * Calculate the learning progress based on reward changes
     */
    float calculateLearningProgress(size_t window = 100) const;
    
    /**
     * Get the peak memory usage
     */
    size_t getPeakMemoryUsage() const { return peakMemoryUsage_; }
    
    /**
     * Get the average memory usage
     */
    float getAverageMemoryUsage() const { return averageMemoryUsage_; }
    
private:
    bool isMonitoring_;
    
    // Current metrics
    PerformanceMetrics currentMetrics_;
    
    // Historical metrics
    std::deque<PerformanceMetrics> recentMetrics_;
    size_t maxRecentMetrics_;
    
    // Statistics
    size_t stepCount_;
    double totalWallClockTime_;
    double totalSimulationTime_;
    
    // Firing rate statistics
    float maxFiringRate_;
    float minFiringRate_;
    float firingRateSum_;
    float firingRateStdDev_;
    
    // Memory usage statistics
    size_t peakMemoryUsage_;
    float averageMemoryUsage_;
    
    // Time tracking
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Helper functions
    void updateFiringRateStats(float newFiringRate);
    void updateMemoryUsageStats(size_t memoryUsage);
    void updateTimeStats(double wallClockTime, double simulationTime);
};

} // namespace nlm
