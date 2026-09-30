# Core Brain API

## Overview

This document provides the complete API reference for the core NLM Brain class, which is the central component of the neural network simulation system.

## Table of Contents

1. [Brain Class](brain_class.md)
2. [Region Class](region_class.md)
3. [Neuron Class](neuron_class.md)
4. [Synapse Class](synapse_class.md)
5. [Integration Methods](integration_methods.md)
6. [Configuration Methods](configuration_methods.md)
7. [Simulation Control](simulation_control.md)
8. [Memory Systems](memory_systems.md)
9. [Error Handling](core_error_handling.md)

## Brain Class

The `Brain` class is the main controller for neural network simulations. It manages regions, neurons, synapses, and the integration of various brain systems.

### Class Definition

```cpp
class Brain {
public:
    // Construction and destruction
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core functionality
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // I/O operations
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    
    // System management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    void logStatus() const;
    
    // System access
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    RegionId addRegion(const std::string& name);
    
    // Statistics and introspection
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // System components access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    DevelopmentSystem* getDevelopmentSystem();
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Neural computation components
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // Statistics
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
};

// Brain construction with configuration
std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
```

### Constructor

```cpp
Brain(std::shared_ptr<Config> config);
```

**Parameters:**
- `config`: Shared pointer to configuration object containing simulation parameters

**Description:**
Constructs a new Brain instance with the specified configuration. The configuration should contain parameters such as neuron count, region count, connection probability, etc.

### Move Constructor and Assignment Operator

```cpp
Brain(Brain&& other) noexcept;
Brain& operator=(Brain&& other) noexcept;
```

**Description:**
Enables efficient transfer of ownership of the Brain instance and its internal resources.

### Initialize Method

```cpp
bool Brain::initialize();
```

**Returns:**
- `true` if initialization succeeds, `false` otherwise

**Description:**
Initializes the brain simulation with the configured parameters. This includes:

1. **Validation**: Validates configuration parameters
2. **Region Creation**: Creates neural regions based on configuration
3. **Neuron Creation**: Creates neurons within each region
4. **Population Setup**: Sets up neuron populations (sensory, internal, motor)
5. **Connectivity Initialization**: Establishes random synaptic connections
6. **System Integration**: Initializes all integrated systems (memory, prediction, cognition, neuromodulation)
7. **Event Registration**: Registers handlers for spike events and plasticity updates

**Error Handling:**
Throws `NLMException` with error category `IntegrationFailed` for initialization failures.

**Example:**
```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);
config->set("connection_probability", 0.1f);

auto brain = std::make_shared<Brain>(config);
if (!brain->initialize()) {
    throw std::runtime_error("Brain initialization failed");
}
```

### Step Methods

```cpp
void Brain::step(SimulationStep currentStep);
void Brain::step(SimulationStep currentStep, Timestamp currentTime);
```

**Parameters:**
- `currentStep`: Current simulation step number
- `currentTime`: Current simulation time in seconds (optional)

**Description:**
Advances the simulation by one timestep, implementing the complete integrated brain loop:

1. **Process pending delayed spikes**: Delivers synaptic input to destination neurons
2. **Update neuron membrane potentials**: Applies LIF dynamics to all neurons
3. **Detect spikes and schedule events**: Identifies firing neurons and queues spike events
4. **Update working memory**: Maintains and competes memory traces
5. **Apply neuromodulation effects**: Updates neuromodulator levels and effects
6. **Apply plasticity rules**: Executes STDP and Hebbian learning
7. **Update episodic memory**: Stores current experience as episodes
8. **Update prediction system**: Processes prediction errors
9. **Update attention system**: Applies competitive selection
10. **Update concept formation**: Forms new concepts from neural patterns
11. **Apply structural plasticity**: Performs synaptogenesis and pruning
12. **Replay memories**: Replays important episodes during rest
13. **Apply development effects**: Updates developmental stage and effects
14. **Collect statistics**: Updates system statistics

**Example:**
```cpp
for (nlm::SimulationStep step = 0; step < maxSteps; ++step) {
    brain->step(step);
    // Process output, collect data, etc.
}
```

