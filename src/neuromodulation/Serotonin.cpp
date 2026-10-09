#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

SerotoninImpl::SerotoninImpl(const SerotoninConfig& config)
    : config_(config)
    , level_(config.baseline)
    , burstLevel_(0.0f)
    , tonicLevel_(0.0f)
    , recentActivity_(0.0f)
    , rewardPredictionError_(0.0f)
    , punishmentAccumulator_(0.0f) {
    
    // Set initial state
    state_ = SerotoninState::Tonic;
    stateTime_ = 0.0f;
    
    NLM_LOG_INFO("Serotonin neuromodulator initialized with baseline=" + std::to_string(config_.baseline) +
                 ", peak=" + std::to_string(config_.peak));
}

void SerotoninImpl::update(float dt) {
    // Update state
    updateState(dt, recentActivity_);
    
    // Apply state-specific dynamics
    switch (state_) {
        case SerotoninState::Tonic:
            // Tonic firing mode
            level_ = tonicLevel_;
            break;
            
        case SerotoninState::Phasic:
            // Phasic firing mode
            level_ = std::min(config_.peak, level_ + config_.releaseRate * dt);
            break;
            
        case SerotoninState::Burst:
            // Burst firing mode
            level_ = burstLevel_;
            // Decay after burst
            level_ = std::max(config_.baseline, level_ - config_.decayRate * dt);
            break;
            
        case SerotoninState::Depression:
            // Depressive mode
            level_ = std::max(config_.baseline, level_ - config_.decayRate * dt);
            break;
            
        case SerotoninState::Recovery:
            // Recovery mode
            level_ = std::min(config_.peak, level_ + config_.releaseRate * dt);
            break;
    }
    
    // Apply homeostatic regulation
    if (level_ > config_.homeostaticTarget) {
        level_ = std::max(config_.homeostaticTarget, level_ - config_.decayRate * dt);
    } else if (level_ < config_.homeostaticTarget) {
        level_ = std::min(config_.homeostaticTarget, level_ + config_.releaseRate * dt);
    }
    
    // Constrain to valid range
    level_ = std::clamp(level_, config_.baseline, config_.peak);
    
    // Update reward prediction error accumulator
    rewardPredictionError_ *= std::exp(-dt / 100.0f);
    punishmentAccumulator_ *= std::exp(-dt / 100.0f);
    
    NLM_LOG_DEBUG("Serotonin level: " + std::to_string(level_) + 
                 " (state=" + std::to_string(static_cast<int>(state_)) + ")");
}

void SerotoninImpl::signalReward(float reward) {
    // Signal reward affects serotonin positively
    rewardPredictionError_ += reward * config_.rewardPredictionErrorWeight;
    
    // Update tonic level based on reward
    tonicLevel_ = std::min(config_.peak, tonicLevel_ + reward * 0.1f);
    
    // Switch to phasic mode if reward is significant
    if (reward > 0.5f && state_ != SerotoninState::Burst) {
        state_ = SerotoninState::Phasic;
        stateTime_ = 0.0f;
        burstLevel_ = level_;
    }
    
    NLM_LOG_DEBUG("Serotonin received reward: " + std::to_string(reward));
}

void SerotoninImpl::signalPunishment(float punishment) {
    // Signal punishment affects serotonin negatively
    punishmentAccumulator_ += punishment * config_.punishmentWeight;
    
    // Update burst level based on punishment
    burstLevel_ = std::max(config_.baseline, burstLevel_ - punishment * 0.1f);
    
    // Switch to depression mode if punishment is significant
    if (punishment > 0.5f) {
        state_ = SerotoninState::Depression;
        stateTime_ = 0.0f;
    }
    
    NLM_LOG_DEBUG("Serotonin received punishment: " + std::to_string(punishment));
}

void SerotoninImpl::signalRewardPredictionError(float error) {
    // Signal reward prediction error affects serotonin
    rewardPredictionError_ += error * config_.rewardPredictionErrorWeight;
    
    // Switch to phasic mode for prediction error
    if (std::abs(error) > 0.1f) {
        state_ = SerotoninState::Phasic;
        stateTime_ = 0.0f;
        burstLevel_ = level_;
    }
    
    NLM_LOG_DEBUG("Serotonin received reward prediction error: " + std::to_string(error));
}

float SerotoninImpl::getExcitabilityEffect() const {
    // Serotonin generally inhibits neural excitability
    float effect = 1.0f - level_ * config_.neuromodulatorStrength;
    return std::max(0.0f, effect);
}

float SerotoninImpl::getPlasticityEffect() const {
    // Serotonin can modulate plasticity
    // Low serotonin promotes LTP, high serotonin promotes LTD
    if (level_ < config_.homeostaticTarget) {
        // Below target: promote LTP
        return 1.0f + (config_.homeostaticTarget - level_) * config_.neuromodulatorStrength;
    } else {
        // Above target: promote LTD
        return 1.0f - (level_ - config_.homeostaticTarget) * config_.neuromodulatorStrength;
    }
}

float SerotoninImpl::getMemoryEffect() const {
    // Serotonin affects memory consolidation and retrieval
    // Low serotonin enhances memory formation
    float effect = 1.0f - level_ * 0.5f;
    return std::max(0.0f, effect);
}

void SerotoninImpl::updateState(float dt, float activity) {
    // Update state based on activity and time
    stateTime_ += dt;
    
    // Update recent activity for homeostatic regulation
    recentActivity_ = recentActivity_ * 0.9f + activity * 0.1f;
    
    // State transitions based on activity and time
    if (state_ == SerotoninState::Phasic) {
        if (stateTime_ > config_.phasicDuration) {
            // Phasic period ended, return to tonic
            state_ = SerotoninState::Tonic;
            stateTime_ = 0.0f;
        }
    } else if (state_ == SerotoninState::Depression) {
        if (stateTime_ > 200.0f || level_ > config_.homeostaticTarget) {
            // Recovery from depression
            state_ = SerotoninState::Recovery;
            stateTime_ = 0.0f;
        }
    } else if (state_ == SerotoninState::Recovery) {
        if (stateTime_ > 100.0f || level_ > config_.homeostaticTarget - 0.1f) {
            // Recovery complete, return to tonic
            state_ = SerotoninState::Tonic;
            stateTime_ = 0.0f;
        }
    }
    
    // Burst detection
    if (activity > config_.burstThreshold && level_ > config_.burstThreshold) {
        state_ = SerotoninState::Burst;
        stateTime_ = 0.0f;
        burstLevel_ = level_;
    }
}

} // namespace nlm
