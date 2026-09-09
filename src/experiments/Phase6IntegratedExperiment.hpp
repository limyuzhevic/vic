#pragma once

/**
 * Phase6IntegratedExperiment - Phase 6 final integration test
 * 
 * This experiment demonstrates that the integrated brain systems work together:
 * - Memory systems are connected to neural processing
 * - Neuromodulation affects plasticity and neural dynamics
 * - Development affects plasticity rates
 * - Prediction system is integrated
 * - Replay and consolidation are functional
 * - Checkpoint save/load works
 * 
 * The experiment runs a complete lifetime simulation and verifies
 * that all systems interact properly.
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// Result structure for test functions
struct TestResult {
    std::string testName;
    bool passed;
    std::string errorMessage;
    std::string details;
    double executionTime;
};

/**
 * Integration verification result with detailed breakdown
 */
struct DetailedIntegrationResult {
    // Individual system integration status
    bool memoryWorkingMemoryIntegrated;
    std::string memoryWorkingMemoryError;
    bool memoryEpisodicMemoryIntegrated;
    std::string memoryEpisodicMemoryError;
    
    bool neuromodulationIntegrated;
    std::string neuromodulationError;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    
    bool predictionIntegrated;
    std::string predictionError;
    
    bool developmentIntegrated;
    std::string developmentError;
    
    // Additional system status
    bool plannerIntegrated;
    std::string plannerError;
    bool conceptFormationIntegrated;
    std::string conceptFormationError;
    bool attentionIntegrated;
    std::string attentionError;
    
    // Timing and metrics
    double verificationTime;
    size_t testedNeuronCount;
    
    DetailedIntegrationResult()
        : memoryWorkingMemoryIntegrated(false), memoryWorkingMemoryError("Not tested"),
          memoryEpisodicMemoryIntegrated(false), memoryEpisodicMemoryError("Not tested"),
          neuromodulationIntegrated(false), neuromodulationError("Not tested"),
          dopamineLevel(0.0f), curiosityLevel(0.0f), noveltyLevel(0.0f),
          predictionIntegrated(false), predictionError("Not tested"),
          developmentIntegrated(false), developmentError("Not tested"),
          plannerIntegrated(false), plannerError("Not tested"),
          conceptFormationIntegrated(false), conceptFormationError("Not tested"),
          attentionIntegrated(false), attentionError("Not tested"),
          verificationTime(0.0), testedNeuronCount(0) {}
};

/**
 * Memory integration metrics
 */
struct MemoryMetrics {
    bool workingMemorySuccess;
    std::string workingMemoryError;
    size_t workingMemoryActiveTraces;
    size_t workingMemoryCapacity;
    
    bool episodicMemorySuccess;
    std::string episodicMemoryError;
    size_t episodicMemoryEpisodeCount;
    size_t episodicMemoryMaxCapacity;
    
    double memoryVerificationTime;
    size_t testedSteps;
    
    MemoryMetrics()
        : workingMemorySuccess(false), workingMemoryError("Not tested"),
          workingMemoryActiveTraces(0), workingMemoryCapacity(0),
          episodicMemorySuccess(false), episodicMemoryError("Not tested"),
          episodicMemoryEpisodeCount(0), episodicMemoryMaxCapacity(0),
          memoryVerificationTime(0.0), testedSteps(0) {}
};

/**
 * Neuromodulation metrics
 */
struct NeuromodulationMetrics {
    bool dopamineSuccess;
    std::string dopamineError;
    float dopamineBaseline;
    float dopaminePeak;
    
    bool curiositySuccess;
    std::string curiosityError;
    float curiosityBaseline;
    float curiosityPeak;
    
    bool noveltySuccess;
    std::string noveltyError;
    float noveltyBaseline;
    float noveltyPeak;
    
    bool neuromodulationSignals;
    std::string neuromodulationError;
    double neuromodulationVerificationTime;
    size_t signalInjections;
    
    NeuromodulationMetrics()
        : dopamineSuccess(false), dopamineError("Not tested"),
          dopamineBaseline(0.0f), dopaminePeak(0.0f),
          curiositySuccess(false), curiosityError("Not tested"),
          curiosityBaseline(0.0f), curiosityPeak(0.0f),
          noveltySuccess(false), noveltyError("Not tested"),
          noveltyBaseline(0.0f), noveltyPeak(0.0f),
          neuromodulationSignals(false), neuromodulationError("Not tested"),
          neuromodulationVerificationTime(0.0), signalInjections(0) {}
};

/**
 * Checkpoint validation result
 */
struct CheckpointValidation {
    bool saveSuccess;
    std::string saveError;
    uint64_t savedNeurons;
    uint64_t savedSpikes;
    
