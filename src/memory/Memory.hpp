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
    ~WorkingMemory();
    
    // Store item
    // TODO PHASE 2: Implement real storage
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// Base implementation on NeuralEpisodicMemory for neural integration

class EpisodicMemory : public NeuralEpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory() override;
    
    // Store episode using neural pattern storage
    void storeEpisode(const EpisodicMemoryItem& episode) override;
    
    // Retrieve episode by index
    EpisodicMemoryItem retrieveEpisode(size_t index) const override;
    
    // Get episode count
    size_t getEpisodeCount() const override;
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const override;
    
    // Clear all episodes
    void clear() override;
    
    // Memory consolidation (move to long-term)
    void consolidate(float relevanceThreshold) override;
    
private:
    // Convert between simple and neural episodic memory items
    void convertToNeuralItem(const EpisodicMemoryItem& simpleItem, 
                           EpisodicMemoryItem& neuralItem) const;
    void convertFromNeuralItem(const EpisodicMemoryItem& neuralItem,
                             EpisodicMemoryItem& simpleItem) const;
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
// Base implementation on NeuralAssociativeMemory for neural integration

class AssociativeMemory : public NeuralAssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory() override;
    
    // Create association using neural pattern matching
    void associate(NeuronId a, NeuronId b, float strength) override;
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const override;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const override;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta) override;
    
    // Clear all associations
    void clear() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
