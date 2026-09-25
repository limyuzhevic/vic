# NLM Architecture

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The architecture is modular and designed to support incremental biological inspiration while maintaining computational efficiency.

```
──────────────────────────────────────────────────────────────────┐
│                          NLM Brain                                │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
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
│                                                                  │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 6: FINAL INTEGRATION                     ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Memory  │  │Prediction│  │Attention │  │Social   │       ││
│  │  │Systems │  │Systems   │  │Systems   │  │Learning │       ││
│  │  │         │  │         │  │         │  │         │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└──────────────────────────────────────────────────────────────────┘
```

## Current Phase (6) – Final Integration

NLM is currently in **Phase 6: Final Integration**, where all systems work together as a coherent artificial brain. Phase 6 focuses on integrating the individual components into a unified architecture that can reproduce complex cognitive behaviors.

## Core Components

### 1. Core Layer

**Fundamental Infrastructure**
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, NeuralPopulationId, NeuralRegionId)
- **Config**: Hierarchical configuration management system
- **Random**: Deterministic random number generator with seed control
- **Logger**: Structured logging system with multiple output formats
- **SimulationClock**: High-precision time management with pause/resume

### 2. Brain Layer

**Neural Computation Engine**
- **Neuron**: Leaky Integrate-and-Fire neuron with adaptive properties
- **Synapse**: Synaptic connections with dynamic weights and delays
- **NeuralPopulation**: Groups of neurons with shared properties and update rules
- **NeuralRegion**: Brain regions with internal connectivity and communication
- **Brain**: Central coordinator with access to all subsystems

### 3. Dynamics Layer

**Spiking Neural Dynamics**
- **NeuralDynamics**: Abstract base for neuron dynamics (LIF, adaptive models)
- **SpikeSystem**: Event-driven spike processing with temporal precision
- **RefractoryPeriod**: Dynamic refractory period management
- **MembraneProperties**: Configurable ion channel dynamics

### 4. Plasticity Layer

**Learning Mechanisms**
- **PlasticityRule**: Abstract base for plasticity computation
- **STDP**: Spike-Timing-Dependent Plasticity with multiple forms
- **Hebbian**: Correlation-based Hebbian learning
- **StructuralPlasticity**: Activity-dependent synapse formation and elimination

### 5. Development Layer

**Developmental Programming**
- **DevelopmentSystem**: Stage-based development with plasticity modulation
- **Synaptogenesis**: Activity-dependent synapse formation algorithms
- **Pruning**: Competitive and activity-dependent synapse elimination
- **Maturation**: Age-dependent property development and refinement

### 6. Neuromodulation Layer

**Modulatory Signal Systems**
- **Neuromodulator**: Abstract base for neuromodulatory signals
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Novelty detection and exploration motivation
- **PredictionError**: Temporal difference learning signals
- **Novelty**: Surprise-based behavioral flexibility

### 7. Memory Systems (Phase 4)

**Multi-Modal Memory Architecture**
- **NeuralWorkingMemory**: Persistent activity for temporary information maintenance
- **NeuralEpisodicMemory**: Experience encoding with pattern completion
- **NeuralAssociativeMemory**: Stimulus-response associations and cue-based retrieval
- **ConceptFormation**: Pattern discovery and category abstraction
- **SpatialRepresentation**: Learned spatial relationships and navigation
- **TemporalRelation**: Sequential and causal relationship encoding

### 8. Prediction Layer (Phase 4)

**Temporal and Causal Modeling**
- **NeuralPrediction**: Temporal sequence learning and prediction
- **ActionConsequencePredictor**: Action-outcome association learning
- **PredictionErrorSignal**: Error computation for learning and adaptation

### 9. Cognitive Layer (Phase 4)

**Advanced Cognitive Mechanisms**
- **AttentionalSelection**: Competitive attention without Transformer architecture
- **NeuralPlanner**: Predictive action selection and evaluation
- **SelfModel**: Sensorimotor self-awareness and body schema
- **SocialLearning**: Observational learning and imitation capabilities

### 10. Sensory Layer

**Perception Systems**
- **SensoryInput**: Base class for all sensory modalities
- **Vision**: Visual processing with field-of-view models
- **Audio**: Sound processing and source localization
- **Proprioception**: Body position and movement sensing
- **InternalSignals**: Physiological state monitoring

### 11. Motor Layer

**Action Execution**
- **Action**: Motor command representation and planning
- **MotorSystem**: Neural-to-action decoding and motor control
- **MovementGeneration**: Trajectory planning and execution

### 12. Environment Layer

**Interaction Framework**
- **Environment**: Abstract environment interface with physics simulation
- **Observation**: Sensory perception and feature extraction
- **Action**: Motor command execution and effect simulation

### 13. Experiment Framework (Phase 6)

**Scientific Validation**
- **Experiment**: Single experiment design and execution
- **ExperimentRunner**: Automated experiment management and analysis
- **Metrics**: Comprehensive performance and behavior metrics
- **Phase6Experiment**: Integration tests for all systems

## Performance Infrastructure (Phase 5)

### 1. Memory Management

**Advanced Memory Systems**
- **MemoryPool**: Pre-allocated memory pools for neurons and synapses
- **EventQueue**: Lock-free ring buffers for spike events
- **SparseConnectivity**: Adjacency list representations
- **SoA Storage**: Structure-of-Arrays for better cache locality

### 2. Computational Optimizations

**Performance Engineering**
- **SIMDVectorization**: SIMD-accelerated neuron updates
- **ParallelProcessing**: Thread pool and cache-aware algorithms
- **EventDriven**: Skip inactive neurons for efficiency
- **OptimizedBrain**: High-performance brain interface

