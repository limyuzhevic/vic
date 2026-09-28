#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Memory System Integration
// Handles working memory, episodic memory, and associative memory integration

class BrainMemory {
public:
    // Initialize memory systems
    void initializeMemorySystems(Brain::Impl& pImpl);
    
    // Update working memory (transient active information)
    void updateWorkingMemory(Brain::Impl& pImpl, double timestep);
    
    // Update episodic memory (experience storage)
    void updateEpisodicMemory(Brain::Impl& pImpl, SimulationStep currentStep);
    
    // Update associative memory (pattern associations)
    void updateAssociativeMemory(Brain::Impl& pImpl, SimulationStep currentStep);
    
    // Capture current brain state as an episode
    void captureCurrentState(Brain::Impl& pImpl, SimulationStep currentStep);
    
    // Get memory system statistics
    void logMemoryStats(const Brain::Impl& pImpl) const;
    
private:
    // Helper functions for memory integration
    void integrateMemoryWithNeurons(Brain::Impl& pImpl);
    void updateMemoryWithNeuromodulation(Brain::Impl& pImpl);
    void handleMemoryConsolidation(Brain::Impl& pImpl, SimulationStep currentStep);
};

} // namespace nlm
