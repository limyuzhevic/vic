#include "Pruning.hpp"

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    // Real pruning: biologically-inspired synaptic elimination
    if (!brain || pImpl->pruningRate <= 0.0f) {
        return;
    }
    
    const auto& regions = brain->getRegions();
    if (regions.empty()) return;
    
    // Collect all synapses for potential pruning
    std::vector<Synapse*> allSynapses;
    for (const auto& region : regions) {
        const auto& neurons = region.getNeurons();
        for (const Neuron* neuron : neurons) {
            const auto& outputs = neuron->getOutputs();
            for (Synapse* synapse : outputs) {
                allSynapses.push_back(synapse);
            }
        }
    }
    
    if (allSynapses.empty()) return;
    
    // Prune weak synapses based on weight threshold and stochastic rate
    size_t synapsesToPrune = static_cast<size_t>(allSynapses.size() * pImpl->pruningRate);
    
    for (size_t i = 0; i < synapsesToPrune && i < allSynapses.size(); ++i) {
        Synapse* synapse = allSynapses[i];
        
        // Check if synapse meets pruning criteria
        if (pruneSynapse(synapse)) {
            // Remove from brain's synapse list
            brain->removeSynapse(synapse);
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // Real synapse pruning: remove weak or ineffective connections
    if (!synapse) {
        return false;
    }
    
    float weight = synapse->getWeight();
    
    // Prune based on weight threshold (Hebbian principle)
    if (std::abs(weight) < pImpl->threshold) {
        return true; // Mark for removal
    }
    
    // Additional biological criteria for pruning
    // 1. Old synapses are more likely to be pruned (age-based)
    Timestamp currentTime = synapse->getBrain()->getCurrentTime();
    Timestamp age = currentTime - synapse->getCreationTime();
    if (age > 1000.0f && rng.nextFloat() < 0.1f) { // 10% chance for old synapses
        return true;
    }
    
    // 2. Synaptic efficacy too low (neuroplasticity principle)
    float efficacy = synapse->getEfficacy();
    if (efficacy < 0.05f && rng.nextFloat() < 0.3f) { // 30% chance for low efficacy
        return true;
    }
    
    return false;
}

} // namespace nlm