### I/O Methods

#### Receive Sensory Input

```cpp
void Brain::receiveSensoryInput(const SensoryInput& input);
```

**Parameters:**
- `input`: Sensory input data containing sensory signals

**Description:**
Injects sensory input into sensory neurons of the brain. This method:

1. **Distributes input**: Spreads input across all sensory neurons
2. **Normalizes values**: Maps input to appropriate current range (-10 to 10 mV)
3. **Updates working memory**: Stores salient sensory input in working memory
4. **Applies neuromodulation**: May trigger novelty detection

**Example:**
```cpp
nlm::SensoryInput visualInput;
// Populate visualInput with visual data...
brain->receiveSensoryInput(visualInput);
```

#### Produce Action

```cpp
std::unique_ptr<Action> Brain::produceAction();
```

**Returns:**
- Unique pointer to action object

**Description:**
Performs action selection based on motor neuron activity. The method:

1. **Evaluates motor neuron groups**: Analyzes activity of motor neuron populations
2. **Selects action**: Chooses action based on highest motor activity
3. **Returns action**: Creates and returns action object

**Example:**
```cpp
auto action = brain->produceAction();
if (action->getType() == ActionType::MoveForward) {
    // Execute movement...
}
```

### System Management Methods

#### Reset

```cpp
void Brain::reset();
```

**Description:**
Resets the brain to its initial state, clearing all neural activity and memory traces.

#### Save and Load

```cpp
bool Brain::save(const std::string& filepath) const;
bool Brain::load(const std::string& filepath);
```

**Parameters:**
- `filepath`: Path to checkpoint file

**Returns:**
- `true` if operation succeeds, `false` otherwise

**Description:**
Manages checkpoint save/load functionality for persistence and resuming simulations.

#### Log Status

```cpp
void Brain::logStatus() const;
```

**Description:**
Outputs comprehensive system status information including neuron counts, synapse counts, firing rates, and component states.

### System Access Methods

#### Region Management

```cpp
NeuralRegion* Brain::getRegion(RegionId id);
const NeuralRegion* Brain::getRegion(RegionId id) const;
RegionId Brain::addRegion(const std::string& name);
```

**Description:**
Provides access to neural regions and region creation functionality.

#### Statistics Methods

```cpp
size_t Brain::getRegionCount() const;
std::vector<RegionId> Brain::getRegionIds() const;
size_t Brain::getTotalNeuronCount() const;
size_t Brain::getTotalSynapseCount() const;
size_t Brain::getActiveNeuronCount() const;
size_t Brain::getFiringNeuronCount() const;
float Brain::getAverageFiringRate() const;
float Brain::getExcitationInhibitionRatio() const;
```

**Description:**
Provides various statistics about the brain system for monitoring and analysis.

#### System Component Access

```cpp
NeuralWorkingMemory* Brain::getWorkingMemory();
NeuralEpisodicMemory* Brain::getEpisodicMemory();
NeuralAssociativeMemory* Brain::getAssociativeMemory();
PredictionSystem* Brain::getPredictionSystem();
NeuralPlanner* Brain::getPlanner();
ConceptFormation* Brain::getConceptFormation();
AttentionalSelection* Brain::getAttention();
DevelopmentSystem* Brain::getDevelopmentSystem();
Dopamine* Brain::getDopamine();
Curiosity* Brain::getCuriosity();
Novelty* Brain::getNovelty();
PredictionError* Brain::getPredictionErrorSignal();
```

**Description:**
Provides access to all integrated brain systems for advanced applications and customization.

#### Neural Computation Components

```cpp
SpikeSystem* Brain::getSpikeSystem();
const SpikeSystem* Brain::getSpikeSystem() const;
STDP* Brain::getSTDP();
Hebbian* Brain::getHebbian();
StructuralPlasticity* Brain::getStructuralPlasticity();
```

**Description:**
Provides access to core neural computation components for fine-grained control.

#### Configuration and Random Access

