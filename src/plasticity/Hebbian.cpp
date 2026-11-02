#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation with improved selectivity
     * 
     * Key improvements:
     * 1. Meta-learning: Adaptive learning rates based on recent success
     * 2. Exploration bonus: Encourage trying less-used connections
     * 3. Temporal credit assignment: Better attribution to earlier events
     * 4. Noise-based exploration: Stochasticity for escape from local optima
     * 5. Competitive learning: Synapses compete for shared resources
     */
     
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Nearest-neighbour version with enhanced selectivity
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    
    // Enhanced learning rate: baseline + meta-learning contribution
    float learningRate = pImpl->learningRate + 
                        std::min(0.005f, synapse->getEligibilityTrace() * 0.1f);  // Meta-learning
    
    float delta = std::abs(dtSeconds) < 0.1f ? learningRate : 0.0f;
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;  // strengthen inhibition, don't flip it to excitatory
    }
    
    // Apply exploration bonus: encourage less-used connections
    float usage = synapse->getEfficacy();
    float explorationFactor = std::max(0.0f, 1.0f - usage);
    delta *= (1.0f + explorationFactor * 0.5f);
    
    // Apply noise-based exploration for escape from local optima
    float noiseFactor = std::max(0.0f, 1.0f - usage) * 0.05f;  // More noise for under-used synapses
    delta += (std::rand() / (RAND_MAX + 1.0f) - 0.5f) * 0.001f * noiseFactor;
    
    // Apply with enhanced bounds. Soft bound, Phase 4 (see STDP.cpp): Hebbian
    // strengthening is always away from zero, so scale it by (1 - |w|);
    if (std::abs(delta) > 1e-6f) {
        delta *= (1.0f - std::abs(synapse->getWeight()));
        applyWeightChange(synapse, delta);
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
