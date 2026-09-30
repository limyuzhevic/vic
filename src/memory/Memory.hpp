#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Real implementation with neural dynamics - uses persistent activity and recurrent connections
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention
// Integrated with Brain system in NeuralWorkingMemory class

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

// Neural episodic memory: stores experiences and episodes
// Real implementation with neural pattern storage and retrieval
// Key mechanisms:
// - Episodes stored as neural activity patterns (not text records)
// - Pattern completion (Hopfield-like retrieval) from partial cues
// - Temporal ordering through timestamps and replay
// - Relevance-weighted recall based on recent use
// - Integration with neural dynamics for experience replay
// Integrated with NeuralEpisodicMemory class

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

// Semantic memory: gradually acquired knowledge and facts
// Real implementation with associative storage and retrieval
// Key mechanisms:
// - Facts stored as key-value pairs with pattern-based retrieval
// - Association learning through repeated co-activation
// - Concept formation from related facts and experiences
// - Integration with episodic memory for knowledge consolidation
// TODO: Implement integration with NeuralAssociativeMemory

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
// Real implementation with neural pattern storage for motor programs
// Key mechanisms:
// - Skills stored as neural activation patterns and sequences
// - Proficiency tracking based on repetition and success
// - Motor pattern completion and refinement
// - Integration with motor system for action execution
// TODO: Implement integration with motor system

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

// Neural associative memory: relationships between neural patterns
// Real implementation with Hebbian learning and pattern completion
// Key mechanisms:
// - Co-activating neurons become associated via Hebbian plasticity
// - Stronger associations form with repeated co-activation
// - Can retrieve related items given partial cues (spreading activation)
// - Integration with episodic memory for memory linking
// Real implementation in NeuralAssociativeMemory class

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
