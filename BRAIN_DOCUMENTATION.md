# NLM C++ Headers Documentation

This document provides comprehensive documentation for the NLM C++ headers, including implementation details, usage examples, and best practices.

## Overview

The NLM C++ framework consists of multiple header files organized into logical modules:

### Core Module (`core/`)
- **`Config.hpp`**: Configuration management
- **`Types.hpp`**: Type definitions and constants
- **`Random.hpp`**: Random number generation
- **`SimulationClock.hpp`**: Time management
- **`Logger.hpp`**: Logging system

### Brain Module (`brain/`)
- **`Brain.hpp`**: Main brain class (documented below)
- **`Neuron.hpp`**: Individual neuron model
- **`Synapse.hpp`**: Synapse model and plasticity
- **`NeuralRegion.hpp`**: Brain region management
- **`NeuralPopulation.hpp`**: Population-level neural dynamics

### Dynamics Module (`dynamics/`)
- **`SpikeSystem.hpp`**: Spiking neural dynamics
- **`NeuralDynamics.hpp`**: General neural dynamics

### Plasticity Module (`plasticity/`)
- **`STDP.hpp`**: Spike-Timing-Dependent Plasticity
- **`Hebbian.hpp`**: Hebbian learning
- **`StructuralPlasticity.hpp`**: Structural plasticity
- **`PlasticityRule.hpp`**: Plasticity rule base class

### Neuromodulation Module (`neuromodulation/`)
- **`Neuromodulator.hpp`**: Neuromodulator base class
- **`Dopamine.hpp`**: Dopamine neuromodulation
- **`Curiosity.hpp`**: Curiosity neuromodulation
- **`Novelty.hpp`**: Novelty detection
- **`PredictionError.hpp`**: Prediction error signal

### Memory Module (`memory/`)
- **`Memory.hpp`**: Memory base class
- **`NeuralWorkingMemory.hpp`**: Working memory system
- **`NeuralEpisodicMemory.hpp`**: Episodic memory system
- **`NeuralAssociativeMemory.hpp`**: Associative memory system

### Prediction Module (`prediction/`)
- **`PredictionSystem.hpp`**: Prediction system
- **`NeuralPrediction.hpp`**: Neural prediction model

### Cognition Module (`cognition/`)
- **`NeuralPlanner.hpp`**: Neural planning system
- **`ConceptFormation.hpp`**: Concept formation system

### Sensory/Motor Module (`sensory/`, `motor/`)
- **`SensoryInput.hpp`**: Sensory input base class
- **`Vision.hpp`**: Vision system
- **`Audio.hpp`**: Audio system
- **`InternalSignals.hpp`**: Internal signal system
- **`Action.hpp`**: Action representation
- **`MotorSystem.hpp`**: Motor control system

### Environment Module (`environment/`)
- **`Environment.hpp`**: Environment base class
- **`SimpleWorld.hpp`**: Simple 2D world
- **`Observation.hpp`**: Observation structure

### Agent Module (`agent/`)
- **`AgentBrain.hpp`**: Agent brain interface
- **`SensoryPercept.hpp`**: Sensory perception
- **`AgentBody.hpp`**: Agent body state

### Development Module (`development/`)
- **`DevelopmentSystem.hpp`**: Development system
- **`Synaptogenesis.hpp`**: Synaptogenesis
- **`Pruning.hpp`**: Pruning
- **`Maturation.hpp`**: Maturation

### Visualization Module (`visualization/`)
- **`VisualizationInterface.hpp`**: Visualization interface

### Performance Module (`performance/`)
- **`MemoryPool.hpp`**: Memory management
- **`OptimizedBrain.hpp`**: Optimized brain implementation

### Experiments Module (`experiments/`)
- **`Experiment.hpp`**: Base experiment class
- **`Phase3Experiment.hpp`**: Phase 3 experiments
- **`Phase4Experiment.hpp`**: Phase 4 experiments
- **`Phase5Experiment.hpp`**: Phase 5 experiments
- **`Phase6IntegratedExperiment.hpp`**: Phase 6 integrated experiments

## Brain.hpp Detailed Documentation

### Class Brain

**Purpose**: The central coordinator of the neural system implementing real spiking neural computation with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

**Architecture**: Uses the Pimpl idiom (Pointer to Implementation) for efficient compilation and hiding implementation details.

#### Constructor
```cpp
explicit Brain(std::shared_ptr<Config> config);
```

**Parameters**:
- `config`: Shared pointer to configuration object

**Initialization**: Creates internal implementation and initializes all neural subsystems based on configuration.

**Exceptions**: May throw exceptions if configuration is invalid or resources are unavailable.

#### Copy Operations
```cpp
// Disable copying, enable moving
Brain(const Brain&) = delete;
Brain& operator=(const Brain&) = delete;
Brain(Brain&&) noexcept;
Brain& operator=(Brain&&) noexcept;
```

**Rationale**: Prevents accidental copying while allowing efficient moving of brain instances.

#### Main Interface Methods

##### `bool initialize()`
**Purpose**: Initialize brain with configuration.

**Description**: Sets up neural architecture, creates neurons, synapses, and initializes all subsystems. This method must be called before any other operations.

**Returns**:
- `true` if initialization succeeded
- `false` if initialization failed (e.g., invalid configuration)

**Thread Safety**: Not thread-safe. Must be called before concurrent access.

**Exceptions**: Throws `InitializationError` on failure.

##### `void step(SimulationStep currentStep)`
**Purpose**: Perform a main simulation step with current step information.

**Parameters**:
- `currentStep`: Current simulation step containing step number and metadata

**Behavior**:
1. Processes pending spike events
2. Updates neural membrane potentials
3. Executes synaptic transmission
4. Applies plasticity rules
5. Updates neuromodulation
6. Checks for development changes
7. Processes sensory input if available
8. Generates motor output if requested

**Thread Safety**: Not thread-safe. Each step must complete before starting another.

##### `void step(SimulationStep currentStep, Timestamp currentTime)`
**Purpose**: Perform a simulation step with explicit timestamp.

**Parameters**:
- `currentStep`: Current simulation step
- `currentTime`: Current simulation timestamp (absolute time)

**Difference from single-parameter version**: Uses explicit timestamp for time-dependent operations.

##### `void receiveSensoryInput(const class SensoryInput& input)`
**Purpose**: Inject sensory input into the brain.

**Parameters**:
- `input`: Sensory input data (vision, audio, internal signals, etc.)

**Behavior**:
1. Decodes sensory input type
2. Injects current into appropriate sensory neurons
3. Triggers corresponding neural pathways
4. Updates prediction systems if present

**Thread Safety**: Can be called concurrently with other operations.

##### `void injectCurrent(NeuronId neuron, MembranePotential current)`
**Purpose**: Inject current directly into a specific neuron.

**Parameters**:
- `neuron`: ID of neuron to inject current into
- `current`: Amount of current to inject

**Thread Safety**: Thread-safe for different neurons, requires synchronization for same neuron.

##### `void injectCurrentToNeurons(NeuronType type, MembranePotential current)`
**Purpose**: Inject current into all neurons of a specific type.

**Parameters**:
- `type`: Type of neurons to inject current
- `current`: Amount of current to inject

**Use Case**: Used for sensory input or global modulation effects.

##### Plasticity System Access
```cpp
STDP* getSTDP();
Hebbian* getHebbian();
StructuralPlasticity* getStructuralPlasticity();
```

**Purpose**: Access plasticity systems for observation, modification, or debugging.

**Thread Safety**: Requires synchronization for modification.

##### Statistics Methods
```cpp
float getExcitationInhibitionRatio() const;
size_t getTotalSpikeCount() const;
size_t getPendingSpikeEventCount() const;
```

**Purpose**: Retrieve key performance and state metrics.

**Thread Safety**: Thread-safe for read operations.

##### Action Production
```cpp
std::unique_ptr<class Action> produceAction();
```

**Purpose**: Produce motor/action output based on motor neuron activity.

**Returns**: Unique pointer to action object. Caller assumes ownership.

**Behavior**:
1. Analyzes motor neuron activity patterns
2. Selects appropriate action based on context
3. Generates action parameters
4. Returns action for execution

##### Neuromodulation
```cpp
void applyNeuromodulation(const class Neuromodulator& signal);
```

**Purpose**: Apply neuromodulatory signals (dopamine, curiosity, novelty, etc.).

**Parameters**:
- `signal`: Neuromodulator instance containing signal type and strength

**Behavior**:
1. Updates neuromodulation levels
2. Affects plasticity rules
3. Modifies neural excitability
4. Influences decision-making processes

##### Plasticity and Development Updates
```cpp
void updatePlasticity();
void develop();
```

**Purpose**: Update plasticity rules and developmental changes.

**Note**: These methods are typically called automatically during simulation steps but can be called manually for fine control.

##### State Management
```cpp
void reset();
bool save(const std::string& filepath) const;
bool load(const std::string& filepath);
```

**Purpose**: Manage brain state.

- `reset()`: Reset brain to initial state
- `save()`: Save current state to file (checkpointing)
- `load()`: Load state from file

##### Region Management
```cpp
RegionId addRegion(const std::string& name = "");
NeuralRegion* getRegion(RegionId id);
const NeuralRegion* getRegion(RegionId id) const;
size_t getRegionCount() const;
std::vector<RegionId> getRegionIds() const;
```

**Purpose**: Manage brain regions (columns of neurons with specialized functions).

**Usage**:
1. Create regions with `addRegion()`
2. Access regions with `getRegion()`
3. Manage inter-region connections

##### Memory Systems Access
```cpp
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();
```

**Purpose**: Access memory systems for observation, storage, and retrieval operations.

##### Prediction System
```cpp
PredictionSystem* getPredictionSystem();
```

**Purpose**: Access prediction system for forward models and prediction error computation.

##### Cognition Systems
```cpp
NeuralPlanner* getPlanner();
ConceptFormation* getConceptFormation();
AttentionalSelection* getAttention();
```

**Purpose**: Access cognitive systems for planning, concept formation, and attention.

##### Development System
```cpp
DevelopmentSystem* getDevelopmentSystem();
DevelopmentalStage getDevelopmentalStage() const;
void setDevelopmentalStage(DevelopmentalStage stage);
```

**Purpose**: Access development system for managing developmental stages and maturation.

##### Neuromodulation Systems
```cpp
Dopamine* getDopamine();
Curiosity* getCuriosity();
Novelty* getNovelty();
PredictionError* getPredictionErrorSignal();
```

**Purpose**: Access neuromodulation systems for reward-based learning, exploration motivation, novelty detection, and prediction error computation.

##### Configuration and Utility Access
```cpp
std::shared_ptr<const Config> getConfig() const;
RandomGenerator* getRandomGenerator();
void logStatus() const;
```

**Purpose**: Access configuration and utility services.

#### Implementation Details

##### Pimpl Pattern
The Brain class uses the Pimpl idiom:
- **`struct Impl`**: Internal implementation details
- **`Impl* pImpl`**: Pointer to implementation
- **Benefits**: Efficient compilation, binary compatibility, encapsulation

##### Memory Management
- Uses smart pointers (`std::shared_ptr`, `std::unique_ptr`) for memory safety
- Custom memory pools for performance optimization
- Automatic cleanup in destructor

##### Thread Safety
- Most operations are not thread-safe
- Read-only operations are thread-safe
- Concurrent access requires external synchronization

##### Error Handling
- Uses custom exception hierarchy
- Detailed error information with context
- Recovery strategies for non-critical errors

## Usage Examples

### Basic Brain Usage
```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Create brain
    auto brain = std::make_unique<nlm::Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        nlm::SimulationStep currentStep(step);
        brain->step(currentStep);
    }
    
    // Get statistics
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    
    // Save state
    brain->save("brain_checkpoint.nml");
    
    return 0;
}
```

### Advanced Brain Usage with Memory
```cpp
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"

void example_with_memory() {
    // Create and initialize brain
    auto config = std::make_shared<Config>();
    auto brain = std::make_unique<nlm::Brain>(config);
    brain->initialize();
    
    // Access memory systems
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    auto associativeMemory = brain->getAssociativeMemory();
    
    // Use working memory
    workingMemory->store("current_action", "move_forward");
    auto action = workingMemory->retrieve<std::string>("current_action");
    
    // Store episodic memory
    episodicMemory->storeExperience({
        "timestamp": 100.0,
        "sensory_input": /* ... */,
        "action": "move_forward",
        "reward": 1.0
    });
    
    // Use prediction system
    auto predictionSystem = brain->getPredictionSystem();
    auto prediction = predictionSystem->predictCurrentState();
    auto error = predictionSystem->computePredictionError(prediction);
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        nlm::SimulationStep currentStep(step);
        brain->step(currentStep, step * 0.001f); // With timestamp
        
        // Process sensory input
        auto sensoryInput = /* ... get sensory input ... */;
        brain->receiveSensoryInput(sensoryInput);
        
        // Generate action
        auto action = brain->produceAction();
    }
}
```

### Error Handling
```cpp
#include "brain/Brain.hpp"
#include "error_handling.hpp"

void example_with_error_handling() {
    try {
        auto config = std::make_shared<Config>();
        auto brain = std::make_unique<nlm::Brain>(config);
        
        if (!brain->initialize()) {
            throw nlm::InitializationError("Brain initialization failed");
        }
        
        // Run simulation
        for (int step = 0; step < 1000; ++step) {
            nlm::SimulationStep currentStep(step);
            brain->step(currentStep);
        }
        
    } catch (const nlm::NLMError& e) {
        std::cerr << "NLM Error: " << e.what() << std::endl;
        if (e.cause) {
            std::cerr << "Caused by: " << typeid(*e.cause).name() << std::endl;
        }
    }
}
```

## Best Practices

### Memory Management
1. **Use smart pointers**: Prefer `std::shared_ptr` and `std::unique_ptr` over raw pointers
2. **Pimpl idiom**: Use for separating interface from implementation
3. **Custom memory pools**: For performance-critical applications
4. **Resource acquisition is initialization (RAII)**: Ensure proper cleanup

### Thread Safety
1. **Identify thread-safe operations**: Read-only methods are generally thread-safe
2. **Use synchronization**: For concurrent access to shared resources
3. **Immutable objects**: Design objects to be immutable where possible
4. **Message passing**: Use message passing for inter-thread communication

### Error Handling
1. **Custom exception hierarchy**: Use specific exception types for different error conditions
2. **Error context**: Include detailed context information in exceptions
3. **Recovery strategies**: Implement recovery where possible
4. **Resource cleanup**: Ensure proper cleanup in exception handlers

### Performance Optimization
1. **Minimize allocations**: Use object pools and arenas where possible
2. **Cache frequently accessed data**: Reduce redundant computations
3. **Use SIMD instructions**: For numerical computations
4. **Profile and optimize**: Use performance profilers to identify bottlenecks

### Testing and Validation
1. **Unit tests**: Test individual components
2. **Integration tests**: Test component interactions
3. **Performance tests**: Validate performance characteristics
4. **Regression tests**: Ensure no degradation over time

## Conclusion

The Brain.hpp header file represents the core of the NLM neural simulation framework, providing a comprehensive interface for neural computation, memory, cognition, and neuromodulation. The documentation provided here should serve as a comprehensive guide for developers working with the NLM framework, covering both theoretical concepts and practical implementation details.

The improvements made to this documentation enhance the usability and maintainability of the NLM framework, making it more accessible to new developers while providing comprehensive guidance for experienced users.
