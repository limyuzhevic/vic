# NLM Codebase Refactoring - Detailed Task List

## Overview

This document provides a detailed breakdown of all refactoring tasks required to address duplication and structural issues in the NLM codebase. Each task includes specific file modifications, effort estimates, and implementation details.

## Task 1: Plasticity System Consolidation
**Priority:** High
**Effort:** ~40 hours
**Phase:** Foundation

### Files to Modify:

#### 1.1 src/plasticity/PlasticityRule.hpp (High Priority)
- **Lines:** 1-86
- **Changes:**
  - Create new abstract base class `PlasticityRuleBase`
  - Remove `HebbianRule` class (move to `HebbianRule.hpp`)
  - Remove `AntiHebbianRule` class (move to `HebbianRule.hpp`)
  - Keep only `PlasticityRule` as a concrete implementation with common functionality
  - Fix inconsistent inheritance

#### 1.2 src/plasticity/Hebbian.cpp (Low Priority)
- **Lines:** 1-106
- **Changes:**
  - Move to `src/plasticity/Hebbian.cpp` from `src/plasticity/PlasticityRule.cpp`
  - Update include to "Hebbian.hpp"
  - Keep implementation intact

#### 1.3 src/plasticity/Hebbian.hpp (Medium Priority)
- **Lines:** 1-31
- **Changes:**
  - Rename `Hebbian` to `HebbianRule` for consistency
  - Make inherit from `PlasticityRuleBase`
  - Update all method signatures to match base class
  - Ensure parameter consistency

#### 1.4 src/plasticity/STDP.hpp (Medium Priority)
- **Lines:** 1-62
- **Changes:**
  - Remove `RewardModulatedSTDP` and `TripletSTDP` classes
  - Create `RewardModulatedSTDP` and `TripletSTDP` as parameterized variants of STDP
  - Make `STDP` inherit from `PlasticityRuleBase`
  - Extract common STDP parameters to base class

#### 1.5 src/plasticity/StructuralPlasticity.hpp (High Priority)
- **Lines:** 1-49
- **Changes:**
  - Make `StructuralPlasticity` inherit from `PlasticityRuleBase`
  - Implement abstract methods
  - Remove duplicate parameter management
  - Integrate with existing plasticity system

#### 1.6 Create src/plasticity/PlasticitySystem.hpp (New File)
- **Lines:** 1-100
- **Changes:**
  - Create central manager for all plasticity rules
  - Implement rule registration and management
  - Provide unified interface for plasticity operations
  - Integrate with Brain class

### Post-Modification Verification:
- Run existing STDP tests to ensure functionality preserved
- Update Brain.cpp to use new PlasticitySystem
- Verify all plasticity rules still work correctly

---

## Task 2: Memory System Standardization
**Priority:** High
**Effort:** ~35 hours
**Phase:** Foundation

### Files to Modify:

#### 2.1 src/memory/Memory.hpp (High Priority)
- **Lines:** 1-174
- **Changes:**
  - Create abstract base `MemorySystem` with common interface
  - Define `MemoryKey`, `MemoryValue`, `MemoryEntry` types
  - Extract common operations: store, retrieve, remove, clear, contains
  - Add type enumerations for memory types
  - Implement `MemoryAdapter` for backward compatibility

#### 2.2 src/memory/WorkingMemory.cpp (Medium Priority)
- **Lines:** 1-? (from full read)
- **Changes:**
  - Update to implement `MemorySystem` interface
  - Adapt existing methods to new interface
  - Remove duplicate parameter management
  - Integrate with Brain class

#### 2.3 src/memory/EpisodicMemory.cpp (Medium Priority)
- **Lines:** 1-? (from full read)
- **Changes:**
  - Update to implement `MemorySystem` interface
  - Adapt `EpisodicMemoryItem` structure
  - Integrate time-based operations
  - Maintain backward compatibility

#### 2.4 src/memory/SemanticMemory.cpp (Medium Priority)
- **Lines:** 1-? (from full read)
- **Changes:**
  - Update to implement `MemorySystem` interface
  - Adapt key-value storage pattern
  - Remove duplicate operations
  - Integrate with knowledge graph

#### 2.5 src/memory/ProceduralMemory.cpp (Medium Priority)
- **Lines:** 1-? (from full read)
- **Changes:**
  - Update to implement `MemorySystem` interface
  - Adapt skill-based storage pattern
  - Remove duplicate operations
  - Integrate with action selection

#### 2.6 src/memory/AssociativeMemory.cpp (Medium Priority)
- **Lines:** 1-? (from full read)
- **Changes:**
  - Update to implement `MemorySystem` interface
  - Adapt association pattern
  - Remove duplicate operations
  - Integrate with pattern completion

#### 2.7 src/memory/NeuralWorkingMemory.hpp (High Priority)
- **Lines:** 1-107
- **Changes:**
  - Create abstract base `NeuralMemorySystem`
  - Make `NeuralWorkingMemory` inherit from `NeuralMemorySystem`
  - Remove duplicate neural-specific implementations
  - Integrate with Brain class

