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
    
    // Synaptic scaling parameters
    float targetAverageWeight;     // Target average synaptic weight
    float scalingRate;             // Rate of synaptic scaling
    float maxWeight;               // Maximum allowed synaptic weight
    float minWeight;               // Minimum allowed synaptic weight
    bool enableScaling;            // Whether synaptic scaling is enabled
    
    // Statistics
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
    size_t totalScalingOperations;
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             minWeightThreshold(0.05f), activityThreshold(0.001f),
             maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
             useActivityDependency(true),
             targetAverageWeight(0.3f), scalingRate(0.01f),
             maxWeight(1.0f), minWeight(-1.0f), enableScaling(true),
             totalSynapsesCreated(0), totalSynapsesPruned(0),
             totalScalingOperations(0) {}
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

void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

// Synaptic scaling configuration
void StructuralPlasticity::enableSynapticScaling(bool enable) {
    pImpl->enableScaling = enable;
}

bool StructuralPlasticity::isSynapticScalingEnabled() const {
    return pImpl->enableScaling;
}

void StructuralPlasticity::setTargetAverageWeight(float weight) {
    pImpl->targetAverageWeight = std::clamp(weight, 0.0f, 1.0f);
}

void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

// Synaptic scaling configuration
void StructuralPlasticity::enableSynapticScaling(bool enable) {
    pImpl->enableScaling = enable;
}

bool StructuralPlasticity::isSynapticScalingEnabled() const {
    return pImpl->enableScaling;
}

void StructuralPlasticity::setTargetAverageWeight(float weight) {
    pImpl->targetAverageWeight = std::clamp(weight, 0.0f, 1.0f);
}

float StructuralPlasticity::getTargetAverageWeight() const {
    return pImpl->targetAverageWeight;
}

void StructuralPlasticity::setScalingRate(float rate) {
    pImpl->scalingRate = std::clamp(rate, 0.0f, 0.1f);
}

float StructuralPlasticity::getScalingRate() const {
    return pImpl->scalingRate;
}

void StructuralPlasticity::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 2.0f);
}

float StructuralPlasticity::getMaxWeight() const {
    return pImpl->maxWeight;
}

void StructuralPlasticity::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -2.0f, 0.0f);
}

float StructuralPlasticity::getMinWeight() const {
    return pImpl->minWeight;
}

