// Brain Memory System Implementation - Contains memory system management
#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

// Memory system management
namespace MemoryManagement {

    // Working memory management
    void initializeWorkingMemory(Brain::Impl* impl);
    void updateWorkingMemory(Brain::Impl* impl, const TimestepDuration& timestep);
    void manageWorkingMemoryCapacity(Brain::Impl* impl);
    
    // Episodic memory management
    void initializeEpisodicMemory(Brain::Impl* impl);
    void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep);
    void manageEpisodicMemoryCapacity(Brain::Impl* impl);
    void processEpisodicReplay(Brain::Impl* impl, SimulationStep currentStep);
    void consolidateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep);
    
    // Associative memory management
    void initializeAssociativeMemory(Brain::Impl* impl);
    void updateAssociativeMemory(Brain::Impl* impl);
    void manageAssociativeMemoryCapacity(Brain::Impl* impl);
    
    // Cross-memory system integration
    void integrateMemorySystems(Brain::Impl* impl);
    void synchronizeMemorySystems(Brain::Impl* impl);
    
    // Memory statistics and monitoring
    void updateMemoryStatistics(Brain::Impl* impl);
    std::string getMemoryStatisticsReport(const Brain::Impl* impl);
    
    // Memory compression and optimization
    void compressMemory(Brain::Impl* impl);
    void optimizeMemoryAccessPatterns(Brain::Impl* impl);
}

} // namespace nlm