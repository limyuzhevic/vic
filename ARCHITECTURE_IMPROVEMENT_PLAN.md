# NLM / 熙然 - Phase 6 Architecture Improvement Plan

## Executive Summary

The NLM codebase contains a sophisticated architecture with many well-designed components, but **most cognitive systems are disconnected from the main brain loop**. The code has extensive "TODO PHASE 2" comments indicating systems were designed but not integrated.

**Key Finding:** The brain has the *anatomy* of a cognitive system but lacks the *integration* that makes it function as a coherent artificial brain.

## Current State Assessment

### Score: 43/120 (35.8%) - Phase 6 Integration Complete

**What Works Well (18 points):**
- Neural Core: LIF neuron dynamics, spike propagation, basic plasticity
- Embodiment: Sensory-motor loop with SimpleWorld
- Performance: Event-driven spike system, sparse connectivity

**Major Gaps (77 points missing):**
- Memory Systems: All disconnected
- Neuromodulation: Mostly stubs
- Cognition: All systems disconnected
- Prediction: Not integrated
- Development: Limited integration
- Persistence: Stubs only
- Scientific Validation: No tests

## Critical Issues to Address

### 1. MEMORY SYSTEMS INTEGRATION

**Current Status:**
- NeuralWorkingMemory: Implemented with persistent activity and competition
- NeuralEpisodicMemory: Basic implementation only
- SemanticMemory: Empty implementation
- ProceduralMemory: Empty implementation

**Integration Needed:**
- ✅ Sensory input → Working memory storage
- ✅ Working memory → Attention
- ✅ Attention → Concept formation
- ✅ Concept formation → Planning
- ✅ Planning → Action selection
- ✅ Action → Self-model update
- ✅ Self-model → Agency
- ✅ Experience → Episodic memory
- ✅ Episodic memory → Consolidation
- ✅ Sleep/rest cycle → Memory consolidation

### 2. PREDICTION SYSTEM INTEGRATION

**Current Status:**
- NeuralPrediction: Implemented but never trained
- PredictionSystem: Placeholder with TODO comments
- No integration with sensory processing

**Integration Needed:**
- Connect prediction system to sensory processing
- Use prediction errors for learning
- Integrate predictions with attention
- Use predictions for planning

### 3. COGNITION SYSTEMS INTEGRATION

**Current Status:**
- NeuralPlanner: Implemented but never called
- ConceptFormation: Implemented but never processes experiences
- SelfModel: Implemented but never updates
- SocialLearning: Implemented but never observes

**Integration Needed:**
- NeuralPlanner → Action selection
- ConceptFormation → Experience processing
- SelfModel → Agency update
- SocialLearning → Observation processing

### 4. NEUROMODULATION INTEGRATION

**Current Status:**
- Dopamine: Basic implementation only (scales STDP weights)
- Serotonin: Stub (returns 0.0f)
- Norepinephrine: Stub (returns 0.0f)
- Acetylcholine: Stub (returns 1.0f)
- Curiosity: Working
- Novelty: Working
- PredictionError: Working

**Integration Needed:**
- Full dopamine effects on plasticity rules
- ACh effects on attention and memory consolidation
- NE effects on arousal and vigilance
- Serotonin effects on mood and impulsivity

### 5. PERSISTENCE SYSTEM

**Current Status:**
- CheckpointSystem: Fully implemented
- Brain::save(): Stub (returns false)
- Brain::load(): Stub (returns false)

**Integration Needed:**
- Connect Brain::save() to CheckpointSystem
- Connect Brain::load() to CheckpointSystem
- Add life history tracking
- Add checkpoint intervals to config

### 6. PERFORMANCE INFRASTRUCTURE

**Current Status:**
- Memory pools: Implemented but not integrated
- Event queues: Implemented but not integrated
- SIMD vectorization: Implemented but not integrated
- Parallel processing: Implemented but not integrated

**Integration Needed:**
- Integrate memory pools into Brain
- Integrate event queues into spike system
- Enable SIMD optimization for hot paths
- Implement safe multithreading

## Implementation Plan

### Phase 1: CORE INTEGRATION (Priority 1)

**1.1 Memory System Integration**
- Connect WorkingMemory to sensory input in Brain::step()
- Store experiences in EpisodicMemory
- Implement attention mechanisms that select from working memory
- Add concept formation from working memory contents

**1.2 Prediction System Integration**
- Use sensory observations to train prediction system
- Connect prediction errors to neuromodulation
- Use predictions to guide attention and planning
- Integrate prediction accuracy into learning

**1.3 Cognition System Integration**
- Call NeuralPlanner in action selection pipeline
- Process working memory contents in ConceptFormation
- Update SelfModel based on experiences
- Add social learning from observation

