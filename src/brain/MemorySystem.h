// MemorySystem.h - Aggregates working, episodic, and associative memory
#pragma once

#include "BrainComponentBase.h"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class Brain;

class MemorySystem : public BrainComponentBase {
public:
    MemorySystem();
    ~MemorySystem() override;
    
    // Initialize component with brain reference
    bool initialize(Brain* brain) override;
    
    // Update component state for current timestep
    void update(const TimestepDuration& dt) override;
    
    // Reset component to initial state
    void reset() override;
    
    // Log component status
    void logStatus() const override;
    
    // Get component name
    const char* getName() const override { return "MemorySystem"; }
    
    // Access to individual memory components
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    
    // Working memory methods
    void storeToNeuron(NeuronId neuron, float value);
    float retrieveFromNeuron(NeuronId neuron) const;
    void decay(float decayRate);
    size_t getActiveTraces() const;
    
    // Episodic memory methods
    void storeEpisode(const EpisodicMemoryItem& episode);
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    size_t getEpisodeCount() const;
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    void consolidate(float relevanceThreshold);
    void replayEpisode(const EpisodicMemoryItem* episode);
    std::vector<EpisodicMemoryItem> getEpisodesForReplay(size_t count) const;
    
    // Associative memory methods
    void associate(NeuronId a, NeuronId b, float strength);
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Configuration
    void setCapacity(size_t capacity) { workingMemoryCapacity_ = capacity; }
    void setMaxEpisodes(size_t maxEpisodes) { episodicMaxEpisodes_ = maxEpisodes; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    size_t workingMemoryCapacity_ = 100;
    size_t episodicMaxEpisodes_ = 1000;
};

} // namespace nlm
