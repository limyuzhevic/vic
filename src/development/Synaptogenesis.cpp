#include "Synaptogenesis.hpp"

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate current synapse density
    size_t currentSynapses = 0;
    for (auto& region : brain->getRegions()) {
        currentSynapses += region->getSynapses().size();
    }
    
    float currentDensity = currentSynapses > 0 ? 
        static_cast<float>(currentSynapses) / 100.0f : 0.0f; // Normalize by expected max
    
    // Don't form synapses if we've reached target density
    if (currentDensity >= pImpl->targetDensity) {
        return;
    }
    
    // Find all neurons that could form new synapses
    std::vector<std::pair<NeuronId, NeuronId>> candidatePairs;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        // Only consider neurons with recent activity
        for (auto* source : neurons) {
            if (source->isFiring() || source->getState().membranePotential > -50.0f) {
                for (auto* target : neurons) {
                    if (source == target) continue;
                    
                    // Check if synapse already exists
                    bool synapseExists = false;
                    for (auto& syn : region->getSynapses()) {
                        if ((syn->getSource() == source->getId() && syn->getDestination() == target->getId()) ||
                            (syn->getSource() == target->getId() && syn->getDestination() == source->getId())) {
                            synapseExists = true;
                            break;
                        }
                    }
                    
                    if (!synapseExists) {
                        candidatePairs.push_back({source->getId(), target->getId()});
                    }
                }
            }
        }
    }
    
    // Form new synapses probabilistically
    for (const auto& pair : candidatePairs) {
        if (rng.generateFloat() < pImpl->formationRate) {
            // Create new synapse with appropriate weight
            float weight = rng.generateFloat() * 0.5f + 0.1f; // Range [0.1, 0.6]
            
            // Create synapse through brain's structural plasticity
            if (brain->getStructuralPlasticity()) {
                brain->getStructuralPlasticity()->createSynapse(brain, pair.first, pair.second, weight);
            }
        }
    }
}

} // namespace nlm
