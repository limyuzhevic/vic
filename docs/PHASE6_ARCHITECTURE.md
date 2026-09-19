# NLM/熙然 Phase 6 Architecture Documentation

## Overview

Phase 6 partially integrates all previously separate systems into a coherent artificial brain. **CURRENT STATUS: 35.8% INTEGRATION (43/120 achievable)**. The architecture follows a closed-loop design where neural activity, plasticity, memory, neuromodulation, and behavior influence each other, but most systems remain disconnected from the main brain loop.

**Key Reality:** The brain has the *anatomy* of a cognitive system but lacks the *integration* that makes it function as a coherent artificial brain.

## Brain Loop Architecture

The main simulation loop (`Brain::step()`) implements the partially integrated brain simulation:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes (deliver synaptic input) ✅
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics) ✅
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // 3. Detect spikes and schedule spike events ✅
    // ... spike detection and scheduling ...
    
    // 4. Update working memory ❌
    // pImpl->workingMemory->update(pImpl->timestep); // NOT CALLED
    
    // 5. Apply neuromodulation effects ⚠️
    // - Dopamine affects neural excitability (limited)
    // - Novelty detection updates ✅
    // - Curiosity computation ✅
    
    // 6. Apply plasticity rules (STDP and Hebbian) ✅
    // with neuromodulation modulation
    
    // 7. Update episodic memory ❌
    // Episodic memory capture/storing NOT IMPLEMENTED
    
    // 8. Update prediction system ❌
    // Prediction system NOT CALLED
    
    // 9. Update attention system ❌
    // Attention system NOT CALLED
    
    // 10. Update concept formation ❌
    // Concept formation NOT CALLED
    
    // 11. Apply structural plasticity (synaptogenesis/pruning) ✅
    // Periodic structural plasticity working
    
    // 12. Replay important memories ❌
    // Replay mechanism NOT IMPLEMENTED
    
    // 13. Apply development effects ⚠️
    // Development affects plasticity rates (limited)
    
    // 14. Memory consolidation ❌
    // Consolidation NOT IMPLEMENTED
    
    // 15. Checkpoint management ❌
    // Checkpoints NOT CONNECTED
}
```

## Current Integration Status by System

### Memory Systems - MOSTLY DISCONNECTED

#### Working Memory (NeuralWorkingMemory)

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp`

**Mechanisms:**
- ✅ Persistent neural activity for maintaining information
- ✅ Recurrent connections for maintenance
- ✅ Competition among memory traces
- ✅ Decay of weak traces

**Current Status:**
- ❌ **NOT INTEGRATED**: `Brain::getWorkingMemory()` returns nullptr
- ❌ **NEVER UPDATED**: Not called in `Brain::step()`
- ❌ **NO SENSOR INPUT**: Never receives sensory data
- ❌ **NO ACTION OUTPUT**: Never influences action selection

#### Episodic Memory (NeuralEpisodicMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp`

**Mechanisms:**
- ✅ Stores experiences as neural activity patterns
- ✅ Temporal ordering through timestamps
- ✅ Relevance-weighted recall
- ✅ Replay during offline periods

**Current Status:**
- ❌ **NOT INTEGRATED**: Never updated with experiences
- ❌ **NO CAPTURE**: Not called in brain loop
- ❌ **NO REPLAY**: Replay mechanism not implemented
- ❌ **NO CONSOLIDATION**: No long-term storage

#### Associative Memory (NeuralAssociativeMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp` (in same file)

**Mechanisms:**
- ✅ Hebbian associations between patterns
- ✅ Spreading activation for recall
- ✅ Strength modification through experience

**Current Status:**
- ❌ **EMPTY IMPLEMENTATION**: Not actually implemented
- ❌ **NO INTEGRATION**: Not connected to brain loop

#### Novelty

**Location:** `src/neuromodulation/Novelty.hpp/cpp`

**Mechanisms:**
- ✅ Novelty detection from sensory patterns
- ✅ Comparison with previous patterns
- ✅ Decay over time

**Integration:**
- ✅ **WORKING**: Detected from vision changes in AgentBrain
- ✅ **USED**: Used for curiosity computation
- ✅ **STORED**: Stored for later use

### Prediction System - DISCONNECTED

**Location:** `src/prediction/PredictionSystem.hpp/cpp`

**Mechanisms:**
- ✅ Next state prediction
- ✅ Prediction error computation
- ✅ Confidence tracking
- ✅ History tracking

