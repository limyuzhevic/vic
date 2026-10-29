#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityFactor;
    float rewardStore;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.2f)
        , releaseRate(1.0f)
        , plasticityFactor(1.0f)
        , rewardStore(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->rewardStore = std::max(0.0f, pImpl->rewardStore - 0.1f * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->rewardStore += reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::modulatePlasticity(STDP* stdpSystem, Hebbian* hebbianSystem) {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity
    // PLACEHOLDER: Dopamine modulates both STDP and Hebbian plasticity
    if (pImpl->level > 0.5f) {
        // High dopamine enhances LTP (long-term potentiation)
        if (stdpSystem) {
            stdpSystem->setLTPWeight(stdpSystem->getLTPWeight() * (1.0f + pImpl->level * 0.5f));
        }
    } else if (pImpl->level < 0.2f) {
        // Low dopamine enhances LTD (long-term depression)
        if (stdpSystem) {
            stdpSystem->setLTDWeight(stdpSystem->getLTDWeight() * (1.0f + (1.0f - pImpl->level) * 0.3f));
        }
    }
}

float Dopamine::getRewardPredictionError(float expectedReward, float actualReward) {
    // TODO PHASE 2: Implement real reward prediction error computation
    // PLACEHOLDER: Simple difference
    return actualReward - expectedReward;
}

} // namespace nlm