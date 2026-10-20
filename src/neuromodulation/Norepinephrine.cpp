#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float vigilanceFactor;
    float decayRate;
    float attentionBoost;
    std::vector<float> arousalHistory;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , arousalLevel(0.5f)
        , vigilanceFactor(1.0f)
        , decayRate(0.01f)
        , attentionBoost(0.8f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized (arousal & vigilance)");
}

const char* Norepinephrine::getName() const {
    return "Norepinephrine (NE)";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances signal-to-noise ratio and plasticity
    return 1.0f + pImpl->level * pImpl->vigilanceFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: regulates arousal and alertness
    float currentArousal = calculateArousalLevel();
    
    // Smooth update towards target arousal level
    float targetLevel = currentArousal * pImpl->arousalLevel;
    pImpl->level += (targetLevel - pImpl->level) * 0.03f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Store arousal level
    pImpl->arousalHistory.push_back(pImpl->level);
    if (pImpl->arousalHistory.size() > 100) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin());
    }
}

void Norepinephrine::increaseArousal(float stimulusIntensity) {
    // Strong stimuli increase arousal
    float arousalBoost = stimulusIntensity * pImpl->attentionBoost;
    pImpl->level = std::min(2.0f, pImpl->level + arousalBoost);
}

void Norepinephrine::enhanceVigilance(float predictionConfidence) {
    if (predictionConfidence > 0.7f) {
        // High confidence reduces need for vigilance
        pImpl->level *= 0.9f;
    } else {
        // Low confidence increases vigilance
        pImpl->level = std::min(2.0f, pImpl->level + 0.3f);
    }
}

float Norepinephrine::calculateArousalLevel() const {
    if (!pImpl->brain) return 0.5f;
    
    float arousal = 0.5f;  // Baseline arousal
    
    // Increase arousal with prediction errors
    if (auto* predictionError = pImpl->brain->getPredictionErrorSignal()) {
        float error = predictionError->getError();
        arousal += std::min(0.5f, std::abs(error) * 0.5f);
    }
    
    // Decrease arousal with successful predictions
    if (auto* curiosity = pImpl->brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        arousal -= curiosityLevel * 0.2f;
    }
    
    return std::clamp(arousal, 0.0f, 1.0f);
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->arousalHistory.clear();
}

} // namespace nlm