#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Motor command selection with curiosity/exploration
MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Phase 4: the old code gated exploration behind curiosity > 0.3
    // (decode) and > 0.5 (selectWithCuriosity), thresholds novelty could
    // never reach after step 0, so exploration was dead. Exploration
    // is now proportional to curiosity (see decodeMotorCommand) with
    // no hard gate. Curiosity rises with novelty and prediction error,
    // so exploration responds to the actual environment state.
    float exploreFactor = curiosityLevel_;
    if (exploreFactor <= 0.0f) return defaultCmd;
    // Turn curiosity into an exploration bias: replace the default
    // command with a random action from the remaining 5 — exactly
    // what the old code did, but proportional to curiosity instead of
    // gated. The 0.15 threshold prevents exploration when curiosity is
    // too low to overcome the motor group's intrinsic firing rates.
    if (exploreFactor < 0.15f) return defaultCmd;
    
    std::vector<MotorCommand> candidates;
    for (int i = 0; i < 5; ++i) {
        if (i != static_cast<int>(defaultCmd)) {
            candidates.push_back(static_cast<MotorCommand>(i));
        }
    }
    if (candidates.empty()) return defaultCmd;
    
    return candidates[rand() % candidates.size()];
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_) return;
    
    predictionError_ = reward - predictedReward;
    expectedReward_ = predictedReward;
    
    // STDP reward modulation: strengthen synapses that led to predicted rewards
    // Eligibility traces carry the historical correlation between actions and rewards
    // Reward prediction error drives neuromodulatory systems
    
    if (brain_ && brain_->getSTDP()) {
        brain_->getSTDP()->applyRewardModulation(predictionError_);
    }
    
    // Dopamine response scales with prediction error magnitude
    // Positive error (better than expected) -> dopamine increase
    // Negative error (worse than expected) -> dopamine decrease
    if (brain_ && brain_->getDopamine()) {
        brain_->getDopamine()->update(predictionError_);
    }
    
    // Curiosity system update based on prediction error
    if (brain_ && brain_->getCuriosity()) {
        brain_->getCuriosity()->update(predictionError_);
    }
    
    // Update neuromodulation levels for next step
    if (brain_) {
        dopamineLevel_ = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
        curiosityLevel_ = brain_->getCuriosity() ? brain_->getCuriosity()->getLevel() : 0.0f;
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Developmental stage transitions based on age
    // These stages correspond to different neural development patterns
    DevelopmentalStage oldStage = getDevelopmentalStage();
    
    if (developmentalAge_ < 100.0) {
        setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 500.0) {
        setDevelopmentalStage(DevelopmentalStage::Critical);
    } else if (developmentalAge_ < 1000.0) {
        setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Apply developmental plasticity modifiers
    if (oldStage != getDevelopmentalStage()) {
        // Transition between stages triggers different plasticity regimes
        if (getDevelopmentalStage() == DevelopmentalStage::Critical) {
            // Critical period - high plasticity for learning
            plasticityModifier_ = 2.0f;
        } else if (getDevelopmentalStage() == DevelopmentalStage::Adult) {
            // Adult - stable plasticity
            plasticityModifier_ = 1.0f;
        } else {
            // Other stages - moderate plasticity
            plasticityModifier_ = 1.5f;
        }
    }
    
    // Structural plasticity updates during development
    if (structuralPlasticityEnabled_ && brain_ && brain_->getStructuralPlasticity()) {
        brain_->getStructuralPlasticity()->update(timestep * plasticityModifier_);
    }
    
    NLM_LOG_DEBUG("Development update: age=" + std::to_string(developmentalAge_) + 
                  " stage=" + std::to_string(static_cast<int>(getDevelopmentalStage())) +
                  " plasticity=" + std::to_string(plasticityModifier_));
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (developmentalAge_ < 100.0) return DevelopmentalStage::Initial;
    else if (developmentalAge_ < 500.0) return DevelopmentalStage::Critical;
    else if (developmentalAge_ < 1000.0) return DevelopmentalStage::Maturation;
    else return DevelopmentalStage::Adult;
}

void AgentBrain::setDevelopmentalStage(DevelopmentalStage stage) {
    // This would normally be implemented in the private implementation
    // For now, this is a placeholder to show the concept
    (void)stage;
}

void AgentBrain::reset() {
    // Reset all neuromodulation levels
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset motor group states
    lastWinnerGroup_ = -1;
    lastWinnerMargin_ = 0.0f;
    consecutiveWinnerCount_ = 0;
    stepsSinceTurn_ = 0;
    repeatCount_ = 0;
    lastDecoded_ = MotorCommand::Wait;
    hasLastDecoded_ = false;
    
    // Reset diagnostic counters
    reflexBonusCount_ = 0;
    reflexSuppressCount_ = 0;
    wtaPulseCount_ = 0;
    plannerOverrideCount_ = 0;
    plannerAgreementCount_ = 0;
    plannerDecisionCount_ = 0;
    conceptBiasCount_ = 0;
    selfBiasCount_ = 0;
    socialBiasCount_ = 0;
    
    // Reset percept history
    lastVision_.clear();
    lastTouch_.clear();
    lastText_.clear();
    thoughtPercept_ = "none";
    thoughtPlanned_ = MotorCommand::Wait;
    hasThoughtPlanned_ = false;
    thoughtReward_ = 0.0f;
    hasThoughtOutcome_ = false;
    lastTouch_.clear();
    
    // Reset self-model and social learning states
    selfModelReady_ = false;
    socialReady_ = false;
    
    NLM_LOG_INFO("AgentBrain reset completed");
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

float AgentBrain::getLearnedPreference() const {
    return brain_ ? brain_->getLearnedPreference() : 0.0f;
}

std::string AgentBrain::getThoughtTrace() const {
    std::ostringstream oss;
    oss << "Thought trace: " << thoughtPercept_;
    if (hasThoughtPlanned_) {
        oss << " | Planned: " << static_cast<int>(thoughtPlanned_);
    }
    if (hasThoughtOutcome_) {
        oss << " | Reward: " << thoughtReward_;
    }
    return oss.str();
}

} // namespace nlm