#pragma once

#include "../core/Config/Config.hpp"
#include <chrono>
#include <string>
#include <vector>

namespace nlm {

struct Phase6Config {
    size_t neuronCount = 1000;
    size_t maxSteps = 10000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    bool enablePrediction = true;
    bool enableCognition = true;
    float curiosityLevel = 0.1f;
    float rewardScale = 1.0f;
    std::string experimentName = "Phase6Integration";
    std::string checkpointPath = "./nlm_checkpoint.bin";
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
};

struct Phase6ExperimentResult {
    double totalReward;
    double avgFiringRate;
    size_t memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    size_t spikesGenerated;
    size_t synapsesModified;
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    double totalWallClockTime;
    std::string experimentName;
    size_t totalSteps;
    size_t successfulSteps;
    time_t startTime;
    time_t endTime;
};

class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    // Verify that all systems are properly connected
    bool verifyIntegration();
    
    // Test individual memory systems
    void testMemoryIntegration();
    
    // Test neuromodulation integration
    void testNeuromodulationIntegration();
    
    // Test checkpointing functionality
    void testCheckpointing();
    
    // Test memory replay functionality
    void testReplay();
    
    // Run the full Phase 6 integration experiment
    Phase6ExperimentResult run(const Phase6Config& config);
    
    // Get experiment status
    const std::string& getStatus() const { return status_; }
    
    // Reset experiment state
    void reset();
    
    // Get experiment statistics
    size_t getStepCount() const { return stepCount_; }
    size_t getEpisodeCount() const { return episodeCount_; }
    float getAvgFiringRate() const { return avgFiringRate_; }
    
private:
    // Internal experiment state
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Experiment progress
    std::string status_;
    size_t stepCount_;
    size_t episodeCount_;
    float avgFiringRate_;
    double totalReward_;
    
    // Integration verification helpers
    bool verifyMemorySystems() const;
    bool verifyNeuromodulationSystems() const;
    bool verifyDevelopmentSystems() const;
    bool verifyPredictionSystems() const;
    bool verifyCognitionSystems() const;
    
    // Integration testing helpers
    void runMemoryTest(const Phase6Config& config);
    void runNeuromodulationTest(const Phase6Config& config);
    void runCheckpointTest(const Phase6Config& config);
    void runReplayTest(const Phase6Config& config);
    
    // Integration verification helpers
    void verifyWorkingMemoryIntegration() const;
    void verifyEpisodicMemoryIntegration() const;
    void verifyAssociativeMemoryIntegration() const;
    void verifyDopamineIntegration() const;
    void verifyCuriosityIntegration() const;
    void verifyNoveltyIntegration() const;
    void verifyPredictionErrorIntegration() const;
    void verifyDevelopmentSystemIntegration() const;
    void verifyAttentionIntegration() const;
    void verifyPlannerIntegration() const;
    void verifyConceptFormationIntegration() const;
};