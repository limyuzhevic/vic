#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

class MemorySystem {
public:
    MemorySystem();
    ~MemorySystem();
    
    // Disable copying, enable moving
    MemorySystem(const MemorySystem&) = delete;
    MemorySystem& operator=(const MemorySystem&) = delete;
    MemorySystem(MemorySystem&&) noexcept;
    MemorySystem& operator=(MemorySystem&&) noexcept;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Working memory access
    NeuralWorkingMemory* getWorkingMemory();
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    // Episodic memory access
    NeuralEpisodicMemory* getEpisodicMemory();
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    // Associative memory access
    NeuralAssociativeMemory* getAssociativeMemory();
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // Memory operations
    void updateMemory(float timestep);
    void storeSensoryExperience(const class SensoryInput& input, const class Action& action, float reward);
    void updateEpisodicMemory(SimulationStep currentStep, float reward, const class Brain& brain);
    void consolidateMemories(float consolidationThreshold);
    void replayMemories(size_t episodeCount);
    void clearAllMemories();
    
    // Memory statistics
    size_t getWorkingMemoryTraceCount() const;
    size_t getEpisodicMemoryEpisodeCount() const;
    float getMemoryUtilization() const;
    
    // Configuration
    void setWorkingMemoryCapacity(size_t capacity);
    void setMaxEpisodes(size_t maxEpisodes);
    void setConsolidationThreshold(float threshold);
    void setReplayInterval(size_t interval);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm