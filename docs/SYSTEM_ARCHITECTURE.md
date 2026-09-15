# NLM System Architecture Documentation

## Overview

This document provides a comprehensive overview of the NLM (Neural Learning Machine) system architecture. NLM is an experimental artificial brain project designed to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

The system integrates multiple neural subsystems including neural dynamics, plasticity, memory, prediction, cognition, neuromodulation, development, and sensory-motor processing into a coherent artificial brain architecture.

## Architecture Overview

The NLM brain operates as a closed-loop system with the following major components:

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

## Core Layer

### 1. Types and Core Utilities

The core layer provides fundamental types and utilities:

**Strongly-typed Identifiers**
- `NeuronId`: Unique identifier for neurons
- `SynapseId`: Unique identifier for synapses
- `RegionId`: Unique identifier for brain regions
- `PopulationId`: Unique identifier for neuron populations

**Configuration System**
- `Config`: Key-value configuration with support for JSON, YAML, command-line, and runtime settings
- Configuration values support: int, int64_t, double, bool, string, vectors of primitives
- Type-safe getters with optional returns and default values

**Random Number Generation**
- `RandomGenerator`: Deterministic random number generator with configurable seeds
- Supports multiple distributions (uniform, normal, etc.)

**Logging System**
- `Logger`: Structured logging with multiple output formats (console, file, etc.)
- Supports different log levels (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- Thread-safe logging for parallel processing

**Time Management**
- `SimulationClock`: Manages simulation time, step counting, and time conversions
- Supports fixed and variable timestep simulations
- Provides timing statistics and profiling capabilities

### 2. Neural Computation (Phase 2-3)

#### Neural Core
- **LIF Neurons**: Leaky Integrate-and-Fire neurons with biological realism
- **Spike System**: Event-driven spike propagation with precise timing
- **Synaptic Transmission**: Excitatory/inhibitory synapses with delays and plasticity
- **Neural Populations**: Groups of neurons with shared properties
- **Neural Regions**: Hierarchical organization of neural tissue

#### Plasticity Mechanisms
- **STDP (Spike-Timing-Dependent Plasticity)**: Pairwise spike timing-dependent weight changes
- **Hebbian Learning**: Coactivity-based strengthening
- **Structural Plasticity**: Synapse formation and elimination
- **Reward-Modulated Plasticity**: Integration with reward signals

### 3. Neuromodulation (Phase 3-4)

#### Neuromodulator System
- **Dopamine**: Reward prediction error, reinforcement learning, motivation
- **Curiosity**: Exploration drive, novelty detection
- **Novelty**: Change detection, surprise signals
- **Prediction Error**: Mismatch between expected and actual outcomes

#### Neuromodulation Architecture
```
Dopamine System:
├── Temporal Difference Learning
├── Eligibility Traces
├── Burst/Pause Dynamics
└── Learning Rate Modulation

Curiosity System:
├── Novelty Detection
├── Exploration Policy
└── Reward Prediction

Novelty System:
├── Change Detection
├── Pattern Matching
└── Signal Amplification

Prediction Error:
├── Forward Models
├── Prediction Errors
└── Credit Assignment
```

## Cognitive Layer (Phase 4)

### Memory Systems

#### Working Memory
- **Neural Working Memory**: Persistent activity using recurrent connections
- **Capacity Management**: LRU/LFU eviction policies, compression, consolidation
- **Importance-based Prioritization**: Dynamic allocation based on relevance
- **Competition Mechanisms**: Lateral inhibition for selective retention

#### Episodic Memory
- **Experience Encoding**: Neural pattern storage of events
- **Pattern Completion**: Retrieval from partial cues
- **Temporal and Spatial Indexing**: Organized memory access
- **Replay Mechanism**: Consolidation during rest/sleep phases

#### Associative Memory
- **Pattern Associations**: Links between related experiences
- **Concept Formation**: Abstract representation discovery
- **Generalization**: Transfer of learned patterns
- **Retrieval Optimization**: Efficient access to stored memories

### Prediction Systems

#### Neural Prediction
- **Temporal Sequence Learning**: Prediction of future states
- **Action Consequence Prediction**: Expected outcomes of actions
- **Multi-step Prediction**: Extended horizon forecasting
- **Confidence Tracking**: Uncertainty estimation

#### Cognitive Prediction
- **Action Planning**: Selection of optimal action sequences
- **Self-Model**: Internal representation of agent's capabilities
- **Theory of Mind**: Understanding others' states and intentions
- **Social Learning**: Observation and imitation of others

### Cognitive Functions

#### Attention
- **Competitive Selection**: Winner-take-all dynamics
- **Top-down and Bottom-up Bias**: Goal-directed and stimulus-driven attention
- **Resource Allocation**: Limited processing resources
- **Modulatory Integration**: Neuromodulatory influence on attention

#### Planning
- **Predictive Processing**: Evaluation of potential futures
- **Goal-directed Behavior**: Action selection based on objectives
- **Hierarchical Planning**: Multi-level action decomposition
- **Adaptive Planning**: Learning from experience

#### Self-Model
- **Body Schema**: Internal representation of agent's body
- **Agency**: Sense of control over actions
- **Capability Representation**: Learned limits and strengths
- **Self-monitoring**: Awareness of own states and processes

## Developmental Layer

### Development System
- **Age-based Plasticity Modifiers**: Changing learning rates over development
- **Critical Period**: Sensitive learning windows
- **Maturation**: Progressive development toward adult properties
- **Homeostatic Regulation**: Maintaining stability

### Synaptogenesis
- **Activity-dependent Formation**: New synapses based on neural activity
- **Molecular Guidance**: Targeted synapse formation
- **Pruning Mechanisms**: Elimination of weak synapses
- **Structural Optimization**: Network refinement

### Synaptic Pruning
- **Use-it-or-lose-it**: Weak synapses eliminated
- **Competitive Elimination**: Limited resources allocation
- **Developmental Regulation**: Age-dependent pruning rates
- **Functional Reorganization**: Synaptic rewiring

### Maturation
- **Neuron Properties**: Changes in excitability, dynamics
- **Synaptic Properties**: Weight normalization, stability
- **Network Architecture**: Refinement of connectivity
- **Functional Specialization**: Regional specialization

## Sensory-Motor Integration

### Sensory Processing
- **SensoryInput**: Abstract base class for all sensory modalities
- **Vision**: Visual input processing (grayscale intensity grid)
- **Touch**: Proximity and collision detection
- **Internal Signals**: Homeostatic and physiological signals
- **Proprioception**: Body position and movement sensing
- **Audio**: Sound input processing (if enabled)

### Motor Processing
- **Action Representation**: High-level action descriptions
- **Motor Decoding**: Neural activity to action conversion
- **Motor System**: Output generation and execution
- **Agent Body**: Physical state and kinematics

### World Interface
- **SimpleWorld**: 2D simulation environment
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Interaction**: Action application and consequence processing

## Performance Infrastructure (Phase 5)

### Memory Management
- **Memory Pools**: Efficient allocation/deallocation
- **Cache Optimization**: Performance-critical data access patterns
- **SIMD Vectorization**: Parallel processing of neural computations
- **NUMA Awareness**: Memory locality optimization

### Parallel Processing
- **Thread Pool**: Concurrent execution management
- **Work Queues**: Task distribution and scheduling
- **Lock-free Data Structures**: High-performance concurrent data access
- **Load Balancing**: Even distribution of computational load

### Event-Driven Architecture
- **Event Queues**: Asynchronous event processing
- **Spike Events**: Precise timing of neural spikes
- **Delay Management**: Synaptic transmission delays
- **Callback System**: Dynamic event handling

## Data Flow and Integration

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

## Integration Points

### Memory Integration
- **Working Memory**: Updated each simulation step with firing neurons
- **Episodic Memory**: Stores experiences every 10 steps, replays every 100 steps
- **Associative Memory**: Integrated with episodic memory for pattern associations

### Neuromodulation Integration
- **Dopamine**: Modulates neural excitability and STDP rates
- **Novelty**: Detected from sensory patterns, affects curiosity
- **Curiosity**: Drives exploration decisions

### Brain Loop Integration
All systems now updated within `Brain::step()`:
1. Process spikes
2. Update neurons (LIF)
3. Detect spikes
4. Update working memory
5. Apply neuromodulation
6. Apply plasticity (STDP, Hebbian)
7. Store episodic memory
8. Update prediction
9. Update attention
10. Update concept formation
11. Structural plasticity
12. Replay
13. Development update
14. Consolidation
15. Checkpoint management

## Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Key Components and Interfaces

### Brain Interface
```cpp
// Central brain coordinator
class Brain {
public:
    // Core operations
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp time);
    
    // Sensory input
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Motor output
    std::unique_ptr<Action> produceAction();
    
    // Plasticity and development
    void updatePlasticity();
    void develop();
    
    // State management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // System access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Neuromodulation
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
};
```

### Agent-Brain Interface
```cpp
class AgentBrain {
public:
    // Sensory processing
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor decoding
    MotorCommand decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Development
    void updateDevelopment(double timestep);
    
    // Statistics
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Configuration
    void enableRewardModulation(bool enable);
    void enableCuriosity(bool enable);
    void enableDevelopment(bool enable);
};
```

### Memory System Interfaces
```cpp
class NeuralWorkingMemory {
public:
    // Storage and retrieval
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    std::vector<float> retrieve() const;
    bool contains(NeuronId neuron) const;
    
    // Update and maintenance
    void update(TimestepDuration dt);
    void strengthenMemory(float factor);
    void runCompetition();
};

class NeuralEpisodicMemory {
public:
    // Storage and retrieval
    void storeEpisode(const SensoryInput& sensoryInput,
                     const Action& action,
                     float reward,
                     Timestamp timestamp);
    
    // Replay and consolidation
    void replayEpisode(size_t episodeId, float strength = 1.0f);
    void replayAllEpisodes(float strength = 1.0f);
    void consolidateOldEpisodes(Timestamp cutoffTime);
};
```

### Neuromodulation Interfaces
```cpp
class Neuromodulator {
public:
    virtual float getLevel() const = 0;
    virtual float getPlasticityFactor() const = 0;
    virtual void update(TimestepDuration dt) = 0;
    virtual bool isActive() const = 0;
};

class Dopamine : public Neuromodulator {
public:
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void updateEligibilityTrace(float decayRate = 0.9f);
    float getEligibilityTrace() const;
};
```

## Performance Considerations

### Memory Usage
- **Memory Pools**: Pre-allocated memory blocks for performance
- **Sparse Connectivity**: Efficient storage of synaptic connections
- **Compression**: Memory-efficient storage of memory traces
- **Cache Locality**: Optimized data access patterns

### Computational Efficiency
- **Event-driven Architecture**: Only process spikes when they occur
- **SIMD Vectorization**: Parallel processing of neuron computations
- **Lazy Evaluation**: Defer computations until needed
- **Batch Processing**: Group similar operations together

### Scalability
- **NUMA Awareness**: Memory access optimization for multi-core systems
- **Lock-free Data Structures**: Concurrent access without bottlenecks
- **Work Stealing**: Dynamic load balancing
- **Hierarchical Memory**: Multi-level memory organization

## Extension Guidelines

### Adding New Neuromodulators
1. Implement `Neuromodulator` abstract base class
2. Register with `NeuromodulatorSystem`
3. Integrate with plasticity rules
4. Implement biological dynamics

### Adding New Plasticity Rules
1. Implement `PlasticityRule` abstract base class
2. Register with brain's plasticity system
3. Integrate with neuromodulation
4. Configure parameters from config

### Adding New Cognitive Systems
1. Implement system-specific interfaces
2. Integrate with brain's access methods
3. Ensure temporal compatibility
4. Test integration with existing systems

## Testing and Validation

### Unit Tests
- Individual component testing
- Interface validation
- Edge case testing
- Performance benchmarking

### Integration Tests
- Component interaction testing
- Data flow validation
- Performance under load
- Stress testing

### System Tests
- End-to-end functionality
- Memory retention testing
- Learning progress measurement
- Multi-system interaction validation

### Validation Metrics
- **Neural Dynamics**: Firing rates, spike timing accuracy
- **Learning Performance**: Convergence speed, accuracy
- **Memory Retention**: Decay rates, retrieval success
- **Prediction Accuracy**: Error rates, confidence calibration
- **Cognitive Performance**: Planning quality, concept stability

## Documentation and Examples

### API Documentation
- Comprehensive header file documentation
- Examples for all major classes
- Configuration parameter reference
- Performance tuning guidelines

### Tutorial Examples
1. **Basic Brain Simulation**: Simple neural network demonstration
2. **Agent in World**: Complete agent-world interaction
3. **Memory Systems**: Working memory and episodic memory usage
4. **Plasticity Learning**: STDP and Hebbian learning demonstrations
5. **Neuromodulation**: Reward and curiosity systems

### Best Practices
- **Configuration Management**: Use configuration files for parameter tuning
- **Memory Management**: Monitor memory usage in long simulations
- **Performance Profiling**: Identify and optimize bottlenecks
- **Testing**: Comprehensive testing for all components
- **Integration**: Test components together before deployment

## Future Extensions

### Phase 7: Advanced Cognition
- **Working Memory Enhancement**: Multiple memory buffers, chunking
- **Episodic Memory Expansion**: Temporal indexing, causal relations
- **Concept Abstraction**: Higher-level conceptual learning
- **Social Cognition**: Theory of mind, empathy simulation

### Phase 8: Biological Realism
- **Ion Channel Dynamics**: Hodgkin-Huxley neuron models
- **Dendritic Computation**: Complex dendritic integration
- **Cortical Architecture**: Hierarchical cortical processing
- **Neurovascular Coupling**: Resource allocation dynamics

### Phase 9: Advanced Integration
- **Cross-regional Dynamics**: Inter-regional communication
- **Multi-scale Modeling**: Linking micro and macro scales
- **Embodied Cognition**: Tight sensorimotor coupling
- **Developmental Trajectory**: Individual development paths

## Conclusion

The NLM system architecture provides a comprehensive framework for creating artificial brains with biological inspiration. The modular design allows for incremental development and integration of new capabilities while maintaining a coherent, coherent system architecture. The integration of memory, prediction, cognition, neuromodulation, and development systems creates a unified artificial brain capable of learning, adapting, and exhibiting complex cognitive behaviors.

The architecture balances biological realism with computational efficiency, enabling detailed investigation of neural mechanisms while remaining computationally tractable. The extensible design allows for future enhancements and integration of new scientific insights.

---

*This documentation covers NLM system architecture as of Phase 6. For specific implementation details,
refer to the source code headers and implementation files.*
