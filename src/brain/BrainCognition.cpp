#pragma once

namespace nlm {

/**
 * @file BrainCognition.cpp
 * @brief Implementation of brain cognition systems
 * 
 * This file contains the implementation of cognitive systems that enable
 * intelligent behavior and thought processes:
 * - NeuralPlanner: Action planning and decision making
 * - ConceptFormation: Pattern recognition and abstraction
 * - AttentionalSelection: Focus and selective attention
 * 
 * Cognitive systems process sensory information, plan actions, and select
 * relevant information for processing, enabling intelligent behavior.
 */

#include "Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <algorithm>

/**
 * @brief Cognition system implementation for the Brain class
 * 
 * This section contains methods and helper functions that manage the brain's
 * cognitive systems. Cognitive mechanisms provide:
 * - Action planning and decision making
 * - Pattern recognition and abstraction
 * - Selective attention and focus
 * - Goal-directed behavior
 */

// Cognition system getter implementations
NeuralPlanner* Brain::getPlanner() {
    if (pImpl->planner) {
        return pImpl->planner.get();
    }
    return nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    if (pImpl->conceptFormation) {
        return pImpl->conceptFormation.get();
    }
    return nullptr;
}

AttentionalSelection* Brain::getAttention() {
    if (pImpl->attention) {
        return pImpl->attention.get();
    }
    return nullptr;
}

/**
 * @brief Get working memory neurons
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
 * @brief Process cognitive competition
 * 
 * This function processes competition between different cognitive candidates
 * for selection and prioritization of information and actions.
 * 
 * @param competitors Vector of neuron IDs competing for attention
 */
void Brain::processCognitiveCompetition(const std::vector<NeuronId>& competitors) {
    if (pImpl->attention) {
        pImpl->attention->processCompetition(competitors);
    }
}

/**
 * @brief Get cognitive statistics
 * 
 * @return String containing cognitive system statistics
 */
std::string Brain::getCognitiveStats() const {
    std::stringstream stats;
    stats << "Cognitive Statistics:\n";
    
    if (pImpl->planner) {
        stats << "  NeuralPlanner: Active\n";
        // Could add more detailed planner statistics here
    }
    
    if (pImpl->conceptFormation) {
        stats << "  ConceptFormation: Active\n";
        // Could add more detailed concept formation statistics here
    }
    
    if (pImpl->attention) {
        stats << "  AttentionalSelection: Active\n";
        // Could add more detailed attention statistics here
    }
    
    return stats.str();
}

/**
 * @brief Update cognitive systems
 * 
 * This function updates all cognitive systems based on current brain state
 * and environmental inputs. It processes:
 * - Planning for future actions
 * - Formation of new concepts from patterns
 * - Selection of relevant information for processing
 * - Integration with other brain systems
 * 
 * @param timestep Current simulation timestep
 */
void Brain::updateCognitiveSystems(TimestepDuration timestep) {
    if (pImpl->planner) {
        pImpl->planner->update(timestep);
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(timestep);
    }
    
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
    }
}

/**
 * @brief Reset cognitive systems
 */
void Brain::resetCognitiveSystems() {
    if (pImpl->planner) {
        pImpl->planner->reset();
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->reset();
    }
    
    if (pImpl->attention) {
        pImpl->attention->reset();
    }
}

} // namespace nlm
