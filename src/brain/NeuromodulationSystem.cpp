// NeuromodulationSystem.cpp - Implementation of the neuromodulation system

#include "NeuromodulationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

NeuromodulationSystem::NeuromodulationSystem()
    : dopamineLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , predictionError_(0.0f)
    , plasticityFactor_(1.0f)
    , lastUpdateTime_(0.0)
    , learningRate_(0.01f)
    , explorationRate_(0.1f)
    , predictionErrorSensitivity_(1.0f)
    , noveltyThreshold_(0.5f) {
}

void NeuromodulationSystem::initialize(BrainImpl* brain) {
    brain_ = brain;
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize NeuromodulationSystem with null brain");
        return;
    }
    
    // Initialize neuromodulation subsystems
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    
    // Initialize subsystems with brain reference
    dopamine_->initialize(brain_);
    curiosity_->initialize(brain_);
    novelty_->initialize(brain_);
    predictionError_->initialize(brain_);
    
    // Initialize signal history
    predictionHistory_.reserve(100);
    rewardHistory_.reserve(100);
    
    NLM_LOG_INFO("Neuromodulation system initialized successfully");
}

void NeuromodulationSystem::update(double timestep) {
    lastUpdateTime_ += timestep;
    
    if (!brain_) return;
    
    // Update neuromodulation subsystems
    dopamine_->update(timestep);
    curiosity_->update(timestep);
    novelty_->update(timestep);
    predictionError_->update(timestep);
    
    // Compute integrated motivation
    computeIntegratedMotivation();
    
    // Update motivational drives
    updateMotivationalDrives(timestep);
    
    // Apply neuromodulation effects
    applyNeuromodulationToPlasticity();
    
    // Adapt to environment if needed
    adaptToEnvironment(std::vector<float>()); // Would pass sensory input in real scenario
}

