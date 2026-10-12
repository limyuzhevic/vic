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
    try {
        /*
         * Create a new synapse between source and destination neurons
         * 
         * The synapse is added to the appropriate neural region based on
         * which region contains the destination neuron.
         * 
         * Returns INVALID_SYNAPSE_ID if creation fails.
         */
        if (!brain) {
            Logger::getGlobal().error("StructuralPlasticity::createSynapse: Brain pointer is null");
            return INVALID_SYNAPSE_ID;
        }
        
        // Validate neuron IDs
        if (source.index() == 0 || destination.index() == 0) {
            Logger::getGlobal().error("StructuralPlasticity::createSynapse: Invalid neuron IDs: source=" + std::to_string(source.index()) + ", destination=" + std::to_string(destination.index()));
            return INVALID_SYNAPSE_ID;
        }
        
        // Validate weight
        if (std::isnan(weight) || std::isinf(weight)) {
            Logger::getGlobal().error("StructuralPlasticity::createSynapse: Invalid synapse weight: " + std::to_string(weight));
            return INVALID_SYNAPSE_ID;
        }
        
        if (weight < -1.0f || weight > 1.0f) {
            Logger::getGlobal().warning("StructuralPlasticity::createSynapse: Synapse weight out of range [-1,1]: " + std::to_string(weight) + ", clamping");
            weight = std::clamp(weight, -1.0f, 1.0f);
        }
        
        // Find the region containing the destination neuron with error handling
        for (auto& region : brain->getRegions()) {
            try {
                auto synapsesTo = region->getSynapsesTo(destination);
                
                // Validate synapsesTo
                if (synapsesTo.empty()) {
                    continue;
                }
                
                // Check if connection already exists with validation
                for (Synapse* syn : synapsesTo) {
                    if (!syn) {
                        Logger::getGlobal().warning("StructuralPlasticity::createSynapse: Null synapse found");
                        continue;
                    }
                    
                    if (syn->getSourceNeuron() == source) {
                        Logger::getGlobal().debug("StructuralPlasticity::createSynapse: Connection already exists between neurons");
                        return INVALID_SYNAPSE_ID;  // Connection already exists
                    }
                }
                
                // Check max synapses limit
                if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
                    Logger::getGlobal().warning("StructuralPlasticity::createSynapse: Region synapse limit reached");
                    continue;
                }
                
                // Count outgoing synapses from source in this region
                auto synapsesFrom = region->getSynapsesFrom(source);
                if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
                    Logger::getGlobal().debug("StructuralPlasticity::createSynapse: Max synapses per neuron limit reached in region");
                    continue;  // Try next region
                }
                
                // Create the synapse with error handling
                try {
                    SynapseId synId = region->addSynapse(source, destination, weight, 1);
                    if (synId != INVALID_SYNAPSE_ID) {
                        ++pImpl->totalSynapsesCreated;
                        return synId;
                    }
                } catch (const std::exception& e) {
                    Logger::getGlobal().error("StructuralPlasticity::createSynapse: Exception creating synapse: " + std::string(e.what()));
                }
                
            } catch (const std::exception& e) {
                Logger::getGlobal().error("StructuralPlasticity::createSynapse: Exception processing region: " + std::string(e.what()));
                continue;
            }
        }
        
        return INVALID_SYNAPSE_ID;
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("StructuralPlasticity::createSynapse: Memory allocation failed");
        return INVALID_SYNAPSE_ID;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("StructuralPlasticity::createSynapse: Unexpected exception: " + std::string(e.what()));
        return INVALID_SYNAPSE_ID;
    }
}

