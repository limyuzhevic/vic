#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float releaseRate;
    float decayRate;
    float attentionGain;
    float memoryGain;
    float workingMemoryGain;
    float cognitionGain;
    float attentionModulator;
    float memoryModulator;
    float workingMemoryModulator;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.01f)
        , peak(1.0f)
        , releaseRate(0.1f)
        , decayRate(0.05f)
        , attentionGain(1.0f)
        , memoryGain(0.8f)
        , workingMemoryGain(0.9f)
        , cognitionGain(0.7f)
        , attentionModulator(0.0f)
        , memoryModulator(0.0f)
        , workingMemoryModulator(0.0f) {}
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
    updateModulators();
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity: moderate levels enhance LTP, high levels reduce plasticity (stability)
    if (pImpl->level < 0.3f) {
        return 0.5f + 0.5f * (pImpl->level / 0.3f) * 2.0f;  // Low DA: moderate plasticity
    } else {
        return 1.5f - 0.5f * ((pImpl->level - 0.3f) / 0.7f);  // High DA: reduced plasticity
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateModulators();
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, 
                           pImpl->level + reward * pImpl->releaseRate);
    updateModulators();
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error (positive for better than expected)
    if (error > 0.0f) {
        pImpl->level = std::min(pImpl->peak, 
                               pImpl->level + error * pImpl->releaseRate * 0.5f);
    } else if (error < -0.1f) {
        // Significant prediction error reduces dopamine (unexpected bad outcome)
        pImpl->level = std::max(pImpl->baseline, 
                               pImpl->level - std::abs(error) * pImpl->releaseRate * 0.3f);
    }
    updateModulators();
}

float Dopamine::getAttentionModulator() const {
    return pImpl->attentionModulator;
}

float Dopamine::getMemoryModulator() const {
    return pImpl->memoryModulator;
}

float Dopamine::getWorkingMemoryModulator() const {
    return pImpl->workingMemoryModulator;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = peak;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

void Dopamine::setAttentionGain(float gain) {
    pImpl->attentionGain = gain;
    updateModulators();
}

void Dopamine::setMemoryGain(float gain) {
    pImpl->memoryGain = gain;
    updateModulators();
}

void Dopamine::setWorkingMemoryGain(float gain) {
    pImpl->workingMemoryGain = gain;
    updateModulators();
}

void Dopamine::updateModulators() {
    // Calculate neuromodulatory effects based on current dopamine level
    float normalizedLevel = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
    normalizedLevel = std::clamp(normalizedLevel, 0.0f, 1.0f);
    
    // Attention: DA enhances focus and signal salience
    pImpl->attentionModulator = 0.5f + 0.5f * normalizedLevel * pImpl->attentionGain;
    
    // Memory consolidation: DA promotes memory stabilization
    pImpl->memoryModulator = 0.7f + 0.3f * normalizedLevel * pImpl->memoryGain;
    
    // Working memory: DA maintains working memory representations
    pImpl->workingMemoryModulator = 0.6f + 0.4f * normalizedLevel * pImpl->workingMemoryGain;
    
    // General cognition: moderate DA improves cognitive flexibility
    pImpl->cognitionGain = 1.0f - std::abs(0.5f - normalizedLevel) * 0.3f;  // Inverted U-shaped function
}

} // namespace nlm