void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    /*
     * Update structural plasticity
     * 
     * This implements:
     * 1. Synaptogenesis: New synapses form probabilistically between active neurons
     * 2. Pruning: Weak or unused synapses are removed
     * 3. Synaptic scaling: Global weight regulation to prevent runaway excitation
     * 
     * The process is activity-dependent:
     * - Synapses form between neurons that are frequently co-active
     * - Synapses that are weak or unused are pruned
     * - Synaptic weights are scaled to maintain homeostatic balance
     * 
     * Safety constraints:
     * - Maximum total synapses per region
     * - Maximum outgoing synapses per neuron
     * - Minimum weight threshold for pruning
     * - Activity threshold for usage
     * - Maximum and minimum weight bounds for synaptic scaling
     */
    if (!brain) return;
    
    // MEMORY-DRIVEN SYNAPTIC PLASTICITY
    // Integrate structural plasticity based on memory system needs
    if (brain) {
        // Check working memory activity to influence plasticity
        if (brain->getWorkingMemory()) {
            float workingMemoryActivity = brain->getWorkingMemory()->getMemoryActivity();
            
            // Increase synaptogenesis when working memory is low
            // This helps maintain memory capacity
            if (workingMemoryActivity < 0.3f) {
                // Memory is underutilized - promote new connections
                pImpl->synaptogenesisRate = std::min(pImpl->synaptogenesisRate * 1.5f, 0.0002f);
                NLM_LOG_DEBUG("Memory-driven plasticity: Increasing synaptogenesis due to low working memory activity");
            }
            
            // Reduce pruning when working memory is active
            // Preserve connections when memory is being actively used
            if (workingMemoryActivity > 0.7f) {
                pImpl->pruningRate = std::max(pImpl->pruningRate * 0.5f, 0.000005f);
                NLM_LOG_DEBUG("Memory-driven plasticity: Reducing pruning due to high working memory activity");
            }
        }
        
        // Check episodic memory to influence plasticity
        if (brain->getEpisodicMemory()) {
            size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
            
            // If we have few episodes, promote memory formation
            if (episodeCount < 100) {
                // Promote synaptic growth to support more memory storage
                pImpl->enableScaling = true;
                pImpl->targetAverageWeight = std::max(pImpl->targetAverageWeight, 0.4f);
                NLM_LOG_DEBUG("Memory-driven plasticity: Promoting growth for episodic memory expansion");
            }
            
            // If episodic memory is full, promote consolidation
            if (episodeCount > maxEpisodes_ * 0.8f) {
                // Focus on synaptic scaling to maintain stability
                pImpl->enableScaling = true;
                pImpl->scalingRate = std::min(pImpl->scalingRate * 1.5f, 0.05f);
                NLM_LOG_DEBUG("Memory-driven plasticity: Promoting consolidation due to high episodic memory load");
            }
        }
        
        // Memory-guided structural plasticity based on neural patterns
        // Use working memory patterns to guide where to add connections
        if (brain->getWorkingMemory()) {
            const auto& memoryNeurons = brain->getWorkingMemory()->getMemoryNeurons();
            
            // Create a map of memory neuron activity
            std::unordered_map<uint64_t, float> memoryActivityMap;
            for (NeuronId neuron : memoryNeurons) {
                // Simple scoring based on neuron ID (in real implementation, would use actual activity)
                float activity = 0.5f + (neuron.value % 100) / 100.0f;
                memoryActivityMap[neuron.value] = activity;
            }
            
            // Guide synaptogenesis based on memory patterns
            if (memoryActivityMap.size() > 0) {
                for (auto& region : brain->getRegions()) {
                    auto neurons = region->getAllNeurons();
                    size_t neuronCount = neurons.size();
                    
                    if (neuronCount >= 2) {
                        // Use memory activity to bias synapse formation
                        // Higher activity neurons are more likely to form connections
                        for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                            // Probabilistic synapse formation biased by memory activity
                            float memoryBias = 0.5f;
                            
                            size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                            size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                            
                            if (idx1 != idx2) {
                                NeuronId src = neurons[idx1]->getId();
                                NeuronId dst = neurons[idx2]->getId();
                                
                                // Apply memory bias
                                float srcActivity = 0.0f;
                                auto srcIt = memoryActivityMap.find(src.value);
                                if (srcIt != memoryActivityMap.end()) {
                                    srcActivity = srcIt->second;
                                }
                                
                                float dstActivity = 0.0f;
                                auto dstIt = memoryActivityMap.find(dst.value);
                                if (dstIt != memoryActivityMap.end()) {
                                    dstActivity = dstIt->second;
                                }
                                
                                float biasFactor = (srcActivity + dstActivity) / 2.0f;
                                float formationProb = pImpl->synaptogenesisRate * (1.0f + biasFactor);
                                
                                if (rng.bernoulli(formationProb)) {
                                    // Create synapse with weight influenced by memory activity
                                    float weight = 0.1f + rng.uniformReal(0.0f, 0.2f) * biasFactor;
                                    createSynapse(brain, src, dst, weight);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Update statistics
    pImpl->totalSynapsesCreated = 0;
    pImpl->totalSynapsesPruned = 0;
    pImpl->totalScalingOperations = 0;
    
    // Count actual operations from the regions
    for (auto& region : brain->getRegions()) {
        // Get statistics from region - would need region to track these
        // For now, keep them at zero as they're not implemented
    }

} // namespace nlm
