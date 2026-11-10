#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Provides short-term storage for neural information and activation patterns
// Key properties:
// - Limited capacity (configurable via constructor)
// - Automatic decay to prevent indefinite storage
// - Fast access by neuron ID for efficient retrieval
// - Supports episodic memory encoding through neural patterns

class WorkingMemory {
public:
    /// Constructor - initializes with default capacity
    WorkingMemory();
    
    /// Destructor
    ~WorkingMemory();
    
    /// Store information in working memory
    /// @param neuron Identifier of the neuron associated with this information
    /// @param value Numeric value representing the stored information
    /// @note If neuron ID is invalid (INVALID_NEURON_ID), the operation is ignored
    void store(NeuronId neuron, float value);
    
    /// Retrieve information from working memory
    /// @param neuron Identifier of the neuron
    /// @return Stored value, or 0.0f if not found
    float retrieve(NeuronId neuron) const;
    
    /// Check if information is stored for given neuron
    /// @param neuron Identifier of the neuron
    /// @return True if information exists for this neuron
    bool contains(NeuronId neuron) const;
    
    /// Clear all stored information
    void clear();
    
    /// Get maximum capacity of working memory
    /// @return Maximum number of items that can be stored
    size_t getCapacity() const;
    
    /// Get current number of stored items
    /// @return Current memory usage
    size_t getCurrentSize() const;
    
    /// Decay all stored information by given factor
    /// @param decayRate Decay factor (0.0 to 1.0, where 1.0 means complete decay)
    /// @note Invalid decay rates are clamped to valid range
    void decay(float decayRate);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
