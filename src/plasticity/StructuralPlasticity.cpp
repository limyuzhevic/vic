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
    
    // NEW: Enhanced structural plasticity parameters
    float spineFormationRate;      // Rate of new dendritic spine formation
    float spineEliminationRate;    // Rate of spine elimination
    float axonalSproutingRate;     // Rate of axonal sprouting
    float axonalRetractionRate;    // Rate of axonal retraction
    float synapticCompetitionStrength; // Strength of synapse competition
    bool enableMetabolicConstraints;   // Whether metabolic constraints affect plasticity
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             minWeightThreshold(0.05f), activityThreshold(0.001f),
             maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
             useActivityDependency(true),
             totalSynapsesCreated(0), totalSynapsesPruned(0),
             spineFormationRate(0.001f), spineEliminationRate(0.0005f),
             axonalSproutingRate(0.0002f), axonalRetractionRate(0.00005f),
             synapticCompetitionStrength(0.5f), enableMetabolicConstraints(true) {}
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
     * 3. Enhanced structural mechanisms:
     *    - Dendritic spine dynamics
     *    - Axonal sprouting and retraction
     *    - Activity-dependent synaptogenesis
     *    - Synapse competition
     *    - Metabolic constraints
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
        
        // NEW: Dendritic spine dynamics
        updateDendriticSpineDynamics(brain, region, rng, activities);
        
        // NEW: Axonal sprouting and retraction
        updateAxonalDynamics(brain, region, rng, activities);
        
        // NEW: Activity-dependent synaptogenesis with higher thresholds
        size_t activityDrivenSynapses = activityDependentSynaptogenesis(
            brain, rng, 0.5f, static_cast<float>(pImpl->maxSynapsesPerNeuron)
        );
        
        // NEW: Synapse competition
        synapseCompetition(brain, region, rng);
        
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

// NEW: Implementation of enhanced structural plasticity methods

bool StructuralPlasticity::formDendriticSpine(Brain* brain, NeuronId neuron,
                                             const std::vector<float>& position) {
    if (!brain) return false;
    
    // Find neuron in any region
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                // In real implementation, would add spine to neuron's dendrite structure
                // For now, just log that spine formation was requested
                NLM_LOG_INFO("Dendritic spine formation requested for neuron " + 
                           std::to_string(neuron) + " at position " + 
                           std::to_string(position[0]) + ", " +
                           std::to_string(position[1]));
                return true;
            }
        }
    }
    return false;
}

bool StructuralPlasticity::eliminateDendriticSpine(Brain* brain, NeuronId neuron,
                                                   size_t spineIndex) {
    if (!brain) return false;
    
    // Find neuron in any region
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                // In real implementation, would remove spine from neuron's dendrite structure
                NLM_LOG_INFO("Dendritic spine elimination requested for neuron " + 
                           std::to_string(neuron) + " spine index " +
                           std::to_string(spineIndex));
                return true;
            }
        }
    }
    return false;
}

bool StructuralPlasticity::sproutAxon(Brain* brain, NeuronId source, NeuronId target,
                                     float outgrowthRate) {
    if (!brain) return false;
    
    // Check if sprouting is allowed based on metabolic constraints
    if (pImpl->enableMetabolicConstraints && outgrowthRate > 0.1f) {
        // Metabolic cost check (simplified)
        if (rng.bernoulli(0.3f)) {  // 30% chance of metabolic constraint violation
            NLM_LOG_INFO("Axonal sprouting aborted - metabolic constraint exceeded");
            return false;
        }
    }
    
    // Check if connection already exists
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(target);
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return false;  // Already connected
            }
        }
    }
    
    // Create new synapse with higher initial weight for sprouted connections
    float sproutedWeight = 0.3f + rng.uniformReal(0.0f, 0.4f);
    SynapseId synId = createSynapse(brain, source, target, sproutedWeight);
    
    if (synId != INVALID_SYNAPSE_ID) {
        NLM_LOG_INFO("Axonal sprouting successful from neuron " + 
                     std::to_string(source) + " to neuron " +
                     std::to_string(target));
        return true;
    }
    
    return false;
}

