#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Effects on arousal and vigilance
    float arousalLevel;
    float arousalThreshold;
    
    // Effects on learning rate
    float learningGain;
    float learningThreshold;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(1.0f),
             arousalLevel(1.0f), arousalThreshold(0.5f),
             learningGain(1.0f), learningThreshold(0.5f) {}
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
    // TODO PHASE 2: Implement real NE-modulated plasticity factor
    // NE typically increases arousal and learning
    return 0.5f + 0.5f * pImpl->level;  // High NE = more plasticity
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // TODO PHASE 2: Implement arousal signaling
    // NE increases with arousal/stress
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
}

void Norepinephrine::signalLearning(float learning) {
    // TODO PHASE 2: Implement learning signaling
    // NE increases with novelty and learning opportunities
    pImpl->level = std::min(pImpl->peak, pImpl->level + learning * pImpl->releaseRate);
}

} // namespace nlm