# NLM C++ API Reference

## Overview

The NLM C++ API provides direct access to the brain simulation engine with full control over neural dynamics, plasticity, and cognitive processes.

## Brain Class

### Class Definition

```cpp
class Brain {
public:
    // Construction and Initialization
    Brain(const Config& config);
    ~Brain();
    
    void initialize();
    void step(SimulationStep step, Timestamp time = 0.0);
    void reset();
    
    // Configuration Access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // Statistics and Monitoring
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Sensory-Motor Interface
    void receiveSensoryInput(const SensoryInput& input);
    Action produceAction() const;
    
    // State Management
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Region Management
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Development
    float getDevelopmentalStage() const;
    void updateDevelopment(float timeStep);
    
    // Memory Systems (Phase 2)
    WorkingMemory* getWorkingMemory();
    EpisodicMemory* getEpisodicMemory();
    SemanticMemory* getSemanticMemory();
    ProceduralMemory* getProceduralMemory();
    
    // Prediction System (Phase 2)
    PredictionSystem* getPredictionSystem();\n    
    // Step Information
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
};
```

### Key Methods

#### Construction and Initialization

**`Brain(const Config& config)`**

Creates a new brain instance with the given configuration.

*Parameters:*
- `config`: Configuration object containing brain parameters

*Example:*
```cpp
Config config;
config.set("brain.neuron_count", 1000);
config.set("brain.synapse_density", 0.1);
brain = std::make_unique<Brain>(config);
```

**`void initialize()`**

Initializes the brain with neurons, synapses, and all cognitive systems. Must be called before simulation.

*Example:*
```cpp
brain->initialize();  // Sets up all neural populations and connections
```

#### Simulation Control

**`void step(SimulationStep step, Timestamp time = 0.0)`**

Executes one simulation step with neural computation, plasticity updates, and memory operations.

*Parameters:*
- `step`: Current simulation step number (for logging and indexing)
- `time`: Current simulation time in milliseconds (default: 0.0)

*Example:*
```cpp
for (SimulationStep step = 0; step < maxSteps; ++step) {
    brain->step(step, step * timestep);
    // Process results
}
```

**`void reset()`**

Resets the brain to its initial state while preserving configuration.

*Example:*
```cpp
brain->reset();  // Clear all activity, reset neurons to resting state
```

#### Configuration Access

**`const Config& getConfig() const`**

Returns reference to the brain's configuration.

**`void setConfig(const Config& config)`**

Updates brain configuration. Note that some parameters cannot be changed after initialization.

#### Statistics and Monitoring

**`size_t getTotalNeuronCount() const`**

Returns total number of neurons in the brain.

**`size_t getTotalSynapseCount() const`**

Returns total number of synaptic connections.

**`size_t getFiringNeuronCount() const`**

Returns number of neurons currently firing in this step.

**`float getAverageFiringRate() const`**

Returns average firing rate across all neurons (spikes per second).

**`float getExcitationInhibitionRatio() const`**

Returns E/I balance ratio (excitatory/inhibitory activity).

#### Sensory-Motor Interface

**`void receiveSensoryInput(const SensoryInput& input)`**

Receives sensory data from the environment and converts to neural activity.

*Parameters:*
- `input`: Sensory input data (vision, proprioception, internal state)

**`Action produceAction() const`**

Converts neural activity to motor commands.

*Returns:* Action to be executed in the environment

#### State Management

**`bool save(const std::string& filepath) const`**

Saves current brain state to file for persistence and recovery.

*Parameters:*
- `filepath`: Path to save file

*Returns:* `true` if successful, `false` otherwise

**`bool load(const std::string& filepath)`**

Loads brain state from file, restoring neural activity and cognitive states.

*Parameters:*
- `filepath`: Path to save file

*Returns:* `true` if successful, `false` otherwise

#### Region Management

**`RegionId addRegion(const std::string& name)`**

Adds a new neural region to the brain.

**`NeuralRegion* getRegion(RegionId id)`**

Gets mutable reference to a neural region by ID.

#### Development

**`float getDevelopmentalStage() const`**

Returns current developmental stage (0.0 to 1.0).

**`void updateDevelopment(float timeStep)`**

Advances developmental stage based on elapsed time.

### Configuration Parameters

The following configuration parameters affect brain behavior:

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Probability of connection between neurons |
| `brain.connection_probability` | float | 0.05 | Target connectivity density |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Memory Systems API

### WorkingMemory Class

```cpp
class WorkingMemory {
public:
    void store(NeuralPattern pattern, float priority = 1.0f);
    NeuralPattern retrieve(size_t index = 0);
    size_t size() const;
    void clear();
    void update(float timeStep);
};
```

### EpisodicMemory Class

```cpp
class EpisodicMemory {
public:
    void store(Experience experience);
    Experience retrieve(const TimeWindow& window);
    void replay(float consolidationStrength = 1.0f);
    void consolidate();
};
```

### PredictionSystem Class

```cpp
class PredictionSystem {
public:
    void train(const std::vector<Observation>& observations);
    float predictNext(const std::vector<Observation>& context);
    float getPredictionError() const;
    void update(float timeStep);
};
```

---

## Development System API

### DevelopmentSystem Class

