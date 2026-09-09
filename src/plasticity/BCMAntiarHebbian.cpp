#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct BCMAntiarHebbian::Impl {
    float theta;    // BCM threshold
    float nu;       // BCM learning rate
    float maxWeight;
    float minWeight;
    float activityHistory;
    
    Impl() : theta(0.5f), nu(0.01f), maxWeight(1.0f), minWeight(-1.0f), activityHistory(0.0f) {}
};

BCMAntiarHebbian::BCMAntiarHebbian() : pImpl(new Impl) {}

BCMAntiarHebbian::~BCMAntiarHebbian() = default;

void BCMAntiarHebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || !synapse->isEnabled() || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // BCM (Bienenstock-Cooper-Munro) rule
    // Δw = ν * y * (y - θ) where y is postsynaptic activity
    
    // Calculate postsynaptic activity
    float postActivity = static_cast<float>(postSpikes.size()) / 100.0f;  // Normalize
    
    // Update activity history
    pImpl->activityHistory = (pImpl->activityHistory * 0.9f) + (postActivity * 0.1f);
    
    // BCM plasticity rule
    float delta = pImpl->nu * postActivity * (postActivity - pImpl->theta);
    
    applyWeightChange(synapse, delta);
}

void BCMAntiarHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMAntiarHebbian::getName() const {
    return "BCMAntiarHebbian";
}

void BCMAntiarHebbian::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

float BCMAntiarHebbian::getTheta() const {
    return pImpl->theta;
}

void BCMAntiarHebbian::setNu(float nu) {
    pImpl->nu = std::clamp(nu, 0.0f, 1.0f);
}

float BCMAntiarHebbian::getNu() const {
    return pImpl->nu;
}

} // namespace nlm