#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// Now implemented with neural dynamics using NeuralWorkingMemory
// This class maintains backward compatibility with basic interface

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item
    // Uses neural dynamics for persistent activity representation
    void store(NeuronId neuron, float value);
    
    // Retrieve item
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all
    void clear();
    
    // Get capacity
    size_t getCapacity() const { return 100; }  // Default capacity
    size_t getCurrentSize() const { return currentSize_; }
    
    // Decay all items
    void decay(float decayRate);
    
    // Access to neural implementation for advanced users
    void initialize(class NeuralWorkingMemory* neuralMem) {
        neuralMemory_ = neuralMem;
    }
    
    // Get number of active traces from neural implementation
    size_t getActiveTraces() const {
        return neuralMemory_ ? neuralMemory_->getActiveTraces() : 0;
    }
    
    // Update neural working memory dynamics
    void update(float dt) {
        if (neuralMemory_) {
            neuralMemory_->update(dt);
        }
    }
    
    // Add a neuron to working memory
    void addNeuron(NeuronId neuron, float activation) {
        if (neuralMemory_) {
            neuralMemory_->storeToNeuron(neuron, activation);
        }
    }
    
private:
    struct Impl;
    Impl* pImpl;
    NeuralWorkingMemory* neuralMemory_;
    size_t currentSize_;
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

// Episodic memory: storage of events and experiences
// Now delegates to NeuralEpisodicMemory for real implementation

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
    
    // Initialize with neural implementation
    void initialize(class NeuralEpisodicMemory* neuralMem) {
        neuralMemory_ = neuralMem;
    }
    
    // Get neural implementation for advanced usage
    class NeuralEpisodicMemory* getNeuralImplementation() { return neuralMemory_; }
    
private:
    struct Impl;
    Impl* pImpl;
    NeuralEpisodicMemory* neuralMemory_;
};

// Semantic memory: gradually acquired knowledge
// Now implements basic knowledge storage for Phase 1

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
    
    // Create concept from pattern (Phase 1 basic implementation)
    void createConcept(const std::vector<float>& pattern, const std::string& conceptName);
    
    // Retrieve concept by name
    std::vector<float> retrieveConcept(const std::string& conceptName) const;
    
    // Get all concept names
    std::vector<std::string> getConceptNames() const;
    
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

// Procedural memory: learned skills and habits
// Now implements basic skill learning for Phase 1

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
    
    // Get skill proficiency
    float getProficiency(const std::string& name) const;
    
    // Check if skill exists
    bool hasSkill(const std::string& name) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// Now implements basic associations for Phase 1

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
    
    // Spread activation from a neuron
    std::vector<NeuronId> spreadActivation(NeuronId source, float strength = 1.0f) const;
    
    // Get all associations
    std::vector<std::tuple<NeuronId, NeuronId, float>> getAllAssociations() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
