// MemorySystem.h - Integrated memory management system

#pragma once

#include "BrainImpl.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include <memory>

namespace nlm {

class MemorySystem {
public:
    MemorySystem();
    ~MemorySystem() = default;
    
    // Initialize with brain reference
    void initialize(BrainImpl* brain);
    
    // Core memory operations
    void update(double timestep);
    void storeToNeuron(NeuronId neuronId, float activation);
    void consolidate(double currentTime);
    void replayMemories(double currentTime, size_t episodeCount = 3);
    
    // Memory management
    void clear();
    void setCapacity(size_t capacity);
    void setMaxEpisodes(size_t maxEpisodes);
    
    // Memory access
    const std::vector<NeuronId>& getActiveNeurons() const { return activeNeurons_; }
    const std::vector<float>& getActivations() const { return activations_; }
    size_t getActiveTraces() const { return workingMemory_->getActiveTraces(); }
    size_t getEpisodeCount() const { return episodicMemory_->getEpisodeCount(); }
    
    // Episode management
    void storeEpisode(const EpisodicMemoryItem& episode);
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Statistics
    float getMemoryEfficiency() const;
    size_t getTotalMemoryUsage() const;
    
    // Configuration
    void configure(size_t workingMemoryCapacity, size_t maxEpisodicMemorySize);
    
private:
    BrainImpl* brain_ = nullptr;
    
    // Memory subsystems
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    // Integration state
    std::vector<NeuronId> activeNeurons_;
    std::vector<float> activations_;
    double lastConsolidationTime_;
    double lastReplayTime_;
    
    // Configuration
    size_t workingMemoryCapacity_;
    size_t maxEpisodicMemorySize_;
    float consolidationThreshold_;
    float replayFrequency_;
    
    // Helper methods
    void maintainWorkingMemory();
    void updateEpisodicMemory(double currentTime);
    void applyPatternCompletion(const std::vector<NeuronId>& patterns);
    void compressMemoryIfNeeded();
    
    friend class BrainImpl;
};

} // namespace nlm
