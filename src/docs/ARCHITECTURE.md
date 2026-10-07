# NLM Architecture Documentation

## Overview

The NLM (Neural Learning Machine) is designed as a modular computational brain with a clear separation of concerns and well-defined interfaces between components. This architecture enables incremental development, testing, and extension of the system while maintaining biological plausibility and computational efficiency.

## System Architecture Overview

### Core Architecture Philosophy

NLM follows these key design principles:

1. **No ML Frameworks**: Implements its own neural computation from first principles
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

### Layered Architecture

NLM is organized into distinct layers that represent different aspects of brain function:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM BRAIN                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   SENSORY   │───▶│    BRAIN    │───▶│    MOTOR    │          │
│  │   INPUT    │    │   CORE      │    │    OUTPUT   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  SENSORY    │    │  MEMORY     │    │   ACTION    │          │
│  │  PROCESSING │    │  SYSTEMS   │    │  SELECTION  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  PREDICTION │                              │
│                    │   SYSTEM    │                              │
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

## Component Layers

### 1. Core Layer (Foundation)

The core layer provides the fundamental building blocks and utilities:

#### Types
- **Strong identifiers**: NeuronId, SynapseId, RegionId, PopulationId
- **Numeric types**: SimulationStep, Timestamp, MembranePotential, SynapticWeight
- **Enumerated types**: NeuronType, SynapseType, DevelopmentalStage, FiringState
- **Structures**: SpikeEvent, DelayedSpikeEvent, PlasticityFlags

#### Configuration
- **Config class**: Key-value configuration management
- **BrainConfig structure**: High-level brain configuration
- **Configuration templates**: Default, learning, development, optimized

#### Random Generation
- **Deterministic random**: Reproducible simulations
- **Seed management**: Explicit control over random state

#### Logging and Debugging
- **Structured logging**: Multi-level logging system
- **Real-time debugging**: Performance monitoring and debugging
- **Progress tracking**: Step-by-step execution monitoring

#### Simulation Time
- **Time management**: Discrete event simulation
- **Timestamps**: Absolute and relative time tracking
- **Timestep control**: Variable time step support

### 2. Neural Computation Layer (Core Brain)

This layer implements the fundamental neural computation:

#### Brain
- **Central coordinator**: Manages all neural processes
- **LIF neuron dynamics**: Leaky Integrate-and-Fire computation
- **Event-driven spikes**: Asynchronous spike propagation
- **Synaptic connections**: Weighted, delayed, plastic connections
- **Plasticity systems**: STDP, Hebbian, structural plasticity

#### Neuron
- **Membrane potential dynamics**: Integrate-and-fire computation
- **Spike generation**: Action potential generation and reset
- **Refractory period**: Absolute refractory period
- **Neuron types**: Excitatory, inhibitory, sensory, motor, modulatory

#### Synapse
- **Synaptic transmission**: Weighted current injection
- **Synaptic types**: Excitatory, inhibitory, electrical, modulatory
- **Plasticity control**: Enable/disable plasticity mechanisms
- **Delay management**: Transmission delays

#### Spike System
- **Event-driven processing**: Spike propagation
- **Delay handling**: Spike timing and delivery
- **Spike counting**: Activity statistics
- **Spike routing**: Connection management

#### Neural Population and Region
- **Population grouping**: Neurons with shared properties
- **Region organization**: Spatial and functional organization

### 3. Plasticity Layer

This layer implements all learning mechanisms:

#### Plasticity Rule Base Class
- **Abstract interface**: Common plasticity API
- **Weight modification**: Synaptic weight changes
- **Eligibility traces**: Temporal credit assignment

#### STDP (Spike-Timing-Dependent Plasticity)
- **Pre-post pairing**: Spike timing dependent changes
- **LTP/LTD mechanisms**: Long-term potentiation/depression
- **Time constants**: Asymmetric learning windows

#### Hebbian Learning
- **Fire together, wire together**: Correlated activity strengthening
- **Weight normalization**: Stability maintenance
- **Competition**: Lateral inhibition

#### Structural Plasticity
- **Synaptogenesis**: New synapse formation
- **Pruning**: Synapse elimination
- **Critical periods**: Developmentally regulated plasticity

### 4. Development Layer

This layer implements developmental processes:

#### Development System
- **Stage progression**: Initial → CriticalPeriod → Maturation → Adult → Aging
- **Plasticity modulation**: Age-dependent learning rates
- **Synaptogenesis control**: Activity-dependent growth
- **Pruning control**: Experience-dependent elimination

