# NLM Architecture

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 4: EMERGING COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │ Social  │       ││
│  │  │         │  │         │  │         │  │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### 5. Development Layer
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### 6. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### 7. Memory Layer (Phase 4)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

### 8. Prediction Layer (Phase 4)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

### 9. Cognitive Layer (Phase 4)
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

### 10. Sensory Layer
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

### 11. Motor Layer
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### 12. Environment Layer
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 13. Experiments Layer
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

## Data Flow

### Phase 2: Neural Computation
```
Sensory Input (test signals)
      ↓
Brain.receiveSensoryInput()
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Plasticity (STDP, Hebbian)
      ↓
Structural Plasticity (synaptogenesis/pruning)
```

### Phase 3: World Interaction
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
AgentBrain.processSensoryInput()
      ↓
Brain.receiveSensoryInput() → Sensory Neurons
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Motor Decoding → MotorCommand
      ↓
AgentBrain.decodeMotorCommand()
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT (reward, success)
      ↓
Reward Prediction Error
      ↓
Neuromodulation (dopamine-like signal)
      ↓
Eligibility Trace Update
      ↓
Plasticity (STDP, Hebbian, Reward-Modulated)
      ↓
Structural Plasticity (synaptogenesis/pruning)
      ↓
Development System (stage progression)
```

### Phase 4: Emerging Cognition
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT
      ↓
ATTENTION (competitive selection)
      ↓
NEURAL PROCESSING
      ↓
PREDICTION (temporal sequence learning)
      ↓
WORKING MEMORY (persistent activity)
      ↓
EPISODIC MEMORY (experience storage)
      ↓
CONCEPT FORMATION (pattern discovery)
      ↓
PLANNING (multi-step prediction)
      ↓
SELF-MODEL (sensorimotor learning)
      ↓
MOTOR ACTION
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT
      ↓
REWARD / ERROR
      ↓
NEUROMODULATION
      ↓
PLASTICITY
      ↓
SOCIAL LEARNING (observation/imitation)
```

## Agent-Brain Interface

Phase 3/4 introduces the AgentBrain class that connects the NLM brain to the world:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Implementation Status

### Phase 2: Neural Core - ✅ COMPLETE

**Components:**
- ✅ LIF Neuron dynamics with event-driven spike processing
- ✅ Synaptic transmission with delays and excitatory/inhibitory types
- ✅ Structural plasticity (synaptogenesis and pruning)
- ✅ Spike-timing-dependent plasticity (STDP)
- ✅ Hebbian learning rules

### Phase 3: World Interaction - ✅ COMPLETE

**Components:**
- ✅ Sensory input processing (vision, touch, internal, proprioception)
- ✅ Motor output decoding and action selection
- ✅ Reward prediction error computation
- ✅ Developmental stage progression
- ✅ Novelty detection and curiosity

### Phase 4: Cognitive Systems - ⚠️ PARTIAL INTEGRATION

**Components (status):**

| System | Status | Integration |
|--------|--------|-------------|
| Neural Working Memory | ✅ IMPLEMENTED | ❌ Connected to brain loop (Step 4 in Brain::step()) |
| Neural Episodic Memory | ✅ IMPLEMENTED | ❌ Connected to brain loop (Step 5 in Brain::step()) |
| Neural Associative Memory | ✅ IMPLEMENTED | ❌ Connected to brain loop (Step 6.5 in Brain::step()) |
| Concept Formation | ✅ IMPLEMENTED | ❌ Never instantiated or used |
| Spatial Representation | ✅ IMPLEMENTED | ❌ Never instantiated or used |
| Temporal Relation | ✅ IMPLEMENTED | ❌ Never instantiated or used |

### Phase 4: Prediction System - ⚠️ PARTIAL INTEGRATION

**Components (status):**

| Component | Status | Integration |
|-----------|--------|-------------|
| Neural Prediction | ✅ IMPLEMENTED | ❌ Connected to brain loop (Step 8 in Brain::step()) |
| ActionConsequencePredictor | ✅ IMPLEMENTED | ❌ Never used in brain loop |
| PredictionErrorSignal | ✅ IMPLEMENTED | ❌ Limited use, exists but disconnected |

### Phase 4: Cognitive Mechanisms - ❌ DISCONNECTED

**Components:**
- ❌ NeuralPlanner - exists but never instantiated
- ❌ SelfModel - exists but never instantiated  
- ❌ SocialLearning - exists but never instantiated
- ❌ SpatialRepresentation - exists but never instantiated
- ❌ TemporalRelation - exists but never instantiated

### Phase 5: Performance Infrastructure - ⚠️ PARTIAL

**Components (status):**

| Component | Status | Integration |
|-----------|--------|-------------|
| Memory pools | ✅ IMPLEMENTED | ⚠️ Not integrated into Brain |
| Event queues | ✅ IMPLEMENTED | ⚠️ Not integrated into Brain |
| Sparse connectivity | ✅ IMPLEMENTED | ⚠️ Not integrated into Brain |
| Parallel processing | ✅ IMPLEMENTED | ⚠️ Not integrated into Brain |
| SIMD vectorization | ✅ IMPLEMENTED | ⚠️ Not integrated into Brain |
| Checkpoint system | ✅ IMPLEMENTED | ❌ Not connected to Brain |

### Phase 6: Integration Status - ❌ INCOMPLETE

**Key Integration Issues:**

1. **Memory Integration**: 
   - Working memory connected (Step 4)
   - Episodic memory connected (Step 5) 
   - Associative memory partially connected (Step 6.5)
   - Semantic and procedural memory: empty implementations

