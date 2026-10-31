# NLM Component Guide

## Major Subsystems Overview

This document provides detailed information about the major subsystems that make up the NLM brain architecture. Each subsystem has specific responsibilities and interacts with other subsystems to create a unified cognitive architecture.

## 1. Core Neural Components

### Brain System (`brain/`)
**Purpose**: The central neural architecture implementing spiking neural computation

#### Key Components:
- **Neuron**: LIF (Leaky Integrate-and-Fire) model with membrane potential dynamics
- **Synapse**: Event-driven spike propagation with synaptic delays
- **Neural Population**: Organized groups of neurons with shared properties
- **Neural Region**: Anatomical brain regions containing multiple populations

#### Interaction Patterns:
- Regions contain Populations
- Populations contain Neurons
- Neurons connect via Synapses (directional: pre → post)
- Spike propagation follows temporal sequences

#### Development Stage Integration:
- Synaptogenesis increases connection probability over time
- Pruning reduces connections in less active pathways
- Structural plasticity reshapes brain architecture

#### Key Methods:
- `brain->step(step, time)` - Main processing loop
- `brain->injectCurrentToNeurons(type, current)` - External input
- `brain->reset()` / `brain->initialize()` - State management

### Neural Dynamics (`dynamics/`)
**Purpose**: Real-time neural state computation and spike propagation

#### Key Components:
- **NeuralDynamics**: Continuous membrane potential updates
- **SpikeSystem**: Event-driven spike generation and propagation

#### Physics Model:
- LIF neuron equation: τₘ dv/dt = -v + I/ gₘ
- Membrane time constant (τₘ) determines integration speed
- Threshold crossing triggers spike output
- Refractory period prevents immediate re-firing

#### Spike Propagation:
- Event-driven (not time-stepped)
- Synaptic delays modeled as transmission lags
- Competition through inhibitory connections
- Temporal coding through precise spike timing

## 2. Cognitive Architecture

### Cognition System (`cognition/`)
**Purpose**: High-level cognitive functions and intelligent behavior

#### Key Components:
- **NeuralPlanner**: Action sequence evaluation and planning
- **ConceptFormation**: Abstract concept learning from neural patterns

#### Cognitive Functions:
- **Planning**: Evaluate action sequences for goal achievement
- **Concept Learning**: Extract patterns and form abstractions
- **Attention**: Competitive selection of neural representations
- **Self-Model**: Internal representation of agent's capabilities

#### Integration:
- Receives processed sensory input from sensory systems
- Stores learned concepts in semantic memory
- Generates motor commands via motor system
- Modulated by neuromodulators (reward, curiosity)

## 3. Memory Systems

### Working Memory (`memory/NeuralWorkingMemory.hpp`)
**Purpose**: Short-term active maintenance of information

#### Implementation:
- **Persistent Activity**: Recurrent connections maintain neural firing
- **Competitive Dynamics**: Winner-take-all mechanisms select representations
- **Theta Rhythm**: Temporal organization of memory updates

#### Key Features:
- Active maintenance of current sensory inputs
- Maintenance of motor plans
- Integration of prediction signals
- Rapid updates with new information

### Episodic Memory (`memory/NeuralEpisodicMemory.hpp`)
**Purpose**: Encoding and replay of specific experiences

#### Key Components:
- **Experience Encoding**: Neural patterns from events stored
- **Memory Replay**: Consolidation during rest/sleep phases
- **Temporal Indexing**: Time-based memory organization

#### Integration:
- Receives predictions from prediction system
- Provides ground truth for prediction error calculation
- Contributes to development through consolidation

### Semantic Memory (`memory/SemanticMemory.hpp`)
**Purpose**: Abstract conceptual knowledge and relationships

#### Key Features:
- Distributed representation of concepts
- Associative connections between related concepts
- Hierarchical organization of knowledge
- Abstraction from specific experiences

### Procedural Memory (`memory/ProceduralMemory.hpp`)
**Purpose**: Learned behavioral patterns and skills

#### Implementation:
- Hebbian strengthening of successful action patterns
- Structural plasticity creates dedicated pathways
- Reinforcement through reward signals

### Associative Memory (`memory/AssociativeMemory.hpp`)
**Purpose**: Pattern completion and association

#### Key Features:
- Pattern completion from partial cues
- Bidirectional associations (A→B and B→A)
- Noise tolerance through attractor dynamics

## 4. Learning and Plasticity

### Neuromodulation System (`neuromodulation/`)
**Purpose**: Global regulation of neural plasticity and behavior

#### Core Components:

#### Reward System (`Reward.hpp`)
- **Function**: Reinforcement learning signals
- **Mechanism**: Dopamine-like modulation of synaptic strength
- **Integration**: Affects plasticity rules throughout brain

#### Prediction Error (`PredictionError.hpp`)
- **Function**: Discrepancy detection
- **Mechanism**: Phasic signals when predictions are wrong
- **Integration**: Guides learning and exploration

#### Novelty Detection (`Novelty.hpp`)
- **Function**: Detection of unexpected information
- **Mechanism**: Based on prediction errors and surprise
- **Integration**: Drives exploration behavior

#### Curiosity (`Curiosity.hpp`)
- **Function**: Intrinsic motivation for exploration
- **Mechanism**: Internal drive for information seeking
- **Integration**: Affects action selection

### Plasticity Rules (`plasticity/`)
**Purpose**: Synaptic and structural learning mechanisms

#### Hebbian Plasticity (`Hebbian.hpp`)
- **Rule**: "Fire together, wire together"
- **Implementation**: Long-term potentiation (LTP)
- **Effect**: Strengthens co-activated synapses

#### STDP (`STDP.hpp`)
- **Rule**: Spike-timing dependent plasticity
- **Mechanism**: Pre-before-post → LTP, Post-before-pre → LTD
- **Effect**: Precise temporal learning

#### Structural Plasticity (`StructuralPlasticity.hpp`)
- **Rule**: Creation and elimination of neural connections
- **Mechanism**: Activity-dependent synaptogenesis and pruning
- **Effect**: Changes brain architecture over time

## 5. Sensory-Motor Loop

### Sensory System (`sensory/`)
**Purpose**: Perception of internal and external world

#### Key Components:
- **Vision**: Visual processing pipeline
- **Audio**: Sound processing
- **Internal Signals**: Internal state monitoring
- **SensoryInput**: Unified sensory interface

#### Processing Pipeline:
1. External stimuli → Sensory receptors
2. Transduction → Neural signals
3. Feature extraction → Pattern recognition
4. Integration → Percept formation

### Motor System (`motor/`)
**Purpose**: Generation and execution of actions

#### Key Components:
- **MotorSystem**: Action selection and coordination
- **Action**: Action parameters and execution

#### Action Selection:
- **Goal-Directed**: Based on rewards and predictions
- **Habitual**: Based on learned associations
- **Reflexive**: Hard-coded responses

### Environment System (`environment/`)
**Purpose**: Simulation of agent-environment interaction

#### Key Components:
- **Environment**: Abstract interface
- **Observation**: Perceived world state
- **Action**: Possible actions

#### World Model:
- **Perception**: What agent senses
- **Action Space**: What agent can do
- **Feedback Loop**: Consequences of actions

## 6. Integration and Development

### Development System (`development/`)
**Purpose**: Age-dependent changes and maturation

#### Key Components:
- **DevelopmentSystem**: Overall developmental trajectory
- **Synaptogenesis**: Activity-independent connection formation
- **Pruning**: Activity-dependent connection elimination
- **Maturation**: Systematic strengthening/weakening

#### Developmental Stages:
1. **Early**: High plasticity, exploration focus
2. **Middle**: Balanced learning and performance
3. **Late**: Specialization, reduced plasticity

### Experiment Framework (`experiments/`)
**Purpose**: Testing, validation, and benchmarking

#### Key Components:
- **Experiment**: Base experiment class
- **ExperimentRunner**: Experiment management
- **Metrics**: Performance measurement

#### Experiment Types:
- **Phase 3**: Basic neural computation
- **Phase 4**: Advanced cognitive functions
- **Phase 6**: Full integration testing

## 7. Supporting Systems

### Core Utilities (`core/`)
**Purpose**: Foundational infrastructure

#### Configuration (`core/Config/`)
- **Purpose**: Centralized parameter management
- **Features**: File loading, command-line overrides, validation
- **Sources**: Default, file, command-line

#### Random Number Generation (`core/Random/`)
- **Purpose**: Reproducible stochastic processes
- **Algorithm**: PCG64 with configurable seeds

#### Logging (`core/Logger/`)
- **Purpose**: Debug and monitoring
- **Levels**: Trace, Debug, Info, Warning, Error, Critical
- **Outputs**: Console, file, network

#### Simulation Clock (`core/SimulationClock/`)
- **Purpose**: Time management
- **Features**: Fixed timestep, elapsed time tracking

### Visualization (`visualization/`)
**Purpose**: Brain state visualization and debugging

#### Key Components:
- **VisualizationInterface**: Abstraction layer
- **Renderers**: Various visualization backends

## System Integration Patterns

