#pragma once

#include "../cognition/AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

class BrainMemory {
public:
    // Constructor
    BrainMemory(std::shared_ptr<Config> config);
    ~BrainMemory();
    
    // Initialize memory systems
    void initialize(Brain* brain);
    
    // Update working memory
    void updateWorkingMemory(TimestepDuration timestep);
    
    // Store sensory input in working memory
    void storeSensoryInput(NeuronId neuronId, float activation);
    
    // Update episodic memory
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime, Brain* brain);
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Update associative memory
    void updateAssociativeMemory();
    
    // Replay important memories
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count);
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Memory consolidation
    void consolidateMemories(float threshold);
    
    // Memory system accessors
    NeuralWorkingMemory* getWorkingMemory();
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    NeuralEpisodicMemory* getEpisodicMemory();
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    NeuralAssociativeMemory* getAssociativeMemory();
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // Working memory helper
    void collectActiveNeuronsForWorkingMemory(Brain* brain);
    
    // Episodic memory helper
    void captureCurrentStateAsEpisode(SimulationStep currentStep, Timestamp currentTime, Brain* brain);
    
    // Statistics
    size_t getWorkingMemorySize() const { return workingMemorySize; }
    size_t getEpisodicMemorySize() const { return episodicMemorySize; }
    size_t getEpisodeCount() const { return episodeCount; }
    
    // Reset memory state
    void reset();
    
    // Log memory system status
    void logStatus() const;
    
private:
    // Memory system components
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Configuration
    std::shared_ptr<Config> config;
    
    // Statistics
    size_t workingMemorySize;
    size_t episodicMemorySize;
    size_t episodeCount;
    
    // State tracking
    size_t stepsSinceLastEpisode;
    SimulationStep lastReplayStep;
    
    // Helper methods
    void initializeFromConfig();
    void updateMemoryCompetition(Brain* brain);
};

} // namespace nlm