#include "BCM_Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMHebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float bcmThreshold;   // BCM threshold (theta_m)
    float bcmLearningRate; // BCM learning rate (eta)
    float bcmRegularization; // BCM regularization (lambda)
    float postActivity;    // Current post-synaptic activity (running average)
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             bcmThreshold(0.5f), bcmLearningRate(0.01f), bcmRegularization(0.1f),
             postActivity(0.0f) {}
};

BCMHebbian::BCMHebbian() : pImpl(new Impl) {}

BCMHebbian::~BCMHebbian() = default;

void BCMHebbian::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Estimate post-synaptic activity
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Update running average
    pImpl->postActivity = pImpl->postActivity * 0.99f + postActivity * 0.01f;
    
    // BCM rule: Δw = η * (y - θ_m) * x
    // Where y is average post-synaptic activity, x is pre-synaptic input
    
    for (Timestamp preTime : preSpikes) {
        // For each pre-synaptic spike, compute weight change
        float weightChange = pImpl->bcmLearningRate * 
                            (pImpl->postActivity - pImpl->bcmThreshold);
        
        // Apply BCM regularization to prevent runaway potentiation
        if (pImpl->postActivity > pImpl->bcmThreshold) {
            // When activity exceeds threshold, apply regularization
            weightChange *= std::exp(-pImpl->bcmRegularization * 
                                   synapse->getWeight() * synapse->getWeight());
        }
        
        // Apply weight change
        if (std::abs(weightChange) > 1e-6f) {
            applyWeightChange(synapse, weightChange);
        }
    }
}

void BCMHebbian::setBCMThreshold(float threshold) {
    pImpl->bcmThreshold = std::clamp(threshold, 0.0f, 2.0f);
}

float BCMHebbian::getBCMThreshold() const {
    return pImpl->bcmThreshold;
}

void BCMHebbian::setBCMLearningRate(float rate) {
    pImpl->bcmLearningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMHebbian::getBCMLearningRate() const {
    return pImpl->bcmLearningRate;
}

void BCMHebbian::setBCMRegularization(float reg) {
    pImpl->bcmRegularization = std::clamp(reg, 0.01f, 1.0f);
}

float BCMHebbian::getBCMRegularization() const {
    return pImpl->bcmRegularization;
}

} // namespace nlm
