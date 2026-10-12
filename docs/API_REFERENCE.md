# NLM API Reference

## Overview

This document provides complete reference documentation for the NLM (Neural Learning Machine) API. NLM is an experimental computational brain project that implements neural systems with spiking neurons, plasticity, memory, and cognition capabilities.

## Core Brain Class

### Brain Class (src/brain/Brain.hpp:51)

#### Constructor
```cpp
explicit Brain(std::shared_ptr<Config> config)
```

**Parameters:**
- `config`: Configuration object containing all brain parameters

**Description:** Creates a new brain instance with the given configuration.

**Thread Safety:** Not thread-safe. Each Brain instance must be used by a single thread.

**Memory Management:** Takes ownership of the configuration object.

#### Destructor
```cpp
~Brain()
```

**Description:** Destroys the brain instance and cleans up all resources.

#### Move Constructor and Assignment
```cpp
Brain(Brain&&) noexcept;
Brain& operator=(Brain&&) noexcept;
```

**Description:** Enables efficient transfer of brain ownership between instances.

#### Initialization
```cpp
bool initialize()
```

**Returns:** `true` if initialization succeeds, `false` otherwise.

**Description:** Initializes the brain with configuration parameters:
- Creates neural regions and neurons
- Sets up plasticity systems (STDP, Hebbian, structural)
- Initializes memory systems (working, episodic, associative)
- Configures prediction, cognition, and neuromodulation systems
- Sets up checkpoint management

**Thread Safety:** Must be called before any other operations. Thread-safe initialization.

**Memory Management:** Allocates all brain resources during initialization.

#### Main Simulation Loop
```cpp
void step(SimulationStep currentStep)
void step(SimulationStep currentStep, Timestamp currentTime)
```

**Parameters:**
- `currentStep`: Integer simulation step counter
- `currentTime`: Current simulation time in seconds

**Description:** Executes one simulation step implementing the complete brain loop:

1. **Process delayed spikes** - Deliver synaptic inputs from previous spikes
2. **Update neurons** - Integrate LIF dynamics for all neurons
3. **Detect spikes** - Queue spike events and update synaptic plasticity
4. **Working memory** - Update persistent activity and competition
5. **Neuromodulation** - Apply dopamine, curiosity, and novelty effects
6. **Plasticity** - Apply STDP and Hebbian learning rules
7. **Episodic memory** - Store current experience as memory episode
8. **Prediction system** - Update predictive models
9. **Cognition** - Attention, planning, concept formation
10. **Structural plasticity** - Synaptogenesis and pruning
11. **Replay** - Replay important memories
12. **Development** - Age-dependent plasticity modulation
13. **Consolidation** - Memory cleanup and consolidation
14. **Checkpoints** - Periodic state saving

**Thread Safety:** Not thread-safe. Must be called from a single thread.

**Memory Management:** Updates neural states in-place.

#### Input/Output Methods

##### Sensory Input
```cpp
void receiveSensoryInput(const class SensoryInput& input)
```

**Parameters:**
- `input`: Sensory data object containing input values

**Description:** Injects current into sensory neurons based on input pattern. Distributes input across all sensory neurons in the brain.

**Thread Safety:** Thread-safe for input reception.

**Memory Management:** Updates neuron membrane potentials in-place.

##### Motor Output
```cpp
std::unique_ptr<class Action> produceAction()
```

**Returns:** Unique pointer to Action object representing the produced behavior.

**Description:** Selects action based on motor neuron activity. The motor neuron group with highest activity determines the action.

**Thread Safety:** Thread-safe for output production.

**Memory Management:** Returns newly allocated Action object.

##### Direct Neuron Control
```cpp
void injectCurrent(NeuronId neuron, MembranePotential current)
void injectCurrentToNeurons(NeuronType type, MembranePotential current)
```

**Parameters:**
- `neuron`: Neuron identifier
- `current`: Current to inject in millivolts
- `type`: Neuron type (Sensory, Internal, Motor)

**Description:** Directly control neural activity by injecting current.

**Thread Safety:** Thread-safe.

**Memory Management:** Updates neuron states in-place.

#### Accessors

##### Spike System
```cpp
SpikeSystem* getSpikeSystem()
const SpikeSystem* getSpikeSystem() const
```

**Returns:** Pointer to the spike processing system.

##### Plasticity Systems
```cpp
STDP* getSTDP()
Hebbian* getHebbian()
StructuralPlasticity* getStructuralPlasticity()
```

**Returns:** Pointers to individual plasticity rule implementations.

##### Memory Systems
```cpp
NeuralWorkingMemory* getWorkingMemory()
NeuralEpisodicMemory* getEpisodicMemory()
NeuralAssociativeMemory* getAssociativeMemory()
```

**Returns:** Pointers to memory system implementations.

##### Prediction and Cognition
```cpp
PredictionSystem* getPredictionSystem()
NeuralPlanner* getPlanner()
ConceptFormation* getConceptFormation()
AttentionalSelection* getAttention()
```

**Returns:** Pointers to prediction and cognition systems.

