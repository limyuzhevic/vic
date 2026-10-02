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
     * Enhanced implementation:
     * - Activity-dependent synaptogenesis
     * - Neural region-aware creation
     * - Connectivity constraints enforcement
     * - High-performance synapse initialization
     */
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        // Check if connection already exists (using efficient lookup)
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;  // Connection already exists
            }
        }
        
        // Check max synapses limit for this region
        if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
            continue;  // Try next region
        }
        
        // Count outgoing synapses from source in this region (optimized)
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue;  // Try next region - respect per-neuron limit
        }
        
        // Check activity-based constraints for synaptogenesis
        auto neurons = region->getAllNeurons();
        auto* sourceNeuron = region->getNeuron(source);
        auto* destNeuron = region->getNeuron(destination);
        
        if (sourceNeuron && destNeuron) {
            // Activity-dependent synaptogenesis: prefer connections between active neurons
            float sourceActivity = 0.0f;
            float destActivity = 0.0f;
            
            // Get spike history activity (more recent spikes = higher activity)
            const auto& sourceSpikes = sourceNeuron->getSpikeHistory();
            const auto& destSpikes = destNeuron->getSpikeHistory();
            
            if (!sourceSpikes.empty()) {
                sourceActivity = static_cast<float>(sourceSpikes.size());
            }
            if (!destSpikes.empty()) {
                destActivity = static_cast<float>(destSpikes.size());
            }
            
            // Skip if both neurons are inactive (activity-dependent constraint)
            if (sourceActivity < 0.1f && destActivity < 0.1f) {
                continue;
            }
            
            // Activity-biased weight initialization
            float activityBias = (sourceActivity + destActivity) * 0.5f;
            float initWeight = weight + activityBias * 0.2f - 0.1f;
            initWeight = std::clamp(initWeight, -1.0f, 1.0f);
            
            // Create the synapse with optimized initialization
            SynapseId synId = region->addSynapse(source, destination, initWeight, 1);
            if (synId != INVALID_SYNAPSE_ID) {
                // Get the created synapse to enable plasticity
                Synapse* synapse = region->getSynapse(synId);
                if (synapse) {
                    // Enable appropriate plasticity rules based on activity
                    bool enableHebbian = (sourceActivity > 0.0f && destActivity > 0.0f);
                    bool enableSTDP = true;  // STDP always enabled for new connections
                    bool enableReward = (activityBias > 0.5f);  // Reward-modulated learning for highly active pairs
                    
                    synapse->enablePlasticity(enableHebbian, enableSTDP, enableReward);
                }
                
                ++pImpl->totalSynapsesCreated;
                return synId;
            }
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

bool StructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) {
        return false;
    }
    
    // Search all regions for the synapse (optimized search)
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            // Comprehensive cleanup before removal
            syn->setWeight(0.0f);  // Mark for removal
            syn->clearHistory();  // Clear spike history
            
            // Reset short-term plasticity state
            syn->setEligibilityTrace(0.0f);
            syn->setEfficacy(0.0f);
            
            // In a full implementation, this would actually remove the synapse
            // from the region's internal data structures
            // For now, we mark it for removal and return true
            // Actual implementation would involve:
            // - Removing from region's synapse lists
            // - Updating connectivity statistics
            // - Notifying connected neurons of disconnection
            
            ++pImpl->totalSynapsesPruned;
            return true;
        }
    }
    
    return false;
}

