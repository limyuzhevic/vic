#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

class BrainMemoryManager {
public:
    // Create memory manager with configuration
    explicit BrainMemoryManager(std::shared_ptr<Config> config);
    
    ~BrainMemoryManager();
    
    // Initialize memory manager
    bool initialize(BrainNeuralCore* neuralCore);
    
    // Update memory systems
    void update(TimestepDuration timestep);
    
    // Memory operations
    void storeToWorkingMemory(NeuronId neuronId, float activation);
    void updateEpisodicMemory(const EpisodicMemoryItem& episode);
    void storeAssociativeMemory(const std::vector<NeuronId>& pattern1,
                                const std::vector<NeuronId>& pattern2,
                                float strength);
    
    // Memory retrieval
    std::vector<NeuronId> retrieveFromWorkingMemory(size_t maxTraces) const;
    const std::vector<EpisodicMemoryItem>* getEpisodesForReplay(size_t maxEpisodes) const;
    float retrieveAssociativeMemory(const std::vector<NeuronId>& pattern) const;
    
    // Memory management
    void clearAllMemory();
    void consolidateMemory(float threshold);
    void replayEpisodes(BrainNeuralCore* neuralCore, size_t episodeCount);
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    NeuralEpisodicMemory* getEpisodicMemory();
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    NeuralAssociativeMemory* getAssociativeMemory();
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // Memory statistics
    size_t getWorkingMemorySize() const;
    size_t getEpisodicMemorySize() const;
    size_t getAssociativeMemorySize() const;
    
    // Configuration
    void setWorkingMemoryCapacity(size_t capacity);
    void setEpisodicMemoryMaxEpisodes(size_t maxEpisodes);
    
    // Reset memory state
    void reset();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Log memory manager status
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm