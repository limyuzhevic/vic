#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Also update baseline to track recent level
    pImpl->baseline = pImpl->baseline * 0.9f + pImpl->level * 0.1f;
}

float Dopamine::getPlasticityFactor() const {
    // Improved plasticity factor based on dopamine level
    // Dopamine modulates plasticity in a U-shaped curve: low levels decrease plasticity, optimal levels maximize it
    if (pImpl->level < 0.2f) {
        // Low dopamine: reduced plasticity
        return 0.3f + 0.7f * (pImpl->level / 0.2f);
    } else {
        // High dopamine: enhanced plasticity up to optimum, then decreases to avoid runaway
        float optimalLevel = 0.7f;
        float deviation = std::abs(pImpl->level - optimalLevel);
        return std::max(0.3f, 1.0f - deviation * 2.0f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Improved dopamine dynamics with exponential decay
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Add small random fluctuations to simulate biological noise
    if (pImpl->level < 1.0f) {
        float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.01f;
        pImpl->level = std::clamp(pImpl->level + noise, pImpl->baseline, pImpl->peak);
    }
}

void Dopamine::signalReward(float reward) {
    // Improved reward signaling with saturating dynamics
    float rewardBoost = reward * pImpl->releaseRate;
    pImpl->peak = std::max(pImpl->peak, pImpl->level + rewardBoost * 0.5f);
    
    // Strong burst response to unexpected rewards
    float burstResponse = std::min(1.0f, reward * 2.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstResponse * 0.3f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Improved prediction error signaling based on temporal difference learning
    if (error > 0.0f) {
        // Positive prediction error (better than expected): stronger burst response
        float potentiation = error * pImpl->releaseRate * 0.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + potentiation);
        
        // Update peak for future learning
        pImpl->peak = std::max(pImpl->peak, pImpl->level);
    } else if (error < 0.0f) {
        // Negative prediction error (worse than expected): mild depression
        float depression = std::abs(error) * pImpl->releaseRate * 0.3f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - depression);
    }
}

// Set decay rate based on current level
void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 1.0f);
}

// Get decay rate
float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

// Set release rate for reward signaling
void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::clamp(rate, 0.0f, 10.0f);
}

// Get release rate
float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

// Set peak dopamine level
void Dopamine::setPeak(float peak) {
    pImpl->peak = std::clamp(peak, 0.0f, 1.0f);
}

// Get peak dopamine level
float Dopamine::getPeak() const {
    return pImpl->peak;
}

// Set baseline dopamine level
void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

// Get baseline dopamine level
float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

// Reset dopamine to baseline level
void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->peak = std::max(pImpl->peak, pImpl->baseline);
}

} // namespace nlm