### 3. Biological Fidelity (Phase 5)

**Neuroscientific Accuracy**
- **AdaptiveNeuronModels**: Dynamic ion channel properties
- **ShortTermPlasticity**: Facilitation and depression mechanisms
- **ConfigurableInhibitory**: Balanced excitation-inhibition
- **HomeostaticPlasticity**: Stability maintenance mechanisms

## Data Flow (Current Phase 6)

### Phase 6: Integrated Brain Loop

```
WORLD (SimpleWorld/Advanced Environment)
      ↓
SENSORY PERCEPT (Multi-modal: Vision, Touch, Internal)
      ↓
ATTENTION (Competitive selection, winner-take-most)
      ↓
NEURAL PROCESSING (LIF dynamics, plasticity)
      ↓
PREDICTION (Temporal sequences, action consequences)
      ↓
WORKING MEMORY (Persistent activity, maintenance)
      ↓
EPISODIC MEMORY (Experience encoding, replay)
      ↓
CONCEPT FORMATION (Pattern discovery, categorization)
      ↓
PLANNING (Multi-step prediction, action evaluation)
      ↓
SELF-MODEL (Sensorimotor learning, body schema)
      ↓
ACTION SELECTION (Planned motor commands)
      ↓
MOTOR OUTPUT (Decoded neural activity)
      ↓
WORLD.applyAction()
      ↓
WORLD STATE CHANGE (Rewards, feedback)
      ↓
REWARD / PREDICTION ERROR
      ↓
NEUROMODULATION (Dopamine, curiosity, prediction error)
      ↓
PLASTICITY UPDATE (STDP, Hebbian, structural)
      ↓
DEVELOPMENTAL CHANGE (Stage progression, plasticity modulation)
      ↓
CHANGED BRAIN STATE
      ↓
ALTERNATIVE BEHAVIOR
```

## Agent-Brain Interface

The **AgentBrain** class provides the bridge between the NLM brain and the external world, coordinating all subsystems:

- **SensoryTransduction**: Converts world percepts to neural input patterns
- **MotorDecoding**: Transforms neural activity into executable motor commands
- **Neuromodulation**: Integrates reward signals and internal states
- **Development**: Manages developmental stage progression and plasticity
- **Attention**: Implements competitive selection among neural representations
- **Planning**: Uses predictive models for action selection and evaluation

## Key Design Principles

1. **Neurobiological Plausibility**: Architecture inspired by empirical brain data
2. **Anti-Transformer Commitment**: No transformer or deep learning architectures
3. **Developmental Trajectory**: Brain develops from primitive to complex capabilities
4. **Memory Integration**: All systems work together as unified architecture
5. **Performance Optimization**: Efficient algorithms and data structures
6. **Deterministic Experimentation**: Reproducible results for scientific validation
7. **Modular Extensibility**: Clear interfaces for adding new components
8. **Multi-Modal Interaction**: Integrates sensory, motor, and cognitive processing

## Phase 6 Integration Achievements

### Memory System Integration
- **Working Memory**: Persistent activity with competitive maintenance
- **Episodic Memory**: Experience encoding with pattern completion
- **Associative Memory**: Stimulus-response associations
- **Concept Formation**: Abstract pattern discovery

### Neuromodulation Integration
- **Dopamine**: Reward prediction error signals
- **Curiosity**: Exploration motivation and novelty detection
- **Prediction Error**: Temporal difference learning
- **Novelty**: Surprise-based behavioral flexibility

### Cognitive Integration
- **Attention**: Competitive selection without Transformer architecture
- **Planning**: Predictive action evaluation using learned models
- **Self-Model**: Sensorimotor self-awareness and agency
- **Social Learning**: Observational learning and imitation

### Development Integration
- **Developmental Stages**: Initial → CriticalPeriod → Maturation → Adult → Aging
- **Plasticity Modulation**: Age-dependent learning rates and synaptic changes
- **Critical Periods**: Sensitive windows for skill acquisition
- **Maturation**: Refinement and specialization of capabilities

## Current Status and Validation

Phase 6 demonstrates that NLM can integrate all major subsystems into a coherent artificial brain capable of:

- Learning from experience through multiple plasticity mechanisms
- Maintaining working memory for active information processing
- Encoding and recalling episodic experiences
- Forming abstract concepts and categories
- Planning actions using predictive models
- Developing self-awareness and social cognition
- Adapting behavior based on rewards and prediction errors

The architecture validates the hypothesis that cognition can emerge from the interaction of spiking neural dynamics, developmental processes, memory systems, and neuromodulation within a unified computational framework.

## Future Extensions (Phase 7+)

The Phase 6 architecture provides a foundation for future extensions:

1. **Multi-agent systems**: Parallel processing of multiple brains
2. **Complex environments**: Rich physics and social dynamics
3. **Higher cognition**: Language, reasoning, and abstract thought
4. **Neuromodulation expansion**: Additional modulatory signals
5. **Learning acceleration**: Meta-learning and transfer learning
6. **Biological fidelity**: More detailed neuron models

## Summary

NLM's Phase 6 architecture represents a significant milestone in the development of brain-inspired computational systems. By integrating neural dynamics, plasticity, development, neuromodulation, memory, and cognitive systems into a unified framework, NLM demonstrates the potential for artificial systems that can learn, adapt, and exhibit complex behaviors through the interaction of biologically-inspired components.

The anti-Transformer commitment ensures that NLM remains fundamentally different from deep learning approaches, focusing on spiking neural computation and emergent cognitive capabilities rather than pattern recognition and statistical learning.

This architecture provides a solid foundation for exploring the fundamental principles of biological intelligence and developing applications that require adaptive, learning-based behavior in complex environments.