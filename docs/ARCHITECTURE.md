# NLM Architecture Documentation

## Overview

NLM (熙然) is a modular, biologically-inspired neural simulation framework designed to implement artificial developmental brains that can learn from experience. The architecture supports Phase 2 (real spiking neural computation) through Phase 6 (integrated artificial brain).

## Core Architecture Layers

### Phase 1: Configuration & Infrastructure
- **Config.hpp**: Configuration management system
- **Types.hpp**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Random.hpp**: Deterministic random number generation
- **Logger.hpp**: Structured logging infrastructure
- **SimulationClock.hpp**: Time management for simulation steps

### Phase 2: Real Neural Computation
- **Brain.hpp**: Central coordinator implementing integrated artificial brain
- **Brain.cpp**: Main brain simulation logic
- **Neuron.hpp/Neuron.cpp**: Individual LIF neuron implementation
- **Synapse.hpp/Synapse.cpp**: Synaptic connections with plasticity
- **NeuralRegion.hpp/NeuralRegion.cpp**: Brain regions with neural populations
- **NeuralPopulation.hpp/NeuralPopulation.cpp**: Groups of neurons with shared properties

### Phase 3: Event-Driven Processing
- **SpikeSystem.hpp/SpikeSystem.cpp**: Event-driven spike propagation and synaptic delay management
- **Dynamics.hpp**: Abstract base for neuron dynamics models

### Phase 4: Cognitive Integration
- **PredictionSystem.hpp/PredictionSystem.cpp**: Temporal sequence learning and prediction
- **Memory.hpp**: Integrated memory systems
- **WorkingMemory.hpp**: Persistent activity storage
- **EpisodicMemory.hpp**: Experience encoding
- **AssociativeMemory.hpp**: Pattern associations
- **Cognition.hpp**: Cognitive functions (attention, planning)

### Phase 5: Neuromodulation
- **Neuromodulator.hpp**: Abstract neuromodulator interface
- **Dopamine.hpp**: Reward prediction error signals
- **Novelty.hpp**: Novelty detection
- **Curiosity.hpp**: Exploration motivation
- **PredictionError.hpp**: Prediction mismatch signals

### Phase 6: Integrated Brain
- **DevelopmentSystem.hpp**: Brain maturation and stage progression
- **AgentBrain.hpp**: Interface connecting NLM brain to external world
- **Integration Architecture**: Complex integration of all systems

## Algorithmic Complexity Analysis

### Time Complexity

| Operation | Complexity | Details |
|-----------|------------|---------|
| Neuron Update (LIF) | O(1) | Constant time per neuron |
| Spike Detection | O(N_neurons) | Linear scan across all neurons |
| Synapse Processing | O(N_synapses) | Linear scan across all synapses |
| Plasticity Updates | O(N_synapses) | STDP and Hebbian rule application |
| Memory Operations | O(1) to O(M) | Working memory constant, episodic variable |
| Prediction System | O(T^2) | Sequence prediction over time window T |

### Space Complexity

- **Neurons**: O(N_neurons × N_features) where N_features includes membrane potential, refractory state, etc.
- **Synapses**: O(N_synapses × N_properties) where N_properties includes weight, delay, eligibility trace
- **Working Memory**: O(N_neurons_working) for active traces
- **Episodic Memory**: O(E × N_neurons_episodic) where E is number of episodes
- **Integration Overhead**: O(M × N_components) where M is number of integrated systems

### Memory Management

1. **Stack Allocation**: Fixed-size neural populations for cache efficiency
2. **Heap Allocation**: Dynamic structures for memory systems and large populations
3. **Pool Allocators**: Memory pools for frequently allocated structures (neurons, synapses)
4. **Reference Counting**: Shared_ptr for safe ownership transfer
5. **Custom Allocators**: Performance-optimized allocators for simulation hotspots

## Thread Safety Information

### Thread-Safe Components
- **Config**: Read-only after initialization, thread-safe for concurrent reads
- **Constants and Enums**: Immutable, thread-safe
- **Logger**: Thread-safe logging with queue-based architecture

### Non-Thread-Safe Components
- **Brain**: Not thread-safe - single simulation instance
- **Memory Systems**: Concurrent access requires external synchronization
- **Plasticity Rules**: State modifications require exclusive access

### Concurrency Considerations
- **Simulation Loop**: Single-threaded execution with precise timing guarantees
- **External Integration**: Thread-safe interfaces for sensory input/motor output
- **Parallel Processing**: SIMD vectorization for neuron updates where possible

## Performance Considerations

### Critical Paths
1. **Neuron Update Loop**: Inner loop with potential for optimization
2. **Spike Processing**: Event-driven vs polling trade-offs
3. **Plasticity Application**: Weight updates and eligibility trace management
4. **Memory Consolidation**: Periodic memory operations

