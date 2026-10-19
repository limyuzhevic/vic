#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;

// Memory Item Structure
struct MemoryItem {
    size_t id;
    std::vector<float> pattern;
    float relevance;
    SimulationStep age;
    SimulationStep lastAccess;
    float strength;
    
    MemoryItem(size_t idx, const std::vector<float>& pat)
        : id(idx), pattern(pat), relevance(1.0f), age(0), lastAccess(0), strength(1.0f) {}
};

// Base Memory Interface
class Memory {
public:
    virtual ~Memory() = default;
    
    // Core memory operations
    virtual void store(const std::vector<float>& pattern, float relevance = 1.0f) = 0;
    virtual std::vector<float> retrieve(size_t index) const = 0;
    virtual bool contains(const std::vector<float>& pattern) const = 0;
    
    // Memory management
    virtual void decay(float decayRate) = 0;
    virtual void consolidate(float relevanceThreshold) = 0;
    virtual void clear() = 0;
    
    // Capacity and statistics
    virtual size_t getCapacity() const = 0;
    virtual size_t getCurrentSize() const = 0;
    virtual float getLoad() const = 0;
    
    // Neural integration
    virtual void updateFromBrain(Brain* brain) = 0;
    virtual void applyToBrain(Brain* brain) = 0;
};

// Working Memory Interface
class WorkingMemory : public Memory {
public:
    virtual ~WorkingMemory() = default;
    
    // Working memory specific operations
    virtual void addTrace(const std::vector<float>& trace, float initialStrength = 1.0f) = 0;
    virtual void removeTrace(size_t index) = 0;
    virtual void updateTrace(size_t index, const std::vector<float>& newPattern) = 0;
    virtual std::vector<size_t> getActiveTraces() const = 0;
    virtual void setDecayRate(float rate) = 0;
    virtual void setCapacity(size_t capacity) = 0;
    
    // Competition and selection
    virtual void runCompetition() = 0;
    virtual std::vector<size_t> getWinners() const = 0;
    virtual void strengthenWinners(float factor) = 0;
    virtual void inhibitLosers(float factor) = 0;
};

// Episodic Memory Interface
class EpisodicMemory : public Memory {
public:
    virtual ~EpisodicMemory() = default;
    
    // Episodic memory specific operations
    virtual void storeEpisode(const std::vector<float>& sensoryPattern, 
                             const std::vector<float>& action,
                             float reward,
                             const std::vector<float>& resultingPattern) = 0;
    virtual std::vector<const MemoryItem*> retrieveSimilar(const std::vector<float>& pattern, 
                                                          size_t maxResults = 5) const = 0;
    virtual std::vector<const MemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                           SimulationStep endTime,
                                                           size_t maxResults = 10) const = 0;
    virtual void replay(size_t index) = 0;
    virtual void enableReplay(bool enable) = 0;
    virtual bool isReplayEnabled() const = 0;
    
    // Temporal processing
    virtual void advanceTime(SimulationStep steps) = 0;
    virtual void consolidateOld(float ageThreshold) = 0;
};

// Semantic Memory Interface
class SemanticMemory : public Memory {
public:
    virtual ~SemanticMemory() = default;
    
    // Semantic memory specific operations
    virtual void learnConcept(const std::string& name, const std::vector<float>& pattern) = 0;
    virtual std::string retrieveConcept(const std::vector<float>& pattern) const = 0;
    virtual float getConceptStrength(const std::string& name) const = 0;
    virtual void updateConceptStrength(const std::string& name, float delta) = 0;
    virtual std::vector<std::string> getAllConcepts() const = 0;
    
    // Concept formation
    virtual void formConceptsFromData(const std::vector<std::vector<float>>& patterns) = 0;
    virtual void mergeConcepts(const std::string& conceptA, const std::string& conceptB) = 0;
};

// Procedural Memory Interface
class ProceduralMemory : public Memory {
public:
    virtual ~ProceduralMemory() = default;
    
    // Procedural memory specific operations
    virtual void learnSkill(const std::string& name, const std::vector<float>& pattern) = 0;
    virtual std::vector<float> executeSkill(const std::string& name) = 0;
    virtual float getSkillProficiency(const std::string& name) const = 0;
    virtual void updateSkillProficiency(const std::string& name, float delta) = 0;
    virtual std::vector<std::string> getAllSkills() const = 0;
    
    // Skill composition
    virtual void sequenceSkills(const std::vector<std::string>& skillSequence) = 0;
    virtual void interruptSkill(const std::string& name) = 0;
    virtual void pauseSkill(const std::string& name) = 0;
    virtual void resumeSkill(const std::string& name) = 0;
};

// Associative Memory Interface
class AssociativeMemory : public Memory {
public:
    virtual ~AssociativeMemory() = default;
    
    // Associative memory specific operations
    virtual void createAssociation(const std::vector<float>& patternA,
                                  const std::vector<float>& patternB,
                                  float strength = 1.0f) = 0;
    virtual std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cue,
                                                           size_t steps = 2) const = 0;
    virtual float getAssociationStrength(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB) const = 0;
    virtual void updateAssociation(const std::vector<float>& patternA,
                                  const std::vector<float>& patternB,
                                  float delta) = 0;
    virtual void clearAssociations() = 0;
    
    // Pattern completion
    virtual std::vector<float> completePattern(const std::vector<float>& partialPattern) const = 0;
    virtual bool isPatternAssociated(const std::vector<float>& pattern) const = 0;
};

// Memory System Interface
class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    // Initialize all memory levels
    virtual void initialize() = 0;
    
    // Memory access
    virtual WorkingMemory* getWorkingMemory() = 0;
    virtual EpisodicMemory* getEpisodicMemory() = 0;
    virtual SemanticMemory* getSemanticMemory() = 0;
    virtual ProceduralMemory* getProceduralMemory() = 0;
    virtual AssociativeMemory* getAssociativeMemory() = 0;
    
    // System-wide operations
    virtual void updateAll(SimulationStep step, TimestepDuration dt) = 0;
    virtual void consolidateAll(float relevanceThreshold) = 0;
    virtual void replayAll() const = 0;
    virtual void clearAll() = 0;
    
    // Integration between levels
    virtual void propagateToLowerLevels(const std::vector<float>& pattern, float strength = 1.0f) = 0;
    virtual void propagateToHigherLevels(const std::vector<float>& pattern, float strength = 1.0f) = 0;
    
    // Statistics
    virtual float getSystemLoad() const = 0;
    virtual size_t getTotalMemoryUsage() const = 0;
};

} // namespace nlm