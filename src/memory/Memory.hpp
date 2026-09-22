#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// Forward declarations for all memory types
class Brain;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class SemanticMemory;
class ProceduralMemory;
class Skill;

// Working memory: temporary active storage of information
// Phase 6: Simplified implementation for basic functionality

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store item - simplified version
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
    
    // Get all items
    const std::unordered_map<NeuronId, float>& getItems() const { return items_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Simplified storage
    std::unordered_map<NeuronId, float> items_;
    std::unordered_map<NeuronId, float> decayRates_;
    size_t capacity_;
    Brain* brain_;
};


// Episodic memory: storage of events and experiences
// Phase 6: Simplified implementation for basic experience logging

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store episode - simplified version
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
    
    // Get average reward
    float getAverageReward() const;
    
private:
    // Simplified storage
    std::vector<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    Brain* brain_;
};


// Semantic memory: gradually acquired knowledge
// Phase 6: Real implementation with concept learning

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact (concept) from neural pattern
    void storeFact(const std::string& key, const std::string& value);
    void storeFromNeurons(const std::vector<NeuronId>& neurons, const std::string& concept);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Get concept from pattern
    std::string getConcept(const std::vector<float>& pattern) const;
    
    // Clear all
    void clear();
    
    // Get number of concepts
    size_t getConceptCount() const { return concepts_.size(); }
    
    // Update concept strength
    void updateConceptStrength(const std::string& concept, float delta);
    
    // Get concept stability (how stable the concept is)
    float getConceptStability(const std::string& concept) const;
    
private:
    struct Impl;
    Impl* pImpl;
};


// Procedural memory: learned skills and habits
// Phase 6: Simplified implementation for skill learning

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn skill - simplified version
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
    // Get skill count
    size_t getSkillCount() const { return skills_.size(); }
    
private:
    // Simplified storage
    std::vector<Skill> skills_;
};


// Associative memory: relationships between representations
// Phase 6: Simplified implementation for basic associations

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association - simplified version
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
    // Get association count
    size_t getAssociationCount() const { return associations_.size(); }
    
private:
    // Simplified storage
    std::vector<std::pair<NeuronId, NeuronId>> associations_;
    std::vector<float> associationStrengths_;
};

} // namespace nlm