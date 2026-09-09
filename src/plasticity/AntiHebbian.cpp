#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    if (!synapse || !synapse->isEnabled() || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Anti-Hebbian learning: "neurons that fire together, weaken together"
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Calculate activity correlation
    float correlation = std::min(preActivity, postActivity) / 
                        std::max(preActivity, postActivity);
    
    // Weight change is negative correlation (anti-Hebbian)
    float delta = -pImpl->learningRate * correlation;
    
    applyWeightChange(synapse, delta);
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm