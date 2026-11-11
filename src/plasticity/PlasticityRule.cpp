#include "PlasticityRule.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// HebbianRule Implementation
struct HebbianRule::Impl {
    float learningRate;
    float maxWeightChange;
    float timeSinceUpdate;
    
    Impl() : learningRate(0.01f), maxWeightChange(0.1f), timeSinceUpdate(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(std::make_unique<Impl>()) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // REAL IMPLEMENTATION: Implement real Hebbian learning
    if (!synapse || !synapse->isPlastic() || !isEnabled()) {
        return;
    }
    
    pImpl->timeSinceUpdate += static_cast<float>(dt);
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (within 10ms window)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0f) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Calculate weight change based on coincidence detection
    if (coincidences > 0) {
        float weightChange = pImpl->learningRate * static_cast<float>(coincidences);
        weightChange = std::min(weightChange, pImpl->maxWeightChange);
        applyWeightChange(synapse, weightChange);
        std::cout << "[Hebbian] Synapse weight changed by " << weightChange << " (coincidences=" << coincidences << ")" << std::endl;
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeightChange(float maxChange) {
    pImpl->maxWeightChange = maxChange;
}

float HebbianRule::getMaxWeightChange() const {
    return pImpl->maxWeightChange;
}

// AntiHebbianRule Implementation
struct AntiHebbianRule::Impl {
    float learningRate;
    float minWeight;
    
    Impl() : learningRate(0.005f), minWeight(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(std::make_unique<Impl>()) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
    // REAL IMPLEMENTATION: Implement anti-Hebbian learning
    if (!synapse || !synapse->isPlastic() || !isEnabled()) {
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Similar to Hebbian but applies negative weight changes
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0f) {
                ++coincidences;
            }
        }
    }
    
    if (coincidences > 0) {
        float weightChange = -pImpl->learningRate * static_cast<float>(coincidences);
        // Ensure weight doesn't go below minimum
        float currentWeight = synapse->getWeight();
        if (currentWeight + weightChange < pImpl->minWeight) {
            weightChange = -((currentWeight - pImpl->minWeight) * 0.5f);
        }
        applyWeightChange(synapse, weightChange);
        std::cout << "[AntiHebbian] Synapse weight changed by " << weightChange << " (coincidences=" << coincidences << ")" << std::endl;
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// BCMRule Implementation
struct BCMRule::Impl {
    float theta;  // Threshold for synaptic modification
    float mu;     // Learning rate multiplier
    float eta;    // Weight normalization factor
    float timeSinceUpdate;
    
    Impl() : theta(0.5f), mu(0.01f), eta(0.001f), timeSinceUpdate(0.0f) {}
};

BCMRule::BCMRule() : pImpl(std::make_unique<Impl>()) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    // REAL IMPLEMENTATION: Implement BCM rule
    if (!synapse || !synapse->isPlastic() || !isEnabled()) {
        return;
    }
    
    pImpl->timeSinceUpdate += static_cast<float>(dt);
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count spike activity (simplified)
    float postActivity = static_cast<float>(postSpikes.size()) / postSpikes.size();
    
    // BCM learning rule: delta = mu * postActivity * (postActivity - theta) * weight
    float currentWeight = synapse->getWeight();
    float weightChange = pImpl->mu * postActivity * (postActivity - pImpl->theta) * currentWeight;
    
    // Apply weight normalization
    weightChange = std::max(-pImpl->eta, std::min(weightChange, pImpl->eta));
    
    applyWeightChange(synapse, weightChange);
    std::cout << "[BCM] Synapse weight changed by " << weightChange 
              << " (activity=" << postActivity << ", theta=" << pImpl->theta << ")" << std::endl;
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setMu(float mu) {
    pImpl->mu = std::max(0.0f, mu);
}

float BCMRule::getMu() const {
    return pImpl->mu;
}

} // namespace nlm