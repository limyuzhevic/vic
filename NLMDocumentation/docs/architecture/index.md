# Architecture Guide

This document provides comprehensive documentation for NLM (Neural Learning Machine) architecture. It covers the system design, components, data flow, and implementation details across all phases of development.

## Overview

NLM is designed as a **modular computational brain** with a clear separation of concerns. The architecture supports incremental development from simple neural systems to complex cognitive architectures while maintaining biological plausibility and computational efficiency.

### Key Design Principles

1. **Modularity**: Clear interfaces between components for independent development
2. **Biological Inspiration**: Brain-inspired mechanisms without claiming biological accuracy
3. **Incremental Development**: Phase-based approach from simple to complex
4. **Determinism**: Reproducible simulations through explicit state management
5. **Performance**: Data-oriented design for large-scale neural simulations
6. **Extensibility**: Clear patterns for adding new components
7. **No ML Frameworks**: Implements neural computation from first principles
8. **No Transformer Architecture**: Uses spiking dynamics and competitive attention

## Architecture Overview

The NLM architecture can be viewed at multiple levels:

### System Level

```
┌─────────────────────────────────────────────────────────────┐
│                    NLM Brain System                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │    Sensory      │  │    Neural       │  │    Motor    │ │
│  │   Processing    │  │   Computation   │  │   Output    │ │
│  │                 │  │                 │  │             │ │
│  │ ┌─────────────┐ │  │ │ ┌─────────────┐ │  │ ┌─────────┐ │ │
│  │ │   Vision    │ │  │ │ │   Brain     │ │  │ │ Action   │ │ │
│  │ │   Touch     │ │  │ │ │   Core      │ │  │ │ Selection│ │ │
│  │ │   Audio     │ │  │ │ │   Layer     │ │  │ │   Layer  │ │ │
│  │ │  Internal   │ │  │ │ │             │ │  │ │         │ │ │
│  │ │   Signals   │ │  │ │ │             │ │  │ │         │ │ │
│  │ └─────────────┘ │  │ │ └─────────────┘ │  │ └─────────┘ │ │
│         │             │                  │              │
│         ▼             ▼                  ▼              │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │   Memory        │  │  Prediction     │  │  Neuromodul.│ │
│  │   Systems       │  │   System        │  │  Signals     │ │
│  │                 │  │                 │  │             │ │
│  │ ┌─────────────┐ │  │ │ ┌─────────────┐ │  │ ┌─────────┐ │ │
│  │ │ Working     │ │  │ │ │ Temporal    │ │  │ │ Dopamine │ │ │
│  │ │  Memory     │ │  │ │ │  Prediction │ │  │ │  Signals │ │ │
│  │ │  Systems    │ │  │ │ │  Error      │ │  │ │  Curiosity│ │ │
│  │ │  Episodic    │ │  │ │ │  Systems    │ │  │ │  Novelty  │ │ │
│  │ │  Memory     │ │  │ │ │             │ │  │ │  Signals   │ │ │
│  │ │  Concept    │ │  │ │ │             │ │  │ │  Error    │ │ │
│  │ │  Formation  │ │  │ │ │             │ │  │ │  Systems   │ │ │
│  │ └─────────────┘ │  │ │ └─────────────┘ │  │ └─────────┘ │ │
│                             │                  │
│                             ▼                  ▼             │
│                    ┌─────────────────┐  ┌─────────────┐      │
│                    │  Cognitive      │  │ Development │      │
│                    │   Systems       │  │   System    │      │
│                    │                 │  │             │      │
│                    │ ┌─────────────┐ │  │ ┌─────────┐ │      │
│                    │ │ Attention    │ │  │ │Growth    │ │      │
│                    │ │ Planning     │ │  │ │Stages    │ │      │
│                    │ │ Self-Model   │ │  │ │ Maturation│ │      │
│                    │ │ Social-Learn │ │  │ │ Pruning   │ │      │
│                    │ └─────────────┘ │  │ │ Synapto-  │ │      │
│                    │                 │  │ │  genesis  │ │      │
│                    └─────────────────┘  │ └─────────┘ │      │
│                                                             │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              PHASE 4: EMERGING COGNITION                    │ │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       │ │
│  │  │Attention│  │Planning │  │Self-Model│  │ Social  │       │ │
│  │  │         │  │         │  │         │  │Learning │       │ │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Component Layers

NLM is organized into distinct layers, each with specific responsibilities:

#### 1. Core Layer
- **Purpose**: Fundamental types and utilities
- **Components**: Type definitions, configuration management, random number generation, logging, time management
- **Phase**: 1-6 (throughout)

#### 2. Brain Layer
- **Purpose**: Neural representation and computation
- **Components**: Neurons, synapses, neural populations, brain regions, central coordinator
- **Phase**: 1-6 (throughout)

#### 3. Dynamics Layer
- **Purpose**: Neural dynamics and spike processing
- **Components**: Abstract dynamics base, spike system, neuron implementations
- **Phase**: 2-6

#### 4. Plasticity Layer
- **Purpose**: Learning and synaptic modification
- **Components**: Plasticity rules, STDP, Hebbian learning, structural plasticity
- **Phase**: 2-6

#### 5. Development Layer
- **Purpose**: Developmental processes and maturation
- **Components**: Development system, synaptogenesis, pruning, maturation
- **Phase**: 3-6

#### 6. Neuromodulation Layer
- **Purpose**: Motivation and reward processing
- **Components**: Neuromodulators, dopamine, curiosity, novelty detection
- **Phase**: 3-6

#### 7. Memory Layer (Phase 4+)
- **Purpose**: Cognitive memory and representation
- **Components**: Working memory, episodic memory, concept formation, spatial/temporal representation
- **Phase**: 4-6

#### 8. Prediction Layer (Phase 4+)
- **Purpose**: Temporal prediction and action planning
- **Components**: Neural prediction, action consequence prediction
- **Phase**: 4-6

#### 9. Cognitive Layer (Phase 4+)
- **Purpose**: Higher cognitive functions
- **Components**: Attention, planning, self-model, social learning
- **Phase**: 4-6

#### 10. Sensory Layer
- **Purpose**: Perceptual processing
- **Components**: Sensory input base, vision, audio, internal signals
- **Phase**: 3-6

#### 11. Motor Layer
- **Purpose**: Action generation and control
- **Components**: Action representation, motor system
- **Phase**: 3-6

#### 12. Environment Layer
- **Purpose**: Simulation environment interface
- **Components**: Environment interface, observation wrapper, action wrapper
- **Phase**: 3-6

#### 13. Experiments Layer
- **Purpose**: Experiment framework and metrics
- **Components**: Experiment definitions, experiment runner, metrics
- **Phase**: 4-6

## Phase Architecture

### Phase 1: Core Components (Complete)

**Focus**: Project skeleton and core infrastructure

```
┌─────────────────────────────────────────────────────────────┐
│                    PHASE 1: CORE INFRASTRUCTURE               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │    Core    │  │  Types      │  │ Config      │          │
│  │  Engine    │  │  System     │  │  Management │          │
│  │            │  │            │  │            │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
│         │                  │                  │         │
│         ▼                  ▼                  ▼         │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │   Brain    │    │   Neural   │    │   Interfaces│      │
│  │   Interface│    │ Interfaces │    │            │      │
│  │            │    │            │    │            │      │
│  └─────────────┘    └─────────────┘    └─────────────┘      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Key Innovations**:
- Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- Configuration management system
- Deterministic random number generator
- Structured logging system
- Simulation clock with time management

