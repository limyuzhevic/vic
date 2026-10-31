#pragma once

#include "AgentBrainCore.hpp"
#include "../brain/Brain.hpp"
#include <cmath>

namespace nlm {

class AgentBrainLearning {
public:
    AgentBrainLearning(AgentBrainCore* parent) : parent_(parent) {}
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Update prediction error
    void updatePredictionError(float reward, float predictedReward);
    
    // Update dopamine level
    void updateDopamineLevel();
    
    // Update expected reward
    void updateExpectedReward(float reward);
    
    // Apply reward-modulated weight changes
    void applyRewardModulatedWeightChanges();
    
    // Compute prediction error
    float computePredictionError(float reward, float predictedReward) const;
    
    // Compute neuromodulation level
    float computeNeuromodulationLevel() const;
    
    // Get dopamine level
    float getDopamineLevel() const { return parent_->dopamineLevel_; }
    
    // Set dopamine level
    void setDopamineLevel(float level) { parent_->dopamineLevel_ = level; }
    
    // Get prediction error
    float getPredictionError() const { return parent_->predictionError_; }
    
    // Set prediction error
    void setPredictionError(float error) { parent_->predictionError_ = error; }
    
    // Get expected reward
    float getExpectedReward() const { return parent_->expectedReward_; }
    
    // Set expected reward
    void setExpectedReward(float reward) { parent_->expectedReward_ = reward; }
    
    // Get developmental age
    double getDevelopmentalAge() const { return parent_->developmentalAge_; }
    
    // Increment developmental age
    void incrementDevelopmentalAge(double timestep) { parent_->developmentalAge_ += timestep; }
    
    // Get plasticity modifier
    float getPlasticityModifier() const { return parent_->plasticityModifier_; }
    
    // Set plasticity modifier
    void setPlasticityModifier(float modifier) { parent_->plasticityModifier_ = modifier; }
    
    // Update development stages
    void updateDevelopmentStages();
    
    // Apply structural plasticity
    void applyStructuralPlasticity(double timestep);
    
private:
    AgentBrainCore* parent_;
};

inline void AgentBrainLearning::applyRewardModulation(float reward, float predictedReward) {
    if (!parent_->brain_ || !parent_->rewardModulationEnabled_) return;
    
    // Update prediction error
    parent_->predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    parent_->expectedReward_ = 0.95f * parent_->expectedReward_ + 0.05f * reward;
    
    // Compute neuromodulation level
    parent_->dopamineLevel_ = computePredictionError(reward, predictedReward);
    parent_->dopamineLevel_ = std::clamp(parent_->dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : parent_->brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Reward-modulated weight change
                float delta = eligibility * parent_->dopamineLevel_ * parent_->plasticityModifier_ * 0.05f;
                delta = std::clamp(delta, -0.1f, 0.1f);
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    float plasticityFactor = 0.5f + 0.5f * parent_->dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = parent_->brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

inline void AgentBrainLearning::updateDevelopment(double timestep) {
    if (!parent_->brain_ || !parent_->developmentEnabled_) return;
    
    parent_->developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (parent_->developmentalAge_ < 60.0) {  // ~1 minute
        parent_->plasticityModifier_ = 1.0f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (parent_->developmentalAge_ < 300.0) {  // ~5 minutes
        parent_->plasticityModifier_ = 0.8f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (parent_->developmentalAge_ < 900.0) {  // ~15 minutes
        parent_->plasticityModifier_ = 0.5f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        parent_->plasticityModifier_ = 0.2f;  // Adult - more stable
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (parent_->structuralPlasticityEnabled_) {
        auto* sp = parent_->brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0001f * parent_->plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - parent_->plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

inline void AgentBrainLearning::updatePredictionError(float reward, float predictedReward) {
    parent_->predictionError_ = computePredictionError(reward, predictedReward);
}

inline void AgentBrainLearning::updateDopamineLevel() {
    parent_->dopamineLevel_ = computeNeuromodulationLevel();
    parent_->dopamineLevel_ = std::clamp(parent_->dopamineLevel_, -1.0f, 1.0f);
}

inline void AgentBrainLearning::updateExpectedReward(float reward) {
    parent_->expectedReward_ = 0.95f * parent_->expectedReward_ + 0.05f * reward;
}

inline void AgentBrainLearning::applyRewardModulatedWeightChanges() {
    for (const auto& region : parent_->brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * parent_->dopamineLevel_ * parent_->plasticityModifier_ * 0.05f;
                delta = std::clamp(delta, -0.1f, 0.1f);
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

inline float AgentBrainLearning::computePredictionError(float reward, float predictedReward) const {
    return reward - predictedReward;
}

inline float AgentBrainLearning::computeNeuromodulationLevel() const {
    return parent_->predictionError_;
}

inline void AgentBrainLearning::updateDevelopmentStages() {
    if (parent_->developmentalAge_ < 60.0) {
        parent_->plasticityModifier_ = 1.0f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (parent_->developmentalAge_ < 300.0) {
        parent_->plasticityModifier_ = 0.8f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (parent_->developmentalAge_ < 900.0) {
        parent_->plasticityModifier_ = 0.5f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        parent_->plasticityModifier_ = 0.2f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

inline void AgentBrainLearning::applyStructuralPlasticity(double timestep) {
    if (parent_->structuralPlasticityEnabled_) {
        auto* sp = parent_->brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0001f * parent_->plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - parent_->plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

} // namespace nlm