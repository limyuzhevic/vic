#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../brain/Neuron.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Brain;

class BrainMemory {
public:
    BrainMemory() = default;
    ~BrainMemory() = default;
    
    // Configuration and setup
    void initializeAllMemorySystems(Brain* brain);
    void configureMemoryFromConfig(const Config* config);
    
    // Memory updates (called during step)
    void updateWorkingMemory(Brain* brain, TimestepDuration timestep);
    void updateEpisodicMemory(Brain* brain, SimulationStep currentStep);
    void updateAssociativeMemory(Brain* brain, TimestepDuration timestep);
    void updateAllMemorySystems(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    
    // Memory access and statistics
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    
    // Memory management
    void storeInWorkingMemory(Brain* brain, NeuronId neuron, float activation);
    void storeEpisode(Brain* brain, const EpisodicMemoryItem& episode);
    void consolidateMemories(Brain* brain, float threshold);
    
    // Memory statistics and diagnostics
    void collectMemoryStats(Brain* brain);
    size_t getWorkingMemorySize() const { return workingMemorySize_; }
    size_t getEpisodicMemorySize() const { return episodicMemorySize_; }
    size_t getTotalMemoryUsage() const { return workingMemorySize_ + episodicMemorySize_ + associativeMemorySize_; }
    
    // Memory system management
    void clearAllMemories(Brain* brain);
    void backupMemories(Brain* brain);
    void restoreMemories(Brain* brain);
    
    // Memory configuration for experimentation
    void setWorkingMemoryCapacity(Brain* brain, size_t capacity);
    void setEpisodicMemoryMaxEpisodes(Brain* brain, size_t maxEpisodes);
    void enableMemoryReplay(Brain* brain, bool enable);
    void setReplayInterval(Brain* brain, size_t interval);
    
    // Memory-specific processing
    std::vector<NeuronId> getActiveMemoryNeurons(Brain* brain) const;
    std::vector<EpisodicMemoryItem> getRecentEpisodes(Brain* brain, size_t count) const;
    float getMemoryEfficiency(Brain* brain) const;
    
private:
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    // Memory statistics
    size_t workingMemorySize_ = 0;
    size_t episodicMemorySize_ = 0;
    size_t associativeMemorySize_ = 0;
    float memoryEfficiency_ = 0.0f;
    
    // Configuration parameters
    size_t workingMemoryCapacity_ = 100;
    size_t episodicMemoryMaxEpisodes_ = 1000;
    bool memoryReplayEnabled_ = false;
    size_t replayInterval_ = 100;
    
    // Helper methods
    void setupMemoryConnections(Brain* brain);
    void validateMemoryState(Brain* brain) const;
    void optimizeMemoryLayout(Brain* brain);
    void processMemoryConsolidation(Brain* brain, float threshold);
    
    friend class BrainCore; // For internal access
};

} // namespace nlm