### Phase 2: Neural Computation (Complete)

**Focus**: LIF neuron dynamics and event-driven processing

```
┌─────────────────────────────────────────────────────────────┐
│                    PHASE 2: NEURAL COMPUTATION                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   LIF      │  │  Event     │  │  Plasticity│          │
│  │  Dynamics  │  │  System    │  │  Rules      │          │
│  │            │  │            │  │            │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
│         │                  │                  │         │
│         ▼                  ▼                  ▼         │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │   Spike    │    │  STDP       │    │  Hebbian    │      │
│  │ Processing │    │  Learning   │    │  Learning   │      │
│  │            │    │            │    │            │      │
│  └─────────────┘    └─────────────┘    └─────────────┘      │
│                             │                              │
│                             ▼                              │
│                    ┌─────────────┐                        │
│                    │ Structural  │                        │
│                    │  Plasticity│                        │
│                    │            │                        │
│                    └─────────────┘                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Key Components**:

#### LIF Neuron Dynamics
- **Integrate-and-Fire model** with membrane potential dynamics
- **Leaky integration** towards resting potential
- **Threshold-based firing** with reset mechanism
- **Refractory period** after firing
- **Event-driven processing** only for active neurons

#### Event-Driven Spike Propagation
- **Efficient spike transmission** through synapses
- **Synaptic delays** for realistic temporal dynamics
- **Parallel processing** of multiple spikes
- **Memory pools** for spike storage and delivery

#### STDP (Spike-Timing-Dependent Plasticity)
- **Pre-post spike timing** dependent weight changes
- **Long-term potentiation (LTP)** for causal connections
- **Long-term depression (LTD)** for anti-causal connections
- **Eligibility traces** for delayed reward signals

#### Hebbian Learning
- **Correlation-based weight changes** (neurons that fire together, wire together)
- **Simple associative learning** mechanism
- **Competitive dynamics** for selective strengthening

#### Structural Plasticity
- **Synapse formation (synaptogenesis)** during development
- **Synaptic pruning** of weak connections
- **Dynamic connectivity** that changes over time

### Phase 3: Environment Integration (Complete)

**Focus**: Closed-loop interaction with simulated environment

```
┌─────────────────────────────────────────────────────────────┐
│                    PHASE 3: ENVIRONMENT INTEGRATION          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   World    │  │   Agent    │  │   Brain     │          │
│  │   System   │  │   Brain    │  │   Loop      │          │
│  │            │  │   Interface│  │             │          │
│  │ ┌─────────┐ │  │ ┌─────────┐ │  │ ┌─────────┐ │          │
│  │ │ Sensory │ │  │ │Process  │ │  │ │Think    │ │          │
│  │ │  Input │ │  │ │ Sensory │ │  │ │Decide   │ │          │
│  │ │ Systems │ │  │ │ Input   │ │  │ │Action   │ │          │
│  │ └─────────┘ │  │ └─────────┘ │  │ └─────────┘ │          │
│         │           │              │              │         │
│         ▼           ▼              ▼              │         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐      │         │
│  │   Reward   │  │   Neuromod │  │  Learning   │      │         │
│  │  Signals   │  │ ulation    │  │  Updates    │      │         │
│  │            │  │            │  │            │      │         │
│  └─────────────┘  └─────────────┘  └─────────────┘      │         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Key Components**:

#### World System
- **2D simulated environment** with objects and entities
- **Sensory percepts** representing world state
- **Agent body** with position, orientation, velocity
- **World objects** (resources, hazards, walls)
- **Time-stepped simulation** with physics integration

#### AgentBrain Interface
- **Sensory transduction** from world percepts to neural input
- **Motor decoding** from neural activity to actions
- **Reward modulation** from environment feedback
- **Development updates** for learning progression
- **Neuromodulation** from prediction errors

#### Learning Loop
- **Closed-loop architecture** connecting perception to action
- **Reward prediction error** signals for learning
- **Prediction error backpropagation** through eligibility traces
- **Developmental progression** from plasticity to stability

### Phase 4: Emerging Cognition (Complete)

**Focus**: Higher cognitive functions and prediction systems

```
┌─────────────────────────────────────────────────────────────┐
│                    PHASE 4: EMERGING COGNITION              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │  Attention  │  │ Prediction  │  │ Working     │          │
│  │  Systems    │  │  Systems    │  │  Memory     │          │
│  │             │  │             │  │             │          │
│  │ ┌─────────┐ │  │ ┌─────────┐ │  │ ┌─────────┐ │          │
│  │ │ Competitive │  │ │ Temporal │  │ │ Persistent│ │          │
│  │ │   Selection│  │ │ Learning│  │ │  Activity │ │          │
│  │ │   Dynamics │  │ │  Systems │  │ │  Systems  │ │          │
│  │ └─────────┘ │  │ └─────────┘ │  │ └─────────┘ │          │
│         │           │              │              │         │
│         ▼           ▼              ▼              │         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐      │         │
│  │  Episodic   │  │  Concept   │  │  Cognitive  │      │         │
│  │  Memory    │  │  Formation │  │  Systems    │      │         │
│  │            │  │            │  │             │      │         │
│  └─────────────┘  └─────────────┘  └─────────────┘      │         │
│                                                             │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              PHASE 4: EMERGING COGNITION                    │ │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       │ │
│  │  │Attention│  │Planning │  │Self-Model│  │ Social  │       │ │
│  │  │         │  │         │  │         │  │Learning │       │ │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

**Key Innovations**:

#### Prediction Systems
- **Temporal sequence learning** through neural prediction
- **Action consequence prediction** for planning
- **Multi-step prediction** for forward planning
- **Prediction error signals** for learning optimization

#### Working Memory
- **Persistent neural activity** for temporary information storage
- **Competitive selection** for selective attention
- **Recurrent circuits** for information maintenance
- **Neural grounding** rather than vector storage

#### Episodic Memory
- **Experience encoding** as neural activity patterns
- **Pattern completion** for memory retrieval
- **Temporal and spatial indexing** for organization
- **Replay mechanisms** during consolidation

#### Concept Formation
- **Pattern discovery** without pre-defined categories
- **Prototype formation** through averaging similar experiences
- **Stability measurement** for concept reliability
- **Property-based hinting** for conceptual development

#### Attention
- **Lateral inhibition competition** for selective processing
- **Winner-take-most dynamics** for focused attention
- **Top-down and bottom-up bias** for goal-directed behavior
- **NOT Transformer QKV attention** - biologically inspired alternative

#### Planning
- **Prediction-based action evaluation** for goal achievement
- **Multi-step forward simulation** of potential outcomes
- **Adaptive planning** based on outcomes and errors
- **Hierarchical planning** through predictive hierarchies

#### Self-Model
- **Body schema learning** from sensorimotor interactions
- **Self-generated event prediction** for agency detection
- **Self vs. external distinction** for cause-effect understanding
- **Capability representation** for action planning

#### Social Learning
- **Observation of other agents** for learning from others
- **Imitation capabilities** for reproducing observed actions
- **Simple communication signals** for social interaction
- **Theory of mind limitations** for realistic social cognition

### Phase 5: Performance Infrastructure (Complete)

**Focus**: Scalability and efficiency for large-scale neural simulations

```
┌─────────────────────────────────────────────────────────────┐
│                    PHASE 5: PERFORMANCE INFRASTRUCTURE       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   Event    │  │  Memory    │  │  SIMD       │          │
│  │   Queues   │  │  Pools     │  │  Ops        │          │
│  │            │  │            │  │            │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
│         │                  │                  │         │
│         ▼                  ▼                  ▼         │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │   Sparse   │    │   Parallel │    │  Benchmark  │      │
│  │  Connectivity│  │ Processing │  │  Framework  │      │
│  │  Adjacency  │    │  Thread    │  │  Profiling  │      │
│  │  Lists     │    │  Pools     │  │  Systems    │      │
│  └─────────────┘    └─────────────┘  └─────────────┘      │
│                             │                              │
└─────────────────────────────────────────────────────────────┘
```

**Key Components**:

#### Event Queues
- **Lock-free SPSC ring buffers** for spike delivery
- **Cache-aligned event structures** (16 bytes each)
- **O(1) push/pop operations** for efficient spike handling
- **Time-bucketed delayed spike queues** for synaptic delays

#### Memory Pools
- **Pre-allocated neuron storage** with Structure-of-Arrays layout
- **SoA layout**: membrane potentials, refractory counters, firing states
- **Cache line alignment** (64 bytes per cache line)
- **Free list management** for O(1) allocation/deallocation

#### Sparse Connectivity
- **Adjacency list representation** for efficient sparse connections
- **Outgoing/incoming** synapse indexing for both directions
- **Contiguous synapse records** for cache efficiency
- **Avoids dense N×N matrices** for millions of neurons

#### Parallel Processing
- **Thread pool** for parallel neural computation
- **Lock-free task queues** for work distribution
- **Thread-local state accumulation** for parallel efficiency
- **Synchronized aggregation** for final results

#### SIMD Vectorization
- **AVX-512**: 16 neurons per iteration
- **AVX2**: 8 neurons per iteration
- **SSE**: 4 neurons per iteration
- **Scalar fallback** for non-SIMD architectures

#### Benchmarking Framework
- **Standard benchmarks** for performance validation
- **Scaling benchmarks** for large-network testing
- **Performance metrics** for comprehensive analysis
- **Hardware-specific optimizations** for different CPU architectures

### Phase 6: Final Integration (Complete)

**Focus**: Integration of all systems into a coherent artificial brain

```
┌─────────────────────────────────────────────────────────────┐
│                    PHASE 6: FINAL INTEGRATION               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   All      │  │   Systems  │  │   Integration│          │
│  │   Systems  │  │   Connected│  │   Test      │          │
│  │  Integrated │  │  Together  │  │             │          │
│  │            │  │            │  │             │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
│         │                  │                  │         │
│         ▼                  ▼                  ▼         │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │   Memory   │    │  Neuromod  │    │  Prediction │      │
│  │  Systems   │    │  Signals   │    │  Systems    │      │
│  │            │    │            │    │            │      │
│  └─────────────┘    └─────────────┘    └─────────────┘      │
│                             │                              │
│                             ▼                              │
│                    ┌─────────────┐                        │
│                    │  Cognition │                        │
│                    │   Systems  │                        │
│                    │            │                        │
│                    └─────────────┘                        │
│                             │                              │
│                             ▼                              │
│                    ┌─────────────┐                        │
│                    │ Development │                        │
│                    │   System    │                        │
│                    │            │                        │
│                    └─────────────┘                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Integration Achievements**:

