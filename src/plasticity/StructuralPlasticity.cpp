#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../brain/NeuralPopulation.hpp"
#include "../../core/Types/Types.hpp"
#include "../../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <numeric>
#include <random>

namespace nlm {

struct StructuralPlasticity::Impl {
    // ========== SYNAPTOGENESIS PARAMETERS ==========
    float synaptogenesisRate;              // Base probability of new synapse formation
    float hebbianLearningRate;            // Hebbian synaptogenesis learning rate
    float targetSynapticDensity;          // Target synaptic density (0.0-1.0)
    float criticalPeriodModulation;       // Critical period modulation factor (0.0-1.0)
    bool experienceExpectantPlasticity;  // Enable experience-expectant plasticity
    
    // ========== PRUNING PARAMETERS ==========
    float pruningRate;                    // Base pruning probability
    float pruningThreshold;               // Minimum weight threshold for pruning
    float activityThreshold;              // Minimum activity threshold for synaptic stability
    float competitivePruningLevel;        // Level of competitive pruning (0.0-1.0)
    float homeostaticTargetFiringRate;   // Target firing rate for homeostatic pruning
    
    // ========== SPATIAL CONSTRAINTS ==========
    float maxConnectionDistance;          // Maximum distance for local connections
    bool layerSpecificRules;              // Enable layer-specific connectivity rules
    bool dendriticCompartmentConstraints; // Enable dendritic compartment constraints
    
    // ========== NEURON TYPE-SPECIFIC PARAMETERS ==========
    float excitatorySynapseFormationRate; // Excitatory synapse formation rate
    float inhibitorySynapseFormationRate; // Inhibitory synapse formation rate
    bool sensorySpecificRules;            // Enable sensory neuron-specific rules
    bool motorSpecificRules;              // Enable motor neuron-specific rules
    
    // ========== DEVELOPMENTAL PARAMETERS ==========
    DevelopmentalStage currentStage;      // Current developmental stage
    float developmentalModulation;        // Current modulation factor based on stage
    
    // ========== EFFICIENCY PARAMETERS ==========
    bool enableBatchOperations;           // Enable batch operations for efficiency
    size_t batchSize;                      // Size of batch operations
    bool efficiencyMode;                  // Enable efficiency optimizations
    
    // ========== SYNAPSE MATURATION ==========
    float maturationTimeConstant;         // Time constant for synapse maturation
    float stabilityThreshold;             // Threshold for synapse stabilization
    float weakeningRate;                   // Rate of synaptic weakening
    
    // ========== STATISTICS ==========
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
    size_t hebbianSynapsesCreated;
    size_t localSynapsesCreated;
    size_t developmentallyPruned;
    size_t competitivePruned;
    size_t unusedPruned;
    
    // Synapse activity tracking
    std::unordered_map<SynapseId, float> synapseWeights;
    std::unordered_map<SynapseId, SynapseMaturationStage> synapseStages;
    std::unordered_map<SynapseId, float> synapseStability;
    std::unordered_map<SynapseId, float> synapseMaturationProgress;
    
    // Neuron activity tracking for competitive pruning
    std::unordered_map<NeuronId, float> neuronFiringRates;
    std::unordered_map<NeuronId, size_t> neuronSynapseCounts;
    std::unordered_map<NeuronId, float> neuronActivityHistory;
    
    // Structural memory
    std::vector<std::tuple<SynapseId, float, Timestamp>> structuralEvents;
    
    Impl() : 
        // Initialize synaptogenesis parameters
        synaptogenesisRate(0.0005f), 
        hebbianLearningRate(0.01f),
        targetSynapticDensity(0.05f),  // 5% of possible connections
        criticalPeriodModulation(1.0f),
        experienceExpectantPlasticity(true),
        
        // Initialize pruning parameters
        pruningRate(0.0001f),
        pruningThreshold(0.05f),
        activityThreshold(0.001f),
        competitivePruningLevel(0.5f),
        homeostaticTargetFiringRate(5.0f), // Hz
        
        // Initialize spatial parameters
        maxConnectionDistance(100.0f),   // Arbitrary distance units
        layerSpecificRules(true),
        dendriticCompartmentConstraints(true),
        
        // Initialize neuron type parameters
        excitatorySynapseFormationRate(0.006f),
        inhibitorySynapseFormationRate(0.002f),
        sensorySpecificRules(true),
        motorSpecificRules(true),
        
        // Initialize developmental parameters
        currentStage(DevelopmentalStage::Initial),
        developmentalModulation(1.0f),
        
        // Initialize efficiency parameters
        enableBatchOperations(true),
        batchSize(100),
        efficiencyMode(true),
        
        // Initialize synapse maturation parameters
        maturationTimeConstant(50.0f),    // Steps
        stabilityThreshold(0.7f),         // 0.0-1.0 stability
        weakeningRate(0.005f),             // Weight decay rate
        
