#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Real working memory with capacity limits and decay dynamics

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item with value (e.g., neuron activation level)
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
    
    // Decay all items over time
    void decay(float decayRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// Real episodic memory with consolidation and replay mechanisms

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    float reward;  // Associated reward value
    
    EpisodicMemoryItem() : timestamp(0), reward(0.0f) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes for replay and consolidation
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Get episodes suitable for replay (high relevance)
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation (move to long-term storage)
    void consolidate(float relevanceThreshold);
    
    // Replay selected episodes
    void replayEpisode(const EpisodicMemoryItem* episode);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge
// Placeholder for future implementation

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
    Impl* pImpl;
};

// Procedural memory: learned skills and habits
// Placeholder for future implementation

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
    
    // Learn skill from neural activity patterns
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
    Impl* pImpl;
};

// Associative memory: relationships between representations
// Real associative memory with Hebbian learning mechanisms

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association between neurons
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association strength
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
