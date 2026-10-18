#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Phase 6 Implementation: NeuralWorkingMemory provides temporary active storage
// of neural activation patterns with competitive dynamics, maintenance, and decay

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item: neural activation becomes a memory trace
    void store(NeuronId neuron, float value);
    
    // Retrieve item: get stored activation for a neuron
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists in working memory
    bool contains(NeuronId neuron) const;
    
    // Clear all memory traces
    void clear();
    
    // Get memory capacity (maximum number of traces)
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Decay all items over time
    void decay(float decayRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// Phase 6 Implementation: NeuralEpisodicMemory stores experiences as episodes
// with sensory states, actions, rewards, and temporal information

struct EpisodicMemoryItem {
    SimulationStep timestamp;      // When the episode occurred
    std::vector<NeuronId> activeNeurons;  // Which neurons were active
    std::vector<float> neuronActivations; // Their activation levels
    std::vector<float> sensoryState;     // Sensory input pattern
    ActionType action;               // Action taken
    float reward;                    // Reward received
    float age;                       // How old the episode is (steps)
    float relevance;                 // How relevant the episode is for learning
    std::string metadata;            // Additional metadata
    
    EpisodicMemoryItem() : timestamp(0), action(ActionType::Wait), 
                          reward(0.0f), age(0.0f), relevance(0.0f) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode: experience becomes permanent memory
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode by index
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all episodes
    void clear();
    
    // Memory consolidation: remove irrelevant episodes
    void consolidate(float relevanceThreshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge and facts
// Phase 6 Implementation: SemanticMemory stores learned facts and relationships

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact: add a new learned fact
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact: get stored value by key
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all facts
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Procedural memory: learned skills and habits
// Phase 6 Implementation: ProceduralMemory stores skills and their neural patterns

struct Skill {
    std::string name;                 // Skill name
    std::vector<NeuronId> neuralPattern; // Neural pattern that implements the skill
    float proficiency;               // How well the skill is executed
    float lastUsed;                   // When the skill was last used
    
    Skill() : proficiency(0.0f), lastUsed(0.0f) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn skill: add a new skill with its neural pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill by name
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency: improve or degrade a skill
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all skills
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// Phase 6 Implementation: NeuralAssociativeMemory stores and retrieves associations

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association: link two patterns together with strength
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons: find patterns similar to query
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength between two patterns
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association: modify association strength
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all associations
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};
    
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