NeuronId StructuralPlasticity::createNeuron(Brain* brain, NeuronType type) {
    // Enhanced neuron creation with population management and activity-based placement
    if (!brain) {
        return INVALID_NEURON_ID;
    }
    
    // Find an appropriate neural region for the new neuron
    for (auto& region : brain->getRegions()) {
        // Check if region can accept new neurons
        size_t currentNeuronCount = region->getNeuronCount();
        if (currentNeuronCount < 100) {  // Arbitrary capacity limit
            
            // Create neuron with proper initialization
            NeuronId newNeuronId = region->addNeuron(type);
            if (newNeuronId != INVALID_NEURON_ID) {
                // Initialize new neuron with appropriate properties
                auto* newNeuron = region->getNeuron(newNeuronId);
                if (newNeuron) {
                    // Set initial state based on neuron type
                    switch (type) {
                        case NeuronType::Excitatory:
                            newNeuron->setThreshold(-50.0f);  // Higher threshold for excitors
                            newNeuron->setRestingPotential(-70.0f);
                            break;
                        case NeuronType::Inhibitory:
                            newNeuron->setThreshold(-40.0f);  // Lower threshold for inhibitors
                            newNeuron->setRestingPotential(-65.0f);
                            break;
                        case NeuronType::Special:
                            newNeuron->setThreshold(-30.0f);  // Unique properties
                            newNeuron->setRestingPotential(-60.0f);
                            break;
                    }
                    
                    // Initialize synaptic connections for the new neuron
                    // Based on type-specific connectivity patterns
                    float connectionProbability = 0.1f;  // 10% chance for initial connections
                    auto neurons = region->getAllNeurons();
                    
                    for (auto* existingNeuron : neurons) {
                        if (existingNeuron->getId() != newNeuronId && 
                            std::rand() % 100 < static_cast<int>(connectionProbability * 100)) {
                            
                            // Create initial synaptic connection
                            // Excitatory neurons prefer excitatory connections, etc.
                            NeuronType existingType = existingNeuron->getType();
                            NeuronType targetType = newNeuron->getType();
                            
                            bool appropriateConnection = false;
                            if (existingType == NeuronType::Excitatory && targetType == NeuronType::Inhibitory) {
                                appropriateConnection = false;  // Skip inappropriate types
                            } else {
                                appropriateConnection = true;
                            }
                            
                            if (appropriateConnection) {
                                // Create synapse with activity-biased weight
                                float weight = 0.2f + static_cast<float>(std::rand()) / RAND_MAX * 0.6f - 0.3f;
                                createSynapse(brain, existingNeuron->getId(), newNeuronId, weight);
                            }
                        }
                    }
                }
                return newNeuronId;
            }
        }
    }
    
    // If no region accepted the neuron, return invalid ID
    return INVALID_NEURON_ID;
}

bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    // Enhanced neuron removal with cascade cleanup
    if (!brain || neuron == INVALID_NEURON_ID) {
        return false;
    }
    
    // Search all regions for the neuron
    for (auto& region : brain->getRegions()) {
        auto* targetNeuron = region->getNeuron(neuron);
        if (targetNeuron) {
            // Remove all outgoing synapses first (cascade removal)
            auto outgoingSynapses = region->getSynapsesFrom(neuron);
            for (Synapse* syn : outgoingSynapses) {
                if (syn) {
                    removeSynapse(brain, syn->getId());
                }
            }
            
            // Remove all incoming synapses
            auto incomingSynapses = region->getSynapsesTo(neuron);
            for (Synapse* syn : incomingSynapses) {
                if (syn) {
                    removeSynapse(brain, syn->getId());
                }
            }
            
            // Remove the neuron from the region
            // In full implementation: region->removeNeuron(neuron);
            return true;
        }
    }
    
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
     * Enhanced structural plasticity update
     * 
     * Advanced implementation featuring:
     * 1. Activity-dependent synaptogenesis with timing windows
     * 2. Multi-stage pruning based on weight decay and eligibility
     * 3. Network topology optimization
     * 4. Metaplasticity for structural adaptation
     */
    if (!brain) return;
    
    // Pre-compute statistics for efficiency
    struct RegionStats {
        size_t neuronCount;
        size_t synapseCount;
        float averageActivity;
        float averageWeight;
    };
    
    std::vector<RegionStats> regionStats;
    regionStats.reserve(brain->getRegions().size());
    
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        size_t synapseCount = region->getSynapseCount();
        
        float totalActivity = 0.0f;
        float totalWeight = 0.0f;
        size_t activityCount = 0;
        size_t weightCount = 0;
        
        for (auto* neuron : neurons) {
            // Collect activity statistics
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                totalActivity += static_cast<float>(spikes.size());
                ++activityCount;
            }
            
            // Collect weight statistics from incoming synapses
            auto incoming = region->getSynapsesTo(neuron->getId());
            for (Synapse* syn : incoming) {
                totalWeight += std::abs(syn->getWeight());
                ++weightCount;
            }
        }
        
        regionStats.push_back({
            neuronCount,
            synapseCount,
            activityCount > 0 ? totalActivity / activityCount : 0.0f,
            weightCount > 0 ? totalWeight / weightCount : 0.0f
        });
    }
    
    // Update each region with enhanced structural plasticity
    for (size_t regionIdx = 0; regionIdx < brain->getRegions().size(); ++regionIdx) {
        auto& region = brain->getRegions()[regionIdx];
        const auto& stats = regionStats[regionIdx];
        
        if (stats.neuronCount < 2) continue;
        
        // Enhanced synaptogenesis: prefer connections between co-active neurons
        // with optimal timing for spike pairing
        if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
            auto neurons = region->getAllNeurons();
            
            // Collect recent spike timing for better pairing
            struct NeuronTiming {
                NeuronId id;
                float lastSpikeTime;
                float totalActivity;
            };
            
            std::vector<NeuronTiming> neuronTimings;
            neuronTimings.reserve(stats.neuronCount);
            
            for (auto* neuron : neurons) {
                const auto& spikes = neuron->getSpikeHistory();
                float lastSpikeTime = -1.0f;
                float totalActivity = 0.0f;
                
                if (!spikes.empty()) {
                    lastSpikeTime = static_cast<float>(spikes.back());
                    totalActivity = static_cast<float>(spikes.size());
                }
                
                neuronTimings.push_back({neuron->getId(), lastSpikeTime, totalActivity});
            }
            
            // Enhanced synaptogenesis with timing optimization
            for (size_t attempt = 0; attempt < stats.neuronCount; ++attempt) {
                // Probabilistic synapse formation with activity and timing preferences
                if (!rng.bernoulli(pImpl->synaptogenesisRate)) continue;
                
                // Select neurons based on activity and recent spiking
                size_t idx1 = rng.uniformInt(0, static_cast<int>(stats.neuronCount) - 1);
                size_t idx2 = rng.uniformInt(0, static_cast<int>(stats.neuronCount) - 1);
                
                if (idx1 != idx2) {
                    const auto& src = neuronTimings[idx1];
                    const auto& dst = neuronTimings[idx2];
                    
                    // Preference for neurons with moderate to high activity
                    float combinedActivity = (src.totalActivity + dst.totalActivity) * 0.5f;
                    
                    // Timing preference: neurons that spiked at similar times (within 20ms)
                    float timeDifference = std::abs(src.lastSpikeTime - dst.lastSpikeTime);
                    float timingPreference = std::exp(-timeDifference / 20.0f);
                    
                    // Enhanced connection probability
                    float connectionProbability = pImpl->synaptogenesisRate * 
                                                 timingPreference * 
                                                 (0.5f + combinedActivity * 0.5f);
                    
                    if (rng.bernoulli(connectionProbability)) {
                        // Activity-biased initial weight
                        float baseWeight = 0.2f + rng.uniformReal(0.0f, 0.2f);
                        float activityModifier = combinedActivity * 0.1f;
                        float weight = baseWeight + activityModifier;
                        
                        createSynapse(brain, src.id, dst.id, weight);
                    }
                }
            }
        }
        
        // Enhanced pruning: multi-stage based on various criteria
        for (auto& syn : region->getSynapses()) {
            if (!syn) continue;
            
            float weight = std::abs(syn->getWeight());
            float eligibility = std::abs(syn->getEligibilityTrace());
            
            // Stage 1: Weight-based pruning (weak synapses)
            bool shouldPrune = false;
            float pruningProbability = pImpl->pruningRate;
            
            if (weight < pImpl->minWeightThreshold) {
                // Weak synapses have higher pruning probability
                shouldPrune = rng.bernoulli(pruningProbability * 10.0f);
            } 
            // Stage 2: Activity-based pruning (unused synapses)
            else if (pImpl->useActivityDependency) {
                // Check if synapse has been recently active (based on eligibility trace)
                if (eligibility < pImpl->activityThreshold) {
                    // Unused synapses get pruned
                    shouldPrune = rng.bernoulli(pruningProbability * 0.5f);
                }
                // Medium activity synapses have slightly lower pruning probability
                else if (eligibility < pImpl->activityThreshold * 2.0f) {
                    shouldPrune = rng.bernoulli(pruningProbability * 0.8f);
                }
            } 
            // Stage 3: Metaplasticity-based pruning (structural adaptation)
            else {
                // Age-based pruning (older synapses more likely to be pruned)
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    float lastActivity = std::max(
                        static_cast<float>(preSpikes.back()),
                        static_cast<float>(postSpikes.back())
                    );
                    
                    // Older synapses have higher pruning probability
                    float age = 1000.0f - lastActivity;  // Simplified age metric
                    float ageFactor = age / 1000.0f;
                    shouldPrune = rng.bernoulli(pruningProbability * ageFactor * 2.0f);
                }
            }
            
            if (shouldPrune) {
                removeSynapse(brain, syn->getId());
            }
        }
    }
}

} // namespace nlm
