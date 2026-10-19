#pragma once

#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class WorkingMemory;
class EpisodicMemory;
class SemanticMemory;
class ProceduralMemory;
class AssociativeMemory;
class Memory;

class MemorySystem {
public:
    MemorySystem(Brain* brain);
    ~MemorySystem();
    
    // Initialize all memory levels
    void initialize();
    
    // Memory access
    WorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    EpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    SemanticMemory* getSemanticMemory() { return semanticMemory_.get(); }
    ProceduralMemory* getProceduralMemory() { return proceduralMemory_.get(); }
    AssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    
    // System-wide operations
    void updateAll(size_t step, float dt);
    void consolidateAll(float relevanceThreshold);
    void replayAll() const;
    void clearAll();
    
    // Integration between levels
    void propagateToLowerLevels(const std::vector<float>& pattern, float strength = 1.0f);
    void propagateToHigherLevels(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Statistics
    float getSystemLoad() const;
    size_t getTotalMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm