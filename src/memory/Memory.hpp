#pragma once

#include "../core/Types/Types.hpp"
#include "../motor/Action.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace nlm {

// Working memory: temporary active storage of information
// Now using neural implementation with capacity limits, neural activation patterns,
// competition among memory traces, and decay mechanisms

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item (delegate to neural working memory)
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
    
    // Neural-specific methods
    void update(TimestepDuration dt);
    size_t getActiveTraces() const;
    std::vector<float> getAllActivations() const;
    void applyHebbianLearning(const std::vector<float>& pattern, float learningRate);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Episodic memory: storage of events and experiences
// Now using neural implementation with real consolidation, temporal binding,
// replay mechanisms, and memory strength tracking

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
    
    EpisodicMemoryItem() : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait), reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode (delegate to neural episodic memory)
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Get recent episodes (neural implementation)
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Retrieve episodes similar to query pattern
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern, size_t maxResults = 5) const;
    
    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }
    
    // Clear all
    void clear();
    
    // Memory consolidation (move to long-term)
    void consolidate(float relevanceThreshold);
    
    // Replay during rest
    void replayDuringRest(float strength);
    
    // Apply prediction error to modulate memory relevance
    void applyPredictionError(float errorSignal);
    
    // Update all episodes with current simulation time
    void updateAllEpisodes(SimulationStep currentStep);

private:
    // Legacy compatibility - store episodes for retrieval
    std::vector<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
};

// Semantic memory: gradually acquired knowledge
// PLACEHOLDER - Phase 2 will implement real semantic memory

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural memory: learned skills and habits
// PLACEHOLDER - Phase 2 will implement real procedural memory

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
    
    // Learn skill
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative memory: relationships between representations
// Now using neural implementation with pattern association learning,
// temporal relationship storage, pattern completion, and Hebbian integration

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association (delegate to neural associative memory)
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
    // Neural-specific methods
    void storeAssociation(const std::vector<float>& pattern1,
                         const std::vector<float>& pattern2,
                         float strength = 1.0f);
    std::vector<float> getAssociation(const std::vector<float>& pattern) const;
    float getAssociationStrength(const std::vector<float>& pattern1,
                                 const std::vector<float>& pattern2) const;
    std::vector<float> retrieve(const std::vector<float>& input,
                               float threshold = 0.5f) const;
    void applyHebbianLearning(const std::vector<float>& pattern, float learningRate);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
