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
#include "../experiments/ResearchMetrics.hpp"
#include "../experiments/ResearchTools.hpp"
#include "../visualization/NLMLiveVisualization.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * Phase 6 integration test result with advanced metrics
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
    
    // Advanced metrics
    float avgLearningRate;
    float memoryConsolidationProgress;
    float neuralPlasticityIndex;
    float predictionAccuracy;
    float cognitiveFlexibility;
    float developmentalMaturation;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    // Visualization data
    std::string visualizationDataPath;
    uint64_t capturedFrames;
    
    // Research data
    std::string researchMetricsPath;
    std::string researchToolsPath;
    
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
        , avgLearningRate(0.0f)
        , memoryConsolidationProgress(0.0f)
        , neuralPlasticityIndex(0.0f)
        , predictionAccuracy(0.0f)
        , cognitiveFlexibility(0.0f)
        , developmentalMaturation(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0)
        , capturedFrames(0) {}
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
    bool enableVisualization;
    std::string visualizationOutputPath;
    bool enableAdvancedMetrics;
    std::string metricsOutputPath;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , enableVisualization(true)
        , visualizationOutputPath("./visualization_data")
        , enableAdvancedMetrics(true)
        , metricsOutputPath("./advanced_metrics") {}
};

/**
 * Phase 6 integrated experiment runner with advanced visualization and metrics
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test with advanced visualization and metrics
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
    
    /**
     * Enable research visualization mode
     */
    bool enableResearchVisualization();
    
    /**
     * Enable advanced metrics collection
     */
    bool enableAdvancedMetrics();
    
    /**
     * Get research tools instance
     */
    nlm::ResearchTools* getResearchTools();
    
    /**
     * Get visualization instance
     */
    nlm::NLMLiveVisualization* getVisualization();
    
    /**
     * Get research metrics instance
     */
    nlm::ResearchMetrics* getResearchMetrics();
    
    /**
     * Export research data
     */
    bool exportResearchData(const std::string& filepath);
    
    /**
     * Import research data
     */
    bool importResearchData(const std::string& filepath);
    
private:
    std::unique_ptr<nlm::NLMLiveVisualization> visualization_;
    std::unique_ptr<nlm::ResearchMetrics> researchMetrics_;
    std::unique_ptr<nlm::ResearchTools> researchTools_;
    bool researchVisualizationEnabled_;
    bool advancedMetricsEnabled_;
};

} // namespace nlm