**Integration:**
- ❌ **NOT USED**: Never called in brain loop or agent
- ❌ **NO TRAINING**: Never processes sensory data
- ❌ **NO ATTENTION**: Doesn't affect attention
- ❌ **NO PLANNING**: Doesn't influence action selection

### Cognition Systems - COMPLETELY DISCONNECTED

#### Neural Planner

**Location:** `src/cognition/NeuralPlanner.hpp/cpp`

**Mechanisms:**
- ✅ Multi-step action sequence evaluation
- ✅ Planning depth configuration
- ✅ Action quality from experience
- ✅ Plan success tracking

**Integration:**
- ❌ **NEVER INSTANTIATED**: Not created anywhere in code
- ❌ **NEVER USED**: Not called by AgentBrain or Brain
- ❌ **NO GOALS**: Doesn't connect to motivation systems

#### Concept Formation

**Location:** `src/cognition/ConceptFormation.hpp/cpp`

**Mechanisms:**
- ✅ Pattern clustering through similarity
- ✅ Prototype formation
- ✅ Instance tracking
- ✅ Stability measurement

**Integration:**
- ❌ **NEVER INSTANTIATED**: Not created in any system
- ❌ **NO PROCESSING**: Never receives sensory patterns
- ❌ **NO GENERALIZATION**: Doesn't create abstract concepts

#### Attentional Selection

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp` (AttentionalSelection class)

**Mechanisms:**
- ✅ Competition between neural populations
- ✅ Winner-take-more dynamics
- ✅ Top-down and bottom-up bias
- ✅ Inhibition and excitation

**Integration:**
- ❌ **NOT USED**: Never processes working memory competitors
- ❌ **NO SENSOR INPUT**: Doesn't receive from perception
- ❌ **NO ACTION OUTPUT**: Doesn't influence motor systems

### Development System - LIMITED INTEGRATION

**Location:** `src/development/`

**Mechanisms:**
- ✅ Developmental stages (Initial → CriticalPeriod → Maturation → Adult → Aging)
- ✅ Plasticity modification by stage
- ✅ Auto-advance based on age

**Integration:**
- ✅ **WORKING**: Updated every 1000 steps (structural plasticity)
- ⚠️ **LIMITED**: Only affects structural plasticity rates
- ❌ **NO NEURAL EXCITABILITY**: Doesn't affect firing properties
- ❌ **NO MEMORY EFFECTS**: Doesn't influence memory systems

### Performance Infrastructure - MOSTLY NOT INTEGRATED

**Location:** `src/performance/`

| Component | Status | Integration |
|-----------|--------|-------------|
| Memory pools | ✅ Implemented | ❌ Not used by Brain |
| Event queues | ✅ Implemented | ❌ Not used by Brain |
| Sparse connectivity | ✅ Implemented | ❌ Not used by Brain |
| Parallel processing | ✅ Implemented | ❌ Not used by Brain |
| SIMD vectorization | ✅ Implemented | ❌ Not used by Brain |
| Checkpoint system | ✅ Defined | ❌ Not connected to Brain |

## Data Flow Analysis

### Current Working Flow

```
WORLD
  ↓
SENSORY INPUT (SimpleWorld.observe)
  ↓
SENSORY NEURONS (receiveSensoryInput)
  ↓
LIF DYNAMICS (neuron->stepLIF)
  ↓
SPIKE DETECTION
  ↓
SYNAPTIC TRANSMISSION
  ↓
NEUROMODULATION (dopamine scales STDP)
  ↓
STDP + HEBBIAN PLASTICITY
  ↓
STRUCTURAL PLASTICITY (every 100 steps)
  ↓
MOTOR NEURONS (decodeMotorCommand)
  ↓
ACTION SELECTION (AgentBrain)
  ↓
WORLD INTERACTION
```

### Missing Integration Links

```
SENSORY INPUT
    ↓
SENSORY NEURONS
    ↓
??? (gap - no working memory storage) ← Working memory disconnected
    ↓
??? (gap - no prediction processing) ← Prediction system disconnected
    ↓
??? (gap - no concept formation) ← Cognition systems disconnected
    ↓
MOTOR NEURONS
    ↓
ACTION SELECTION
    ↓
??? (gap - no planning integration) ← No goal-directed behavior
    ↓
WORLD INTERACTION
```

### Current Working Flow

```
WORLD
  ↓
SENSORY INPUT (SimpleWorld.observe)
  ↓
SENSORY NEURONS (receiveSensoryInput)
  ↓
