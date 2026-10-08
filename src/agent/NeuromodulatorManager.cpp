// NeuromodulatorManager.cpp
#include "NeuromodulatorManager.h"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

NeuromodulatorManager::NeuromodulatorManager(std::shared_ptr<Brain> brain, AgentBrain* parent)
    : brain_(brain)
    , parent_(parent)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , cachedPlasticityFactor_(0.0f)
    , plasticityCacheValid_(false) {
}

NeuromodulatorManager::~ NeuromodulatorManager() = default;

void NeuromodulatorManager::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Invalidate cache
    plasticityCacheValid_ = false;
}

void NeuromodulatorManager::update(double timestep) {
    // Update development system
    if (developmentEnabled_) {
        updateDevelopmentEffects(timestep);
    }
    
    // Apply structural plasticity if enabled
    if (structuralPlasticityEnabled_) {
        applyStructuralPlasticityEffects();
    }
    
    // Invalidate cache when state changes
    plasticityCacheValid_ = false;
}

void NeuromodulatorManager::calculateNovelty(const std::vector<float>& vision) {
    if (vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    // Calculate novelty as normalized difference from previous vision
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize and decay
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    previousVision_ = vision;
}

void NeuromodulatorManager::calculateCuriosity() {
    if (!curiosityEnabled_) {
        curiosityLevel_ = 0.0f;
        return;
    }
    
    // Curiosity combines novelty with prediction error
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void NeuromodulatorManager::updateDopamine(float predictionError) {
    if (!rewardModulationEnabled_) {
        dopamineLevel_ = 0.0f;
        return;
    }
    
    // Dopamine signal is based on prediction error
    predictionError_ = predictionError;
    dopamineLevel_ = calculateDopamineFromPredictionError(predictionError_);
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
}

float NeuromodulatorManager::calculateDopamineFromPredictionError(float error) const {
    // Neuroscience-based dopamine response to prediction error
    // Positive RPE -> burst, Negative RPE -> dip
    if (error > 0.0f) {
        // Positive prediction error: reward prediction burst
        // Scaled by error magnitude
        return std::min(1.0f, error * 2.0f); // Cap at 1.0
    } else {
        // Negative prediction error: reward prediction dip
        // Scales with absolute error, can't go below -1.0
        return std::max(-1.0f, error * 1.5f);
    }
}

void NeuromodulatorManager::applyRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_ || !brain_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Update dopamine based on prediction error
    updateDopamine(predictionError_);
    
    // Apply neuromodulation to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Update STDP weights based on neuromodulatory state
    if (auto* stdp = brain_->getSTDP()) {
        stdp->setLTPWeight(0.01f * getIntegratedPlasticityFactor());
        stdp->setLTDWeight(0.012f * getIntegratedPlasticityFactor());
    }
}

float NeuromodulatorManager::getIntegratedPlasticityFactor() const {
    if (!plasticityCacheValid_) {
        cachedPlasticityFactor_ = calculatePlasticityFromNeuromodulators();
        plasticityCacheValid_ = true;
    }
    return cachedPlasticityFactor_;
}

float NeuromodulatorManager::calculatePlasticityFromNeuromodulators() const {
    // Consolidated plasticity factor calculation
    // Combines dopamine, development, and baseline effects
    
    // Base plasticity from development
    float plasticity = plasticityModifier_;
    
    // Dopamine enhancement: positive dopamine increases plasticity, negative decreases
    // Using neuroscience-based relationship: phasic-to-tonic conversion
    float dopamineEffect = 0.5f + 0.5f * dopamineLevel_; // 0.0 to 1.0 range
    dopamineEffect = std::clamp(dopamineEffect, 0.1f, 2.0f);
    
    // Apply neuromodulatory effects
    plasticity *= dopamineEffect;
    
    return std::clamp(plasticity, 0.1f, 2.0f);
}

float NeuromodulatorManager::getCuriosityExplorationModifier() const {
    // Returns exploration multiplier based on curiosity level
    if (!curiosityEnabled_) return 1.0f;
    
    // Higher curiosity = more exploration
    // Range: 1.0f to 1.5f (up to 50% more exploration)
    return 1.0f + curiosityLevel_ * 0.5f;
}

void NeuromodulatorManager::updateDevelopmentEffects(double timestep) {
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
    }
    
    // Update brain developmental stage
    if (parent_) {
        parent_->setDevelopmentalStageFromAge(developmentalAge_);
    }
    
    // Invalidate cache when plasticity changes
    plasticityCacheValid_ = false;
}

void NeuromodulatorManager::applyStructuralPlasticityEffects() {
    if (!brain_ || !parent_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Synaptogenesis and pruning rates scale with plasticity
        float synRate = 0.0001f * plasticityModifier_;
        float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

void NeuromodulatorManager::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Invalidate cache
    plasticityCacheValid_ = false;
}

} // namespace nlm