```cpp
std::shared_ptr<const Config> Brain::getConfig() const;
RandomGenerator* Brain::getRandomGenerator();
```

**Description:**
Provides access to configuration and random number generator.

#### Statistics and Monitoring

```cpp
size_t Brain::getTotalSpikeCount() const;
size_t Brain::getPendingSpikeEventCount() const;
```

**Description:**
Provides statistics for monitoring simulation progress and performance.

### Error Handling

The Brain class uses the NLM exception system for error reporting:

```cpp
try {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    brain->step(0);
} catch (const nlm::NLMException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << "Category: " << static_cast<int>(e.getErrorCategory()) << std::endl;
    std::cerr << "Code: " << static_cast<int>(e.getErrorCode()) << std::endl;
}
```

## Brain Construction

### Standard Construction

```cpp
std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
```

**Parameters:**
- `config`: Configuration object

**Returns:**
- Shared pointer to Brain instance

**Description:**
Factory function for creating Brain instances with standard configuration.

### Builder-Based Construction

```cpp
std::shared_ptr<Brain> brain = nlm::BrainBuilder()
    .createCognitiveArchitecture()
    .withNeuronCount(5000)
    .withRegionCount(3)
    .withDopamineLevel(0.7f)
    .withCuriosityLevel(0.8f)
    .buildAndInitialize();
```

### Task-Based Construction

```cpp
std::shared_ptr<Brain> brain = nlm::BrainFactory::createBrainForTask(
    nlm::TaskType::Navigation, 
    2000
);
```

## Integration Methods

### Memory System Integration

The Brain class provides access to integrated memory systems:

```cpp
// Working memory
auto workingMemory = brain->getWorkingMemory();
workingMemory->storeToNeuron(neuronId, activation);
workingMemory->update(dt);

// Episodic memory
auto episodicMemory = brain->getEpisodicMemory();
EpisodicMemoryItem episode;
episode.timestamp = step;
episode.reward = reward;
episodicMemory->storeEpisode(episode);

// Associative memory
auto associativeMemory = brain->getAssociativeMemory();
associativeMemory->associate(pattern1, pattern2, strength);
```

### Cognitive System Integration

```cpp
// Prediction system
auto predictionSystem = brain->getPredictionSystem();
// Process prediction errors

// Planning system
auto planner = brain->getPlanner();
planner->setPlanningDepth(5);
planner->planAction(currentState);

// Attention system
auto attention = brain->getAttention();
attention->setInhibitionStrength(0.5f);
attention->setExcitationStrength(1.5f);

// Concept formation
auto conceptFormation = brain->getConceptFormation();
conceptFormation->update(brain.get());
```

### Neuromodulation Integration

```cpp
// Dopamine system
auto dopamine = brain->getDopamine();
dopamine->setLevel(0.5f);
dopamine->applyToNeuralExcitability(brain.get());

// Curiosity system
auto curiosity = brain->getCuriosity();
curiosity->setLevel(0.7f);
curiosity->applyToExploration(brain.get());
```

## Configuration Methods

The Brain class provides configuration validation and parameter access:

```cpp
// Validate configuration
bool isValid = brain->validateConfiguration();

// Get configuration parameters
auto config = brain->getConfig();
size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);

// Set configuration parameters
config->set("dopamine_level", 0.7f);
```

## Simulation Control

### Advanced Simulation Control

```cpp
// Step with specific time
brain->step(currentStep, currentTime);

// Step with automatic time calculation
double timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
brain->step(currentStep, currentTime + timestep);

// Batch processing
for (nlm::SimulationStep step = 0; step < maxSteps; step += batchSize) {
    brain->step(step, step * timestep);
}
```

### Performance Monitoring

```cpp
// Monitor simulation performance
brain->logStatus();

// Get performance statistics
size_t totalSpikes = brain->getTotalSpikeCount();
size_t pendingEvents = brain->getPendingSpikeEventCount();
float eiratio = brain->getExcitationInhibitionRatio();
```

## Memory Systems

The Brain class provides access to all integrated memory systems:

### Working Memory

```cpp
// Access working memory
auto workingMemory = brain->getWorkingMemory();

// Store and retrieve traces
workingMemory->storeToNeuron(neuronId, activation);
workingMemory->update(dt);
auto traces = workingMemory->getActiveTraces();
```

### Episodic Memory

```cpp
// Access episodic memory
auto episodicMemory = brain->getEpisodicMemory();

// Store episodes
EpisodicMemoryItem episode;
episode.timestamp = step;
episode.reward = reward;
episodicMemory->storeEpisode(episode);

// Retrieve episodes
auto episodes = episodicMemory->getEpisodesForReplay(count);
```

### Associative Memory

```cpp
// Access associative memory
auto associativeMemory = brain->getAssociativeMemory();

// Form associations
associativeMemory->associate(pattern1, pattern2, strength);

// Retrieve associations
auto association = associativeMemory->getAssociation(pattern1);
```

## Error Handling

The Brain class uses the NLM exception system for comprehensive error handling.

### Error Categories

- **Configuration**: Configuration parameter errors
- **Neural**: Neural computation errors
- **Memory**: Memory system errors
- **Simulation**: Simulation control errors
- **Integration**: System integration errors
- **Performance**: Performance-related errors
- **ThreadSafety**: Thread safety violations
- **Resource**: Resource allocation errors

### Exception Handling Example

```cpp
void runSimulation() {
    try {
        auto brain = createBrain(config);
        brain->initialize();
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < maxSteps; ++step) {
            brain->step(step);
            // Process output...
        }
        
    } catch (const nlm::ConfigurationException& e) {
        // Handle configuration errors
        std::cerr << "Configuration error: " << e.what() << std::endl;
    } catch (const nlm::NeuralException& e) {
        // Handle neural computation errors
        std::cerr << "Neural error: " << e.what() << std::endl;
    } catch (const nlm::MemoryException& e) {
        // Handle memory system errors
        std::cerr << "Memory error: " << e.what() << std::endl;
    } catch (const nlm::NLMException& e) {
        // Handle general NLM errors
        std::cerr << "NLM error: " << e.what() << std::endl;
    }
}
```

## Integration Guidelines

### Building Custom Applications

When building custom applications with the Brain class, follow these guidelines:

1. **Error Handling**: Always use try-catch blocks for Brain operations
2. **Configuration**: Validate configuration before brain initialization
3. **Resource Management**: Use smart pointers for Brain instances
4. **Performance Monitoring**: Regularly call logStatus() for monitoring
5. **Cleanup**: Properly cleanup Brain instances to release resources

### Best Practices

```cpp
// Best practice: Proper error handling and resource management
std::shared_ptr<Brain> brain;

try {
    brain = createBrain(config);
    brain->initialize();
    
    // Run simulation...
    
    // Save checkpoint for recovery
    brain->save("checkpoint.sav");
    
} catch (const std::exception& e) {
    std::cerr << "Simulation failed: " << e.what() << std::endl;
    return 1;
}

// Cleanup handled automatically by smart pointer
return 0;
```

## Migration Guide

### From Old API to New API

If migrating from an older version of the NLM framework:

```cpp
// Old API
Brain brain(config);
brain.initialize();
brain.step(step);

// New API
auto brain = BrainBuilder()
    .createCognitiveArchitecture()
    .withNeuronCount(1000)
    .buildAndInitialize();
```

### Performance Considerations

- **Use Builder API**: The new Builder API provides better performance through fluent configuration
- **Configure for Task**: Choose appropriate presets for specific tasks
- **Optimize Parameters**: Adjust parameters for your specific use case
- **Monitor Performance**: Use logStatus() to monitor simulation performance

## Future Enhancements

The Brain API will continue to evolve with:

1. **New Configuration Options**: Additional parameters for fine-grained control
2. **Advanced Features**: New cognitive architectures and learning mechanisms
3. **Integration APIs**: Enhanced integration with external systems
4. **Performance Optimizations**: Further performance improvements and hardware acceleration

## License and Attribution

This documentation is part of the NLM framework. Please cite the original research when using this code in publications.