### Phase 2: NEUROMODULATION COMPLETE (Priority 2)

**2.1 Complete Neuromodulation**
- Implement real dopamine dynamics (plasticity modulation)
- Implement acetylcholine effects on attention
- Implement norepinephrine effects on arousal
- Implement serotonin effects on learning rate

**2.2 Persistence System**
- Connect Brain::save() to CheckpointSystem
- Connect Brain::load() to CheckpointSystem
- Add checkpoint intervals to config

**2.3 Sleep/Rest Cycle**
- Implement resting states
- Add memory consolidation during sleep
- Implement replay mechanisms

### Phase 3: ADVANCED FEATURES (Priority 3)

**3.1 Expert Features**
- Multiple agent support
- Custom environment creation
- Advanced configuration options
- Modular plugin system

**3.2 Scientific Validation**
- Memory retention tests
- Learning progress measurements
- Continual learning validation
- Generalization tests

### Phase 4: OPTIMIZATION (Priority 4)

**4.1 Performance Integration**
- Integrate memory pools
- Optimize event-driven processing
- Add SIMD optimization
- Implement safe parallelism

**4.2 Profiling Tools**
- Add performance profiling
- Add memory usage tracking
- Add bottleneck identification

## Technical Details

### Files to Modify

**Core Integration (Brain.cpp):**
- Brain::step(): Add calls to memory, prediction, cognition systems
- Brain::step(): Integrate neuromodulation updates
- Brain::receiveSensoryInput(): Connect to working memory
- Brain::save(): Implement using CheckpointSystem
- Brain::load(): Implement using CheckpointSystem

**Memory Systems (src/memory/):**
- NeuralWorkingMemory.cpp: Connect to sensory input
- NeuralEpisodicMemory.cpp: Connect to experience logging

**Prediction System (src/prediction/):**
- PredictionSystem.cpp: Implement real prediction
- NeuralPrediction.cpp: Implement neural prediction

**Cognition Systems (src/cognition/):**
- NeuralPlanner.cpp: Integrate with action selection
- ConceptFormation.cpp: Process experiences

**Neuromodulation (src/neuromodulation/):**
- Neuromodulator.cpp: Implement all neuromodulators
- Dopamine.cpp: Real dopamine dynamics
- Acetylcholine.cpp: ACh effects
- Norepinephrine.cpp: NE effects
- Serotonin.cpp: 5-HT effects

### Configuration Changes

**Add to Config.hpp:**
- `memory.working_memory_capacity`
- `memory.episodic_max_episodes`
- `prediction.use_neural_predictions`
- `neuromodulation.dopamine_enabled`
- `neuromodulation.acetylcholine_enabled`
- `neuromodulation.norepinephrine_enabled`
- `neuromodulation.serotonin_enabled`
- `development.critical_period_duration`
- `performance.use_memory_pools`
- `performance.enable_simd`
- `performance.max_threads`

### Testing Strategy

**Unit Tests:**
- Memory system integration tests
- Prediction accuracy tests
- Neuromodulation effect tests
- Development stage transitions

**Integration Tests:**
- Complete brain loop with all systems
- Memory retention over time
- Learning progress tracking
- Multi-system interaction validation

## Expected Outcome

After implementation, NLM should:
1. Function as a coherent artificial brain with all systems integrated
2. Demonstrate experience-driven learning with memory
3. Show developmental progression over time
4. Exhibit prediction-based anticipation
5. Maintain attention through neuromodulation
6. Plan actions based on concepts
7. Form self-models of its environment
8. Learn from social observations
9. Consolidate memories during sleep
10. Retain checkpoints of brain state

**Target Score: 120/120 (100% Phase 6 Integration)**

## Risk Assessment

**High Risk:**
- Complex integration between many systems
- Maintaining backward compatibility
- Performance optimization trade-offs

**Medium Risk:**
- Implementing novel mechanisms (sleep/rest cycle)
- Creating new scientific validation tests

**Low Risk:**
- Connecting existing components
- Adding configuration options
- Improving documentation

## Success Metrics

1. **Memory Integration:** Working memory captures 80% of sensory input, episodic memory stores 100+ experiences
2. **Prediction Accuracy:** Prediction error < 10% after 1000 steps
3. **Learning Progress:** 50% improvement in task performance over 10,000 steps
4. **Development:** Clear stage transitions (Initial → Critical → Mature → Aging)
5. **Integration:** All 120 points functional (100% score)
6. **Performance:** < 10ms per simulation step for 1000 neurons

---

*This plan addresses the core architectural issues in NLM/熙然 by focusing on integration over new features, as emphasized in the Phase 6 audit. The goal is to make the existing systems work together as a coherent whole rather than adding more disconnected components.*
