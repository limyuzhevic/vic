# NLM (Neural Learning Machine) - Architecture Documentation

## Overview

This document provides comprehensive documentation of the NLM (Neural Learning Machine) framework architecture. NLM is an experimental computational brain project designed to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## System Architecture Overview

### Core Architecture Principles

1. **No ML Frameworks**: NLM implements its own neural computation rather than relying on machine learning frameworks
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention instead of Transformer QKV attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

### System Architecture Diagram

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

## System Components

### 1. Core Layer

The Core Layer provides fundamental types, configuration management, and simulation utilities:

#### Core Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **Types** | Strongly-typed identifiers | NeuronId, SynapseId, RegionId, etc. |
| **Config** | Configuration management system | Parameter validation, history tracking |
| **Random** | Deterministic random number generator | Reproducible random numbers |
| **Logger** | Structured logging system | Event logging, debugging |
| **Clock** | Time management for simulation | Timestamp, step management |

#### Core Technologies

- **C++20**: Modern C++ with constexpr, modules, coroutines
- **Template Metaprogramming**: Compile-time computations and type safety
- **RAII**: Resource acquisition is initialization for memory management
- **STL**: Standard Template Library for containers and algorithms

### 2. Brain Layer

The Brain Layer contains the main neural computation components:

#### Primary Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **Brain** | Central coordinator | Integration of all systems |
| **Neuron** | Individual neuron representation | LIF dynamics, spike generation |
| **Synapse** | Synaptic connection representation | Event-driven transmission |
| **NeuralPopulation** | Group of neurons with shared properties | Population-level dynamics |
| **NeuralRegion** | Brain region with multiple populations | Regional connectivity |

#### Brain Architecture

```cpp
class Brain {
public:
    // Core computation
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp time);
    
    // Input/output
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    
    // Access to subsystems
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    // ... and more
};
```

### 3. Dynamics Layer

The Dynamics Layer implements neural computation and spike processing:

#### Dynamics Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **NeuralDynamics** | Abstract base for neuron dynamics | Extensible dynamics models |
| **SpikeSystem** | Event-driven spike processing | Efficient spike propagation |

#### Dynamics Technologies

- **Event-Driven Architecture**: Only process neurons that spike
- **Spike Queues**: Prioritized spike processing
- **Delay Lines**: Synaptic delay implementation
- **Vectorization**: SIMD optimization for performance

### 4. Plasticity Layer

The Plasticity Layer implements synaptic learning rules:

#### Plasticity Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **PlasticityRule** | Abstract base for plasticity rules | Template for new rules |
| **STDP** | Spike-Timing-Dependent Plasticity | Bi-directional synaptic learning |
| **Hebbian** | Hebbian learning | Activity-dependent strengthening |
| **StructuralPlasticity** | Synapse formation/removal | Network topology changes |

#### Plasticity Mechanisms

- **STDP**: Spike-timing dependent potentiation/depression
- **Hebbian Learning**: Co-activation based strengthening
- **Structural Plasticity**: Synaptogenesis and pruning

### 5. Development Layer

The Development Layer implements developmental stages that affect plasticity:

#### Development Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **DevelopmentSystem** | Orchestrates development | Stage progression |
| **Synaptogenesis** | New synapse formation | Network growth |
| **Pruning** | Weak synapse removal | Network refinement |
| **Maturation** | Adult property development | Functional specialization |

#### Developmental Stages

1. **Initial**: High plasticity, rapid learning
2. **CriticalPeriod**: Sensitive window for learning
3. **Maturation**: Decreased plasticity, fine-tuning
4. **Adult**: Stable performance with limited learning

### 6. Neuromodulation Layer

The Neuromodulation Layer provides learning signals and motivation:

#### Neuromodulation Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **Neuromodulator** | Abstract base for neuromodulators | Template for new signals |
| **Dopamine** | Reward and reinforcement | RL signaling |
| **Novelty** | Novelty detection | Exploration drive |
| **Curiosity** | Exploration motivation | Intrinsic reward |
| **PredictionError** | Error signals | Learning modulation |

#### Neuromodulation Functions

- **Dopamine**: Reward prediction error, reinforcement learning
- **Novelty**: Detection of unexpected events
- **Curiosity**: Exploration motivation, intrinsic reward
- **PredictionError**: Discrepancy between expected and actual outcomes

### 7. Memory Layer (Phase 4)

The Memory Layer implements working memory, episodic memory, and associative memory:

#### Memory Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **NeuralWorkingMemory** | Persistent activity for temporary storage | Active maintenance |
| **NeuralEpisodicMemory** | Experience encoding in neural patterns | Sequence storage |
| **NeuralAssociativeMemory** | Experience-based associations | Pattern completion |
| **ConceptFormation** | Pattern discovery without labels | Abstract representation |

#### Memory Technologies

- **Persistent Activity**: Recurrent neural connections
- **Pattern Completion**: Content-addressable retrieval
- **Neural Grounding**: Memory grounded in neural patterns

### 8. Prediction Layer (Phase 4)

The Prediction Layer implements temporal sequence learning and prediction:

#### Prediction Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **NeuralPrediction** | Temporal sequence learning | Sequence prediction |
| **ActionConsequencePredictor** | Action effect prediction | Forward planning |
| **PredictionErrorSignal** | Error computation for learning | Learning modulation |

#### Prediction Functions

- **Sequence Learning**: Temporal pattern discovery
- **Forward Prediction**: Anticipating action consequences
- **Error Signals**: Learning from prediction errors

### 9. Cognitive Layer (Phase 4)

The Cognitive Layer implements higher-level cognitive functions:

#### Cognitive Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **AttentionalSelection** | Competitive selection dynamics | Feature binding |
| **NeuralPlanner** | Predictive action selection | Multi-step planning |
| **SelfModel** | Sensorimotor self-awareness | Body representation |
| **SocialLearning** | Observation and imitation | Social cognition |

#### Cognitive Technologies

- **Competitive Selection**: Lateral inhibition, winner-take-all
- **Multi-step Prediction**: Planning multiple actions ahead
- **Forward Models**: Prediction of sensory consequences

### 10. Sensory Layer

The Sensory Layer processes external and internal inputs:

#### Sensory Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **SensoryInput** | Base class for sensory data | Input abstraction |
| **Vision** | Visual input processing | Image processing |
| **Audio** | Audio input processing | Sound processing |
| **InternalSignals** | Internal state signals | State monitoring |

#### Sensory Technologies

- **Multimodal Processing**: Integration of multiple sensory modalities
- **Feature Extraction**: Extraction of relevant features
- **Noise Reduction**: Filtering of irrelevant information

### 11. Motor Layer

The Motor Layer converts neural activity into actions:

#### Motor Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **Action** | Action representation | Action abstraction |
| **MotorSystem** | Neural-to-action conversion | Output generation |

#### Motor Technologies

- **Inverse Kinematics**: Mapping neural activity to movements
- **Action Selection**: Choosing appropriate actions
- **Smooth Control**: Continuous action generation

### 12. Environment Layer

The Environment Layer provides the interface for interaction:

#### Environment Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **Environment** | Abstract environment interface | Environment abstraction |
| **Observation** | Sensory observation wrapper | Observation packaging |
| **Action** | Action wrapper | Action packaging |

#### Environment Technologies

- **Discrete Time**: Step-by-step environment interaction
- **State Space**: Well-defined environment states
- **Action Space**: Well-defined action possibilities

### 13. Experiments Layer

The Experiments Layer provides tools for scientific experiments:

#### Experiment Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **Experiment** | Single experiment recording | Experiment management |
| **ExperimentRunner** | Experiment execution | Batch processing |
| **Metrics** | Data analysis | Statistical analysis |
| **Phase4Experiment** | Phase 4 cognitive experiments | Cognitive testing |

