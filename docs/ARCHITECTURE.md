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

## Architecture Philosophy

The NLM brain operates as a closed-loop system with these key principles:

1. **Modularity**: Clear separation between components (brain, world, agent)
2. **Event-driven computation**: Spiking neural dynamics with asynchronous processing
3. **Hierarchical organization**: Core → Brain → Memory → Cognition → Action
4. **Integrated learning**: Multiple plasticity mechanisms work together
5. **Biological plausibility**: Inspired by brain architecture and dynamics
6. **Performance optimization**: SIMD vectorization, memory pools, parallel processing

## Core Layer Components

### 1. Configuration System
- **Config**: JSON/YAML-based configuration management
- Supports nested key-value pairs with type checking
- Load/save from files and command-line arguments

### 2. Type System
- **NeuronId, SynapseId, RegionId, PopulationId**: Strong typing for neural entities
- Enumerated types for neuron states, action types, developmental stages
- Provides type safety throughout the system

### 3. Simulation Infrastructure
- **SimulationClock**: Deterministic time management
- **Random**: Reproducible pseudo-random number generation
- **Logger**: Structured logging with severity levels

## Neural Computation Layer

### 1. Neuron Dynamics
- **LIF Neuron**: Leaky Integrate-and-Fire model with biological parameters
- **Sensory Neurons**: Process external/internal stimuli
- **Motor Neurons**: Generate action outputs
- **Modulatory Neurons**: Control plasticity and neuromodulation

### 2. Synaptic Connections
- **Excitatory/Inhibitory**: Differential connection types
- **Electrical/Gap Junction**: Alternative coupling mechanisms
- **STDP**: Spike-timing dependent plasticity
- **Hebbian**: Co-activity based learning
- **Structural**: Synapse formation and elimination

### 3. Neural Populations and Regions
- **NeuralPopulation**: Groups of neurons with shared properties
- **NeuralRegion**: Functional brain regions
- **Inter-Regional Connectivity**: Long-range connections with delays

## Memory Systems (Phase 4)

### 1. Working Memory
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- Recurrent connections for information maintenance
- Competitive dynamics for selective attention

### 2. Episodic Memory
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- Pattern completion for memory recall
- Temporal and spatial indexing

### 3. Associative Memory
- **NeuralAssociativeMemory**: Pattern association learning
- Auto-associative and hetero-associative capabilities
- Novelty detection and storage

### 4. Concept Formation
- Discovers patterns without explicit labels
- Prototype formation through averaging
- Category abstraction and generalization

## Prediction System (Phase 4)

### 1. Neural Prediction
- **NeuralPrediction**: Temporal sequence learning
- Predicts sensory inputs and action consequences
- Multi-step prediction horizons

### 2. Action Consequence Prediction
- **ActionConsequencePredictor**: Links actions to outcomes
- Uses learned sequences for planning
- Updates with prediction errors

## Cognitive Systems (Phase 4)

### 1. Attention
- **AttentionalSelection**: Competitive selection dynamics
- Winner-take-all mechanisms
- Top-down and bottom-up modulation

### 2. Neural Planner
- **NeuralPlanner**: Multi-step action selection
- Uses predictions to evaluate futures
- Temporal abstraction and planning

### 3. Self-Model
- Learns body schema and action effects
- Distinguishes self from environment
- Capability tracking and error correction

## Neuromodulation Systems

### 1. Dopamine
- **Dopamine**: Reward prediction error signals
- Reinforcement learning modulation
- Plasticity weight adjustment

### 2. Curiosity
- **Curiosity**: Exploration motivation
- Novelty-driven learning
- Intrinsic reward signals

### 3. Novelty
- Detects unexpected sensory inputs
- Triggers exploration behavior
- Modulates attention

### 4. Prediction Error
- **PredictionError**: Mismatch signals
- Supervised learning component
- Memory system updating

## Development System

### 1. Developmental Stages
- **Initial**: High plasticity, basic connectivity
- **CriticalPeriod**: Sensitive learning window
- **Maturation**: Stabilization and refinement
- **Adult**: Optimized performance
- **Aging**: Gradual decline and adaptation

