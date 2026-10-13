#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Main memory system for Phase 6: Integrated Artificial Brain
// This file now contains only the Neural memory system implementations
// that are actively used in the production Phase 6 system

// Neural Working Memory: temporarily active information storage
// Real implementation for Phase 6 (previously placeholder WorkingMemory)
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Store pattern in working memory
    void store(const std::vector<float>& pattern, float strength);
    
    // Store directly in neuron
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current memory pattern
    std::vector<float> retrieve() const;
    
    // Check if neuron is in memory
    bool contains(NeuronId neuron) const;
    
    // Get neuron activation level
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update memory dynamics (called each step)
    void update(TimestepDuration dt);
    
    // Clear all memory traces
    void clear();
    
    // Get memory statistics
    float getMemoryActivity() const;
    bool isWinning(NeuronId neuron) const;
    size_t getActiveTraces() const { return memoryNeurons_.size(); }
    
    // Set capacity and decay rate
    void setCapacity(size_t capacity) { capacity_ = capacity; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    // Get stored neurons and activations
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Internal memory storage
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    std::vector<NeuronId> winners_;
    
    size_t capacity_;
    float decayRate_;
    class Brain* brain_;
};

// Neural Episodic Memory: long-term storage of experiences
// Real implementation for Phase 6 (previously placeholder EpisodicMemory)
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> neuronActivations;
    float reward;
    float relevance;
    ActionType action;
    std::string metadata;
    std::vector<float> sensoryState;
    std::vector<float> resultingSensoryState;
    
    EpisodicMemoryItem() : timestamp(0), reward(0.0f), relevance(0.0f), action(ActionType::Wait) {}
};

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Store episode of experience
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve similar episodes to query pattern
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 10) const;
    
    // Retrieve episodes within time range
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const;
    
    // Retrieve episodes by spatial proximity
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(
        float x, float y, float radius,
        size_t maxResults = 10) const;
    
    // Retrieve episodes with specific action type
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(
        ActionType action,
        size_t maxResults = 10) const;
    
    // Replay episode to reactivate neural patterns
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Update relevance of specific episode
    void updateRelevance(SimulationStep episodeId, float relevanceDelta);
    
    // Get specific episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    
    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Get episodes for replay based on importance
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Replay sequence of episodes
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Clear all episodes
    void clear();
    
    // Get memory statistics
    size_t getEpisodeCount() const { return episodes_.size(); }
    float getAverageReward() const;
    
    // Consolidate memory (move to long-term storage)
    void consolidate(float relevanceThreshold);
    
    // Set maximum episodes
    void setMaxEpisodes(size_t max) { maxEpisodes_ = max; }
    
    // Check if replay is enabled
    void setReplayEnabled(bool enabled) { replayEnabled_ = enabled; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Internal storage
    std::vector<EpisodicMemoryItem> episodes_;
    std::vector<NeuronId> episodeNeurons_;
    
    size_t maxEpisodes_;
    bool replayEnabled_;
    class Brain* brain_;
    
    // Helper methods
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const;
    bool matchesCriteria(const EpisodicMemoryItem& episode,
                        const std::vector<float>* sensoryQuery,
                        SimulationStep* startTime,
                        SimulationStep* endTime,
                        ActionType* actionQuery) const;
};

// Neural Associative Memory: relationships between patterns
// Real implementation for Phase 6 (previously placeholder AssociativeMemory)
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Create association between two patterns
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 0.8f);
    
    // Associate from episodic experience
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    // Retrieve patterns associated with query
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults = 10) const;
    
    // Get association strength between patterns
    float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const;
    
    // Update association strength
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    
    // Spread activation through association network
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps = 3) const;
    
    // Clear all associations
    void clear();
    
    // Set pattern neuron capacity
    size_t getPatternCount() const { return patternNeurons_.size(); }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Internal storage
    std::vector<std::tuple<NeuronId, std::vector<float>, float>> associations_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    
    class Brain* brain_;
    
    // Helper methods
    NeuronId findPatternNeuron(const std::vector<float>& pattern) const;
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
};

// Additional memory systems for Phase 6 integration
// Note: SemanticMemory and ProceduralMemory implementations 
// are available in their respective header files but not directly 
// used in the core Phase 6 brain architecture

// Semantic Memory (for conceptual knowledge)
class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    void storeFact(const std::string& key, const std::string& value);
    std::string retrieveFact(const std::string& key) const;
    bool hasFact(const std::string& key) const;
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Procedural Memory (for learned skills)  
class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    Skill* getSkill(const std::string& name);
    void updateProficiency(const std::string& name, float delta);
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