#### Experiment Technologies

- **Data Collection**: Systematic data recording
- **Analysis Tools**: Statistical and visual analysis
- **Reproducibility**: Ensuring experimental reproducibility

### 14. Visualization Layer

The Visualization Layer provides tools for visualizing neural activity:

#### Visualization Components

| Component | Description | Key Functions |
|-----------|-------------|---------------|
| **Visualizer** | Main visualization interface | Multiple output formats |
| **NeuronViewer** | Individual neuron visualization | Spike visualization |
| **PopulationViewer** | Population activity visualization | Heatmaps |
| **NetworkViewer** | Network topology visualization | Graph visualization |

#### Visualization Technologies

- **2D/3D Visualization**: Multi-dimensional representation
- **Real-time Updates**: Live visualization during simulation
- **Export Support**: Multiple output formats (PNG, PDF, interactive)

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

### AgentBrain Components

1. **SensoryTransduction**: Converts world percepts to neural input
2. **MotorDecoding**: Converts neural activity to motor commands
3. **Neuromodulation**: Applies reward signals to plasticity
4. **Development**: Updates developmental stage
5. **Attention**: Selective processing through competition
6. **Planning**: Uses predictions to select actions

### AgentBrain Architecture

```cpp
class AgentBrain {
public:
    // Initialization
    void initialize(const Environment& world);
    
    // Sensory processing
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor decoding
    std::unique_ptr<Action> decodeMotorCommand() const;
    
    // Neuromodulation
    void applyRewardModulation(double reward, double predictedReward);
    
    // Development
    void updateDevelopment(Timestamp dt);
    
    // Subsystem control
    void enableRewardModulation(bool enabled);
    void enableStructuralPlasticity(bool enabled);
    void enableDevelopment(bool enabled);
    void enableCuriosity(bool enabled);
    
    // Statistics
    double getCuriosityLevel() const;
    double getNoveltyLevel() const;
    double getPredictionError() const;
    double getNeuromodulationLevel() const;
};
```

## Design Patterns

### 1. Composite Pattern

Used for neural population and region composition:

```cpp
class NeuralRegion {
    std::vector<NeuralPopulation> populations;
    // ... other components
};

class NeuralPopulation {
    std::vector<Neuron> neurons;
    // ... other components
};
```

### 2. Observer Pattern

Used for event-driven spike processing:

```cpp
class SpikeObserver {
public:
    virtual void onSpike(NeuronId neuron, Timestamp time) = 0;
};

class SpikeSystem {
    std::vector<SpikeObserver*> observers;
    // ... spike processing logic
};
```

### 3. Strategy Pattern

Used for different plasticity rules:

```cpp
class PlasticityRule {
public:
    virtual void applyPlasticity(NeuronId pre, NeuronId post, double weight) = 0;
};

class STDP : public PlasticityRule {
    // STDP specific implementation
};

class Hebbian : public PlasticityRule {
    // Hebbian specific implementation
};
```

### 4. Chain of Responsibility

Used for neural input processing:

```cpp
class InputProcessor {
public:
    virtual std::unique_ptr<InputProcessor> next() const = 0;
    virtual void process(Input input) = 0;
};

class SensoryProcessor : public InputProcessor {
    // Sensory input processing
};
```

### 5. Facade Pattern

Used for brain system access:

```cpp
class Brain {
public:
    // Simple interface to access complex subsystems
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    // ... other facade methods
};
```

## Architectural Principles

### 1. Separation of Concerns

Each component has a single, well-defined responsibility:

- **Core**: Fundamental types and utilities
- **Dynamics**: Neural computation
- **Plasticity**: Learning mechanisms
- **Memory**: Storage and retrieval
- **Prediction**: Temporal processing
- **Cognition**: Higher-level functions
- **Sensory**: Input processing
- **Motor**: Output generation
- **Environment**: Interaction interface

### 2. Information Hiding

Implementation details are hidden behind well-defined interfaces:

```cpp
// Hide implementation details
class Brain {
private:
    // Complex internal implementation
    std::vector<Neuron> neurons;
    std::vector<Synapse> synapses;
    // ... other implementation details
public:
    // Simple public interface
    void step(SimulationStep step);
    std::unique_ptr<Action> produceAction();
    // ... other public methods
};
```

### 3. Loose Coupling

Components are loosely coupled through interfaces:

```cpp
// Interface for neural systems
class NeuralSystem {
public:
    virtual void update(Timestamp dt) = 0;
    virtual std::string getName() const = 0;
};

// Concrete implementation
class WorkingMemory : public NeuralSystem {
    // Implementation details
};
```

### 4. High Cohesion

Related functionality is grouped together:

```cpp
// Group related plasticity rules
class PlasticityLayer {
public:
    void applySTDP(NeuronId pre, NeuronId post, double weight);
    void applyHebbian(NeuronId pre, NeuronId post, double weight);
    void applyStructuralPlasticity(NeuronId pre, NeuronId post);
    // ... other plasticity methods
};
```

### 5. Single Responsibility

Each class has one reason to change:

```cpp
class Neuron {
public:
    // Handle only neuron dynamics
    void integrateInputs(const std::vector<double>& inputs);
    bool shouldFire(double threshold) const;
    void reset() const;
    // ... other neuron-specific operations
};
```

## Technology Stack

### Backend Technologies

| Layer | Technology | Rationale |
|-------|------------|-----------|
| **Core** | C++20 | Performance, type safety, modern features |
| **Build System** | CMake | Cross-platform, modern build system |
| **Python Bindings** | pybind11 | High-performance Python integration |
| **Testing** | Google Test | Comprehensive testing framework |
| **Documentation** | Markdown | Human-readable, portable format |

### Performance Optimization

#### Memory Management

1. **Memory Pools**: Pre-allocated memory for performance
2. **Object Reuse**: Reuse objects to reduce allocation overhead
3. **Cache Locality**: Optimize memory access patterns

#### Computation Optimization

1. **SIMD Instructions**: Vectorized operations for performance
2. **Event-Driven Processing**: Only process neurons that spike
3. **Lazy Evaluation**: Defer computations until necessary
4. **Parallel Processing**: Multi-threading for large simulations

#### Data Structures

1. **Hash Tables**: Fast lookup for neuron/synapse access
2. **Skip Lists**: Efficient ordered data structures
3. **Vector Containers**: Compact storage, good cache locality
4. **Linked Lists**: Flexible dynamic structures

## Scalability Considerations

### Small Scale (100-1000 neurons)

- Single-threaded execution
- In-memory storage
- Real-time simulation possible

### Medium Scale (1000-10000 neurons)

- Multi-threading support
- Out-of-core storage for some data
- Approximate algorithms for some computations

### Large Scale (10000-100000 neurons)

- Distributed computing
- Out-of-core storage
- Approximation algorithms
- Parallel spike processing

### Very Large Scale (>100000 neurons)

- Distributed memory systems
- Specialized hardware acceleration
- Approximation algorithms
- Sub-sampling for analysis

## Extensibility

### Adding New Components

1. **Follow Existing Patterns**: Use similar architecture to existing components
2. **Implement Abstract Base Classes**: Ensure compatibility with existing systems
3. **Register with Core**: Add to registration system for easy discovery
4. **Update Documentation**: Document new components in the architecture guide

### Adding New Plasticity Rules

```cpp
// Template for new plasticity rules
class MyPlasticityRule : public PlasticityRule {
public:
    void applyPlasticity(NeuronId pre, NeuronId post, double weight) override {
        // Custom plasticity implementation
    }
};

// Register with plasticity system
void registerMyPlasticityRule() {
    PlasticitySystem::registerRule("MyPlasticity", []() -> PlasticityRule* {
        return new MyPlasticityRule();
    });
}
```

### Adding New Memory Systems

