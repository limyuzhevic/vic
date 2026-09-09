#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float socialGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), socialGain(2.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates learning rate for social and contextual information
    return 0.6f + 0.4f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::applyAttentionEffect(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Serotonin modulates impulsivity and decision time
    auto* attention = brain->getAttention();
    if (attention) {
        // Higher serotonin = more patience, less impulsivity
        float patienceFactor = pImpl->level;
        attention->setCompetitionThreshold(0.4f + patienceFactor * 0.2f);
        // Less aggressive competition, more thorough evaluation
    }
}

void Serotonin::applyMemoryEffect(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Serotonin influences social memory and contextual associations
    // In a real implementation, this would affect semantic memory formation
    if (brain->getAssociativeMemory()) {
        float socialModulation = pImpl->level;
        NLM_LOG_DEBUG("Serotonin: Enhancing social and contextual memory associations");
    }
}

void Serotonin::applyArousalEffect(Brain* brain) {
    if (!brain) return;
    
    // Serotonin regulates mood, behavioral inhibition, and social engagement
    if (brain->getDevelopmentSystem()) {
        // Serotonin affects developmental aspects of social behavior
        NLM_LOG_DEBUG("Serotonin: Regulating mood and social arousal");
    }
}

void Serotonin::modulateSocialBehavior(Brain* brain, float socialSalience) {
    // Increase serotonin based on social salience
    setLevel(std::min(1.0f, getLevel() + socialSalience * 0.3f));
}

void Serotonin::enhanceSocialMemory(const std::vector<float>& pattern, float socialValue) {
    // Enhance encoding of social patterns based on social value
    if (!pattern.empty() && socialValue > 0.0f) {
        float socialModulation = socialValue * pImpl->socialGain;
        setLevel(std::min(1.0f, getLevel() + socialModulation * 0.1f));
    }
}

} // namespace nlm
