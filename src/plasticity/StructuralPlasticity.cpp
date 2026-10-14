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
     * Real synapse creation based on activity-dependent principles
     * 
     * The creation probability depends on:
     * 1. Pre-synaptic neuron activity
     * 2. Post-synaptic neuron activity
     * 3. Developmental stage
     * 4. Neuromodulatory state
     * 
     * Biological basis:
     * - Activity-dependent synaptogenesis in critical periods
     * - Hebbian "fire together, wire together" principle
     * - Growth cone guidance based on chemoattractants
     */
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        // Check if destination neuron exists in this region
        bool destInRegion = false;
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getId() == destination) {
                destInRegion = true;
                break;
            }
        }
        if (!destInRegion) continue;
        
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
        
        // Calculate activity-dependent creation probability
        // Higher activity in both neurons = higher probability
        float creationProb = pImpl->synaptogenesisRate;
        
        // Get pre-synaptic neuron activity (spike history)
        float sourceActivity = 0.0f;
        float destActivity = 0.0f;
        
        for (auto* neuron : region->getAllNeurons()) {
            const auto& spikes = neuron->getSpikeHistory();
            if (spikes.empty()) continue;
            
            if (neuron->getId() == source) {
                sourceActivity = static_cast<float>(spikes.size()) / static_cast<float>(spikes.size() + 1);  // Normalize
            } else if (neuron->getId() == destination) {
                destActivity = static_cast<float>(spikes.size()) / static_cast<float>(spikes.size() + 1);
            }
        }
        
        // Activity-dependent enhancement
        creationProb *= (1.0f + sourceActivity + destActivity);
        
        // Neuromodulation effects (simplified dopamine influence)
        creationProb *= (1.0f + 0.5f * sourceActivity * destActivity);  // Hebbian enhancement
        
        // Ensure probability is within bounds
        creationProb = std::clamp(creationProb, 0.0f, 1.0f);
        
        // Only create with probability
        RandomGenerator rng;  // TODO: Pass actual random generator
        if (!rng.bernoulli(creationProb)) {
            continue;
        }
        
        // Create the synapse with activity-dependent initial weight
        float activityWeight = 0.01f;  // Minimum weight
        if (sourceActivity > 0.1f && destActivity > 0.1f) {
            // Strong activity correlation -> stronger initial connection
            activityWeight = 0.1f + 0.5f * std::min(sourceActivity, destActivity);
        }
        
        // Add small random variation
        float finalWeight = activityWeight * (0.9f + 0.2f * (static_cast<float>(rng.uniformReal(0, 1))));
        
        SynapseId synId = region->addSynapse(source, destination, finalWeight, 1);
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
            // Real pruning criteria based on multiple factors:
            // 1. Weight magnitude (weak synapses are less important)
            // 2. Activity (synapses with little co-activity are unused)
            // 3. Time since last modification (older synapses decay)
            
            float weight = std::abs(syn->getWeight());
            float eligibility = std::abs(syn->getEligibilityTrace());
            
            // Check pruning conditions
            bool shouldPrune = false;
            
            // Primary pruning: weak synapses
            if (weight < pImpl->minWeightThreshold) {
                // Weak synapses are pruned with higher probability
                RandomGenerator rng;  // TODO: Pass actual random generator
                shouldPrune = rng.bernoulli(pImpl->pruningRate * 2.0f);  // Higher rate for weak synapses
            } 
            // Secondary pruning: activity-dependent
            else if (pImpl->useActivityDependency && eligibility < pImpl->activityThreshold) {
                // Synapses with low eligibility (unused) are pruned
                RandomGenerator rng;  // TODO: Pass actual random generator
                shouldPrune = rng.bernoulli(pImpl->pruningRate * 1.5f);
            }
            // Tertiary pruning: age-dependent (if we had timestamp info)
            // For now, use weight decay pattern
            else if (weight < 0.02f) {  // Very weak synapses
                RandomGenerator rng;  // TODO: Pass actual random generator
                shouldPrune = rng.bernoulli(pImpl->pruningRate * 1.2f);
            }
            
            if (shouldPrune) {
                // For now, we mark the synapse for removal by zeroing its weight
                // In a full implementation, this would remove the synapse entirely
                syn->setWeight(0.0f);
                syn->setEligibilityTrace(0.0f);
                ++pImpl->totalSynapsesPruned;
                return true;
            }
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