#### Memory Systems Integration
- **Working memory** connected to neural processing
- **Episodic memory** with experience encoding and replay
- **Associative memory** with Hebbian pattern associations
- **All memory systems** interconnected with neural dynamics

#### Neuromodulation Integration
- **Dopamine** affects neural excitability and plasticity
- **Curiosity** drives exploration behavior
- **Novelty detection** integrated with sensory processing
- **All neuromodulators** connected to plasticity rules

#### Prediction System Integration
- **Prediction system** integrated into brain loop
- **Prediction error signals** affect learning
- **Confidence tracking** implemented

#### Cognition Systems Integration
- **Neural planner** with action sequence evaluation
- **Concept formation** from experience patterns
- **Attention** with competitive selection dynamics
- **All cognition systems** connected to perception and action

#### Development Integration
- **Developmental stages** affect plasticity rates
- **Structural plasticity** modulated by age
- **Neural excitability** changes with development

#### Persistence
- **Checkpoint save/load** implemented
- **Brain state serialization** working
- **Can resume from saved checkpoints**

#### Replay and Consolidation
- **Episodic memory replay** during simulation
- **Memory consolidation** for important episodes
- **Integration with sleep/rest cycle**

## Data Flow Architecture

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

### Phase 3: Environment Integration

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
Developmental Change
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
ACTION SELECTION
      ↓
