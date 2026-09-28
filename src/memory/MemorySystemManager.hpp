#pragma once

#include "../core/Types/Types.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

// Memory System Manager
// Coordinates integration of working memory, episodic memory, and associative memory
// Manages memory updates, consolidation, and replay strategies

class MemorySystemManager {
public:
    MemorySystemManager();
    ~MemorySystemManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update all memory systems
    void update(float timestep);
    
    // Get memory system instances
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    // Configure memory parameters
    void configure(size_t workingMemoryCapacity, size_t maxEpisodes,
                   size_t maxAssociations);
    
    // Trigger memory consolidation
    void consolidate(float importanceThreshold);
    
    // Process memory replay
    void processReplay(uint32_t episodeCount);
    
    // Get memory statistics
    size_t getActiveMemoryTraces() const;
    size_t getEpisodeCount() const;
    size_t getActiveAssociations() const;
    
    // Reset all memory systems
    void reset();
    
    // Set storage efficiency parameters
    void setEfficiencyParameters(float memoryDecayRate, 
                                  float consolidationFrequency);
    
    // Process sensory input for memory formation
    void processSensoryInput(const class SensoryInput& input);
    
    // Record current neural activity in episodic memory
    void recordCurrentActivity(Timestamp timestamp, float reward);
    
    // Update associative memories with new connections
    void updateAssociations(const std::vector<std::pair<NeuronId, float>>& patterns);
    
private:
    // Private implementation for PIMPL pattern
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
