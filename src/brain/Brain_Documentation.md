# NLM Codebase Organization and Documentation

This document provides comprehensive guidance on the NLM (Neural Learning Machine) codebase structure, organization, and documentation standards.

## Overview

The NLM codebase is a sophisticated computational brain framework that implements artificial developmental intelligence through spiking neural networks. The codebase is organized into several logical layers:

```
NLM/
├── src/
│   ├── core/           # Fundamental types and utilities
│   │   ├── Types/      # NeuronId, SynapseId, etc.
│   │   ├── Config/     # Configuration management
│   │   ├── Random/     # Deterministic random generation
│   │   ├── Logger/     # Structured logging
│   │   └── SimulationClock/ # Time management
│   ├── brain/          # Neural computation core
│   │   ├── Neuron      # LIF neuron implementation
│   │   ├── Synapse     # Synaptic connections
│   │   ├── NeuralPopulation # Neuron groups
│   │   ├── NeuralRegion # Brain regions
│   │   └── Brain       # Main brain coordinator
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Learning rules
│   ├── development/    # Developmental processes
│   ├── neuromodulation/# Neuromodulators
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/     # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor control
│   ├── environment/   # Environment interface
│   ├── agent/         # Agent brain interface
│   └── world/          # World simulation
│
├── docs/              # Documentation (separate directory)
├── examples/          # Code examples and demonstrations
├── tests/             # Test suites
├── python/            # Python bindings
│   ├── pynlm_enhanced.py # Enhanced Python API
│   └── ...            # Python bindings
├── configs/           # Configuration files
└── build/             # Build artifacts
```

## Documentation Structure

### Core Documentation Files

#### 1. **ARCHITECTURE.md** (Main Architecture Documentation)
- High-level system overview
- Component interaction diagrams
- Data flow descriptions
- Design principles and trade-offs

#### 2. **ARCHITECTURE.md** (Detailed Implementation)
- Component-specific documentation
- API references for major classes
- Implementation details
- Performance characteristics

#### 3. **EXPERIMENTS.md** (Experimental Framework)
- Experiment design and methodology
- Configuration options
- Expected results and analysis
- Anti-cheating measures

#### 4. **PYTHON_API.md** (Python Bindings Documentation)
- API reference for Python bindings
- Examples and tutorials
- Configuration options
- Error handling and troubleshooting

#### 5. **PERFORMANCE.md** (Performance Documentation)
- Performance characteristics
- Optimization techniques
- Benchmarking results
- Scalability analysis

### Component Documentation

#### Class Documentation Template

Each major class should follow this documentation template:

```markdown
# [ClassName] - [Brief Description]

## Overview

[High-level description of the class's purpose and role in the system]

## Key Features

- [Feature 1]
- [Feature 2]
- [Feature 3]

## Usage Examples

### Basic Usage

```cpp
// Basic instantiation and usage
[Example code]
```

### Advanced Usage

```cpp
// Advanced features and patterns
[Example code]
```

## Configuration

### Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| parameter1 | type | default | Description of parameter1 | range |

## Methods

### Core Methods

#### methodName

**Description:** Brief description of what the method does.

**Parameters:**
- `param1` (type): Description of parameter1
- `param2` (type): Description of parameter2

**Returns:** Return type and description

**Example:**
```cpp
// Example usage
methodName(param1, param2);
```

### Specialized Methods

#### specializedMethod

**Description:** More detailed description.

## Implementation Notes

- [Implementation detail 1]
- [Implementation detail 2]
- [Implementation detail 3]

## Performance Considerations

- [Performance note 1]
- [Performance note 2]

## Error Handling

- [Error condition 1]
- [Error condition 2]

## See Also

- Related class: [ClassName]
- Related function: [functionName]
- Related topic: [topic]
```

### Example Class Documentation

#### Brain Class Documentation

