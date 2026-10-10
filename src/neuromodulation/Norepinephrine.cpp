#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float stressLevel;
    float focusGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f),
             arousalLevel(0.5f), stressLevel(0.0f), focusGain(1.0f) {
        history.clear();
    }
    
    std::vector<float> history;
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

const char* Norepinephrine::getEffectType() const {
    return "Arousal";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    updateArousalState();
}

void Norepinephrine::signalStress(float stress) {
    pImpl->stressLevel = std::clamp(stress, 0.0f, 1.0f);
    
    float response = pImpl->stressLevel * 0.6f;
    pImpl->level += response;
    
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
    
    NLM_LOG_INFO(std::string("Norepinephrine stress response: +") + std::to_string(response) +
                " (stress=" + std::to_string(stress) + ")");
}

void Norepinephrine::signalFocus(float focus) {
    float focusBoost = focus * 0.4f;
    pImpl->level += focusBoost;
    
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
    
    pImpl->focusGain = std::min(pImpl->focusGain, 1.0f + focus * 0.5f);
    
    NLM_LOG_INFO(std::string("Norepinephrine focus boost: +") + std::to_string(focusBoost) +
                " (focus=" + std::to_string(focus) + ")");
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getFocusGain() const {
    return pImpl->focusGain;
}

float Norepinephrine::getPlasticityFactor() const {
    if (pImpl->stressLevel > 0.5f) {
        return 0.5f * (1.0f + pImpl->level * 0.5f);
    } else {
        float normalized = pImpl->level / pImpl->peak;
        return 0.8f + 0.5f * normalized;
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateArousalState();
    
    if (pImpl->stressLevel > 0.0f) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.005f * static_cast<float>(dt));
    }
    
    if (pImpl->focusGain > 1.0f) {
        pImpl->focusGain = std::max(1.0f, pImpl->focusGain - 0.01f * static_cast<float>(dt));
    }
}

void Norepinephrine::updateArousalState() {
    float normalized = pImpl->level / pImpl->peak;
    pImpl->arousalLevel = 0.3f + 0.7f * normalized;
}

void Norepinephrine::habituate(float amount) {
    if (amount > 0.0f) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - amount * 0.5f);
    }
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->history.clear();
}

const std::vector<float>& Norepinephrine::getHistory() const {
    return pImpl->history;
}

void Norepinephrine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm