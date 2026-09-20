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

## Core Components

### 1. Core Layer (Modern C++2020 with Performance Focus)
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Thread-safe configuration with copy-on-write optimization, schema validation, history tracking
- **Random**: Deterministic thread-safe RNG with multiple distributions (uniform, normal, Poisson, exponential, Bernoulli)
- **Logger**: Structured logging with thread safety, validation, rotation, and performance optimizations
- **SimulationClock**: High-resolution timing system with TPS control

### 2. Brain Layer (Modern Architecture)
- **OptimizedBrain**: Event-driven, multithreaded, SIMD-optimized implementation with SoA and sparse connectivity
- **NeuralStateSoA**: Structure-of-Arrays for cache efficiency and vectorization
- **SynapseStateSoA**: Optimized synapse representation with circular buffers for STDP
- **Neuron**: Modern LIF implementation with performance optimizations
- **Synapse**: Efficient representation with eligibility traces
- **NeuralPopulation**: Concurrent population management with work-stealing
- **NeuralRegion**: Region-based organization with load balancing
- **Brain**: Central coordinator with thread-safe operations

### 3. Dynamics Layer (Event-Driven Architecture)
- **NeuralDynamics**: Abstract base with modern C++ patterns
- **SpikeSystem**: Event-driven spike processing with time-bucketed delays
- **EventQueue**: Thread-safe event system with priority queues
- **SparseConnectivity**: Efficient adjacency lists for large-scale networks

### 4. Plasticity Layer (Modern Learning)
- **PlasticityRule**: Abstract base with exception safety
- **Hebbian**: Modern implementation with thread safety
- **STDP**: High-performance with circular buffer history
- **StructuralPlasticity**: Synapse formation/removal with rate-based learning
- **EligibilityTrace**: Modern RAII-based implementation

### 5. Development Layer (Stage-Based Learning)
- **DevelopmentSystem**: Orchestrates developmental stages with plasticity modulation
- **Synaptogenesis**: Activity-dependent and probabilistic synapse formation
- **Pruning**: Competition-based and activity-dependent synapse elimination
- **Maturation**: Gradual property development with critical periods

### 6. Neuromodulation Layer (Modern Reinforcement Learning)
- **Neuromodulator**: Abstract base with multiple implementations
- **Dopamine**: Reward prediction error with eligibility traces
- **Novelty**: Change detection with adaptation
- **Curiosity**: Exploration-exploitation balance
- **PredictionError**: Temporal difference learning

### 7. Memory Layer (Phase 4 - Modern Cognitive Systems)
- **WorkingMemory**: Persistent activity with competitive selection
- **EpisodicMemory**: Pattern completion with temporal indexing
- **SemanticMemory**: Concept-based knowledge representation
- **ProceduralMemory**: Learned action sequences
- **AssociativeMemory**: Vector-based and neural-grounded associations
- **NeuralWorkingMemory**: Modern implementation with recurrent connections
- **NeuralEpisodicMemory**: Neural pattern storage with replay

### 8. Prediction Layer (Phase 4 - Modern Predictive Processing)
- **NeuralPrediction**: Temporal sequence learning with hierarchical prediction
- **ActionConsequencePredictor**: Forward model with state estimation
- **PredictionErrorSignal**: Predictive coding with Bayesian inference

### 9. Cognitive Layer (Phase 4 - Modern Cognitive Architecture)
- **AttentionalSelection**: Competitive dynamics without Transformer QKV
- **NeuralPlanner**: Predictive action selection with tree search
- **SelfModel**: Forward model with agency computation
- **SocialLearning**: Observation and imitation with theory of mind

### 10. Sensory Layer (Modern Sensory Processing)
- **SensoryInput**: Abstract interface with implementations
- **Vision**: Efficient 2D/3D vision processing
- **Audio**: Frequency-domain processing
- **InternalSignals**: Homeostatic and metabolic signals

### 11. Motor Layer (Modern Motor Control)
- **Action**: Action representation with inverse dynamics
- **MotorSystem**: Neural-to-action conversion with optimization

### 12. Environment Layer (Modern World Models)
- **Environment**: Abstract interface with implementations
- **Observation**: Efficient sensory observation wrapper
- **Action**: Action wrapper with validation

### 13. Experiments Layer (Modern Evaluation Framework)
- **Experiment**: Single experiment with comprehensive metrics
- **ExperimentRunner**: Parallel experiment execution
- **Metrics**: Statistical analysis and visualization
- **Phase4Experiment**: Full cognitive experiment suite

## Data Flow (Modern Implementation)

### Phase 2: Neural Computation
```
Sensory Input → Brain.receiveSensoryInput() → Neural Computation → Spike Processing → Plasticity → Structural Plasticity
```

### Phase 3: World Interaction
```
WORLD → SENSORY PERCEPT → AgentBrain.processSensoryInput() → Brain.receiveSensoryInput() → Neural Computation → Spike Processing → Motor Decoding → AgentBrain.decodeMotorCommand() → WORLD.applyMotorCommand() → ACTION RESULT → Reward Prediction Error → Neuromodulation → Eligibility Trace Update → Plasticity → Structural Plasticity → Development System
```

### Phase 4: Emerging Cognition
```
WORLD → ATTENTION → SENSORY INPUT → PREDICTION → WORKING MEMORY → EPISODIC MEMORY → CONCEPTS → PLANNING → SELF-MODEL → ACTION SELECTION → MOTOR OUTPUT → WORLD → REWARD / CONSEQUENCE → SOCIAL OBSERVATION → LEARNING UPDATES
```

## Agent-Brain Interface (Modern Design)

Phase 3/4 introduces the AgentBrain class with modern interfaces:

- **SensoryTransduction**: Converts world percepts to neural input with interpolation
- **MotorDecoding**: Converts neural activity to motor commands with optimization
- **Neuromodulation**: Applies reward signals with temporal credit assignment
- **Development**: Updates developmental stage with plasticity modulation
- **Attention**: Selective processing through competitive dynamics
- **Planning**: Uses predictions with search algorithms

## Key Design Principles (Modern Software Engineering)

1. **Modern C++2020**: Uses contracts, modules, coroutines, and ranges
2. **Performance First**: Cache-friendly data layouts, SIMD, parallelization
3. **Thread Safety**: Fine-grained locking, atomic operations, lock-free structures
4. **Exception Safety**: Strong exception guarantee, RAII, resource acquisition
5. **Memory Management**: Custom allocators, memory pools, efficient reuse
6. **Extensibility**: Clear patterns, plugin architecture, composition over inheritance
7. **Testing**: Unit testing, integration testing, performance validation
8. **Determinism**: Explicit seeding, reproducible experiments
9. **No ML Frameworks**: Pure neural computation without external dependencies
10. **No Transformer Architecture**: Spiking dynamics and competitive attention only
11. **Biological Inspiration**: Computationally inspired, not biologically accurate
12. **Modularity**: Clear interfaces, separation of concerns
13. **Data-Oriented Design**: Cache-friendly, vectorized operations
14. **Concurrent Processing**: Work-stealing, thread-local state
15. **No Predefined Knowledge**: All learning from experience

## Phase 4 Key Innovations (Modern Cognitive Architecture)

### Prediction System
- Learns temporal sequences from experience
- Predicts action consequences
- Multi-step prediction capability with forward models
- Error signals modulate learning with predictive coding

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

## Performance Optimizations (Modern Implementation)

### 1. Event-Driven Processing
- Only active neurons consume computation
- Efficient spike event queues with time-bucketing
- Deferred spike delivery with processing chains

### 2. Structure-of-Arrays (SoA)
- Contiguous neuron state arrays
- Better cache utilization
- Improved SIMD vectorization

### 3. Sparse Connectivity
- Adjacency lists instead of dense matrices
- Efficient synapse lookup
- Reduced memory for large networks

### 4. Multithreading
- Parallel population updates
- Work-stealing load balancing
- Thread-local state accumulation

### 5. Memory Efficiency
- Pre-allocated neuron/synapse pools
- Reduced heap fragmentation
- Compact data structures

## Memory Management (Modern Techniques)

### 1. Custom Allocators
- Memory pools for neuron/synapse objects
- Object reuse with ownership transfer
- Lock-free allocators for hot paths

### 2. Smart Pointers
- use_unique_for_ptr <std::shared_ptr> where appropriate
- Minimal overhead with custom deleters
- Cyclic references avoided

### 3. Memory Monitoring
- Leak detection with stack traces
- Fragmentation tracking
- Real-time memory profiling

## Threading and Concurrency (Modern Patterns)

### 1. Fine-Grained Locking
- Shared mutexes for read operations
- Unique mutexes for write operations
- Atomic operations for counters

### 2. Lock-Free Algorithms
- Ring buffers for spike queues
- Atomic flags for state changes
- Lock-free hash tables for lookups

### 3. Work Stealing
- Parallel population processing
- Load balancing across threads
- Thread-local accumulation

### 4. Thread-Safe Collections
- Concurrent queues for events
- Lock-free stacks for neurons
- Thread-safe maps for state

## Debugging and Profiling (Modern Tools)

### 1. Performance Profiling
- Time-series profiling with export
- Memory leak detection
- Thread utilization tracking

### 2. Event Profiling
- Event queuing latency
- Spike delivery timing
- Component processing times

### 3. Memory Profiling
- Allocation tracking
- Fragmentation analysis
- Component memory usage

### 4. Debugging Tools
- Validation with descriptive errors
- Structured logging with metadata
- Interactive debugging interfaces

## Migration Guide (For Older Versions)

### From Version 0.1 to Version 1.0

1. **Configuration System**: Migration from simple config to thread-safe Config class
2. **Logging**: Migration from basic logging to structured logging
3. **Random Number Generation**: Migration from std::mt19937 to RandomGenerator
4. **API Changes**: Updated method signatures and new features

### Modern C++ Migration Patterns

1. **RAII**: Replace manual memory management with smart pointers
2. **Move Semantics**: Use std::move where appropriate
3. **Range-Based For Loops**: Modern iteration patterns
4. **Auto and Deduction**: Type inference for cleaner code
5. **Lambda Expressions**: Compact function objects
6. **constexpr**: Compile-time computations where possible
7. **[[nodiscard]]: Attribute usage for better API design
8. **Coroutines**: Asynchronous operations for performance

## Future Directions

### 1. Advanced Optimizations
- SIMD intrinsics for specific operations
- GPU acceleration for matrix operations
- NUMA-aware memory allocation

### 2. Enhanced Cognitive Models
- More sophisticated prediction systems
- Advanced social learning mechanisms
- Complex planning algorithms

### 3. Modern Tooling
- Integration with modern IDEs
- Better visualization tools
- Automated testing frameworks

## Technology Stack

### Core Libraries
- **C++2020 Standard**: Modern language features
- **nlohmann/json**: Configuration and data interchange
- **Standard Threading**: std::thread, std::mutex, std::atomic

### Performance Tools
- Custom profiling system
- Benchmarking framework
- Memory profiling tools
- Performance counters

### Build System
- CMake 3.16+ with modern features
- Static library for core components
- Separate libraries for different layers

## Conclusion

The NLM architecture combines modern software engineering practices with brain-inspired computing. It provides a solid foundation for implementing and experimenting with different learning algorithms, neural dynamics, and cognitive architectures. The design emphasizes performance, correctness, and extensibility while maintaining a clear separation of concerns and biological plausibility.