```markdown
# Brain - Neural Simulation Coordinator

## Overview

The Brain class is the central coordinator of the NLM neural system. It implements real spiking neural computation with event-driven dynamics and integrates multiple subsystems including memory, prediction, cognition, and neuromodulation.

## Key Features

- Real-time spiking neural computation using LIF neuron dynamics
- Event-driven spike propagation with synaptic delays
- Integration of multiple learning and memory systems
- Neuromodulation for reward-based learning
- Developmental stage progression
- Checkpoint-based state persistence

## Usage Examples

### Basic Neural Simulation

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    return 0;
}
```

### Advanced Simulation with Memory

```cpp
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"

void advancedSimulationExample() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 2000);
    config->set("enable_plasticity", true);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Access memory systems
    auto working_memory = brain->getWorkingMemory();
    auto episodic_memory = brain->getEpisodicMemory();
    
    // Store and retrieve experiences
    working_memory->storeExperience(42.0f, "initial_learning");
    episodic_memory->recordExperience(42.0f, {1.0, 2.0, 3.0}, "test_experience");
    
    // Run complex simulation with memory integration
    for (int step = 0; step < 5000; ++step) {
        brain->step(step, step * 0.001);
        
        // Inject learning signals
        if (step % 100 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
    }
}
```

## Configuration

### Core Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| neuron_count | int64_t | 100 | Total number of neurons | 1-100000 |
| region_count | int64_t | 1 | Number of neural regions | 1-100 |
| connection_probability | float | 0.15 | Probability of synaptic connections | 0.01-0.5 |
| simulation_timestep | double | 0.001 | Time step for simulation | 0.0001-0.01 |
| random_seed | int64_t | 42 | Random seed for reproducibility | 0-2^63-1 |

### Plasticity Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| stdp_ltp_weight | float | 0.02 | STDP Long-Term Potentiation weight | 0.001-0.1 |
| stdp_ltd_weight | float | 0.015 | STDP Long-Term Depression weight | 0.001-0.05 |
| stdp_tau | float | 20.0 | STDP time constant | 1.0-100.0 |
| hebbian_learning_rate | float | 0.01 | Hebbian learning rate | 0.001-0.1 |

### Memory Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| working_memory_capacity | size_t | 100 | Working memory storage capacity | 10-10000 |
| working_memory_decay_rate | float | 0.01 | Memory decay rate | 0.001-0.1 |
| episodic_memory_capacity | size_t | 1000 | Episodic memory storage capacity | 100-100000 |

## Methods

### Core Functionality

#### initialize()

**Description:** Initialize brain with configuration, create neural regions, establish connections, and set up all subsystems.

**Parameters:** None

**Returns:** bool - True if initialization successful, false otherwise

**Example:**
```cpp
if (!brain->initialize()) {
    std::cerr << "Failed to initialize brain!" << std::endl;
    return 1;
}
```

#### step(SimulationStep currentStep, Timestamp currentTime)

**Description:** Perform one simulation step with neural dynamics, spike propagation, and plasticity updates.

**Parameters:**
- `currentStep` (SimulationStep): Current simulation step number
- `currentTime` (Timestamp): Current simulation time in seconds

**Returns:** void

**Example:**
```cpp
for (SimulationStep step = 0; step < total_steps; ++step) {
    double time = step * config->get<double>("simulation_timestep");
    brain->step(step, time);
}
```

#### receiveSensoryInput(const SensoryInput& input)

**Description:** Inject sensory input into sensory neurons based on input pattern.

**Parameters:**
- `input` (const SensoryInput&): Sensory input to inject

**Returns:** void

**Example:**
```cpp
SensoryInput vision_input(640, 480, 3);
// Fill with visual data...
brain->receiveSensoryInput(vision_input);
```

#### produceAction()

**Description:** Produce motor action based on neural activity and internal state.

**Parameters:** None

**Returns:** std::unique_ptr<Action> - Generated motor action

