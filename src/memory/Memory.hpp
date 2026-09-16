#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Implements working memory using neural mechanisms
// Based on neural evidence of persistent activity

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Store item using synaptic plasticity
    void store(NeuronId neuron, float value);
    
    // Retrieve item from active neural traces
    float retrieve(NeuronId neuron) const;
    
    // Check if neuron is active in working memory
    bool contains(NeuronId neuron) const;
    
    // Clear all working memory traces
    void clear();
    
    // Get capacity (number of items)
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Decay all items based on time
    void decay(float decayRate);
    
    // Get all active neurons
    const std::vector<NeuronId>& getActiveNeurons() const { return activeNeurons_; }
    
    // Get current activation level
    float getActivation(NeuronId neuron) const;
    
    // Update working memory (maintain and decay)
    void update(float dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<NeuronId> activeNeurons_;
    std::vector<float> activations_;
    std::vector<SimulationStep> lastUpdateTimes_;
    size_t capacity_;
    float decayRate_;
};

// Episodic memory: storage of events and experiences
// Implements episodic memory using neural pattern completion
// Similar to how hippocampus stores and retrieves episodic memories

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    
    // Sensory state at time of experience (encoded as neural pattern)
    std::vector<float> sensoryState;
    
    // Location/position information
    float positionX;
    float positionY;
    float orientation;
    
    // Action taken (if any)
    ActionType action;
    
    // Internal state (energy, reward, etc.)
    float reward;
    float energy;
    float novelty;
    
    // Consequence of action
    std::vector<float> resultingSensoryState;
    float resultingReward;
    
    // Associated neural activity pattern (who was firing)
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Time since this episode
    SimulationStep age;
    
    // Relevance score for consolidation
    float relevance;
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0), relevance(0.0f) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Store a new experience (encode as neural pattern)
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episodes similar to query pattern
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 5) const;
    
    // Retrieve episodes from time window
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const;
    
    // Get episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    
    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }
    
    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Clear all episodes
    void clear();
    
    // Consolidate important memories
    void consolidate(float relevanceThreshold);
    
    // Get average reward from episodes
    float getAverageReward() const;
    
    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
    // Get episodes for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    
    // Compute similarity between query and stored episode
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const;
};

// Semantic memory: gradually acquired knowledge
// Implements semantic memory using associative networks
// Represents facts and knowledge as distributed neural patterns

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a fact (associating concept with meaning)
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all facts
    void clear();
    
    // Get concept associations
    std::vector<std::string> getAssociatedConcepts(const std::string& concept) const;
    
    // Get strength of association
    float getAssociationStrength(const std::string& conceptA,
                                const std::string& conceptB) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<std::pair<std::string, std::string>> facts_;
    std::vector<std::pair<std::string, std::string>> associations_;
};

// Procedural memory: learned skills and habits
// Implements procedural memory using motor patterns
// Stores skills as sequences of neural activation patterns

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    
    Skill() : proficiency(0.0f) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn a new skill
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency based on performance
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all skills
    void clear();
    
    // Execute skill (activate neural pattern)
    void executeSkill(const std::string& name);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<Skill> skills_;
};

// Associative memory: relationships between representations
// Implements Hebbian associative memory
// Creates and retrieves connections between neural patterns

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association between two patterns
    void associate(const std::vector<float>& patternA,
                  const std::vector<float>& patternB,
                  float strength = 1.0f);
    
    // Retrieve patterns associated with query
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults = 5) const;
    
    // Get association strength
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;
    
    // Update association strength
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    
    // Clear all associations
    void clear();
    
    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> associations_;
    std::vector<float> associationStrengths_;
    
    // Compute pattern similarity
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
    
    // Find or create pattern neuron
    NeuronId findPatternNeuron(const std::vector<float>& pattern);
};

} // namespace nlm