bool StructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    try {
        // Validate brain and synapse parameters
        if (!brain || synapse == INVALID_SYNAPSE_ID) {
            Logger::getGlobal().error("StructuralPlasticity::removeSynapse: Invalid brain or synapse ID");
            return false;
        }
        
        // Search all regions for the synapse with error handling
        for (auto& region : brain->getRegions()) {
            try {
                Synapse* syn = region->getSynapse(synapse);
                if (syn) {
                    // Validate synapse before removal
                    if (std::isnan(syn->getWeight())) {
                        Logger::getGlobal().warning("StructuralPlasticity::removeSynapse: Synapse has NaN weight, skipping removal");
                        continue;
                    }
                    
                    // For now, we mark the synapse for removal by zeroing its weight
                    // Actual removal would require modifying the region's synapse storage
                    syn->setWeight(0.0f);
                    ++pImpl->totalSynapsesPruned;
                    return true;
                }
            } catch (const std::exception& e) {
                Logger::getGlobal().error("StructuralPlasticity::removeSynapse: Exception processing region: " + std::string(e.what()));
                continue;
            }
        }
        
        return false;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("StructuralPlasticity::removeSynapse: Unexpected exception: " + std::string(e.what()));
        return false;
    }
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
    try {
        // Validate rate
        if (std::isnan(rate) || std::isinf(rate)) {
            Logger::getGlobal().error("StructuralPlasticity::setSynaptogenesisRate: Invalid rate: " + std::to_string(rate));
            return;
        }
        
        pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.1f);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("StructuralPlasticity::setSynaptogenesisRate: Exception while setting rate: " + std::string(e.what()));
    }
}

float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

void StructuralPlasticity::setPruningRate(float rate) {
    try {
        // Validate rate
        if (std::isnan(rate) || std::isinf(rate)) {
            Logger::getGlobal().error("StructuralPlasticity::setPruningRate: Invalid rate: " + std::to_string(rate));
            return;
        }
        
        pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("StructuralPlasticity::setPruningRate: Exception while setting rate: " + std::string(e.what()));
    }
}

