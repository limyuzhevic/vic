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

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = peak;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

// Implement Hodgkin-Huxley model for more realistic dopamine dynamics
void Dopamine::updateWithHodgkinHuxley(TimestepDuration dt, float input) {
    // TODO PHASE 3: Implement full HH model for dopamine neurons
    // For now, use simpler leaky integrator with HH-like dynamics
    
    // Subthreshold dynamics (membrane potential integration)
    pImpl->level = pImpl->level + (input - pImpl->level) * pImpl->decayRate * static_cast<float>(dt);
    
    // Spike generation and reset (simplified HH)
    if (pImpl->level > 0.9f) {
        // Spike detected
        pImpl->level = 0.0f;  // Reset
        pImpl->level = pImpl->peak;  // Repolarization
    }
}

// Implement dopamine burst-pause dynamics
void Dopamine::updateWithBurstPause(TimestepDuration dt, bool rewardReceived, float predictionError) {
    if (rewardReceived) {
        // Burst response: high phasic dopamine
        pImpl->level = pImpl->peak;
    } else if (predictionError != 0.0f) {
        // Phasic response to prediction error
        pImpl->level = std::max(0.0f, pImpl->level + predictionError * pImpl->releaseRate);
    } else {
        // Baseline decay
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
}

// Implement dopamine concentration dynamics with realistic time constants
void Dopamine::updateWithConcentrationDynamics(TimestepDuration dt, float extracellularInput) {
    // Two-compartment model: intracellular and extracellular
    static float intracellular = 0.0f;
    static float extracellular = 0.0f;
    
    // Intracellular dynamics (synthesis, release)
    intracellular += (extracellularInput - intracellular) * 0.1f * static_cast<float>(dt);
    
    // Release from intracellular to extracellular
    extracellular += (intracellular * pImpl->releaseRate * static_cast<float>(dt)) - (extracellular * pImpl->decayRate * static_cast<float>(dt));
    
    // Update level
    pImpl->level = std::clamp(extracellular, 0.0f, 1.0f);
}
