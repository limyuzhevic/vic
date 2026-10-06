#include "Dopamine.hpp"
#include "brain/Brain.hpp"
#include "Neuromodulator.hpp"
#include "Curiosity.hpp"
#include "PredictionError.hpp"
#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float rewardHistory;
    float predictionErrorHistory;
    float noveltyHistory;
    float plasticityInfluence;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(2.5f)
        , rewardHistory(0.0f)
        , predictionErrorHistory(0.0f)
        , noveltyHistory(0.0f)
        , plasticityInfluence(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

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
    return pImpl->plasticityInfluence;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    pImpl->plasticityInfluence = 0.5f + 0.5f * pImpl->level;
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardHistory = reward;
    pImpl->level = std::min(pImpl->peak, 
                          pImpl->level + reward * pImpl->releaseRate);
    
    pImpl->plasticityInfluence = std::min(2.0f, pImpl->plasticityInfluence + 0.2f * reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionErrorHistory = error;
    
    if (error > 0.0f) {
        pImpl->level = std::min(pImpl->peak, 
                              pImpl->level + error * pImpl->releaseRate * 2.0f);
        pImpl->plasticityInfluence = std::min(2.0f, pImpl->plasticityInfluence + error * 0.3f);
    } else if (error < 0.0f) {
        pImpl->level = std::max(pImpl->baseline,
                              pImpl->level + error * pImpl->releaseRate);
    }
}

void Dopamine::integrateCuriosity(float curiosityLevel) {
    pImpl->noveltyHistory = curiosityLevel;
    
    pImpl->level = std::min(pImpl->peak,
                          pImpl->level + curiosityLevel * 0.5f * pImpl->releaseRate);
    
    pImpl->plasticityInfluence = std::min(2.0f, pImpl->plasticityInfluence + curiosityLevel * 0.3f);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->plasticityInfluence = 1.0f;
}

} // namespace nlm
