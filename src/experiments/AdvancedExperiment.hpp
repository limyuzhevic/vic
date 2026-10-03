// Advanced Experiment - Enhanced Phase 6 Integration Test
// Demonstrates advanced learning protocols and complex brain dynamics

#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <random>

namespace nlm {

class AdvancedExperiment {
public:
    AdvancedExperiment();
    ~AdvancedExperiment();
    
    // Advanced configuration for complex learning scenarios
    struct AdvancedConfig {
        uint64_t maxSteps;
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        bool enableCheckpointing;
        bool enableReplay;
        bool enableDevelopment;
        bool enableAdvancedLearning;
        bool enableMultiTaskLearning;
        bool enableMetaLearning;
        float learningRate;
        float explorationRate;
        float noveltyThreshold;
        std::string checkpointPath;
        
        // Advanced learning parameters
        bool usePolicyGradient;
        bool useReinforcementLearning;
        bool useCurriculumLearning;
        bool useTransferLearning;
        float rewardDecay;
        float curiosityExploration;
        
        // Performance and monitoring
        bool enablePerformanceMonitoring;
        bool enableDetailedLogging;
        size_t logInterval;
        
        AdvancedConfig()
            : maxSteps(20000)
            , neuronCount(2000)
            , regionCount(2)
            , connectionProbability(0.08)
            , enableCheckpointing(true)
            , enableReplay(true)
            , enableDevelopment(true)
            , enableAdvancedLearning(true)
            , enableMultiTaskLearning(false)
            , enableMetaLearning(false)
            , learningRate(0.01f)
            , explorationRate(0.1f)
            , noveltyThreshold(0.5f)
            , checkpointPath("./advanced_checkpoint.bin")
            , usePolicyGradient(false)
            , useReinforcementLearning(true)
            , useCurriculumLearning(true)
            , useTransferLearning(false)
            , rewardDecay(0.99f)
            , curiosityExploration(0.3f)
            , enablePerformanceMonitoring(true)
            , enableDetailedLogging(true)
            , logInterval(1000) {}
    };
    
    // Advanced experiment result with comprehensive metrics
    struct AdvancedResult {
        // Basic metrics
        float totalReward;
        float avgFiringRate;
        float avgSynapticWeight;
        float memoryEpisodesStored;
        
        // Advanced learning metrics
        float policyGradientScore;
        float reinforcementLearningReturn;
        float metaLearningAccuracy;
        float curriculumLearningProgress;
        float transferLearningEffectiveness;
        
        // Neuromodulation metrics
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        
        // Memory system metrics
        float workingMemoryEfficiency;
        float episodicMemoryAccuracy;
        float associativeMemoryStrength;
        
        // Performance metrics
        float simulationSpeed;
        float memoryUsage;
        float cpuUsage;
        double wallClockTime;
        
        // Integration status
        bool memoryWorkingMemoryIntegrated;
        bool memoryEpisodicMemoryIntegrated;
        bool memoryAssociativeMemoryIntegrated;
        bool neuromodulationIntegrated;
        bool predictionIntegrated;
        bool developmentIntegrated;
        bool advancedLearningIntegrated;
        bool multiTaskLearningIntegrated;
        bool metaLearningIntegrated;
        
        // Learning effectiveness
        float learningCurveProgress;
        float generalizationAccuracy;
        float adaptationSpeed;
        
        // System health
        bool systemStable;
        bool noMemoryOverflow;
        bool noSynapseExploding;
        
        AdvancedResult() : totalReward(0.0f), avgFiringRate(0.0f), avgSynapticWeight(0.0f),
                         memoryEpisodesStored(0.0f), policyGradientScore(0.0f),
                         reinforcementLearningReturn(0.0f), metaLearningAccuracy(0.0f),
                         curriculumLearningProgress(0.0f), transferLearningEffectiveness(0.0f),
                         dopamineLevel(0.0f), curiosityLevel(0.0f), noveltyLevel(0.0f),
                         predictionError(0.0f), workingMemoryEfficiency(0.0f),
                         episodicMemoryAccuracy(0.0f), associativeMemoryStrength(0.0f),
                         simulationSpeed(0.0f), memoryUsage(0.0f), cpuUsage(0.0f),
                         wallClockTime(0.0), systemStable(true), noMemoryOverflow(true),
                         noSynapseExploding(true), learningCurveProgress(0.0f),
                         generalizationAccuracy(0.0f), adaptationSpeed(0.0f) {}
    };
    
    // Run advanced experiment with enhanced learning protocols
    AdvancedResult run(const AdvancedConfig& config);
    
private:
    std::mt19937 randomGen_;
    
    // Advanced learning algorithm implementations
    float computePolicyGradient(std::shared_ptr<Brain> brain, uint64_t step, const AdvancedConfig& config);
    float computeQValue(std::shared_ptr<Brain> brain, MotorCommand action, float reward, const AdvancedConfig& config);
    float updateCurriculumLearning(AgentBrain& agent, uint64_t step, const AdvancedConfig& config);
    
    // System health checks
    bool checkSystemStability(std::shared_ptr<Brain> brain);
    bool checkMemoryOverflow(std::shared_ptr<Brain> brain);
    bool checkSynapseStability(std::shared_ptr<Brain> brain);
    
    // Performance calculations
    float calculateLearningCurve(std::shared_ptr<Brain> brain, const AdvancedConfig& config);
    float calculateGeneralization(std::shared_ptr<Brain> brain);
    float calculateAdaptationSpeed(std::shared_ptr<Brain> brain, const AdvancedConfig& config);
};

} // namespace nlm