        // Initialize statistics
        totalSynapsesCreated(0),
        totalSynapsesPruned(0),
        hebbianSynapsesCreated(0),
        localSynapsesCreated(0),
        developmentallyPruned(0),
        competitivePruned(0),
        unusedPruned(0) {}
};

StructuralPlasticity::StructuralPlasticity() : pImpl(new Impl) {}

StructuralPlasticity::~StructuralPlasticity() = default;

SynapseId StructuralPlasticity::createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                                           SynapseType type, float baseWeight) {
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Apply neuron type-specific rules
    Neuron* sourceNeuron = nullptr;
    Neuron* destNeuron = nullptr;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == source) sourceNeuron = neuron;
            if (neuron->getId() == destination) destNeuron = neuron;
        }
    }
    
    if (!sourceNeuron || !destNeuron) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Check neuron type compatibility
    NeuronType sourceType = sourceNeuron->getType();
    NeuronType destType = destNeuron->getType();
    
    // Special rules for sensory neurons (can only receive input)
    if (sourceType == NeuronType::Sensory) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Special rules for motor neurons (can only send output)
    if (destType == NeuronType::Motor) {
        // Motor neurons can receive from internal neurons
        if (sourceType != NeuronType::Internal && sourceType != NeuronType::Sensory) {
            return INVALID_SYNAPSE_ID;
        }
    }
    
    // Check spatial constraints if enabled
    if (pImpl->dendriticCompartmentConstraints) {
        // Get neuron positions (simplified - using IDs for distance calculation)
        float distance = static_cast<float>(std::abs(source.index() - destination.index()));
        if (distance > pImpl->maxConnectionDistance) {
            return INVALID_SYNAPSE_ID;
        }
    }
    
    // Create the synapse using region's implementation
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        bool sourceFound = false;
        bool destFound = false;
        
        for (auto* neuron : neurons) {
            if (neuron->getId() == source) sourceFound = true;
            if (neuron->getId() == destination) destFound = true;
        }
        
        if (sourceFound && destFound) {
            // Use existing synapse creation with appropriate weight
            float weight = baseWeight;
            if (type == SynapseType::Inhibitory) {
                weight = -baseWeight;
            }
            
            SynapseId synId = region->addSynapse(source, destination, weight, 1);
            if (synId != INVALID_SYNAPSE_ID) {
                // Initialize synapse properties
                Synapse* synapse = region->getSynapse(synId);
                if (synapse) {
                    synapse->setType(type);
                    synapse->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
                    
                    // Initialize synapse stage and stability
                    pImpl->synapseStages[synId] = SynapseMaturationStage::Initial;
                    pImpl->synapseStability[synId] = 0.0f;
                    pImpl->synapseMaturationProgress[synId] = 0.0f;
                    pImpl->synapseWeights[synId] = weight;
                    
                    ++pImpl->totalSynapsesCreated;
                }
                return synId;
            }
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

bool StructuralPlasticity::createSynapseHebbian(Brain* brain, NeuronId pre, NeuronId post,
                                              const std::vector<Timestamp>& preSpikes,
                                              const std::vector<Timestamp>& postSpikes,
                                              RandomGenerator& rng) {
    if (!brain) return false;
    
    // Calculate firing rate correlation
    float preRate = calculateFiringRate(preSpikes);
    float postRate = calculateFiringRate(postSpikes);
    
    // Hebbian rule: co-activation probability
    float correlation = calculateCorrelation(preSpikes, postSpikes);
    
    // Probability of synapse formation based on correlation
    float formationProb = pImpl->hebbianLearningRate * correlation;
    formationProb = std::min(formationProb, 1.0f);
    
    if (!rng.bernoulli(formationProb)) {
        return false;
    }
    
    // Check if synapses already exist
    bool exists = false;
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(post);
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == pre) {
                exists = true;
                break;
            }
        }
        if (exists) break;
    }
    
    if (exists) {
        return false;
    }
    
    // Create new synapse
    float weight = 0.1f + rng.uniformReal(0.0f, 0.2f);
    SynapseId synId = createSynapse(brain, pre, post, SynapseType::Excitatory, weight);
    
    if (synId != INVALID_SYNAPSE_ID) {
        ++pImpl->hebbianSynapsesCreated;
        return true;
    }
    
    return false;
}

bool StructuralPlasticity::createSynapseLocal(Brain* brain, NeuronId source, NeuronId destination,
                                            float maxDistance, RandomGenerator& rng) {
    if (!brain) return false;
    
    // Calculate distance between neurons (simplified)
    float distance = static_cast<float>(std::abs(source.index() - destination.index()));
    
    // Distance-dependent probability
    float distanceFactor = std::exp(-distance / maxDistance);
    
    // Local connection probability
    float localProb = pImpl->synaptogenesisRate * distanceFactor;
    
    if (!rng.bernoulli(localProb)) {
        return false;
    }
    
    // Check existing synapse
    bool exists = false;
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                exists = true;
                break;
            }
        }
        if (exists) break;
    }
    
    if (exists) {
        return false;
    }
    
    // Create synapse
    float weight = 0.05f + rng.uniformReal(0.0f, 0.1f);
    SynapseId synId = createSynapse(brain, source, destination, SynapseType::Excitatory, weight);
    
    if (synId != INVALID_SYNAPSE_ID) {
        ++pImpl->localSynapsesCreated;
        return true;
    }
    
    return false;
}

bool StructuralPlasticity::createSynapseForType(Brain* brain, NeuronId source, NeuronId destination,
                                              NeuronType sourceType, NeuronType destType,
                                              RandomGenerator& rng) {
    if (!brain) return false;
    
    float formationRate = pImpl->excitatorySynapseFormationRate;
    
    // Adjust rate based on neuron types
    if (sourceType == NeuronType::Inhibitory) {
        formationRate = pImpl->inhibitorySynapseFormationRate;
    } else if (sourceType == NeuronType::Sensory && pImpl->sensorySpecificRules) {
        formationRate *= 0.5f;  // Sensory neurons have lower formation rates
    } else if (sourceType == NeuronType::Motor && pImpl->motorSpecificRules) {
        formationRate *= 0.3f;  // Motor neurons have lower formation rates
    }
    
    if (!rng.bernoulli(formationRate)) {
        return false;
    }
    
    // Check existing synapse
    bool exists = false;
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                exists = true;
                break;
            }
        }
        if (exists) break;
    }
    
    if (exists) {
        return false;
    }
    
    // Create synapse with appropriate type
    SynapseType synType = (sourceType == NeuronType::Inhibitory) ? 
                          SynapseType::Inhibitory : SynapseType::Excitatory;
    
    float weight = (synType == SynapseType::Inhibitory) ? 
                  -(0.05f + rng.uniformReal(0.0f, 0.1f)) : 
                  (0.05f + rng.uniformReal(0.0f, 0.1f));
    
    SynapseId synId = createSynapse(brain, source, destination, synType, std::abs(weight));
    
    return synId != INVALID_SYNAPSE_ID;
}

bool StructuralPlasticity::createSynapseForDevelopment(Brain* brain, NeuronId source, NeuronId destination,
                                                     DevelopmentalStage stage, RandomGenerator& rng) {
    if (!brain) return false;
    
    // Apply developmental stage modulation
    float stageModulation = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            stageModulation = 1.5f;  // High plasticity during initial stage
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageModulation = 1.2f;
            break;
        case DevelopmentalStage::Maturation:
            stageModulation = 0.8f;
            break;
        case DevelopmentalStage::Adult:
            stageModulation = 0.5f;
            break;
        case DevelopmentalStage::Aging:
            stageModulation = 0.3f;
            break;
    }
    
    // Critical period modulation
    if (pImpl->criticalPeriodModulation > 0.0f) {
        stageModulation *= pImpl->criticalPeriodModulation;
    }
    
    float formationRate = pImpl->synaptogenesisRate * stageModulation;
    formationRate = std::min(formationRate, 1.0f);
    
    if (!rng.bernoulli(formationRate)) {
        return false;
    }
    
    // Create synapse
    float weight = 0.05f + rng.uniformReal(0.0f, 0.15f);
    SynapseId synId = createSynapse(brain, source, destination, SynapseType::Excitatory, weight);
    
    if (synId != INVALID_SYNAPSE_ID) {
        ++pImpl->developmentallyPruned;
        return true;
    }
    
    return false;
}

bool StructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) {
        return false;
    }
    
    // Find and remove synapse from all regions
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            // Remove from connectivity maps
            region->getSynapses();  // Access the vector
            // Note: In a real implementation, we would need to modify the region's internal structures
            // For now, we'll just mark it as pruned
            syn->setWeight(0.0f);
            ++pImpl->totalSynapsesPruned;
            
            // Update statistics
            if (pImpl->synapseStages.find(synapse) != pImpl->synapseStages.end()) {
                if (pImpl->synapseStages[synapse] == SynapseMaturationStage::Stable) {
                    ++pImpl->competitivePruned;
                }
            }
            
            return true;
        }
    }
    
    return false;
}

void StructuralPlasticity::pruneCompetitively(Brain* brain, RandomGenerator& rng, float competitionLevel) {
    if (!brain || competitionLevel <= 0.0f) return;
    
    // Collect all synapses with their weights
    std::vector<std::pair<SynapseId, float>> allSynapses;
    
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        for (const auto& syn : synapses) {
            if (syn && syn->getWeight() != 0.0f) {
                allSynapses.push_back({syn->getId(), std::abs(syn->getWeight())});
            }
        }
    }
    
    if (allSynapses.empty()) return;
    
    // Sort by weight (lower weights first for competitive pruning)
    std::sort(allSynapses.begin(), allSynapses.end(), 
              [](const auto& a, const auto& b) { return a.second < b.second; });n
    // Calculate how many synapses to prune based on competition level
    size_t pruneCount = static_cast<size_t>(allSynapses.size() * competitionLevel * pImpl->pruningRate);
    pruneCount = std::min(pruneCount, allSynapses.size());
    
    for (size_t i = 0; i < pruneCount; ++i) {
        SynapseId synId = allSynapses[i].first;
        
        // Remove the synapse
        removeSynapse(brain, synId);
    }
}

void StructuralPlasticity::prunePerNeuron(Brain* brain, NeuronId neuron, RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate neuron firing rate
    float firingRate = 0.0f;
    if (pImpl->neuronFiringRates.find(neuron) != pImpl->neuronFiringRates.end()) {
        firingRate = pImpl->neuronFiringRates[neuron];
    }
    
    // Compare with target firing rate
    float rateDifference = std::abs(firingRate - pImpl->homeostaticTargetFiringRate);
    
    // Determine pruning probability based on deviation from target
    float pruningProb = pImpl->pruningRate * (rateDifference / pImpl->homeostaticTargetFiringRate);
    pruningProb = std::min(pruningProb, 1.0f);
    
    // Get outgoing synapses from this neuron
    for (auto& region : brain->getRegions()) {
        auto synapsesFrom = region->getSynapsesFrom(neuron);
        
        for (Synapse* syn : synapsesFrom) {
            if (!rng.bernoulli(pruningProb)) continue;
            
            // Prune this synapse
            removeSynapse(brain, syn->getId());
        }
    }
}

void StructuralPlasticity::pruneUnused(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Find weakly active synapses
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        
        for (Synapse* syn : synapses) {
            if (!syn) continue;
            
            float weight = std::abs(syn->getWeight());
            
            // Check pruning conditions
            bool shouldPrune = false;
            
            if (weight < pImpl->pruningThreshold) {
                // Synapse is too weak
                if (rng.bernoulli(pImpl->pruningRate)) {
                    shouldPrune = true;
                }
            } else if (pImpl->useActivityDependency) {
                // Check activity threshold
                float eligibility = std::abs(syn->getEligibilityTrace());
                if (eligibility < pImpl->activityThreshold) {
                    if (rng.bernoulli(pImpl->pruningRate * 0.5f)) {
                        shouldPrune = true;
                    }
                }
            }
            
            if (shouldPrune) {
                removeSynapse(brain, syn->getId());
                ++pImpl->unusedPruned;
            }
        }
    }
}

void StructuralPlasticity::pruneForDevelopment(Brain* brain, DevelopmentalStage stage, RandomGenerator& rng) {
    if (!brain) return;
    
    float stageModulation = 1.0f;
    
    // Apply stage-specific modulation
    switch (stage) {
        case DevelopmentalStage::Initial:
            stageModulation = 1.5f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageModulation = 1.2f;
            break;
        case DevelopmentalStage::Maturation:
            stageModulation = 0.8f;
            break;
        case DevelopmentalStage::Adult:
            stageModulation = 0.5f;
            break;
        case DevelopmentalStage::Aging:
            stageModulation = 0.3f;
            break;
    }
    
    float pruningProb = pImpl->pruningRate * stageModulation;
    pruningProb = std::min(pruningProb, 1.0f);
    
    // Prune based on developmental stage
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        
        for (Synapse* syn : synapses) {
            if (!syn) continue;
            
            // Random developmental pruning
            if (rng.bernoulli(pruningProb)) {
                removeSynapse(brain, syn->getId());
                ++pImpl->developmentallyPruned;
            }
        }
    }
}

NeuronId StructuralPlasticity::createNeuron(Brain* brain, NeuronType type, float metabolicCost) {
    // Placeholder implementation - in real system would add neuron to a population
    // For now, just return invalid ID
    (void)metabolicCost;
    
    if (!brain) {
        return INVALID_NEURON_ID;
    }
    
    // Find a region to add neuron to
    for (auto& region : brain->getRegions()) {
        // Add neuron to first available population
        auto populations = region->getPopulations();
        if (!populations.empty()) {
            // In real implementation, this would actually create and add a neuron
            // For now, return a placeholder
            return NeuronId(12345);  // Placeholder ID
        }
    }
    
    return INVALID_NEURON_ID;
}

bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    if (!brain || neuron == INVALID_NEURON_ID) {
        return false;
    }
    
    // Find and remove all synapses connected to this neuron
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        
        for (Synapse* syn : synapses) {
            if (syn && (syn->getSourceNeuron() == neuron || syn->getDestinationNeuron() == neuron)) {
                removeSynapse(brain, syn->getId());
            }
        }
    }
    
    // In real implementation, would also remove the neuron itself
    return true;
}

bool StructuralPlasticity::stabilizeSynapse(Brain* brain, SynapseId synapse, float stabilityThreshold) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) {
        return false;
    }
    
    // Find synapse in regions
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            if (pImpl->synapseStability.find(synapse) != pImpl->synapseStability.end()) {
                float stability = pImpl->synapseStability[synapse];
                if (stability >= stabilityThreshold) {
                    // Synapse is stable
                    if (pImpl->synapseStages.find(synapse) != pImpl->synapseStages.end()) {
                        pImpl->synapseStages[synapse] = SynapseMaturationStage::Stable;
                    }
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool StructuralPlasticity::matureSynapse(Brain* brain, SynapseId synapse, SynapseMaturationStage stage,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) {
        return false;
    }
    
    float maturationProgress = 0.0f;
    
    // Calculate maturation progress based on stage and activity
    switch (stage) {
        case SynapseMaturationStage::Initial:
            maturationProgress = calculateMaturationProgress(preSpikes, postSpikes);
            break;
        case SynapseMaturationStage::Potentiating:
            maturationProgress = std::min(1.0f, maturationProgress + 0.1f);
            break;
        case SynapseMaturationStage::Stable:
            maturationProgress = 1.0f;
            break;
        case SynapseMaturationStage::Depressing:
            maturationProgress = calculateMaturationProgress(preSpikes, postSpikes) * 0.5f;
            break;
        case SynapseMaturationStage::Pruned:
            return false;
    }
    
    // Update synapse state
    pImpl->synapseMaturationProgress[synapse] = maturationProgress;
    
    // Update stability based on activity
    float activityFactor = calculateActivityFactor(preSpikes, postSpikes);
    pImpl->synapseStability[synapse] = maturationProgress * activityFactor;
    
    return true;
}

bool StructuralPlasticity::weakenSynapse(Brain* brain, SynapseId synapse, float decayRate) {
    if (!brain || synapse == INVALID_SYNAPSE_ID || decayRate <= 0.0f) {
        return false;
    }
    
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            float currentWeight = syn->getWeight();
            float newWeight = currentWeight * (1.0f - decayRate);
            
            // Ensure weight doesn't cross zero sign
            if (currentWeight < 0 && newWeight > 0) {
                newWeight = 0.0f;
            } else if (currentWeight > 0 && newWeight < 0) {
                newWeight = 0.0f;
            }
            
            syn->setWeight(newWeight);
            pImpl->synapseWeights[synapse] = newWeight;
            
            // If weight becomes very small, mark for pruning
            if (std::abs(newWeight) < pImpl->pruningThreshold) {
                // Don't immediately prune, just weaken
            }
            
            return true;
        }
    }
    
    return false;
}

