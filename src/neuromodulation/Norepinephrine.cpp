#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalLevel;
    float vigilanceFactor;
    float burstFiringRate;
    float tonicReleaseRate;
    SimulationStep lastBurstTime;
    float tonicFiringRate;
    bool isBursting;
    
    Impl() : level(0.0f), baseline(0.05f), arousalLevel(1.0f), 
             vigilanceFactor(1.2f), burstFiringRate(5.0f),
             tonicReleaseRate(0.8f), lastBurstTime(0), tonicFiringRate(0.3f),
             isBursting(false) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine enhances plasticity during arousal
    float arousalBoost = pImpl->isBursting ? pImpl->burstFiringRate : 1.0f;
    return 0.5f + 0.5f * pImpl->level * arousalLevel * arousalBoost;
}

void Norepinephrine::update(TimestepDuration dt, SimulationStep currentStep) {
    // Simulate locus coeruleus firing dynamics
    float tonicInput = pImpl->tonicReleaseRate;
    
    // Burst firing during significant events (approx. every 100-200 steps)
    if (currentStep - pImpl->lastBurstTime > 100 + static_cast<size_t>(pImpl->level * 100)) {
        pImpl->isBursting = true;
        pImpl->lastBurstTime = currentStep;
    } else {
        pImpl->isBursting = false;
    }
    
    if (pImpl->isBursting) {
        // Burst firing increases norepinephrine level significantly
        pImpl->level = std::min(1.0f, pImpl->level + pImpl->burstFiringRate * dt);
    } else {
        // Tonic decay towards baseline
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level - pImpl->baseline * pImpl->tonicFiringRate * static_cast<float>(dt));
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::setArousalLevel(float level) {
    pImpl->arousalLevel = std::max(0.1f, std::min(3.0f, level));
}

float Norepinephrine::getVigilanceFactor() const {
    return pImpl->vigilanceFactor;
}

void Norepinephrine::setVigilanceFactor(float factor) {
    pImpl->vigilanceFactor = std::max(0.5f, std::min(4.0f, factor));
}

float Norepinephrine::getBurstFiringRate() const {
    return pImpl->burstFiringRate;
}

void Norepinephrine::setBurstFiringRate(float rate) {
    pImpl->burstFiringRate = std::max(1.0f, std::min(10.0f, rate));
}

float Norepinephrine::getTonicReleaseRate() const {
    return pImpl->tonicReleaseRate;
}

void Norepinephrine::setTonicReleaseRate(float rate) {
    pImpl->tonicReleaseRate = std::max(0.1f, std::min(2.0f, rate));
}

bool Norepinephrine::isInBurstMode() const {
    return pImpl->isBursting;
}

float Norepinephrine::computeExcitabilityModulation(NeuronId neuron, float baselineExcitability) const {
    // NE modulates neural excitability based on arousal
    float modulation = 1.0f;
    
    if (pImpl->isBursting) {
        // Burst firing dramatically increases excitability
        modulation *= pImpl->burstFiringRate * 0.5f;
    } else {
        // Tonic NE increases excitability moderately
        modulation *= (1.0f + pImpl->level * pImpl->arousalLevel * pImpl->vigilanceFactor);
    }
    
    return baselineExcitability * modulation;
}

} // namespace nlm