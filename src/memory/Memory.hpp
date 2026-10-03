#ifndef NLM_MEMORY_HPP
#define NLM_MEMORY_HPP

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <algorithm>

namespace nlm {

// Forward declarations
class Brain;

// Neuron ID type
using NeuronId = uint64_t;

// Memory item
struct MemoryItem {
    NeuronId neuronId;
    float value;
    float timestamp;
};

// Episodic memory item
struct EpisodicMemoryItem {
    uint64_t timestamp;
    float reward;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
};

// Semantic memory fact
struct SemanticFact {
    std::string key;
    std::string value;
    float confidence;
};

// Procedural memory skill
struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
};

// Forward declarations for pImpl pattern
struct WorkingMemory::Impl;
struct EpisodicMemory::Impl;
struct SemanticMemory::Impl;
struct ProceduralMemory::Impl;
struct AssociativeMemory::Impl;

// Working memory: temporary active storage of information
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
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    float reward;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
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
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation (move to long-term)
    void consolidate(float relevanceThreshold);
    
    // Get episodes for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Replay episode
    void replayEpisode(const EpisodicMemoryItem& episode);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Semantic memory: gradually acquired knowledge
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
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_MEMORY_HPP
