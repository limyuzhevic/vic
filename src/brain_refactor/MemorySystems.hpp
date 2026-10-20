#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include "ISubsystem.hpp"
#include <memory>

namespace nlm {

class MemorySystems : public ISubsystem {
public:
    MemorySystems();
    ~MemorySystems() override;
    
    // ISubsystem interface
    bool initialize(Brain* brain) override;
    void update(float dt) override;
    void reset() override;
    void logStatus() const override;
    
    // Working memory access
    NeuralWorkingMemory* getWorkingMemory() const;
    void storeToWorkingMemory(NeuronId neuron, float activation);
    std::vector<float> retrieveFromWorkingMemory() const;
    bool containsInWorkingMemory(NeuronId neuron) const;
    
    // Episodic memory access
    NeuralEpisodicMemory* getEpisodicMemory() const;
    void storeEpisode(const EpisodicMemoryItem& episode);
    void replayEpisodes(size_t count);
    float getEpisodicMemoryConsolidationStrength() const;
    
    // Associative memory access
    NeuralAssociativeMemory* getAssociativeMemory() const;
    void storeAssociation(const std::vector<float>& pattern1, const std::vector<float>& pattern2, float strength);
    std::vector<float> retrieveAssociation(const std::vector<float>& pattern) const;
    
    // Memory coordination
    void synchronizeMemorySystems();
    void updateReplaySchedule();
    void applyConsolidation();
    
    // Memory statistics
    size_t getWorkingMemoryCapacity() const;
    size_t getWorkingMemoryActiveTraces() const;
    size_t getEpisodicMemoryEpisodeCount() const;
    size_t getEpisodicMemoryReplayCount() const;
    size_t getConsolidationCount() const;
    
    // Configuration
    void setWorkingMemoryCapacity(size_t capacity);
    void setWorkingMemoryDecayRate(float rate);
    void setEpisodicMemoryMaxEpisodes(size_t maxEpisodes);
    void setReplayInterval(size_t interval);
    void setConsolidationInterval(size_t interval);
    
    // Memory state management
    void clearAllMemory();
    void resetAllMemory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
