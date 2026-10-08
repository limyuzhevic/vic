#include "AntiHebbianRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    // BCM-like threshold for anti-Hebbian learning
    float threshold;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             threshold(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    /*
     * Real Anti-Hebbian learning implementation
     * 
     * Anti-Hebbian plasticity: "neurons that fire together, disconnect together"
     * When pre-synaptic and post-synaptic neurons fire together, the synapse weakens
     * 
     * Mathematical formulation (simplified):
     * Δw = -η * f(post activity) * g(pre activity)
     * 
     * Where:
     *   η = learning rate
     *   f(post) = post-synaptic activity function
     *   g(pre) = pre-synaptic activity function
     * 
     * Biological inspiration:
     *   - Implements homeostatic plasticity
     *   - Prevents runaway excitation
     *   - Maintains network stability
     *   - Found in cortical and hippocampal circuits
     *   
     * Implementation details:
     *   - Weight change depends on both pre and post spike counts
     *   - More post spikes + more pre spikes = stronger depression
     *   - Weight changes are bounded to prevent instability
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate postsynaptic activity (number of recent post spikes)
    size_t postSpikeCount = postSpikes.size();
    
    // Calculate presynaptic activity (number of recent pre spikes)
    size_t preSpikeCount = preSpikes.size();
    
    // Anti-Hebbian weight change: more simultaneous firing = more depression
    float weightChange = -pImpl->learningRate * 
                         static_cast<float>(postSpikeCount) * 
                         static_cast<float>(preSpikeCount);
    
    // Apply weight change with bounds
    applyWeightChange(synapse, weightChange);
    
    // Ensure synapse stays within valid bounds
    float currentWeight = synapse->getWeight();
    if (currentWeight > pImpl->maxWeight) {
        synapse->setWeight(pImpl->maxWeight);
    } else if (currentWeight < pImpl->minWeight) {
        synapse->setWeight(pImpl->minWeight);
    }
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

void AntiHebbianRule::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float AntiHebbianRule::getThreshold() const {
    return pImpl->threshold;
}

} // namespace nlm
