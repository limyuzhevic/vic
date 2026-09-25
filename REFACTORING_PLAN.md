# NLM Codebase Refactoring Plan

## Summary

The NLM (Neural Learning Machine) codebase contains significant duplication and structural issues across multiple system domains. This refactoring plan addresses these issues through systematic consolidation, interface standardization, and improved modularity while maintaining backward compatibility.

## Core Issues Identified

### 1. Plasticity Systems Duplication
- **PlasticityRule.hpp** contains base class + HebbianRule and AntiHebbianRule (but AntiHebbianRule doesn't inherit!)
- **Hebbian.hpp** has class `Hebbian` (not `HebbianRule`) - inconsistent naming
- **STDP.hpp** has STDP, RewardModulatedSTDP, and TripletSTDP
- **StructuralPlasticity.hpp** is separate from PlasticityRule, causing fragmentation

### 2. Memory Systems Overlap
- **WorkingMemory, EpisodicMemory, SemanticMemory, ProceduralMemory, AssociativeMemory** - all similar storage/retrieval patterns
- **NeuralWorkingMemory** and **NeuralEpisodicMemory** duplicate basic memory patterns with neural implementations
- Inconsistent interfaces across memory types

### 3. Neuromodulation Interface Inconsistency
- **Neuromodulator** is abstract base with **Dopamine** inheriting from it
- **Reward, Curiosity, Novelty, PredictionError** are separate classes (not inheriting)
- Similar patterns: level management, update methods, history tracking

### 4. Cognition System Duplication
- **NeuralPlanner, ConceptFormation, SelfModel, SocialLearning, SpatialRepresentation, TemporalRelation** - all similar processing patterns
- Multiple classes with similar initialize/update methods
- Replicated similarity computation logic

### 5. Development System Overlap
- **Synaptogenesis, Maturation, Pruning, DevelopmentSystem** - overlapping functionality
- DevelopmentSystem orchestrates others but doesn't inherit from them
- Similar rate management and update patterns

## Refactoring Strategy

### Phase 1: Foundation Consolidation (High Priority)

#### 1.1 Unify Plasticity Systems
**Goal:** Create a cohesive plasticity system with consistent inheritance and clear separation of concerns.

**Changes:**
- Rename `HebbianRule` to `Hebbian` for consistency
- Create abstract `PlasticityRuleBase` as base class
- Make `AntiHebbianRule` inherit from `PlasticityRuleBase`
- Create `PlasticitySystem` class to manage all plasticity rules
- Extract common parameters into base class

**Files to modify:**
- `src/plasticity/PlasticityRule.hpp` - New base class, remove duplicate rules
- `src/plasticity/Hebbian.cpp` - Rename to `Hebbian.cpp`
- `src/plasticity/STDP.hpp` - Remove standalone rule classes, integrate into system
- `src/plasticity/StructuralPlasticity.hpp` - Make inherit from PlasticityRuleBase

#### 1.2 Standardize Memory Interface
**Goal:** Create abstract memory interface with consistent CRUD operations.

**Changes:**
- Create `MemorySystem` abstract base with common interface
- Implement `MemoryAdapter` for backward compatibility
- Refactor all memory classes to implement consistent interface
- Create memory type enumerations for type safety

**Files to modify:**
- `src/memory/Memory.hpp` - Create new abstract base, refactor existing classes
- All memory system files - Implement new interface

### Phase 2: Neuromodulation System Integration (Medium Priority)

#### 2.1 Create Unified Neuromodulation Framework
**Goal:** Standardize neuromodulator interfaces and create system for managing signals.

**Changes:**
- Create abstract `NeuromodulatorBase` with consistent interface
- Make all neuromodulators inherit from base
- Create `NeuromodulationSystem` to manage signal interactions
- Standardize parameter management and history tracking

**Files to modify:**
- `src/neuromodulation/Neuromodulator.hpp` - Create new base, refactor Dopamine
- `src/neuromodulation/Reward.hpp` - Make inherit from NeuromodulatorBase
- `src/neuromodulation/Curiosity.hpp` - Make inherit from NeuromodulatorBase
- `src/neuromodulation/Novelty.hpp` - Make inherit from NeuromodulatorBase
- `src/neuromodulation/PredictionError.hpp` - Make inherit from NeuromodulatorBase

### Phase 3: Cognition System Consolidation (Medium Priority)

#### 3.1 Abstract Cognitive Processing Interface
**Goal:** Create common interface for cognitive processing with shared utilities.

**Changes:**
- Create `CognitiveProcessor` abstract base
- Extract common utilities (similarity computation, state management)
- Implement interface for planning, learning, and attention
- Create `CognitiveSystem` to manage all processors

**Files to modify:**
- `src/cognition/NeuralPlanner.hpp` - Refactor to implement CognitiveProcessor interface
- `src/cognition/ConceptFormation.hpp` - Refactor to implement CognitiveProcessor interface
- Create new `cognitive/CognitiveSystem.hpp`

### Phase 4: Development System Integration (Medium Priority)

#### 4.1 Unify Development Mechanisms
**Goal:** Create consistent development system with clear progression stages.

**Changes:**
- Create `DevelopmentStage` enum for clear state management
- Make all development mechanisms inherit from `DevelopmentProcess`
- Create `DevelopmentSystem` to coordinate all processes
- Extract common timing and rate management

**Files to modify:**
- `src/development/DevelopmentSystem.hpp` - Refactor as central manager
- `src/development/Synaptogenesis.hpp` - Make inherit from DevelopmentProcess
- `src/development/Maturation.hpp` - Make inherit from DevelopmentProcess
- `src/development/Pruning.hpp` - Make inherit from DevelopmentProcess

## Specific Refactoring Tasks

### Task 1: Plasticity System Consolidation
**Priority:** High
**Effort:** ~40 hours
**Files:** 5 header files, 4 implementation files

**Deliverables:**
1. Unified plasticity inheritance hierarchy
2. Consistent parameter management
3. Centralized plasticity rule management
4. Backward compatibility maintained

### Task 2: Memory System Standardization
**Priority:** High  
**Effort:** ~35 hours
**Files:** 7 header files, 6 implementation files

**Deliverables:**
1. Abstract memory interface
2. Consistent CRUD operations
3. Type-safe memory management
4. Performance optimizations (shared storage)

### Task 3: Neuromodulation Framework
**Priority:** Medium
**Effort:** ~25 hours
**Files:** 5 header files, 5 implementation files

**Deliverables:**
1. Consistent neuromodulator interfaces
2. Unified parameter management
3. Signal interaction framework
4. Backward compatibility maintained

### Task 4: Cognition System Integration
**Priority:** Medium
**Effort:** ~45 hours
**Files:** 4 header files, 4 implementation files
**New Files:** 2 header files, 1 implementation file

**Deliverables:**
1. Abstract cognitive processor interface
2. Shared utility functions
3. Central cognitive system manager
4. Enhanced processing capabilities

### Task 5: Development System Cohesion
**Priority:** Medium
**Effort:** ~30 hours
**Files:** 5 header files, 4 implementation files

**Deliverables:**
1. Consistent development stages
2. Unified process management
3. Enhanced progression tracking
4. Backward compatibility maintained

## Code Examples

### Example 1: Improved Plasticity System Structure

**Before (duplicated patterns):**
```cpp
// PlasticityRule.hpp - Contains base + some derived
class PlasticityRule {
    virtual void update(Synapse*, const std::vector<Timestamp>&, const std::vector<Timestamp>&, TimestepDuration) = 0;
    // ... common methods
};

class HebbianRule : public PlasticityRule {  // Inconsistent naming!
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
};

class AntiHebbianRule : public PlasticityRule {  // Doesn't actually inherit in original!
    void update(...) override {}  // Empty implementation
    // ... similar patterns
};

// Hebbian.hpp - Separate class with different name
class Hebbian : public PlasticityRule {  // Inconsistent naming!
    // Parameters
    void setLearningRate(float rate);  // Duplicate method signature
    float getLearningRate() const;
};
```

**After (consolidated structure):**
```cpp
// PlasticityRuleBase.hpp - Clean base class
class PlasticityRuleBase {
public:
    virtual ~PlasticityRuleBase() = default;
    virtual void update(Synapse*, const std::vector<Timestamp>&, const std::vector<Timestamp>&, TimestepDuration) = 0;
    virtual void applyWeightChange(Synapse*, SynapticWeight) = 0;
    virtual const char* getName() const = 0;
    virtual float getPlasticityFactor() const = 0;
    
    // Common parameters
    virtual void setLearningRate(float rate) {}
    virtual float getLearningRate() const { return 0.01f; }
    
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
protected:
    PlasticityRuleBase() : enabled_(true) {}
    bool enabled_;
};

// PlasticityRule.hpp - Specific implementations
class HebbianRule : public PlasticityRuleBase {
public:
    void update(...) override;  // Real implementation
    const char* getName() const override { return "Hebbian"; }
    
    void setLearningRate(float rate) override { learningRate_ = rate; }
    float getLearningRate() const override { return learningRate_; }
    
private:
    float learningRate_ = 0.01f;
};

class AntiHebbianRule : public PlasticityRuleBase {
public:
    void update(...) override { /* Real implementation */ }
    const char* getName() const override { return "AntiHebbian"; }
};

// PlasticitySystem.hpp - Central manager
class PlasticitySystem {
public:
    void registerRule(std::unique_ptr<PlasticityRuleBase> rule);
    void updateAll(Synapse*, const std::vector<Timestamp>&, const std::vector<Timestamp>&, TimestepDuration);
    PlasticityRuleBase* getRule(const std::string& name) const;
    
private:
    std::vector<std::unique_ptr<PlasticityRuleBase>> rules_;
};
```

### Example 2: Standardized Memory Interface

**Before (inconsistent patterns):**
```cpp
// Memory.hpp - Mixed approaches
class WorkingMemory {
    void store(NeuronId neuron, float value);
    float retrieve(NeuronId neuron) const;
    // Simple key-value storage
};

class EpisodicMemory {
    void storeEpisode(const EpisodicMemoryItem& episode);
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    // Time-based retrieval
};

class SemanticMemory {
    void storeFact(const std::string& key, const std::string& value);
    std::string retrieveFact(const std::string& key) const;
    // String-based storage
};
```

**After (unified interface):**
```cpp
// MemorySystem.hpp - Abstract base
class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    // Universal interface
    virtual bool store(const MemoryKey& key, const MemoryValue& value) = 0;
    virtual MemoryResult retrieve(const MemoryKey& key, MemoryValue& out) const = 0;
    virtual bool remove(const MemoryKey& key) = 0;
    virtual bool contains(const MemoryKey& key) const = 0;
    virtual void clear() = 0;
    
    // Type-specific operations
    virtual bool store(const MemoryEntry& entry) = 0;
    virtual MemoryResult find(const MemoryQuery& query) const = 0;
    
    // Common functionality
    virtual size_t getCapacity() const { return capacity_; }
    virtual size_t getCurrentSize() const = 0;
    virtual float getDecayRate() const { return decayRate_; }
    virtual void setDecayRate(float rate) { decayRate_ = rate; }
    
protected:
    MemorySystem(size_t capacity, float decayRate) 
        : capacity_(capacity), decayRate_(decayRate) {}
    size_t capacity_;
    float decayRate_;
};

// MemoryAdapter.hpp - Backward compatibility
class MemoryAdapter : public MemorySystem {
public:
    // Adapt old interfaces to new unified interface
    bool store(const MemoryKey& key, const MemoryValue& value) override {
        // Map to appropriate old-style storage
        return storeInternal(key, value);
    }
    
    // ... adapt all other methods
};
```

### Example 3: Unified Neuromodulation Framework

**Before (inconsistent inheritance):**
```cpp
// Neuromodulator.hpp - Abstract base
class Neuromodulator {
    virtual float getPlasticityFactor() const = 0;
    virtual void update(TimestepDuration dt) = 0;
};

// Dopamine.hpp - Inherits correctly
class Dopamine : public Neuromodulator {
    // Proper implementation
};

// Reward.hpp - Does NOT inherit (duplicated interface!)
class Reward {
    float getPlasticityFactor() const { return 0.5f; }  // Duplicate interface!
    void update(TimestepDuration dt) { /* implementation */ }
    // ... other similar methods
};
```

**After (consistent hierarchy):**
```cpp
// NeuromodulatorBase.hpp - True abstract base
class NeuromodulatorBase {
public:
    virtual ~NeuromodulatorBase() = default;
    
    virtual const char* getName() const = 0;
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    virtual float getPlasticityFactor() const = 0;
    virtual void update(TimestepDuration dt) = 0;
    
    // Common neuromodulation effects
    virtual void applyToPlasticity(float& weightChange) const {}
    virtual void applyToExcitability(float& excitabilityChange) const {}
};

// Neuromodulator.hpp - Now includes base class
class Neuromodulator : public NeuromodulatorBase {
    // Implementation details
};

// Dopamine.hpp - Proper inheritance
class Dopamine : public NeuromodulatorBase {
    const char* getName() const override { return "Dopamine"; }
    // ... complete implementation
};

// Reward.hpp - Now inherits correctly
class Reward : public NeuromodulatorBase {
    const char* getName() const override { return "Reward"; }
    // ... complete implementation with consistent interface
};
```

### Example 4: Cognitive System Integration

**Before (duplicated patterns):**
```cpp
// NeuralPlanner.hpp - First cognitive system
class NeuralPlanner {
    void initialize(Brain* brain);
    ActionType planAction(const std::vector<float>& state, float reward);
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions, const std::vector<float>& state);
    
    // Common utilities duplicated in other systems
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
        // Cosine similarity implementation
    }
};

// ConceptFormation.hpp - Second cognitive system  
class ConceptFormation {
    void initialize(Brain* brain);
    size_t presentExperience(const std::vector<float>& pattern, const std::vector<float>& features, float reward);
    
    // Duplicated utility - EXACT SAME CODE!
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
        // Identical cosine similarity implementation
    }
};
```

**After (shared utilities):**
```cpp
// CognitiveUtilities.hpp - Shared functions
namespace CognitiveUtilities {
    float computeCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
    float computeEuclideanDistance(const std::vector<float>& a, const std::vector<float>& b);
    float computeManhattanDistance(const std::vector<float>& a, const std::vector<float>& b);
    float clamp(float value, float min, float max);
    bool isInRange(float value, float min, float max);
}

// CognitiveProcessor.hpp - Abstract base
class CognitiveProcessor {
public:
    virtual ~CognitiveProcessor() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(TimestepDuration dt) = 0;
    virtual bool isInitialized() const { return initialized_; }
    
protected:
    CognitiveProcessor() : initialized_(false) {}
    bool initialized_;
};

// NeuralPlanner.hpp - Now implements CognitiveProcessor
class NeuralPlanner : public CognitiveProcessor {
public:
    ActionType planAction(const std::vector<float>& state, float reward);
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions, const std::vector<float>& state);
    
    // Uses shared utilities
    float similarity(const std::vector<float>& a, const std::vector<float>& b) const {
        return CognitiveUtilities::computeCosineSimilarity(a, b);
    }
};

// ConceptFormation.hpp - Now implements CognitiveProcessor
class ConceptFormation : public CognitiveProcessor {
public:
    size_t presentExperience(const std::vector<float>& pattern, const std::vector<float>& features, float reward);
    
    // Uses shared utilities
    float similarity(const std::vector<float>& a, const std::vector<float>& b) const {
        return CognitiveUtilities::computeCosineSimilarity(a, b);
    }
};
```

## Implementation Roadmap

### Phase 1: Foundation (Months 1-2)
1. Consolidate plasticity systems
2. Standardize memory interface  
3. Ensure backward compatibility throughout
4. Update tests and documentation

### Phase 2: Integration (Months 3-4)
1. Implement neuromodulation framework
2. Integrate cognitive systems
3. Consolidate development processes
4. Performance optimization

### Phase 3: Validation (Months 5-6)
1. Comprehensive testing
2. Performance benchmarking
3. Documentation updates
4. Final code review

## Benefits

### Immediate Benefits (Phase 1)
- Reduced code duplication (~40% reduction in boilerplate)
- Improved maintainability
- Consistent interfaces across systems
- Enhanced testability
- Better code organization

### Long-term Benefits (Phases 2-3)
- Enhanced extensibility
- Improved performance
- Better modularity
- Simplified debugging
- Future-proof architecture

## Risk Mitigation

### Technical Risks
- **Backward Compatibility**: Maintain existing APIs through adapters
- **Performance Impact**: Benchmark and optimize throughout
- **Testing Complexity**: Implement incremental testing approach

### Project Risks
- **Timeline**: Phase-based approach with clear deliverables
- **Resource Allocation**: Focus on high-impact, low-effort changes first
- **Team Knowledge**: Document changes thoroughly and provide training

## Conclusion

This refactoring plan addresses the core structural issues in the NLM codebase by:

1. **Eliminating Code Duplication**: Consolidating similar functionality into shared bases
2. **Improving Modularity**: Creating clear hierarchies and interfaces
3. **Enhancing Maintainability**: Standardizing patterns across all systems
4. **Preserving Compatibility**: Ensuring existing code continues to work
5. **Future-Proofing**: Creating extensible architecture for future enhancements

The phased approach minimizes risk while delivering maximum value, creating a solid foundation for continued development of the NLM artificial brain project.