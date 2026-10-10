#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionalGain;
    float memoryBoost;
    float arousalLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f),
             attentionalGain(1.0f), memoryBoost(1.0f), arousalLevel(0.5f) {
        history.clear();
    }
    
    std::vector<float> history;
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

const char* Acetylcholine::getEffectType() const {
    return "Attention";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    updateAttentionalGain();
}

void Acetylcholine::signalAttention(float salience) {
    float boost = salience * 0.8f;
    pImpl->level += boost;
    
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
    
    NLM_LOG_INFO(std::string("Acetylcholine attentional burst: +") + std::to_string(boost) +
                " (salience=" + std::to_string(salience) + ")");
}

void Acetylcholine::signalMemoryConsolidation(float strength) {
    pImpl->level *= (1.0f + strength * 0.3f);
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
    
    pImpl->memoryBoost = std::min(pImpl->memoryBoost, 1.0f + strength * 0.5f);
}

float Acetylcholine::getAttentionalGain() const {
    return pImpl->attentionalGain;
}

float Acetylcholine::getMemoryBoost() const {
    return pImpl->memoryBoost;
}

float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateAttentionalGain();
    
    if (pImpl->memoryBoost > 1.0f) {
        pImpl->memoryBoost = std::max(1.0f, pImpl->memoryBoost - 0.01f * static_cast<float>(dt));
    }
}

void Acetylcholine::updateAttentionalGain() {
    float normalized = pImpl->level / pImpl->peak;
    pImpl->attentionalGain = 1.0f + 1.5f * std::exp(-4.0f * std::pow(normalized - 0.6f, 2.0f));
}

void Acetylcholine::signalArousal(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
    if (pImpl->arousalLevel > 0.7f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
    }
}

float Acetylcholine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->history.clear();
}

const std::vector<float>& Acetylcholine::getHistory() const {
    return pImpl->history;
}

void Acetylcholine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm