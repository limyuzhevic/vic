# NLM_architecture.md

# NLM Architecture Documentation

## Overview

NLM (熙然, "serene flow") is an experimental artificial developmental brain project that implements a complete, closed-loop brain system where cognition emerges from neural dynamics without symbolic AI.

## Core Architecture

### Brain Layer

The NLM brain consists of multiple interconnected systems that work together as a coherent unit:

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

### System Components

#### 1. Core Layer (src/core)
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

#### 2. Brain Layer (src/brain)
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

#### 3. Dynamics Layer (src/dynamics)
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

#### 4. Plasticity Layer (src/plasticity)
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

#### 5. Development Layer (src/development)
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

#### 6. Neuromodulation Layer (src/neuromodulation)
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Serotonin**: Mood, learning, sleep, attention
- **Norepinephrine**: Arousal, attention, working memory
- **Acetylcholine**: Attention, memory consolidation, sleep
- **Curiosity**: Exploration motivation based on novelty/error
- **Novelty**: Novelty detection
- **PredictionError**: Error signals for learning
- **Reward**: Reward signaling system

#### 7. Memory Layer (src/memory)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

#### 8. Prediction Layer (src/prediction)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

#### 9. Cognitive Layer (src/cognition)
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

#### 10. Sensory Layer (src/sensory)
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

#### 11. Motor Layer (src/motor)
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

#### 12. Environment Layer (src/environment)
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

#### 13. Experiments Layer (src/experiments)
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

## Brain Loop Architecture

The main simulation loop (`Brain::step()`) implements the complete brain simulation:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // 3. Detect spikes and schedule spike events
    // ... spike detection and propagation ...
    
    // 4. Update working memory
    pImpl->workingMemory->update(pImpl->timestep);
    
    // 5. Apply neuromodulation effects
    // - Dopamine affects neural excitability
    // - Novelty detection updates
    // - Curiosity computation
    
    // 6. Apply plasticity rules (STDP and Hebbian)
    // with neuromodulation modulation
    
    // 7. Update episodic memory with current experience
    
    // 8. Update prediction system
    
    // 9. Update attention system
    
    // 10. Update concept formation
    
    // 11. Apply structural plasticity (synaptogenesis/pruning)
    
    // 12. Replay important memories
    
    // 13. Apply development effects
    
    // 14. Memory consolidation
    
    // 15. Checkpoint management
}
```

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
WORKING MEMORY (maintain information)
      ↓
EPISODIC MEMORY (store experiences)
      ↓
CONCEPT FORMATION (pattern discovery)
      ↓
PLANNING (multi-step prediction)
      ↓
SELF-MODEL (predict self)
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

## Memory Systems Integration

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

## Neuromodulation Systems Integration

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

## Prediction System Integration

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

## Cognition Systems Integration

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

## Development System Integration

**Location:** `src/development/DevelopmentSystem.hpp/cpp`

**Mechanisms:**
- Developmental stages (Initial → CriticalPeriod → Maturation → Adult → Aging)
- Plasticity modification by stage
- Auto-advance based on age

**Integration:**
- Updated every 1000 steps
- Modifies structural plasticity rates
- Affects neural excitability indirectly through plasticity

## Checkpoint System Integration

**Location:** `src/performance/CheckpointSystem.hpp`

**Mechanisms:**
- Brain state serialization
- Neuron and synapse state saving
- Checkpoint manager for automated saves

**Integration:**
- `Brain::save()` writes checkpoint
- `Brain::load()` reads checkpoint
- CheckpointManager handles periodic saves

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

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## What NLM IS (Not)

**NLM is NOT:**
- A claim that we can simulate a human brain
- A model that accurately reproduces neural physiology
- A replacement for neuroscience research
- An intelligent system
- A transformer or LLM
- A pretrained AI system

**NLM IS:**
- A computational exploration of brain-inspired architecture
- A testbed for developmental learning hypotheses
- A research platform for artificial neural systems
- An educational tool for understanding neural computation
- An investigation of whether cognition can emerge from spiking dynamics

## References

- NLM README.md for building and running
- docs/ARCHITECTURE.md for detailed architecture
- docs/SCIENCE.md for scientific background
- docs/EXPERIMENTS.md for experiment descriptions
