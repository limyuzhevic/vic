#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal level (0.0 = low arousal, 1.0 = high arousal)
    float arousalLevel;
    float arousalDecayRate;
    
    // Attention enhancement
    float attentionStrength;
    float attentionSaturation;
    
    // Working memory enhancement
    float memoryEnhancementFactor;
    
    // Behavioral flexibility
    float flexibilityIndex;
    std::deque<float> flexibilityHistory;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , arousalLevel(0.5f), arousalDecayRate(0.05f)
        , attentionStrength(1.0f), attentionSaturation(0.9f)
        , memoryEnhancementFactor(1.0f)
        , flexibilityIndex(1.0f) {}
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
    // Norepinephrine enhances plasticity for attended/stimuli
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionStrength;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Arousal decays with time
    pImpl->arousalLevel = std::max(0.5f, pImpl->arousalLevel - pImpl->arousalDecayRate * static_cast<float>(dt));
    
    // Clean up flexibility history
    while (pImpl->flexibilityHistory.size() > 100) {
        pImpl->flexibilityHistory.pop_front();
    }
}

void Norepinephrine::setArousalLevel(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::activateLocusCoeruleus(float novelty, float salience, float predictionError) {
    // Phasic NE release based on unexpected salient events
    float burstStrength = novelty * 0.4f + salience * 0.4f + std::abs(predictionError) * 0.2f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstStrength * pImpl->releaseRate);
    
    // Increase arousal
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + burstStrength * 0.3f);
    
    // Enhance attention
    pImpl->attentionStrength = std::min(pImpl->attentionSaturation, 
                                        pImpl->attentionStrength + burstStrength * 0.2f);
}

float Norepinephrine::enhanceWorkingMemoryCapacity(float currentCapacity) const {
    // NE enhances working memory capacity
    float enhancementFactor = 1.0f + pImpl->level * pImpl->memoryEnhancementFactor * 0.5f;
    return currentCapacity * enhancementFactor;
}

void Norepinephrine::applyFocusedAttention(float attentionStrength) {
    // Apply focused attention based on NE levels
    pImpl->attentionStrength = std::clamp(attentionStrength * pImpl->level, 0.0f, 1.0f);
}

void Norepinephrine::increaseBehavioralFlexibility() {
    // NE increases behavioral flexibility
    pImpl->flexibilityIndex *= 1.1f;
    pImpl->flexibilityHistory.push_back(pImpl->flexibilityIndex);
}

} // namespace nlm
