#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>

namespace nlm {

// Advanced experiment configuration options
struct AdvancedExperimentConfig {
    // Simulation parameters
    SimulationStep maxSteps = 10000;
    double timestep = 0.001;
    uint64_t randomSeed = 42;
    
    // Output and logging
    std::string outputPath;
    std::string logPath;
    std::string configPath;
    
    // Performance optimization
    int numThreads = 1;
    size_t batchSize = 100;
    bool enableProfiling = false;
    
    // Data collection
    bool collectNeuralTraces = false;
    bool collectSynapticEvents = false;
    bool recordSpikes = true;
    
    // Parallel execution
    bool enableParallel = false;
    int maxConcurrentExperiments = 4;
    
    // Output formats
    bool enableJSONOutput = false;
    bool enableCSVOutput = false;
    bool enableBinaryOutput = false;
    
    // Experimental variants
    enum class Variant {
        BASIC,
        PLASTICITY,
        STDP,
        LEARNING,
        DEVELOPMENTAL,
        SOCIAL_LEARNING,
        CONTINUAL_LEARNING,
        NOVELTY_DRIVEN
    };
    
    Variant experimentVariant = Variant::BASIC;
    
    // Special features
    bool enableReplay = false;
    bool enableCheckpointing = false;
    bool enableDevelopment = false;
    bool enableCuriosity = false;
    bool enablePrediction = false;
};

// Experiment results with advanced metrics
struct AdvancedExperimentResult {
    // Basic results
    bool success = false;
    std::string experimentName;
    uint64_t seed;
    
    // Performance metrics
    double wallClockTime = 0.0;
    double cpuTime = 0.0;
    size_t memoryUsage = 0;
    double stepsPerSecond = 0.0;
    
    // Neural metrics
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    float avgFiringRate = 0.0f;
    float avgSynapticWeight = 0.0f;
    size_t maxActiveTraces = 0;
    size_t totalEpisodesStored = 0;
    
    // Learning metrics
    float finalLearningProgress = 0.0f;
    float predictionAccuracy = 0.0f;
    float rewardEfficiency = 0.0f;
    float explorationRate = 0.0f;
    
    // Neuromodulation metrics
    float dopamineLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float noveltyLevel = 0.0f;
    float predictionError = 0.0f;
    
    // Development metrics
    float developmentalStage = 0.0f;
    float plasticityRate = 0.0f;
    float synapticStability = 0.0f;
    
    // Integration verification
    bool memoryIntegrated = false;
    bool neuromodulationIntegrated = false;
    bool predictionIntegrated = false;
    bool developmentIntegrated = false;
    bool replayWorks = false;
    bool checkpointingWorks = false;
    
    // Time tracking
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    
    // Experiment metadata
    std::unordered_map<std::string, float> customMetrics;
    std::vector<std::string> experimentalNotes;
};

// ExperimentRunner: Runs experiments and collects data
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Create new experiment with advanced features
    std::shared_ptr<Experiment> createExperiment(const std::string& name,
                                                  const AdvancedExperimentConfig& config);
    
    // Create specific experiment variants
    std::shared_ptr<Experiment> createPlasticityExperiment(const std::string& name);
    std::shared_ptr<Experiment> createDevelopmentalExperiment(const std::string& name);
    std::shared_ptr<Experiment> createSocialLearningExperiment(const std::string& name);
    std::shared_ptr<Experiment> createContinualLearningExperiment(const std::string& name);
    std::shared_ptr<Experiment> createNoveltyDrivenExperiment(const std::string& name);
    
    // Run single experiment with advanced result collection
    AdvancedExperimentResult runExperiment(std::shared_ptr<Experiment> experiment,
                                          std::shared_ptr<Brain> brain,
                                          std::shared_ptr<Environment> environment,
                                          const AdvancedExperimentConfig& config);
    
    // Run batch of experiments with parallel execution
    std::vector<AdvancedExperimentResult> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        const AdvancedExperimentConfig& config);
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Save experiment results with advanced formatting
    bool saveResults(const std::string& filepath, 
                    const std::vector<AdvancedExperimentResult>& results) const;
    
    // Load experiment configuration from file
    bool loadConfig(const std::string& filepath, AdvancedExperimentConfig& config) const;
    
    // Save experiment configuration to file
    bool saveConfig(const std::string& filepath, const AdvancedExperimentConfig& config) const;
    
    // Validate experiment configuration
    bool validateConfig(const AdvancedExperimentConfig& config) const;
    
    // Get experiment statistics
    size_t getExperimentCount() const;
    float getAverageSuccessRate() const;
    double getAverageWallClockTime() const;
    
    // Export results to various formats
    bool exportResultsToJSON(const std::vector<AdvancedExperimentResult>& results, 
                           const std::string& filepath) const;
    bool exportResultsToCSV(const std::vector<AdvancedExperimentResult>& results,
                           const std::string& filepath) const;
    bool exportResultsToBinary(const std::vector<AdvancedExperimentResult>& results,
                             const std::string& filepath) const;
    
    // Advanced experiment execution modes
    enum class ExecutionMode {
        SEQUENTIAL,
        PARALLEL,
        PIPELINED,
        THREADED
    };
    
    // Set execution mode
    void setExecutionMode(ExecutionMode mode);
    
    // Enable/disable specific experimental features
    void enableFeature(const std::string& featureName, bool enable = true);
    bool isFeatureEnabled(const std::string& featureName) const;
    
    // Get experiment recommendations
    std::vector<std::string> getRecommendedExperiments(const AdvancedExperimentConfig& config) const;
    
    // Export detailed experiment analysis
    std::string generateAnalysisReport(const std::vector<AdvancedExperimentResult>& results) const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable = true);
    double getAveragePerformanceScore() const;
    
    // Configuration validation and schema checking
    static std::vector<std::string> getValidConfigKeys();
    static std::unordered_map<std::string, std::string> getConfigDescription(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