bool StructuralPlasticity::retractAxon(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) return false;
    
    // Find and remove the synapse
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            // In real implementation, would remove synapse from region
            syn->setWeight(0.0f);
            NLM_LOG_INFO("Axonal retraction: synapse " + std::to_string(synapse) + 
                         " removed between neurons " +
                         std::to_string(syn->getSourceNeuron()) + " and " +
                         std::to_string(syn->getDestinationNeuron()));
            return true;
        }
    }
    return false;
}

size_t StructuralPlasticity::activityDependentSynaptogenesis(Brain* brain,
                                                             RandomGenerator& rng,
                                                             float minActivityThreshold,
                                                             float maxSynapsesPerNeuron) {
    if (!brain) return 0;
    
    size_t newSynapses = 0;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        // Collect neurons above activity threshold
        std::vector<NeuronId> activeNeurons;
        for (auto* neuron : neurons) {
            if (neuron->getState().firingRate > minActivityThreshold) {
                activeNeurons.push_back(neuron->getId());
            }
        }
        
        // Create synapses between highly active neurons
        if (activeNeurons.size() > 1) {
            for (size_t i = 0; i < activeNeurons.size(); ++i) {
                for (size_t j = i + 1; j < activeNeurons.size(); ++j) {
                    // Probabilistic formation with higher rate for active neurons
                    if (rng.bernoulli(pImpl->synaptogenesisRate * 2.0f)) {
                        // Check synapse limit for source neuron
                        auto synapsesFrom = region->getSynapsesFrom(activeNeurons[i]);
                        if (synapsesFrom.size() < maxSynapsesPerNeuron) {
                            float weight = 0.2f + rng.uniformReal(0.0f, 0.3f);
                            if (createSynapse(brain, activeNeurons[i], activeNeurons[j], weight) != INVALID_SYNAPSE_ID) {
                                ++newSynapses;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return newSynapses;
}

void StructuralPlasticity::synapseCompetition(Brain* brain, 
                                            class RandomGenerator& rng) {
    // Simplified synapse competition - weaker synapses are more likely to be eliminated
    // In real implementation, would use more sophisticated competition rules
    
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        
        // Sort by weight (weakest first)
        std::sort(synapses.begin(), synapses.end(),
                 [](const Synapse* a, const Synapse* b) {
                     return std::abs(a->getWeight()) < std::abs(b->getWeight());
                 });
        
        // Eliminate some of the weakest synapses
        size_t numToEliminate = std::max(size_t(1), 
                                       static_cast<size_t>(synapses.size() * pImpl->synapticCompetitionStrength * 0.01f));
        
        for (size_t i = 0; i < numToEliminate && i < synapses.size(); ++i) {
            if (rng.bernoulli(0.3f)) {  // 30% chance of elimination
                removeSynapse(brain, synapses[i]->getId());
            }
        }
    }
}

void StructuralPlasticity::updateDendriticSpineDynamics(Brain* brain,
                                                       class RandomGenerator& rng,
                                                       const std::vector<NeuronActivity>& activities) {
    // Implement dendritic spine dynamics
    // Spines form more frequently on active neurons
    
    for (const auto& activity : activities) {
        if (activity.activity > 1.0f) {  // High activity threshold
            // Spine formation probability increases with activity
            float spineFormationProb = pImpl->spineFormationRate * activity.activity;
            if (rng.bernoulli(spineFormationProb)) {
                // Request spine formation
                std::vector<float> position = {0.0f, 0.0f, 0.0f};  // Placeholder
                formDendriticSpine(brain, activity.id, position);
            }
        }
        
        // Spine elimination increases with time (activity-dependent)
        float spineEliminationProb = pImpl->spineEliminationRate * activity.avgWeight;
        if (rng.bernoulli(spineEliminationProb)) {
            eliminateDendriticSpine(brain, activity.id, 0);  // First spine
        }
    }
}

void StructuralPlasticity::updateAxonalDynamics(Brain* brain,
                                               class RandomGenerator& rng,
                                               const std::vector<NeuronActivity>& activities) {
    // Implement axonal sprouting and retraction
    
    if (activities.size() < 2) return;
    
    // Choose random source and target for potential sprouting
    size_t srcIdx = rng.uniformInt(0, static_cast<int>(activities.size()) - 1);
    size_t tgtIdx = rng.uniformInt(0, static_cast<int>(activities.size()) - 1);
    
    if (srcIdx != tgtIdx && activities[srcIdx].activity > 0.5f && activities[tgtIdx].activity > 0.5f) {
        // Sprouting probability based on activity and rate
        float sproutingProb = pImpl->axonalSproutingRate * 
                             (activities[srcIdx].activity + activities[tgtIdx].activity) * 0.5f;
        
        if (rng.bernoulli(sproutingProb)) {
            sproutAxon(brain, activities[srcIdx].id, activities[tgtIdx].id, sproutingProb);
        } else {
            // Occasional retraction
            if (rng.bernoulli(pImpl->axonalRetractionRate)) {
                // Pick a random existing synapse
                for (auto& region : brain->getRegions()) {
                    auto synapses = region->getSynapses();
                    if (!synapses.empty()) {
                        size_t idx = rng.uniformInt(0, static_cast<int>(synapses.size()) - 1);
                        retractAxon(brain, synapses[idx]->getId());
                        break;
                    }
                }
            }
        }
    }
}

void StructuralPlasticity::synapseCompetition(Brain* brain, RandomGenerator& rng) {
    // Wrapper for region-specific synapse competition
    for (auto& region : brain->getRegions()) {
        synapseCompetition(brain, region, rng);
    }
}

void StructuralPlasticity::synapseCompetition(Brain* brain, 
                                            const class NeuralRegion* region,
                                            RandomGenerator& rng) {
    // Synapse competition for a specific region
    if (!region) return;
    
    auto synapses = region->getSynapses();
    
    // Sort by weight (weakest first)
    std::vector<Synapse*> sortedSynapses(synapses.begin(), synapses.end());
    std::sort(sortedSynapses.begin(), sortedSynapses.end(),
             [](const Synapse* a, const Synapse* b) {
                 return std::abs(a->getWeight()) < std::abs(b->getWeight());
             });
    
    // Eliminate some of the weakest synapses
    size_t numToEliminate = std::max(size_t(1), 
                                   static_cast<size_t>(sortedSynapses.size() * pImpl->synapticCompetitionStrength * 0.01f));
    
    for (size_t i = 0; i < numToEliminate && i < sortedSynapses.size(); ++i) {
        if (rng.bernoulli(0.3f)) {  // 30% chance of elimination
            removeSynapse(brain, sortedSynapses[i]->getId());
        }
    }
}

// NEW: Additional structural plasticity parameters

void StructuralPlasticity::setSpineFormationRate(float rate) {
    pImpl->spineFormationRate = std::clamp(rate, 0.0f, 0.1f);
}

float StructuralPlasticity::getSpineFormationRate() const {
    return pImpl->spineFormationRate;
}

void StructuralPlasticity::setSpineEliminationRate(float rate) {
    pImpl->spineEliminationRate = std::clamp(rate, 0.0f, 0.05f);
}

float StructuralPlasticity::getSpineEliminationRate() const {
    return pImpl->spineEliminationRate;
}

void StructuralPlasticity::setAxonalSproutingRate(float rate) {
    pImpl->axonalSproutingRate = std::clamp(rate, 0.0f, 0.05f);
}

float StructuralPlasticity::getAxonalSproutingRate() const {
    return pImpl->axonalSproutingRate;
}

void StructuralPlasticity::setAxonalRetractionRate(float rate) {
    pImpl->axonalRetractionRate = std::clamp(rate, 0.0f, 0.01f);
}

float StructuralPlasticity::getAxonalRetractionRate() const {
    return pImpl->axonalRetractionRate;
}

} // namespace nlm
