#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

struct NeuromodulationConfig {
    // Enable/disable neuromodulation systems
    bool rewardModulationEnabled;
    bool structuralPlasticityEnabled;
    bool developmentEnabled;
    bool curiosityEnabled;
    
    // Novelty detection thresholds
    float noveltyThreshold;
    float noveltyDecayRate;
    
    // Curiosity weights
    float curiosityNoveltyWeight;
    float curiosityPredictionErrorWeight;
    float curiosityDecayRate;
    
    // Reward modulation parameters
    float rewardLearningRate;
    float predictionErrorClampMin;
    float predictionErrorClampMax;
    
    // Dopamine dynamics
    float dopamineBaseline;
    float dopaminePeak;
    float dopamineDecayRate;
    float dopamineReleaseRate;
    
    // Plasticity modulation
    float plasticityBaseline;
    float plasticityModulationRange;
    
    // Development stages
    double developmentStageInitial;
    double developmentStageCriticalPeriod;
    double developmentStageMaturation;
    
    // Structural plasticity rates
    float synaptogenesisRateInitial;
    float synaptogenesisRateMaturation;
    float pruningRateInitial;
    float pruningRateAdult;
    
    // Motor thresholds
    float motorActivityThreshold;
    float curiosityExplorationThreshold;
    float curiosityExplorationProbability;
    
    // Environmental dimensions
    int sensoryVisionWidth;
    int sensoryVisionHeight;
    
    // Default configuration
    NeuromodulationConfig() 
        : rewardModulationEnabled(true)
        , structuralPlasticityEnabled(true)
        , developmentEnabled(true)
        , curiosityEnabled(true)
        , noveltyThreshold(0.3f)
        , noveltyDecayRate(0.99f)
        , curiosityNoveltyWeight(0.5f)
        , curiosityPredictionErrorWeight(0.5f)
        , curiosityDecayRate(0.05f)
        , rewardLearningRate(0.05f)
        , predictionErrorClampMin(-1.0f)
        , predictionErrorClampMax(1.0f)
        , dopamineBaseline(0.0f)
        , dopaminePeak(1.0f)
        , dopamineDecayRate(0.1f)
        , dopamineReleaseRate(1.0f)
        , plasticityBaseline(0.5f)
        , plasticityModulationRange(1.0f)
        , developmentStageInitial(60.0)
        , developmentStageCriticalPeriod(300.0)
        , developmentStageMaturation(900.0)
        , synaptogenesisRateInitial(0.0001f)
        , synaptogenesisRateMaturation(0.00005f)
        , pruningRateInitial(0.00001f)
        , pruningRateAdult(0.000005f)
        , motorActivityThreshold(0.5f)
        , curiosityExplorationThreshold(0.5f)
        , curiosityExplorationProbability(0.3f)
        , sensoryVisionWidth(16)
        , sensoryVisionHeight(16) {}
};

} // namespace nlm