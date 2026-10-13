#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

// MemorySystem: Coordinates memory systems
// Manages working memory, episodic memory, and associative memory
class MemorySystem {
public:
    MemorySystem(std::shared_ptr<Config> config);
    ~MemorySystem();
    
    // Initialize memory systems
    bool initialize(Brain* brain);
    
    // Working memory access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    const NeuralWorkingMemory* getWorkingMemory() const { return workingMemory_.get(); }
    
    // Episodic memory access
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    const NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory_.get(); }
    
    // Associative memory access
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    const NeuralAssociativeMemory* getAssociativeMemory() const { return associativeMemory_.get(); }
    
    // Memory management
    void update(double timestep);
    void storeEpisode(const EpisodicMemoryItem& episode);
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    size_t getEpisodeCount() const;
    const std::vector<const EpisodicMemoryItem*>& getEpisodesForReplay(size_t maxCount) const;
    void replayEpisode(const EpisodicMemoryItem* episode);
    void consolidate(float threshold);
    void clear();
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Capacity management
    void setWorkingMemoryCapacity(size_t capacity);
    void setEpisodicMemoryCapacity(size_t maxEpisodes);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
