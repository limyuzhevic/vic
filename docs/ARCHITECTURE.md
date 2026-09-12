# NLM Architecture - Phase 6 Integrated View

## Overview

NLM (熙然) is designed as a **fully integrated artificial cognitive system** with all components working together in a coherent loop:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM BRAIN (Phase 6)                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    BRAIN    │───▶│    MOTOR    │          │
│  │   Input    │    │   CORE      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Working    │    │  Episodic   │    │   Action    │          │
│  │  Memory     │    │  Memory     │    │  Selection  │          │
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
│                    │  Attention  │                              │
│                    │  System     │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Concept    │                              │
│                    │  Formation  │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │   Planner   │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Self-Model │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Neuromod.  │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Development │                             │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 6: COHERENT COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Memory  │  │Attention│  │Prediction│  │Social   │       ││
│  │  │Integration│ │Selection│ │System    │ │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Phase 6: Integrated Brain Loop

The Phase 6 brain implements a **closed-loop artificial cognitive system** where all components interact seamlessly:

### 1. Sensory Input Processing
- **Sensory Input** from environment enters brain
- **Sensory Neurons** receive current injection
- **Working Memory** stores active sensory patterns
- **Prediction System** predicts next sensory states

### 2. Neural Computation
- **LIF Neuron Dynamics** process inputs
- **Spike Events** propagate through network
- **Neuromodulation** (Dopamine, Novelty, Curiosity) adjusts learning
- **STDP & Hebbian Plasticity** update synaptic weights
- **Structural Plasticity** forms/weakens connections

### 3. Cognitive Processing
- **Attention System** selects focus through competitive dynamics
- **Concept Formation** discovers patterns and abstractions
- **Prediction System** generates forward models
- **Memory Integration** connects working → episodic → semantic

### 4. Planning and Action
- **Neural Planner** selects actions based on predictions
- **Self-Model** predicts action consequences
- **Action Selection** produces motor commands
- **Motor Output** affects environment

### 5. Learning and Consolidation
- **Reward Signals** modulate plasticity via dopamine
- **Prediction Error** drives learning
- **Sleep/Consolidation** replayes and strengthens memories
- **Development** adjusts plasticity with age

## Core Components (Phase 6)

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Neural Layer (Working)
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator with all systems integrated

### 3. Dynamics Layer (Working)
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### 4. Plasticity Layer (Working)
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### 5. Development Layer (Working)
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### 6. Neuromodulation Layer (Working)
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### 7. Memory Layer (Phase 6 - Integrated)
- **NeuralWorkingMemory**: Transient active information with persistent activity
- **NeuralEpisodicMemory**: Experience encoding in neural patterns with replay
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

### 8. Prediction Layer (Phase 6 - Integrated)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

### 9. Cognitive Layer (Phase 6 - Integrated)
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

### 10. Sensory/Motor Layers (Working)
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### 11. Environment Layer (Working)
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 12. Experiments Layer (Working)
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

## Data Flow - Phase 6 Integration

### Closed-Loop Cognitive Loop
```
WORLD (Environment)
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
ATTENTION (competitive selection)
      ↓
NEURAL COMPUTATION (LIF dynamics + plasticity)
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
SOCIAL LEARNING (observation/imitation)
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
MEMORY CONSOLIDATION
```

## Key Design Principles (Phase 6)

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Full Integration**: All cognitive systems interconnected
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **Experience-Driven**: All learning from interaction, no predefined knowledge

## Phase 6: Key Innovations

### Integration Architecture
- **Unified Brain Loop**: All systems process in single timestep
- **Real-time Prediction**: Continuous forward model updating
- **Memory Replay**: Sleep/consolidation with neural replay
- **Neuromodulation Integration**: Dopamine, ACh, NE all connected to learning

### Cognitive Integration
- **Prediction-Episodic Coupling**: Predictions guide what gets stored
- **Attention-Memory Linkage**: Attention selects working memory winners
- **Planning-Execution Loop**: Neural planner drives action selection
- **Self-Model Updating**: Self-model learns from action consequences

### Development-Driven Learning
- **Stage-Based Plasticity**: All rates change with developmental stage
- **Maturation Effects**: Neural properties evolve over time
- **Critical Periods**: Sensitive windows for learning
- **Stability-Plasticity Balance**: Development balances learning and stability

## Agent-Brain Interface

The AgentBrain class seamlessly connects the NLM brain to the world:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

## Performance Optimizations

### Event-Driven Architecture
- **Sparse Connectivity**: Only active neurons processed
- **Event Queues**: Spike events processed asynchronously
- **Memory Pools**: Efficient allocation/deallocation
- **SIMD Vectorization**: Parallel processing of neuron groups

### Memory Efficiency
- **Neural Memory**: Working/episodic memory stored in neural patterns
- **Sparse Representations**: Only firing neurons maintain activity
- **Replay Compression**: Important memories replayed during consolidation
- **Checkpoint System**: Full state serialization with compression

## Scalability Features

### Parallel Processing
- **Region Parallelism**: Different brain regions processed in parallel
- **Population Parallelism**: Neurons in same region processed in parallel
- **Thread-Safe Design**: Multiple threads can access different regions
- **NUMA Awareness**: Memory locality optimized for multi-core systems

### Data Locality
- **Cache-Friendly Layouts**: Neural data stored contiguously
- **Preallocated Arrays**: No dynamic allocation during simulation
- **Batch Processing**: Operations on neuron groups rather than individuals
- **Memory Pooling**: Efficient reuse of memory allocations

## Compliance with NLM Core Philosophy

### Experience-Driven
- ✅ Sensory input → working memory → episodic memory
- ✅ All learning from interaction, no predefined knowledge
- ✅ Continual learning without catastrophic forgetting

### Development-Driven
- ✅ Age-based plasticity modifiers
- ✅ Critical periods for learning
- ✅ Maturation from plastic to stable

### Neural
- ✅ LIF neuron dynamics with spiking
- ✅ Synaptic transmission with delays
- ✅ Event-driven spike processing

### Plastic
- ✅ STDP and Hebbian plasticity
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Neuromodulation effects on learning

### Recurrent Where Appropriate
- ✅ Working memory with recurrent connections
- ✅ Neural populations with recurrent connectivity
- ✅ Spiking dynamics support recurrence

### Sparse/Event-Driven
- ✅ Event queues for spike delivery
- ✅ Only firing neurons maintain activity
- ✅ Lazy computation for sparse networks

### Embodied
- ✅ Agent + SimpleWorld integration
- ✅ Motor commands affect environment
- ✅ Sensory feedback loops

### Self-Organizing
- ✅ Structural plasticity shapes network
- ✅ Developmental stages guide learning
- ✅ Memory consolidation reorganizes representations

### Continually Learning
- ✅ Memory integration for continual improvement
- ✅ Replay during consolidation
- ✅ Development-driven plasticity adjustment