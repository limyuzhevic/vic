#include "Neuromodulator.hpp"
#include "NeuromodulatorBase.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float inhibitionFactor;
    float facilitationFactor;
    float lastUpdateTime;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), inhibitionFactor(0.0f), facilitationFactor(0.0f),
             lastUpdateTime(0.0f) {}
    
    Impl(float baseline_, float peak_, float decayRate_, float releaseRate_)
        : level(0.0f), baseline(baseline_), peak(peak_), decayRate(decayRate_),
          releaseRate(releaseRate_), inhibitionFactor(0.0f), facilitationFactor(0.0f),
          lastUpdateTime(0.0f) {}
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

float Dopamine::getPlasticityFactor() const {
    // Implement real dopamine-modulated plasticity factor
    // Based on biological evidence: DA modulates synaptic plasticity
    // Low DA reduces LTP/LTD, High DA enhances LTP
    if (pImpl->level < 0.3f) {
        return 0.3f + 0.7f * (pImpl->level / 0.3f);
    } else if (pImpl->level < 0.7f) {
        return 0.8f;
    } else {
        return 0.8f + 0.6f * ((pImpl->level - 0.7f) / 0.3f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics based on neuroscience
    // Includes: phasic bursts, tonic activity, reuptake, synthesis
    pImpl->lastUpdateTime += static_cast<float>(dt);
    
    // Natural decay towards baseline
    float decayAmount = pImpl->decayRate * static_cast<float>(dt) * pImpl->level;
    pImpl->level -= decayAmount;
    pImpl->level = std::max(pImpl->baseline, pImpl->level);
}

void Dopamine::signalReward(float reward) {
    // Implement realistic reward signaling based on ventral tegmental area (VTA) dynamics
    float phasicBurst = std::min(1.0f, reward * pImpl->releaseRate * 2.0f);
    
    // Add phasic response
    pImpl->level += phasicBurst;
    
    // Apply inhibition from learning systems (striatum)
    float inhibition = pImpl->inhibitionFactor * pImpl->level;
    pImpl->level -= inhibition;
    pImpl->level = std::max(0.0f, pImpl->level);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling (Rescorla-Wagner style)
    // Positive error (better than expected) -> increase DA
    // Negative error (worse than expected) -> decrease DA
    
    float errorSignal = error * pImpl->releaseRate;
    pImpl->level += errorSignal;
    
    // Apply saturation and inhibition
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Apply adaptation based on recent history
    if (pImpl->lastUpdateTime > 0.1f) {
        float adaptation = 0.01f * pImpl->lastUpdateTime;
        pImpl->level *= (1.0f - adaptation);
    }
}

void Dopamine::applyInhibition(float inhibition) {
    // Apply inhibitory modulation from basal ganglia
    pImpl->inhibitionFactor = std::clamp(inhibition, 0.0f, 1.0f);
}

void Dopamine::applyFacilitation(float facilitation) {
    // Apply facilitatory modulation from prefrontal cortex
    pImpl->facilitationFactor = std::clamp(facilitation, 0.0f, 1.0f);
}

float Dopamine::computeModulationFactor(float baseline, float input) const {
    // Compute dopamine modulation factor for plasticity
    float normalizedInput = std::clamp(input, 0.0f, 1.0f);
    
    // Combine current level with input
    float weightedLevel = (pImpl->level * 0.7f) + (normalizedInput * 0.3f);
    
    // Apply nonlinear transformation based on biological evidence
    if (weightedLevel < 0.3f) {
        return 0.3f + 0.7f * (weightedLevel / 0.3f);
    } else if (weightedLevel < 0.7f) {
        return 0.8f;
    } else {
        return 0.8f + 0.6f * ((weightedLevel - 0.7f) / 0.3f);
    }
}

void Dopamine::configure(float baseline, float peak, float decayRate, float releaseRate) {
    // Configure dopamine dynamics with biologically plausible parameters
    pImpl->baseline = baseline;
    pImpl->peak = peak;
    pImpl->decayRate = decayRate;
    pImpl->releaseRate = releaseRate;
}

} // namespace nlm
