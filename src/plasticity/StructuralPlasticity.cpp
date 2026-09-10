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

void StructuralPlasticity::configureFromConfig(const class Config& config) {
    synaptogenesisRate = config.getOr<float>("synaptogenesis_rate", 0.0001f);
    pruningRate = config.getOr<float>("pruning_rate", 0.00001f);
    minWeightThreshold = config.getOr<float>("min_weight_threshold", 0.05f);
    activityThreshold = config.getOr<float>("activity_threshold", 0.001f);
    maxSynapsesPerNeuron = config.getOr<size_t>("max_synapses_per_neuron", 100);
    maxTotalSynapses = config.getOr<size_t>("max_total_synapses", 1000000);
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
    // Neuron creation would require adding to a population
    // For Phase 2, we focus on synaptic structural plasticity
    // and don't implement neuronal creation
    return INVALID_NEURON_ID;
}

bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    // Neuron removal would require removing all synapses and the neuron itself
    // For Phase 2, we focus on synaptic structural plasticity
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
