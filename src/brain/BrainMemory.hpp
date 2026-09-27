// BrainMemory.hpp - Memory system integration interface
#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../brain/Types/SimulationStep.hpp"
#include "../brain/Types/Timestamp.hpp"
#include <vector>

namespace nlm {

class Brain;
struct EpisodicMemoryItem;

class BrainMemory {
public:
    BrainMemory();
    ~BrainMemory();
    
    // Set reference to parent brain
    void setBrain(Brain* brain) { brain_ = brain; }
    
    // Initialize memory systems with brain reference
    void initializeMemorySystems(Brain* brain);
    
    // Update working memory based on neural activity
    void updateWorkingMemory(double timestep);
    
    // Store current neural activity in episodic memory
    void storeEpisode(EpisodicMemoryItem& episode, SimulationStep step, Timestamp time);
    
    // Process memory consolidation
    void consolidateMemory(float threshold);
    
    // Get memory statistics
    size_t getWorkingMemoryActiveTraces() const;
    size_t getEpisodicMemoryEpisodeCount() const;
    
    // Access to underlying memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
private:
    Brain* brain_;
    
    // Reference to memory systems
    NeuralWorkingMemory* workingMemory_;
    NeuralEpisodicMemory* episodicMemory_;
    NeuralAssociativeMemory* associativeMemory_;
};

} // namespace nlm