void StructuralPlasticity::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.1f);
}

void StructuralPlasticity::setHebbianLearningRate(float rate) {
    pImpl->hebbianLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

void StructuralPlasticity::setTargetSynapticDensity(float density) {
    pImpl->targetSynapticDensity = std::clamp(density, 0.0f, 1.0f);
}

void StructuralPlasticity::setCriticalPeriodModulation(float modulation) {
    pImpl->criticalPeriodModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void StructuralPlasticity::setExperienceExpectantPlasticity(bool enabled) {
    pImpl->experienceExpectantPlasticity = enabled;
}

void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

void StructuralPlasticity::setPruningThreshold(float threshold) {
    pImpl->pruningThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void StructuralPlasticity::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void StructuralPlasticity::setCompetitivePruningLevel(float level) {
    pImpl->competitivePruningLevel = std::clamp(level, 0.0f, 1.0f);
}

void StructuralPlasticity::setHomeostaticTargetFiringRate(float rate) {
    pImpl->homeostaticTargetFiringRate = std::max(0.0f, rate);
}

void StructuralPlasticity::setMaxConnectionDistance(float distance) {
    pImpl->maxConnectionDistance = std::max(0.0f, distance);
}

void StructuralPlasticity::setLayerSpecificRules(bool enabled) {
    pImpl->layerSpecificRules = enabled;
}

void StructuralPlasticity::setDendriticCompartmentConstraints(bool enabled) {
    pImpl->dendriticCompartmentConstraints = enabled;
}

void StructuralPlasticity::setExcitatorySynapseFormationRate(float rate) {
    pImpl->excitatorySynapseFormationRate = std::clamp(rate, 0.0f, 1.0f);
}

void StructuralPlasticity::setInhibitorySynapseFormationRate(float rate) {
    pImpl->inhibitorySynapseFormationRate = std::clamp(rate, 0.0f, 1.0f);
}

void StructuralPlasticity::setSensorySpecificRules(bool enabled) {
    pImpl->sensorySpecificRules = enabled;
}

void StructuralPlasticity::setMotorSpecificRules(bool enabled) {
    pImpl->motorSpecificRules = enabled;
}

void StructuralPlasticity::setDevelopmentStage(DevelopmentalStage stage) {
    pImpl->currentStage = stage;
    
    // Set modulation based on stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            pImpl->developmentalModulation = 1.5f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->developmentalModulation = 1.2f;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->developmentalModulation = 0.8f;
            break;
        case DevelopmentalStage::Adult:
            pImpl->developmentalModulation = 0.5f;
            break;
        case DevelopmentalStage::Aging:
            pImpl->developmentalModulation = 0.3f;
            break;
    }
}

void StructuralPlasticity::setDevelopmentalStageModulation(float modulation) {
    pImpl->developmentalModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void StructuralPlasticity::enableBatchOperations(bool enabled) {
    pImpl->enableBatchOperations = enabled;
}

void StructuralPlasticity::setBatchSize(size_t size) {
    pImpl->batchSize = std::max(static_cast<size_t>(1), size);
}

void StructuralPlasticity::setEfficiencyMode(bool enabled) {
    pImpl->efficiencyMode = enabled;
}

float StructuralPlasticity::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

float StructuralPlasticity::getCurrentSynapticDensity() const {
    // Calculate current synaptic density
    size_t totalNeurons = 0;
    size_t totalPossibleSynapses = 0;
    
    for (const auto& region : pImpl->brainRegions) {
        totalNeurons += region->getTotalNeuronCount();
        totalPossibleSynapses += totalNeurons * (totalNeurons - 1);
    }
    
    if (totalPossibleSynapses == 0) return 0.0f;
    
    return static_cast<float>(pImpl->totalSynapsesCreated) / 
           static_cast<float>(totalPossibleSynapses);
}

float StructuralPlasticity::getTotalSynapticStability() const {
    if (pImpl->synapseStability.empty()) return 0.0f;
    
    float totalStability = 0.0f;
    for (const auto& pair : pImpl->synapseStability) {
        totalStability += pair.second;
    }
    
    return totalStability / static_cast<float>(pImpl->synapseStability.size());
}

size_t StructuralPlasticity::getTotalSynapsesCreated() const {
    return pImpl->totalSynapsesCreated;
}

size_t StructuralPlasticity::getTotalSynapsesPruned() const {
    return pImpl->totalSynapsesPruned;
}

float StructuralPlasticity::getActivityDependentPrunedRatio() const {
    if (pImpl->totalSynapsesPruned == 0) return 0.0f;
    return static_cast<float>(pImpl->unusedPruned) / 
           static_cast<float>(pImpl->totalSynapsesPruned);
}

float StructuralPlasticity::getCompetitivePruningRatio() const {
    if (pImpl->totalSynapsesPruned == 0) return 0.0f;
    return static_cast<float>(pImpl->competitivePruned) / 
           static_cast<float>(pImpl->totalSynapsesPruned);
}

float StructuralPlasticity::getHebbianSynaptogenesisRatio() const {
    if (pImpl->totalSynapsesCreated == 0) return 0.0f;
    return static_cast<float>(pImpl->hebbianSynapsesCreated) / 
           static_cast<float>(pImpl->totalSynapsesCreated);
}

float StructuralPlasticity::getLocalConnectivityRatio() const {
    if (pImpl->totalSynapsesCreated == 0) return 0.0f;
    return static_cast<float>(pImpl->localSynapsesCreated) / 
           static_cast<float>(pImpl->totalSynapsesCreated);
}

float StructuralPlasticity::getMaturationStageRatio(SynapseMaturationStage stage) const {
    size_t count = 0;
    for (const auto& pair : pImpl->synapseStages) {
        if (pair.second == stage) {
            count++;
        }
    }
    
    size_t total = 0;
    for (const auto& pair : pImpl->synapseStages) {
        total++;
    }
    
    if (total == 0) return 0.0f;
    return static_cast<float>(count) / static_cast<float>(total);
}

size_t StructuralPlasticity::getStablyConnectedSynapses() const {
    size_t count = 0;
    for (const auto& pair : pImpl->synapseStages) {
        if (pair.second == SynapseMaturationStage::Stable) {
            count++;
        }
    }
    return count;
}

size_t StructuralPlasticity::getUnstableSynapses() const {
    size_t count = 0;
    for (const auto& pair : pImpl->synapseStages) {
        if (pair.second != SynapseMaturationStage::Stable && 
            pair.second != SynapseMaturationStage::Pruned) {
            count++;
        }
    }
    return count;
}

void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Update developmental stage modulation
    float stageModulation = 1.0f;
    switch (pImpl->currentStage) {
        case DevelopmentalStage::Initial:
            stageModulation = 1.5f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageModulation = 1.2f;
            break;
        case DevelopmentalStage::Maturation:
            stageModulation = 0.8f;
            break;
        case DevelopmentalStage::Adult:
            stageModulation = 0.5f;
            break;
        case DevelopmentalStage::Aging:
            stageModulation = 0.3f;
            break;
    }
    
    pImpl->developmentalModulation = stageModulation;
    
    // Apply developmentally-modulated rates
    float currentSynaptogenesisRate = pImpl->synaptogenesisRate * pImpl->developmentalModulation;
    float currentPruningRate = pImpl->pruningRate * (2.0f - pImpl->developmentalModulation);
    
    // Update synaptic density based on target
    float currentDensity = getCurrentSynapticDensity();
    float densityError = pImpl->targetSynapticDensity - currentDensity;
    
    // Adjust rates based on density error
    if (densityError > 0.01f) {
        // Need more synapses
        currentSynaptogenesisRate *= (1.0f + densityError * 2.0f);
    } else if (densityError < -0.01f) {
        // Need to prune more synapses
        currentPruningRate *= (1.0f + std::abs(densityError) * 2.0f);
    }
    
    // ========== SYNAPTOGENESIS ==========
    
    // Activity-dependent synaptogenesis (Hebbian)
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Collect neuron activities
        std::vector<std::pair<NeuronId, float>> neuronActivities;
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            
            // Calculate activity from spike history
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                // More recent spikes = higher activity
                activity = static_cast<float>(spikes.size());
            }
            
            neuronActivities.push_back({neuron->getId(), activity});
            pImpl->neuronFiringRates[neuron->getId()] = activity;
        }
        
        // Hebbian synaptogenesis
        if (region->getSynapseCount() < pImpl->maxTotalSynapses * 0.9f) {
            for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                if (!rng.bernoulli(currentSynaptogenesisRate)) continue;
                
                size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                
                if (idx1 != idx2) {
                    NeuronId src = neuronActivities[idx1].first;
                    NeuronId dst = neuronActivities[idx2].first;
                    
                    // Only create if both neurons are somewhat active
                    if (neuronActivities[idx1].second > 0.1f || 
                        neuronActivities[idx2].second > 0.1f) {
                        
                        // Create Hebbian synapse
                        float weight = 0.1f + rng.uniformReal(0.0f, 0.2f);
                        createSynapse(brain, src, dst, SynapseType::Excitatory, weight);
                    }
                }
            }
        }
    }
    
    // ========== PRUNING ==========
    
    // Competitive pruning for stability
    pruneCompetitively(brain, rng, pImpl->competitivePruningLevel);
    
    // Per-neuron competitive pruning
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            prunePerNeuron(brain, neuron->getId(), rng);
        }
    }
    
    // Activity-dependent pruning
    pruneUnused(brain, rng);
    
    // Development-stage specific pruning
    pruneForDevelopment(brain, pImpl->currentStage, rng);
    
    // ========== MATURATION ==========
    
    // Update synapse maturation
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        
        for (Synapse* syn : synapses) {
            if (!syn) continue;
            
            // Get spike history for this synapse
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            // Update maturation stage
            SynapseMaturationStage currentStage = SynapseMaturationStage::Initial;
            if (pImpl->synapseStages.find(syn->getId()) != pImpl->synapseStages.end()) {
                currentStage = pImpl->synapseStages[syn->getId()];
            }
            
            // Mature synapse based on activity
            if (!preSpikes.empty() && !postSpikes.empty()) {
                matureSynapse(brain, syn->getId(), currentStage, preSpikes, postSpikes);
                
                // Try to stabilize if mature enough
                if (pImpl->synapseMaturationProgress.find(syn->getId()) != 
                    pImpl->synapseMaturationProgress.end()) {
                    float progress = pImpl->synapseMaturationProgress[syn->getId()];
                    if (progress > 0.8f) {
                        stabilizeSynapse(brain, syn->getId(), pImpl->stabilityThreshold);
                    }
                }
            }
        }
    }
    
    // ========== EFFICIENCY OPERATIONS ==========
    
    if (pImpl->enableBatchOperations) {
        // Perform batch synaptogenesis if needed
        if (currentDensity < pImpl->targetSynapticDensity * 0.8f) {
            batchCreateSynapses(brain, generateConnectionPairs(brain, pImpl->batchSize), rng);
        }
        
        // Perform batch pruning if needed
        if (currentDensity > pImpl->targetSynapticDensity * 1.2f) {
            batchRemoveSynapses(brain, selectSynapsesForPruning(brain, pImpl->batchSize));
        }
    }
}

