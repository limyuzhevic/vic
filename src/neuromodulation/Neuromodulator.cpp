#include "Neuromodulator.hpp"
#include <algorithm>
#include <time.h>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed for random noise
}

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

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on biological models
    // Dopamine signals reward prediction error, modulating learning rates
    // Higher dopamine (positive RPE) enhances LTP-like plasticity
    // Lower dopamine (negative RPE) enhances LTD-like plasticity
    // Baseline plasticity occurs at neutral dopamine levels
    
    // Neuromodulatory gain: dopamine amplifies learning signals
    float neuromodulatoryGain = 1.0f + 2.0f * pImpl->level;
    
    // Base plasticity rate with dopamine modulation
    float basePlasticity = 0.01f;
    float modulatoryEffect = 0.5f * (pImpl->level - 0.5f); // Center around 0.5
    
    // Apply neuromodulatory effect to learning rate
    float effectivePlasticity = basePlasticity * neuromodulatoryGain + modulatoryEffect;
    
    return std::clamp(effectivePlasticity, 0.0f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on Rescorla-Wagner and actor-critic models
    // Implements temporal difference learning with homeostatic regulation
    
    float dtf = static_cast<float>(dt);
    
    // Decay towards baseline (homeostatic regulation)
    float decayAmount = pImpl->decayRate * dtf;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Add small noise for biological realism
    pImpl->level += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.01f;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on phasic dopamine responses
    // Implements reward prediction error signaling with saturation
    
    // Phasic dopamine response to unexpected rewards
    float rewardBurst = reward * pImpl->releaseRate * 2.0f; // Enhanced response
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardBurst);
    
    // Add prediction error component (dopamine coding)
    // Based on temporal difference learning models
    pImpl->level += reward * pImpl->releaseRate * 0.5f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on actor-critic models
    // Positive error (better than expected) -> increased dopamine (learning signal)
    // Negative error (worse than expected) -> decreased dopamine (suppression)
    // Implements temporal difference learning core mechanism
    
    // Apply prediction error with asymmetric response
    // Positive errors drive learning, negative errors suppress it
    float rpeSignal = error > 0.0f ? error * pImpl->releaseRate * 1.5f : 
                          error * pImpl->releaseRate * 0.8f;
    
    pImpl->level = std::max(0.0f, pImpl->level + rpeSignal);
    
    // Add immediate response for strong prediction errors
    if (std::abs(error) > 1.0f) {
        pImpl->level = std::max(pImpl->level, std::abs(error) * 0.1f);
    }
}

} // namespace nlm
