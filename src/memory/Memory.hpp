#ifndef NLM_MEMORY_HPP
#define NLM_MEMORY_HPP

#include "../core/Types/Types.hpp"

namespace nlm {

// Memory system interface
class Memory {
public:
    virtual ~Memory() = default;
    
    // Clear all memory
    virtual void clear() = 0;
    
    // Get current usage/statistics
    virtual float getUsage() const = 0;
    virtual size_t getCapacity() const = 0;
};

// Working memory interface - temporary activation traces
class WorkingMemory : public Memory {
public:
    virtual ~WorkingMemory() = default;
    
    // Store a neuron with activation strength
    virtual void storeToNeuron(NeuronId neuron, float activation) = 0;
    
    // Get active traces
    virtual size_t getActiveTraces() const = 0;
    virtual const std::vector<NeuronId>& getMemoryNeurons() const = 0;
    virtual const std::vector<float>& getActivationValues() const = 0;
    
    // Update working memory (competition, decay)
    virtual void update(TimestepDuration dt) = 0;
    
    // Set capacity
    virtual void setCapacity(size_t capacity) = 0;
};

// Episodic memory interface - experience storage
class EpisodicMemory : public Memory {
public:
    virtual ~EpisodicMemory() = default;
    
    // Store an episode
    struct EpisodicMemoryItem {
        Timestamp timestamp;
        std::vector<NeuronId> activeNeurons;
        std::vector<float> neuronActivations;
        float reward;
        float novelty;
    };
    
    virtual void storeEpisode(const EpisodicMemoryItem& episode) = 0;
    
    // Get episodes
    virtual size_t getEpisodeCount() const = 0;
    virtual const EpisodicMemoryItem* getEpisode(size_t index) const = 0;
    
    // Replay episodes for consolidation
    virtual std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t maxCount) const = 0;
    
    // Set maximum episodes
    virtual void setMaxEpisodes(size_t max) = 0;
};

// Associative memory interface - pattern associations
class AssociativeMemory : public Memory {
public:
    virtual ~AssociativeMemory() = default;
    
    // Learn associations
    virtual void learnAssociation(const std::vector<float>& pattern1,
                                 const std::vector<float>& pattern2,
                                 float strength) = 0;
    
    // Retrieve associations
    virtual std::vector<std::pair<std::vector<float>, float>> 
        retrieveAssociations(const std::vector<float>& pattern,
                            size_t maxResults) const = 0;
    
    // Get association statistics
    virtual size_t getPatternCount() const = 0;
    virtual float getAverageStrength() const = 0;
};

} // namespace nlm

#endif // NLM_MEMORY_HPP