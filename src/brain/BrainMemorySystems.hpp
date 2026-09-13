#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class Brain;
class Neuron;

class BrainMemorySystems {
public:
    // Forward declaration for friendship
    friend class Brain;
    
    BrainMemorySystems() = default;
    ~BrainMemorySystems() = default;
    
    // Working memory operations
    void storeToNeuron(NeuronId neuronId, float activation);
    void update(float timestep);
    void setCapacity(size_t capacity);
    size_t getActiveTraces() const;
    void clear();
    
    // Episodic memory operations
    void storeEpisode(const EpisodicMemoryItem& episode);
    void getEpisodesForReplay(size_t count, std::vector<const EpisodicMemoryItem*>& episodes) const;
    void replayEpisode(const EpisodicMemoryItem* episode);
    void consolidate(float threshold);
    size_t getEpisodeCount() const;
    void setMaxEpisodes(size_t max);
    
    // Associative memory operations
    void storeAssociation(const std::string& key, const std::vector<float>& pattern);
    bool retrieveAssociation(const std::string& key, std::vector<float>& pattern) const;
    float calculateAssociationStrength(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const;
    
    // Memory statistics
    size_t getWorkingMemoryCapacity() const;
    float getMemoryUtilization() const;
    
    // Configuration
    void initialize(class Brain* brain);
    
private:
    // Working memory
    struct MemoryTrace {
        NeuronId neuronId;
        float activation;
        Timestamp timestamp;
        float decayRate;
    };
    
    std::vector<MemoryTrace> workingMemory;
    size_t workingMemoryCapacity;
    
    // Episodic memory
    std::vector<EpisodicMemoryItem> episodicMemory;
    size_t maxEpisodes;
    
    // Associative memory
    std::unordered_map<std::string, std::vector<float>> associativeMemory;
    
    // Reference to brain for system coordination
    class Brain* brainRef;
};

} // namespace nlm