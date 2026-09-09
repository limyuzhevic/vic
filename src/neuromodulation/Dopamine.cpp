#include "Dopamine.hpp"
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

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Dopamine at 0.0 = no modulation (factor = 1.0)
    // Dopamine at 1.0 = maximum modulation (factor = 1.5)
    // Middle values scale linearly
    return 1.0f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay toward baseline
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + pImpl->decayRate * static_cast<float>(dt));
    }
}

void Dopamine::signalReward(float reward) {
    // Reward signaling: rapid increase toward peak proportional to reward magnitude
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling: increases for positive error, decreases for negative
    float delta = error * pImpl->releaseRate;
    pImpl->level = std::clamp(pImpl->level + delta, 0.0f, pImpl->peak);
}

void Dopamine::applyToNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Dopamine modulates neural excitability by adjusting effective current injection
    // Higher dopamine increases excitability (lower effective threshold)
    float excitabilityMod = pImpl->level * 0.5f;
    if (excitabilityMod > 0.0f) {
        neuron->injectCurrent(excitabilityMod);
    }
}

void Dopamine::applyToSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Dopamine modulates synaptic plasticity eligibility trace
    // Stronger dopamine creates longer-lasting eligibility traces
    float eligibilityBoost = 1.0f + pImpl->level * 2.0f;  // Boost factor
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace * eligibilityBoost);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
    if (pImpl->level < pImpl->baseline) {
        pImpl->level = pImpl->baseline;
    }
}

bool Dopamine::isActive() const {
    return pImpl->level > pImpl->baseline;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::max(0.0f, rate);
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

} // namespace nlm