    bool loadSuccess;
    std::string loadError;
    uint64_t loadedNeurons;
    uint64_t loadedSpikes;
    
    bool integritySuccess;
    std::string integrityError;
    double checkpointTime;
    
    CheckpointValidation()
        : saveSuccess(false), saveError("Not attempted"),
          savedNeurons(0), savedSpikes(0),
          loadSuccess(false), loadError("Not attempted"),
          loadedNeurons(0), loadedSpikes(0),
          integritySuccess(false), integrityError("Not tested"),
          checkpointTime(0.0) {}
};

/**
 * Replay system metrics
 */
struct ReplayMetrics {
    bool episodicMemoryAvailable;
    std::string episodicMemoryError;
    size_t availableEpisodes;
    size_t maxReplayableEpisodes;
    
    bool replayRetrievalSuccess;
    std::string replayRetrievalError;
    size_t replayedEpisodeCount;
    double replayQualityScore;
    
    bool replayConsistency;
    std::string replayConsistencyError;
    double replayVerificationTime;
    
    ReplayMetrics()
        : episodicMemoryAvailable(false), episodicMemoryError("Not tested"),
          availableEpisodes(0), maxReplayableEpisodes(0),
          replayRetrievalSuccess(false), replayRetrievalError("Not tested"),
          replayedEpisodeCount(0), replayQualityScore(0.0),
          replayConsistency(false), replayConsistencyError("Not tested"),
          replayVerificationTime(0.0) {}
};

/**
 * Comprehensive experiment result with error handling
 */
struct Phase6IntegrationResult {
    // System integration status with error details
    bool memoryWorkingMemoryIntegrated;
    std::string memoryWorkingMemoryError;
    bool memoryEpisodicMemoryIntegrated;
    std::string memoryEpisodicMemoryError;
    bool neuromodulationIntegrated;
    std::string neuromodulationError;
    bool predictionIntegrated;
    std::string predictionError;
    bool developmentIntegrated;
    std::string developmentError;
    bool checkpointingWorks;
    std::string checkpointingError;
    bool replayWorks;
    std::string replayError;
    
    // Metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    // Recovery and error handling
    bool recoveryApplied;
    std::string recoveryMessage;
    std::vector<std::string> errorRecoveryActions;
    
    // Detailed metrics
    MemoryMetrics memoryMetrics;
    NeuromodulationMetrics neuromodulationMetrics;
    CheckpointValidation checkpointValidation;
    ReplayMetrics replayMetrics;
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false), memoryWorkingMemoryError("Not tested"),
          memoryEpisodicMemoryIntegrated(false), memoryEpisodicMemoryError("Not tested"),
          neuromodulationIntegrated(false), neuromodulationError("Not tested"),
          predictionIntegrated(false), predictionError("Not tested"),
          developmentIntegrated(false), developmentError("Not tested"),
          checkpointingWorks(false), checkpointingError("Not tested"),
          replayWorks(false), replayError("Not tested"),
          totalReward(0.0f), avgFiringRate(0.0f), avgSynapticWeight(0.0f),
          memoryEpisodesStored(0.0f), noveltyLevel(0.0f), curiosityLevel(0.0f),
          dopamineLevel(0.0f), startTime(0), endTime(0), totalWallClockTime(0.0),
          recoveryApplied(false), recoveryMessage("No recovery needed"),
          memoryMetrics(), neuromodulationMetrics(),
          checkpointValidation(), replayMetrics() {}
};

/**
 * Configuration for Phase 6 integration experiment
 */
struct Phase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    // Error handling settings
    bool enableRecovery;
    size_t maxRecoveryAttempts;
    double recoveryTimeout;
    
    Phase6Config()
        : maxSteps(10000), neuronCount(1000), regionCount(1), connectionProbability(0.1f),
          enableCheckpointing(true), enableReplay(true), enableDevelopment(true),
          checkpointPath("./checkpoint_test.bin"), enableRecovery(true),
          maxRecoveryAttempts(3), recoveryTimeout(5.0) {}
};

/**
 * Phase 6 integrated experiment runner
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test with detailed error handling
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     */
    DetailedIntegrationResult verifyIntegration();
    
    /**
     * Test memory integration with detailed metrics
     */
    MemoryMetrics testMemoryIntegration();
    
    /**
     * Test neuromodulation integration with detailed metrics
     */
    NeuromodulationMetrics testNeuromodulationIntegration();
    
    /**
     * Test checkpoint save/load with validation
     */
    CheckpointValidation testCheckpointing();
    
    /**
     * Test replay system with detailed metrics
     */
    ReplayMetrics testReplay();
};
