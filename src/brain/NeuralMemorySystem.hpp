#pragma once

#include "Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>

namespace nlm {

// Forward declarations for event-driven computation
struct SpikeEvent;
struct DelayedSpikeEvent;

class NeuralMemorySystem {
public:
    NeuralMemorySystem() = default;
    
    void initialize(Brain* brain);
    
    // Working memory operations
    void storeToNeuron(Brain& brain, NeuronId neuronId, float activation);
    void update(Brain& brain, TimestepDuration dt);
    void clearWorkingMemory(Brain& brain);
    
    // Episodic memory operations
    void storeEpisode(Brain& brain);
    void replayEpisodes(Brain& brain, size_t count);
    void consolidate(Brain& brain, float threshold);
    void clearEpisodicMemory(Brain& brain);
    
    // Associative memory operations
    void storeAssociation(Brain& brain, std::vector<NeuronId> pattern1, 
                         std::vector<NeuronId> pattern2, float strength);
    std::vector<NeuronId> recallAssociation(Brain& brain, 
                                           std::vector<NeuronId> pattern1);
    void updateAssociativeMemory(Brain& brain, TimestepDuration dt);
    
    // Memory management
    size_t getActiveTraces(Brain& brain) const;
    size_t getEpisodeCount(Brain& brain) const;
    
    // Episodic memory management
    const std::vector<const EpisodicMemoryItem*>* getEpisodesForReplay(Brain& brain, 
                                                                       size_t count) const;
    
    void setWorkingMemoryCapacity(Brain& brain, size_t capacity);
    void setMaxEpisodes(Brain& brain, size_t maxEpisodes);
    
    void logMemoryStatus(Brain& brain) const;
    
private:
    // Track active memory traces
    void updateWorkingMemoryTraces(Brain& brain, TimestepDuration dt);
    
    // Competition for working memory slots
    void processWorkingMemoryCompetition(Brain& brain);
    
    // Capture current state as episode
    EpisodicMemoryItem captureCurrentState(Brain& brain) const;
    
    // Replay episode by reactivating neurons
    void replayEpisode(Brain& brain, const EpisodicMemoryItem& episode) const;
    
    // Find similar episodes for consolidation
    std::vector<const EpisodicMemoryItem*> findSimilarEpisodes(
        Brain& brain, const EpisodicMemoryItem& current) const;
    
    // Compress memory representations
    void compressMemoryRepresentation(EpisodicMemoryItem& episode) const;
    
    float compareEpisodes(const EpisodicMemoryItem& episode1,
                         const EpisodicMemoryItem& episode2) const;
};

} // namespace nlm
