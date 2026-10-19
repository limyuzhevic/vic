#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance state
    float arousalLevel;
    float vigilanceLevel;
    float signalDetectionSensitivity;
    float reactionTimeModifier;
    
    //phasic vs tonic firing
    float tonicLevel;
    float phasicRelease;
    
    // Random generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> randDist;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f)
        , arousalLevel(0.0f), vigilanceLevel(0.0f), signalDetectionSensitivity(1.0f)
        , reactionTimeModifier(1.0f), tonicLevel(0.0f), phasicRelease(0.0f) {
        std::random_device rd;
        rng.seed(rd());
        randDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
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
    float oldLevel = pImpl->level;
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update arousal and vigilance based on NE level
    // NE correlates with locus coeruleus activity
    pImpl->tonicLevel = oldLevel + (pImpl->level - oldLevel) * 0.1f; // Slower tonic changes
    pImpl->phasicRelease = std::max(0.0f, pImpl->level - oldLevel); // Phasic bursts
    
    // NE effects on performance
    pImpl->arousalLevel = pImpl->level;
    pImpl->vigilanceLevel = 0.5f + 0.5f * pImpl->level;
    pImpl->signalDetectionSensitivity = 1.0f + 0.5f * pImpl->level;
    pImpl->reactionTimeModifier = 1.0f - 0.3f * pImpl->level; // Faster reactions with more NE
}

float Norepinephrine::getPlasticityFactor() const {
    // NE has inverted-U relationship with learning:
    // Moderate NE = optimal learning, too high = anxiety/stress reduces learning
    float normalizedLevel = pImpl->level;
    return 1.0f - std::pow(std::abs(normalizedLevel - 0.5f) / 0.5f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline but with stochastic bursts
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Occasional phasic bursts (surprise signaling)
    float burstChance = 0.001f * static_cast<float>(dt);
    if (pImpl->randDist(pImpl->rng) < burstChance && pImpl->level < 0.8f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
    }
}

} // namespace nlm