#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>

namespace nlm {

class MemorySystem {
public:
    MemorySystem(Brain* brain, 
                 NeuralWorkingMemory* workingMemory,
                 NeuralEpisodicMemory* episodicMemory,
                 NeuralAssociativeMemory* associativeMemory);
    
    // Update working memory with current neural activity
    void updateWorkingMemory(TimestepDuration dt);
    
    // Update episodic memory with current experience
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    
    // Store current brain state as an episode
    void storeCurrentEpisode(SimulationStep currentStep, Timestamp currentTime);
    
    // Consolidate memory based on importance/relevance
    void consolidateMemories(float threshold);
    
    // Replay important memories to strengthen connections
    void replayMemories(size_t count, SimulationStep currentStep);
    
    // Update developmental effects on memory systems
    void updateDevelopment(double timestep);
    
    // Get memory statistics and state
    size_t getWorkingMemoryActiveTraces() const;
    size_t getEpisodicMemoryEpisodeCount() const;
    float getMemoryConsolidationLevel() const;
    
    // Memory control
    void clearAllMemories();
    void enableReplay(bool enable);
    
    // Set memory capacities and limits
    void setWorkingMemoryCapacity(size_t capacity);
    void setEpisodicMemoryMaxEpisodes(size_t maxEpisodes);
    void setConsolidationInterval(size_t interval);
    
private:
    Brain* brain;
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    NeuralAssociativeMemory* associativeMemory;
    
    // State tracking
    size_t stepsSinceLastEpisode;
    size_t consolidationInterval;
    bool replayEnabled;
};
