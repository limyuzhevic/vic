#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// PLACEHOLDER - Phase 2 will implement real working memory

class WorkingMemory {
public:
    WorkingMemory();
    virtual ~WorkingMemory();
    
    // Store item
    virtual void store(NeuronId neuron, float value) = 0;
    
    // Retrieve item
    virtual float retrieve(NeuronId neuron) const = 0;
    
    // Check if item exists
    virtual bool contains(NeuronId neuron) const = 0;
    
    // Clear all
    virtual void clear() = 0;
    
    // Get capacity
    virtual size_t getCapacity() const = 0;
    virtual size_t getCurrentSize() const = 0;
    
    // Decay all items
    virtual void decay(float decayRate) = 0;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// PLACEHOLDER - Phase 2 will implement real episodic memory

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    
    EpisodicMemoryItem() : timestamp(0) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    virtual ~EpisodicMemory();
    
    // Store episode
    virtual void storeEpisode(const EpisodicMemoryItem& episode) = 0;
    
    // Retrieve episode
    virtual EpisodicMemoryItem retrieveEpisode(size_t index) const = 0;
    
    // Get episode count
    virtual size_t getEpisodeCount() const = 0;
    
    // Get recent episodes
    virtual std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const = 0;
    
    // Clear all
    virtual void clear() = 0;
    
    // Memory consolidation (move to long-term)
    virtual void consolidate(float relevanceThreshold) = 0;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge
// PLACEHOLDER - Phase 2 will implement real semantic memory

class SemanticMemory {
public:
    SemanticMemory();
    virtual ~SemanticMemory();
    
    // Store fact
    virtual void storeFact(const std::string& key, const std::string& value) = 0;
    
    // Retrieve fact
    virtual std::string retrieveFact(const std::string& key) const = 0;
    
    // Check if fact exists
    virtual bool hasFact(const std::string& key) const = 0;
    
    // Get all facts
    virtual std::vector<std::pair<std::string, std::string>> getAllFacts() const = 0;
    
    // Clear all
    virtual void clear() = 0;
    
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
    virtual ~ProceduralMemory();
    
    // Learn skill
    virtual void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) = 0;
    
    // Retrieve skill
    virtual Skill* getSkill(const std::string& name) = 0;
    
    // Get all skills
    virtual std::vector<Skill*> getAllSkills() = 0;
    
    // Update proficiency
    virtual void updateProficiency(const std::string& name, float delta) = 0;
    
    // Clear all
    virtual void clear() = 0;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// PLACEHOLDER - Phase 2 will implement real associative memory

class AssociativeMemory {
public:
    AssociativeMemory();
    virtual ~AssociativeMemory();
    
    // Create association
    virtual void associate(NeuronId a, NeuronId b, float strength) = 0;
    
    // Get associated neurons
    virtual std::vector<NeuronId> getAssociations(NeuronId neuron) const = 0;
    
    // Get association strength
    virtual float getAssociationStrength(NeuronId a, NeuronId b) const = 0;
    
    // Update association
    virtual void updateAssociation(NeuronId a, NeuronId b, float delta) = 0;
    
    // Clear all
    virtual void clear() = 0;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
