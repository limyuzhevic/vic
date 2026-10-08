#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Uses neural dynamics for persistent activity and competition

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item using neural activation pattern
    void store(NeuronId neuron, float value);
    
    // Retrieve item value
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all stored items
    void clear();
    
    // Get memory capacity
    size_t getCapacity() const;
    
    // Get current number of stored items
    size_t getCurrentSize() const;
    
    // Apply decay to all items over time
    void decay(float decayRate);
    
    // Get current active traces
    std::vector<NeuronId> getActiveNeurons() const;
    
    // Get activation strength for a specific neuron
    float getActivation(NeuronId neuron) const;
    
    // Update memory (maintain traces, apply competition)
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// Uses neural replay for consolidation and replay

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
    
    // Store episode with neural activation patterns
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode by index
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get total number of stored episodes
    size_t getEpisodeCount() const;
    
    // Get recent episodes (most recent first)
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all stored episodes
    void clear();
    
    // Consolidate important memories to long-term storage
    void consolidate(float relevanceThreshold);
    
    // Replay episodes for memory strengthening
    void replayEpisodes(size_t count);
    
    // Retrieve episode by exact timestamp
    EpisodicMemoryItem retrieveEpisodeByTimestamp(SimulationStep timestamp) const;
    
    // Check if episode is consolidated
    bool isConsolidated(size_t episodeIndex) const;
    
    // Get count of consolidated episodes
    size_t getConsolidatedCount() const;
    
    // Update episodic memory with current experience
    void update(const std::vector<NeuronId>& activeNeurons, const std::vector<float>& activations, SimulationStep timestamp);
    
    // Get episode by similarity matching
    std::vector<EpisodicMemoryItem> retrieveSimilarEpisodes(const std::vector<NeuronId>& pattern, float similarityThreshold) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge and facts
// Connects concepts through neural associations

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store a fact with key-value representation
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve a fact by key
    std::string retrieveFact(const std::string& key) const;
    
    // Check if a fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all stored facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all facts
    void clear();
    
    // Update semantic network based on neural activity
    void update(const std::vector<std::pair<std::string, float>>& conceptActivations);
    
    // Get concepts related to a given concept
    std::vector<std::string> getRelatedConcepts(const std::string& concept) const;
    
    // Get concept strength
    float getConceptStrength(const std::string& concept) const;
    
    // Learn new concept associations
    void learnAssociation(const std::string& conceptA, const std::string& conceptB, float strength);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Procedural memory: learned skills and habits
// Encodes motor programs and behavioral patterns as neural sequences

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    float lastExecuted;
    size_t executionCount;
    
    Skill() : proficiency(0.0f), lastExecuted(0), executionCount(0) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn a new skill from neural activation pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve a skill by name
    const Skill* getSkill(const std::string& name) const;
    
    // Get all learned skills
    std::vector<Skill*> getAllSkills();
    
    // Update skill proficiency based on execution
    void updateProficiency(const std::string& name, float delta, float reward = 1.0f);
    
    // Execute a skill (neural pattern activation)
    bool executeSkill(const std::string& name, const std::vector<NeuronId>& targetNeurons);
    
    // Clear all skills
    void clear();
    
    // Get most frequently used skills
    std::vector<std::string> getMostFrequentSkills(size_t count) const;
    
    // Get skills by proficiency level
    std::vector<Skill*> getSkillsAboveProficiency(float threshold) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// Creates pattern completions and predicts related concepts

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create or strengthen association between two neurons
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get all neurons associated with a given neuron
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get strength of association between two neurons
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association strength
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all associations
    void clear();
    
    // Get all associations involving a specific neuron
    std::vector<std::pair<NeuronId, NeuronId>> getAllAssociationsForNeuron(NeuronId neuron) const;
    
    // Find completion for partial pattern (predict related neurons)
    std::vector<NeuronId> predictCompletions(const std::vector<NeuronId>& pattern, float threshold) const;
    
    // Get association statistics
    size_t getAssociationCount() const;
    size_t getNeuronAssociationCount(NeuronId neuron) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