LIF DYNAMICS (neuron->stepLIF)
  ↓
SPIKE DETECTION
  ↓
SYNAPTIC TRANSMISSION
  ↓
NEUROMODULATION (dopamine scales STDP)
  ↓
STDP + HEBBIAN PLASTICITY
  ↓
STRUCTURAL PLASTICITY (every 100 steps)
  ↓
MOTOR NEURONS (decodeMotorCommand)
  ↓
ACTION SELECTION (AgentBrain)
  ↓
WORLD INTERACTION
```

### Missing Integration Links

```
SENSORY INPUT
    ↓
SENSORY NEURONS
    ↓
??? (gap - no working memory storage) ← Working memory disconnected
    ↓
??? (gap - no prediction processing) ← Prediction system disconnected
    ↓
??? (gap - no concept formation) ← Cognition systems disconnected
    ↓
MOTOR NEURONS
    ↓
ACTION SELECTION
    ↓
??? (gap - no planning integration) ← No goal-directed behavior
    ↓
WORLD INTERACTION
```

## Architecture Implications

### Current Capabilities
1. **Neural Core**: ✅ LIF neurons with spikes and synaptic transmission
2. **Basic Plasticity**: ✅ STDP, Hebbian, structural plasticity
3. **Embodiment**: ✅ Sensory input and motor output loop
4. **Simple Learning**: ✅ Reward-modulated STDP via dopamine
5. **Development**: ⚠️ Age-based structural plasticity

### Critical Integration Gaps
1. **Memory Gap**: No experience storage or retrieval
2. **Cognition Gap**: No planning, concept formation, or goal-directed behavior
3. **Prediction Gap**: No future-oriented processing
4. **Neuromodulation Gap**: Limited to STDP scaling only
5. **Persistence Gap**: No life history or state persistence

### Design Recommendations for Phase 6 Completion

**Priority 1: Connect Existing Systems**
1. `Brain::getWorkingMemory()` → Implement and connect
2. `Brain::getPredictionSystem()` → Implement and connect
3. `Brain::save()/load()` → Connect to CheckpointSystem
4. `Brain::getEpisodicMemory()` → Implement and connect

**Priority 2: Fix Implementation**
1. Complete empty memory implementations (semantic, procedural)
2. Implement ACh, NE, 5-HT systems
3. Complete prediction system
4. Implement attention and concept formation integration

**Phase 6 Status Summary**
- **What we have**: Sophisticated architecture with many components
- **What we lack**: Integration that makes it function as a coherent artificial brain
- **Goal**: Make existing systems work together rather than add more components

**The primary challenge of Phase 6 is integration, not new features.**

## Memory Systems

### Working Memory (NeuralWorkingMemory)

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp`

**Mechanisms:**
- Persistent neural activity for maintaining information
- Recurrent connections for maintenance
- Competition among memory traces
- Decay of weak traces

**Integration:**
- Stores firing neurons each step
- Updated each simulation step
- Influences attention competition
- Decays over time if not reinforced

### Episodic Memory (NeuralEpisodicMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp`

**Mechanisms:**
- Stores experiences as neural activity patterns
- Temporal ordering through timestamps
- Relevance-weighted recall
- Replay during offline periods

**Integration:**
- Episodes captured every 10 simulation steps
- Replay triggered every 100 steps
- Consolidation every 1000 steps
- Influences future behavior through replay

### Associative Memory (NeuralAssociativeMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp` (in same file)

**Mechanisms:**
- Hebbian associations between patterns
- Spreading activation for recall
- Strength modification through experience

**Integration:**
- Associated with episodic memory
- Updated during experience encoding

## Neuromodulation Systems

### Dopamine

**Location:** `src/neuromodulation/Neuromodulator.hpp/cpp`

**Mechanisms:**
- Reward prediction error computation
- Phasic and tonic signaling
- Behavioral vigor modulation

**Integration:**
- Affects neural excitability (dopamine level modulates membrane potential)
- Scales STDP learning rates
- Modulates plasticity through eligibility traces
- Affects action selection indirectly through learning

### Curiosity

**Location:** `src/neuromodulation/Curiosity.hpp/cpp`

**Mechanisms:**
- Combines novelty and prediction error
- Exploration drive computation
- Decay over time

**Integration:**
- Used by AgentBrain for exploration decisions
- Stored in AgentBrain for action selection
- Affects motor command decoding

### Novelty

