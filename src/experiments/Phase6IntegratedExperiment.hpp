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
#include <chrono>

namespace nlm {

/**
 * Phase 6 integration test result
 */
struct Phase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
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
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0) {}
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
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin") {}
};

/**
 * IntegrationHandler - Coordinates interaction between all brain systems
 */
class IntegrationHandler {
public:
    IntegrationHandler(std::shared_ptr<Brain> brain, AgentBrain& agent);
    ~IntegrationHandler();
    
    // Process sensory input through the entire integration chain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Update all systems for current step
    void updateIntegrationStep();
    
    // Get system integration status
    std::unordered_map<std::string, bool> getSystemStatus() const;
    
    // Get integration metrics
    std::unordered_map<std::string, float> getIntegrationMetrics() const;
    
    // Get last step time
    double getLastStepTime() const { return lastStepTime_; }
    
    // Set last step time
    void setLastStepTime(double time) { lastStepTime_ = time; }
    
private:
    // Private helper methods for system coordination
    void handleSensoryToWorkingMemory(const SensoryPercept& percept);
    void handleSensoryToAttention(const SensoryPercept& percept);
    void handleSensoryToConceptFormation(const SensoryPercept& percept);
    void handleSensoryToPrediction(const SensoryPercept& percept);
    void handleSensoryToSelfModel(const SensoryPercept& percept);
    
    void handleWorkingMemoryToAttention();
    void handleAttentionToConceptFormation();
    void handleConceptFormationToPlanning();
    void handlePlanningToActionSelection();
    void handleActionToSelfModel();
    void handleSelfModelToEpisodicMemory();
    void handleEpisodicMemoryToConsolidation();
    void handleDevelopmentUpdate(double dt);
    void handleNeuromodulationUpdate();
    void handleRestCycleUpdate();
    
    void strengthenRelatedConnections(const EpisodicMemoryItem* episode);
    
    std::vector<float> extractPatternFromMemoryTrace(const WorkingMemoryTrace& trace);
    float computeTraceRelevance(float value);
    std::vector<float> extractRegionPattern(NeuralRegion* region);
    std::vector<float> extractCompleteNeuralState(const Brain& brain);
    std::vector<float> extractCompleteSensoryState(const Brain& brain);
    std::vector<NeuronId> extractActiveNeurons(const Brain& brain);
    std::vector<float> extractFeaturesFromPattern(const std::vector<float>& pattern);
    std::vector<float> extractStateFromPercept(const SensoryPercept& percept);
    float computeVisualSalience(const std::vector<float>& vision);
    float computePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual);
    bool isEpisodeImportant(const EpisodicMemoryItem* episode, float threshold);
    
    float getCurrentReward() const;
    float getCurrentEnergy() const;
    float getActualActionOutcome() const;
    
    std::shared_ptr<Brain> brain_;
    AgentBrain& agent_;
    double lastStepTime_;
};

/**
 * Phase 6 integrated experiment runner
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint save/load
     */
    bool testCheckpointing();
    
    /**
     * Test replay system
     */
    bool testReplay();
};

} // namespace nlm
