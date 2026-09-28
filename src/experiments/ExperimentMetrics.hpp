#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <chrono>
#include <map>
#include <functional>

namespace nlm {

/**
 * @brief Comprehensive experiment metrics tracking class
 * 
 * This class provides advanced metrics tracking for experiments with support for:
 * - Real-time step recording
 * - Multiple metric types (scalar, temporal, derived)
 * - Statistical analysis
 * - Performance benchmarking
 * - Thread-safe operations
 * - Memory usage tracking
 * - Checkpoint support
 */
class ExperimentMetrics {
public:
    // Structure for step-level metrics
    struct StepMetrics {
        SimulationStep step;
        float reward;
        float firingRate;
        size_t activeNeurons;
        size_t firingNeurons;
        float totalSpikes;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        std::chrono::high_resolution_clock::time_point timestamp;
        
        StepMetrics()
            : step(0), reward(0.0f), firingRate(0.0f), activeNeurons(0), 
              firingNeurons(0), totalSpikes(0.0f), curiosityLevel(0.0f),
              noveltyLevel(0.0f), dopamineLevel(0.0f) {}
    };
    
    // Constructor and destructor
    ExperimentMetrics();
    ~ExperimentMetrics();
    
    // Copy control
    ExperimentMetrics(const ExperimentMetrics& other);
    ExperimentMetrics& operator=(const ExperimentMetrics& other);
    ExperimentMetrics(ExperimentMetrics&& other) noexcept;
    ExperimentMetrics& operator=(ExperimentMetrics&& other) noexcept;
    
    // Core recording methods
    void recordStep(SimulationStep step, float reward, float firingRate,
                   size_t activeNeurons, size_t firingNeurons,
                   float totalSpikes, float curiosityLevel = 0.0f,
                   float noveltyLevel = 0.0f, float dopamineLevel = 0.0f);
    
    void recordReward(float reward);
    void recordFiringRate(float firingRate);
    void recordActiveNeurons(size_t count);
    void recordFiringNeurons(size_t count);
    void recordTotalSpikes(float spikes);
    void recordNeuromodulation(float curiosity, float novelty, float dopamine);
    
    // Batch recording methods
    void recordSteps(const std::vector<StepMetrics>& steps);
    void recordEpisode(float totalReward, float avgFiringRate,
                      size_t totalActiveNeurons, size_t totalFiringNeurons,
                      float totalSpikes);
    
    // Query methods
    float getTotalReward() const;
    float getAverageFiringRate() const;
    float getAverageRewardPerStep() const;
    size_t getTotalSteps() const;
    size_t getTotalActiveNeurons() const;
    size_t getTotalFiringNeurons() const;
    float getTotalSpikes() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getDopamineLevel() const;
    
    // Advanced query methods
    StepMetrics getStepMetrics(SimulationStep step) const;
    const std::vector<StepMetrics>& getAllStepMetrics() const;
    
    // Statistical analysis methods
    float getRewardMean() const;
    float getRewardStdDev() const;
    float getFiringRateMean() const;
    float getFiringRateStdDev() const;
    float getRewardPerStepMean() const;
    
    // Performance metrics
    double getAverageStepDuration() const;
    double getTotalSimulationTime() const;
    float getStepsPerSecond() const;
    
    // System performance metrics
    size_t getMemoryUsage() const;
    size_t getMetricCount() const;
    
    // Metrics snapshot and analysis
    struct MetricsSnapshot {
        float totalReward;
        float avgFiringRate;
        float avgRewardPerStep;
        size_t stepsRecorded;
        size_t peakActiveNeurons;
        size_t peakFiringNeurons;
        float totalSpikes;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        double simulationTime;
        double throughput;
        
        std::string toString() const;
    };
    
    MetricsSnapshot createSnapshot() const;
    std::string getSummaryReport() const;
    
    // Comparison and analysis
    double compareWith(const ExperimentMetrics& other, const std::string& metricName) const;
    
    // Reset and cleanup
    void reset();
    void clear();
    
    // Metrics export and import
    bool exportToFile(const std::string& filepath) const;
    bool importFromFile(const std::string& filepath);
    
    // Validation and consistency checks
    bool isValid() const;
    std::string validateAndGetError() const;
    
    // Iteration support
    using StepIterator = std::vector<StepMetrics>::const_iterator;
    StepIterator begin() const;
    StepIterator end() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void initializeInternalStructures();
    void updateDerivedMetrics();
    void validateMetricsConsistency() const;
};

} // namespace nlm
