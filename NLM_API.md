# NLM C++ API Documentation

## Overview

NLM (熙然) is a high-performance spiking neural network simulation framework designed for artificial developmental systems. This documentation provides comprehensive API reference for the C++ implementation.

## Main Components

### 1. Core System

#### Config.h
- Configuration management system with type-safe parameter setting
- Supports file loading, command-line arguments, and default values
- Type: Config (main configuration class)

```cpp
std::make_shared<Config>();
config->loadFromFile("config.cfg");
config->set("neuron_count", 500, ConfigSource::Default);
```

#### Logger.h
- Structured logging system with multiple output targets
- Type: Logger, ConsoleLogger
- Methods: logInfo(), logError(), addLogger()

#### Random.h
- Deterministic random number generator with seed control
- Type: RandomGenerator

#### SimulationClock.h
- High-precision time management for discrete simulations
- Type: SimulationClock
- Methods: getTime(), getCurrentStep()

### 2. Neural Computation

#### Brain.h
- Central coordinator for neural population management
- Manages multiple regions, neurons, and synaptic connections
- Key methods: initialize(), step(), getTotalSpikeCount()

#### Neuron.h
- Leaky Integrate-and-Fire (LIF) neuron model
- Implements: membrane potential, threshold, refractory period
- Methods: stepLIF(), injectCurrent(), recordSpike()

#### Synapse.h
- Synaptic connection with weight management and plasticity
- Properties: weight, delay, plasticity flags
- Methods: getWeight(), setWeight(), applyWeightChange()

### 3. Plasticity System

#### PlasticityRule.h (Abstract Base)
```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on neural activity
    virtual void update(Synapse* synapse, 
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) = 0;
    
    // Apply synaptic weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Enable/disable plasticity rule
    bool isEnabled() const;
    void setEnabled(bool enabled);
};
```

#### Hebbian.h
- Implementation of Hebbian learning rule
- "Neurons that fire together, wire together"
- Methods: setLearningRate(), getLearningRate()

#### STDP.h
- Spike-Timing-Dependent Plasticity implementation
- Asymmetric LTP/LTD based on spike timing
- Methods: configure(), setLTPWeight(), setLTDWeight()

#### StructuralPlasticity.h
- Dynamic synapse formation and elimination
- Methods: enableSynaptogenesis(), enablePruning()

### 4. Environment System

#### Environment.h (Abstract Base)
```cpp
class Environment {
public:
    virtual std::unique_ptr<SensoryInput> reset() = 0;
    virtual std::unique_ptr<SensoryInput> step(const Action& action) = 0;
    virtual const EnvironmentState& getState() const = 0;
    virtual float getLastReward() const = 0;
    virtual bool isDone() const = 0;
    virtual const char* getName() const = 0;
};
```

#### SimpleWorld.h
- Grid-world environment for testing and development
- Provides visual, tactile, and proprioceptive inputs

### 5. Agent System

#### AgentBrain.h
- Interface between NLM brain and external world
- Handles sensory transduction and motor decoding
- Methods: processSensoryInput(), decodeMotorCommand()

### 6. Memory Systems

#### NeuralWorkingMemory.h
- Persistent neural activity for temporary information storage
- Competitive dynamics for information prioritization

#### NeuralEpisodicMemory.h
- Neural pattern storage for experience encoding
- Pattern completion and replay capabilities

### 7. Neuromodulation

#### Neuromodulator.h (Abstract Base)
- Abstract base for behavioral modulation
- Includes: Dopamine, Novelty, Curiosity, PredictionError

### 8. Cognitive Systems

#### NeuralPlanner.h
- Predictive action selection with multiple-step evaluation
- Uses learned sequences for planning

#### ConceptFormation.h
- Pattern discovery without predefined labels
- Prototype formation and category learning

## Initialization Patterns

### Basic Brain Setup
```cpp
// Create configuration
auto config = std::make_shared<Config>();
config->set("neuron_count", 500);
config->set("random_seed", 42);

// Initialize brain
auto brain = std::make_shared<Brain>(config);
if (!brain->initialize()) {
    std::cerr << "Failed to initialize brain!" << std::endl;
    return 1;
}
```

### Complete Simulation
```cpp
// Create world
auto world = std::make_shared<SimpleWorld>();
world->configure(20, 20, 8, 8);
world->reset();

// Create agent
auto agent = std::make_shared<AgentBrain>(brain);
agent->initialize(world);
agent->enableRewardModulation(true);
agent->enableCuriosity(true);

// Run simulation
for (int step = 0; step < 1000; ++step) {
    world->update(0.1);  // Update environment
    
    auto percept = world->getSensoryPercept();  // What the agent sees
    agent->processSensoryInput(percept);       // Brain processes it
    
    brain->step(step);                         // Brain thinks
    
    auto action = agent->decodeMotorCommand(); // Brain decides action
    world->applyMotorCommand(action, world->getSimulationTime()); // Do action
}
```

## Performance Considerations

### Memory Management
- All neural components use RAII for automatic memory management
- Smart pointers prevent memory leaks
- Configurable allocation pools for high-frequency operations

### SIMD Optimization
```cpp
// Performance-critical operations support SIMD
float computePotential[] = {...};
float result = simd_reduce(computePotential, simd_count);
```

### Parallel Processing
- Independent neural populations can run in parallel
- Thread-safe spike propagation system
- Lock-free data structures for high concurrency

## Extensibility

### Adding New Plasticity Rules
```cpp
class CustomPlasticityRule : public PlasticityRule {
public:
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom implementation
    }
    
    const char* getName() const override {
        return "CustomRule";
    }
};
```

### Creating New Environments
```cpp
class CustomEnvironment : public Environment {
public:
    std::unique_ptr<SensoryInput> reset() override {
        return std::make_unique<CustomSensoryInput>();
    }
    
    // Implement other virtual methods...
};
```

## Advanced Usage

### Checkpoint System
```cpp
// Save brain state
brain->saveToCheckpoint("brain_state.checkpoint");

// Load brain state
brain->loadFromCheckpoint("brain_state.checkpoint");
```

### Development System
```cpp
// Enable developmental progression
agent->enableDevelopment(true);

// Get developmental stage
int stage = brain->getDevelopmentalStage();
```

### Replay and Consolidation
```cpp
// Replay important memories
brain->startMemoryReplay();
brain->replayExperience(memory_id);
brain->endMemoryReplay();
```

## Configuration Parameters

Common configuration options include:
- `neuron_count`: Number of neurons
- `region_count`: Number of brain regions
- `simulation_timestep`: Simulation time step (seconds)
- `connection_probability`: Probability of connection formation
- `random_seed`: Deterministic seed
- `stdp_ltp_weight`, `stdp_ltd_weight`: STDP parameters
- `synaptogenesis_rate`, `pruning_rate`: Structural plasticity rates

## Error Handling

NLM uses exceptions for error reporting:
- `std::invalid_argument` for invalid parameters
- `std::runtime_error` for simulation errors
- Custom exceptions for NLM-specific conditions

## Testing and Validation

NLM provides comprehensive test framework:
- Unit tests for all core components
- Integration tests for system interactions
- Performance benchmarks
- Memory usage validation

## Building and Installation

### Dependencies
- C++20 compiler
- CMake 3.16+
- Python bindings (optional)

### Build Instructions
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Tests
```bash
ctest --output-on-failure
```

## License

MIT License. See LICENSE file for details.