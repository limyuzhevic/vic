// AgentBrain development and neuromodulation module
// Handles development processes, neuromodulation, and learning systems

#include "AgentBrainDevelopment.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void AgentBrainDevelopment::updateDevelopment(
    Brain& brain,
    double timestep,
    double& developmentalAge,
    float& plasticityModifier,
    bool developmentEnabled,
    bool structuralPlasticityEnabled) {
    
    if (!developmentEnabled) return;
    
    developmentalAge += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge < 60.0) {  // ~1 minute
        plasticityModifier = 1.0f;  // High plasticity
        brain.setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge < 300.0) {  // ~5 minutes
        plasticityModifier = 0.8f;
        brain.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge < 900.0) {  // ~15 minutes
        plasticityModifier = 0.5f;
        brain.setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier = 0.2f;  // Adult - more stable
        brain.setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled) {
        auto* sp = brain.getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AgentBrainDevelopment::applyRewardModulation(
    Brain& brain,
    float reward,
    float predictedReward,
    float& predictionError,
    float& expectedReward,
    float& dopamineLevel,
    float plasticityModifier,
    bool rewardModulationEnabled) {
    
    if (!rewardModulationEnabled) return;
    
    // Compute prediction error
    predictionError = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward = 0.95f * expectedReward + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel = predictionError;
    
    // Clamp to reasonable range
    dopamineLevel = std::clamp(dopamineLevel, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain.getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Reward-modulated weight change
                float delta = eligibility * dopamineLevel * plasticityModifier * 0.05f;
                delta = std::clamp(delta, -0.1f, 0.1f);
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain.getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrainDevelopment::updateNoveltyAndCuriosity(
    const std::vector<float>& vision,
    const std::vector<float>& touch,
    std::vector<float>& predictedVision,
    float& noveltyLevel,
    float& curiosityLevel,
    float predictionError,
    float& developmentalAge,
    float& plasticityModifier,
    bool curiosityEnabled,
    float sensoryNoveltyDecay) {
    
    // Calculate novelty as prediction error against a slowly adapting
    // expectation of the scene (habituation)
    if (!vision.empty()) {
        if (predictedVision.empty() || predictedVision.size() != vision.size()) {
            predictedVision.assign(vision.size(), 0.0f);
        }
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size(); ++i) {
            totalDiff += std::abs(vision[i] - predictedVision[i]);
        }
        noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel *= sensoryNoveltyDecay;
        
        // Adapt the prediction toward the current scene
        constexpr float kPredictionAlpha = 0.05f;
        for (size_t i = 0; i < vision.size(); ++i) {
            predictedVision[i] += kPredictionAlpha * (vision[i] - predictedVision[i]);
        }
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled) {
        curiosityLevel = noveltyLevel * 2.0f + std::abs(predictionError) * 0.5f
                        + 0.6f * curiosityLevel;
        curiosityLevel = std::clamp(curiosityLevel, 먹튀f, 1.0f);
    }
}

} // namespace nlm