#### Synaptogenesis
- **Probabilistic formation**: New connection creation
- **Activity-dependent**: Correlated activity triggers
- **Resource constraints**: Limited synaptic resources

#### Pruning
- **Weak synapse elimination**: Activity-independent removal
- **Competitive processes**: Synaptic competition
- **Memory consolidation**: Strengthening important connections

#### Maturation
- **Adult properties**: Development of mature characteristics
- **Optimized performance**: Fine-tuning for efficiency
- **Stability**: Reduced plasticity for maintenance

### 5. Neuromodulation Layer

This layer implements behavioral modulation:

#### Neuromodulation Manager
- **Central coordinator**: Integrates all neuromodulators
- **Signal processing**: Reward, curiosity, novelty, prediction error
- **Effect application**: Modulates plasticity and dynamics
- **State management**: Maintains neuromodulator levels

#### Dopamine System
- **Reward prediction error**: Compares expected vs actual reward
- **Learning modulation**: Enhances plasticity for rewarded actions
- **Approach motivation**: Drives reward-seeking behavior

#### Novelty Detection
- **Change detection**: Identifies novel stimuli
- **Attention modulation**: Increases attention to novel inputs
- **Exploration drive**: Encourages investigation of new things

#### Curiosity System
- **Exploration motivation**: Intrinsic drive to explore
- **Intrinsic reward**: Rewards for information gain
- **Behavior selection**: Chooses novel/uncertain actions

#### Prediction Error System
- **Temporal prediction**: Anticipates future states
- **Error computation**: Detects prediction mismatches
- **Learning acceleration**: Drives faster learning from errors

### 6. Memory Layer (Phase 4)

This layer implements memory and cognition:

#### Working Memory
- **Persistent activity**: Maintains information actively
- **Competitive selection**: Winner-take-all dynamics
- **Decay mechanisms**: Temporal forgetting
- **Recurrent connections**: Self-sustaining activity

#### Episodic Memory
- **Experience storage**: Encodes events as neural patterns
- **Pattern completion**: Retrieves from partial cues
- **Replay mechanisms**: Reactivates past experiences
- **Temporal indexing**: Chronological organization

#### Associative Memory
- **Pattern associations**: Links related experiences
- **Hebbian clustering**: Groups similar patterns
- **Recall assistance**: Cues trigger related memories

#### Concept Formation
- **Pattern discovery**: Extracts structure from data
- **Prototype formation**: Creates stable representations
- **Stability measurement**: Quantifies consistency
- **Category learning**: Groups similar items

#### Attention System
- **Competitive selection**: Lateral inhibition dynamics
- **Top-down modulation**: Goal-directed selection
- **Bottom-up saliency**: Stimulus-driven attention
- **Winner-take-more**: Amplifies selected representations

#### Planning System
- **Multi-step prediction**: Simulates action sequences
- **Action evaluation**: Assesses action outcomes
- **Future optimization**: Chooses best actions
- **Adaptive planning**: Updates based on experience

#### Self-Model
- **Body schema**: Internal representation of body
- **Agency detection**: Distinguishes self vs external
- **Forward models**: Predicts action consequences

#### Social Learning
- **Observation**: Processes other agents' actions
- **Imitation**: Reproduces observed behaviors
- **Communication**: Learns shared signals

### 7. Prediction Layer (Phase 4)

This layer implements temporal prediction:

#### Neural Prediction
- **Sequence learning**: Learns temporal order
- **Action consequences**: Links actions to outcomes
- **Multi-step prediction**: Forecast distant futures
- **Hierarchical modeling**: Layered temporal representations

#### Prediction Error
- **Error computation**: Detects prediction mismatches
- **Learning signal**: Drives plasticity for prediction
- **Adaptive timing**: Adjusts prediction horizon

### 8. Sensory/Motor Layer

This layer interfaces with the environment:

#### Sensory Input
- **Base class**: Abstract sensory interface
- **Vision**: Visual input processing
- **Audio**: Auditory input processing
- **Internal signals**: Internal state monitoring

#### Motor System
- **Action selection**: Neural-to-motor conversion
- **Movement generation**: Produces physical actions
- **Feedback integration**: Processes action outcomes

### 9. Environment Layer

This layer simulates the external world:

#### Environment Base Class
- **Abstract interface**: Environment specification
- **Observation interface**: Access to world state
- **Action interface**: Application of actions

