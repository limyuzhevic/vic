#include "CompetitionForResources.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace nlm {

struct CompetitionForResources::Impl {
    // Competition parameters
    float competitionIntensity;    // Competition intensity (α)
    size_t resourcePoolSize;       // Maximum available resources
    float pruningThreshold;        // Threshold for resource-based pruning
    int resourceAllocationStrategy; // Resource allocation strategy
    
    // Resource statistics
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
    size_t currentResourceUsage;   // Current synapses in use
    
    Impl() : competitionIntensity(0.5f), resourcePoolSize(10000), pruningThreshold(0.1f),
             resourceAllocationStrategy(1), // 1: random, 2: weight-based, 3: activity-based
             totalSynapsesCreated(0), totalSynapsesPruned(0), currentResourceUsage(0) {}
};

CompetitionForResources::CompetitionForResources() : pImpl(new Impl) {}

CompetitionForResources::~CompetitionForResources() = default;

SynapseId CompetitionForResources::createSynapse(Brain* brain, NeuronId source, 
                                                NeuronId destination, SynapticWeight weight) {
    if (!brain) return INVALID_SYNAPSE_ID;
    
    // Check if there are enough resources
    size_t additionalSynapses = 1;  // We're trying to create one synapse
    if (!hasEnoughResources(brain, additionalSynapses)) {
        return INVALID_SYNAPSE_ID;  // Not enough resources
    }
    
    // Check if connection already exists
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;  // Connection already exists
            }
        }
        
        // Create the synapse
        SynapseId synId = region->addSynapse(source, destination, weight, 1);
        
        if (synId != INVALID_SYNAPSE_ID) {
            ++pImpl->totalSynapsesCreated;
            ++pImpl->currentResourceUsage;
            return synId;
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

bool CompetitionForResources::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) return false;
    
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            syn->setWeight(0.0f);
            ++pImpl->totalSynapsesPruned;
            --pImpl->currentResourceUsage;
            return true;
        }
    }
    
    return false;
}

bool CompetitionForResources::removeNeuron(Brain* brain, NeuronId neuron) {
    if (!brain) return false;
    
    size_t prunedCount = 0;
    
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(neuron);
        auto synapsesFrom = region->getSynapsesFrom(neuron);
        
        for (Synapse* syn : synapsesTo) {
            syn->setWeight(0.0f);
            ++prunedCount;
        }
        
        for (Synapse* syn : synapsesFrom) {
            syn->setWeight(0.0f);
            ++prunedCount;
        }
    }
    
    pImpl->totalSynapsesPruned += prunedCount;
    pImpl->currentResourceUsage -= prunedCount;
    
    return true;
}

void CompetitionForResources::setCompetitionIntensity(float intensity) {
    pImpl->competitionIntensity = std::clamp(intensity, 0.0f, 2.0f);
}

float CompetitionForResources::getCompetitionIntensity() const {
    return pImpl->competitionIntensity;
}

void CompetitionForResources::setResourcePoolSize(size_t poolSize) {
    pImpl->resourcePoolSize = std::max<size_t>(100, poolSize);
}

size_t CompetitionForResources::getResourcePoolSize() const {
    return pImpl->resourcePoolSize;
}

void CompetitionForResources::setPruningThreshold(float threshold) {
    pImpl->pruningThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float CompetitionForResources::getPruningThreshold() const {
    return pImpl->pruningThreshold;
}

void CompetitionForResources::setResourceAllocationStrategy(int strategy) {
    pImpl->resourceAllocationStrategy = std::clamp(strategy, 1, 3);
}

int CompetitionForResources::getResourceAllocationStrategy() const {
    return pImpl->resourceAllocationStrategy;
}

void CompetitionForResources::updateResourceAllocation(Brain* brain) {
    if (!brain) return;
    
    // Check if we need to prune synapses due to resource constraints
    if (pImpl->currentResourceUsage > pImpl->resourcePoolSize * (1.0f - pImpl->pruningThreshold)) {
        size_t excessSynapses = pImpl->currentResourceUsage - 
                               static_cast<size_t>(pImpl->resourcePoolSize * (1.0f - pImpl->pruningThreshold));
        
        if (excessSynapses > 0) {
            size_t prunedCount = selectSynapsesForPruning(brain, excessSynapses);
            // Note: selectSynapsesForPruning only selects which synapses to prune,
            // actual pruning is done by removeSynapse
        }
    }
}

float CompetitionForResources::calculateResourceDemand(Brain* brain) const {
    if (!brain) return 0.0f;
    
    float totalDemand = 0.0f;
    
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        
        for (Synapse* syn : synapses) {
            // Different synapses may have different resource demands
            // Simplified: each synapse has a base demand
            float demand = 1.0f;
            
            // Modify demand based on weight
            demand *= std::abs(syn->getWeight()) + 0.1f;  // Avoid zero weight
            
            // Modify demand based on activity (simplified)
            float eligibility = std::abs(syn->getEligibilityTrace());
            demand *= (1.0f + eligibility);
            
            totalDemand += demand;
        }
    }
    
    return totalDemand;
}

bool CompetitionForResources::hasEnoughResources(Brain* brain, size_t additionalSynapses) const {
    if (!brain) return false;
    
    size_t totalSynapses = 0;
    
    for (auto& region : brain->getRegions()) {
        totalSynapses += region->getSynapseCount();
    }
    
    size_t requiredResources = totalSynapses + additionalSynapses;
    return requiredResources <= pImpl->resourcePoolSize;
}

size_t CompetitionForResources::selectSynapsesForPruning(Brain* brain, size_t count) const {
    if (!brain || count == 0) return 0;
    
    // Collect all synapses with their pruning scores
    struct SynapseInfo {
        SynapseId id;
        float pruneScore;
        float weight;
        float activity;
    };
    
    std::vector<SynapseInfo> synapseInfos;
    
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        
        for (Synapse* syn : synapses) {
            if (syn->getWeight() == 0.0f) continue;  // Already pruned
            
            SynapseInfo info;
            info.id = syn->getId();
            info.weight = std::abs(syn->getWeight());
            info.activity = std::abs(syn->getEligibilityTrace());
            
            // Calculate prune score based on strategy
            switch (pImpl->resourceAllocationStrategy) {
                case 1: { // Random
                    info.pruneScore = static_cast<float>(rand()) / RAND_MAX;
                    break;
                }
                case 2: { // Weight-based
                    info.pruneScore = 1.0f - info.weight;  // Lower weight = higher prune score
                    break;
                }
                case 3: { // Activity-based
                    info.pruneScore = 1.0f - info.activity;  // Lower activity = higher prune score
                    break;
                }
                default: { // Default: random
                    info.pruneScore = static_cast<float>(rand()) / RAND_MAX;
                    break;
                }
            }
            
            synapseInfos.push_back(info);
        }
    }
    
    // Sort by prune score (descending)
    std::sort(synapseInfos.begin(), synapseInfos.end(), 
              [](const SynapseInfo& a, const SynapseInfo& b) {
                  return a.pruneScore > b.pruneScore;
              });
    
    // Prune the top synapses
    size_t prunedCount = 0;
    for (size_t i = 0; i < std::min(count, synapseInfos.size()); ++i) {
        removeSynapse(brain, synapseInfos[i].id);
        ++prunedCount;
    }
    
    return prunedCount;
}

} // namespace nlm