##### Neuromodulation
```cpp
Dopamine* getDopamine()
Curiosity* getCuriosity()
Novelty* getNovelty()
PredictionError* getPredictionErrorSignal()
```

**Returns:** Pointers to neuromodulator implementations.

##### Development and Configuration
```cpp
DevelopmentSystem* getDevelopmentSystem()
DevelopmentalStage getDevelopmentalStage() const
void setDevelopmentalStage(DevelopmentalStage stage)
RandomGenerator* getRandomGenerator()
std::shared_ptr<const Config> getConfig() const
```

**Returns:** Pointers or values from development and configuration systems.

#### Statistics and Metrics

```cpp
float getExcitationInhibitionRatio() const
```

**Returns:** Ratio of total excitatory to inhibitory synaptic weights.

```cpp
size_t getTotalSpikeCount() const
```

**Returns:** Total number of spikes generated by all neurons.

```cpp
size_t getPendingSpikeEventCount() const
```

**Returns:** Number of pending spike events in the spike queue.

```cpp
size_t getTotalNeuronCount() const
```

**Returns:** Total number of neurons in the brain.

```cpp
size_t getTotalSynapseCount() const
```

**Returns:** Total number of synapses in the brain.

#### State Management

```cpp
bool save(const std::string& filepath) const
```

**Parameters:**
- `filepath`: Path to save the brain state

**Returns:** `true` if save succeeds, `false` otherwise.

**Description:** Saves brain state to a checkpoint file including:
- Neuron states (membrane potential, firing state, etc.)
- Synapse states (weights, delays, eligibility traces)
- Simulation state (current step and time)
- Memory system states

**Thread Safety:** Not thread-safe during save operation.

```cpp
bool load(const std::string& filepath)
```

**Parameters:**
- `filepath`: Path to load brain state from

**Returns:** `true` if load succeeds, `false` otherwise.

**Description:** Loads brain state from checkpoint file.

**Thread Safety:** Not thread-safe during load operation.

```cpp
void reset()
```

**Description:** Resets brain to initial state, clearing all memories and neural states.

**Thread Safety:** Thread-safe.

## Core Types

### Types (src/core/Types/Types.hpp)

#### Neuron Identifiers
```cpp
using NeuronId = uint64_t;
using PopulationId = uint64_t;
using RegionId = uint64_t;
```

#### Neural States
```cpp
enum class NeuronType { Sensory, Internal, Motor };

struct MembranePotential {
    float value;
    // Comparison operators
};

typedef float Timestamp;
typedef float TimestepDuration;
typedef uint64_t SimulationStep;
```

## Configuration System

### Config Class (src/core/Config/Config.hpp)

#### Configuration Parameters

**Core Neural Parameters**
- `neuron_count`: Total number of neurons to create
- `region_count`: Number of brain regions
- `connection_probability`: Probability of synapse formation
- `simulation_timestep`: Integration time step in seconds

**Plasticity Parameters**
- `stdp_ltp_weight`: STDP Long-Term Potentiation weight
- `stdp_ltd_weight`: STDP Long-Term Depression weight
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse removal

**Memory Parameters**
- `replay_interval`: Steps between memory replay events
- `consolidation_interval`: Steps between memory consolidation
- `working_memory_capacity`: Capacity of working memory
- `max_episodes`: Maximum number of episodic memories

**Development Parameters**
- `critical_period_start`: Step when critical period begins
- `critical_period_end`: Step when critical period ends

**Neuromodulation Parameters**
- `dopamine_baseline`: Baseline dopamine level
- `curiosity_rate`: Novelty detection rate
- `novelty_threshold`: Threshold for novelty detection

**Checkpoint Parameters**
- `checkpoint_dir`: Directory for checkpoint files
- `max_checkpoints`: Maximum number of checkpoints to keep
- `checkpoint_interval`: Steps between checkpoints

## Mathematical Formulations

### LIF Neuron Dynamics

The Leaky Integrate-and-Fire neuron model used in NLM:

```
dV/dt = (-V + R*I - V_reset)/τ_m
```

Where:
- V = membrane potential (mV)
- I = input current (mA)
- R = membrane resistance (MOhm)
- V_reset = reset potential (mV)
- τ_m = membrane time constant (ms)

**Firing Condition:** When V ≥ V_threshold, neuron emits spike and V is reset to V_reset.

**Refractory Period:** After firing, neuron enters refractory period where it cannot fire again.

### STDP Learning Rule

Spike-Timing-Dependent Plasticity weight update:

```
Δw = {
    A_LTP * exp(-Δt/τ_LTP)              if post fires after pre
    -A_LTD * exp(Δt/τ_LTD)             if pre fires after post
    0                                 otherwise
}
```

Where:
- Δt = t_post - t_pre (post minus pre spike times)
- A_LTP = LTP learning rate
- A_LTD = LTD learning rate
- τ_LTP = LTP time constant
- τ_LTD = LTD time constant

### Hebbian Learning

Simple Hebbian rule implemented:

```
Δw = α * x_pre * x_post
```

Where:
- x_pre, x_post = pre- and post-synaptic activity
- α = learning rate

