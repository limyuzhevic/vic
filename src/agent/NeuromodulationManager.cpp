#include "NeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

NeuromodulationManager::NeuromodulationManager(const NeuromodulationConfig& config) 
    : dopamineLevel_(0.0f), noveltyLevel_(0.0f), curiosityLevel_(0.0f),
      predictionError_(0.0f), expectedReward_(0.0f), currentTime_(0.0),
      config_(config) {
    
    NLM_LOG_DEBUG("NeuromodulationManager initialized");
}

NeuromodulationManager::~NeuromodulationManager() {
    // Clean up active signals
    activeSignals_.clear();
}

void NeuromodulationManager::update(double timestep) {
    currentTime_ += timestep;
    
    // Decay active signals
    decayActiveSignals(timestep);
    
    // Remove expired signals
    auto it = std::remove_if(activeSignals_.begin(), activeSignals_.end(),
        [this](const NeuromodulatorSignal& signal) {
            return signal.duration <= 0.0f;
        });
    activeSignals_.erase(it, activeSignals_.end());
    
    NLM_LOG_DEBUG("NeuromodulationManager updated: dopamine=" +
                 std::to_string(dopamineLevel_) + 
                 ", novelty=" + std::to_string(noveltyLevel_) +
                 ", curiosity=" + std::to_string(curiosityLevel_));
}

void NeuromodulationManager::processReward(float reward, float predictedReward) {
    if (!config_.enableRewardModulation_) {
        return;
    }
    
    // Compute prediction error
    computeRewardPredictionError(reward, predictedReward);
    
    // Update expected reward
    updateExpectedReward(reward);
    
    // Generate dopamine signal
    generateDopamineSignal();
    
    // Apply signal effects
    applySignalEffects();
}

void NeuromodulationManager::processNovelty(const std::vector<float>& currentInput, 
                                            const std::vector<std::vector<float>>& history) {
    if (!config_.enableNoveltyDetection_) {
        return;
    }
    
    // Compute novelty level
    computeNoveltyLevel(currentInput);
    
    // Update history
    if (!history.empty()) {
        noveltyHistory_ = history;
    } else {
        updateNoveltyHistory(currentInput);
    }
    
    // Generate curiosity signal
    generateCuriositySignal();
    
    // Apply signal effects
    applySignalEffects();
}

void NeuromodulationManager::computeRewardPredictionError(float reward, float predictedReward) {
    predictionError_ = reward - predictedReward;
    
    // Clamp to reasonable range
    predictionError_ = std::clamp(predictionError_, -1.0f, 1.0f);
    
    NLM_LOG_DEBUG("Reward prediction error: " + std::to_string(predictionError_));
}

void NeuromodulationManager::updateExpectedReward(float reward) {
    expectedReward_ = config_.rewardPredictionDecay_ * expectedReward_ +
                     (1.0f - config_.rewardPredictionDecay_) * reward;
    
    NLM_LOG_DEBUG("Expected reward: " + std::to_string(expectedReward_));
}

void NeuromodulationManager::generateDopamineSignal() {
    // Compute dopamine effect based on prediction error
    float dopamineEffect = calculateDopamineEffect();
    
    // Apply dopamine gain
    dopamineLevel_ = dopamineEffect * config_.dopamineGain_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Add dopamine signal to active signals
    addSignal(NeuromodulatorType::Dopamine, dopamineLevel_, 10.0f);
    
    NLM_LOG_DEBUG("Dopamine level: " + std::to_string(dopamineLevel_));
}

