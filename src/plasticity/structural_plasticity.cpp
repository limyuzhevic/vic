#include "structural_plasticity.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../core/RandomGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <map>

namespace nlm
{

// StructuralPlasticity implementation
struct StructuralPlasticity::Impl {
    float synaptogenesisRate;
    float pruningRate;
    float minWeightThreshold;
    float activityThreshold;
    size_t maxSynapsesPerNeuron;
    size_t maxTotalSynapses;
    bool activityDependentFormation;
    bool activityDependentPruning;
    bool homeostaticStructuralPlasticity;
    
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             minWeightThreshold(0.05f), activityThreshold(0.001f),
             maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
             activityDependentFormation(true), activityDependentPruning(true),
             homeostaticStructuralPlasticity(true),
             totalSynapsesCreated(0), totalSynapsesPruned(0) {}
};

StructuralPlasticity::StructuralPlasticity() : pImpl(std::make_unique<Impl>()) {}

StructuralPlasticity::~StructuralPlasticity() = default;

void StructuralPlasticity::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    // Apply structural plasticity rules to this synapse
    float weight = std::abs(synapse->getWeight());
    float eligibility = std::abs(synapse->getEligibilityTrace());
    
    // Activity-dependent formation and pruning
    if (pImpl->activityDependentFormation) {
        // Check if synapse should be strengthened/stabilized
        if (weight < pImpl->minWeightThreshold && eligibility > pImpl->activityThreshold) {
            // Strengthen weak but eligible synapses
            applyWeightChange(synapse, 0.01f);
        }
    }
    
    if (pImpl->activityDependentPruning) {
        // Check if synapse should be pruned
        if (weight < pImpl->minWeightThreshold) {
            // Weak synapses are more likely to be pruned
            float pruneProbability = pImpl->pruningRate * (1.0f - weight / pImpl->minWeightThreshold);
            
            // In a real implementation, this would use a random number generator
            // For now, we'll apply a simple heuristic
            if (pruneProbability > 0.5f) {
                // Mark synapse for pruning by setting weight to 0
                synapse->setWeight(0.0f);
                pImpl->totalSynapsesPruned++;
            }
        }
    }
    
    if (pImpl->homeostaticStructuralPlasticity) {
        // Apply homeostatic structural plasticity
        float homeostaticEffect = 1.0f - (weight / pImpl->minWeightThreshold);
        if (homeostaticEffect > 0.0f) {
            // Apply structural strengthening based on homeostatic need
            applyWeightChange(synapse, homeostaticEffect * 0.001f);
        }
    }
}

void StructuralPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* StructuralPlasticity::getName() const {
    return "StructuralPlasticity";
}

void StructuralPlasticity::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.1f);
}

void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

void StructuralPlasticity::setMinWeightThreshold(float threshold) {
    pImpl->minWeightThreshold = std::max(0.0f, threshold);
}

void StructuralPlasticity::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::max(0.0f, threshold);
}

void StructuralPlasticity::setMaxSynapsesPerNeuron(size_t max) {
    pImpl->maxSynapsesPerNeuron = max;
}

void StructuralPlasticity::setMaxTotalSynapses(size_t max) {
    pImpl->maxTotalSynapses = max;
}

void StructuralPlasticity::enableActivityDependentFormation(bool enable) {
    pImpl->activityDependentFormation = enable;
}

void StructuralPlasticity::enableActivityDependentPruning(bool enable) {
    pImpl->activityDependentPruning = enable;
}

void StructuralPlasticity::enableHomeostaticStructuralPlasticity(bool enable) {
    pImpl->homeostaticStructuralPlasticity = enable;
}

float StructuralPlasticity::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

float StructuralPlasticity::getMinWeightThreshold() const {
    return pImpl->minWeightThreshold;
}

float StructuralPlasticity::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

size_t StructuralPlasticity::getMaxSynapsesPerNeuron() const {
    return pImpl->maxSynapsesPerNeuron;
}

size_t StructuralPlasticity::getMaxTotalSynapses() const {
    return pImpl->maxTotalSynapses;
}

bool StructuralPlasticity::isActivityDependentFormationEnabled() const {
    return pImpl->activityDependentFormation;
}

bool StructuralPlasticity::isActivityDependentPruningEnabled() const {
    return pImpl->activityDependentPruning;
}

