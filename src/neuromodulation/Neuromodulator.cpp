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
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

float Dopamine::getPlasticityFactor() const {
    // Realistic dopamine-modulated plasticity factor
    // Dopamine has complex effects on plasticity:
    // 1. Mildly elevated dopamine increases LTP probability
    // 2. Strongly elevated dopamine can decrease LTP (inverted U)
    // 3. Low dopamine impairs LTP
    // 4. Baseline activity supports normal plasticity
    
    // Inverted-U function models dopamine's non-linear effects:
    float normalized = pImpl->level;
    
    if (normalized < 0.3f) {
        // Low dopamine: impaired plasticity
        return 0.3f + 1.5f * normalized;
    } else if (normalized < 0.6f) {
        // Moderate dopamine: optimal plasticity
        return 0.6f + 0.5f * normalized;
    } else {
        // High dopamine: reduced plasticity (inverted U)
        return 0.9f - 0.5f * normalized;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics with multiple time constants
    // Dopamine exhibits both phasic and tonic activity with different kinetics
    // 
    // Components:
    // 1. Phasic signals: Fast transients for specific events
    // 2. Tonic activity: Slow baseline maintained by cholinergic input
    // 3. Baseline: Expected value based on learned reward statistics
    // 4. Adaptation: Changing baseline based on experience
    
    // Phasic decay: Fast return towards baseline after events
    float phasicDecay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - phasicDecay);
    
    // Slow adaptation of baseline based on recent experience
    // This implements reward prediction error learning at the system level
    float adaptationRate = 0.001f * static_cast<float>(dt);
    float predictionError = pImpl->peak - pImpl->baseline;
    if (std::abs(predictionError) > 0.01f) {
        pImpl->baseline += predictionError * adaptationRate;
        pImpl->baseline = std::clamp(pImpl->baseline, 0.0f, 1.0f);
    }
    
    // Add small noise for biological realism
    if (brain_ && brain_->getRandomGenerator()) {
        float noise = (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) - 0.5f) * 0.01f;
        pImpl->level += noise;
    }
}

void Dopamine::signalReward(float reward) {
    // Reward prediction error signaling
    // Dopamine neurons encode reward prediction error (RPE)
    // RPE = actual reward - predicted reward
    
    // In realistic models:
    // - Phasic bursts on unexpected rewards (positive RPE)
    // - Phasic dips on omitted expected rewards (negative RPE)
    // - Tonic baseline maintained through cholinergic input
    
    // Simplified RPE model with biological plausibility:
    // 1. Error signal magnitude proportional to prediction error
    // 2. Bounded response to prevent runaway excitation
    // 3. Realistic time constant for dopamine dynamics
    
    float predictionError = reward - pImpl->baseline;
    
    if (predictionError > 0.0f) {
        // Reward-prediction positive error: phasic burst
        // Reset level towards prediction, then add burst
        pImpl->level = std::max(pImpl->baseline, 
                               pImpl->level + predictionError * pImpl->releaseRate * 2.0f);
    } else if (predictionError < -0.1f) {
        // Reward-prediction negative error: phasic dip
        // Reduce level below baseline but maintain minimum
        pImpl->level = std::max(0.0f, 
                               pImpl->level + predictionError * pImpl->releaseRate);
    }
    
    // Add some noise for biological realism
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
    pImpl->level += noise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (strict RPE model)
    // This is the canonical dopamine response to prediction errors
    // 
    // Biological principles:
    // - Response magnitude proportional to absolute prediction error
    // - Phasic increases for positive errors (unexpected rewards)
    // - Phasic decreases for negative errors (omitted rewards)
    // - Predictable, reliable responses based on task statistics
    // 
    // Algorithm:
    // 1. Compute signed prediction error
    // 2. Apply appropriate scaling based on error magnitude
    // 3. Update dopamine level with biological constraints
    // 4. Enforce reasonable bounds
    
    // Apply error with biological plausibility
    pImpl->level = std::max(0.0f, 
                           std::min(1.0f, 
                                   pImpl->level + error * pImpl->releaseRate * 1.5f));
    
    // Add some biological noise for realism
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
    pImpl->level += noise;
}

} // namespace nlm