## Thread Safety Guarantees

### Overall Brain
- Brain class is NOT thread-safe for concurrent operations
- Individual methods are thread-safe only for read-only operations
- Must use external synchronization for multi-threaded usage

### Memory Systems
- WorkingMemory: Thread-safe for concurrent read/write operations
- EpisodicMemory: Thread-safe with mutex protection
- AssociativeMemory: Thread-safe with atomic operations

### Spike System
- Event-driven architecture with thread-safe queues
- Multiple threads can add/remove events safely
- Processing is single-threaded per brain instance

## Memory Management Guidelines

### General Guidelines
1. **Ownership:** Brain owns all allocated resources
2. **Lifetimes:** Brain lifetime should encompass all resource usage
3. **Exceptions:** Resource cleanup is guaranteed even with exceptions
4. **Pointers:** Never store Brain pointers across threads without synchronization

### Specific Systems

#### Spike System
- Uses event pools for memory efficiency
- Delayed spike events are managed in priority queues
- Spike handlers are called from simulation thread only

#### Memory Systems
- WorkingMemory: Uses circular buffers for O(1) access
- EpisodicMemory: Implements compact storage with sparse encoding
- AssociativeMemory: Uses hash tables for fast lookup

#### Checkpoint System
- Checkpoint files are memory-mapped for efficient I/O
- Compression reduces disk usage significantly
- Recovery is atomic - either fully succeeds or fails

## Error Handling

NLM uses a comprehensive exception hierarchy:

### Core Exceptions
- `NLMException`: Base exception class
- `ConfigError`: Configuration-related errors
- `SimulationError`: Simulation runtime errors
- `MemoryError`: Memory allocation or corruption errors
- `FileError`: File I/O errors
- `ValidationError`: Data validation errors

### Error Reporting
- All exceptions include descriptive messages
- Errors are logged via structured logging system
- Debug information includes context and stack traces
- Errors can be retrieved via exception manager API

## Performance Characteristics

### Time Complexity
- **Step() operation**: O(N_neurons + N_synapses) per simulation step
- **Memory operations**: O(1) for working memory, O(log N) for episodic memory
- **Plasticity updates**: O(N_synapses) per step

### Memory Usage
- **Base brain**: ~50-100 MB for 1000 neurons
- **Memory systems**: 10-50 MB additional
- **Checkpoints**: Configurable, default ~100 MB
- **Total**: ~150-250 MB for full configuration

### Optimization Features
- SIMD vectorization for neuron updates
- Memory pools reduce allocation overhead
- Event-driven spike processing
- Cache-friendly data layouts
- Parallel region processing

## Integration Points

### Sensorimotor Loop
```cpp
// Complete sensorimotor loop example
Brain brain(config);
if (!brain.initialize()) { /* Handle error */ }

while (simulation_running) {
    // 1. Receive sensory input
    brain.receiveSensoryInput(sensory_data);
    
    // 2. Process brain loop
    brain.step(step_count, current_time);
    
    // 3. Produce motor output
    auto action = brain.produceAction();
    
    // 4. Send action to world
    send_to_world(*action);
    
    // 5. Update neuromodulation with reward
    brain.applyNeuromodulation(reward_signal);
}
```

### Custom Experiment Framework
```cpp
class Experiment {
public:
    virtual void setup(Brain& brain) = 0;
    virtual bool runStep(Brain& brain, SimulationStep step) = 0;
    virtual void cleanup(Brain& brain) = 0;
};

// Example usage
class PlasticityExperiment : public Experiment {
    void setup(Brain& brain) override {
        // Initialize experiment-specific brain state
        auto* stdp = brain.getSTDP();
        stdp->configure(0.01f, 0.012f, 20.0f);
    }
    
    bool runStep(Brain& brain, SimulationStep step) override {
        // Custom simulation logic
        if (step % 100 == 0) {
            // Record measurements
            record_plasticity_stats(brain);
        }
        return true;
    }
};
```

## Best Practices

### Development Best Practices
1. **Configuration:** Validate configuration parameters before initialization
2. **Error Handling:** Use try-catch blocks for operations that can fail
3. **Memory:** Monitor memory usage in long simulations
4. **Performance:** Profile critical paths during development
5. **Testing:** Use checkpoint/restart for reliable testing

### Usage Best Practices
1. **Lifecycle:** Initialize brain once, use for entire simulation
2. **Threading:** Run simulation in single thread for determinism
3. **Scaling:** Use smaller brains for rapid prototyping
4. **Monitoring:** Use logging for debugging and performance tracking
5. **Persistence:** Use checkpoints for long simulations

## See Also

- [NLM Architecture Documentation](ARCHITECTURE.md)
- [Phase 2 Tutorial](TUTORIALS/PHASE2/README.md)
- [Phase 3 Tutorial](TUTORIALS/PHASE3/README.md)
- [Phase 4 Tutorial](TUTORIALS/PHASE4/README.md)
- [Phase 6 Tutorial](TUTORIALS/PHASE6/README.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Cheat Sheet](CHEAT_SHEET.md)
