#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations for integration
class Brain;
class ConceptFormation;
class NeuralPlanner;
class NeuralPrediction;
class Neuromodulator;

// Working memory: temporary active storage of information
// REALIZED with neural working memory (persistent activity and recurrent connections)

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item
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
}
;

// Episodic memory: storage of events and experiences
// REALIZED with neural episodic memory (neural patterns and pattern completion)

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
    ~EpisodicMemory();
    
    // Store episode
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation (move to long-term)
    void consolidate(float relevanceThreshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge - NEURAL IMPLEMENTATION
// Uses neural patterns and Hebbian clustering for concept formation

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store fact as neural pattern
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact from neural pattern
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    void clear();
    
    // Neural pattern operations
    void storePattern(const std::vector<float>& pattern,
                      const std::string& key,
                      const std::string& value);
    
    size_t findMatchingConcept(const std::vector<float>& pattern) const;
    float computePatternSimilarity(const std::vector<float>& a,
                                  const std::vector<float>& b) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Procedural memory: learned skills and habits - NEURAL IMPLEMENTATION
// Uses sequential neural patterns and forward models for skill execution

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    std::vector<float> sequencePattern;
    
    Skill() : proficiency(0.0f) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn skill as neural routine
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
    // Neural skill operations
    void createSkill(size_t skillIndex,
                     const std::vector<float>& sequence,
                     const std::vector<NeuronId>& neurons);
    void createSkillConnections(size_t skillIndex,
                                const std::vector<NeuronId>& neurons);
    void initializeForwardModel(size_t skillIndex,
                                const std::vector<float>& sequence);
    size_t getSkillIndex(const std::string& name) const;
    void registerSkillWithPlanner(const std::string& skillName,
                                 size_t skillIndex);
    
private:
    struct Impl;
    Impl* pImpl;
    std::vector<Skill> skills;  // Direct storage for skill instances
};

// Associative memory: relationships between representations
// REALIZED with neural associations encoded in synaptic connectivity

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

// Include neural implementations
#include "SemanticMemory.cpp"
#include "ProceduralMemory.cpp"
