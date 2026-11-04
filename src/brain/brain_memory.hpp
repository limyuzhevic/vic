#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

// Memory systems integration
class BrainMemory {
public:
    BrainMemory() = default;
    ~BrainMemory() = default;
    
    // Initialize memory systems
    void initialize(std::shared_ptr<class Brain> brain, size_t neuronCount);
    
    // Working memory operations
    void updateWorkingMemory(double timestep,
                           std::shared_ptr<NeuralWorkingMemory> workingMemory,
                           const std::vector<std::unique_ptr<class NeuralRegion>>& regions);
    
    // Episodic memory operations
    void storeEpisodicMemory(const std::vector<std::unique_ptr<class NeuralRegion>>& regions,
                           SimulationStep currentStep,
                           float reward,
                           std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                           std::shared_ptr<class Dopamine> dopamine,
                           SimulationStep& stepsSinceLastEpisode,
                           const SimulationStep replayInterval,
                           const SimulationStep consolidationInterval);
    
    // Memory consolidation and replay
    void updateMemoryConsolidation(SimulationStep currentStep,
                                  std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                                  const SimulationStep replayInterval,
                                  const SimulationStep consolidationInterval);
    
    // Get memory system accessors
    NeuralWorkingMemory* getWorkingMemory() const;
    NeuralEpisodicMemory* getEpisodicMemory() const;
    NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // Clear memory
    void clearWorkingMemory(std::shared_ptr<NeuralWorkingMemory> workingMemory);
    void clearEpisodicMemory(std::shared_ptr<NeuralEpisodicMemory> episodicMemory);
    void clearAssociativeMemory(std::shared_ptr<NeuralAssociativeMemory> associativeMemory);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
