// src/plasticity/StructuralPlasticity.cpp
// Implementation of structural plasticity: creation and removal of synapses and neurons

#include "StructuralPlasticity.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Forward declarations for helper functions
namespace {
    NeuronId findAvailableNeuronId(Brain* brain);
    bool isValidConnection(Brain* brain, NeuronId source, NeuronId destination);
}

struct StructuralPlasticity::Impl {
    // Structural plasticity parameters
    float synaptogenesisRate;  // Rate of new synapse creation per timestep
    float pruningRate;         // Rate of synapse removal per timestep
    float neuronBirthRate;     // Rate of new neuron creation
    float neuronDeathRate;     // Rate of neuron removal
    
    // Historical tracking for plasticity regulation
    float recentActivity;      // Recent network activity (0-1)
    size_t totalSynapsesCreated;
    size_t totalSynapsesRemoved;
    size_t totalNeuronsCreated;
    size_t totalNeuronsRemoved;
    
    // Plasticity state
    float activityThreshold;  // Threshold for structural changes
    float maturationFactor;   // Factor affecting neuron maturation
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             neuronBirthRate(0.001f), neuronDeathRate(0.00005f),
             recentActivity(0.0f), totalSynapsesCreated(0), totalSynapsesRemoved(0),
             totalNeuronsCreated(0), totalNeuronsRemoved(0),
             activityThreshold(0.5f), maturationFactor(0.8f) {}
};

StructuralPlasticity::StructuralPlasticity() : pImpl(new Impl()) {
    NLM_LOG_INFO("StructuralPlasticity initialized");
    NLM_LOG_INFO("  Synaptogenesis rate: " + std::to_string(pImpl->synaptogenesisRate));
    NLM_LOG_INFO("  Pruning rate: " + std::to_string(pImpl->pruningRate));
}

StructuralPlasticity::~StructuralPlasticity() {
    delete pImpl;
}

// Move constructor
StructuralPlasticity::StructuralPlasticity(StructuralPlasticity&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

// Move assignment operator
StructuralPlasticity& StructuralPlasticity::operator=(StructuralPlasticity&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

SynapseId StructuralPlasticity::createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                                           SynapticWeight weight) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot create synapse: invalid brain pointer");
        return INVALID_SYNAPSE_ID;
    }
    
    // Validate source and destination neurons
    auto* sourceRegion = brain->getRegion(brain->getNeuron(source).getRegionId());
    auto* destRegion = brain->getRegion(brain->getNeuron(destination).getRegionId());
    
    if (!sourceRegion || !destRegion) {
        NLM_LOG_ERROR("Cannot create synapse: invalid neuron regions");
        return INVALID_SYNAPSE_ID;
    }
    
    // Check if connection is allowed (no self-connections, no duplicate connections)
    if (source == destination) {
        NLM_LOG_WARNING("Cannot create synapse: self-connection not allowed");
        return INVALID_SYNAPSE_ID;
    }
    
    // Check for existing connection
    for (const auto& syn : sourceRegion->getSynapses()) {
        if (syn->getDestinationNeuron() == destination) {
            NLM_LOG_WARNING("Synapse already exists between neurons " + 
                           std::to_string(source) + " and " + std::to_string(destination));
            return syn->getId();
        }
    }
    
    // Create new synapse with unique ID
    SynapseId newId = static_cast<SynapseId>(pImpl->totalSynapsesCreated + 1);
    
    // In a real implementation, this would actually create a synapse in the brain
    // For now, we'll just log the attempt
    NLM_LOG_INFO("Attempting to create synapse " + std::to_string(newId) +
                " between neurons " + std::to_string(source) + " and " + std::to_string(destination) +
                " with weight " + std::to_string(weight));
    
    pImpl->totalSynapsesCreated++;
    return newId;
}

bool StructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot remove synapse: invalid brain pointer");
        return false;
    }
    
    // In a real implementation, this would remove the synapse from the brain
    // For now, we'll just log the attempt
    NLM_LOG_INFO("Removing synapse " + std::to_string(synapse));
    pImpl->totalSynapsesRemoved++;
    return true;
}

NeuronId StructuralPlasticity::createNeuron(Brain* brain, NeuronType type) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot create neuron: invalid brain pointer");
        return INVALID_NEURON_ID;
    }
    
    // Find a valid region to place the new neuron
    auto* region = brain->getRegion(RegionId(1));  // Default to first region
    if (!region) {
        NLM_LOG_ERROR("Cannot create neuron: no regions found");
        return INVALID_NEURON_ID;
    }
    
    // Get the next available neuron ID
    size_t currentCount = brain->getTotalNeuronCount();
    NeuronId newId = static_cast<NeuronId>(currentCount + 1);
    
    // In a real implementation, this would create a neuron in the brain
    // For now, we'll just log the attempt
    NLM_LOG_INFO("Creating new neuron " + std::to_string(newId) +
                " of type " + std::to_string(static_cast<int>(type)));
    
    pImpl->totalNeuronsCreated++;
    return newId;
}

bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot remove neuron: invalid brain pointer");
        return false;
    }
    
    // Check if neuron exists
    try {
        auto& n = brain->getNeuron(neuron);
        NLM_LOG_INFO("Removing neuron " + std::to_string(neuron));
        pImpl->totalNeuronsRemoved++;
        return true;
    } catch (...) {
        NLM_LOG_ERROR("Cannot remove neuron " + std::to_string(neuron) + ": neuron not found");
        return false;
    }
}

float StructuralPlasticity::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

void StructuralPlasticity::setSynaptogenesisRate(float rate) {
    if (rate < 0.0f) {
        NLM_LOG_WARNING("Synaptogenesis rate cannot be negative, using absolute value");
        rate = std::abs(rate);
    }
    pImpl->synaptogenesisRate = rate;
    NLM_LOG_INFO("Synaptogenesis rate set to: " + std::to_string(rate));
}

float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

void StructuralPlasticity::setPruningRate(float rate) {
    if (rate < 0.0f) {
        NLM_LOG_WARNING("Pruning rate cannot be negative, using absolute value");
        rate = std::abs(rate);
    }
    pImpl->pruningRate = rate;
    NLM_LOG_INFO("Pruning rate set to: " + std::to_string(rate));
}

void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot update structural plasticity: invalid brain pointer");
        return;
    }
    
    // Update recent activity based on spike counts
    size_t totalSpikes = brain->getTotalSpikeCount();
    pImpl->recentActivity = std::min(1.0f, static_cast<float>(totalSpikes) / 10000.0f); // Normalize
    
    // Determine if structural changes should occur based on activity
    float activityThreshold = pImpl->activityThreshold * (1.0f + pImpl->recentActivity * 0.5f);
    
    // Synaptogenesis (create new synapses)
    if (pImpl->recentActivity > activityThreshold && rng.uniformReal(0.0f, 1.0f) < pImpl->synaptogenesisRate) {
        NLM_LOG_INFO("Synaptogenesis triggered by high activity");
    }
    
    // Pruning (remove weak synapses)
    if (pImpl->recentActivity < 0.1f && rng.uniformReal(0.0f, 1.0f) < pImpl->pruningRate) {
        NLM_LOG_INFO("Pruning triggered by low activity");
    }
    
    // Neuron creation
    if (rng.uniformReal(0.0f, 1.0f) < pImpl->neuronBirthRate) {
        NLM_LOG_INFO("Neuron birth event triggered");
    }
    
    // Neuron death
    if (pImpl->recentActivity < 0.05f && rng.uniformReal(0.0f, 1.0f) < pImpl->neuronDeathRate) {
        NLM_LOG_INFO("Neuron death event triggered");
    }
    
    // Log statistics
    NLM_LOG_INFO("Structural plasticity update completed");
    NLM_LOG_INFO("  Total synapses created: " + std::to_string(pImpl->totalSynapsesCreated));
    NLM_LOG_INFO("  Total synapses removed: " + std::to_string(pImpl->totalSynapsesRemoved));
    NLM_LOG_INFO("  Total neurons created: " + std::to_string(pImpl->totalNeuronsCreated));
    NLM_LOG_INFO("  Total neurons removed: " + std::to_string(pImpl->totalNeuronsRemoved));
    NLM_LOG_INFO("  Recent activity level: " + std::to_string(pImpl->recentActivity));
}

// Helper function implementations
namespace {
    NeuronId findAvailableNeuronId(Brain* brain) {
        if (!brain) return INVALID_NEURON_ID;
        return static_cast<NeuronId>(brain->getTotalNeuronCount() + 1);
    }
    
    bool isValidConnection(Brain* brain, NeuronId source, NeuronId destination) {
        if (!brain) return false;
        
        // Check if both neurons exist
        try {
            auto& sourceNeuron = brain->getNeuron(source);
            auto& destNeuron = brain->getNeuron(destination);
            
            // Check if they are in the same region
            auto sourceRegionId = sourceNeuron.getRegionId();
            auto destRegionId = destNeuron.getRegionId();
            
            return sourceRegionId == destRegionId;
        } catch (...) {
            return false;
        }
    }
}

} // namespace nlm
