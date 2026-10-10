# NLM Architecture

## Overview

NLM (熙然) is an **integrated artificial brain** that successfully connects all its components into a coherent system. Unlike the previous disconnected skeleton, NLM now functions as a complete computational brain with all systems working together.

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain (PHASE 6)                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │   Memory     │    │   Action    │          │
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

                   ┌─────────────────────────────────────┐
                   │        COGNITIVE LOOP              │
                   │  (Top-down modulation)           │
                   └─────────────────────────────────────┘
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
- **Brain**: Central coordinator **with integrated systems** (Memory, Prediction, Cognition, Development, Neuromodulation)

### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing with spike event handlers

### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity with neuromodulation
- **StructuralPlasticity**: Synapse formation/removal with developmental control

### 5. Development Layer (Enhanced)
- **DevelopmentSystem**: Orchestrates development with sophisticated age-dependent effects
- **Synaptogenesis**: New synapse formation with developmental stage modulation
- **Pruning**: Weak synapse removal with activity-dependent regulation
- **Maturation**: Adult property development with multiple stages
- **CRITICAL PERIODS**: Enhanced sensitive period modeling with social and cognitive stages

### 6. Neuromodulation Layer (Enhanced)
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement with working memory integration
- **Acetylcholine**: Learning, memory encoding, and attention control (NEWLY IMPLEMENTED)
- **Norepinephrine**: Arousal, attention enhancement, and stress response (NEWLY IMPLEMENTED)
- **Serotonin**: Social behavior, attention scope, and memory consolidation (NEWLY IMPLEMENTED)
- **Novelty**: Novelty detection integrated with curiosity
- **Curiosity**: Exploration motivation integrated with prediction error
- **PredictionError**: Error signals for predictive learning

### 7. Memory Layer (Phase 4 + Enhanced Integration)
- **NeuralWorkingMemory**: Persistent activity for temporary storage with neuromodulation
- **NeuralEpisodicMemory**: Experience encoding in neural patterns with consolidation
- **NeuralAssociativeMemory**: Experience-based associations with cross-context retrieval
- **ConceptFormation**: Pattern discovery without labels (NEWLY INTEGRATED)
- **SpatialRepresentation**: Learned spatial relationships with place cell formation (NEWLY INTEGRATED)
- **TemporalRelation**: Before/after/sequence encoding with prediction (NEWLY INTEGRATED)

### 8. Prediction Layer (Phase 4 + Enhanced Integration)
- **NeuralPrediction**: Learns temporal sequences through repeated experience (ENHANCED)
- **ActionConsequencePredictor**: Associates actions with their effects (ENHANCED)
- **Multi-step Prediction**: Chains predictions to anticipate distant futures (ENHANCED)
- **CognitivePrediction**: Integrates with action planning and decision making

### 9. Cognition Layer (Phase 4 + Enhanced Integration)
- **NeuralPlanner**: Action planning with concept consideration (ENHANCED)
- **ConceptFormation**: Pattern discovery and category formation (ENHANCED)
- **SelfModel**: Self-knowledge and body awareness (NEWLY INTEGRATED)
- **SocialLearning**: Imitation and social knowledge acquisition (NEWLY INTEGRATED)
- **AttentionalSelection**: Focus control with neuromodulatory integration (ENHANCED)

### 10. Performance Layer
- **Memory Pools**: Efficient memory allocation management
- **Event Queues**: High-performance event processing
- **Sparse Connectivity**: Optimized memory usage for sparse connections
- **Parallel Processing**: Multi-threaded computation support
- **SIMD Vectorization**: Hardware-accelerated computation
- **CheckpointSystem**: Brain state persistence and recovery

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