#### Simple World
- **2D environment**: Grid-based world simulation
- **Object management**: Resource and hazard objects
- **Agent tracking**: Position and state monitoring
- **Physics simulation**: Simple movement dynamics

### 10. Agent Layer

This layer connects brain to world:

#### Agent Brain
- **Sensory transduction**: Converts world percepts to neural input
- **Motor decoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals
- **Development**: Updates developmental stage

#### Agent Body
- **Physical representation**: Embodied agent
- **Sensing**: Perceptual capabilities
- **Acting**: Motor capabilities

### 11. Experiment Layer

This layer supports scientific investigation:

#### Experiment Framework
- **Protocol definition**: Structured experimental setup
- **Data recording**: Systematic observation
- **Metrics calculation**: Performance assessment
- **Statistical analysis**: Quantitative results

#### Experiment Runner
- **Execution control**: Manages experiment lifecycle
- **Data collection**: Records experimental data
- **Result analysis**: Computes and presents results

### 12. Performance Layer

This layer optimizes for efficiency:

#### Checkpoint Manager
- **State persistence**: Save/load brain state
- **Compression**: Memory-efficient storage
- **Validation**: Data integrity checking

#### Real-Time Debug System
- **Performance monitoring**: Resource usage tracking
- **Debugging tools**: Runtime inspection
- **Profiling**: Performance analysis

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
Development System (stage progression)
```

### Phase 4: Emerging Cognition

```
WORLD
      ↓
ATTENTION (competitive selection)
      ↓
SENSORY INPUT
      ↓
PREDICTION (learn temporal relationships)
      ↓
WORKING MEMORY (maintain information)
      ↓
EPISODIC MEMORY (store experiences)
      ↓
CONCEPTS (discover patterns)
      ↓
PLANNING (simulate futures)
      ↓
SELF-MODEL (predict self)
      ↓
ACTION SELECTION
      ↓
MOTOR OUTPUT
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT
      ↓
REWARD / CONSEQUENCE
      ↓
SOCIAL OBSERVATION (if others present)
      ↓