```cpp
class DevelopmentSystem {
public:
    void update(float timeStep);
    float getCurrentStageProgress() const;
    DevelopmentalStage getCurrentStage() const;
    void setPlasticityRate(float rate);
};
```

## Configuration Class API

### Config Class

```cpp
class Config {
public:
    // Query Operations
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& prefix) const;
    
    // Value Retrieval
    template<typename T> T get(const std::string& key) const;
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    float getFloat(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Value Setting
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, float value);
    void set(const std::string& key, bool value);
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Configuration Hierarchy
    Config* getSubConfig(const std::string& prefix);
    const Config* getSubConfig(const std::string& prefix) const;
};
```

## Design Patterns and Architecture Notes

### PIMPL Pattern

The `Brain` class uses the Pimpl idiom to hide implementation details:
- Public interface defined in header files
- Implementation details in `.cpp` files
- Forward declarations of implementation classes
- Reduces compilation dependencies

### Event-Driven Architecture

NLM uses an event-driven architecture for:
- **Spike Events**: Neural spike propagation with delays
- **Plasticity Events**: Synaptic weight updates triggered by neural activity
- **Development Events**: Age-based system updates
- **Memory Events**: Storage and retrieval operations

### Component-Based Design

The brain is organized into layers:
1. **Core Layer**: Basic types and utilities
2. **Dynamics Layer**: Neural computation and spike processing
3. **Plasticity Layer**: Learning rules and structural changes
4. **Development Layer**: Growth and maturation
5. **Neuromodulation Layer**: Gating and modulation signals
6. **Memory Layer**: Storage and retrieval systems
7. **Prediction Layer**: Temporal sequence learning
8. **Cognitive Layer**: Higher-level processing

### Memory Management

- **Resource Management**: Smart pointers for automatic cleanup
- **Memory Pools**: Pre-allocated memory for performance-critical structures
- **Event Queues**: Deferred operations for deterministic behavior
- **Sparse Connectivity**: Efficient storage of neural connections

## Error Handling

### Exception Hierarchy

```cpp
class NLMException : public std::runtime_error {
public:
    enum class Type { Configuration, Memory, Simulation, I/O };
    NLMException(Type type, const std::string& message);
    Type getType() const;
};

class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message);
};

class SimulationException : public NLMException {
public:
    SimulationException(const std::string& message);
};
```

### Error Reporting

Use the logging system for runtime errors:

```cpp
#include "core/Logger.hpp"

NLM_LOG_ERROR("Failed to load brain from " + filepath);
NLM_LOG_WARN("Using default value for " + key);
```

## Thread Safety

- **Thread-Safe Operations**: Configuration access, statistics reading
- **Non-Thread-Safe Operations**: Brain simulation, state modification
- **Recommended Usage**: One simulation thread per brain instance

## Performance Considerations

### Optimization Strategies

1. **SIMD Vectorization**: Neural computations use SIMD instructions
2. **Memory Pools**: Reduce dynamic allocation overhead
3. **Event-Driven Processing**: Only process active neurons
4. **Sparse Connectivity**: Store only existing synapses
5. **Cache Locality**: Optimize memory access patterns

### Profiling and Benchmarking

```bash
# Run performance benchmarks
cd build
./nlm_test --benchmark=all

# Profile with Linux perf
perf record -g ./nlm_phase6_demo
perf report
```

## Extending the C++ API

### Adding New Plasticity Rules

```cpp
class MyPlasticityRule : public PlasticityRule {
public:
    void update(Synapse& synapse, Neuron* pre, Neuron* post, 
                float timeSincePre, float timeSincePost) override;
    
    static PlasticityRule* create();
};
```

Register with plasticity system:

```cpp
void registerMyPlasticityRule() {
    PlasticityFactory::registerRule("my_rule", &MyPlasticityRule::create);
}
```

### Adding New Neuron Types

```cpp
class MyNeuronType : public NeuralDynamics {
public:
    void initialize(Neuron* neuron) override;
    void step(Neuron* neuron, float dt) override;
    bool shouldSpike(Neuron* neuron) override;
    void onSpike(Neuron* neuron) override;
    
    static NeuralDynamics* create();
};
```

Register with neuron factory:
```cpp
void registerMyNeuronType() {
    NeuralDynamicsFactory::registerType("my_neuron", &MyNeuronType::create);
}
```

## Migration Guide

### From NLM Phase 5 to Phase 6

Key changes in Phase 6:

1. **Memory Integration**: Memory systems are now connected to the main brain loop
2. **Prediction Integration**: Prediction system processes sensory input
3. **Enhanced Neuromodulation**: Full dopamine and other neuromodulator effects
4. **Development Effects**: Development now affects plasticity rules and attention

Migration steps:

```cpp
// Phase 5 code (memory systems disconnected)
WorkingMemory* wm = brain->getWorkingMemory();  // Returns nullptr

// Phase 6 code (memory systems integrated)
if (auto* wm = brain->getWorkingMemory()) {
    wm->store(pattern, priority);
}
```

### Backward Compatibility

- Most public APIs remain unchanged
- New cognitive systems are opt-in
- Default behavior preserved unless configuration changed
- Phase 6 integration is gradual, not immediate
