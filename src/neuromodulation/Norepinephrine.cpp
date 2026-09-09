#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float salienceGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), salienceGain(3.0f) {}
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
    // NE modulates plasticity for salient events
    return 0.3f + 0.7f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::applyAttentionEffect(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // NE increases signal-to-noise ratio in attention
    auto* attention = brain->getAttention();
    if (attention) {
        // Increase attention strength based on NE level
        float attentionStrength = pImpl->level * pImpl->salienceGain;
        attention->setExcitationStrength(1.2f + attentionStrength);
        attention->setCompetitionThreshold(0.2f - attentionStrength * 0.05f);
    }
}

void Norepinephrine::applyMemoryEffect(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // NE enhances consolidation of highly salient events
    // In a real implementation, this would prioritize episodes with high NE
    if (brain->getEpisodicMemory()) {
        float salienceEffect = pImpl->level;
        NLM_LOG_DEBUG("Norepinephrine: Enhancing salience-based memory consolidation");
    }
}

void Norepinephrine::applyArousalEffect(Brain* brain) {
    if (!brain) return;
    
    // NE increases arousal, alertness, and readiness to respond
    if (brain->getDevelopmentSystem()) {
        // NE affects developmental arousal patterns
        NLM_LOG_DEBUG("Norepinephrine: Increasing arousal and vigilance");
    }
}

void Norepinephrine::increaseVigilance(float amount) {
    // Increase NE level to boost alertness
    setLevel(std::min(1.0f, getLevel() + amount));
}

void Norepinephrine::enhanceNoveltyEncoding(const std::vector<float>& pattern) {
    // NE enhances encoding of novel or salient patterns
    if (!pattern.empty()) {
        // Calculate pattern salience (simple heuristic)
        float avg = 0.0f, variance = 0.0f;
        for (float v : pattern) {
            avg += v;
        }
        avg /= pattern.size();
        for (float v : pattern) {
            variance += (v - avg) * (v - avg);
        }
        variance /= pattern.size();
        
        // Higher variance = more novel/salient
        float novelty = std::sqrt(variance);
        if (novelty > 0.1f) {
            increaseVigilance(novelty * 0.5f);
        }
    }
}

} // namespace nlm
