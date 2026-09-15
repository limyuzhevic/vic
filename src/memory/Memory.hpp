#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>
#include <memory>

namespace nlm {

// Working memory: temporary active storage of information with capacity limits and decay
// Real implementation with consolidation mechanisms

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item with capacity management
    void store(NeuronId neuron, float value);
    
    // Retrieve item
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all
    void clear();
    
    // Get capacity
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Decay all items
    void decay(float decayRate);
    
    // Consolidate weak items
    void consolidate(float threshold);
    
    // Update for new timestep
    void update(SimulationStep timestep);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Episodic memory: storage of events and experiences with replay capabilities
// Real implementation with pattern completion and relevance weighting

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<float> sensoryState;
    float positionX;
    float positionY;
    float orientation;
    ActionType action;
    float reward;
    float energy;
    float novelty;
    std::vector<float> resultingSensoryState;
    float resultingReward;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    SimulationStep age;
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait), reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store episode with encoding
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Retrieve similar episodes via pattern completion
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 5) const;
    
    // Retrieve episodes from time window
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const;
    
    // Retrieve episodes by location
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(
        float x, float y, float radius,
        size_t maxResults = 5) const;
    
    // Clear all
    void clear();
    
    // Consolidate based on relevance
    void consolidate(float relevanceThreshold);
    
    // Replay episode for reinforcement
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Update episode relevance
    void updateRelevance(size_t episodeId, float relevanceDelta);
    
    // Get episodes suitable for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Replay episode sequence
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Get average reward from episodes
    float getAverageReward() const;
    
    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
private:
    // Compute similarity between query and episode
    float computeSimilarity(
        const std::vector<float>& query,
        const EpisodicMemoryItem& episode) const;
    
    // Check if episode matches criteria
    bool matchesCriteria(
        const EpisodicMemoryItem& episode,
        const std::vector<float>* sensoryQuery,
        SimulationStep* startTime,
        SimulationStep* endTime,
        ActionType* actionQuery) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool replayEnabled_;
};

// Semantic memory: gradually acquired knowledge with distributed representations
// Real implementation with concept formation and pattern completion

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store fact with distributed representation
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    void clear();
    
    // Store concept (distributed representation)
    void storeConcept(const std::vector<float>& pattern, const std::string& label);
    
    // Retrieve concept by pattern
    std::string retrieveConcept(const std::vector<float>& pattern) const;
    
    // Get concept similarity
    float getConceptSimilarity(const std::vector<float>& patternA,
                               const std::vector<float>& patternB) const;
    
    // Update concept based on feedback
    void updateConcept(const std::vector<float>& pattern, float feedback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural memory: learned skills and habits with chaining and automaticity
// Real implementation with skill execution and reinforcement learning

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    std::vector<std::string> prerequisites;
    std::vector<std::string> outcomes;
    float rewardValue;
    bool isChained;
    
    Skill() : proficiency(0.0f), rewardValue(0.0f), isChained(false) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn skill with reinforcement
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    const Skill* getSkill(const std::string& name) const;
    
    // Get all skills
    std::vector<const Skill*> getAllSkills() const;
    
    // Update proficiency based on outcome
    void updateProficiency(const std::string& name, float delta, bool success);
    
    // Execute skill if available
    bool executeSkill(const std::string& name, const std::vector<float>& context);
    
    // Clear all
    void clear();
    
    // Consolidate skills into automaticity
    void consolidate();
    
    // Get skills that can be executed given context
    std::vector<const Skill*> getAvailableSkills(const std::vector<float>& context) const;
    
    // Set context for skills
    void setSkillContext(const std::string& skillName, const std::vector<NeuronId>& context);
    
    // Clear skill context
    void clearSkillContext(const std::string& skillName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative memory: relationships between representations with spreading activation
// Real implementation with Hebbian plasticity and dynamic weights

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association with Hebbian learning
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association with weight decay
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
    // Spread activation from cue
    std::vector<NeuronId> spreadActivation(NeuronId startNeuron, size_t steps) const;
    
    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Simple associative implementation (fallback)
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

// NeuralAssociativeMemory: Distributed representations with neural plasticity
// Real implementation with pattern completion and reinforcement learning

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Associate patterns with neural plasticity
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    
    // Retrieve patterns associated with query
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;
    
    // Get association strength between patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;
    
    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    
    // Spread activation through association chains
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;
    
    // Clear associations
    void clear();
    
    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Simple pattern storage
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    // Helper functions
    NeuronId findPatternNeuron(const std::vector<float>& pattern);
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;
    Brain* brain_;
};

} // namespace nlm
