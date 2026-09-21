#include "Curiosity.hpp"
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    // Real curiosity-modulated plasticity factor implementation
    // Curiosity drives exploration and learning about novel environments
    // Modulates behavior when current state is unexpected or uncertain
    
    if (pImpl->level < 0.05f) {
        return 1.0f;  // Baseline plasticity
    }
    
    // Moderate curiosity enhances plasticity (exploration promotes learning)
    float curiosityMod = 1.0f + 1.5f * pImpl->level;  // Up to 2.5x at moderate levels
    
    // High curiosity can be overwhelming (negative feedback)
    if (pImpl->level > 0.7f) {
        curiosityMod = 2.0f + 0.5f * (1.0f - pImpl->level);  // Decline after 0.7
    }
    
    return std::clamp(curiosityMod, 1.0f, 3.0f);
}

void Curiosity::update(TimestepDuration dt) {
    // Real curiosity dynamics implementation
    // Models exploration-exploitation trade-off
    // Regulates investigation of novel stimuli
    
    float timeDecay = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline exponentially
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-timeDecay);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Curiosity::signalNovelty(float novelty) {
    // Signal novel stimulus detection
    // Curiosity peaks with unexpected or surprising stimuli
    // Promotes investigation and learning about new information
    
    if (novelty > 0.0f) {
        // Novelty burst proportional to surprise level
        float burst = std::min(pImpl->peak, novelty * pImpl->releaseRate * 1.8f);
        
        // Increase level (not replace)
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Maintain novelty sensitivity as baseline
        if (pImpl->baseline == 0.0f) {
            pImpl->baseline = pImpl->level * 0.1f;  // 10% of peak as baseline
        }
    }
}

void Curiosity::signalExplorationDrive(float drive) {
    // Signal increased exploration motivation
    // Curiosity drives systematic search for new information
    // Compensates for uncertainty and reduces exploitation bias
    
    if (drive > 0.0f) {
        // Exploration drive proportional to uncertainty
        float explorationBoost = std::min(pImpl->peak, drive * pImpl->releaseRate * 1.2f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + explorationBoost);
    }
}

void Curiosity::signalUncertaintyReduction(float reduction) {
    // Signal reduction in uncertainty
    // Curiosity decreases as prediction uncertainty decreases
    // Promotes exploration only when information gap is large
    
    if (reduction > 0.0f) {
        // Uncertainty reduction should decrease curiosity
        float reductionFactor = std::min(0.5f, reduction * 0.1f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level * (1.0f - reductionFactor));
    }
}

} // namespace nlm