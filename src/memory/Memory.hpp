#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Implements neural working memory with persistent activity and capacity limits

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Initialize with capacity
    void initialize(size_t capacity) { capacity_ = capacity; }
    
    // Store item with capacity management
    void store(NeuronId neuron, float value);
    
    // Retrieve item
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all
    void clear();
    
    // Get capacity
    size_t getCapacity() const { return capacity_; }
    size_t getCurrentSize() const { return items_.size(); }
    
    // Decay all items
    void decay(float decayRate);
    
    // Get all items for neural integration
    const std::vector<std::pair<NeuronId, float>>& getAllItems() const { return items_; }
    
private:
    std::vector<std::pair<NeuronId, float>> items_;
    size_t capacity_;
};

// Episodic memory: storage of events and experiences
// Implements neural episodic memory with capacity management and consolidation

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    float relevance;  // 0.0 to 1.0
    
    EpisodicMemoryItem()
        : timestamp(0), relevance(0.5f) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Initialize with capacity
    void initialize(size_t maxEpisodes) { maxEpisodes_ = maxEpisodes; }
    
    // Store episode with capacity management
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const { return episodes_.size(); }
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation (move to long-term)
    void consolidate(float relevanceThreshold);
    
    // Get episodes for replay
    std::vector<EpisodicMemoryItem> getEpisodesForReplay(size_t count) const;
    
    // Replay episode
    void replayEpisode(const EpisodicMemoryItem& episode);
    
private:
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
    Impl* pImpl;
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
    Impl* pImpl;
};

// Associative memory: relationships between representations
// PLACEHOLDER - Phase 2 will implement real associative memory

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
