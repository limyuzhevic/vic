#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <numeric>

namespace nlm;

// Memory pool for StructuralPlasticity calculations
struct StructuralPlasticityMemoryPool {
    static constexpr size_t MAX_NEURONS = 1000;
    static constexpr size_t MAX_SYNAPSES = 100000;
    
    // Fixed-size array for neuron activity statistics
    struct NeuronActivity {
        NeuronId id;
        float activity;
        float avgWeight;
    };
    NeuronActivity neuronActivities[MAX_NEURONS];
    size_t neuronCount;
    
    // Fixed-size array for synapse processing
    SynapseHandle synapseBuffer[MAX_SYNAPSES];
    size_t synapseCount;
    
    StructuralPlasticityMemoryPool() : neuronCount(0), synapseCount(0) {}
};

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
    
    // Use memory pool for temporary storage
    StructuralPlasticityMemoryPool memoryPool;
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             minWeightThreshold(0.05f), activityThreshold(0.001f),
             maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
             useActivityDependency(true),
             totalSynapsesCreated(0), totalSynapsesPruned(0) {}
};

SynapseId StructuralPlasticity::createSynapse(Brain* brain, NeuronId source, 
                                               NeuronId destination, SynapticWeight weight) {
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        // Check if connection already exists (optimized search)
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;
            }
        }
        
        // Check max synapses limit
        if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
            return INVALID_SYNAPSE_ID;
        }
        
        // Count outgoing synapses from source in this region (optimized)
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue;
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
            // Mark the synapse for removal by zeroing its weight
            syn->setWeight(0.0f);
            ++pImpl->totalSynapsesPruned;
            return true;
        }
    }
    
    return false;
}

NeuronId StructuralPlasticity::createNeuron(Brain* brain, NeuronType type) {
    return INVALID_NEURON_ID;
}

bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    return false;
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
    if (!brain) return;
    
    StructuralPlasticityMemoryPool& memoryPool = pImpl->memoryPool;
    memoryPool.neuronCount = 0;
    memoryPool.synapseCount = 0;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Collect activity statistics using memory pool
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
            
            // Store in memory pool
            if (memoryPool.neuronCount < StructuralPlasticityMemoryPool::MAX_NEURONS) {
                memoryPool.neuronActivities[memoryPool.neuronCount] = {
                    neuron->getId(), activity, avgWeight
                };
                memoryPool.neuronCount++;
            }
        }
        
        // Synaptogenesis: Create new synapses between active neurons
        if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
            for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                // Probabilistic synapse formation
                if (!rng.bernoulli(pImpl->synaptogenesisRate)) continue;
                
                size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                
                if (idx1 != idx2) {
                    NeuronId src = memoryPool.neuronActivities[idx1].id;
                    NeuronId dst = memoryPool.neuronActivities[idx2].id;
                    
                    // Only create if both neurons are somewhat active
                    if (memoryPool.neuronActivities[idx1].activity > 0.1f || 
                        memoryPool.neuronActivities[idx2].activity > 0.1f) {
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
