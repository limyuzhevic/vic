#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include <memory>
#include <vector>

namespace nlm {

// ==========================================
// MEMORY SYSTEM - Memory Systems
// ==========================================
// 
// Manages: working memory, episodic memory, associative memory
// 
// Contains: NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory
// Handles: memory storage, retrieval, consolidation
// Manages: memory capacity and replay

class MemorySystem {
public:
    MemorySystem(std::shared_ptr<Config> config);
    ~MemorySystem();
    
    // Disable copying, enable moving
    MemorySystem(const MemorySystem&) = delete;
    MemorySystem& operator=(const MemorySystem&) = delete;
    MemorySystem(MemorySystem&&) noexcept;
    MemorySystem& operator=(MemorySystem&&) noexcept;
    
    // Initialize memory system with brain reference
    bool initialize(Brain* brain);
    
    // Get memory system components
    class NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    const class NeuralWorkingMemory* getWorkingMemory() const { return workingMemory_.get(); }
    
    class NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    const class NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory_.get(); }
    
    class NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    const class NeuralAssociativeMemory* getAssociativeMemory() const { return associativeMemory_.get(); }
    
    // Get brain reference
    Brain* getBrain() { return brain_; }
    const Brain* getBrain() const { return brain_; }
    
    // Update memory systems
    void update(TimestepDuration dt);
    
    // Memory management
    void storeMemory(const std::vector<float>& pattern, float strength = 1.0f);
    std::vector<float> retrieveMemory() const;
    void clearMemory();
    
    // Episode management
    void storeEpisode(const EpisodicMemoryItem& episode);
    const std::vector<const EpisodicMemoryItem*>& getEpisodesForReplay(size_t count) const;
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Consolidation
    void consolidate(float threshold);
    
    // Replay management
    void scheduleReplay(SimulationStep step);
    bool shouldReplay(SimulationStep step) const;
    
    // Get statistics
    size_t getActiveMemoryTraces() const;
    size_t getEpisodeCount() const;
    float getMemoryActivity() const;
    
    // Memory capacity management
    void setWorkingMemoryCapacity(size_t capacity);
    void setEpisodicMemoryCapacity(size_t maxEpisodes);
    
    // Reset memory system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::shared_ptr<Config> config_;
    
    std::unique_ptr<class NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<class NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<class NeuralAssociativeMemory> associativeMemory_;
};

} // namespace nlm