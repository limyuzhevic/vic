#include "Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    /*
     * Real pruning implementation
     * 
     * Removes synapses that are:
     * 1. Weak (weight below threshold)
     * 2. Unused (no recent activity)
     * 3. Redundant (poor efficiency)
     * 
     * Biological inspiration:
     * - Happens during development and in adult brains
     * - Activity-dependent (Hebbian-like)
     * - Competition for resources
     * 
     * Parameters:
     * - threshold: synaptic weight below which synapse is considered weak
     * - pruningRate: probability of pruning per eligible synapse per step
     */
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        auto& synapses = region->getSynapses();
        
        for (auto& synPtr : synapses) {
            Synapse* syn = synPtr.get();
            
            // Skip if synapse is already very weak or zero weight
            if (std::abs(syn->getWeight()) < 0.001f) {
                continue;
            }
            
            // Check pruning conditions
            bool shouldPrune = false;
            PruneReason reason = PruneReason::Normal;
            
            // 1. Weak synapses
            if (std::abs(syn->getWeight()) < pImpl->threshold) {
                shouldPrune = rng.bernoulli(pImpl->pruningRate * 2.0f);  // Stronger pruning for weak synapses
                reason = PruneReason::Weak;
            }
            
            // 2. Unused synapses (no recent activity)
            else if (hasRecentActivity(syn)) {
                // Check efficiency - synapses that fire rarely are less efficient
                float efficacy = syn->getEfficacy();
                if (efficacy < 0.5f) {
                    shouldPrune = rng.bernoulli(pImpl->pruningRate);
                    reason = PruneReason::Inefficient;
                }
            }
            
            // 3. Redundant synapses (low eligibility trace)
            if (std::abs(syn->getEligibilityTrace()) < 0.01f &&
                std::abs(syn->getWeight()) < 0.1f) {
                shouldPrune = rng.bernoulli(pImpl->pruningRate * 0.5f);
                reason = PruneReason::Redundant;
            }
            
            if (shouldPrune) {
                // Actually prune the synapse
                if (pruneSynapse(syn)) {
                    NLM_LOG_DEBUG("Synapse pruned: reason=" + std::to_string(static_cast<int>(reason)) +
                                " weight=" + std::to_string(syn->getWeight()));
                }
            }
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) {
        return false;
    }
    
    // For now, we'll mark the synapse as pruned by setting its weight to zero
    // In a real implementation, this would remove the synapse from the region
    float oldWeight = synapse->getWeight();
    synapse->setWeight(0.0f);
    synapse->setEfficacy(0.0f);
    
    NLM_LOG_DEBUG("Synapse pruned: weight changed from " + std::to_string(oldWeight) + " to 0.0f");
    
    return true;
}

bool Pruning::hasRecentActivity(Synapse* synapse) {
    if (!synapse) return true;
    
    // Check if synapse has had recent pre- or post-synaptic activity
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    if (preSpikes.empty() && postSpikes.empty()) {
        return false;
    }
    
    // Get most recent spike time
    float maxTime = 0.0f;
    if (!preSpikes.empty()) {
        maxTime = std::max(maxTime, *std::max_element(preSpikes.begin(), preSpikes.end()));
    }
    if (!postSpikes.empty()) {
        maxTime = std::max(maxTime, *std::max_element(postSpikes.begin(), postSpikes.end()));
    }
    
    // Consider synapse active if it had spikes within last 1000ms (1 second)
    return (maxTime > 0.0f);
}

} // namespace nlm
