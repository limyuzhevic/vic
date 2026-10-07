#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalBoost;
    float stressModulator;
    
    Impl() : level(0.0f), baseline(0.0f), peak(0.8f), decayRate(0.03f), releaseRate(1.0f), 
             arousalBoost(3.0f), stressModulator(0.5f) {}
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
    // NE modulates plasticity based on arousal state
    // High arousal enhances LTP, low arousal enhances LTD
    if (pImpl->level > 0.5f) {
        return 1.2f + 0.8f * (pImpl->level - 0.5f);  // Enhance LTP
    } else {
        return 0.5f + 0.5f * pImpl->level;  // Enhance LTD for learning from errors
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics include recovery and spontaneous fluctuations
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add small spontaneous fluctuations
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float spontaneous = dist(rng) * 0.001f - 0.0005f;  // Small random walk
    pImpl->level = std::clamp(pImpl->level + spontaneous, pImpl->baseline, 1.0f);
}

void Norepinephrine::signalArousal(float stimulusStrength) {
    // NE increases with novel or salient stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + stimulusStrength * pImpl->arousalBoost);
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress response triggers NE release
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->stressModulator);
}

float Norepinephrine::getArousalBoost() const {
    return pImpl->arousalBoost;
}

} // namespace nlm
