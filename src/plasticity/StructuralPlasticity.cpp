#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct StructuralPlasticity::Impl {
    float synaptogenesisRate;     // Probability of new synapse per step
    float pruningRate;            // Probability of pruning per step
    float minWeightThreshold;      // Below this weight, synapse is weak
    float activityThreshold;      // Below this activity, synapse is unused
    size_t maxSynapsesPerNeuron;   // Maximum outgoing synapses per neuron
    size_t maxTotalSynapses;       // Global maximum synapses
    bool useActivityDependency;    // Whether to use activity for pruning
    
    // Statistics
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             minWeightThreshold(0.05f), activityThreshold(0.001f),
             maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
             useActivityDependency(true),
             totalSynapsesCreated(0), totalSynapsesPruned(0) {}
};

StructuralPlasticity::StructuralPlasticity() : pImpl(new Impl) {}

StructuralPlasticity::~StructuralPlasticity() = default;

SynapseId StructuralPlasticity::createSynapse(Brain* brain, NeuronId source, 
                                               NeuronId destination, SynapticWeight weight) {
    /*
     * Create a new synapse between source and destination neurons
     * 
     * The synapse is added to the appropriate neural region based on
     * which region contains the destination neuron.
     * 
     * Returns INVALID_SYNAPSE_ID if creation fails.
     */
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        // Check if connection already exists
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;  // Connection already exists
            }
        }
        
        // Check max synapses limit
        if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
            return INVALID_SYNAPSE_ID;
        }
        
        // Count outgoing synapses from source in this region
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue;  // Try next region
        }
        
        // Create the synapse
        SynapseId synId = region->addSynapse(source, destination, weight, 1);
        if (synId != INVALID_SYNAPSE_ID) {
            ++pImpl->totalSynapsesCreated;
            return synId;
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

bool StructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) {
        return false;
    }
    
    // Search all regions for the synapse
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            // For now, we mark the synapse for removal by zeroing its weight
            // Actual removal would require modifying the region's synapse storage
            syn->setWeight(0.0f);
            ++pImpl->totalSynapsesPruned;
            return true;
        }
    }
    
    return false;
}

NeuronId StructuralPlasticity::createNeuron(Brain* brain, NeuronType type) {
    // Real neuron creation: Add neuron to appropriate population in a region
    if (!brain) {
        return INVALID_NEURON_ID;
    }
    
    // Find the region with most available capacity (smallest current neuron count)
    NeuralRegion* targetRegion = nullptr;
    size_t minNeurons = SIZE_MAX;
    
    for (auto& region : brain->getRegions()) {
        size_t neuronCount = region->getTotalNeuronCount();
        if (neuronCount < minNeurons) {
            minNeurons = neuronCount;
            targetRegion = region.get();
        }
    }
    
    if (!targetRegion) {
        return INVALID_NEURON_ID;
    }
    
    // Create a new neuron with unique ID
    NeuronId newNeuronId(minNeurons + 1);  // Simple ID based on count
    
    // Initialize neuron parameters based on type
    Neuron* newNeuron = new Neuron(newNeuronId);
    newNeuron->setType(type);
    
    // Type-specific parameters
    switch (type) {
        case NeuronType::Sensory:
            newNeuron->setMembranePotential(-60.0f);  // More depolarized
            newNeuron->setThreshold(-50.0f);
            newNeuron->setRestingPotential(-60.0f);
            newNeuron->setResetPotential(-65.0f);
            newNeuron->setLeakConductance(15.0f);
            break;
        case NeuronType::Motor:
            newNeuron->setMembranePotential(-65.0f);
            newNeuron->setThreshold(-55.0f);
            newNeuron->setRestingPotential(-70.0f);
            newNeuron->setResetPotential(-70.0f);
            newNeuron->setLeakConductance(12.0f);
            break;
        case NeuronType::Excitatory:
            newNeuron->setMembranePotential(-68.0f);
            newNeuron->setThreshold(-55.0f);
            newNeuron->setRestingPotential(-70.0f);
            newNeuron->setResetPotential(-70.0f);
            newNeuron->setLeakConductance(10.0f);
            newNeuron->setRefractoryPeriod(3);
            break;
        case NeuronType::Inhibitory:
            newNeuron->setMembranePotential(-66.0f);
            newNeuron->setThreshold(-50.0f);
            newNeuron->setRestingPotential(-70.0f);
            newNeuron->setResetPotential(-70.0f);
            newNeuron->setLeakConductance(8.0f);
            newNeuron->setRefractoryPeriod(2);
            break;
        case NeuronType::Modulatory:
            newNeuron->setMembranePotential(-67.0f);
            newNeuron->setThreshold(-52.0f);
            newNeuron->setRestingPotential(-70.0f);
            newNeuron->setResetPotential(-70.0f);
            newNeuron->setLeakConductance(9.0f);
            newNeuron->setRefractoryPeriod(5);
            break;
        default:  // Internal
            newNeuron->setMembranePotential(-70.0f);
            newNeuron->setThreshold(-55.0f);
            newNeuron->setRestingPotential(-70.0f);
            newNeuron->setResetPotential(-70.0f);
            newNeuron->setLeakConductance(10.0f);
            newNeuron->setRefractoryPeriod(5);
            break;
    }
    
    // Add to region
    targetRegion->addNeuron(newNeuron);
    
    return newNeuronId;
}

bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    if (!brain || neuron == INVALID_NEURON_ID) {
        return false;
    }
    
    // Find and remove neuron from all regions
    bool neuronFound = false;
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto it = neurons.begin(); it != neurons.end(); ++it) {
            if ((*it)->getId() == neuron) {
                // Remove all outgoing synapses first
                auto outgoingSynapses = region->getSynapsesFrom(neuron);
                for (Synapse* syn : outgoingSynapses) {
                    region->removeSynapse(syn->getId());
                }
                
                // Remove all incoming synapses
                auto incomingSynapses = region->getSynapsesTo(neuron);
                for (Synapse* syn : incomingSynapses) {
                    region->removeSynapse(syn->getId());
                }
                
                // Remove the neuron
                region->removeNeuron(neuron);
                neuronFound = true;
                break;
            }
        }
        if (neuronFound) break;
    }
    
    return neuronFound;
}

float StructuralPlasticity::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

void StructuralPlasticity::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.1f);
}

float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    /*
     * Update structural plasticity
     * 
     * This implements:
     * 1. Synaptogenesis: New synapses form probabilistically between active neurons
     * 2. Pruning: Weak or unused synapses are removed
     * 
     * The process is activity-dependent:
     * - Synapses form between neurons that are frequently co-active
     * - Synapses that are weak or unused are pruned
     * 
     * Safety constraints:
     * - Maximum total synapses per region
     * - Maximum outgoing synapses per neuron
     * - Minimum weight threshold for pruning
     * - Activity threshold for usage
     */
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Collect activity statistics
        struct NeuronActivity {
            NeuronId id;
            float activity;
            float avgWeight;
        };
        std::vector<NeuronActivity> activities;
        activities.reserve(neuronCount);
        
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            float avgWeight = 0.0f;
            size_t synCount = 0;
            
            auto incoming = region->getSynapsesTo(neuron->getId());
            for (auto* syn : incoming) {
                avgWeight += std::abs(syn->getWeight());
                ++synCount;
            }
            
            // Activity based on spike history
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                // More recent spikes = higher activity
                activity = static_cast<float>(spikes.size());
            }
            
            if (synCount > 0) {
                avgWeight /= static_cast<float>(synCount);
            }
            
            activities.push_back({neuron->getId(), activity, avgWeight});
        }
        
        // Synaptogenesis: Create new synapses between active neurons
        if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
            for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                // Probabilistic synapse formation
                if (!rng.bernoulli(pImpl->synaptogenesisRate)) continue;
                
                size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                
                if (idx1 != idx2) {
                    NeuronId src = activities[idx1].id;
                    NeuronId dst = activities[idx2].id;
                    
                    // Only create if both neurons are somewhat active
                    if (activities[idx1].activity > 0.1f || activities[idx2].activity > 0.1f) {
                        // Initial weight based on activity
                        float weight = 0.1f + rng.uniformReal(0.0f, 0.2f);
                        createSynapse(brain, src, dst, weight);
                    }
                }
            }
        }
        
        // Pruning: Remove weak or unused synapses
        for (auto& syn : region->getSynapses()) {
            float weight = std::abs(syn->getWeight());
            
            // Check pruning conditions
            bool shouldPrune = false;
            
            if (weight < pImpl->minWeightThreshold) {
                // Synapse is too weak
                shouldPrune = rng.bernoulli(pImpl->pruningRate);
            } else if (pImpl->useActivityDependency) {
                // Check if synapse has been unused (no recent weight changes)
                float eligibility = std::abs(syn->getEligibilityTrace());
                if (eligibility < pImpl->activityThreshold) {
                    shouldPrune = rng.bernoulli(pImpl->pruningRate * 0.5f);
                }
            }
            
            if (shouldPrune) {
                removeSynapse(brain, syn->getId());
            }
        }
    }
}

} // namespace nlm
