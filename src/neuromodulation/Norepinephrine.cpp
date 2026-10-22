#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;                    // Current NE concentration (0-1)
    float baseline;                  // Baseline level (0.2)
    float peak;                      // Peak level (1.0)
    float decayRate;                 // Natural decay rate (0.08)
    float arousalThreshold;          // Threshold for arousal response (0.5)
    float learningAmplification;     // Learning rate amplification (1.8)
    float attentionBoost;            // Attention enhancement (1.3)
    float memoryEncoding;            // Memory encoding boost (1.4)
    
    Impl() 
        : level(0.0f)
        , baseline(0.2f)
        , peak(1.0f)
        , decayRate(0.08f)
        , arousalThreshold(0.5f)
        , learningAmplification(1.8f)
        , attentionBoost(1.3f)
        , memoryEncoding(1.4f) {}
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
    // NE enhances memory encoding and learning consolidation
    // Formula: base_factor * (1 + amplification * level)
    float baseFactor = 1.0f;  // Normal plasticity
    return baseFactor * (1.0f + pImpl->learningAmplification * pImpl->level);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::respondToStress(float stressLevel, float contextImportance) {
    if (stressLevel <= 0.0f) return;
    
    // Calculate stress response
    float stressResponse = std::min(1.0f, stressLevel * contextImportance * 1.5f);
    
    // NE spikes in response to stress
    float spikeAmount = stressResponse * pImpl->peak;
    pImpl->level = std::min(pImpl->peak, pImpl->level + spikeAmount);
    
    // Log the response
    if (stressResponse > pImpl->arousalThreshold) {
        NLM_LOG_INFO("NE arousal response: stress=" + std::to_string(stressLevel) + 
                    " context_importance=" + std::to_string(contextImportance) + 
                    " level=" + std::to_string(pImpl->level));
    }
}

float Norepinephrine::modulateLearningRate(float baselineLearningRate, float attention) const {
    // NE amplifies learning based on attention and current arousal
    float attentionFactor = std::min(2.0f, 1.0f + attention * 0.5f);
    return baselineLearningRate * pImpl->learningAmplification * 
           attentionFactor * (1.0f + pImpl->level * 0.5f);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