```cpp
// Template for new memory systems
class MyMemorySystem : public MemorySystem {
public:
    void store(const Pattern& pattern, float confidence) override {
        // Custom storage implementation
    }
    
    Pattern retrieve(size_t index) const override {
        // Custom retrieval implementation
    }
    
    std::vector<Pattern> retrieveSimilar(const Pattern& pattern, 
                                         float threshold) const override {
        // Custom similarity search
    }
};
```

## Validation and Verification

### Component Testing

Each component undergoes comprehensive testing:

1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test component interactions
3. **System Tests**: Test complete system functionality
4. **Performance Tests**: Test performance characteristics
5. **Stress Tests**: Test system robustness

### Verification Methods

1. **Static Analysis**: Code analysis for correctness
2. **Dynamic Analysis**: Runtime behavior testing
3. **Property Testing**: Test system properties with random inputs
4. **Fuzz Testing**: Test with random inputs to find edge cases
5. **Regression Testing**: Ensure no functionality regression

## Maintenance and Extension

### Code Quality Standards

1. **Documentation**: Document all public APIs
2. **Comments**: Add meaningful comments for complex code
3. **Naming Conventions**: Use consistent naming conventions
4. **Error Handling**: Handle errors gracefully with meaningful messages
5. **Testing**: Maintain comprehensive test coverage

### Version Control

1. **Semantic Versioning**: Follow semantic versioning for releases
2. **Branching Strategy**: Use feature branches for new development
3. **Pull Requests**: Review and merge changes through pull requests
4. **Continuous Integration**: Automated testing and validation

### Documentation Updates

1. **API Documentation**: Update API reference documentation
2. **Architecture Documentation**: Update system architecture documentation
3. **User Guide**: Update user guides and examples
4. **Developer Guide**: Update developer documentation
5. **Release Notes**: Document changes in each release

## Future Directions

### Research Areas

1. **Biologically Realistic Models**: Implement more realistic neuron models
2. **Multi-scale Modeling**: Integrate different scales of neural modeling
3. **Neuromodulation**: Expand neuromodulation capabilities
4. **Cognitive Architecture**: Develop more sophisticated cognitive models
5. **Learning Theory**: Integrate formal learning theory

### Performance Goals

1. **Speed**: Achieve real-time simulation for larger networks
2. **Scalability**: Support networks with millions of neurons
3. **Energy Efficiency**: Reduce energy consumption per operation
4. **Hardware Acceleration**: Utilize specialized hardware
5. **Distributed Computing**: Support distributed simulation

### Application Areas

1. **Neuroscience**: Study neural systems and their properties
2. **Artificial Intelligence**: Develop intelligent systems
3. **Robotics**: Create adaptive robots
4. **Education**: Develop educational tools
5. **Research**: Support computational neuroscience research

## Conclusion

The NLM framework is designed as a modular, extensible, and maintainable computational brain simulation system. Its architecture supports:

1. **Scientific Investigation**: Study neural systems and their properties
2. **Artificial Intelligence**: Develop intelligent systems
3. **Research and Development**: Support computational neuroscience research

The modular design allows for easy extension, maintenance, and validation of the system while maintaining high performance and biological realism.

## References

- [NLM API Reference](API_REFERENCE.md)
- [NLM User Guide](USER_GUIDE.md)
- [NLM Configuration Documentation](CONFIGURATION.md)
- [NLM Python Bindings](PYTHON_BINDINGS.md)
- [NLM Testing Documentation](TESTING.md)
- [NLM Examples and Tutorials](EXAMPLES.md)
- [NLM Upgrading Guide](UPGRADING_GUIDE.md)

## See Also

- [NLM Science Documentation](SCIENCE.md)
- [NLM Experiment Documentation](EXPERIMENTS.md)
- [NLM Performance Documentation](PERFORMANCE.md)
- [NLM Phase 4 Architecture](PHASE4_ARCHITECTURE.md)

## License

MIT License - See LICENSE file for details.