**Example:**
```cpp
auto action = brain->produceAction();
if (action) {
    std::cout << "Produced action: " << action->getName() << std::endl;
}
```

### State Management

#### reset()

**Description:** Reset brain state to initial conditions.

**Parameters:** None

**Returns:** void

**Example:**
```cpp
brain->reset();
brain->initialize();  // Re-initialize after reset
```

#### save(const std::string& filepath)

**Description:** Save brain state to file for checkpointing.

**Parameters:**
- `filepath` (const std::string&): Path to save file

**Returns:** bool - True if save successful, false otherwise

**Example:**
```cpp
if (brain->save("brain_checkpoint.nlm")) {
    std::cout << "Brain state saved successfully" << std::endl;
}
```

#### load(const std::string& filepath)

**Description:** Load brain state from file.

**Parameters:**
- `filepath` (const std::string&): Path to load file

**Returns:** bool - True if load successful, false otherwise

**Example:**
```cpp
if (brain->load("brain_checkpoint.nlm")) {
    std::cout << "Brain state loaded successfully" << std::endl;
}
```

### Statistics and Monitoring

#### getTotalNeuronCount()

**Description:** Get total number of neurons in the brain.

**Parameters:** None

**Returns:** size_t - Total neuron count

#### getTotalSynapseCount()

**Description:** Get total number of synapses in the brain.

**Parameters:** None

**Returns:** size_t - Total synapse count

#### getAverageFiringRate()

**Description:** Get average firing rate across all neurons.

**Parameters:** None

**Returns:** float - Average firing rate in Hz

#### logStatus()

**Description:** Log comprehensive brain status and statistics.

**Parameters:** None

**Returns:** void

**Example:**
```cpp
brain->logStatus();
```

### Neuromodulation Systems

#### applyNeuromodulation(const Neuromodulator& signal)

**Description:** Apply neuromodulatory signals (dopamine, curiosity, novelty) to plasticity and dynamics.

**Parameters:**
- `signal` (const Neuromodulator&): Neuromodulation signal

**Returns:** void

#### getDopamine()

**Description:** Get dopamine neuromodulator for reward processing.

**Parameters:** None

**Returns:** Dopamine* - Pointer to dopamine system

#### getCuriosity()

**Description:** Get curiosity neuromodulator for exploration motivation.

**Returns:** Curiosity* - Pointer to curiosity system

#### getNovelty()

**Description:** Get novelty detector for surprise processing.

**Returns:** Novelty* - Pointer to novelty detection system

### Memory Systems

#### getWorkingMemory()

**Description:** Get working memory system for short-term storage.

**Parameters:** None

**Returns:** NeuralWorkingMemory* - Pointer to working memory system

#### getEpisodicMemory()

**Description:** Get episodic memory system for experience storage.

**Returns:** NeuralEpisodicMemory* - Pointer to episodic memory system

#### getAssociativeMemory()

**Description:** Get associative memory system for pattern associations.

**Returns:** NeuralAssociativeMemory* - Pointer to associative memory system

### Cognitive Systems

#### getPlanner()

**Description:** Get neural planner for action planning.

**Returns:** NeuralPlanner* - Pointer to planning system

#### getConceptFormation()

**Description:** Get concept formation system for pattern discovery.

**Returns:** ConceptFormation* - Pointer to concept formation system

#### getAttention()

**Description:** Get attentional selection system for competitive processing.

**Returns:** AttentionalSelection* - Pointer to attentional system

### Development System

#### getDevelopmentSystem()

**Description:** Get development system for age-dependent plasticity.

**Returns:** DevelopmentSystem* - Pointer to development system

#### getDevelopmentalStage()

**Description:** Get current developmental stage.

**Returns:** DevelopmentalStage - Current developmental stage

#### setDevelopmentalStage(DevelopmentalStage stage)

**Description:** Set developmental stage.

**Parameters:**
- `stage` (DevelopmentalStage): Developmental stage to set

**Returns:** void

## Implementation Notes

### Neural Dynamics Implementation

The Brain class implements Leaky Integrate-and-Fire (LIF) neuron dynamics:

```cpp
class Neuron {
private:
    MembranePotential membranePotential;
    FiringState firingState;
    double membranePotential;
    double threshold;
    double resetPotential;
    double decayConstant;
    double timeConstant;
    
public:
    void update(double dt, double injectedCurrent);
    bool isFiring() const;
    void reset();
    // ... other methods
};
```

### Plasticity Integration

Plasticity is integrated through multiple mechanisms:

1. **STDP (Spike-Timing-Dependent Plasticity):** Pre-post spike timing
2. **Hebbian Learning:** Coincident neural activity
3. **Structural Plasticity:** Synapse formation and pruning
4. **Neuromodulatory Plasticity:** Reward-based learning

### Memory System Integration

Memory systems are integrated through:

1. **Working Memory:** Persistent neural activity for temporary storage
2. **Episodic Memory:** Experience encoding and retrieval
3. **Associative Memory:** Pattern-based associations
4. **Replay:** Memory consolidation during rest/sleep cycles

### Cognitive Architecture

Cognitive systems use:

1. **Prediction:** Temporal sequence learning and prediction error
2. **Attention:** Competitive lateral inhibition for selective processing
3. **Planning:** Multi-step future simulation and action selection
4. **Concept Formation:** Pattern discovery without predefined labels

## Performance Considerations

### Memory Usage

- Memory pools are pre-allocated for performance
- Event-driven architecture minimizes memory overhead
- Neural population memory usage scales with neuron count

### Computational Complexity

- Neuron update: O(1) per neuron
- Synapse update: O(E) where E is number of synapses
- Pattern matching: O(N log N) for large-scale operations

### Optimization Strategies

1. **SIMD Vectorization:** Parallel processing of neuron groups
2. **Sparse Connectivity:** Efficient storage for sparse networks
3. **Event Queues:** Efficient spike propagation
4. **Memory Pools:** Reduce allocation overhead
5. **Cache Locality:** Optimize data layout for CPU caches

## Error Handling

### Common Error Conditions

1. **Invalid Configuration:** Configuration validation fails
2. **Memory Allocation:** Out of memory during initialization
3. **File I/O:** Cannot read/write brain state files
4. **Null Pointer:** Invalid operation on uninitialized components

### Error Recovery

- Graceful degradation for non-critical failures
- Checkpoint restoration on simulation failure
- Configuration validation with helpful error messages

## Testing and Validation

### Unit Tests

- Individual component testing (neurons, synapses, plasticity rules)
- Configuration validation testing
- Error condition testing

### Integration Tests

- Complete brain system testing
- Cross-component interaction testing
- Performance validation

### Performance Tests

- Scalability testing with varying neuron counts
- Memory usage profiling
- Timing validation for real-time applications

## Version and Compatibility

### Versioning

- Semantic versioning: MAJOR.MINOR.PATCH
- Breaking changes documented with migration guides
- Backward compatibility maintained for public APIs

### Platform Support

- Windows, Linux, macOS
- C++17 or later compiler
- Python 3.8+ for bindings

## Related Documentation

- **ARCHITECTURE.md:** High-level system architecture
- **EXPERIMENTS.md:** Experimental procedures and results
- **PYTHON_API.md:** Python bindings documentation
- **PERFORMANCE.md:** Performance characteristics and optimization

## Further Reading

- **Scientific Papers:** NLM research publications
- **Source Code:** Inline comments and Doxygen documentation
- **Community Resources:** Forums, tutorials, and examples
- **Research Applications:** Academic and industrial use cases

This documentation provides a comprehensive guide to the Brain class and its role in the NLM neural simulation framework. The class serves as the central coordinator for artificial developmental intelligence, integrating multiple complex systems into a coherent, learning agent.