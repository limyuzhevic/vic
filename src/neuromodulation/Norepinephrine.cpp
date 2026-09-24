#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float arousal;
    float vigilance;
    float stress;
    float attention;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , arousal(0.5f)
        , vigilance(0.5f)
        , stress(0.0f)
        , attention(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update dependent factors
    pImpl->arousal = 0.3f + 0.7f * pImpl->level;
    pImpl->vigilance = 0.4f + 0.6f * pImpl->level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity and learning under stress
    return 0.5f + 0.5f * pImpl->level * (1.0f + pImpl->stress);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: stress response and adaptation
    // Stress increases NE level, but adaptation occurs
    if (pImpl->stress > 0.5f) {
        float stressResponse = std::min(1.0f, pImpl->stress * 2.0f);
        pImpl->level = std::min(1.0f, pImpl->level + stressResponse * 0.1f * static_cast<float>(dt));
    } else {
        // Natural decay towards baseline
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
    }
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stress = std::clamp(stressLevel, 0.0f, 1.0f);
    setLevel(std::min(1.0f, pImpl->level + stressLevel * 0.3f));
    NLM_LOG_INFO(std::string("Norepinephrine stress: ") + std::to_string(stressLevel));
}

void Norepinephrine::signalAttention(float attentionLevel) {
    pImpl->attention = std::clamp(attentionLevel, 0.0f, 1.0f);
    setLevel(std::min(1.0f, pImpl->level + attentionLevel * 0.2f));
    NLM_LOG_INFO(std::string("Norepinephrine attention: ") + std::to_string(attentionLevel));
}

} // namespace nlm
