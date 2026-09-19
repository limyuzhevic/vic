#include "NE.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float noveltySensitivity;
    float stressResponseIntensity;
    float attentionFilterStrength;
    float arousalLevel;
    float learningEnhancementFactor;
    
    Impl()
        : level(0.0f), baseline(0.02f), peak(1.0f)
        , decayRate(0.15f), releaseRate(1.0f)
        , noveltySensitivity(1.0f), stressResponseIntensity(1.0f)
        , attentionFilterStrength(1.0f), arousalLevel(1.0f), learningEnhancementFactor(1.0f) {}
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
    // NE modulates plasticity based on novelty and prediction error
    // Higher NE during novel or unexpected events enhances learning
    float noveltyBasedPlasticity = pImpl->level * pImpl->noveltySensitivity;
    return 0.2f + 0.8f * noveltyBasedPlasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline with time constant influenced by decayRate
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update arousal and learning enhancement factors
    pImpl->arousalLevel = 1.0f + 3.0f * pImpl->level;
    pImpl->learningEnhancementFactor = 1.0f + pImpl->level * pImpl->stressResponseIntensity;
    pImpl->attentionFilterStrength = 1.0f + 2.0f * pImpl->level;
}

void Norepinephrine::signalNovelty(float noveltyStrength) {
    // Phasic NE release proportional to novelty detection
    // Enhanced for surprising or unexpected stimuli
    float phasicRelease = noveltyStrength * pImpl->releaseRate * pImpl->noveltySensitivity;
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicRelease);
}

void Norepinephrine::signalPredictionError(float predictionError) {
    // NE responds to prediction errors for learning enhancement
    // Absolute value ensures both positive and negative errors trigger response
    float errorSignal = std::abs(predictionError) * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + errorSignal);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // NE increases arousal levels for salient events
    float arousalBoost = arousalLevel * 0.5f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalBoost);
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE contributes to stress response and vigilance
    float stressResponse = stressLevel * 0.7f * pImpl->releaseRate * pImpl->stressResponseIntensity;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse);
}

float Norepinephrine::getAttentionFilterStrength() const {
    return pImpl->attentionFilterStrength;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getLearningEnhancementFactor() const {
    return pImpl->learningEnhancementFactor;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 1.0f;
    pImpl->learningEnhancementFactor = 1.0f;
    pImpl->attentionFilterStrength = 1.0f;
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.1f);
}

void Norepinephrine::setDecayRate(float decayRate) {
    pImpl->decayRate = std::clamp(decayRate, 0.01f, 1.0f);
}

void Norepinephrine::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = std::clamp(releaseRate, 0.5f, 5.0f);
}

void Norepinephrine::setNoveltySensitivity(float sensitivity) {
    pImpl->noveltySensitivity = std::clamp(sensitivity, 0.1f, 5.0f);
}

void Norepinephrine::setStressResponseIntensity(float intensity) {
    pImpl->stressResponseIntensity = std::clamp(intensity, 0.1f, 5.0f);
}

} // namespace nlm