std::vector<SynapseId> StructuralPlasticity::batchCreateSynapses(Brain* brain,
                                                                const std::vector<std::pair<NeuronId, NeuronId>>& connections,
                                                                RandomGenerator& rng) {
    std::vector<SynapseId> createdSynapses;
    
    if (!pImpl->enableBatchOperations || connections.empty()) {
        return createdSynapses;
    }
    
    createdSynapses.reserve(connections.size());
    
    for (const auto& conn : connections) {
        // Apply spatial constraints if needed
        float distance = static_cast<float>(std::abs(conn.first.index() - conn.second.index()));
        if (distance > pImpl->maxConnectionDistance) {
            continue;
        }
        
        // Create synapse
        float weight = 0.1f + rng.uniformReal(0.0f, 0.2f);
        SynapseId synId = createSynapse(brain, conn.first, conn.second, SynapseType::Excitatory, weight);
        
        if (synId != INVALID_SYNAPSE_ID) {
            createdSynapses.push_back(synId);
        }
    }
    
    return createdSynapses;
}

void StructuralPlasticity::batchRemoveSynapses(Brain* brain,
                                              const std::vector<SynapseId>& synapseIds) {
    if (!pImpl->enableBatchOperations || synapseIds.empty()) {
        return;
    }
    
    for (const auto& synId : synapseIds) {
        removeSynapse(brain, synId);
    }
}