MOTOR OUTPUT
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

## Agent-Brain Interface Architecture

Phase 3/4 introduces the **AgentBrain** class that connects the NLM brain to the world:

```
┌─────────────────────────────────────────────────────────────┐
│                    AgentBrain Interface                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │ Sensory    │  │   Motor    │  │  Neuromodul │
│  │  Transduct │  │  Decoding  │  │  Signals    │          │
│  │   ion      │  │            │  │             │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
│         │                  │                  │         │
│         ▼                  ▼                  ▼         │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │ Development │    │   Attention │    │   Planning │      │
│  │   System    │    │   Dynamics  │    │   Systems   │      │
│  │             │    │             │    │             │      │
│  └─────────────┘    └─────────────┘    └─────────────┘      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Interface Components**:

#### SensoryTransduction
- Converts world percepts to neural input signals
- Maps sensory features to neural population activity
- Maintains temporal continuity across simulation steps

#### MotorDecoding
- Reads neural activity and produces motor commands
- Converts distributed neural patterns to coordinated actions
- Integrates multiple motor primitives into complex actions

#### Neuromodulation
- Applies reward signals to plasticity mechanisms
- Modulates learning rates based on prediction errors
- Coordinates multiple neuromodulator systems

#### Development
- Updates developmental stage and plasticity rates
- Manages critical periods and maturation processes
- Tracks learning progress and behavioral changes

#### Attention
- Implements competitive selection of sensory inputs
- Integrates top-down goals with bottom-up salience
- Optimizes information processing efficiency

#### Planning
- Uses predictive models to evaluate action sequences
- Simulates potential futures to select optimal actions
- Adapts plans based on outcomes and new information

## Design Patterns and Implementation Details

### Factory Patterns

#### Brain Creation
```python
// C++ Example
std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) {
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        return brain;
    }
    throw SimulationError("Failed to initialize brain");
}
```

```python
// Python Example
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### Template Patterns

