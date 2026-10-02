#include "Neuromodulator.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {
        // Validate initial parameters
        if (decayRate < 0.0f) {
            throw std::invalid_argument("Dopamine decay rate cannot be negative");
        }
        if (releaseRate < 0.0f) {
            throw std::invalid_argument("Dopamine release rate cannot be negative");
        }
        if (peak < 0.0f) {
            throw std::invalid_argument("Dopamine peak level cannot be negative");
        }
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {
    // Validate that initialization succeeded
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate Dopamine implementation");
    }
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    if (!pImpl) {
        throw std::runtime_error("Dopamine implementation is not initialized");
    }
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    if (!pImpl) {
        throw std::runtime_error("Dopamine implementation is not initialized");
    }
    
    // Validate input range
    if (level < 0.0f || level > 1.0f) {
        throw std::invalid_argument("Dopamine level must be between 0.0 and 1.0");
    }
    
    pImpl->level = level;
}

float Dopamine::getPlasticityFactor() const {
    if (!pImpl) {
        throw std::runtime_error("Dopamine implementation is not initialized");
    }
    
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // Current placeholder implementation provides reasonable approximation
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    if (!pImpl) {
        throw std::runtime_error("Dopamine implementation is not initialized");
    }
    
    // Validate timestep
    if (dt <= 0.0) {
        throw std::invalid_argument("Timestep duration must be positive");
    }
    
    // TODO PHASE 2: Implement real dopamine dynamics
    // Current placeholder implementation provides stable decay behavior
    float newLevel = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = newLevel;
}

void Dopamine::signalReward(float reward) {
    if (!pImpl) {
        throw std::runtime_error("Dopamine implementation is not initialized");
    }
    
    // Validate reward signal
    if (reward < 0.0f) {
        throw std::invalid_argument("Reward signal cannot be negative");
    }
    
    // TODO PHASE 2: Implement real reward signaling
    // Current placeholder implementation provides reasonable approximation
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    if (!pImpl) {
        throw std::runtime_error("Dopamine implementation is not initialized");
    }
    
    // Validate prediction error
    if (std::isnan(error) || std::isinf(error)) {
        throw std::invalid_argument("Prediction error must be a finite number");
    }
    
    // TODO PHASE 2: Implement reward prediction error signaling
    // Current placeholder implementation provides reasonable approximation
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