bool StructuralPlasticity::validateConfiguration() const {
    // Check configuration validity
    if (pImpl->synaptogenesisRate < 0.0f || pImpl->synaptogenesisRate > 1.0f) {
        return false;
    }
    
    if (pImpl->pruningRate < 0.0f || pImpl->pruningRate > 1.0f) {
        return false;
    }
    
    if (pImpl->targetSynapticDensity < 0.0f || pImpl->targetSynapticDensity > 1.0f) {
        return false;
    }
    
    if (pImpl->homeostaticTargetFiringRate < 0.0f) {
        return false;
    }
    
    return true;
}

void StructuralPlasticity::reorganizeNetwork(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Collect all synapses
    std::vector<SynapseId> allSynapses;
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        for (Synapse* syn : synapses) {
            if (syn) {
                allSynapses.push_back(syn->getId());
            }
        }
    }
    
    if (allSynapses.empty()) return;
    
    // Calculate weight statistics
    std::vector<float> weights;
    for (SynapseId synId : allSynapses) {
        for (auto& region : brain->getRegions()) {
            Synapse* syn = region->getSynapse(synId);
            if (syn) {
                weights.push_back(std::abs(syn->getWeight()));
                break;
            }
        }
    }
    
    if (weights.empty()) return;
    
    // Calculate percentiles
    std::sort(weights.begin(), weights.end());
    float medianWeight = weights[weights.size() / 2];
    float q25Weight = weights[weights.size() / 4];
    float q75Weight = weights[3 * weights.size() / 4];
    
    // Reorganize synapses based on weight distribution
    for (SynapseId synId : allSynapses) {
        for (auto& region : brain->getRegions()) {
            Synapse* syn = region->getSynapse(synId);
            if (syn) {
                float weight = std::abs(syn->getWeight());
                
                // Adjust weight based on reorganization rules
                if (weight < q25Weight) {
                    // Increase weak synapses
                    float newWeight = weight * 2.0f;
                    syn->setWeight(syn->getWeight() > 0 ? newWeight : -newWeight);
                } else if (weight > q75Weight) {
                    // Decrease strong synapses for stability
                    float newWeight = weight * 0.8f;
                    syn->setWeight(syn->getWeight() > 0 ? newWeight : -newWeight);
                }
                
                break;
            }
        }
    }
}

