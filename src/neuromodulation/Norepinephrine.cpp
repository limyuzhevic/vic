#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    float signalToNoiseRatio;
    float learningRateModulation;
    
    Impl() 
        : level(0.0f), baseline(0.05f), peak(1.0f), 
          decayRate(0.08f), arousalLevel(0.5f),
          vigilanceLevel(0.5f), stressLevel(0.0f),
          signalToNoiseRatio(1.0f), learningRateModulation(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine system initialized");
}

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
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates learning through arousal and stress
    // Higher NE increases signal-to-noise ratio, enhancing learning
    return 1.0f + pImpl->learningRateModulation * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: maintains arousal, stress response
    // Time constant ~250ms (decayRate = 0.08 per ms)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel = pImpl->level;
    pImpl->vigilanceLevel = pImpl->level * 0.8f + 0.2f; // Vigilance slightly lower than arousal
}

void Norepinephrine::signalEvent(float magnitude) {
    // Events affect arousal and vigilance
    if (magnitude > 0.0f) {
        // Strong events increase NE release
        pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * 1.5f);
        pImpl->signalToNoiseRatio = std::min(2.0f, pImpl->signalToNoiseRatio + magnitude * 0.5f);
    } else if (magnitude < 0.0f) {
        // Negative events (stress) also increase NE
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel - magnitude * 0.5f);
        pImpl->level = std::min(pImpl->peak, pImpl->level - magnitude * 0.5f);
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->stressLevel = 0.0f;
    pImpl->signalToNoiseRatio = 1.0f;
    pImpl->learningRateModulation = 0.0f;
}

float Norepinephrine::getBaselineLevel() const {
    return pImpl->baseline;
}

float Norepinephrine::getPeakLevel() const {
    return pImpl->peak;
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress affects NE release and arousal
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    // Stress increases NE up to a point (optimal arousal)
    if (pImpl->stressLevel > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->stressLevel * 0.5f);
    }
}

void Norepinephrine::signalNovelty(float noveltyLevel) {
    // Novelty increases arousal and vigilance
    pImpl->vigilanceLevel = noveltyLevel;
    if (noveltyLevel > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyLevel * 0.3f);
        pImpl->signalToNoiseRatio = std::min(2.0f, pImpl->signalToNoiseRatio + noveltyLevel * 0.3f);
        pImpl->learningRateModulation = noveltyLevel * 0.5f;
    }
}

void Norepinephrine::enhanceSignalToNoiseRatio(float enhancement) {
    // Direct enhancement of signal-to-noise ratio
    pImpl->signalToNoiseRatio = std::min(2.0f, pImpl->signalToNoiseRatio + enhancement);
    // This also affects learning efficiency
    if (enhancement > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + enhancement * 0.2f);
    }
}

void Norepinephrine::modulateLearningRate(float rateChange) {
    // Direct modulation of learning rate
    pImpl->learningRateModulation = std::clamp(rateChange, -0.5f, 0.5f);
    // Update NE level if learning rate increased significantly
    if (rateChange > 0.1f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + rateChange * 0.3f);
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

} // namespace nlm
