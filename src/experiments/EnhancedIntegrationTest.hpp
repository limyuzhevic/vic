#pragma once

#include <chrono>
#include <cmath>
#include <vector>
#include <string>

namespace nlm {

struct EnhancedIntegrationConfig {
    uint64_t neuronCount;
    uint64_t regionCount;
    float connectionProbability;
    uint64_t maxSteps;
    uint64_t metricsInterval;
    uint64_t sensoryInputSize;
    
    EnhancedIntegrationConfig() : neuronCount(1000), regionCount(1), connectionProbability(0.1f),
        maxSteps(1000), metricsInterval(100), sensoryInputSize(256) {}
};

struct EnhancedIntegrationResult {
    double startTime;
    double endTime;
    double wallClockTime;
    
    // Memory system integration
    float memoryWorkingMemoryIntegrated;
    float memoryEpisodicMemoryIntegrated;
    float memoryAssociativeMemoryIntegrated;
    
    // Neuromodulation integration (complete set)
    float neuromodulationDopamineIntegrated;
    float neuromodulationAChIntegrated;
    float neuromodulationNEIntegrated;
    float neuromodulation5HTIntegrated;
    float neuromodulationCuriosityIntegrated;
    float neuromodulationNoveltyIntegrated;
    float neuromodulationPredictionErrorIntegrated;
    
    // Cognition integration
    float cognitionNeuralPlannerIntegrated;
    float cognitionConceptFormationIntegrated;
    float cognitionAttentionIntegrated;
    float cognitionSelfModelIntegrated;
    float cognitionSocialLearningIntegrated;
    
    // Prediction integration
    float predictionSystemIntegrated;
    float predictionAccuracy;
    
    // Development integration
    float developmentSystemIntegrated;
    float developmentalStageProgression;
    
    // Persistence integration
    float persistenceSystemIntegrated;
    float checkpointSuccessRate;
    
    // Memory replay integration
    float memoryReplayIntegrated;
    float replayEffectiveness;
    
    // Overall integration score
    float overallIntegrationScore;
    
    EnhancedIntegrationResult() : startTime(0), endTime(0), wallClockTime(0),
        memoryWorkingMemoryIntegrated(0), memoryEpisodicMemoryIntegrated(0), memoryAssociativeMemoryIntegrated(0),
        neuromodulationDopamineIntegrated(0), neuromodulationAChIntegrated(0), neuromodulationNEIntegrated(0), neuromodulation5HTIntegrated(0),
        neuromodulationCuriosityIntegrated(0), neuromodulationNoveltyIntegrated(0), neuromodulationPredictionErrorIntegrated(0),
        cognitionNeuralPlannerIntegrated(0), cognitionConceptFormationIntegrated(0), cognitionAttentionIntegrated(0),
        cognitionSelfModelIntegrated(0), cognitionSocialLearningIntegrated(0),
        predictionSystemIntegrated(0), predictionAccuracy(0),
        developmentSystemIntegrated(0), developmentalStageProgression(0),
        persistenceSystemIntegrated(0), checkpointSuccessRate(0),
        memoryReplayIntegrated(0), replayEffectiveness(0),
        overallIntegrationScore(0) {}
};

class EnhancedIntegrationExperiment {
public:
    EnhancedIntegrationExperiment();
    ~EnhancedIntegrationExperiment();
    
    EnhancedIntegrationResult run(const EnhancedIntegrationConfig& config);
    
protected:
    EnhancedIntegrationResult verifySystemIntegration(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config);
    EnhancedIntegrationResult runEnhancedSimulation(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config, EnhancedIntegrationResult result);
    EnhancedIntegrationResult runPersistenceTests(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config, EnhancedIntegrationResult result);
    EnhancedIntegrationResult verifyMemoryReplay(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config, EnhancedIntegrationResult result);
    
    float calculateIntegrationScore(const EnhancedIntegrationResult& result);
    void updateMetrics(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config, EnhancedIntegrationResult& result, uint64_t step);
};

} // namespace nlm