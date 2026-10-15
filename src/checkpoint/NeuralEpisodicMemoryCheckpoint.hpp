// NeuralEpisodicMemory checkpoint data structures

#pragma once

#include <vector>
#include <deque>
#include "../core/Types/Types.hpp"

namespace nlm {

// Enhanced EpisodicMemoryItem for checkpointing
struct EpisodicMemoryItemCheckpoint {
    SimulationStep timestamp;
    
    // Sensory state at time of experience
    std::vector<float> sensoryState;
    
    // Position information
    float positionX;
    float positionY;
    float orientation;
    
    // Action taken
    ActionType action;
    
    // Internal state
    float reward;
    float energy;
    float novelty;
    
    // Resulting state
    std::vector<float> resultingSensoryState;
    float resultingReward;
    
    // Associated neural activity
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Time information
    SimulationStep age;
    
    // Integration state
    float predictionError;
    float confidence;
    
    // Social relevance
    bool sociallyRelevant;
    float socialReward;
    
    EpisodicMemoryItemCheckpoint()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0)
        , predictionError(0), confidence(0)
        , sociallyRelevant(false), socialReward(0) {}
};

// NeuralEpisodicMemory checkpoint state
struct NeuralEpisodicMemoryCheckpoint {
    // All episodes
    std::vector<EpisodicMemoryItemCheckpoint> episodes;
    
    // Ephemeral memory buffers
    std::vector<EpisodicMemoryItemCheckpoint> recentEpisodes;
    std::vector<EpisodicMemoryItemCheckpoint> episodicBuffer;
    
    // Memory parameters
    size_t maxEpisodes;
    bool replayEnabled;
    
    // Consolidation state
    float consolidationEfficiency;
    size_t consolidationCount;
    
    // Integration with other systems
    size_t conceptAssociationCount;
    size_t predictionAssociationCount;
    
    NeuralEpisodicMemoryCheckpoint()
        : maxEpisodes(1000), replayEnabled(true)
        , consolidationEfficiency(1.0f), consolidationCount(0)
        , conceptAssociationCount(0), predictionAssociationCount(0) {}
};

// NeuralEpisodicMemory save/load interface
class NeuralEpisodicMemorySaverLoader {
public:
    virtual ~NeuralEpisodicMemorySaverLoader() = default;
    
    // Save episodic memory state
    virtual bool save(const NeuralEpisodicMemoryCheckpoint& checkpoint) const = 0;
    
    // Load episodic memory state
    virtual bool load(NeuralEpisodicMemoryCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual NeuralEpisodicMemoryCheckpoint createEmptyCheckpoint() const = 0;
    
    // Convert checkpoint back to EpisodicMemoryItem for compatibility
    virtual bool convertToMemoryItem(const EpisodicMemoryItemCheckpoint& checkpoint,
                                    EpisodicMemoryItem& item) const = 0;
};

} // namespace nlm
