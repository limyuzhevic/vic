#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// Working memory: temporary active storage of information
// This implementation uses a simple vector storage with capacity management
// and temporal decay characteristics

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item in working memory
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
    
    // Decay all items based on time
    void decay(float decayRate);
    
    // Apply neuromodulation effects
    void applyNeuromodulation(float level);
    
    // Get winners from competition
    const std::vector<NeuronId>& getWinners() const;
    
    // Get inhibition for neuron
    float getInhibitionFor(NeuronId neuron) const;
    
    // Rehearse (reinforce) memory traces
    void rehearse();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Simple episodic memory implementation
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
    
    // Consolidate memory (remove weak episodes)
    void consolidate(float relevanceThreshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Semantic memory: gradually acquired knowledge
// Gradual knowledge acquisition through experience and rehearsal

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact with confidence
    void storeFact(const std::string& key, const std::string& value, float confidence = 0.5f);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts with confidence
    std::vector<std::tuple<std::string, std::string, float>> getAllFacts() const;
    
    // Get fact strength (0-1)
    float getFactStrength(const std::string& key) const;
    
    // Decay less used facts based on time
    void decayUnusedFacts(float decayRate, float timeSinceAccess);
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural memory: learned skills and habits
// Stored in neural form, retrieved as behavioral sequences

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    float lastUsed;
    size_t successCount;
    
    Skill() : proficiency(0.0f), lastUsed(0), successCount(0) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn skill with neural pattern and initial proficiency
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern, float initialProficiency = 0.1f);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency based on experience
    void updateProficiency(const std::string& name, float delta, bool success = false);
    
    // Get skill effectiveness (proficiency * successRate)
    float getSkillEffectiveness(const std::string& name) const;
    
    // Select best skill for given neural state
    std::string selectBestSkill(const std::vector<float>& neuralPattern) const;
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative memory: relationships between representations
// Dynamic pattern completion through sparse distributed representations

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association with Hebbian-like strengthening
    void associate(NeuronId a, NeuronId b, float strength = 1.0f);
    
    // Get associated neurons with pattern completion
    std::vector<NeuronId> getAssociations(NeuronId neuron, float minStrength = 0.1f) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association with delta rule
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Retrieve pattern - given partial pattern, complete it
    std::vector<NeuronId> retrievePattern(const std::vector<NeuronId>& partialPattern) const;
    
    // Get all associations for pattern completion
    std::vector<std::tuple<NeuronId, NeuronId, float>> getAllAssociations() const;
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
