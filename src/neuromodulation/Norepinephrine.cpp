#include "Norepinephrine.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    Brain* brain;
    float arousalSensitivity;
    float vigilanceGain;
    std::vector<float> noveltySignature;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f), brain(nullptr),
             arousalSensitivity(1.2f), vigilanceGain(1.3f) {}
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
    // NE enhances plasticity for salient/novel events
    return 0.2f + 0.8f * getLevel();
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (brain) {
        pImpl->baseline = 0.05f;
    }
}

void Norepinephrine::signalNovelty(const std::vector<float>& input) {
    // Calculate novelty based on input statistics
    float mean = std::accumulate(input.begin(), input.end(), 0.0f) / input.size();
    float variance = 0.0f;
    for (float val : input) {
        variance += (val - mean) * (val - mean);
    }
    variance /= input.size();
    
    float novelty = std::sqrt(variance) / (mean + 0.1f);  // Normalize by mean
    pImpl->noveltySignature = input;
    
    // Increase NE level based on novelty
    setLevel(std::min(1.0f, getLevel() + novelty * pImpl->releaseRate * 0.8f));
}

void Norepinephrine::increaseArousal(float salience) {
    // NE increases arousal based on stimulus salience
    setLevel(std::min(1.0f, getLevel() + salience * pImpl->releaseRate * 1.0f));
}

float Norepinephrine::getArousalModulation() const {
    // NE directly modulates arousal/alertness
    return 1.0f + pImpl->arousalSensitivity * getLevel();
}

float Norepinephrine::getAttentionModulation() const {
    // NE enhances alertness and selective attention
    return 1.0f + pImpl->vigilanceGain * getLevel();
}

} // namespace nlm