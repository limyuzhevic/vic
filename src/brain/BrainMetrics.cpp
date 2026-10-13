// BrainMetrics.cpp - Implementation of Brain statistics and metrics

#include "BrainMetrics.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Get total neuron count across all regions
inline size_t getTotalNeuronCount(const Brain::Impl* impl) {
    size_t total = 0;
    for (const auto& region : impl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

// Get total synapse count across all regions and inter-region connections
inline size_t getTotalSynapseCount(const Brain::Impl* impl) {
    size_t total = 0;
    for (const auto& region : impl->regions) {
        total += region->getSynapseCount();
    }
    total += impl->interRegionConnections.size();
    return total;
}

// Get count of active neurons across all regions
inline size_t getActiveNeuronCount(const Brain::Impl* impl) {
    size_t total = 0;
    for (const auto& region : impl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

// Get count of currently firing neurons (from spike statistics)
inline size_t getFiringNeuronCount(const Brain::Impl* impl) {
    return impl->totalSpikesThisStep;
}

// Get average firing rate across all neurons
inline float getAverageFiringRate(const Brain::Impl* impl) {
    if (impl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : impl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(impl->regions.size());
}

// Get excitation/inhibition balance ratio
inline float getExcitationInhibitionRatio(const Brain::Impl* impl) {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : impl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

// Get total spike count since brain creation
inline size_t getTotalSpikeCount(const Brain::Impl* impl) {
    return impl->totalSpikesTotal;
}

// Get count of pending spike events
inline size_t getPendingSpikeEventCount(const Brain::Impl* impl) {
    return impl->spikeSystem->getPendingSpikeCount() + impl->spikeSystem->getPendingDelayedCount();
}

// Get current developmental stage
inline DevelopmentalStage getDevelopmentalStage(const Brain::Impl* impl) {
    return impl->developmentalStage;
}

// Additional utility functions for brain metrics
namespace BrainMetrics {
    
    // Get count of neurons by type
    size_t getNeuronCountByType(const Brain::Impl* impl, NeuronType type) {
        size_t total = 0;
        for (const auto& region : impl->regions) {
            for (const auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    total += pop->getNeuronCount();
                }
            }
        }
        return total;
    }
    
    // Get average synaptic weight
    float getAverageSynapticWeight(const Brain::Impl* impl) {
        size_t totalWeight = 0;
        size_t synapseCount = 0;
        
        for (const auto& region : impl->regions) {
            for (const auto& syn : region->getSynapses()) {
                totalWeight += static_cast<size_t>(std::abs(syn->getWeight()));
                synapseCount++;
            }
        }
        
        return synapseCount > 0 ? static_cast<float>(totalWeight) / synapseCount : 0.0f;
    }
    
    // Get spike rate (spikes per second)
    float getSpikeRate(const Brain::Impl* impl, size_t steps) {
        if (steps == 0) return 0.0f;
        float timestep = 0.001f;  // 1ms timestep
        return static_cast<float>(impl->totalSpikesTotal) / (steps * timestep);
    }
    
    // Check if brain is in resting state
    bool isResting(const Brain::Impl* impl) {
        return impl->isResting;
    }
    
    // Get steps since last episode storage
    size_t getStepsSinceLastEpisode(const Brain::Impl* impl) {
        return impl->stepsSinceLastEpisode;
    }
}

} // namespace nlm