#### 2.8 src/memory/NeuralEpisodicMemory.hpp (High Priority)
- **Lines:** 1-214
- **Changes:**
  - Make `NeuralEpisodicMemory` inherit from `NeuralMemorySystem`
  - Remove duplicate episodic storage
  - Integrate with neural patterns
  - Maintain replay functionality

#### 2.9 Create src/memory/MemorySystem.cpp (New File)
- **Lines:** 1-200
- **Changes:**
  - Implement abstract memory system functionality
  - Provide common storage and retrieval operations
  - Include memory consolidation logic
  - Integrate with neural memory systems

### Post-Modification Verification:
- Run memory-related tests to ensure functionality preserved
- Verify all memory types still work correctly
- Performance benchmarking for memory operations
- Memory leak detection

---

## Task 3: Neuromodulation Framework
**Priority:** Medium
**Effort:** ~25 hours
**Phase:** Integration

### Files to Modify:

#### 3.1 src/neuromodulation/Neuromodulator.hpp (High Priority)
- **Lines:** 1-89
- **Changes:**
  - Create new abstract base `NeuromodulatorBase`
  - Make `Neuromodulator` class inherit from `NeuromodulatorBase`
  - Remove duplicate interface definitions
  - Standardize all parameter management
  - Implement common neuromodulatory effects

#### 3.2 src/neuromodulation/Neuromodulator.cpp (Medium Priority)
- **Lines:** 1-56
- **Changes:**
  - Update Dopamine implementation
  - Implement new base class methods
  - Remove duplicate functionality
  - Integrate with plasticity system

#### 3.3 src/neuromodulation/Reward.hpp (Medium Priority)
- **Lines:** 1-38
- **Changes:**
  - Make `Reward` class inherit from `NeuromodulatorBase`
  - Implement all abstract methods
  - Remove duplicate interface
  - Integrate with reward prediction

#### 3.4 src/neuromodulation/Curiosity.hpp (Medium Priority)
- **Lines:** 1-39
- **Changes:**
  - Make `Curiosity` class inherit from `NeuromodulatorBase`
  - Implement all abstract methods
  - Remove duplicate interface
  - Integrate with exploration behavior

#### 3.5 src/neuromodulation/Novelty.hpp (Medium Priority)
- **Lines:** 1-42
- **Changes:**
  - Make `Novelty` class inherit from `NeuromodulatorBase`
  - Implement all abstract methods
  - Remove duplicate interface
  - Integrate with novelty detection

#### 3.6 src/neuromodulation/PredictionError.hpp (Medium Priority)
- **Lines:** 1-39
- **Changes:**
  - Make `PredictionError` class inherit from `NeuromodulatorBase`
  - Implement all abstract methods
  - Remove duplicate interface
  - Integrate with prediction error signals

#### 3.7 Create src/neuromodulation/NeuromodulationSystem.hpp (New File)
- **Lines:** 1-150
- **Changes:**
  - Create central manager for all neuromodulators
  - Implement signal interaction logic
  - Provide unified neuromodulation interface
  - Integrate with Brain class

### Post-Modification Verification:
- Run neuromodulation tests to ensure functionality preserved
- Verify all neuromodulators still work correctly
- Test signal interactions
- Performance benchmarking

---

## Task 4: Cognition System Integration
**Priority:** Medium
**Effort:** ~45 hours
**Phase:** Integration

### Files to Modify:

#### 4.1 Create src/cognitive/CognitiveUtilities.hpp (New File)
- **Lines:** 1-100
- **Changes:**
  - Extract common utility functions
  - Implement similarity computation (cosine, Euclidean, Manhattan)
  - Add vector math utilities
  - Include numerical operations

#### 4.2 src/cognition/NeuralPlanner.hpp (High Priority)
- **Lines:** 1-254
- **Changes:**
  - Create abstract base `CognitiveProcessor`
  - Make `NeuralPlanner` inherit from `CognitiveProcessor`
  - Remove duplicate utility functions
  - Use `CognitiveUtilities` for similarity computation
  - Integrate with planning system

#### 4.3 src/cognition/ConceptFormation.hpp (High Priority)
- **Lines:** 1-271
- **Changes:**
  - Make `ConceptFormation` inherit from `CognitiveProcessor`
  - Remove duplicate utility functions
  - Use `CognitiveUtilities` for similarity computation
  - Integrate with concept discovery
  - Remove duplicate SelfModel implementation

#### 4.4 Create src/cognitive/CognitiveProcessor.hpp (New File)
- **Lines:** 1-100
- **Changes:**
  - Create abstract cognitive processor base class
  - Define common cognitive processing interface
  - Include state management
  - Provide update mechanism

#### 4.5 Create src/cognitive/CognitiveSystem.hpp (New File)
- **Lines:** 1-120
- **Changes:**
  - Create central cognitive system manager
  - Manage all cognitive processors
  - Provide unified cognitive interface
  - Integrate with Brain class

#### 4.6 Update src/cognition/NeuralPlanner.cpp (Medium Priority)
- **Lines:** 1-557
- **Changes:**
  - Remove duplicate SelfModel and SocialLearning implementations
  - Use shared implementations from new files
  - Update to use CognitiveUtilities
  - Remove duplicate similarity computation