### 2. Synaptogenesis
- Formation of new connections
- Activity-dependent stabilization
- Pattern formation

### 3. Pruning
- Elimination of weak synapses
- Efficiency optimization
- Resource allocation

### 4. Maturation
- Synaptic strengthening
- Neural refinement
- Functional specialization

## Agent-Brain Interface

The AgentBrain class bridges NLM brain with external environments:

1. **SensoryTransduction**: World percepts → neural input
2. **MotorDecoding**: Neural activity → action commands
3. **Neuromodulation**: Reward signals → plasticity
4. **Development**: Stage progression → behavioral changes
5. **Attention**: Competitive processing → focus
6. **Planning**: Predictions → action selection

## Data Flow (Phase 3/4/6)

### World → Brain Loop
1. **Sensory Input** (Vision, Touch, Internal, Proprioception)
2. **Attentional Selection** (Competitive filtering)
3. **Neural Processing** (LIF dynamics, spikes)
4. **Prediction** (Temporal sequence learning)
5. **Working Memory** (Information maintenance)
6. **Episodic Memory** (Experience storage)
7. **Concept Formation** (Pattern discovery)
8. **Planning** (Multi-step action selection)
9. **Self-Model** (Body schema)
10. **Motor Action** (Action generation)
11. **World Interaction** (Consequences)
12. **Reward/Error** (Feedback signals)
13. **Neuromodulation** (Learning signals)
14. **Plasticity** (Connection updates)
15. **Memory/Development** (System changes)
16. **Changed Brain** (State modification)
17. **Changed Future Behavior** (Adapted responses)

## Integration Achievements (Phase 6)

### Memory Systems Integration
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

### Prediction System Integration
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

### Cognition Systems Integration
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Key Design Principles

1. **No ML Frameworks**: Implements neural computation from scratch
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Python API Integration

The NLM Python bindings (`pynlm`) expose the complete functionality:

1. **Core Classes**: `Brain`, `Config`, `SimpleWorld`, `AgentBrain`
2. **Data Types**: `Vision`, `Audio`, `InternalSignals`, `Action`
3. **Enumerations**: Neuron types, action types, developmental stages
4. **Factory Functions**: `createDefaultConfig()`, `createBrain()`, `createSimpleWorld()`
5. **Configuration Access**: Type-safe parameter getters/setters
6. **Simulation Loop**: Simple step-by-step execution
7. **Memory Access**: Working, episodic, and associative memory systems
8. **Prediction Integration**: Temporal sequence prediction
9. **Cognition Access**: Planning, attention, concept formation
10. **Neuromodulation Control**: Reward, curiosity, novelty

## Future Extensions

### Phase 7: Advanced Capabilities
- **Social Learning**: Multi-agent interaction and observation
- **Language Processing**: Symbolic and neural language models
- **Complex Reasoning**: Hierarchical planning and causal inference
- **Transfer Learning**: Knowledge transfer across tasks
- **Meta-Learning**: Learning how to learn

### Technical Enhancements
- **GPU Acceleration**: CUDA for large-scale simulations
- **Distributed Computing**: Multi-process, multi-machine scaling
- **Hybrid Models**: Integration with classical AI techniques
- **Explainability**: Interpretability of neural decisions
- **Safety**: Verify and validate critical systems

## Performance Considerations

1. **Memory Efficiency**: Memory pools reduce allocation overhead
2. **Computational Efficiency**: SIMD vectorization and cache-friendly layouts
3. **Scalability**: Event-driven algorithms for sparse connectivity
4. **Parallel Processing**: Lock-free data structures and thread-local storage
5. **Persistence**: Checkpointing for long-running simulations

## Conclusion

The NLM architecture demonstrates how modular, biologically-inspired neural systems can integrate to produce intelligent behavior. By separating concerns and maintaining clear interfaces, the system achieves both biological plausibility and computational efficiency. The integration of memory, prediction, cognition, and development into a unified brain loop represents a significant step toward artificial general intelligence based on neural principles.

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