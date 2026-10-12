#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>
#include <memory>
#include "../brain/Brain.hpp"

namespace nlm {

// Forward declarations for neural memory systems
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class AttentionalSelection;
class NeuralPlanner;

// Working memory: temporary active storage of information
// Three implementation options:
// 1. Simple implementation (default): Basic key-value storage
// 2. Neural implementation (if configured): Uses persistent neural activity
// 3. Hybrid implementation: Combines both approaches

class WorkingMemory {
public:
    // Implementation mode enumeration
    enum class ImplementationMode {
        Simple,           // Basic key-value storage
        Neural,           // Uses neural dynamics (requires brain reference)
        Hybrid            // Combines simple and neural approaches
    };

    WorkingMemory();
    ~WorkingMemory();

    // Configure implementation mode
    void setImplementationMode(ImplementationMode mode);
    ImplementationMode getImplementationMode() const;

    // Store item (implementation varies based on mode)
    // Simple mode: Direct key-value storage
    // Neural mode: Encodes information as neural activity pattern
    // Hybrid mode: Stores in simple system, updates neural representation
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

    // Initialize neural working memory (if using neural implementation)
    void initializeNeuralWorkingMemory(Brain* brain);

    // Update neural working memory (call each simulation step)
    void updateNeuralWorkingMemory(TimestepDuration dt);

    // Integration with brain systems
    void setBrainReference(Brain* brain);
    bool hasBrainReference() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Episodic memory: storage of events and experiences
// Two implementation options:
// 1. Simple implementation (default): Chronological event storage
// 2. Neural implementation (if configured): Uses replay and consolidation

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    
    EpisodicMemoryItem() : timestamp(0) {}
};

class EpisodicMemory {
public:
    // Implementation mode enumeration
    enum class ImplementationMode {
        Simple,           // Chronological event storage
        Neural,           // Uses neural replay and consolidation
        Hybrid            // Combines simple and neural approaches
    };

    EpisodicMemory();
    ~EpisodicMemory();

    // Configure implementation mode
    void setImplementationMode(ImplementationMode mode);
    ImplementationMode getImplementationMode() const;

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

    // Initialize neural episodic memory (if using neural implementation)
    void initializeNeuralEpisodicMemory(Brain* brain);

    // Update neural episodic memory (call during replay phase)
    void updateNeuralEpisodicMemory();

    // Integration with brain systems
    void setBrainReference(Brain* brain);
    bool hasBrainReference() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Semantic memory: gradually acquired knowledge
// Two implementation options:
// 1. Simple implementation (default): Key-value fact storage
// 2. Neural implementation (if configured): Uses concept formation

class SemanticMemory {
public:
    // Implementation mode enumeration
    enum class ImplementationMode {
        Simple,           // Direct key-value storage
        Neural,           // Uses concept formation from neural patterns
        Hybrid            // Combines simple and neural approaches
    };

    SemanticMemory();
    ~SemanticMemory();

    // Configure implementation mode
    void setImplementationMode(ImplementationMode mode);
    ImplementationMode getImplementationMode() const;

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

    // Initialize neural semantic memory (if using neural implementation)
    void initializeNeuralSemanticMemory(Brain* brain);

    // Update neural semantic memory (call each simulation step)
    void updateNeuralSemanticMemory(TimestepDuration dt);

    // Integration with brain systems
    void setBrainReference(Brain* brain);
    bool hasBrainReference() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural memory: learned skills and habits
// Three implementation options:
// 1. Simple implementation (default): Skill database
// 2. Neural implementation (if configured): Uses motor patterns
// 3. Hybrid implementation: Combines skill database with neural patterns

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    float lastUsed;
    
    Skill() : proficiency(0.0f), lastUsed(0.0f) {}
};

class ProceduralMemory {
public:
    // Implementation mode enumeration
    enum class ImplementationMode {
        Simple,           // Simple skill database
        Neural,           // Uses neural motor patterns and skill replay
        Hybrid            // Combines both approaches
    };

    ProceduralMemory();
    ~ProceduralMemory();

    // Configure implementation mode
    void setImplementationMode(ImplementationMode mode);
    ImplementationMode getImplementationMode() const;

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

    // Execute skill (neural implementation)
    void executeSkill(const std::string& name);

    // Initialize neural procedural memory (if using neural implementation)
    void initializeNeuralProceduralMemory(Brain* brain);

    // Update neural procedural memory (call each simulation step)
    void updateNeuralProceduralMemory(TimestepDuration dt);

    // Reinforcement learning for skills
    void reinforceSkill(const std::string& name, float reward);

    // Integration with brain systems
    void setBrainReference(Brain* brain);
    bool hasBrainReference() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative memory: relationships between representations
// Two implementation options:
// 1. Simple implementation (default): Pair-based associations
// 2. Neural implementation (if configured): Uses synaptic connections

class AssociativeMemory {
public:
    // Implementation mode enumeration
    enum class ImplementationMode {
        Simple,           // Simple pair-based associations
        Neural,           // Uses synaptic connections for associations
        Hybrid            // Combines both approaches
    };

    AssociativeMemory();
    ~AssociativeMemory();

    // Configure implementation mode
    void setImplementationMode(ImplementationMode mode);
    ImplementationMode getImplementationMode() const;

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

    // Initialize neural associative memory (if using neural implementation)
    void initializeNeuralAssociativeMemory(Brain* brain);

    // Update neural associative memory (call each simulation step)
    void updateNeuralAssociativeMemory(TimestepDuration dt);

    // Hebbian learning for associations
    void applyHebbianLearning(NeuronId pre, NeuronId post, float learningRate);

    // Integration with brain systems
    void setBrainReference(Brain* brain);
    bool hasBrainReference() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global memory system configuration
class MemorySystem {
public:
    static MemorySystem& getInstance();
    
    void initialize(Brain* brain);
    void update(TimestepDuration dt);
    void shutdown();
    
    // Configure each memory system independently
    void setWorkingMemoryMode(WorkingMemory::ImplementationMode mode);
    void setEpisodicMemoryMode(EpisodicMemory::ImplementationMode mode);
    void setSemanticMemoryMode(SemanticMemory::ImplementationMode mode);
    void setProceduralMemoryMode(ProceduralMemory::ImplementationMode mode);
    void setAssociativeMemoryMode(AssociativeMemory::ImplementationMode mode);
    
    // Get memory system instances
    WorkingMemory& getWorkingMemory() { return workingMemory_; }
    EpisodicMemory& getEpisodicMemory() { return episodicMemory_; }
    SemanticMemory& getSemanticMemory() { return semanticMemory_; }
    ProceduralMemory& getProceduralMemory() { return proceduralMemory_; }
    AssociativeMemory& getAssociativeMemory() { return associativeMemory_; }

private:
    MemorySystem();
    ~MemorySystem();
    MemorySystem(const MemorySystem&) = delete;
    MemorySystem& operator=(const MemorySystem&) = delete;
    
    WorkingMemory workingMemory_;
    EpisodicMemory episodicMemory_;
    SemanticMemory semanticMemory_;
    ProceduralMemory proceduralMemory_;
    AssociativeMemory associativeMemory_;
    
    Brain* brain_;
    bool initialized_;
};

} // namespace nlm