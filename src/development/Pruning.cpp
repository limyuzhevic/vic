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
    if (!brain) return;
    
    // Prune weak synapses based on activity and strength
    for (auto& region : brain->getRegions()) {
        for (size_t i = 0; i < region->getSynapses().size();) {
            auto& synapse = region->getSynapses()[i];
            
            // Calculate synapse strength and activity
            float weight = synapse->getWeight();
            float activity = synapse->getActivity();
            
            // Check if synapse should be pruned
            bool shouldPrune = false;
            
            // Prune if weight is too low
            if (std::abs(weight) < pImpl->threshold) {
                shouldPrune = true;
            }
            
            // Also prune if synapse has been inactive for a while
            // This requires tracking last activity time
            // For now, we use a simplified activity check
            if (activity < 0.01f && rng.generateFloat() < pImpl->pruningRate) {
                shouldPrune = true;
            }
            
            // Additional pruning based on developmental stage
            // More aggressive pruning in early development
            float developmentPruning = 1.0f - brain->getDevelopmentSystem()->getPlasticityModifier() * 0.5f;
            if (rng.generateFloat() < pImpl->pruningRate * developmentPruning) {
                shouldPrune = true;
            }
            
            if (shouldPrune) {
                // Remove the synapse
                if (brain->getStructuralPlasticity()) {
                    brain->getStructuralPlasticity()->removeSynapse(brain, synapse->getId());
                }
                // Remove from region (this is a simplification - in reality,
                // the structural plasticity system would handle this)
                region->getSynapses().erase(region->getSynapses().begin() + i);
            } else {
                i++; // Only increment if we didn't erase this synapse
            }
        }
    }
    
    // Prune weak neurons (neurons with very low activity)
    for (auto& region : brain->getRegions()) {
        for (size_t i = 0; i < region->getPopulations().size();) {
            auto& pop = region->getPopulations()[i];
            
            // Check if population should be pruned (very low activity)
            float totalActivity = 0.0f;
            for (auto* neuron : pop->getNeurons()) {
                totalActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            }
            
            if (totalActivity < pImpl->threshold * pop->getNeurons().size()) {
                // Prune this population
                if (brain->getStructuralPlasticity()) {
                    for (auto* neuron : pop->getNeurons()) {
                        brain->getStructuralPlasticity()->removeNeuron(brain, neuron->getId());
                    }
                }
                // Remove population (simplification)
                region->getPopulations().erase(region->getPopulations().begin() + i);
            } else {
                i++;
            }
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Real synapse removal implementation
    // In a full implementation, this would:
    // 1. Remove from synapset lists in neurons
    // 2. Update synaptic weights in plasticity systems
    // 3. Update spike timing information
    // 4. Free memory resources
    
    // For now, we'll mark the synapse as removed by setting weight to 0
    // This is a placeholder for Phase 2 implementation
    synapse->setWeight(0.0f);
    
    // Also remove from synaptic plasticity histories
    synapse->clearPreSpikeHistory();
    synapse->clearPostSpikeHistory();
    
    return true;
}

} // namespace nlm