bool StructuralPlasticity::isHomeostaticStructuralPlasticityEnabled() const {
    return pImpl->homeostaticStructuralPlasticity;
}

size_t StructuralPlasticity::getTotalSynapsesCreated() const {
    return pImpl->totalSynapsesCreated;
}

size_t StructuralPlasticity::getTotalSynapsesPruned() const {
    return pImpl->totalSynapsesPruned;
}

float StructuralPlasticity::getNetStructuralChange() const {
    return static_cast<float>(pImpl->totalSynapsesCreated - pImpl->totalSynapsesPruned);
}

SynapseId StructuralPlasticity::createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                                             SynapticWeight weight) {
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        // Check if connection already exists
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID; // Connection already exists
            }
        }
        
        // Check max synapses limit
        if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
            return INVALID_SYNAPSE_ID;
        }
        
        // Count outgoing synapses from source in this region
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue; // Try next region
        }
        
        // Create the synapse
        SynapseId synId = region->addSynapse(source, destination, weight, 1);
        if (synId != INVALID_SYNAPSE_ID) {
            pImpl->totalSynapsesCreated++;
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
            pImpl->totalSynapsesPruned++;
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

float StructuralPlasticity::calculateFormationProbability(float preActivity, float postActivity) const {
    // Calculate probability of synapse formation based on activity
    float activitySum = preActivity + postActivity;
    float formationProb = pImpl->synaptogenesisRate * activitySum;
    return std::min(formationProb, 1.0f);
}

float StructuralPlasticity::calculatePruningProbability(float weight, float activity) const {
    // Calculate probability of synapse pruning based on weight and activity
    float weightFactor = (pImpl->minWeightThreshold - weight) / pImpl->minWeightThreshold;
    float activityFactor = 1.0f - activity;
    float pruneProb = pImpl->pruningRate * weightFactor * activityFactor;
    return std::max(0.0f, pruneProb);
}

void StructuralPlasticity::updateStructuralNetwork(Brain* brain, RandomGenerator& rng) {
    // Update structural plasticity across the entire network
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
            } else if (pImpl->activityDependentPruning) {
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

void StructuralPlasticity::applyHomeostaticStructuralConstraints(Brain* brain) {
    // Apply homeostatic constraints to structural plasticity
    if (!brain) return;
    
    // This would implement homeostatic structural constraints
    // For example, maintaining optimal connectivity density
}

// AxonalGrowthPlasticity implementation
struct AxonalGrowthPlasticity::Impl {
    float axonalGrowthRate;
    float axonalRetractionRate;
    float growthActivityThreshold;
    float retractionActivityThreshold;
    bool branchingEnabled;
    bool sproutingEnabled;
    
    Impl() : axonalGrowthRate(0.001f), axonalRetractionRate(0.0001f),
             growthActivityThreshold(0.1f), retractionActivityThreshold(0.01f),
             branchingEnabled(true), sproutingEnabled(true) {}
};

AxonalGrowthPlasticity::AxonalGrowthPlasticity() : pImpl(std::make_unique<Impl>()) {}

AxonalGrowthPlasticity::~AxonalGrowthPlasticity() = default;

void AxonalGrowthPlasticity::update(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate activity for growth decisions
    float preActivity = preSpikes.empty() ? 0.0f : 
                        static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
    float postActivity = postSpikes.empty() ? 0.0f : 
                         static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    
    // Calculate axonal growth
    float growthDelta = calculateAxonalGrowth(preActivity, postActivity);
    
    // Apply growth to synapse (affects weight and other properties)
    applyWeightChange(synapse, growthDelta);
}

void AxonalGrowthPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* AxonalGrowthPlasticity::getName() const {
    return "AxonalGrowthPlasticity";
}

void AxonalGrowthPlasticity::setAxonalGrowthRate(float rate) {
    pImpl->axonalGrowthRate = std::clamp(rate, 0.0f, 0.01f);
}

void AxonalGrowthPlasticity::setAxonalRetractionRate(float rate) {
    pImpl->axonalRetractionRate = std::clamp(rate, 0.0f, 0.001f);
}

void AxonalGrowthPlasticity::setGrowthActivityThreshold(float threshold) {
    pImpl->growthActivityThreshold = std::max(0.0f, threshold);
}

void AxonalGrowthPlasticity::setRetractionActivityThreshold(float threshold) {
    pImpl->retractionActivityThreshold = std::max(0.0f, threshold);
}

void AxonalGrowthPlasticity::enableBranching(bool enable) {
    pImpl->branchingEnabled = enable;
}

void AxonalGrowthPlasticity::enableSprouting(bool enable) {
    pImpl->sproutingEnabled = enable;
}

float AxonalGrowthPlasticity::getAxonalGrowthRate() const {
    return pImpl->axonalGrowthRate;
}

float AxonalGrowthPlasticity::getAxonalRetractionRate() const {
    return pImpl->axonalRetractionRate;
}

float AxonalGrowthPlasticity::getGrowthActivityThreshold() const {
    return pImpl->growthActivityThreshold;
}

float AxonalGrowthPlasticity::getRetractionActivityThreshold() const {
    return pImpl->retractionActivityThreshold;
}

bool AxonalGrowthPlasticity::isBranchingEnabled() const {
    return pImpl->branchingEnabled;
}

bool AxonalGrowthPlasticity::isSproutingEnabled() const {
    return pImpl->sproutingEnabled;
}

float AxonalGrowthPlasticity::calculateAxonalGrowth(float preActivity, float postActivity) const {
    float growth = 0.0f;
    
    // Growth based on activity
    if (preActivity > pImpl->growthActivityThreshold || 
        postActivity > pImpl->growthActivityThreshold) {
        growth = pImpl->axonalGrowthRate * (preActivity + postActivity);
    }
    
    // Branching and sprouting effects
    if (pImpl->branchingEnabled && preActivity > pImpl->growthActivityThreshold * 2.0f) {
        growth *= 1.5f; // Enhanced growth from branching
    }
    
    if (pImpl->sproutingEnabled && postActivity > pImpl->growthActivityThreshold * 2.0f) {
        growth *= 1.3f; // Enhanced growth from sprouting
    }
    
    return growth;
}

float AxonalGrowthPlasticity::calculateAxonalRetraction(float preActivity, float postActivity) const {
    float retraction = 0.0f;
    
    // Retraction based on low activity
    if (preActivity < pImpl->retractionActivityThreshold && 
        postActivity < pImpl->retractionActivityThreshold) {
        retraction = pImpl->axonalRetractionRate;
    }
    
    return retraction;
}

// DendriticGrowthPlasticity implementation
struct DendriticGrowthPlasticity::Impl {
    float dendriticGrowthRate;
    float dendriticRetractionRate;
    float dendriticSpineFormationRate;
    float spineEliminationRate;
    bool spineFormationEnabled;
    bool spineEliminationEnabled;
    
    Impl() : dendriticGrowthRate(0.002f), dendriticRetractionRate(0.0002f),
             dendriticSpineFormationRate(0.0005f), spineEliminationRate(0.00005f),
             spineFormationEnabled(true), spineEliminationEnabled(true) {}
};

DendriticGrowthPlasticity::DendriticGrowthPlasticity() : pImpl(std::make_unique<Impl>()) {}

DendriticGrowthPlasticity::~DendriticGrowthPlasticity() = default;

void DendriticGrowthPlasticity::update(Synapse* synapse,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate activity for growth decisions
    float activity = postSpikes.empty() ? 0.0f : 
                     static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    
    // Calculate dendritic growth and spine formation
    float growthDelta = calculateDendriticGrowth(0.0f, activity);
    float spineDelta = calculateSpineFormation(activity);
    float retractionDelta = calculateDendriticRetraction(0.0f, activity);
    float eliminationDelta = calculateSpineElimination(activity);
    
    // Apply combined effects
    float totalDelta = growthDelta + spineDelta + retractionDelta + eliminationDelta;
    applyWeightChange(synapse, totalDelta);
}

void DendriticGrowthPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* DendriticGrowthPlasticity::getName() const {
    return "DendriticGrowthPlasticity";
}

void DendriticGrowthPlasticity::setDendriticGrowthRate(float rate) {
    pImpl->dendriticGrowthRate = std::clamp(rate, 0.0f, 0.02f);
}

void DendriticGrowthPlasticity::setDendriticRetractionRate(float rate) {
    pImpl->dendriticRetractionRate = std::clamp(rate, 0.0f, 0.002f);
}

void DendriticGrowthPlasticity::setDendriticSpineFormationRate(float rate) {
    pImpl->dendriticSpineFormationRate = std::clamp(rate, 0.0f, 0.005f);
}

void DendriticGrowthPlasticity::setSpineEliminationRate(float rate) {
    pImpl->spineEliminationRate = std::clamp(rate, 0.0f, 0.0005f);
}

void DendriticGrowthPlasticity::enableSpineFormation(bool enable) {
    pImpl->spineFormationEnabled = enable;
}

void DendriticGrowthPlasticity::enableSpineElimination(bool enable) {
    pImpl->spineEliminationEnabled = enable;
}

float DendriticGrowthPlasticity::getDendriticGrowthRate() const {
    return pImpl->dendriticGrowthRate;
}

float DendriticGrowthPlasticity::getDendriticRetractionRate() const {
    return pImpl->dendriticRetractionRate;
}

float DendriticGrowthPlasticity::getDendriticSpineFormationRate() const {
    return pImpl->dendriticSpineFormationRate;
}

float DendriticGrowthPlasticity::getSpineEliminationRate() const {
    return pImpl->spineEliminationRate;
}

bool DendriticGrowthPlasticity::isSpineFormationEnabled() const {
    return pImpl->spineFormationEnabled;
}

bool DendriticGrowthPlasticity::isSpineEliminationEnabled() const {
    return pImpl->spineEliminationEnabled;
}

float DendriticGrowthPlasticity::calculateDendriticGrowth(float preActivity, float postActivity) const {
    float growth = 0.0f;
    
    if (postActivity > 0.1f) {
        growth = pImpl->dendriticGrowthRate * postActivity;
    }
    
    return growth;
}

float DendriticGrowthPlasticity::calculateDendriticRetraction(float preActivity, float postActivity) const {
    float retraction = 0.0f;
    
    if (postActivity < 0.01f) {
        retraction = pImpl->dendriticRetractionRate;
    }
    
    return retraction;
}

float DendriticGrowthPlasticity::calculateSpineFormation(float activity) const {
    float spineFormation = 0.0f;
    
    if (pImpl->spineFormationEnabled && activity > 0.05f) {
        spineFormation = pImpl->dendriticSpineFormationRate * activity;
    }
    
    return spineFormation;
}

float DendriticGrowthPlasticity::calculateSpineElimination(float activity) const {
    float spineElimination = 0.0f;
    
    if (pImpl->spineEliminationEnabled && activity < 0.005f) {
        spineElimination = pImpl->spineEliminationRate * (1.0f - activity);
    }
    
    return spineElimination;
}

// HomeostaticStructuralPlasticity implementation
struct HomeostaticStructuralPlasticity::Impl {
    float homeostaticTarget;
    float homeostaticDeviation;
    float structuralPlasticityRate;
    float synapticTargetSize;
    bool compensatoryGrowth;
    bool compensatoryPruning;
    
    Impl() : homeostaticTarget(0.1f), homeostaticDeviation(0.05f),
             structuralPlasticityRate(0.001f), synapticTargetSize(0.1f),
             compensatoryGrowth(true), compensatoryPruning(true) {}
};

HomeostaticStructuralPlasticity::HomeostaticStructuralPlasticity() : pImpl(std::make_unique<Impl>()) {}

HomeostaticStructuralPlasticity::~HomeostaticStructuralPlasticity() = default;

void HomeostaticStructuralPlasticity::update(Synapse* synapse,
                                             const std::vector<Timestamp>& preSpikes,
                                             const std::vector<Timestamp>& postSpikes,
                                             TimestepDuration dt) {
    if (!synapse) return;
    
    float currentSize = std::abs(synapse->getWeight());
    
    // Calculate homeostatic effects
    float compensatoryGrowthEffect = calculateCompensatoryGrowth(currentSize, pImpl->synapticTargetSize);
    float compensatoryPruningEffect = calculateCompensatoryPruning(currentSize, pImpl->synapticTargetSize);
    
    // Apply homeostatic structural plasticity
    float totalDelta = compensatoryGrowthEffect + compensatoryPruningEffect;
    applyWeightChange(synapse, totalDelta);
}

void HomeostaticStructuralPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* HomeostaticStructuralPlasticity::getName() const {
    return "HomeostaticStructuralPlasticity";
}

void HomeostaticStructuralPlasticity::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::max(0.0f, target);
}

void HomeostaticStructuralPlasticity::setHomeostaticDeviation(float deviation) {
    pImpl->homeostaticDeviation = std::max(0.0f, deviation);
}

void HomeostaticStructuralPlasticity::setStructuralPlasticityRate(float rate) {
    pImpl->structuralPlasticityRate = std::clamp(rate, 0.0f, 0.01f);
}

void HomeostaticStructuralPlasticity::setSynapticTargetSize(float size) {
    pImpl->synapticTargetSize = std::max(0.0f, size);
}

void HomeostaticStructuralPlasticity::setCompensatoryGrowth(bool enable) {
    pImpl->compensatoryGrowth = enable;
}

void HomeostaticStructuralPlasticity::setCompensatoryPruning(bool enable) {
    pImpl->compensatoryPruning = enable;
}

float HomeostaticStructuralPlasticity::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

float HomeostaticStructuralPlasticity::getHomeostaticDeviation() const {
    return pImpl->homeostaticDeviation;
}

float HomeostaticStructuralPlasticity::getStructuralPlasticityRate() const {
    return pImpl->structuralPlasticityRate;
}

float HomeostaticStructuralPlasticity::getSynapticTargetSize() const {
    return pImpl->synapticTargetSize;
}

bool HomeostaticStructuralPlasticity::isCompensatoryGrowthEnabled() const {
    return pImpl->compensatoryGrowth;
}

bool HomeostaticStructuralPlasticity::isCompensatoryPruningEnabled() const {
    return pImpl->compensatoryPruning;
}

float HomeostaticStructuralPlasticity::calculateCompensatoryGrowth(float currentSize, float targetSize) const {
    float growth = 0.0f;
    
    if (pImpl->compensatoryGrowth) {
        float sizeError = targetSize - currentSize;
        if (sizeError > pImpl->homeostaticDeviation) {
            growth = pImpl->structuralPlasticityRate * sizeError;
        }
    }
    
    return growth;
}

float HomeostaticStructuralPlasticity::calculateCompensatoryPruning(float currentSize, float targetSize) const {
    float pruning = 0.0f;
    
    if (pImpl->compensatoryPruning) {
        float sizeError = currentSize - targetSize;
        if (sizeError > pImpl->homeostaticDeviation) {
            pruning = -pImpl->structuralPlasticityRate * sizeError;
        }
    }
    
    return pruning;
}

void HomeostaticStructuralPlasticity::applyHomeostaticStructuralConstraints(Brain* brain) {
    // Apply homeostatic constraints to structural plasticity
    if (!brain) return;
    
    // This would implement homeostatic structural constraints
    // For example, maintaining optimal connectivity density
}

// ActivityDependentStructuralPlasticity implementation
struct ActivityDependentStructuralPlasticity::Impl {
    float activityDependentGrowthRate;
    float activityDependentPruningRate;
    float activityThreshold;
    float coactivityThreshold;
    bool hebbianStructuralLearning;
    bool antiHebbianStructuralLearning;
    
    Impl() : activityDependentGrowthRate(0.002f), activityDependentPruningRate(0.0002f),
             activityThreshold(0.1f), coactivityThreshold(0.5f),
             hebbianStructuralLearning(true), antiHebbianStructuralLearning(true) {}
};

ActivityDependentStructuralPlasticity::ActivityDependentStructuralPlasticity() : pImpl(std::make_unique<Impl>()) {}

ActivityDependentStructuralPlasticity::~ActivityDependentStructuralPlasticity() = default;

void ActivityDependentStructuralPlasticity::update(Synapse* synapse,
                                                   const std::vector<Timestamp>& preSpikes,
                                                   const std::vector<Timestamp>& postSpikes,
                                                   TimestepDuration dt) {
    if (!synapse) return;
    
    float preActivity = preSpikes.empty() ? 0.0f : 
                        static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
    float postActivity = postSpikes.empty() ? 0.0f : 
                         static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    float coactivity = preActivity * postActivity;
    
    // Calculate activity-dependent growth and pruning
    float growthDelta = calculateActivityDependentGrowth(preActivity, postActivity);
    float pruningDelta = calculateActivityDependentPruning(preActivity, postActivity);
    
    // Calculate Hebbian/anti-Hebbian structural changes
    float hebbianDelta = pImpl->hebbianStructuralLearning ? 
                         calculateHebbianStructuralChange(preActivity, postActivity) : 0.0f;
    float antiHebbianDelta = pImpl->antiHebbianStructuralLearning ? 
                             calculateAntiHebbianStructuralChange(preActivity, postActivity) : 0.0f;
    
    // Apply combined effects
    float totalDelta = growthDelta + pruningDelta + hebbianDelta + antiHebbianDelta;
    applyWeightChange(synapse, totalDelta);
}

void ActivityDependentStructuralPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* ActivityDependentStructuralPlasticity::getName() const {
    return "ActivityDependentStructuralPlasticity";
}

void ActivityDependentStructuralPlasticity::setActivityDependentGrowthRate(float rate) {
    pImpl->activityDependentGrowthRate = std::clamp(rate, 0.0f, 0.02f);
}

void ActivityDependentStructuralPlasticity::setActivityDependentPruningRate(float rate) {
    pImpl->activityDependentPruningRate = std::clamp(rate, 0.0f, 0.002f);
}

void ActivityDependentStructuralPlasticity::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::max(0.0f, threshold);
}

void ActivityDependentStructuralPlasticity::setCoactivityThreshold(float threshold) {
    pImpl->coactivityThreshold = std::max(0.0f, threshold);
}

void ActivityDependentStructuralPlasticity::enableHebbianStructuralLearning(bool enable) {
    pImpl->hebbianStructuralLearning = enable;
}

void ActivityDependentStructuralPlasticity::enableAntiHebbianStructuralLearning(bool enable) {
    pImpl->antiHebbianStructuralLearning = enable;
}

float ActivityDependentStructuralPlasticity::getActivityDependentGrowthRate() const {
    return pImpl->activityDependentGrowthRate;
}

float ActivityDependentStructuralPlasticity::getActivityDependentPruningRate() const {
    return pImpl->activityDependentPruningRate;
}

float ActivityDependentStructuralPlasticity::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

float ActivityDependentStructuralPlasticity::getCoactivityThreshold() const {
    return pImpl->coactivityThreshold;
}

bool ActivityDependentStructuralPlasticity::isHebbianStructuralLearningEnabled() const {
    return pImpl->hebbianStructuralLearning;
}

bool ActivityDependentStructuralPlasticity::isAntiHebbianStructuralLearningEnabled() const {
    return pImpl->antiHebbianStructuralLearning;
}

float ActivityDependentStructuralPlasticity::calculateActivityDependentGrowth(float preActivity, float postActivity) const {
    float growth = 0.0f;
    
    if (preActivity > pImpl->activityThreshold || postActivity > pImpl->activityThreshold) {
        growth = pImpl->activityDependentGrowthRate * (preActivity + postActivity);
    }
    
    return growth;
}

float ActivityDependentStructuralPlasticity::calculateActivityDependentPruning(float preActivity, float postActivity) const {
    float pruning = 0.0f;
    
    float activity = std::min(preActivity, postActivity);
    if (activity < pImpl->activityThreshold * 0.5f) {
        pruning = pImpl->activityDependentPruningRate * (1.0f - activity / pImpl->activityThreshold);
    }
    
    return pruning;
}

float ActivityDependentStructuralPlasticity::calculateHebbianStructuralChange(float preActivity, float postActivity) const {
    float hebbianChange = 0.0f;
    
    float coactivity = preActivity * postActivity;
    if (coactivity > pImpl->coactivityThreshold) {
        hebbianChange = 0.01f * coactivity;
    }
    
    return hebbianChange;
}

float ActivityDependentStructuralPlasticity::calculateAntiHebbianStructuralChange(float preActivity, float postActivity) const {
    float antiHebbianChange = 0.0f;
    
    float coactivity = preActivity * postActivity;
    if (coactivity > pImpl->coactivityThreshold * 2.0f) {
        antiHebbianChange = -0.005f * coactivity;
    }
    
    return antiHebbianChange;
}

} // namespace nlm

