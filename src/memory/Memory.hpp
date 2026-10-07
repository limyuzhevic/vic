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
    
    // Get episodes with relevance scores (for replay)
    std::vector<std::pair<EpisodicMemoryItem, float>> getRelevantEpisodes(
        size_t maxCount = 10, float minRelevance = 0.5f) const;
    
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
    float stability;
    Timestamp lastPracticed;
    Timestamp lastImproved;
    std::vector<float> performanceHistory;
    
    Skill() : proficiency(0.0f), stability(1.0f), lastPracticed(0.0f), lastImproved(0.0f) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn skill from neural pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill by name
    Skill* getSkill(const std::string& name);
    const Skill* getSkill(const std::string& name) const;
    
    // Get all skills (with proficiency thresholds)
    std::vector<Skill*> getAllSkills();
    std::vector<const Skill*> getAllSkills() const;
    
    // Filter skills by proficiency
    std::vector<Skill*> getSkillsAboveProficiency(float threshold);
    std::vector<const Skill*> getSkillsAboveProficiency(float threshold) const;
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Practice skill to improve proficiency
    void practiceSkill(const std::string& name, float performance);
    
    // Get average proficiency
    float getAverageProficiency() const;
    
    // Get best skills
    std::vector<Skill*> getBestSkills(size_t count = 5);
    std::vector<const Skill*> getBestSkills(size_t count = 5) const;
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// PLACEHOLDER - Phase 2 will implement real associative memory

struct Association {
    std::vector<NeuronId> patternA;
    std::vector<NeuronId> patternB;
    float strength;
    float stability;
    float frequency;
    Timestamp lastActivated;
    
    Association(const std::vector<NeuronId>& a, const std::vector<NeuronId>& b) 
        : patternA(a), patternB(b), strength(0.0f), stability(1.0f), frequency(0), lastActivated(0.0f) {}
};

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association between two neural patterns
    void associate(const std::vector<NeuronId>& patternA, const std::vector<NeuronId>& patternB, float strength = 0.5f);
    
    // Get associations for a pattern (returns patternB)
    std::vector<NeuronId> getAssociations(const std::vector<NeuronId>& pattern) const;
    
    // Get associations for a single neuron
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(const std::vector<NeuronId>& patternA, const std::vector<NeuronId>& patternB) const;
    
    // Update association strength
    void updateAssociation(const std::vector<NeuronId>& patternA, const std::vector<NeuronId>& patternB, float delta);
    
    // Find associations for pattern (auto-match)
    bool findAssociation(const std::vector<NeuronId>& pattern, std::vector<NeuronId>& matchedPattern) const;
    
    // Get all associations
    std::vector<Association> getAllAssociations() const;
    
    // Update association based on activation (Hebbian-like)
    void updateFromActivation(const std::vector<NeuronId>& patternA, const std::vector<NeuronId>& patternB);
    
    // Clear all associations
    void clear();
    
    // Get average association strength
    float getAverageStrength() const;
    
    // Find strongest associations
    std::vector<Association> getStrongestAssociations(size_t count = 10) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
