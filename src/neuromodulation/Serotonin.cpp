#include "../neuromodulation/Serotonin.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodBoost;
    float modulationRate;
    float impulsivityRate;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.0f), moodBoost(1.0f), modulationRate(0.2f), impulsivityRate(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood, attention, and impulsivity
    return 0.9f + 0.1f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->modulationRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Enhance mood based on positive experiences
    pImpl->level = std::min(1.0f, pImpl->level + mood * pImpl->moodBoost);
    pImpl->history.push_back(pImpl->level);
}

void Serotonin::signalImpulsivity(float drive) {
    // Increase impulsivity when something is motivating
    pImpl->level = std::min(1.0f, pImpl->level + drive * pImpl->impulsivityRate);
    pImpl->history.push_back(pImpl->level);
}

float Serotonin::getModulationRate() const {
    return pImpl->modulationRate;
}

void Serotonin::setModulationRate(float rate) {
    pImpl->modulationRate = rate;
}

const std::vector<float>& Serotonin::getHistory() const {
    return pImpl->history;
}

void Serotonin::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