**Location:** `src/neuromodulation/Novelty.hpp/cpp`

**Mechanisms:**
- Novelty detection from sensory patterns
- Comparison with previous patterns
- Decay over time

**Integration:**
- Computed from vision changes in AgentBrain
- Used for curiosity computation
- Stored for later use

## Prediction System

**Location:** `src/prediction/PredictionSystem.hpp/cpp`

**Mechanisms:**
- Next state prediction
- Prediction error computation
- Confidence tracking
- History tracking

**Integration:**
- Called in brain loop (placeholder for full implementation)
- Prediction error affects curiosity
- Future: will affect attention and planning

## Cognition Systems

### Neural Planner

**Location:** `src/cognition/NeuralPlanner.hpp/cpp`

**Mechanisms:**
- Multi-step action sequence evaluation
- Planning depth configuration
- Action quality from experience
- Plan success tracking

**Integration:**
- Initialized in Brain
- Future: will be used for goal-directed behavior

### Concept Formation

**Location:** `src/cognition/ConceptFormation.hpp/cpp`

**Mechanisms:**
- Pattern clustering through similarity
- Prototype formation
- Instance tracking
- Stability measurement

**Integration:**
- Initialized in Brain
- Future: will process sensory patterns for generalization

### Attentional Selection

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp` (AttentionalSelection class)

**Mechanisms:**
- Competition between neural populations
- Winner-take-more dynamics
- Top-down and bottom-up bias
- Inhibition and excitation

**Integration:**
- Processes working memory competitors
- Updated each step
- Applies excitation to winners, inhibition to losers

## Development System

**Location:** `src/development/DevelopmentSystem.hpp/cpp`

**Mechanisms:**
- Developmental stages (Initial → CriticalPeriod → Maturation → Adult → Aging)
- Plasticity modification by stage
- Auto-advance based on age

**Integration:**
- Updated every 1000 steps
- Modifies structural plasticity rates
- Affects neural excitability indirectly through plasticity

## Checkpoint System

**Location:** `src/performance/CheckpointSystem.hpp`

**Mechanisms:**
- Brain state serialization
- Neuron and synapse state saving
- Checkpoint manager for automated saves

**Integration:**
- `Brain::save()` writes checkpoint
- `Brain::load()` reads checkpoint
- CheckpointManager handles periodic saves

## Data Flow

### Sensory Processing
```
World → SensoryPercept → AgentBrain::processSensoryInput()
                              ↓
                        Brain::receiveSensoryInput()
                              ↓
                        Sensory neurons receive current
                              ↓
                        LIF dynamics integrate input
```

### Action Selection
```
Motor neuron activity → AgentBrain::decodeMotorCommand()
                              ↓
                        Curiosity exploration check
                              ↓
                        MotorCommand selected
                              ↓
                        World::applyAction()
```

### Memory Formation
```
Brain step (spikes) → Working memory stores firing neurons
                              ↓
                        Every 10 steps: Episodic memory captures state
                              ↓
                        Every 100 steps: Replay of important episodes
                              ↓
                        Every 1000 steps: Consolidation
```

### Learning
```
Reward from environment → AgentBrain::applyRewardModulation()
                              ↓
                        Dopamine computed (prediction error)
                              ↓
                        Synapses with eligibility traces updated
                              ↓
                        STDP weights modulated by dopamine
```

## System Independence and Coupling

### Loose Coupling (can function somewhat independently):
- Working memory (stores current activity)
- Episodic memory (stores experiences)
- Prediction system (computes errors)
- Concept formation (organizes patterns)

### Tight Coupling (strongly interconnected):
- Neurons ↔ Synapses ↔ Plasticity
- Dopamine ↔ Eligibility traces ↔ Synaptic weights
- Novelty ↔ Curiosity ↔ Exploration
- Development ↔ Plasticity rates ↔ Structural plasticity

## Extension Points

### Where to add new mechanisms:

1. **New plasticity rules**: Add to `Brain::step()` after STDP/Hebbian
2. **New neuromodulators**: Add to `Brain::Impl` and update `Brain::step()`
3. **New memory types**: Add to `Brain::Impl` and update `Brain::step()`
4. **New cognition**: Add to `Brain::Impl` and connect to perception/action

### Key integration hooks:

1. `receiveSensoryInput()` - Entry point for perception
2. `produceAction()` - Exit point for action
3. `step()` - Main loop where all systems update
4. `applyRewardModulation()` - Where learning signals apply
5. `save()/load()` - State persistence
