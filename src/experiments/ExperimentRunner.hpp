#pragma once

#include "../experiments/ExperimentConfig.hpp"
#include "../experiments/ExperimentMetrics.hpp"
#include "../experiments/Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../environment/Environment.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace nlm {

/**
 * @brief Comprehensive experiment running framework
 * 
 * This class provides a complete framework for running neural learning machine
 * experiments with support for:
 * - Automatic experiment setup and initialization
 * - Episode-based simulation execution
 * - Multi-episode experiment running
 * - Checkpoint saving and loading
 * - Thread-safe operations
 * - Comprehensive metrics collection
 * - Parameter validation
 * - Error handling with NLMRuntimeError
 * - Serialization support
 */
class ExperimentRunner {
public:
    // Constructor and destructor
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Copy control
    ExperimentRunner(const ExperimentRunner& other);
    ExperimentRunner& operator=(const ExperimentRunner& other);
    ExperimentRunner(ExperimentRunner&& other) noexcept;
    ExperimentRunner& operator=(ExperimentRunner&& other) noexcept;
    
    // Core experiment setup and execution
    bool setup(std::shared_ptr<ExperimentConfig> config,
               std::shared_ptr<Brain> brain,
               std::shared_ptr<AgentBrain> agent,
               std::shared_ptr<SimpleWorld> world,
               std::shared_ptr<Environment> environment);
    
    float runEpisode();
    bool runExperiment(int maxEpisodes = -1);
    
    // Component management
    bool initializeComponents();
    void resetComponents();
    void cleanupComponents();
    
    // Checkpoint management
    bool saveCheckpoint(const std::string& filepath);
    bool loadCheckpoint(const std::string& filepath);
    
    // State management
    void pause();
    void resume();
    bool isRunning() const;
    
    // Results and metrics
    const std::vector<std::shared_ptr<ExperimentMetrics>>& getExperimentMetrics() const;
    const std::shared_ptr<ExperimentMetrics>& getCurrentMetrics() const;
    std::string getResultsSummary() const;
    
    // Configuration access
    std::shared_ptr<ExperimentConfig> getConfig() const;
    void setConfig(std::shared_ptr<ExperimentConfig> config);
    
    // Component access
    std::shared_ptr<Brain> getBrain() const;
    std::shared_ptr<AgentBrain> getAgent() const;
    std::shared_ptr<SimpleWorld> getWorld() const;
    std::shared_ptr<Environment> getEnvironment() const;
    
    // Progress tracking
    double getProgress() const;
    size_t getCurrentEpisode() const;
    size_t getTotalEpisodes() const;
    
    // Validation and error handling
    bool validateSetup() const;
    std::string validateAndGetError() const;
    
    // Thread safety
    void lock();
    void unlock();
    bool isThreadSafe() const;
    
    // Convenience methods
    float runQuickTest();
    float runTrainingSession();
    float runBenchmark();
    
    // Statistics
    struct PerformanceStats {
        double averageEpisodeDuration;
        float averageEpisodeReward;
        float totalReward;
        float peakFiringRate;
        size_t totalSteps;
        size_t peakActiveNeurons;
        size_t peakFiringNeurons;
        
        std::string toString() const;
    };
    
    PerformanceStats getPerformanceStats() const;
    
    // Batch operations
    std::vector<std::shared_ptr<ExperimentMetrics>> runMultipleExperiments(
        const std::vector<std::shared_ptr<ExperimentConfig>>& configs,
        int episodesPerConfig = 1);
    
    // Comparison utilities
    double compareWith(const ExperimentRunner& other, const std::string& metricName) const;
    
    // Export and import
    bool exportResults(const std::string& filepath) const;
    bool importResults(const std::string& filepath);
    
    // Monitoring and debugging
    std::string getDebugInfo() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void initializeMetrics();
    void updateProgress();
    void cleanupOnError();
    void validateComponents() const;
};

} // namespace nlm