void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    try {
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
        
        // Validate inputs
        if (!brain) {
            Logger::getGlobal().error("StructuralPlasticity::update: Brain pointer is null");
            return;
        }
        
        // Validate random generator
        if (!rng.isSeeded()) {
            Logger::getGlobal().error("StructuralPlasticity::update: Random generator not properly initialized");
            return;
        }
        
        // Validate structural plasticity parameters
        if (pImpl->synaptogenesisRate < 0.0f || pImpl->synaptogenesisRate > 0.1f) {
            Logger::getGlobal().warning("StructuralPlasticity::update: Invalid synaptogenesis rate: " + std::to_string(pImpl->synaptogenesisRate) + ", clamping");
            pImpl->synaptogenesisRate = std::clamp(pImpl->synaptogenesisRate, 0.0f, 0.1f);
        }
        
        if (pImpl->pruningRate < 0.0f || pImpl->pruningRate > 0.01f) {
            Logger::getGlobal().warning("StructuralPlasticity::update: Invalid pruning rate: " + std::to_string(pImpl->pruningRate) + ", clamping");
            pImpl->pruningRate = std::clamp(pImpl->pruningRate, 0.0f, 0.01f);
        }
        
        if (pImpl->minWeightThreshold < 0.0f || pImpl->minWeightThreshold > 1.0f) {
            Logger::getGlobal().warning("StructuralPlasticity::update: Invalid minimum weight threshold: " + std::to_string(pImpl->minWeightThreshold) + ", clamping");
            pImpl->minWeightThreshold = std::clamp(pImpl->minWeightThreshold, 0.0f, 1.0f);
        }
        
        if (pImpl->activityThreshold < 0.0f || pImpl->activityThreshold > 1.0f) {
            Logger::getGlobal().warning("StructuralPlasticity::update: Invalid activity threshold: " + std::to_string(pImpl->activityThreshold) + ", clamping");
            pImpl->activityThreshold = std::clamp(pImpl->activityThreshold, 0.0f, 1.0f);
        }
        
        if (pImpl->maxSynapsesPerNeuron < 1 || pImpl->maxSynapsesPerNeuron > 1000) {
            Logger::getGlobal().warning("StructuralPlasticity::update: Invalid max synapses per neuron: " + std::to_string(pImpl->maxSynapsesPerNeuron) + ", clamping");
            pImpl->maxSynapsesPerNeuron = std::clamp(pImpl->maxSynapsesPerNeuron, 1u, 1000u);
        }
        
        if (pImpl->maxTotalSynapses < 1000 || pImpl->maxTotalSynapses > 10000000) {
            Logger::getGlobal().warning("StructuralPlasticity::update: Invalid max total synapses: " + std::to_string(pImpl->maxTotalSynapses) + ", clamping");
            pImpl->maxTotalSynapses = std::clamp(pImpl->maxTotalSynapses, 1000u, 10000000u);
        }
        
        // Process each region with error handling
        for (auto& region : brain->getRegions()) {
            try {
                auto neurons = region->getAllNeurons();
                size_t neuronCount = neurons.size();
                
                if (neuronCount < 2) {
                    continue;  // Not enough neurons for structural plasticity
                }
                
                // Validate neurons list
                if (neurons.empty()) {
                    Logger::getGlobal().warning("StructuralPlasticity::update: Region has no neurons");
                    continue;
                }
                
                // Collect activity statistics with error handling
                struct NeuronActivity {
                    NeuronId id;
                    float activity;
                    float avgWeight;
                };
                std::vector<NeuronActivity> activities;
                activities.reserve(neuronCount);
                
                for (auto* neuron : neurons) {
                    if (!neuron) {
                        Logger::getGlobal().warning("StructuralPlasticity::update: Null neuron in region");
                        continue;
                    }
                    
                    float activity = 0.0f;
                    float avgWeight = 0.0f;
                    size_t synCount = 0;
                    
                    try {
                        auto incoming = region->getSynapsesTo(neuron->getId());
                        for (auto* syn : incoming) {
                            if (!syn) {
                                continue;
                            }
                            avgWeight += std::abs(syn->getWeight());
                            ++synCount;
                        }
                    } catch (const std::exception& e) {
                        Logger::getGlobal().error("StructuralPlasticity::update: Exception getting incoming synapses: " + std::string(e.what()));
                    }
                    
                    // Activity based on spike history
                    try {
                        const auto& spikes = neuron->getSpikeHistory();
                        if (!spikes.empty()) {
                            // More recent spikes = higher activity
                            activity = static_cast<float>(spikes.size());
                        }
                    } catch (const std::exception& e) {
                        Logger::getGlobal().error("StructuralPlasticity::update: Exception getting spike history: " + std::string(e.what()));
                    }
                    
                    if (synCount > 0) {
                        avgWeight /= static_cast<float>(synCount);
                    }
                    
                    activities.push_back({neuron->getId(), activity, avgWeight});
                }
                
                // Validate activities collected
                if (activities.empty()) {
                    Logger::getGlobal().warning("StructuralPlasticity::update: No valid neurons collected for activity statistics");
                    continue;
                }
                
                // Synaptogenesis: Create new synapses between active neurons
                if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
                    for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                        try {
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
                        } catch (const std::exception& e) {
                            Logger::getGlobal().error("StructuralPlasticity::update: Exception during synaptogenesis: " + std::string(e.what()));
                        }
                    }
                }
                
                // Pruning: Remove weak or unused synapses
                for (auto& syn : region->getSynapses()) {
                    try {
                        if (!syn) {
                            Logger::getGlobal().warning("StructuralPlasticity::update: Null synapse found during pruning");
                            continue;
                        }
                        
                        float weight = std::abs(syn->getWeight());
                        
                        // Validate weight
                        if (std::isnan(weight) || std::isinf(weight)) {
                            Logger::getGlobal().error("StructuralPlasticity::update: Invalid synapse weight: " + std::to_string(weight));
                            continue;
                        }
                        
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
                    } catch (const std::exception& e) {
                        Logger::getGlobal().error("StructuralPlasticity::update: Exception during synapse pruning: " + std::string(e.what()));
                    }
                }
                
            } catch (const std::exception& e) {
                Logger::getGlobal().error("StructuralPlasticity::update: Exception processing region: " + std::string(e.what()));
                continue;
            }
        }
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("StructuralPlasticity::update: Memory allocation failed");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("StructuralPlasticity::update: Unexpected exception: " + std::string(e.what()));
    }
}

} // namespace nlm
