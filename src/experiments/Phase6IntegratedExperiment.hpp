#pragma once

#include <chrono>
#include <string>
#include <memory>
#include <vector>

namespace nlm {
    
// Forward declarations for included headers
class Config;
class Brain;
class SimpleWorld;
class AgentBrain;
class SensoryPercept;
class MotorCommand;

// Phase6Config structure - Configuration for Phase 6 experiments
struct Phase6Config {
    uint64_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    uint64_t regionCount = 1;
    float connectionProbability = 0.1f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
};

// Phase6IntegrationResult structure - Results from Phase 6 integration experiments
struct Phase6IntegrationResult {
    double totalReward = 0.0;
    double avgFiringRate = 0.0;
    double memoryEpisodesStored = 0.0;
    double dopamineLevel = 0.0;
    double curiosityLevel = 0.0;
    double noveltyLevel = 0.0;
    bool memoryWorkingMemoryIntegrated = false;
    bool memoryEpisodicMemoryIntegrated = false;
    bool neuromodulationIntegrated = false;
    bool predictionIntegrated = false;
    bool developmentIntegrated = false;
    bool checkpointingWorks = false;
    time_t startTime = 0;
    time_t endTime = 0;
    double totalWallClockTime = 0.0;
};

// Phase6IntegratedExperiment - Comprehensive integration test for Phase 6
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    // Main entry point for Phase 6 integration experiment
    Phase6IntegrationResult run(const Phase6Config& config);
    
    // Integration verification methods
    bool verifyIntegration();
    bool testMemoryIntegration();
    bool testNeuromodulationIntegration();
    bool testCheckpointing();
    bool testReplay();
    
    // Helper methods
    void logIntegrationResults(const Phase6IntegrationResult& result) const;
    bool isExperimentEnabled(const std::string& feature) const;
    
private:
    // Integration test methods
    bool testMemorySystemsIntegration() const;
    bool testNeuromodulationSystemsIntegration() const;
    bool testPredictionSystemIntegration() const;
    bool testDevelopmentSystemIntegration() const;
    bool testCognitionSystemsIntegration() const;
    
    // Helper methods
    bool checkSystemConnection(const std::string& systemName, bool isConnected) const;
    double calculateIntegrationScore(const Phase6IntegrationResult& result) const;
    
    // Configuration and state
    Phase6Config currentConfig;
    Phase6IntegrationResult currentResult;
    bool isInitialized = false;
    
    // Statistics and metrics
    uint64_t successfulTests = 0;
    uint64_t totalTests = 0;
    double experimentStartTime = 0.0;
};

} // namespace nlm
