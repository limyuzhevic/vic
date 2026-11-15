#include "../neuromodulation/Acetylcholine.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float consolidationRate;
    float attentionBoost;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.0f), consolidationRate(0.5f), attentionBoost(1.2f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances consolidation and attention
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->consolidationRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float salience) {
    // Boost attention when something is salient
    pImpl->level = std::min(1.0f, pImpl->level + salience * pImpl->attentionBoost);
    pImpl->history.push_back(pImpl->level);
}

float Acetylcholine::getConsolidationRate() const {
    return pImpl->consolidationRate;
}

void Acetylcholine::setConsolidationRate(float rate) {
    pImpl->consolidationRate = rate;
}

const std::vector<float>& Acetylcholine::getHistory() const {
    return pImpl->history;
}

void Acetylcholine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
