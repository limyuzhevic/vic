#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

// Memory system status information
struct MemorySystemStatus {
    size_t workingMemoryTraces{0};
    size_t episodicMemoryEpisodes{0};
    size_t associativeMemoryAssociations{0};
    
    // Check for consistency
    bool isConsistent() const {
        return workingMemoryTraces <= 1000 &&  // Reasonable limits
                episodicMemoryEpisodes <= 10000 &&
                associativeMemoryAssociations <= 10000;
    }
    
    std::string toString() const {
        return "WorkingMemory: " + std::to_string(workingMemoryTraces) +
               ", EpisodicMemory: " + std::to_string(episodicMemoryEpisodes) +
               ", AssociativeMemory: " + std::to_string(associativeMemoryAssociations);
    }
};

class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Initialize all memory systems
    bool initialize(Brain* brain);
    
    // Update memory systems
    void update(float timestep);
    
    // Store and retrieve data
    void storeToNeuron(NeuronId neuronId, float value);
    float retrieveFromNeuron(NeuronId neuronId) const;
    bool containsNeuron(NeuronId neuronId) const;
    
    // Episodic memory management
    void storeEpisode(const EpisodicMemoryItem& episode);
    std::vector<EpisodicMemoryItem*> getEpisodesForReplay(size_t count);
    void replayEpisode(const EpisodicMemoryItem* episode);
    void consolidate(float relevanceThreshold);
    size_t getEpisodeCount() const;
    
    // Capacity management
    void setWorkingMemoryCapacity(size_t capacity);
    void setMaxEpisodes(size_t maxEpisodes);
    
    // Memory queries
    MemorySystemStatus getStatus() const;
    bool isConsistent() const { return status_.isConsistent(); }
    std::string getStatusString() const { return status_.toString(); }
    
    // State management
    void clear();
    size_t getActiveTraces() const { return status_.workingMemoryTraces; }
    
private:
    struct Impl;
    Impl* pImpl;
    MemorySystemStatus status_;
};
