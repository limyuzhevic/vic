#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Neural implementation using persistent activity and synaptic reinforcement

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Store item with neural encoding
    void store(NeuronId neuron, float value);
    
    // Retrieve stored value for specific neuron
    float retrieve(NeuronId neuron) const;
    
    // Check if neuron is currently stored in working memory
    bool contains(NeuronId neuron) const;
    
    // Clear all working memory items
    void clear();
    
    // Capacity management
    size_t getCapacity() const;
    void setCapacity(size_t capacity);
    size_t getCurrentSize() const;
    
    // Decay management
    void decay(float decayRate);
    void setGlobalDecayRate(float rate);
    float getDecayRateForNeuron(NeuronId neuron) const;
    void updateNeuronDecay(NeuronId neuron);
    
    // Memory strengthening/weakening (plasticity)
    void strengthen(NeuronId neuron, float factor);
    void weaken(NeuronId neuron, float factor);
    
    // Advanced queries and analysis
    std::vector<std::pair<NeuronId, float>> getAllItems() const;
    bool needsConsolidation(float threshold) const;
    float getItemAge(NeuronId neuron) const;
    
    // Competition and selection
    NeuronId getWinner() const;
    std::vector<NeuronId> getCompetitors() const;
    
    // State management
    bool isActive() const { return pImpl ? pImpl->isActive_ : false; }
    void activate() { if (pImpl) pImpl->isActive_ = true; }
    void deactivate() { if (pImpl) pImpl->isActive_ = false; }
    
    // Integration with neural networks
    void connectToBrain(Brain* brain) { if (pImpl) pImpl->brain_ = brain; }
    
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
// Neural implementation of semantic representation and inference

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Initialize with brain reference for learning integration
    void initialize(Brain* brain);
    
    // Store fact with learning mechanism
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve stored fact by key
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Update existing fact with new information (learning)
    void updateFact(const std::string& key, const std::string& newValue, float learningRate = -1.0f);
    
    // Clear all knowledge
    void clear();
    
    // Get all stored facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Search facts using keywords
    std::vector<std::pair<std::string, std::string>> search(const std::string& query) const;
    
    // Pattern completion for incomplete knowledge
    std::string completePattern(const std::string& partial) const;
    
    // Get related concepts and categories
    std::vector<std::string> getRelatedConcepts(const std::string& concept) const;
    std::vector<std::string> getAllConcepts() const;
    std::vector<std::string> getAllCategories() const;
    
    // Knowledge consolidation (prune low-confidence facts)
    void consolidate(float threshold);
    
    // Configuration
    void setLearningRate(float rate);
    void setConsolidationThreshold(float threshold);
    void setMaxFacts(size_t max);
    
    // Statistics
    float getAverageConfidence() const;
    size_t getFactCount() const;
    float getLearningRate() const;
    float getConsolidationThreshold() const;
    size_t getMaxFacts() const;
    
    // Integration
    void connectToBrain(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural memory: learned skills and habits
// Neural implementation of skill acquisition and execution

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
    
    // Initialize with brain reference for learning integration
    void initialize(Brain* brain);
    
    // Learn new skill from neural pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill by name
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update skill proficiency based on practice/performance
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all skills
    void clear();
    
    // Additional functionality
    std::vector<Skill*> getSkillsByCategory(const std::string& category);
    std::vector<Skill*> getProficientSkills(float threshold);
    void decaySkills();
    std::vector<NeuronId> executeSkill(const std::string& name, float activationLevel = 1.0f);
    void learnFromExecution(const std::string& name, float success, float learningRate = -1.0f);
    
    // Configuration
    void setLearningRate(float rate);
    void setConsolidationThreshold(float threshold);
    void setMaxSkills(size_t max);
    void setSkillDecayRate(float rate);
    
    // Statistics
    float getAverageProficiency() const;
    size_t getSkillCount() const;
    float getLearningRate() const;
    float getConsolidationThreshold() const;
    size_t getMaxSkills() const;
    float getSkillDecayRate() const;
    
    // Integration
    void connectToBrain(Brain* brain);
    
    // Consolidation
    void consolidate(float threshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative memory: relationships between representations
// Neural implementation of experience-based associations and spreading activation

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Initialize with brain reference for learning integration
    void initialize(Brain* brain);
    
    // Create association between two neural patterns
    void associate(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength = 1.0f);
    
    // Create association based on experience (episode encoding)
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    // Retrieve patterns associated with a query pattern
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern, size_t maxResults = 5) const;
    
    // Get association strength between two patterns
    float getAssociationStrength(const std::vector<float>& patternA, const std::vector<float>& patternB) const;
    
    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta);
    
    // Spread activation from cue pattern through association network
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern, size_t steps = 2) const;
    
    // Clear all associations
    void clear();
    
    // Configuration
    void setLearningRate(float rate);
    void setDecayRate(float rate);
    void setInhibitionStrength(float strength);
    void setMaxAssociations(size_t max);
    void setMaxActivationSpreadSteps(int steps);
    
    // Statistics
    size_t getAssociationCount() const;
    float getAverageAssociationStrength() const;
    
    // Integration
    void connectToBrain(Brain* brain);
    
    // Consolidation
    void consolidate(float threshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