#### Neuron Dynamics
```cpp
template<typename T>
class LIFNeuron {
public:
    T membrane_potential;
    T threshold;
    T resting_potential;
    T reset_potential;
    T time_constant;
    T refractory_counter;
    
    void update(T input, T dt) {
        // LIF dynamics implementation
        membrane_potential += (input - membrane_potential) / time_constant * dt;
        if (membrane_potential >= threshold && refractory_counter <= 0) {
            fire();
            membrane_potential = reset_potential;
            refractory_counter = refractory_time;
        }
        if (refractory_counter > 0) {
            refractory_counter -= dt;
        }
    }
};
```

### Strategy Patterns

#### Plasticity Rules
```cpp
class PlasticityRule {
public:
    virtual void apply(Weight& weight, double pre_spike_time, double post_spike_time) = 0;
    virtual ~PlasticityRule() = default;
};

class STDPPlasticity : public PlasticityRule {
public:
    void apply(Weight& weight, double pre_spike_time, double post_spike_time) override {
        double dt = post_spike_time - pre_spike_time;
        if (dt > 0) {
            weight += learning_rate * std::exp(-dt / tau_plus);
        } else {
            weight -= learning_rate * std::exp(dt / tau_minus);
        }
    }
};
```

### Observer Pattern

#### Configuration Changes
```cpp
class ConfigObserver {
public:
    virtual void onConfigChanged(const std::string& key, const ConfigValue& value) = 0;
    virtual ~ConfigObserver() = default;
};

class Brain : public ConfigObserver {
public:
    void onConfigChanged(const std::string& key, const ConfigValue& value) override {
        if (key == "brain.neuron_count") {
            resizeNeurons(value.as<int>());
        } else if (key == "plasticity.stdp.learning_rate") {
            updateSTDPParameters(value.as<double>());
        }
    }
};
```

## Quality and Reliability

### Testing Strategy

#### Unit Tests
- **Individual components**: Neuron dynamics, plasticity rules, connectivity
- **Isolated functionality**: Memory systems, prediction modules
- **Boundary conditions**: Edge cases and error conditions
- **Performance tests**: Benchmark and scaling validation

#### Integration Tests
- **Phase-specific testing**: Phase 1, 2, 3, 4, 5, 6 tests
- **Cross-component testing**: Interface and data flow validation
- **End-to-end testing**: Complete system functionality
- **Regression testing**: Maintain backward compatibility

#### Performance Testing
- **Micro-benchmarks**: Individual component performance
- **Macro-benchmarks**: System-scale performance
- **Scaling tests**: Performance with increasing network size
- **Stress tests**: Extreme parameter combinations

