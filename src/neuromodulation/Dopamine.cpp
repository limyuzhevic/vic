#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;           // Current dopamine level (0.0 to 2.0+)
    float plasticityFactor; // Modulates synaptic plasticity (0.0 to 2.0)
    float adaptationRate;   // Rate of synaptic adaptation
    float learningModulation; // Additional learning modulation factor
    
    Impl() : level(0.0f), plasticityFactor(1.0f), adaptationRate(0.01f), learningModulation(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::update(TimestepDuration dt) {
    // Dopamine dynamics: integrates incoming signals and decays over time
    // Implements biological dopamine dynamics with burst-pause patterns
    
    // Decay towards baseline
    float decay = 0.01f * static_cast<float>(dt);
    pImpl->level *= (1.0f - decay);
    pImpl->plasticityFactor *= (1.0f - decay * 0.5f);
    
    // Modulate adaptation rate based on current level
    pImpl->adaptationRate = 0.01f + pImpl->level * 0.005f;
}

void Dopamine::setLevel(float level) {
    // Clamp dopamine level to reasonable biological range
    pImpl->level = std::clamp(level, 0.0f, 3.0f);
    
    // Higher dopamine levels increase plasticity factor
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.3f;
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::setPlasticityFactor(float factor) {
    pImpl->plasticityFactor = std::clamp(factor, 0.0f, 3.0f);
}

void Dopamine::injectReward(float amount) {
    // Dopamine burst in response to unexpected reward (reward prediction error)
    // Implements temporal difference learning signal
    
    // Reward magnitude scaled by current level
    float rewardSignal = amount * (1.0f + pImpl->level * 0.5f);
    setLevel(getLevel() + rewardSignal);
    
    // Short-term adaptation to maintain sensitivity
    pImpl->adaptationVariable += rewardSignal * 0.1f;
}

void Dopamine::injectPunishment(float amount) {
    // Dopamine dip in response to punishment (aversive learning)
    // Implements negative prediction error signal
    
    // Punishment magnitude scaled by current level  
    float punishmentSignal = -amount * (1.0f + std::abs(getLevel()) * 0.3f);
    setLevel(getLevel() + punishmentSignal);
}

void Dopamine::applyToPlasticity(float& ltpWeight, float& ltdWeight) const {
    // Apply dopamine modulation to STDP parameters
    // Dopamine can bidirectionally modulate plasticity
    
    if (pImpl->level > 0.5f) {
        // High dopamine enhances LTP (learning from reward)
        ltpWeight *= pImpl->plasticityFactor;
    } else if (pImpl->level < -0.5f) {
        // Low dopamine enhances LTD (extinction learning)
        ltdWeight *= std::abs(pImpl->plasticityFactor);
    }
    
    // Overall scaling based on adaptation
    ltpWeight *= pImpl->adaptationRate;
    ltdWeight *= pImpl->adaptationRate;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->plasticityFactor = 1.0f;
    pImpl->adaptationRate = 0.01f;
    pImpl->learningModulation = 1.0f;
}

const char* Dopamine::getName() const {
    return "Dopamine";
}

} // namespace nlm
