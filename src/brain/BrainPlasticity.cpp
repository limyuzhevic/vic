#pragma once

namespace nlm {

/**
 * @file BrainPlasticity.cpp
 * @brief Implementation of brain plasticity systems
 * 
 * This file contains the implementation of plasticity mechanisms that enable
 * learning and adaptation in the brain:
 * - SpikeSystem: Real-time spike detection and processing
 * - STDP: Spike-Timing-Dependent Plasticity for synaptic learning
 * - Hebbian: Hebbian learning rules for synaptic strengthening
 * - StructuralPlasticity: Synaptogenesis and pruning for network structure
 * 
 * Plasticity systems allow the brain to adapt its connectivity based on
 * neural activity patterns, enabling learning, memory formation, and
 * development.
 */

#include "Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <algorithm>
#include <cmath>

/**
 * @brief Plasticity system implementation for the Brain class
 * 
 * This section contains methods and helper functions that manage the brain's
 * plasticity systems. Plasticity mechanisms provide:
 * - Synaptic weight modification based on activity patterns
 * - Structural changes in neural connectivity
 * - Real-time spike processing and event handling
 * - Learning rules for different types of plasticity
 */

// Plasticity system getter implementations
SpikeSystem* Brain::getSpikeSystem() {
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem.get();
    }
    return nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem.get();
    }
    return nullptr;
}

STDP* Brain::getSTDP() {
    if (pImpl->stdp) {
        return pImpl->stdp.get();
    }
    return nullptr;
}

Hebbian* Brain::getHebbian() {
    if (pImpl->hebbian) {
        return pImpl->hebbian.get();
    }
    return nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    if (pImpl->structuralPlasticity) {
        return pImpl->structuralPlasticity.get();
    }
    return nullptr;
}

/**
 * @brief Get total spike count since brain initialization
 * 
 * @return Total number of spikes recorded
 */
size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

/**
 * @brief Get current step spike count
 * 
 * @return Number of spikes in the current simulation step
 */
size_t Brain::getCurrentStepSpikeCount() const {
    return pImpl->totalSpikesThisStep;
}

/**
 * @brief Get pending spike event count
 * 
 * @return Total number of pending spike events (immediate + delayed)
 */
size_t Brain::getPendingSpikeEventCount() const {
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem->getPendingSpikeCount() + 
               pImpl->spikeSystem->getPendingDelayedCount();
    }
    return 0;
}

/**
 * @brief Calculate excitation/inhibition ratio
 * 
 * @return Ratio of total excitatory weight to total inhibitory weight
 */
float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
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

/**
 * @brief Update plasticity rules
 * 
 * This function applies all plasticity rules to the current synaptic states,
 * including:
 * - Spike-Timing-Dependent Plasticity (STDP)
 * - Hebbian learning
 * - Neuromodulation effects on plasticity
 * 
 * @param currentTime Current simulation time
 */
void Brain::updatePlasticity() {
    // Plasticity is applied during each simulation step
    // This method is kept for API compatibility
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(pImpl->currentStep);
    }
}

/**
 * @brief Apply structural plasticity
 * 
 * This function applies structural plasticity mechanisms that modify the
 * physical connectivity of the brain, including synaptogenesis (formation of
 * new synapses) and pruning (elimination of weak synapses).
 * 
 * @param rng Random number generator for stochastic processes
 */
void Brain::applyStructuralPlasticity(RandomGenerator& rng) {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, rng);
    }
}

/**
 * @brief Configure STDP parameters
 * 
 * @param ltpWeight Long-term potentiation weight factor
 * @param ltdWeight Long-term depression weight factor
 * @param tau STDP time constant
 */
void Brain::configureSTDP(float ltpWeight, float ltdWeight, float tau) {
    if (pImpl->stdp) {
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    }
}

/**
 * @brief Get STDP statistics
 * 
 * @return String containing STDP-related statistics
 */
std::string Brain::getSTDPStats() const {
    std::stringstream stats;
    stats << "STDP Statistics:\n";
    
    if (pImpl->stdp) {
        stats << "  LTP Weight: " << pImpl->stdp->getLTPWeight() << "\n";
        stats << "  LTD Weight: " << pImpl->stdp->getLTDWeight() << "\n";
        stats << "  Time Constant: " << pImpl->stdp->getTau() << "\n";
    }
    
    return stats.str();
}

/**
 * @brief Get current connectivity statistics
 * 
 * @return String containing connectivity statistics
 */
std::string Brain::getConnectivityStats() const {
    std::stringstream stats;
    stats << "Connectivity Statistics:\n";
    
    size_t totalSynapses = 0;
    size_t totalExcitatory = 0;
    size_t totalInhibitory = 0;
    
    for (const auto& region : pImpl->regions) {
        totalSynapses += region->getSynapseCount();
        for (const auto& syn : region->getSynapses()) {
            if (syn->getWeight() > 0) {
                totalExcitatory++;
            } else {
                totalInhibitory++;
            }
        }
    }
    
    stats << "  Total Synapses: " << totalSynapses << "\n";
    stats << "  Excitatory Synapses: " << totalExcitatory << "\n";
    stats << "  Inhibitory Synapses: " << totalInhibitory << "\n";
    
    if (totalExcitatory > 0 && totalInhibitory > 0) {
        float ratio = static_cast<float>(totalExcitatory) / totalInhibitory;
        stats << "  E/I Ratio: " << ratio << "\n";
    }
    
    return stats.str();
}

} // namespace nlm