### Optimization Strategies
1. **Data Layout**: Struct-of-Arrays (SoA) for numerical efficiency
2. **Cache Locality**: Contiguous memory access patterns
3. **SIMD Vectorization**: Parallel processing of neuron arrays
4. **Event-Driven**: Reduced unnecessary computations
5. **Memory Pools**: Reduced allocation overhead

### Scalability Limits
- **Neurons**: ~10^5 to 10^6 neurons (memory-bound)
- **Synapses**: ~10^6 to 10^7 synapses (computation-bound)
- **Time Steps**: Limited by precision requirements (typically 0.001s timestep)
- **Integration Depth**: More integrated systems increase overhead

## Integration Points and Dependencies

### Dependency Graph
```
┌─────────────────────────────────────────────────────────────┐
│                    Phase 2 (Core)                        │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   Neuron    │  │   Synapse   │  │  Population │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
│          │                  │                   │        │
│          └──────────────────┼───────────────────┘        │
│                           ▼                           │
│                 ┌─────────────────┐                   │
│                 │   Spike System  │                   │
│                 └─────────────────┘                   │
│                           │                           │
│                           ▼                           │
│                 ┌─────────────────┐                   │
│                 │   Brain         │                   │
│                 │  (Coordinator)  │                   │
│                 └─────────────────┘                   │
└──────────────────┬───────────────────────────────────────┘
                   │
                   ▼
   ┌───────────────────┬─────────────────────────────┬─────────────────┐
   │                   │                             │                 │
   ▼                   ▼                             ▼                 ▼
┌─────────────────┐ ┌─────────────────┐       ┌─────────────────┐ ┌─────────────────┐
│   Memory        │ │   Prediction    │       │   Neuromod.    │ │   Development  │
│   Systems        │ │   System        │       │   Signals       │ │   System        │
└─────────────────┘ └─────────────────┘       └─────────────────┘ └─────────────────┘
```

### Integration Points Details

1. **Sensory Integration**: Brain.receiveSensoryInput() directly injects current into sensory neurons
2. **Motor Interface**: Brain.produceAction() converts neural activity to motor actions
3. **Memory Access**: Brain.getWorkingMemory(), getEpisodicMemory(), getAssociativeMemory()
4. **Prediction Access**: Brain.getPredictionSystem() provides forward model capabilities
5. **Neuromodulation**: Brain.applyNeuromodulation() modulates learning rates
6. **Development**: Brain.develop() advances developmental stage and modifies plasticity

## Configuration Parameter Explanations

### Core Neural Parameters
- `neuron_count`: Total number of neurons (default: 1000)
- `region_count`: Number of brain regions (default: 1)
- `connection_probability`: Probability of synaptic connection (default: 0.1)
- `simulation_timestep`: Integration timestep (default: 0.001s)

### Plasticity Parameters
- `stdp_ltp_weight`: STDP long-term potentiation weight (default: 0.01)
- `stdp_ltd_weight`: STDP long-term depression weight (default: 0.012)
- `stdp_tau`: STDP time constant (default: 20.0)
- `synaptogenesis_rate`: Rate of new synapse formation (default: 0.0001)
- `pruning_rate`: Rate of synapse elimination (default: 0.00001)

### Memory Parameters
- `replay_interval`: Steps between memory replay (default: 100)
- `consolidation_interval`: Steps between memory consolidation (default: 1000)
- `working_memory_capacity`: Size of working memory (default: neuron_count/10)
- `max_episodes`: Maximum episodic memory storage (default: 1000)

### Neuromodulation Parameters
- `reward_scale`: Scaling factor for reward signals (default: 1.0)
- `novelty_threshold`: Novelty detection threshold
- `curiosity_gain`: Exploration motivation strength

## API Usage Examples and Patterns

### Basic Neural Simulation
```cpp
// Create configuration with custom parameters
auto config = std::make_shared<Config>();
config->set("neuron_count", 5000);
config->set("connection_probability", 0.15);
config->set("random_seed", 123);

// Initialize brain
auto brain = std::make_shared<Brain>(config);
if (!brain->initialize()) {
    std::cerr << "Failed to initialize brain!" << std::endl;
    return 1;
}

// Run simulation
for (SimulationStep step = 0; step < 1000; ++step) {
    brain->step(step);
    
    // Optional: Inject sensory input
    if (step % 100 == 0) {
        auto sensoryInput = std::make_unique<Vision>(64, 64, 3);
        // ... populate with data ...
        brain->receiveSensoryInput(*sensoryInput);
    }
    
    // Optional: Get action
    auto action = brain->produceAction();
}
```

