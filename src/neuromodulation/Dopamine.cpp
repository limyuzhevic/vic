#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float rewardPredictionError;
    float learningRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.01f)
        , peak(2.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , rewardPredictionError(0.0f)
        , learningRate(0.3f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    return 0.3f + 0.7f * (pImpl->level / pImpl->peak);
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 1.5f);
    
    pImpl->rewardPredictionError = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->rewardPredictionError = error;
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::enhanceActionSelection() {
    if (!pImpl->brain) return;
    
    float actionSelectionBoost = pImpl->level * pImpl->learningRate * 0.5f;
    
    pImpl->brain->injectCurrentToNeurons(NeuronType::Motor, actionSelectionBoost);
}

void Dopamine::consolidateMemory(const std::vector<NeuronId>& activeNeurons) {
    if (!pImpl->brain || activeNeurons.empty()) return;
    
    float consolidationFactor = pImpl->level * 0.3f;
    
    for (NeuronId neuronId : activeNeurons) {
        pImpl->brain->injectCurrent(neuronId, consolidationFactor * 0.1f);
    }
}

} // namespace nlm
