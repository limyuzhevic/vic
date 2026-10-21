#pragma once

namespace nlm {

/**
 * @file BrainMemory.cpp
 * @brief Implementation of brain memory systems
 * 
 * This file contains the implementation of all memory-related systems in the brain:
 * - NeuralWorkingMemory: Transient active information storage
 * - NeuralEpisodicMemory: Experience and episode storage
 * - NeuralAssociativeMemory: Pattern association and recall
 * 
 * These memory systems work together to store, maintain, and retrieve neural
 * activity patterns over different timescales.
 */

#include "Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include <algorithm>
#include <cmath>

// Forward declarations
namespace nlm {
    class Brain;
    class Dopamine;
}

/**
 * @brief Memory system implementation for the Brain class
 * 
 * This section contains methods and helper functions that manage the brain
 * memory systems. The implementation provides:
 * - Integration between memory systems and other brain components
 * - Memory initialization and configuration
 * - Memory-related statistics and metrics
 * - Memory maintenance and consolidation routines
 */

// Memory system getter implementations
NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory.get();
    }
    return nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory.get();
    }
    return nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (pImpl->associativeMemory) {
        return pImpl->associativeMemory.get();
    }
    return nullptr;
}

/**
 * @brief Get working memory statistics
 * 
 * @return Vector of NeuronIds representing currently active memory items
 */
std::vector<NeuronId> Brain::getWorkingMemoryNeurons() const {
    std::vector<NeuronId> memoryNeurons;
    if (pImpl->workingMemory) {
        memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
    }
    return memoryNeurons;
}

/**
 * @brief Get episodic memory statistics
 * 
 * @return Number of stored episodes
 */
size_t Brain::getEpisodicMemoryCount() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodeCount();
    }
    return 0;
}

/**
 * @brief Get episodic memory statistics
 * 
 * @return Number of stored episodes
 */
size_t Brain::getEpisodicMemoryCount() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodeCount();
    }
    return 0;
}

/**
 * @brief Get episodic memory statistics
 * 
 * @return Number of stored episodes
 */
size_t Brain::getEpisodicMemoryCount() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodeCount();
    }
    return 0;
}

/**
 * @brief Get episodic memory statistics
 * 
 * @return Number of stored episodes
 */
size_t Brain::getEpisodicMemoryCount() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodeCount();
    }
    return 0;
}

/**
 * @brief Reset working memory system
 */
void Brain::resetWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
    }
}

/**
 * @brief Reset episodic memory system
 */
void Brain::resetEpisodicMemory() {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->clear();
    }
}

/**
 * @brief Reset associative memory system
 */
void Brain::resetAssociativeMemory() {
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->clear();
    }
}

/**
 * @brief Consolidate episodic memory
 * 
 * @param consolidationThreshold Threshold for memory consolidation (0.0 to 1.0)
 */
void Brain::consolidateEpisodicMemory(float consolidationThreshold) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(consolidationThreshold);
    }
}

/**
 * @brief Store current brain state as an episodic memory item
 * 
 * @param reward Current reward signal from the environment
 */
void Brain::storeEpisodicMemory(float reward) {
    if (!pImpl->episodicMemory) {
        return;
    }
    
    EpisodicMemoryItem episode;
    episode.timestamp = pImpl->currentStep;
    episode.reward = reward;
    
    // Store active neurons and their activations
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                if (neuron->isFiring() || 
                    std::abs(state.membranePotential - state.restingPotential) > 5.0f) {
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(
                        std::abs(state.membranePotential - state.restingPotential) / 20.0f);
                }
            }
        }
    }
    
    pImpl->episodicMemory->storeEpisode(episode);
}

/**
 * @brief Get recent memory items for replay
 * 
 * @param count Number of memory items to retrieve
 * @return Vector of episodic memory item pointers
 */
std::vector<const EpisodicMemoryItem*> Brain::getRecentMemoryItems(size_t count) {
    std::vector<const EpisodicMemoryItem*> memoryItems;
    if (pImpl->episodicMemory) {
        memoryItems = pImpl->episodicMemory->getEpisodesForReplay(count);
    }
    return memoryItems;
}

} // namespace nlm
