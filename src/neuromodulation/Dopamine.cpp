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
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system created");
}

Dopamine::~Dopamine() {
    delete pImpl;
}

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine (reward prediction error) system initialized");
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
    float plasticity = 0.5f + 0.5f * pImpl->level;
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    if (pImpl->brain && pImpl->level > 0.0f) {
        float excitabilityMod = pImpl->level * 0.5f;
        if (excitabilityMod > 0.0f) {
            for (auto& region : pImpl->brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    NLM_LOG_INFO("Dopamine reward signal: level=" + std::to_string(pImpl->level) + " reward=" + std::to_string(reward));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    NLM_LOG_INFO("Dopamine prediction error signal: level=" + std::to_string(pImpl->level) + " error=" + std::to_string(error));
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm