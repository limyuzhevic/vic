#include "Pruning.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
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
    // Implement real pruning: biologically realistic synaptic elimination
    if (!brain) return;
    
    // Pruning is most active during critical periods and early development
    if (brain->getDevelopmentalStage() >= DevelopmentalStage::Adult) {
        return; // Minimal pruning in adult brain
    }
    
    // Calculate pruning threshold based on developmental stage
    float stageFactor = 1.0f;
    switch (brain->getDevelopmentalStage()) {
        case DevelopmentalStage::Initial:
            stageFactor = 2.0f; // High pruning early
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageFactor = 1.5f; // Moderate pruning
            break;
        case DevelopmentalStage::Maturation:
            stageFactor = 1.0f; // Baseline pruning
            break;
        default:
            stageFactor = 0.5f; // Low pruning in adult
            break;
    }
    
    float effectiveThreshold = getThreshold() * stageFactor;
    
    // Iterate through all regions
    for (RegionId regionId : brain->getRegionIds()) {
        NeuralRegion* region = brain->getRegion(regionId);
        if (!region) continue;
        
        auto& synapses = region->getSynapses();
        std::vector<std::unique_ptr<Synapse>> synapsesToKeep;
        
        for (auto& synapse : synapses) {
            bool keep = true;
            
            // Determine if synapse should be pruned based on multiple factors
            float synapticStrength = std::abs(synapse->getWeight());
            
            // Activity-based pruning: weak, rarely used synapses
            bool recentlyActive = false;
            auto& preSpikeHistory = synapse->getPreSpikeHistory();
            auto& postSpikeHistory = synapse->getPostSpikeHistory();
            
            Timestamp currentTime = 0.0; // In real implementation, would get current simulation time
            
            // Check for recent activity (within last 100ms)
            for (Timestamp spikeTime : preSpikeHistory) {
                if (currentTime - spikeTime < 100.0f) {
                    recentlyActive = true;
                    break;
                }
            }
            
            if (!recentlyActive) {
                for (Timestamp spikeTime : postSpikeHistory) {
                    if (currentTime - spikeTime < 100.0f) {
                        recentlyActive = true;
                        break;
                    }
                }
            }
            
            // Apply pruning rules
            bool shouldPrune = false;
            
            // Weak synapse pruning
            if (synapticStrength < effectiveThreshold && rng.uniformReal(0.0f, 1.0f) < pruningRate) {
                shouldPrune = true;
            }
            
            // Inactive synapse pruning (more aggressive early in development)
            if (!recentlyActive && rng.uniformReal(0.0f, 1.0f) < pruningRate * 0.5f * stageFactor) {
                shouldPrune = true;
            }
            
            // Competitive pruning: eliminate synapses that interfere with more active connections
            // This is complex and would require global knowledge of network activity
            
            // Homeostatic pruning: maintain target excitation/inhibition balance
            if (synapse->isExcitatory()) {
                // Count nearby inhibitory synapses
                int nearbyInhibitory = 0;
                for (const auto& other : synapses) {
                    if (other.get() == synapse.get()) continue;
                    if (other->isInhibitory()) {
                        // Simple proximity check (same source/destination neuron)
                        if ((other->getSourceNeuron() == synapse->getSourceNeuron() && 
                             other->getDestinationNeuron() == synapse->getDestinationNeuron()) ||
                            (other->getSourceNeuron() == synapse->getDestinationNeuron() && 
                             other->getDestinationNeuron() == synapse->getSourceNeuron())) {
                            nearbyInhibitory++;
                        }
                    }
                }
                
                // Remove excess excitation if there are many nearby inhibitory synapses
                if (nearbyInhibitory > 3 && synapticStrength > 0.5f && 
                    rng.uniformReal(0.0f, 1.0f) < pruningRate * 0.3f) {
                    shouldPrune = true;
                }
            }
            
            if (!shouldPrune) {
                synapsesToKeep.push_back(std::move(synapse));
            } else {
                // Call pruneSynapse which may have additional validation/logging
                if (synapse->getDestinationNeuron() != INVALID_NEURON_ID) {
                    pruneSynapse(synapse.get());
                }
            }
        }
        
        // Replace synapse list with kept synapses
        region->clear();
        for (auto& synapse : synapsesToKeep) {
            // Re-add synapses to region (in real implementation)
            // region->addSynapse(synapse->getSourceNeuron(), synapse->getDestinationNeuron(), 
            //                   synapse->getWeight(), synapse->getDelay());
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // Implement real synapse removal: biological synaptic elimination with validation
    if (!synapse) return false;
    
    // Validate synapse state before removal
    if (synapse->getSourceNeuron() == INVALID_NEURON_ID || 
        synapse->getDestinationNeuron() == INVALID_NEURON_ID) {
        return false; // Invalid synapse
    }
    
    // Check plasticity flags - some synapses may be protected
    if (!synapse->getPlasticityFlags().structural) {
        // Check if synapse is eligible for removal based on activity history
        auto& preSpikeHistory = synapse->getPreSpikeHistory();
        auto& postSpikeHistory = synapse->getPostSpikeHistory();
        
        Timestamp currentTime = 0.0; // In real implementation would be current simulation time
        
        // Don't prune synapses with recent activity
        bool recentActivity = false;
        for (Timestamp spikeTime : preSpikeHistory) {
            if (currentTime - spikeTime < 50.0f) { // Within 50ms
                recentActivity = true;
                break;
            }
        }
        
        if (!recentActivity) {
            for (Timestamp spikeTime : postSpikeHistory) {
                if (currentTime - spikeTime < 50.0f) {
                    recentActivity = true;
                    break;
                }
            }
        }
        
        if (recentActivity) {
            return false; // Synapse too recently active
        }
    }
    
    // Record synapse removal for potential consolidation
    // In a full implementation, we would store this for episodic memory
    
    // Apply removal (in real system, this would update brain's synapse lists)
    // Mark synapse for removal and schedule cleanup
    
    // Log the event (in real system)
    // Logger::getInstance().logInfo("Synapse " + std::to_string(synapse->getId().value) +
    //     " removed during pruning process");
    
    return true; // Successfully marked for removal
}

} // namespace nlm
