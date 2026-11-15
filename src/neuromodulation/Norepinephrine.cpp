#include "../neuromodulation/Norepinephrine.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalRate;
    float vigilanceBoost;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.0f), arousalRate(0.3f), vigilanceBoost(1.5f) {}
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
    // Norepinephrine enhances alertness and arousal
    return 0.7f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->arousalRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity) {
    // Increase arousal based on stimulus intensity
    pImpl->level = std::min(1.0f, pImpl->level + intensity * pImpl->arousalRate);
    pImpl->history.push_back(pImpl->level);
}

float Norepinephrine::getArousalRate() const {
    return pImpl->arousalRate;
}

void Norepinephrine::setArousalRate(float rate) {
    pImpl->arousalRate = rate;
}

const std::vector<float>& Norepinephrine::getHistory() const {
    return pImpl->history;
}

void Norepinephrine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