### Documentation Integration

#### Code Documentation
- **Doxygen comments** for C++ headers and source files
- **Python docstrings** for API documentation
- **Examples** in code comments and documentation
- **Cross-references** between documentation and code

#### Architectural Documentation
- **Architecture diagrams** at multiple levels
- **Component interfaces** and data flow
- **Decision rationale** for design choices
- **Trade-off analysis** between alternatives

## Future Architecture Evolution

### Phase 7: Advanced Learning
- **Meta-learning**: Learning how to learn
- **Transfer learning**: Knowledge transfer between tasks
- **Zero-shot learning**: Generalization from examples
- **Continual learning**: Lifelong skill acquisition

### Phase 8: Enhanced Social Cognition
- **Theory of mind**: Understanding others' mental states
- **Complex imitation**: Advanced action reproduction
- **Cultural transmission**: Knowledge sharing across generations
- **Cooperation**: Collaborative problem solving

### Phase 9: Reflective Cognition
- **Self-reflection**: Meta-cognition and self-awareness
- **Goal refinement**: Dynamic goal setting and adjustment
- **Emotional learning**: Affective processing and regulation
- **Introspection**: Understanding internal mental states

### Phase 10: Consciousness and Qualia
- **Subjective experience**: First-person perspective
- **Awareness**: Global workspace architecture
- **Intention**: Goal-directed consciousness
- **Identity**: Persistent self-representation

## Technical Specifications

### Data Types

#### NeuronId
```cpp
struct NeuronId {
    uint64_t value;
    
    uint64_t index() const { return value & 0xFFFFFFFF; }
    bool operator==(const NeuronId& other) const { return value == other.value; }
    bool operator!=(const NeuronId& other) const { return value != other.value; }
    size_t hash() const { return std::hash<uint64_t>{}(value); }
};
```

#### SynapseId
```cpp
struct SynapseId {
    uint64_t value;
    uint64_t index() const { return value & 0xFFFFFFFF; }
    bool operator==(const SynapseId& other) const { return value == other.value; }
    bool operator!=(const SynapseId& other) const { return value != other.value; }
    size_t hash() const { return std::hash<uint64_t>{}(value); }
};
```

### Memory Management

#### Memory Pool Design
```cpp
class MemoryPool {
private:
    std::vector<char> storage_;
    std::list<size_t> free_list_;
    size_t alignment_;
    size_t capacity_;
    
public:
    void* allocate(size_t size) {
        // Align allocation
        size_t aligned_size = alignUp(size, alignment_);
        
        // Find suitable free block
        for (auto it = free_list_.begin(); it != free_list_.end(); ++it) {
            if (*it >= aligned_size) {
                void* ptr = storage_.data() + *it;
                free_list_.erase(it);
                return ptr;
            }
        }
        
        // Allocate new block if no suitable free block
        return allocateNewBlock(aligned_size);
    }
    
    void deallocate(void* ptr, size_t size) {
        // Find block location
        char* char_ptr = static_cast<char*>(ptr);
        size_t offset = char_ptr - storage_.data();
        
        // Add to free list
        free_list_.push_back(offset);
    }
};
```

### Performance Characteristics

#### Time Complexity

| Operation | Time Complexity | Notes |
|-----------|-----------------|-------|
| Neuron update | O(1) per neuron | Event-driven processing |
| Synapse connection | O(1) with adjacency lists | Sparse connectivity |
| Plasticity update | O(1) per synapse | Local updates |
| Memory access | O(1) with cache alignment | Data-oriented design |
| Spike propagation | O(k) where k = active synapses | Sparse transmission |

#### Space Complexity

| Component | Space Complexity | Optimization |
|-----------|------------------|---------------|
| Neuron state | O(N) | Structure of Arrays |
| Synapse state | O(S) | Compact representation |
| Event queue | O(Q) | Ring buffer |
| Memory pools | O(P) | Pre-allocated |
| Connectivity | O(S) | Adjacency lists |

