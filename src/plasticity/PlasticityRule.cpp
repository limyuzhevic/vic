#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float lambda;           // BCM-like threshold variable
    float tauL;             // LTP time constant
    float tauD;             // LTD time constant
    float baselineWeight;   // Target weight for BCM rule
    
    Impl() : learningRate(0.01f), lambda(0.5f), tauL(20.0f), tauD(20.0f), baselineWeight(0.5f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || !isEnabled()) return;
    
    // Apply different Hebbian variants based on configuration
    applyHebbianLearning(synapse, preSpikes, postSpikes, dt);
}

void HebbianRule::applyHebbianLearning(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    if (!synapse) return;
    
    float deltaWeight = 0.0f;
    
    // Check for coincident spikes
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // Simple Hebbian: strengthen if pre and post fire together
        float maxPreTime = *std::max_element(preSpikes.begin(), preSpikes.end());
        float minPostTime = *std::min_element(postSpikes.begin(), postSpikes.end());
        
        if (std::abs(maxPreTime - minPostTime) < 10.0f) {  // 10ms coincidence window
            deltaWeight = pImpl->learningRate * synapse->getWeight();
        }
    }
    
    // Apply BCM-like learning with sliding threshold
    applyBCMLearning(synapse, preSpikes, postSpikes, dt, deltaWeight);
    
    // Apply weight normalization to prevent runaway excitation
    normalizeWeights(synapse, deltaWeight);
    
    // Update synapse with computed change
    synapse->addToWeight(deltaWeight);
}

void HebbianRule::applyBCMLearning(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt,
                                  float& deltaWeight) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    
    // Update BCM threshold variable (sliding threshold)
    pImpl->lambda += (currentWeight - pImpl->baselineWeight) * 0.001f * static_cast<float>(dt);
    pImpl->lambda = std::clamp(pImpl->lambda, 0.1f, 2.0f);
    
    // Calculate post-synaptic activity (simplified)
    float postActivity = static_cast<float>(postSpikes.size()) / std::max(1.0f, static_cast<float>(preSpikes.size()));
    
    // Apply BCM rule: if post > lambda, potentiate; if post < lambda, depress
    if (postActivity > pImpl->lambda) {
        // LTP: strengthen synapse
        deltaWeight += (postActivity - pImpl->lambda) * pImpl->learningRate * tauL;
    } else if (postActivity < pImpl->lambda) {
        // LTD: weaken synapse
        deltaWeight -= (pImpl->lambda - postActivity) * pImpl->learningRate * tauD;
    }
}

void HebbianRule::normalizeWeights(Synapse* synapse, float& deltaWeight) {
    if (!synapse) return;
    
    float weight = synapse->getWeight();
    
    // Prevent runaway excitation by normalizing large weights
    if (weight > 2.0f) {
        float excess = weight - 2.0f;
        deltaWeight -= excess * 0.5f;  // Reduce excess weight
    }
    
    // Ensure weights stay within reasonable bounds
    deltaWeight *= 0.95f;  // Stabilizing factor
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Clip to biologically plausible range
    newWeight = std::clamp(newWeight, 0.0f, 2.0f);
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