### Integration with Agent Framework
```cpp
// Create agent-brain interface
auto agentBrain = std::make_shared<AgentBrain>(brain);

// Initialize with world environment
auto world = std::make_shared<SimpleWorld>();
world->configure(100, 100, 84, 84);
agentBrain->initialize(world);

// Process sensory input and get motor output
while (world->getAgentBody().health > 0) {
    // Get sensory percept from world
    auto percept = world->getSensoryPercept();
    
    // Process through agent-brain
    agentBrain->processSensoryInput(*percept);
    
    // Simulate neural computation
    brain->step(currentStep);
    
    // Get motor command from brain
    auto action = brain->produceAction();
    
    // Apply to world
    world->applyMotorCommand(action, currentStep);
    
    currentStep++;
}
```

### Learning Experiment Pattern
```cpp
// Create learning experiment
LearningExperiment experiment(brain, 42);

// Record initial state
experiment.recordInitialState();

// Enable plasticity
for (auto& syn : brain->getRegion(RegionId(1))->getSynapses()) {
    syn->enablePlasticity(true, true, false);
}

// Apply learning stimulus
for (SimulationStep step = 0; step < 1000; ++step) {
    // Inject patterned input to stimulate learning
    for (size_t i = 0; i < 20; ++i) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
    }
    
    brain->step(step);
}

// Record final state and analyze learning
experiment.recordFinalState();
experiment.computeStatistics();
```

### Performance Monitoring
```cpp
// Log brain status periodically
void logBrainPerformance(const std::shared_ptr<Brain>& brain) {
    brain->logStatus();
    
    // Get performance metrics
    size_t neurons = brain->getTotalNeuronCount();
    size_t synapses = brain->getTotalSynapseCount();
    size_t firing = brain->getFiringNeuronCount();
    float rate = brain->getAverageFiringRate();
    
    NLM_LOG_INFO("Performance: " + std::to_string(neurons) + " neurons, " +
                 std::to_string(synapses) + " synapses, " +
                 std::to_string(firing) + " firing, " +
                 std::to_string(rate) + " Hz avg");
}
```

## Configuration Loading Patterns

### File-based Configuration
```cpp
// Load from file with error handling
if (!config->loadFromFile("configs/experiment.cfg")) {
    NLM_LOG_WARNING("Using default configuration (file not found)");
    // Set reasonable defaults
    config->set("neuron_count", 1000);
    config->set("learning_rate", 0.01);
}
```

### Command-line Override
```cpp
// Parse command line arguments
int argc = ...;
char** argv = ...;
config->loadFromArgs(argc, argv);

// Command line examples:
// --neuron_count=5000
// -learning_rate 0.05
// --random_seed=42
// --simulation_timestep=0.0005
```

### Runtime Configuration
```cpp
// Modify configuration at runtime
config->set("learning_rate", 0.1, ConfigSource::Runtime);
config->set("novelty_threshold", 0.5, ConfigSource::Runtime);

// Temporary configuration
auto tempConfig = std::make_shared<Config>(*config);
tempConfig->set("neuron_count", 100);
// ... use temporary config ...
```

## Best Practices

### Memory Management
1. **Use smart pointers**: Prefer std::shared_ptr for ownership transfer
2. **Custom allocators**: Use memory pools for frequently allocated objects
3. **Clear references**: Ensure all smart pointers are released before large allocations
4. **Profile first**: Measure memory usage before optimizing

### Configuration Management
1. **Validation**: Validate configuration parameters at load time
2. **Documentation**: Document all configuration parameters with defaults
3. **Hierarchy**: Understand and respect configuration source hierarchy
4. **Testing**: Test configuration loading with various input formats

### Error Handling
1. **Graceful degradation**: Handle missing files, invalid parameters
2. **Logging**: Log configuration loading with source information
3. **Recovery**: Provide reasonable defaults when configuration fails
4. **Validation**: Validate neural parameters before initialization

### Performance
1. **Batch operations**: Process multiple neurons/synapses together
2. **Vectorization**: Use SIMD where possible
3. **Caching**: Cache frequently accessed data
4. **Profiling**: Profile simulation to identify bottlenecks

## Future Extensions

### Recommended Integration Points
1. **External Sensor Interface**: Add ROS, WebSocket, or other sensor interfaces
2. **Distributed Computing**: Implement MPI for distributed simulation
3. **GPU Acceleration**: Port computation to CUDA for massive scale
4. **Real-time Control**: Add real-time control interfaces for external systems
5. **Visualization**: Integrate with visualization frameworks

### Extension Patterns
1. **New Neuron Dynamics**: Implement additional LIF variants
2. **New Plasticity Rules**: Add Hebbian variants, homeostatic plasticity
3. **New Memory Systems**: Implement semantic memory, working memory with slots
4. **New Neuromodulators**: Add attention, locus coeruleus signals
5. **New Cognitive Modules**: Add decision making, reward learning

This architecture documentation provides a comprehensive guide to the NLM system's design, implementation details, and usage patterns. The modular design enables incremental development and extension while maintaining biological plausibility and computational efficiency.