void StructuralPlasticity::reset() {
    pImpl->totalSynapsesCreated = 0;
    pImpl->totalSynapsesPruned = 0;
    pImpl->hebbianSynapsesCreated = 0;
    pImpl->localSynapsesCreated = 0;
    pImpl->developmentallyPruned = 0;
    pImpl->competitivePruned = 0;
    pImpl->unusedPruned = 0;
    
    pImpl->synapseStages.clear();
    pImpl->synapseStability.clear();
    pImpl->synapseMaturationProgress.clear();
    pImpl->synapseWeights.clear();
    
    pImpl->neuronFiringRates.clear();
    pImpl->neuronSynapseCounts.clear();
    pImpl->neuronActivityHistory.clear();
    
    pImpl->structuralEvents.clear();
}

void StructuralPlasticity::recordStructuralEvent(SynapseId synapse, float weightChange, Timestamp timestamp) {
    pImpl->structuralEvents.push_back({synapse, weightChange, timestamp});
    
    // Keep only recent events (last 1000)
    if (pImpl->structuralEvents.size() > 1000) {
        pImpl->structuralEvents.erase(pImpl->structuralEvents.begin());
    }
}

float StructuralPlasticity::getStructuralMemoryScore(NeuronId neuron) const {
    float score = 0.0f;
    
    // Calculate score based on structural events involving this neuron
    for (const auto& event : pImpl->structuralEvents) {
        SynapseId synId = std::get<0>(event);
        // In real implementation, would find which neurons are connected to this synapse
        // For now, use placeholder
        score += std::abs(std::get<1>(event));
    }
    
    return score / static_cast<float>(pImpl->structuralEvents.size() + 1);
}

// ========== HELPER FUNCTIONS ==========

float StructuralPlasticity::calculateFiringRate(const std::vector<Timestamp>& spikes) const {
    if (spikes.empty()) return 0.0f;
    
    // Calculate average firing rate
    float totalTime = 0.0f;
    if (spikes.size() > 1) {
        totalTime = spikes.back() - spikes.front();
    }
    
    if (totalTime <= 0.0f) return static_cast<float>(spikes.size());
    
    return static_cast<float>(spikes.size()) / totalTime;
}

float StructuralPlasticity::calculateCorrelation(const std::vector<Timestamp>& preSpikes,
                                                const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Simplified correlation calculation
    float preMean = calculateFiringRate(preSpikes);
    float postMean = calculateFiringRate(postSpikes);
    
    float covariance = 0.0f;
    float preVar = 0.0f;
    float postVar = 0.0f;
    
    // Simple correlation - in real implementation would be more sophisticated
    covariance = preMean * postMean;
    preVar = preMean * preMean;
    postVar = postMean * postMean;
    
    if (preVar == 0.0f || postVar == 0.0f) return 0.0f;
    
    return covariance / std::sqrt(preVar * postVar);
}

float StructuralPlasticity::calculateMaturationProgress(const std::vector<Timestamp>& preSpikes,
                                                      const std::vector<Timestamp>& postSpikes) const {
    // Calculate maturation progress based on spike timing
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Simplified maturation progress
    float preRate = calculateFiringRate(preSpikes);
    float postRate = calculateFiringRate(postSpikes);
    
    return std::min(1.0f, (preRate + postRate) * 0.1f);
}

float StructuralPlasticity::calculateActivityFactor(const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes) const {
    // Calculate activity factor for synapse stabilization
    float preRate = calculateFiringRate(preSpikes);
    float postRate = calculateFiringRate(postSpikes);
    
    return std::min(1.0f, (preRate + postRate) * 0.05f);
}

std::vector<std::pair<NeuronId, NeuronId>> StructuralPlasticity::generateConnectionPairs(Brain* brain,
                                                                                       size_t count) {
    std::vector<std::pair<NeuronId, NeuronId>> connections;
    
    if (!brain) return connections;
    
    // Collect all neurons
    std::vector<NeuronId> neurons;
    for (auto& region : brain->getRegions()) {
        auto regionNeurons = region->getAllNeurons();
        for (auto* neuron : regionNeurons) {
            neurons.push_back(neuron->getId());
        }
    }
    
    if (neurons.size() < 2) return connections;
    
    // Generate random connection pairs
    connections.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        size_t idx1 = rng.uniformInt(0, static_cast<int>(neurons.size()) - 1);
        size_t idx2 = rng.uniformInt(0, static_cast<int>(neurons.size()) - 1);
        
        if (idx1 != idx2) {
            connections.push_back({neurons[idx1], neurons[idx2]});
        }
    }
    
    return connections;
}

std::vector<SynapseId> StructuralPlasticity::selectSynapsesForPruning(Brain* brain, size_t count) {
    std::vector<SynapseId> synapseIds;
    
    if (!brain) return synapseIds;
    
    // Collect all synapses
    std::vector<std::pair<SynapseId, float>> synapseWeights;
    
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        for (Synapse* syn : synapses) {
            if (syn && syn->getWeight() != 0.0f) {
                synapseWeights.push_back({syn->getId(), std::abs(syn->getWeight())});
            }
        }
    }
    
    if (synapseWeights.empty()) return synapseIds;
    
    // Sort by weight (lower weights first)
    std::sort(synapseWeights.begin(), synapseWeights.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    // Select synapses for pruning
    synapseIds.reserve(std::min(count, synapseWeights.size()));
    for (size_t i = 0; i < std::min(count, synapseWeights.size()); ++i) {
        synapseIds.push_back(synapseWeights[i].first);
    }
    
    return synapseIds;
}

} // namespace nlm
