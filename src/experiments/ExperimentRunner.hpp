#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "BrainClone.hpp"
#include "EnvironmentClone.hpp"
#include "ExperimentResult.hpp"
#include "BatchExperimentManager.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>

namespace nlm {

// ExperimentRunner: Enhanced experiment runner with cloning support
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // ========== BASIC FUNCTIONALITY (Backward Compatible) ==========
    
    // Create new experiment
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    // Run single experiment
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps);
    
    // Run batch of experiments
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Save experiment results
    bool saveResults(const stdstring& filepath) const;
    
    // ========== CLONING FUNCTIONALITY ==========
    
    // Clone brain with configuration template
    static std::shared_ptr<Brain> cloneBrain(std::shared_ptr<Brain> original,
                                            const std::string& templateName = "",
                                            BrainClone::CloneMode mode = BrainClone::CloneMode::DEEP);
    
    // Clone environment with state preservation
    static std::shared_ptr<Environment> cloneEnvironment(std::shared_ptr<Environment> original,
                                                         EnvironmentClone::CloneMode mode = EnvironmentClone::CloneMode::DEEP);
    
    // Clone experiment with full state preservation
    static std::shared_ptr<Experiment> cloneExperiment(std::shared_ptr<Experiment> original);
    
    // Configure cloning templates
    void setBrainTemplate(const std::string& name, std::shared_ptr<Brain> templateBrain);
    void setEnvironmentTemplate(const std::string& name, std::shared_ptr<Environment> templateEnvironment);
    
    // Get template brain
    std::shared_ptr<Brain> getBrainTemplate(const std::string& name) const;
    
    // Get template environment
    std::shared_ptr<Environment> getEnvironmentTemplate(const std::string& name) const;
    
    // ========== BATCH EXPERIMENT MANAGEMENT ==========
    
    // Execute batch of experiments with cloning
    std::vector<ExperimentResult> executeBatchExperiments(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate = nullptr,
        std::shared_ptr<Environment> environmentTemplate = nullptr,
        BatchExperimentManager::BatchConfig config = BatchExperimentManager::BatchConfig()
    );
    
    // Configure batch experiment manager
    void configureBatchManager(const BatchExperimentManager::BatchConfig& config);
    
    // Get batch experiment manager
    BatchExperimentManager& getBatchManager() { return batchManager_; }
    
    // ========== EXPERIMENT RESULT AGGREGATION ==========
    
    // Aggregate experiment results
    static ExperimentResult aggregateResults(const std::vector<ExperimentResult>& results,
                                            const std::string& metricName = "total_reward",
                                            const std::string& aggregationType = "mean");
    
    // Compare experiments results
    static std::vector<bool> compareExperimentResults(const std::vector<ExperimentResult>& results,
                                                      double tolerance = 1e-6);
    
    // Save experiment results to file
    static bool saveExperimentResults(const std::vector<ExperimentResult>& results,
                                      const std::string& filepath);
    
    // Load experiment results from file
    static std::vector<ExperimentResult> loadExperimentResults(const std::string& filepath);
    
    // ========== PAUSE/RESUME FUNCTIONALITY ==========
    
    // Pause batch execution
    void pauseBatchExecution();
    
    // Resume batch execution
    void resumeBatchExecution();
    
    // Check if batch execution is paused
    bool isBatchPaused() const;
    
    // Get batch execution progress
    std::pair<size_t, size_t> getBatchProgress() const;
    
    // Get current batch status message
    std::string getBatchStatusMessage() const;
    
    // ========== CHECKPOINT FUNCTIONALITY ==========
    
    // Save experiment checkpoint
    bool saveExperimentCheckpoint(std::shared_ptr<Experiment> experiment,
                                  const std::string& checkpointPath) const;
    
    // Load experiment checkpoint
    static std::shared_ptr<Experiment> loadExperimentCheckpoint(const std::string& checkpointPath);
    
    // Save brain checkpoint
    bool saveBrainCheckpoint(std::shared_ptr<Brain> brain,
                            const std::string& checkpointPath) const;
    
    // Load brain checkpoint
    static std::shared_ptr<Brain> loadBrainCheckpoint(const std::string& checkpointPath);
    
    // Save environment checkpoint
    bool saveEnvironmentCheckpoint(std::shared_ptr<Environment> environment,
                                   const std::string& checkpointPath) const;
    
    // Load environment checkpoint
    static std::shared_ptr<Environment> loadEnvironmentCheckpoint(const std::string& checkpointPath);
    
    // ========== ADVANCED EXPERIMENT MANAGEMENT ==========
    
    // Configure cloning for enhanced batch experiments
    struct CloningConfig {
        bool enableBrainCloning;
        bool enableEnvironmentCloning;
        BrainClone::CloneMode brainCloneMode;
        EnvironmentClone::CloneMode environmentCloneMode;
        bool preserveInternalState;
        bool threadSafeCloning;
        
        CloningConfig() : enableBrainCloning(true),
                         enableEnvironmentCloning(true),
                         brainCloneMode(BrainClone::CloneMode::DEEP),
                         environmentCloneMode(EnvironmentClone::CloneMode::DEEP),
                         preserveInternalState(false),
                         threadSafeCloning(true) {}
    };
    
    // Configure batch experiment settings
    struct BatchConfig {
        size_t maxConcurrentExperiments;
        SimulationStep maxStepsPerExperiment;
        double timeoutSeconds;
        bool checkpointEnabled;
        std::string checkpointPath;
        bool pauseOnError;
        
        BatchConfig() : maxConcurrentExperiments(4),
                       maxStepsPerExperiment(1000),
                       timeoutSeconds(300.0),
                       checkpointEnabled(false),
                       pauseOnError(false) {}
    };
    
    // Configure complete experiment run
    struct ExperimentRunConfig {
        std::string runName;
        CloningConfig cloningConfig;
        BatchConfig batchConfig;
        std::vector<std::string> experimentNames;
        std::map<std::string, std::string> experimentParameters;
        
        ExperimentRunConfig() : runName("default_run") {}
    };
    
    // Execute experiments with full configuration
    std::vector<ExperimentResult> executeExperimentRun(const ExperimentRunConfig& config);
    
    // ========== UTILITY FUNCTIONALITY ==========
    
    // Get experiment runner version
    static std::string getVersion();
    
    // Reset experiment runner state
    void reset();
    
    // Get cloning statistics
    static BrainClone::CloneStats getBrainCloneStats();
    static EnvironmentClone::CloneStats getEnvironmentCloneStats();
    
    // Enable/disable logging
    void setLoggingEnabled(bool enabled) { loggingEnabled_ = enabled; }
    bool isLoggingEnabled() const { return loggingEnabled_; }
    
    // Set experiment runner name
    void setName(const std::string& name) { runnerName_ = name; }
    const std::string& getName() const { return runnerName_; }
    
    // Get creation timestamp
    std::chrono::system_clock::time_point getCreationTime() const { return creationTime_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    BatchExperimentManager batchManager_;
    bool loggingEnabled_;
    std::string runnerName_;
    std::chrono::system_clock::time_point creationTime_;
};

} // namespace nlm