LEARNING UPDATES
```

## Interface Design

### Component Interfaces

Each major component follows a consistent interface pattern:

1. **Clear boundaries**: Minimal dependencies between layers
2. **Abstract base classes**: Define interfaces before implementations
3. **Virtual methods**: Allow for extensibility and testing
4. **Resource management**: RAII for automatic cleanup
5. **Error handling**: Comprehensive error reporting
6. **Configuration**: Flexible configuration options

### Communication Patterns

#### Message Passing
- **Event-driven**: Spike events propagate through system
- **Synchronous**: Method calls with immediate results
- **Asynchronous**: Background processing and monitoring

#### Data Flow
- **Pipeline processing**: Sequential data transformation
- **Feedback loops**: Cyclic dependencies for learning
- **Broadcast**: Global state updates

## Component Dependencies

### High-Level Dependencies

```
┌─────────────────────────────────────────────────────────────┐
│                    DEPENDENCY GRAPH                        │
├─────────────────────────────────────────────────────────────┤
│   ┌─────────┐    ┌─────────┐    ┌─────────┐                │
│   │   CORE  │───▶│   NEURAL│───▶│  MEMORY │                │
│   │ LAYER   │    │LAYER    │    │ LAYER   │                │
│   └─────────┘    └─────────┘    └─────────┘                │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │COGNITION   │                       │
│                   │   LAYER    │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │  PREDICTION │                       │
│                   │   LAYER    │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │NEUROMOD     │                       │
│                   │ MODULATION  │                       │
│                   │    LAYER    │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │ DEVELPOMENT │                       │
│                   │   LAYER    │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │  PLASTICITY │                       │
│                   │   LAYER    │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │  SENSORY   │                       │
│                   │   /MOTOR   │                       │
│                   │  LAYERS    │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │  ENVIRONMENT│                       │
│                   │    LAYER   │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │   AGENT    │                       │
│                   │   LAYER    │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │  EXPERIMENT│                       │
│                   │   LAYER    │                       │
│                   └─────────────┘                       │
│                            │                            │
│                            ▼                            │
│                   ┌─────────────┐                       │
│                   │  PERFORMANCE│                       │
│                   │   LAYER    │                       │
│                   └─────────────┘                       │
└─────────────────────────────────────────────────────────────┘
```

### Component Coupling

#### Low Coupling
- **Minimal shared state**: Components manage their own data
- **Clear interfaces**: Well-defined method signatures
- **Dependency inversion**: Depend on abstractions, not concretions

#### High Cohesion
- **Single responsibility**: Each component has focused purpose
- **Related functionality**: Related features grouped together
- **Clear boundaries**: Well-defined separation of concerns

## Performance Considerations

### Memory Management

1. **Data-oriented design**: Contiguous memory for better cache locality
2. **Object pooling**: Reuse memory blocks for efficiency
3. **RAII**: Automatic resource management
4. **Move semantics**: Efficient object transfer

### Computational Efficiency

1. **Event-driven**: Only process relevant events
2. **Vectorization**: SIMD operations for parallel computation
3. **Hierarchical processing**: Coarse-to-fine computation
4. **Adaptive resolution**: Adjust computation based on needs

### Scalability

1. **Parallel processing**: Multi-threading support
2. **Distributed computing**: Cluster support for large networks
3. **Load balancing**: Even distribution of computational load
4. **Hierarchical organization**: Handle large-scale systems

## Testing and Validation

### Unit Testing

1. **Component isolation**: Test individual components independently
2. **Mock interfaces**: Replace external dependencies
3. **Deterministic behavior**: Reproducible test results
4. **Edge cases**: Test boundary conditions and error cases

### Integration Testing

1. **Component interaction**: Test system-wide functionality
2. **Data flow**: Verify correct information flow
3. **Performance**: Ensure acceptable performance characteristics
4. **Reliability**: Test under various conditions

### Performance Testing

1. **Scalability testing**: Test with increasing problem sizes
2. **Resource usage**: Monitor memory and CPU consumption
3. **Benchmarking**: Compare against baseline implementations
4. **Stress testing**: Test under extreme conditions

## Extensibility and Customization

### Adding New Components

1. **Plugin architecture**: Load new components at runtime
2. **Template system**: Parameterized component generation
3. **Mix-in classes**: Add functionality without inheritance
4. **Configuration-driven**: Define behavior through configuration

### Customization Options

1. **Configuration files**: Modify behavior without code changes
2. **Parameter tuning**: Adjust algorithm parameters
3. **Algorithm selection**: Choose between different implementations
4. **Extension points**: Hook into system lifecycle

## Security and Safety

### Memory Safety

1. **RAII**: Automatic resource cleanup
2. **Bounds checking**: Prevent buffer overflows
3. **Null pointer safety**: Safe handling of pointers
4. **Exception safety**: Exception guarantees

### Runtime Safety

1. **Input validation**: Validate all external inputs
2. **State validation**: Check invariant violations
3. **Resource limits**: Prevent resource exhaustion
4. **Error recovery**: Graceful error handling

## Documentation and Maintainability

### Code Organization

1. **Clear structure**: Logical directory organization
2. **Comprehensive comments**: Document design decisions
3. **API documentation**: Complete interface documentation
4. **Examples**: Demonstrative code samples

### Maintenance Guidelines

1. **Minimal changes**: Make focused, small changes
2. **Code reviews**: Peer review for significant changes
3. **Testing**: Maintain comprehensive test coverage
4. **Documentation**: Keep documentation up-to-date

## Evolution and Future-Proofing

### Incremental Development

1. **Phase-by-phase rollout**: Add features incrementally
2. **Backward compatibility**: Maintain existing functionality
3. **Modular design**: Allow for feature evolution
4. **Clean architecture**: Support for future enhancements

### Research Integration

1. **Scientific validation**: Compare with biological findings
2. **Benchmarking**: Compare with alternative approaches
3. **Reproducibility**: Support for scientific replication
4. **Extension capabilities**: Allow for new research directions

## Conclusion

The NLM architecture represents a comprehensive approach to building a biologically-inspired computational brain. Its layered design, modular components, and clear interfaces enable systematic development, testing, and extension while maintaining biological plausibility and computational efficiency.

The architecture supports:

- **Scientific investigation**: Study of brain-inspired computation
- **Educational use**: Teaching principles of neural computation
- **Research experimentation**: Testing new hypotheses
- **Practical applications**: Real-world problem solving
- **Performance optimization**: Efficient large-scale simulation
- **Extensibility**: Integration of new features and components

This architecture provides a solid foundation for advancing our understanding of how intelligence can emerge from spiking neural dynamics while maintaining practical utility for a wide range of applications.