### Brain-World Loop
```
SENSORIAL INPUT → AGENT PROCESSOR → BRAIN COMPUTATION → MOTOR OUTPUT → WORLD UPDATE
     ↑                                      ↓
ENVIRONMENT FEEDBACK ← REWARD SIGNALS ← MEMORY SYSTEMS ← PLASTICITY
```

### Information Flow
1. **Sensory Input** → Agent processes percepts
2. **Brain Computation** → Neural processing and decision making
3. **Motor Output** → Actions applied to environment
4. **World Feedback** → New sensory input + rewards
5. **Memory Update** → Consolidation and learning
6. **Plasticity** → Synaptic and structural changes

### Memory Integration
- **Working Memory**: Active maintenance of current state
- **Episodic Memory**: Recording specific experiences
- **Semantic Memory**: Abstract knowledge formation
- **Procedural Memory**: Behavioral pattern learning
- **Associative Memory**: Pattern completion

### Neuromodulation Integration
- **Reward**: Reinforcement of successful behaviors
- **Prediction Error**: Correction of incorrect predictions
- **Novelty**: Detection of interesting information
- **Curiosity**: Drive for exploration

## Component Dependencies

### Direct Dependencies
- **brain/** depends on: core, dynamics, plasticity, neuromodulation, memory
- **agent/** depends on: brain, sensory
- **environment/** depends on: core
- **world/** depends on: environment, brain

### Indirect Dependencies
- **cognition/** depends on: brain, memory
- **prediction/** depends on: brain, memory
- **sensory/** depends on: brain, environment
- **motor/** depends on: brain, environment

## Performance Considerations

### Computation-intensive Components
1. **Neuron update** - O(N²) for dense connectivity
2. **Spike propagation** - Event-driven, scales with activity
3. **Memory access** - Cache-friendly organization recommended

### Memory-intensive Components
1. **Brain connectivity** - Sparse matrices preferred
2. **Memory storage** - Pool allocation for performance
3. **Logging** - Buffered writes, log levels control

### Bottleneck Areas
1. **Synaptic updates** - Synchronization across components
2. **Plasticity computation** - Numerical precision vs. performance
3. **Integration points** - Data conversion and validation

## Development Guidelines

### Component Design
- **Single Responsibility**: Each component has one clear purpose
- **Loose Coupling**: Components communicate through well-defined interfaces
- **High Cohesion**: Related functionality grouped together
- **Abstraction**: Hide implementation details behind interfaces

### Testing Strategy
1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test component interactions
3. **System Tests**: Test complete brain functionality
4. **Performance Tests**: Validate real-time constraints

### Monitoring and Debugging
- Use logging for component state tracking
- Implement health checks for critical components
- Monitor resource usage (CPU, memory, I/O)
- Profile performance bottlenecks

## Phase-Specific Components

### Phase 3 Components (`experiments/Phase3Experiment.cpp`)
- Basic neural computation validation
- STDP and Hebbian learning verification
- Simple sensory-motor loops

### Phase 4 Components (`experiments/Phase4Experiment.cpp`)
- Advanced cognitive functions
- Working and episodic memory
- Complex planning and decision making

### Phase 6 Components (`experiments/Phase6IntegratedExperiment.cpp`)
- Full brain integration
- All cognitive subsystems operational
- Complete developmental trajectory
- Real-world task performance

## Future Extensibility

### Modular Design
- New sensory modalities can be added
- Additional memory types supported
- Extensible plasticity rules
- Pluggable neuromodulators

### Integration Points
- New brain regions can be added
- Custom neural models supported
- External world models compatible
- Plugin architecture for extensions

## Component Lifecycle

### Initialization Sequence
1. **Core** → **Configuration** → **Random** → **Logger** → **Clock**
2. **Brain** → **Memory Systems** → **Neuromodulation**
3. **Sensory** → **Motor** → **Environment** → **World**
4. **Agent** → **Experiments**

### Shutdown Sequence
1. **Experiments** → **Agent** → **World** → **Environment**
2. **Sensory** → **Motor** → **Neuromodulation**
3. **Memory Systems** → **Brain**
4. **Core** → **Logger** → **Random** → **Config**

## Component Communication

### Synchronous Calls
- **Direct method calls** between tightly coupled components
- **Event-driven updates** for temporal processes

### Asynchronous Communication
- **Message queues** for decoupled components
- **Callback systems** for notification patterns

### Data Sharing
- **Memory pointers** for efficient access
- **Immutable copies** for safety
- **Observer pattern** for notification

This guide provides comprehensive documentation for understanding, developing, and integrating the major components of the NLM artificial brain system. Each subsystem has well-defined interfaces and clear responsibilities, enabling modular development and systematic testing.
