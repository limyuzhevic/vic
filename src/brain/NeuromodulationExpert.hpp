#pragma once

#include "NeuromodulationExpert.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <map>
#include <cmath>

namespace nlm {

// ========== NEUROMODULATION EXPERT ==========

NeuromodulationExpert::NeuromodulationExpert(std::shared_ptr<Brain> brain)
    : brain_(brain), neuromodulationEnabled_(true), globalGain_(1.0f) {
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    LOG_INFO("NeuromodulationExpert initialized");
}

NeuromodulationExpert::~NeuromodulationExpert() {
    LOG_INFO("NeuromodulationExpert destroyed");
}

void NeuromodulationExpert::configureDopamineDynamics(float synthesisRate,
                                                     float decayRate,
                                                     float baseline,
                                                     float amplitude) {
    dopamineConfig_.synthesisRate = synthesisRate;
    dopamineConfig_.decayRate = decayRate;
    dopamineConfig_.baseline = baseline;
    dopamineConfig_.amplitude = amplitude;
    
    LOG_INFO("Configured dopamine dynamics: synthesisRate={}, decayRate={}, baseline={}, amplitude={}", 
             synthesisRate, decayRate, baseline, amplitude);
}

void NeuromodulationExpert::configureAChDynamics(float synthesisRate,
                                                 float decayRate,
                                                 float baseline,
                                                 float amplitude) {
    achConfig_.synthesisRate = synthesisRate;
    achConfig_.decayRate = decayRate;
    achConfig_.baseline = baseline;
    achConfig_.amplitude = amplitude;
    
    LOG_INFO("Configured ACh dynamics: synthesisRate={}, decayRate={}, baseline={}, amplitude={}", 
             synthesisRate, decayRate, baseline, amplitude);
}

void NeuromodulationExpert::configureNEDynamics(float synthesisRate,
                                                 float decayRate,
                                                 float baseline,
                                                 float amplitude) {
    neConfig_.synthesisRate = synthesisRate;
    neConfig_.decayRate = decayRate;
    neConfig_.baseline = baseline;
    neConfig_.amplitude = amplitude;
    
    LOG_INFO("Configured NE dynamics: synthesisRate={}, decayRate={}, baseline={}, amplitude={}", 
             synthesisRate, decayRate, baseline, amplitude);
}

void NeuromodulationExpert::configure5HTDynamics(float synthesisRate,
                                                 float decayRate,
                                                 float baseline,
                                                 float amplitude) {
    serotoninConfig_.synthesisRate = synthesisRate;
    serotoninConfig_.decayRate = decayRate;
    serotoninConfig_.baseline = baseline;
    serotoninConfig_.amplitude = amplitude;
    
    LOG_INFO("Configured 5-HT dynamics: synthesisRate={}, decayRate={}, baseline={}, amplitude={}", 
             synthesisRate, decayRate, baseline, amplitude);
}

void NeuromodulationExpert::configureCuriositySystem(float noveltyThreshold,
                                                    float explorationRate,
                                                    float adaptationRate,
                                                    float maxExploration,
                                                    float curiosityGain) {
    curiosityConfig_.noveltyThreshold = noveltyThreshold;
    curiosityConfig_.explorationRate = explorationRate;
    curiosityConfig_.adaptationRate = adaptationRate;
    curiosityConfig_.maxExploration = maxExploration;
    curiosityConfig_.curiosityGain = curiosityGain;
    
    LOG_INFO("Configured curiosity system: threshold={}, explorationRate={}, adaptationRate={}, maxExploration={}, gain={}", 
             noveltyThreshold, explorationRate, adaptationRate, maxExploration, curiosityGain);
}

void NeuromodulationExpert::configureNoveltyDetection(float sensitivity,
                                                     float adaptationRate,
                                                     float maxNovelty,
                                                     float decayRate,
                                                     float noveltyGain) {
    noveltyConfig_.sensitivity = sensitivity;
    noveltyConfig_.adaptationRate = adaptationRate;
    noveltyConfig_.maxNovelty = maxNovelty;
    noveltyConfig_.decayRate = decayRate;
    noveltyConfig_.noveltyGain = noveltyGain;
    
    LOG_INFO("Configured novelty detection: sensitivity={}, adaptationRate={}, maxNovelty={}, decayRate={}, gain={}", 
             sensitivity, adaptationRate, maxNovelty, decayRate, noveltyGain);
}

void NeuromodulationExpert::configureRewardPrediction(float learningRate,
                                                     float predictionAccuracy,
                                                     float errorDecayRate,
                                                     float maxPrediction,
                                                     float rewardGain,
                                                     float errorGain) {
    rewardConfig_.learningRate = learningRate;
    rewardConfig_.predictionAccuracy = predictionAccuracy;
    rewardConfig_.errorDecayRate = errorDecayRate;
    rewardConfig_.maxPrediction = maxPrediction;
    rewardConfig_.rewardGain = rewardGain;
    rewardConfig_.errorGain = errorGain;
    
    LOG_INFO("Configured reward prediction: learningRate={}, predictionAccuracy={}, errorDecayRate={}, maxPrediction={}, rewardGain={}, errorGain={}", 
             learningRate, predictionAccuracy, errorDecayRate, maxPrediction, rewardGain, errorGain);
}

void NeuromodulationExpert::enableCrossSystemCoupling(bool enable) {
    crossSystemCouplingEnabled_ = enable;
    LOG_INFO("Cross-system neuromodulation coupling {}", enable ? "enabled" : "disabled");
}

void NeuromodulationExpert::configureCrossSystemCoupling(const std::vector<std::pair<std::string, float>>& couplings) {
    crossSystemCouplings_.clear();
    for (const auto& coupling : couplings) {
        crossSystemCouplings_[coupling.first] = coupling.second;
    }
    
    LOG_INFO("Configured cross-system coupling with {} couplings", couplings.size());
}

void NeuromodulationExpert::setNeuromodulatorLevel(const std::string& name, float level) {
    // Normalize level to [0, 1]
    float normalizedLevel = std::max(0.0f, std::min(1.0f, level / globalGain_));
    
    // Apply level to neuromodulator
    if (name == "dopamine") {
        dopamineLevel_ = normalizedLevel;
    } else if (name == "acetylcholine" || name == "ACh") {
        achLevel_ = normalizedLevel;
    } else if (name == "noradrenaline" || name == "NE") {
        neLevel_ = normalizedLevel;
    } else if (name == "serotonin" || name == "5-HT") {
        serotoninLevel_ = normalizedLevel;
    }
    
    LOG_INFO("Set neuromodulator {} level to {} (normalized: {})", name, level, normalizedLevel);
}

void NeuromodulationExpert::addNeuromodulatorLevel(const std::string& name, float delta) {
    // Get current level
    float currentLevel = 0.0f;
    if (name == "dopamine") {
        currentLevel = dopamineLevel_;
    } else if (name == "acetylcholine" || name == "ACh") {
        currentLevel = achLevel_;
    } else if (name == "noradrenaline" || name == "NE") {
        currentLevel = neLevel_;
    } else if (name == "serotonin" || name == "5-HT") {
        currentLevel = serotoninLevel_;
    }
    
    // Apply delta and clamp
    float newLevel = std::max(0.0f, std::min(globalGain_, currentLevel + delta));
    setNeuromodulatorLevel(name, newLevel);
}

void NeuromodulationExpert::signalReward(float reward) {
    rewardHistory_.push_back(reward);
    if (rewardHistory_.size() > maxHistorySize_) {
        rewardHistory_.pop_front();
    }
    
    // Calculate reward prediction error
    float predictionError = reward - predictedReward_;
    predictedReward_ = reward;
    
    // Update reward prediction
    rewardPrediction_ += rewardConfig_.learningRate * predictionError;
    
    LOG_INFO("Signaled reward: {}, prediction error: {}", reward, predictionError);
}

void NeuromodulationExpert::signalPredictionError(float error) {
    errorHistory_.push_back(error);
    if (errorHistory_.size() > maxHistorySize_) {
        errorHistory_.pop_front();
    }
    
    // Update reward prediction based on error
    rewardPrediction_ *= (1.0f - error * rewardConfig_.errorGain_);
    
    LOG_INFO("Signaled prediction error: {}", error);
}

void NeuromodulationExpert::signalNovelty(float novelty) {
    noveltyHistory_.push_back(novelty);
    if (noveltyHistory_.size() > maxHistorySize_) {
        noveltyHistory_.pop_front();
    }
    
    // Update novelty-based exploration
    explorationLevel_ = std::min(1.0f, explorationLevel_ + novelty * curiosityConfig_.curiosityGain_);
    
    LOG_INFO("Signaled novelty: {}", novelty);
}

void NeuromodulationExpert::updateNeuromodulation(float dt) {
    if (!neuromodulationEnabled_) return;
    
    // Update each neuromodulator
    updateDopamine(dt);
    updateACh(dt);
    updateNE(dt);
    updateSerotonin(dt);
    
    // Apply cross-system coupling
    if (crossSystemCouplingEnabled_) {
        applyCrossSystemCoupling(dt);
    }
}

void NeuromodulationExpert::updateDopamine(float dt) {
    float targetLevel = calculateTargetDopamineLevel();
    dopamineLevel_ = dopamineLevel_ + (targetLevel - dopamineLevel_) * dopamineConfig_.decayRate * dt;
    dopamineLevel_ = std::max(0.0f, std::min(globalGain_, dopamineLevel_));
}

void NeuromodulationExpert::updateACh(float dt) {
    float targetLevel = calculateTargetAChLevel();
    achLevel_ = achLevel_ + (targetLevel - achLevel_) * achConfig_.decayRate * dt;
    achLevel_ = std::max(0.0f, std::min(globalGain_, achLevel_));
}

void NeuromodulationExpert::updateNE(float dt) {
    float targetLevel = calculateTargetNELevel();
    neLevel_ = neLevel_ + (targetLevel - neLevel_) * neConfig_.decayRate * dt;
    neLevel_ = std::max(0.0f, std::min(globalGain_, neLevel_));
}

void NeuromodulationExpert::updateSerotonin(float dt) {
    float targetLevel = calculateTargetSerotoninLevel();
    serotoninLevel_ = serotoninLevel_ + (targetLevel - serotoninLevel_) * serotoninConfig_.decayRate * dt;
    serotoninLevel_ = std::max(0.0f, std::min(globalGain_, serotoninLevel_));
}

float NeuromodulationExpert::getNeuromodulatorLevel(const std::string& name) const {
    if (name == "dopamine") {
        return dopamineLevel_;
    } else if (name == "acetylcholine" || name == "ACh") {
        return achLevel_;
    } else if (name == "noradrenaline" || name == "NE") {
        return neLevel_;
    } else if (name == "serotonin" || name == "5-HT") {
        return serotoninLevel_;
    }
    return 0.0f;
}

float NeuromodulationExpert::calculateTargetDopamineLevel() const {
    // Calculate target based on reward prediction error and curiosity
    float predictionError = std::abs(rewardPrediction_ - predictedReward_);
    float curiosityFactor = noveltyHistory_.empty() ? 0.0f : noveltyHistory_.back();
    
    float target = dopamineConfig_.baseline + 
                  predictionError * dopamineConfig_.amplitude * rewardConfig_.rewardGain_ +
                  curiosityFactor * dopamineConfig_.amplitude * curiosityConfig_.curiosityGain_;
    
    return std::max(0.0f, std::min(globalGain_, target));
}

float NeuromodulationExpert::calculateTargetAChLevel() const {
    // ACh level increases with novelty and attention demands
    float noveltyFactor = noveltyHistory_.empty() ? 0.0f : noveltyHistory_.back() * noveltyConfig_.sensitivity;
    
    float target = achConfig_.baseline + noveltyFactor * achConfig_.amplitude;
    return std::max(0.0f, std::min(globalGain_, target));
}

float NeuromodulationExpert::calculateTargetNELevel() const {
    // NE level increases with arousal and stress
    float rewardFactor = rewardHistory_.empty() ? 0.0f : rewardHistory_.back();
    
    float target = neConfig_.baseline + rewardFactor * neConfig_.amplitude * rewardConfig_.rewardGain_;
    return std::max(0.0f, std::min(globalGain_, target));
}

float NeuromodulationExpert::calculateTargetSerotoninLevel() const {
    // 5-HT level increases with punishment and decreases with reward
    float errorFactor = errorHistory_.empty() ? 0.0f : errorHistory_.back();
    
    float target = serotoninConfig_.baseline + errorFactor * serotoninConfig_.amplitude * rewardConfig_.errorGain_;
    return std::max(0.0f, std::min(globalGain_, target));
}

void NeuromodulationExpert::applyCrossSystemCoupling(float dt) {
    for (const auto& coupling : crossSystemCouplings_) {
        float currentLevel = getNeuromodulatorLevel(coupling.first);
        float targetLevel = currentLevel * coupling.second;
        
        // Gradually move toward target level
        float delta = (targetLevel - currentLevel) * coupling.second * dt;
        setNeuromodulatorLevel(coupling.first, currentLevel + delta);
    }
}

void NeuromodulationExpert::enableNeuromodulation(bool enable) {
    neuromodulationEnabled_ = enable;
    LOG_INFO("Neuromodulation system {}", enable ? "enabled" : "disabled");
}

void NeuromodulationExpert::getNeuromodulationStats() const {
    LOG_INFO("Neuromodulation statistics:",
             "Dopamine: {}", dopamineLevel_,
             "ACh: {}", achLevel_,
             "NE: {}", neLevel_,
             "5-HT: {}", serotoninLevel_,
             "Exploration: {}", explorationLevel_,
             "Reward Prediction: {}", rewardPrediction_);
}

void NeuromodulationExpert::optimizeNeuromodulation() {
    // Optimize neuromodulation parameters based on performance
    LOG_INFO("Optimizing neuromodulation parameters");
    
    // Adjust decay rates based on system stability
    adjustDecayRates();
    
    // Adjust baseline levels based on average activity
    adjustBaselineLevels();
}

void NeuromodulationExpert::validateNeuromodulation() const {
    // Validate neuromodulation configuration
    if (dopamineConfig_.synthesisRate < 0.0f || dopamineConfig_.synthesisRate > 1.0f) {
        LOG_ERROR("Invalid dopamine synthesis rate: {}", dopamineConfig_.synthesisRate);
    }
    if (achConfig_.synthesisRate < 0.0f || achConfig_.synthesisRate > 1.0f) {
        LOG_ERROR("Invalid ACh synthesis rate: {}", achConfig_.synthesisRate);
    }
    if (neConfig_.synthesisRate < 0.0f || neConfig_.synthesisRate > 1.0f) {
        LOG_ERROR("Invalid NE synthesis rate: {}", neConfig_.synthesisRate);
    }
    if (serotoninConfig_.synthesisRate < 0.0f || serotoninConfig_.synthesisRate > 1.0f) {
        LOG_ERROR("Invalid 5-HT synthesis rate: {}", serotoninConfig_.synthesisRate);
    }
}

void NeuromodulationExpert::exportNeuromodulationConfig(const std::string& filename) const {
    LOG_INFO("Exporting neuromodulation config to {}", filename);
    
    // Implementation would export to file
}

void NeuromodulationExpert::importNeuromodulationConfig(const std::string& filename) {
    LOG_INFO("Importing neuromodulation config from {}", filename);
    
    // Implementation would import from file
}

void NeuromodulationExpert::visualizeNeuromodulation() const {
    LOG_INFO("Visualizing neuromodulation dynamics");
    
    // Implementation would create visualizations
}

void NeuromodulationExpert::adjustDecayRates() {
    // Adjust decay rates based on system stability
    float avgActivity = getAverageActivityLevel();
    
    if (avgActivity > 0.8f) {
        // System is highly active, increase decay
        dopamineConfig_.decayRate = std::min(0.99f, dopamineConfig_.decayRate * 1.1f);
        achConfig_.decayRate = std::min(0.99f, achConfig_.decayRate * 1.1f);
    } else if (avgActivity < 0.2f) {
        // System is less active, decrease decay
        dopamineConfig_.decayRate = std::max(0.5f, dopamineConfig_.decayRate * 0.9f);
        achConfig_.decayRate = std::max(0.5f, achConfig_.decayRate * 0.9f);
    }
}

void NeuromodulationExpert::adjustBaselineLevels() {
    // Adjust baseline levels based on average activity
    float avgActivity = getAverageActivityLevel();
    
    float targetBaseline = 0.5f * avgActivity;
    dopamineConfig_.baseline = targetBaseline;
    achConfig_.baseline = targetBaseline * 0.8f;
    neConfig_.baseline = targetBaseline * 1.2f;
}

float NeuromodulationExpert::getAverageActivityLevel() const {
    // Calculate average activity from brain system
    if (!brain_) return 0.5f;
    
    return brain_->getAverageFiringRate();
}

// ========== NEUROMODULATOR CONFIG STRUCTURES ==========

struct NeuromodulationExpert::DopamineConfig {
    float synthesisRate = 0.1f;
    float decayRate = 0.95f;
    float baseline = 0.0f;
    float amplitude = 1.0f;
};

struct NeuromodulationExpert::AChConfig {
    float synthesisRate = 0.1f;
    float decayRate = 0.95f;
    float baseline = 0.0f;
    float amplitude = 1.0f;
};

struct NeuromodulationExpert::NEConfig {
    float synthesisRate = 0.1f;
    float decayRate = 0.95f;
    float baseline = 0.0f;
    float amplitude = 1.0f;
};

struct NeuromodulationExpert::SerotoninConfig {
    float synthesisRate = 0.1f;
    float decayRate = 0.95f;
    float baseline = 0.0f;
    float amplitude = 1.0f;
};

struct NeuromodulationExpert::CuriosityConfig {
    float noveltyThreshold = 0.5f;
    float explorationRate = 0.1f;
    float adaptationRate = 0.01f;
    float maxExploration = 1.0f;
    float curiosityGain = 1.0f;
};

struct NeuromodulationExpert::NoveltyConfig {
    float sensitivity = 1.0f;
    float adaptationRate = 0.01f;
    float maxNovelty = 1.0f;
    float decayRate = 0.9f;
    float noveltyGain = 1.0f;
};

struct NeuromodulationExpert::RewardConfig {
    float learningRate = 0.01f;
    float predictionAccuracy = 0.8f;
    float errorDecayRate = 0.9f;
    float maxPrediction = 1.0f;
    float rewardGain = 1.0f;
    float errorGain = 1.0f;
};

} // namespace nlm