#### 4.7 Create src/cognitive/SelfModel.hpp (New File)
- **Lines:** 1-180
- **Changes:**
  - Extract SelfModel implementation
  - Remove from NeuralPlanner.cpp
  - Implement cognitive self-model
  - Integrate with action prediction

#### 4.8 Create src/cognitive/SocialLearning.hpp (New File)
- **Lines:** 1-160
- **Changes:**
  - Extract SocialLearning implementation
  - Remove from NeuralPlanner.cpp
  - Implement cognitive social learning
  - Integrate with observation learning

### Post-Modification Verification:
- Run cognition tests to ensure functionality preserved
- Verify all cognitive systems still work correctly
- Test concept formation and planning
- Performance benchmarking
- Memory usage analysis

---

## Task 5: Development System Cohesion
**Priority:** Medium
**Effort:** ~30 hours
**Phase:** Integration

### Files to Modify:

#### 5.1 src/development/DevelopmentSystem.hpp (High Priority)
- **Lines:** 1-55
- **Changes:**
  - Create abstract base `DevelopmentProcess`
  - Make all development classes inherit from `DevelopmentProcess`
  - Define `DevelopmentStage` enum
  - Create unified stage management
  - Integrate with developmental progression

#### 5.2 src/development/Synaptogenesis.hpp (Medium Priority)
- **Lines:** 1-31
- **Changes:**
  - Make `Synaptogenesis` inherit from `DevelopmentProcess`
  - Remove duplicate rate management
  - Integrate with developmental stage
  - Implement stage-specific functionality

#### 5.3 src/development/Maturation.hpp (Medium Priority)
- **Lines:** 1-32
- **Changes:**
  - Make `Maturation` inherit from `DevelopmentProcess`
  - Remove duplicate rate management
  - Integrate with developmental stage
  - Implement stage-specific functionality

#### 5.4 src/development/Pruning.hpp (Medium Priority)
- **Lines:** 1-34
- **Changes:**
  - Make `Pruning` inherit from `DevelopmentProcess`
  - Remove duplicate rate management
  - Integrate with developmental stage
  - Implement stage-specific functionality

#### 5.5 Create src/development/DevelopmentProcess.hpp (New File)
- **Lines:** 1-80
- **Changes:**
  - Create abstract development process base class
  - Define common development interface
  - Include stage management
  - Provide update mechanism

#### 5.6 Create src/development/DevelopmentalStages.hpp (New File)
- **Lines:** 1-100
- **Changes:**
  - Define all developmental stages
  - Implement stage progression
  - Provide stage-specific functionality
  - Integrate with development system

### Post-Modification Verification:
- Run development tests to ensure functionality preserved
- Verify all developmental processes still work correctly
- Test developmental stages
- Performance benchmarking

---

## Implementation Schedule

### Week 1-2: Plasticity System
1. Task 1.1-1.5: Consolidate plasticity systems
2. Task 1.6: Create PlasticitySystem
3. Unit testing and verification

### Week 3-4: Memory System
1. Task 2.1-2.9: Standardize memory interface
2. Memory system integration
3. Performance optimization

### Week 5-6: Neuromodulation System
1. Task 3.1-3.7: Create neuromodulation framework
2. Neuromodulation system integration
3. Signal interaction testing

### Week 7-8: Cognition System
1. Task 4.1-4.8: Integrate cognitive systems
2. Cognitive system unification
3. Performance benchmarking

### Week 9-10: Development System
1. Task 5.1-5.6: Unify development processes
2. Development system integration
3. Final testing

### Week 11-12: Integration and Validation
1. Cross-system testing
2. Performance optimization
3. Documentation updates
4. Final code review

## Quality Assurance

### Code Review Checklist:
- [ ] All inheritance hierarchies correct
- [ ] No duplicate functionality
- [ ] Backward compatibility maintained
- [ ] Performance benchmarks met
- [ ] Memory leaks detected and fixed
- [ ] Test coverage maintained
- [ ] Documentation updated

### Testing Requirements:
- All existing tests pass
- New unit tests for refactored components
- Integration tests for cross-system functionality
- Performance regression tests
- Stress testing

## Deliverables

### Phase 1 Deliverables:
- Unified plasticity system
- Standardized memory interface
- Maintained backward compatibility

### Phase 2 Deliverables:
- Neuromodulation framework
- Integrated cognitive systems
- Enhanced modularity

### Phase 3 Deliverables:
- Cohesive development system
- Performance optimizations
- Comprehensive documentation

## Risk Mitigation

### Technical Risks:
- **Backward Compatibility:** Maintain existing APIs through adapters
- **Performance Impact:** Benchmark and optimize throughout
- **Testing Complexity:** Implement incremental testing approach

### Project Risks:
- **Timeline:** Phase-based approach with clear milestones
- **Resource Allocation:** Focus on high-impact, low-effort changes first
- **Code Quality:** Strict code review at each phase

This detailed task list provides a comprehensive roadmap for refactoring the NLM codebase, addressing all identified duplication and structural issues while maintaining backward compatibility and improving system cohesion.