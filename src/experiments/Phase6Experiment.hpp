#pragma once

#include "ExperimentRunner.hpp"
#include <string>
#include <memory>

namespace nlm {

/**
 * @brief Phase 6 experiment - Specific Phase 6 integration test
 * 
 * This class inherits from ExperimentRunner and provides specific Phase 6 integration tests
 * as requested by the user. It includes methods to verify integration, test memory integration,
 * test neuromodulation integration, test checkpointing, and test replay.
 * 
 * This class is designed for:
 * - Simulation research - Easy setup of different experimental conditions
 * - Hyperparameter tuning - Support for parameter sweeps
 * - Benchmarking - Performance measurement
 * - Reproducible research - Checkpoint saving/loading
 * - Ablation studies - System component testing
 */
class Phase6Experiment : public ExperimentRunner {
public:
    // Constructor and destructor
    Phase6Experiment();
    ~Phase6Experiment() override;
    
    // Copy control
    Phase6Experiment(const Phase6Experiment& other);
    Phase6Experiment& operator=(const Phase6Experiment& other);
    Phase6Experiment(Phase6Experiment&& other) noexcept;
    Phase6Experiment& operator=(Phase6Experiment&& other) noexcept;
    
    // Phase 6 integration test methods
    bool verifyIntegration() override;
    bool testMemoryIntegration() override;
    bool testNeuromodulationIntegration() override;
    bool testCheckpointing() override;
    bool testReplay() override;
    
    // Additional methods for experiment management
    bool runPhase6IntegrationTest(const std::shared_ptr<ExperimentConfig>& config);
    bool runComprehensiveTestSuite();
    
    // Integration result storage
    struct IntegrationResult {
        bool memoryWorkingMemoryIntegrated{false};
        bool memoryEpisodicMemoryIntegrated{false};
        bool neuromodulationIntegrated{false};
        bool predictionIntegrated{false};
        bool developmentIntegrated{false};
        bool checkpointingWorks{false};
        bool replayWorks{false};
        
        // Metrics
        float totalReward{0.0f};
        float avgFiringRate{0.0f};
        float avgSynapticWeight{0.0f};
        float memoryEpisodesStored{0.0f};
        float noveltyLevel{0.0f};
        float curiosityLevel{0.0f};
        float dopamineLevel{0.0f};
        
        // Timestamps
        time_t startTime{0};
        time_t endTime{0};
        double totalWallClockTime{0.0};
        
        // Detailed status
        std::string getDetailedStatus() const;
        std::string getIntegrationSummary() const;
        bool isSuccessfullyIntegrated() const;
    };
    
    // Integration result getter
    const IntegrationResult& getIntegrationResult() const;
    
    // Convenience methods for specific experiments
    bool runMemoryIntegrationExperiment();
    bool runNeuromodulationIntegrationExperiment();
    bool runDevelopmentIntegrationExperiment();
    bool runPredictionIntegrationExperiment();
    bool runCheckpointingIntegrationExperiment();
    bool runReplayIntegrationExperiment();
    
    // Configuration-specific experiments
    bool runAblationStudy(const std::string& ablationType);
    bool runHyperparameterSweep(const std::string& parameterName);
    bool runBenchmarkSuite();
    
    // Error handling and validation
    bool validatePhase6Setup() const;
    std::string validateAndGetPhase6Error() const;
    
    // Results export and import
    bool exportPhase6Results(const std::string& filepath) const;
    bool importPhase6Results(const std::string& filepath);
    
    // Progress tracking
    double getPhase6Progress() const;
    std::string getCurrentPhase6Test() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void initializePhase6Components();
    void cleanupPhase6Components();
    void runIndividualIntegrationTest(const std::string& testName);
    void recordIntegrationResult(const std::string& testName, bool success);
};

} // namespace nlm