void NeuromodulationSystem::processReward(float reward, float predictedReward) {
    predictionError_ = reward - predictedReward;
    
    // Store in history
    predictionHistory_.push_back(predictedReward);
    rewardHistory_.push_back(reward);
    
    // Keep history size manageable
    if (predictionHistory_.size() > 100) {
        predictionHistory_.erase(predictionHistory_.begin());
        rewardHistory_.erase(rewardHistory_.begin());
    }
    
    // Update dopamine signal
    if (dopamine_) {
        dopamine_->updatePredictionError(predictionError_);
    }
    
    // Compute curiosity based on prediction error
    curiosityLevel_ = std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void NeuromodulationSystem::processNovelty(const std::vector<float>& sensoryInput) {
    if (!novelty_) return;
    
    float noveltyScore = novelty_->computeNovelty(sensoryInput);
    noveltyLevel_ = noveltyScore;
    
    // Apply novelty-based learning if above threshold
    if (noveltyScore > noveltyThreshold_) {
        // Increase plasticity for novel experiences
        plasticityFactor_ = std::min(plasticutyFactor_ * 1.2f, 2.0f);
    } else {
        // Decay plasticity
        plasticityFactor_ = std::max(plasticutyFactor_ * 0.95f, 0.1f);
    }
}

void NeuromodulationSystem::processCuriosity(double uncertainty, double surprise) {
    if (!curiosity_) return;
    
    curiosity_->updateUncertainty(uncertainty);
    curiosity_->updateSurprise(surprise);
    
    curiosityLevel_ = curiosity_->getCuriosityLevel();
}

void NeuromodulationSystem::processPredictionError(float prediction, float outcome) {
    if (!predictionError_) return;
    
    predictionError_->updatePredictionError(prediction, outcome);
    predictionError_ = predictionError_->getPredictionErrorLevel();
}

void NeuromodulationSystem::applyDopamineEffects() {
    if (!dopamine_ || !brain_) return;
    
    // Apply dopamine effects on neural excitability
    float dopamineLevel = dopamine_->getLevel();
    
    for (auto& region : brain_->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Dopamine modulates excitability by injecting additional current
                float excitabilityMod = dopamineLevel * 0.5f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void NeuromodulationSystem::applyCuriosityEffects() {
    // Curiosity-driven exploration
    if (curiosityLevel_ > explorationRate_) {
        // Trigger exploratory behavior
        if (brain_) {
            // Would trigger exploration in a real implementation
            // For now, just log the effect
            NLM_LOG_INFO("Curiosity-driven exploration triggered");
        }
    }
}

void NeuromodulationSystem::applyNoveltyEffects() {
    if (noveltyLevel_ > noveltyThreshold_) {
        // Novelty promotes learning and plasticity
        plasticityFactor_ = std::min(plasticutyFactor_ * 1.1f, 2.0f);
        
        // Would trigger new memory formation in real implementation
        NLM_LOG_INFO("Novelty-driven learning activated");
    }
}

void NeuromodulationSystem::applyPredictionErrorEffects() {
    // Prediction error drives learning and adjustment
    float errorInfluence = std::abs(predictionError_) * predictionErrorSensitivity_;
    
    // Apply to plasticity systems
    if (brain_ && brain_->stdp) {
        // Modify STDP learning rates based on prediction error
        float currentLTP = brain_->stdp->getLTPWeight();
        float currentLTD = brain_->stdp->getLTDWeight();
        
        brain_->stdp->setLTPWeight(currentLTP * (1.0f + errorInfluence * 0.1f));
        brain_->stdp->setLTDWeight(currentLTD * (1.0f + errorInfluence * 0.1f));
    }
}

void NeuromodulationSystem::updateMotivationalDrives(double timestep) {
    // Decay inactive drives
    for (auto& pair : motivationalDrives_) {
        pair.second *= (1.0f - timestep * 0.01f); // Decay over time
    }
    
    // Remove drives with very low strength
    motivationalDrives_.erase(
        std::remove_if(motivationalDrives_.begin(), motivationalDrives_.end(),
                      [](const auto& pair) { return pair.second < 0.01f; }),
        motivationalDrives_.end()
    );
}

void NeuromodulationSystem::addMotivationalDrive(const std::string& drive, float strength) {
    motivationalDrives_[drive] = strength;
    activeDrives_.push_back(drive);
}

float NeuromodulationSystem::getMotivationalDrive(const std::string& drive) const {
    auto it = motivationalDrives_.find(drive);
    return it != motivationalDrives_.end() ? it->second : 0.0f;
}

void NeuromodulationSystem::configure(float learningRate, float explorationRate,
                                       float predictionErrorSensitivity, float noveltyThreshold) {
    setLearningRate(learningRate);
    setExplorationRate(explorationRate);
    setPredictionErrorSensitivity(predictionErrorSensitivity);
    setNoveltyDetectionThreshold(noveltyThreshold);
}

void NeuromodulationSystem::computeIntegratedMotivation() {
    // Combine all neuromodulatory signals into integrated motivation
    float totalMotivation = 0.0f;
    
    // Weighted combination of neuromodulators
    totalMotivation += dopamine_->getLevel() * 0.4f;
    totalMotivation += curiosityLevel_ * 0.3f;
    totalMotivation += noveltyLevel_ * 0.2f;
    totalMotivation += std::abs(predictionError_) * predictionErrorSensitivity_ * 0.1f;
    
    // Apply to plasticity factor
    plasticityFactor_ = std::clamp(totalMotivation, 0.1f, 2.0f);
}

void NeuromodulationSystem::applyNeuromodulationToPlasticity() {
    // Apply neuromodulatory signals to plasticity systems
    if (!brain_ || !brain_->stdp || !brain_->dopamine) return;
    
    float dopamineLevel = brain_->dopamine->getLevel();
    float plasticityMod = 0.5f + 0.5f * dopamineLevel;
    plasticityMod = std::clamp(plasticityMod, 0.1f, 2.0f);
    
    // Scale STDP learning rates
    brain_->stdp->setLTPWeight(0.01f * plasticityMod);
    brain_->stdp->setLTDWeight(0.012f * plasticityMod);
}

void NeuromodulationSystem::updateLearningRates() {
    // Adapt learning rates based on recent performance
    if (rewardHistory_.size() >= 10) {
        // Compute average recent reward
        float avgReward = std::accumulate(rewardHistory_.end() - 10, rewardHistory_.end(), 0.0f) / 10.0f;
        
        // Adjust learning rate based on reward stability
        if (avgReward < 0.0f) {
            learningRate_ *= 0.9f; // Reduce learning on negative reward
        } else {
            learningRate_ = std::min(learningRate_ * 1.05f, 0.1f); // Increase learning on positive reward
        }
    }
}

void NeuromodulationSystem::adaptToEnvironment(const std::vector<float>& environmentSignals) {
    // Simple environment adaptation based on recent performance
    updateLearningRates();
    
    // Would implement more sophisticated adaptation in real scenario
    // including reward prediction, value learning, etc.
}

void NeuromodulationSystem::resetDriveStates() {
    motivationalDrives_.clear();
    activeDrives_.clear();
    lastUpdateTime_ = 0.0;
}

void NeuromodulationSystem::evaluateDriveConflicts() {
    // Check for conflicting motivational drives
    if (motivationalDrives_.size() > 1) {
        // Compute drive priorities
        float totalPriority = computeDrivePriorities();
        
        if (totalPriority > 1.5f) {
            // Conflict detected - resolve by prioritizing higher-value drives
            NLM_LOG_INFO("Motivational drive conflict detected and resolved");
        }
    }
}

void NeuromodulationSystem::resolveDriveConflicts() {
    // Simple conflict resolution by scaling down weaker drives
    if (motivationalDrives_.size() > 1) {
        float totalStrength = 0.0f;
        for (const auto& pair : motivationalDrives_) {
            totalStrength += pair.second;
        }
        
        if (totalStrength > 2.0f) {
            // Scale down all drives proportionally
            float scaleFactor = 2.0f / totalStrength;
            for (auto& pair : motivationalDrives_) {
                pair.second *= scaleFactor;
            }
        }
    }
}

float NeuromodulationSystem::computeDrivePriorities() const {
    if (motivationalDrives_.empty()) return 0.0f;
    
    float totalPriority = 0.0f;
    for (const auto& pair : motivationalDrives_) {
        totalPriority += pair.second;
    }
    
    return totalPriority;
}

} // namespace nlm