void NeuromodulationManager::computeNoveltyLevel(const std::vector<float>& currentInput) {
    if (currentInput.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    for (float value : currentInput) {
        totalDiff += std::abs(value - getLastInputValue(value));
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / std::max<size_t>(currentInput.size(), 1);
    
    // Apply threshold
    if (noveltyLevel_ < config_.noveltyThreshold_) {
        noveltyLevel_ = 0.0f;
    }
    
    // Decay
    noveltyLevel_ *= config_.noveltyDecayRate_;
    
    NLM_LOG_DEBUG("Novelty level: " + std::to_string(noveltyLevel_));
}

void NeuromodulationManager::updateNoveltyHistory(const std::vector<float>& input) {
    // Keep only recent history
    const size_t maxHistory = config_.noveltyHistorySize_;
    
    if (noveltyHistory_.size() >= maxHistory) {
        noveltyHistory_.erase(noveltyHistory_.begin());
    }
    
    noveltyHistory_.push_back(input);
}

void NeuromodulationManager::generateCuriositySignal() {
    if (!config_.enableCuriosityDriven_) {
        return;
    }
    
    // Calculate curiosity based on novelty and prediction error
    float curiosityEffect = calculateNoveltyEffect();
    float errorEffect = std::abs(predictionError_) * config_.predictionErrorWeight_;
    
    curiosityLevel_ = (curiosityEffect + errorEffect) * config_.curiosityGain_;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    
    // Add curiosity signal
    addSignal(NeuromodulatorType::Curiosity, curiosityLevel_, 5.0f);
    
    NLM_LOG_DEBUG("Curiosity level: " + std::to_string(curiosityLevel_));
}

void NeuromodulationManager::addSignal(NeuromodulatorType type, float strength, float duration) {
    // Only add significant signals
    if (std::abs(strength) < 0.001f) {
        return;
    }
    
    NeuromodulatorSignal signal;
    signal.type = type;
    signal.strength = strength;
    signal.duration = duration;
    signal.timestamp = currentTime_;
    
    activeSignals_.push_back(signal);
    
    NLM_LOG_DEBUG("Added neuromodulator signal: type=" +
                 std::to_string(static_cast<int>(type)) +
                 ", strength=" + std::to_string(strength) +
                 ", duration=" + std::to_string(duration));
}

void NeuromodulationManager::decayActiveSignals(double timestep) {
    for (auto& signal : activeSignals_) {
        // Simple linear decay
        signal.duration -= timestep;
        
        // Also scale strength based on remaining duration
        if (signal.duration > 0.0f) {
            float ageRatio = 1.0f - (signal.duration / signal.duration); // Simplified
            signal.strength *= (1.0f - ageRatio * 0.1f);
        }
    }
}

void NeuromodulationManager::applySignalEffects() {
    // Calculate current neuromodulatory effects
    float totalDopamineEffect = 0.0f;
    float totalNoveltyEffect = 0.0f;
    
    for (const auto& signal : activeSignals_) {
        switch (signal.type) {
            case NeuromodulatorType::Dopamine:
                totalDopamineEffect += signal.strength;
                break;
            case NeuromodulatorType::Curiosity:
                totalNoveltyEffect += signal.strength;
                break;
            default:
                break;
        }
    }
    
    // Apply effects to state variables
    dopamineLevel_ = totalDopamineEffect;
    noveltyLevel_ = std::min(1.0f, totalNoveltyEffect);
    
    NLM_LOG_DEBUG("Applied signal effects: totalDopamine=" +
                 std::to_string(totalDopamineEffect) +
                 ", totalNovelty=" + std::to_string(totalNoveltyEffect));
}

float NeuromodulationManager::calculateDopamineEffect() const {
    return std::clamp(predictionError_ * 0.5f, -1.0f, 1.0f);
}

float NeuromodulationManager::calculateNoveltyEffect() const {
    return noveltyLevel_ * 0.5f + std::abs(predictionError_) * 0.3f;
}

void NeuromodulationManager::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    currentTime_ = 0.0;
    
    noveltyHistory_.clear();
    activeSignals_.clear();
    
    NLM_LOG_DEBUG("NeuromodulationManager reset");
}

bool NeuromodulationManager::isValid() const {
    if (config_.dopamineGain_ < 0.0f || config_.dopamineGain_ > 10.0f) {
        return false;
    }
    
    if (config_.curiosityGain_ < 0.0f || config_.curiosityGain_ > 10.0f) {
        return false;
    }
    
    if (config_.noveltyDecayRate_ < 0.0f || config_.noveltyDecayRate_ > 1.0f) {
        return false;
    }
    
    return true;
}

float NeuromodulationManager::calculatePlasticityModifier() const {
    // Base plasticity modifier
    float plasticity = 1.0f;
    
    // Apply dopamine effect
    if (std::abs(dopamineLevel_) > 0.1f) {
        float dopamineEffect = config_.plasticityGain_ * std::abs(dopamineLevel_);
        plasticity *= (1.0f + dopamineEffect);
    }
    
    // Apply novelty effect
    if (noveltyLevel_ > config_.noveltyThreshold_) {
        float noveltyEffect = config_.plasticityGain_ * noveltyLevel_;
        plasticity *= (1.0f + noveltyEffect);
    }
    
    // Apply curiosity effect
    if (curiosityLevel_ > config_.explorationThreshold_) {
        float curiosityEffect = config_.plasticityGain_ * curiosityLevel_ * 0.5f;
        plasticity *= (1.0f + curiosityEffect);
    }
    
    // Clamp to reasonable range
    plasticity = std::clamp(plasticity, 0.1f, 3.0f);
    
    return plasticity;
}

float NeuromodulationManager::getExpectedReward() const {
    return expectedReward_;
}

float NeuromodulationManager::getLastInputValue(float current) const {
    if (!noveltyHistory_.empty()) {
        // Get the most recent input (simplified - should be per-modality)
        return noveltyHistory_.back()[0];
    }
    return current;
}

} // namespace nlm