### Hardware Requirements

#### Minimum Specifications
- **CPU**: x86-64, SSE4.2 support
- **Memory**: 2 GB RAM (micro), 8 GB (medium), 16 GB (large)
- **Storage**: 100 MB for models and checkpoints
- **Operating System**: Linux, Windows, macOS

#### Recommended Specifications
- **CPU**: x86-64, AVX-512 support
- **Memory**: 8 GB RAM (micro), 32 GB (medium), 64 GB (large)
- **Storage**: SSD for performance
- **GPU**: CUDA 11+ for accelerated computation (future)

#### Performance Targets

| Scale | Neurons | Synapses | Memory | Steps/sec |
|-------|---------|----------|--------|-----------|
| Micro | 1,000 | ~10,000 | ~10 MB | 10,000+ |
| Small | 10,000 | ~100,000 | ~100 MB | 5,000+ |
| Medium | 100,000 | ~1,000,000 | ~1 GB | 1,000+ |
| Large | 1,000,000 | ~10,000,000 | ~10 GB | 100+ |

## Architecture Documentation Versions

### Version History

- **Version 1.0**: Initial architecture description (Phase 1-4)
- **Version 2.0**: Added Phase 5 performance infrastructure
- **Version 3.0**: Completed Phase 6 integration documentation
- **Version 4.0**: Enhanced with detailed component descriptions

### Architecture Views

#### Logical View
- **Component hierarchy**: Layered architecture
- **Interaction patterns**: Factory, template, strategy
- **State management**: Deterministic simulation
- **Error handling**: Comprehensive exception management

#### Physical View
- **File structure**: Source organization and build system
- **Deployment**: Installation and configuration
- **Scaling**: Horizontal and vertical scaling
- **Monitoring**: Performance and health monitoring

#### Development View
- **Implementation patterns**: C++ and Python bindings
- **Testing strategy**: Unit, integration, performance
- **Build system**: CMake and scikit-build
- **Dependencies**: External library management

## Architecture Assessment

### Strengths

1. **Biological Plausibility**: Brain-inspired mechanisms without claims of accuracy
2. **Modularity**: Clear interfaces for independent development and testing
3. **Scalability**: Performance infrastructure supports large-scale simulations
4. **Flexibility**: Extensible architecture for adding new components
5. **Determinism**: Reproducible simulations for scientific research
6. **Performance**: Optimized for large-scale neural computations

### Limitations

1. **Simplified Biology**: Many biological details omitted for computational tractability
2. **Performance Trade-offs**: Performance optimizations may reduce biological accuracy
3. **Limited Cognition**: Still exploring emergence of higher cognitive functions
4. **Memory Constraints**: Large-scale simulations require significant memory

### Future Directions

1. **Enhanced Learning**: Meta-learning and transfer capabilities
2. **Social Cognition**: Theory of mind and complex social interaction
3. **Reflective Processing**: Self-awareness and meta-cognition
4. **Consciousness Research**: Investigation of subjective experience

## Conclusion

The NLM architecture represents a **modular, biologically-inspired computational brain** designed for research and experimentation. Through a phase-based development approach, it balances biological plausibility with computational efficiency.

Key architectural achievements include:

- **Clear separation of concerns** across multiple layers
- **Flexible integration patterns** for connecting components
- **Performance optimizations** for large-scale neural simulations
- **Extensible design** for future enhancements
- **Comprehensive testing** for reliability and validation

The architecture continues to evolve through ongoing research and development, with Phase 6 representing a significant milestone in creating a **coherent artificial brain** with integrated memory, neuromodulation, prediction, and cognitive capabilities.

---

*Architecture Version*: 3.0
*Last Updated*: 2026-10-09
*Documentation Status*: Complete (Phase 6)

**Related Documentation**:
- [Performance Guide](performance/index.md) - Performance optimization and tuning
- [Configuration Guide](configuration/index.md) - System configuration parameters
- [Python API Reference](python-api/index.md) - Complete API documentation
- [Tutorial Series](tutorials/index.md) - Step-by-step learning guides