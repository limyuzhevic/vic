#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// NOW IMPLEMENTED - Phase 2 functionality

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store item in neural activation pattern
    void store(NeuronId neuron, float value);
    
    // Retrieve item value
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all items
    void clear();
    
    // Get capacity
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Decay all items (time-based forgetting)
    void decay(float decayRate);
    
    // Update working memory (maintenance and competition)
    void update(float dt);
    
    // Get active neurons in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const;
    
    // Check if neuron is currently active in working memory
    bool isActive(NeuronId neuron) const;
    
    // Boost activation of specific memory
    void strengthenMemory(NeuronId neuron, float factor);
    
    // Run competition between active memories
    void runCompetition();
    
    // Get most active neurons
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Episodic memory: storage of events and experiences
// NOW IMPLEMENTED - Phase 2 functionality

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> activations;
    std::string metadata;
    
    EpisodicMemoryItem() : timestamp(0) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store episode (experience)
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes for replay
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all episodes
    void clear();
    
    // Consolidate important memories (move to long-term storage)
    void consolidate(float relevanceThreshold);
    
    // Replay episode (reactivate neural patterns)
    void replayEpisode(const EpisodicMemoryItem& episode);
    
    // Get episodes for replay training
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t maxCount) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Semantic memory: gradually acquired knowledge and facts
// NOW IMPLEMENTED - Phase 2 functionality

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store fact (conceptual knowledge)
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all facts
    void clear();
    
    // Get fact similarity between two concepts
    float getConceptSimilarity(const std::string& concept1, const std::string& concept2) const;
    
    // Get related concepts
    std::vector<std::string> getRelatedConcepts(const std::string& concept) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural memory: learned skills and behaviors
// NOW IMPLEMENTED - Phase 2 functionality

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    SimulationStep lastUsed;
    float effectiveness;
    
    Skill() : proficiency(0.0f), lastUsed(0), effectiveness(0.0f) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn skill from neural pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& neuralPattern);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    const Skill* getSkill(const std::string& name) const;
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    std::vector<const Skill*> getAllSkills() const;
    
    // Execute skill (activate corresponding neural pattern)
    bool executeSkill(const std::string& name, Brain* brain);
    
    // Update proficiency based on success
    void updateProficiency(const std::string& name, float delta, float successRate);
    
    // Get skill effectiveness
    float getSkillEffectiveness(const std::string& name) const;
    
    // Clear all skills
    void clear();
    
    // Get skills sorted by proficiency
    std::vector<Skill*> getTopSkills(size_t count) const;
    
    // Find skill by neural pattern similarity
    std::vector<Skill*> findSimilarSkills(const std::vector<NeuronId>& neuralPattern, float threshold) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative memory: relationships between concepts and patterns
// NOW IMPLEMENTED - Phase 2 functionality

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association between two items (both can be neurons or concepts)
    void associate(const std::string& key1, const std::string& key2, float strength);
    void associate(NeuronId neuron1, NeuronId neuron2, float strength);
    void associate(const std::string& concept, NeuronId neuron, float strength);
    
    // Get associated items
    std::vector<std::string> getAssociations(const std::string& key) const;
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(const std::string& key1, const std::string& key2) const;
    float getAssociationStrength(NeuronId neuron1, NeuronId neuron2) const;
    
    // Update association strength
    void updateAssociation(const std::string& key1, const std::string& key2, float delta);
    void updateAssociation(NeuronId neuron1, NeuronId neuron2, float delta);
    
    // Get association pathways (chains of associations)
    std::vector<std::vector<std::string>> findAssociationPaths(const std::string& start, const std::string& target) const;
    std::vector<std::vector<NeuronId>> findNeuralAssociationPaths(NeuronId start, NeuronId target) const;
    
    // Check if associations exist
    bool hasAssociation(const std::string& key1, const std::string& key2) const;
    bool hasAssociation(NeuronId neuron1, NeuronId neuron2) const;
    
    // Clear all associations
    void clear();
    
    // Get all association statistics
    size_t getTotalAssociations() const;
    size_t getAssociationCount(const std::string& key) const;
    
    // Propagate activation through associations
    void propagateActivation(const std::string& concept, float spreadFactor);
    void propagateActivation(NeuronId neuron, float spreadFactor, Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