2. **Neuromodulation Integration**:
   - Dopamine: Connected (Step 5) - affects neural excitability and plasticity
   - Curiosity: Connected (Step 5) - drives exploration behavior
   - Novelty: Connected (Step 5) - promotes structural plasticity
   - Serotonin/Norepinephrine/Acetylcholine: Stubs only, not integrated

3. **Prediction Integration**:
   - Prediction system connected (Step 8) but requires sensory input
   - Prediction error computed in AgentBrain but doesn't update predictions

4. **Development Integration**:
   - Development system affects plasticity rates (Step 13)
   - Limited effects: only plasticity rates, not excitability or attention

5. **Persistence Integration**:
   - CheckpointSystem implemented but not connected
   - Brain::save() and Brain::load() are stubs returning false

6. **Replay and Consolidation**:
   - Episodic memory replay stub (getEpisodesForReplay())
   - No sleep/rest cycle implementation
   - Memory consolidation not integrated

### Current Brain::step() Implementation

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1-3: Neural processing (UNCHANGED) ✅
    // 4: Working memory update ✅
    // 5: Neuromodulation application ✅
    // 6: Plasticity rules ✅
    // 6.5: Associative memory integration ⚠️
    // 8: Prediction system integration ⚠️
    // 13: Development effects ⚠️
    // 14-15: Memory consolidation and checkpointing ⚠️
}
```

### What's Missing from Current Implementation

**Critical Integration Gaps:**
1. ✅ **Neural substrate** works but lacks integration with higher cognition
2. ❌ **Sensory → Working Memory**: No working memory storage of sensory input
3. ❌ **Working Memory → Attention**: No attention mechanisms
4. ❌ **Attention → Concept Formation**: No concept formation from attention
5. ❌ **Concept Formation → Planning**: No planning based on concepts
6. ❌ **Planning → Action Selection**: Neural planner exists but not used
7. ❌ **Action → Self-model Update**: No self-model learning
8. ❌ **Experience → Episodic Memory**: No episodic memory storage
9. ❌ **Episodic Memory → Consolidation**: No replay mechanisms
10. ❌ **Sleep/Consolidation**: No consolidation during rest

### Integration Priority (Phase 6)

**Priority 1: CONNECT EXISTING SYSTEMS**
1. Connect WorkingMemory to sensory processing
2. Connect EpisodicMemory to experience logging
3. Connect PredictionSystem to sensory processing
4. Connect NeuralPlanner to action selection
5. Connect ConceptFormation to experience processing

**Priority 2: IMPLEMENT MISSING MECHANISMS**
1. Implement Brain::save() and Brain::load() using CheckpointSystem
2. Implement sleep/rest cycle with memory consolidation
3. Implement replay mechanism
4. Implement full dopamine effects on plasticity
5. Implement ACh effects on attention/memory

**Priority 3: VALIDATE INTEGRATION**
1. Test memory retention over time
2. Test prediction accuracy improvement
3. Test continual learning
4. Test developmental progression
5. Test multi-system interaction

### Summary

The NLM architecture has sophisticated components but functions as a **basic neural simulator** rather than an integrated artificial brain. The components exist but they don't interact cohesively. Phase 6 must focus on **integration over new features**.

**Primary Goal**: Make existing systems work together as a coherent whole, not add more disconnected components.

### Current Functionality (What Works)

✅ **Neural computation**: LIF neurons, spikes, synapses
✅ **Basic learning**: STDP, Hebbian, structural plasticity
✅ **World interaction**: Sensory-motor loop with reward modulation
✅ **Basic memory**: Working memory with persistent activity
✅ **Basic neuromodulation**: Dopamine, curiosity, novelty
✅ **Development**: Structural plasticity changes with age
✅ **Performance**: Event-driven architecture with optimization infrastructure

### Current Limitations (What Doesn't Work)

❌ **Cognitive integration**: Planning, attention, concept formation disconnected
❌ **Memory integration**: Episodic, semantic, procedural memory not connected
❌ **Prediction integration**: Prediction system not trained or used
❌ **Advanced neuromodulation**: Only dopamine implemented, others are stubs
❌ **Persistence**: Checkpoint system not connected
❌ **Replay/consolidation**: No sleep cycle or memory consolidation

### Recommendations

1. **First**: Connect all existing systems to create a coherent brain loop
2. **Second**: Implement missing neuromodulators (ACh, NE, 5-HT)
3. **Third**: Complete memory consolidation and replay systems
4. **Fourth**: Implement full checkpoint persistence
5. **Fifth**: Add validation tests for all integrated systems

## Phase 4 Key Innovations

### Prediction System
- Learns temporal sequences from experience
- Predicts action consequences
- Multi-step prediction capability
- Error signals modulate learning

### Working Memory
- Persistent neural activity
- Recurrent connections for maintenance
- Competition for selective retention
- Neural-grounded (not a vector store)

### Episodic Memory
- Experience encoded as neural patterns
- Pattern completion retrieval
- Temporal and spatial indexing
- Replay during consolidation

### Concept Formation
- Discovers patterns without labels
- Prototype formation through averaging
- Stability measurement
- Category hints from properties

### Attention
- Lateral inhibition competition
- Winner-take-more dynamics
- Top-down and bottom-up bias
- NOT Transformer QKV attention

### Planning
- Uses learned predictions
- Evaluates potential futures
- Selects actions leading to goals
- Adapts based on outcomes

### Self-Model
- Learns body schema
- Predicts action consequences
- Self vs external distinction
- Capability representation

### Social Learning
- Observes other agents
- Learns action effects
- Imitation capability
- Simple communication signals