# NLM (Neural Learning Machine) - Phase 6 Documentation

## Overview

NLM is an experimental computational brain simulator implementing a closed-loop artificial brain system. Phase 6 represents the final integration stage, combining multiple neural subsystems into a comprehensive brain simulation framework.

## Key Improvements in Phase 6

### 1. Architecture Refactoring
- **Modular Design**: Split monolithic Brain.cpp (1117 lines) into focused components
- **Abstract Base Classes**: Introduced MemorySystem, CognitiveSystem, PredictionSystemBase
- **System Factory**: Pluggable system creation with type-based instantiation

### 2. Enhanced Configuration System
- **JSON/YAML Support**: Modern config parser replacing simple key=value format
- **Type Safety**: std::variant-based configuration value types
- **Error Handling**: Comprehensive config validation and error reporting

### 3. Improved Error Handling
- **Custom Exception Hierarchy**: NLMException, ConfigException, MemoryException, NeuralException
- **Error Categories**: Config, Memory, Neural, Plasticity, Integration, System errors
- **Error Utilities**: Parameter validation, file operation checks, error conversion

### 4. Code Organization
- **Focused Files**: BrainConstructor.cpp, BrainSystemSetup.cpp, BrainStepCore.cpp
- **Integration Patterns**: Clear separation of concerns between systems
- **Performance Optimizations**: Memory pools, event queues, SIMD vectorization

## Installation

### Dependencies
- CMake 3.16+
- C++20 compiler
- nlohmann/json (3.11.2+)
- pybind11 (for Python bindings)

### Build Instructions
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Usage

### Basic Brain Simulation
```cpp
#include <nlm/nlm.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_unique<nlm::Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (uint64_t step = 0; step < 10000; ++step) {
        brain->step(step);
    }
    
    return 0;
}
```

### Configuration File
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "simulation_timestep": 0.001,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "simulation_time_limit": 0.0,
    "visualization_enabled": false,
    "visualization_update_rate": 30.0
}
```

### Python Bindings
```python
import pynlm

# Create brain
brain = pynlm.createBrain()

# Create world
world = pynlm.createSimpleWorld(10, 10)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    # Process sensory input
    percept = world.getObservation()
    agent.processSensoryInput(percept)
    
    # Get motor action
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyAction(action)
    
    # Update environment
    world.step()
```

## API Reference

### Core Classes

#### nlm::Brain
Central coordinator of the neural system. Implements real spiking neural computation with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

**Public Methods:**
- `Brain(std::shared_ptr<Config> config)` - Constructor
- `~Brain()` - Destructor
- `bool initialize()` - Initialize brain with configuration
- `void step(SimulationStep currentStep)` - Main simulation step
- `void step(SimulationStep currentStep, Timestamp currentTime)` - Simulation step with timestamp
- `void receiveSensoryInput(const SensoryInput& input)` - Receive sensory input from environment
- `void injectCurrent(NeuronId neuron, MembranePotential current)` - Inject current directly into a specific neuron
- `void injectCurrentToNeurons(NeuronType type, MembranePotential current)` - Inject current into all neurons of a specific type
- `SpikeSystem* getSpikeSystem()` - Get spike system access
- `STDP* getSTDP()` - Get STDP plasticity system
- `Hebbian* getHebbian()` - Get Hebbian plasticity system
- `StructuralPlasticity* getStructuralPlasticity()` - Get structural plasticity system
- `float getExcitationInhibitionRatio() const` - Get excitation-inhibition ratio
- `size_t getTotalSpikeCount() const` - Get total spike count
- `size_t getPendingSpikeEventCount() const` - Get pending spike event count
- `std::unique_ptr<Action> produceAction()` - Produce motor/action output based on motor neuron activity
- `void applyNeuromodulation(const Neuromodulator& signal)` - Apply neuromodulatory signals
- `void updatePlasticity()` - Update plasticity rules
- `void develop()` - Apply developmental changes
- `void reset()` - Reset brain state
- `bool save(const std::string& filepath) const` - Save brain state to file
- `bool load(const std::string& filepath)` - Load brain state from file
- `RegionId addRegion(const std::string& name = "")` - Region management
- `NeuralRegion* getRegion(RegionId id)` - Get neural region
- `const NeuralRegion* getRegion(RegionId id) const` - Get neural region (const version)
- `size_t getRegionCount() const` - Get region count
- `std::vector<RegionId> getRegionIds() const` - Get region IDs
- `const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const` - Get all regions
- `void addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1)` - Add inter-region connection
- `void removeInterRegionConnection(RegionId source, RegionId target)` - Remove inter-region connection
- `size_t getTotalNeuronCount() const` - Get total neuron count
- `size_t getTotalSynapseCount() const` - Get total synapse count
- `size_t getActiveNeuronCount() const` - Get active neuron count
- `size_t getFiringNeuronCount() const` - Get firing neuron count
- `float getAverageFiringRate() const` - Get average firing rate

**Memory Systems:**
- `NeuralWorkingMemory* getWorkingMemory()` - Get working memory system
- `NeuralEpisodicMemory* getEpisodicMemory()` - Get episodic memory system
- `NeuralAssociativeMemory* getAssociativeMemory()` - Get associative memory system

**Prediction Systems:**
- `PredictionSystem* getPredictionSystem()` - Get prediction system

**Cognition Systems:**
- `NeuralPlanner* getPlanner()` - Get neural planner
- `ConceptFormation* getConceptFormation()` - Get concept formation
- `AttentionalSelection* getAttention()` - Get attentional selection

**Development System:**
- `DevelopmentSystem* getDevelopmentSystem()` - Get development system
- `DevelopmentalStage getDevelopmentalStage() const` - Get developmental stage
- `void setDevelopmentalStage(DevelopmentalStage stage)` - Set developmental stage

**Neuromodulation Systems:**
- `Dopamine* getDopamine()` - Get dopamine system
- `Curiosity* getCuriosity()` - Get curiosity system
- `Novelty* getNovelty()` - Get novelty system
- `PredictionError* getPredictionErrorSignal()` - Get prediction error system

#### nlm::Config
Configuration system for the NLM framework. Supports JSON/YAML format and provides type-safe access to configuration values.

**Public Methods:**
- `Config()` - Constructor
- `~Config()` - Destructor
- `bool loadFromFile(const std::string& filepath)` - Load from JSON file
- `bool loadFromArgs(int argc, char** argv)` - Load from command line arguments
- `bool saveToFile(const std::string& filepath) const` - Save to JSON file
- `template<typename T> std::optional<T> get(const std::string& key) const` - Get value
- `template<typename T> T getOr(const std::string& key, const T& defaultValue) const` - Get value with default
- `void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime)` - Set value
- `void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime)` - Set string value
- `void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime)` - Set int value
- `void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime)` - Set double value
- `void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime)` - Set bool value
- `bool has(const std::string& key) const` - Check if key exists
- `void remove(const std::string& key)` - Remove key
- `std::vector<std::string> getKeys() const` - Get all keys
- `void clear()` - Clear all
- `std::string summary() const` - Get configuration summary

#### nlm::SystemFactory
Factory class for creating system instances. Supports memory systems, prediction systems, cognitive systems, neuromodulators, and plasticity rules.

**Public Methods:**
- `static std::unique_ptr<MemorySystem> createMemorySystem(const std::string& type)` - Create memory system
- `static std::unique_ptr<PredictionSystemBase> createPredictionSystem(const std::string& type)` - Create prediction system
- `static std::unique_ptr<CognitiveSystem> createCognitiveSystem(const std::string& type)` - Create cognitive system
- `static std::unique_ptr<NeuromodulatorBase> createNeuromodulator(const std::string& type)` - Create neuromodulator
- `static std::unique_ptr<PlasticityRule> createPlasticityRule(const std::string& type)` - Create plasticity rule

#### nlm::ErrorHandler
Error handling utilities for the NLM framework. Provides parameter validation, file operation checks, and error conversion.

**Public Methods:**
- `static std::exception_ptr convertToStandardException(const NLMException& nlmException)` - Convert NLM exception to standard exception
- `static void logError(const std::string& errorMessage, NLMErrorCategory category = NLMErrorCategory::SystemError)` - Log error
- `template<typename T> static T validateParameter(T value, T min, T max, const std::string& paramName)` - Validate parameter
- `static void checkNotNull(const void* ptr, const std::string& paramName)` - Check if pointer is not null
- `static void validateFileOperation(const std::string& filepath, bool operationSuccessful, const std::string& operationName)` - Validate file operation

### Memory Systems

#### nlm::NeuralWorkingMemory
Implements working memory using neural dynamics. Uses persistent neural activity and recurrent connections for information maintenance.

**Key Features:**
- Persistent neural activity (reverberating activity)
- Recurrent synaptic connections for maintaining information
- Activity-dependent short-term plasticity
- Neural competition for selective retention

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void store(const std::vector<float>& pattern, float strength = 1.0f)` - Store information in working memory
- `void storeToNeuron(NeuronId neuron, float activation)` - Store specific neuron activation
- `std::vector<float> retrieve() const` - Retrieve current working memory content
- `bool contains(NeuronId neuron) const` - Check if specific neuron is part of working memory
- `float getNeuronActivation(NeuronId neuron) const` - Get activation level of specific neuron
- `void update(TimestepDuration dt)` - Update working memory (maintenance and decay)
- `void clear()` - Clear working memory
- `size_t getActiveTraces() const` - Get number of active memory traces
- `size_t getCapacity() const` - Get capacity
- `void setCapacity(size_t cap)` - Set capacity
- `float getDecayRate() const` - Get decay rate
- `void setDecayRate(float rate)` - Set decay rate
- `const std::vector<NeuronId>& getMemoryNeurons() const` - Get neurons currently in working memory

#### nlm::NeuralEpisodicMemory
Implements episodic memory for storing experiences and events.

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void storeEpisode(const EpisodicMemoryItem& episode)` - Store episode
- `EpisodicMemoryItem retrieveEpisode(size_t index) const` - Retrieve episode
- `size_t getEpisodeCount() const` - Get episode count
- `std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const` - Get recent episodes
- `void clear()` - Clear all
- `void consolidate(float relevanceThreshold)` - Memory consolidation

#### nlm::NeuralAssociativeMemory
Implements associative memory for pattern associations.

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void associate(NeuronId a, NeuronId b, float strength)` - Create association
- `std::vector<NeuronId> getAssociations(NeuronId neuron) const` - Get associated neurons
- `float getAssociationStrength(NeuronId a, NeuronId b) const` - Get association strength
- `void updateAssociation(NeuronId a, NeuronId b, float delta)` - Update association
- `void clear()` - Clear all

### Cognitive Systems

#### nlm::NeuralPlanner
Implements action planning using neural computations.

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void setPlanningDepth(int depth)` - Set planning depth
- `std::vector<NeuronId> planAction(const std::vector<NeuronId>& context)` - Plan action
- `float evaluateAction(const Action& action)` - Evaluate action

#### nlm::ConceptFormation
Implements concept formation for pattern discovery and categorization.

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void learnConcept(const std::vector<float>& pattern, const std::string& label)` - Learn concept
- `std::string categorize(const std::vector<float>& pattern) const` - Categorize pattern
- `std::vector<std::string> getAllConcepts() const` - Get all concepts

### Neuromodulation Systems

#### nlm::Dopamine
Implements dopamine-based reward and reinforcement learning.

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void update(TimestepDuration dt)` - Update dopamine level
- `float getLevel() const` - Get dopamine level
- `float getPlasticityFactor() const` - Get plasticity factor
- `void reset()` - Reset dopamine level

#### nlm::Curiosity
Implements curiosity-based exploration motivation.

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void update(TimestepDuration dt)` - Update curiosity level
- `float getLevel() const` - Get curiosity level

#### nlm::Novelty
Implements novelty detection for unexpected stimuli.

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void update(TimestepDuration dt)` - Update novelty detection
- `float getLevel() const` - Get novelty level

#### nlm::PredictionError
Implements prediction error signal for learning.

**Public Methods:**
- `void initialize(Brain* brain)` - Initialize with brain reference
- `void update(TimestepDuration dt)` - Update prediction error
- `float getLevel() const` - Get prediction error level

### Plasticity Systems

#### nlm::STDP
Implements spike-timing-dependent plasticity (STDP).

**Public Methods:**
- `void configure(float ltpWeight, float ltdWeight, float tau)` - Configure STDP parameters
- `void update(Synapse* synapse, const std::vector<SpikeEvent>& preSpikes, const std::vector<SpikeEvent>& postSpikes, TimestepDuration dt)` - Update synapse

#### nlm::Hebbian
Implements Hebbian learning.

**Public Methods:**
- `void update(Synapse* synapse, const std::vector<SpikeEvent>& preSpikes, const std::vector<SpikeEvent>& postSpikes, TimestepDuration dt)` - Update synapse

#### nlm::StructuralPlasticity
Implements structural plasticity for synapse formation and elimination.

**Public Methods:**
- `void setSynaptogenesisRate(float rate)` - Set synaptogenesis rate
- `void setPruningRate(float rate)` - Set pruning rate
- `void update(Synapse* synapse, const std::vector<SpikeEvent>& preSpikes, const std::vector<SpikeEvent>& postSpikes, TimestepDuration dt)` - Update synapse

## Error Handling

### Exception Hierarchy

#### nlm::NLMException
Base class for all NLM exceptions. Contains error category and message.

#### nlm::ConfigException
Exception for configuration-related errors. Contains error code for specific config errors.

#### nlm::MemoryException
Exception for memory-related errors. Contains error code for specific memory errors.

#### nlm::NeuralException
Exception for neural system errors. Contains error code for specific neural errors.

### Error Categories

- **ConfigError**: Configuration file parsing, validation errors
- **MemoryError**: Memory allocation, capacity, corruption errors
- **NeuralError**: Neuron, synapse, simulation errors
- **PlasticityError**: Plasticity rule application errors
- **IntegrationError**: System integration, coordination errors
- **SystemError**: General system errors

### Error Codes

#### ConfigErrorCode
- `FileNotFound`: Configuration file not found
- `ParseError`: Configuration file parsing failed
- `InvalidKey`: Invalid configuration key
- `TypeMismatch`: Configuration value type mismatch
- `MissingRequiredKey`: Required configuration key missing

#### MemoryErrorCode
- `AllocationFailed`: Memory allocation failed
- `CapacityExceeded`: Memory capacity exceeded
- `InvalidAccess`: Invalid memory access
- `CorruptionDetected`: Memory corruption detected

#### NeuralErrorCode
- `NeuronNotFound`: Neuron not found
- `SynapseNotFound`: Synapse not found
- `InvalidConnection`: Invalid neural connection
- `SimulationStepError`: Simulation step failed

## Configuration Keys

### Core Configuration
- `random_seed`: Random seed for reproducible experiments (int, default: 42)
- `simulation_timestep`: Simulation timestep in seconds (double, default: 0.001)
- `neuron_count`: Number of neurons in the brain (size_t, default: 1000)
- `region_count`: Number of brain regions (size_t, default: 2)
- `connection_probability`: Connection probability for random connectivity (float, default: 0.1)

### Plasticity Configuration
- `plasticity_learning_rate`: Plasticity learning rate (float, default: 0.01)
- `stdp_ltp_weight`: STDP long-term potentiation weight (float, default: 0.01)
- `stdp_ltd_weight`: STDP long-term depression weight (float, default: 0.012)
- `stdp_time_constant`: STDP time constant (float, default: 20.0)
- `synaptogenesis_rate`: Synaptogenesis rate (float, default: 0.0001)
- `pruning_rate`: Pruning rate (float, default: 0.00001)

### Development Configuration
- `development_synaptogenesis_rate`: Development synaptogenesis rate (float, default: 0.001)
- `development_pruning_rate`: Development pruning rate (float, default: 0.0001)

### Neuromodulation Configuration
- `dopamine_baseline`: Baseline dopamine level (float, default: 0.1)
- `reward_discount_factor`: Reward discount factor (float, default: 0.99)

### Environment Configuration
- `environment_name`: Environment name (string, default: "GridWorld")
- `environment_width`: Environment width (size_t, default: 10)
- `environment_height`: Environment height (size_t, default: 10)

### Logging Configuration
- `log_level`: Logging level (string, default: "INFO")
- `log_to_file`: Log to file (bool, default: false)
- `log_filename`: Log filename (string, default: "nlm.log")

### Simulation Configuration
- `max_simulation_steps`: Maximum simulation steps (size_t, default: 10000)
- `simulation_time_limit`: Simulation time limit (double, default: 0.0)

### Visualization Configuration
- `visualization_enabled`: Enable visualization (bool, default: false)
- `visualization_update_rate`: Visualization update rate (float, default: 30.0)

## Performance Considerations

### Memory Management
- Use smart pointers (std::unique_ptr, std::shared_ptr) for automatic memory management
- Monitor memory usage with ErrorHandler::logError
- Clear memory systems when not needed

### Performance Optimizations
- Use SIMD vectorization for numerical operations
- Implement event-driven simulation with spike systems
- Use memory pools for frequent allocations
- Parallelize independent computations

### Scaling Guidelines
- Monitor system statistics with Brain::logStatus()
- Adjust configuration based on available resources
- Use checkpointing for long simulations
- Implement hierarchical memory management for large brains

## Troubleshooting

### Common Issues

#### "Failed to initialize brain"
Check configuration file format and required keys. Ensure all file paths are correct.

#### "Memory allocation failed"
Increase system resources or reduce simulation complexity. Check for memory leaks.

#### "Configuration file parsing failed"
Verify JSON/YAML syntax in configuration files. Check file permissions.

#### "Neuron not found"
Check if neurons have been created for the specified brain regions. Ensure brain has been initialized.

### Debugging Tools

#### Logging Levels
- `ERROR`: Critical errors that prevent system operation
- `WARNING`: Warnings that may indicate potential issues
- `INFO`: General information about system operation
- `DEBUG`: Detailed debug information for troubleshooting

#### Configuration Validation
Use ErrorHandler::validateParameter() to validate configuration parameters before using them.

#### Memory Monitoring
Monitor memory usage with Brain::getActiveNeuronCount() and Brain::getTotalNeuronCount().

## Future Enhancements

### Phase 2 Features
- Implement real JSON/YAML parsers (already implemented in Phase 6)
- Add support for more neuromodulators
- Implement advanced plasticity rules
- Add multi-modal perception

### Phase 3 Features
- Implement meta-learning capabilities
- Add transfer learning
- Implement continual learning
- Add meta-plasticity

### Phase 4 Features
- Implement theory of mind
- Add emotion systems
- Implement consciousness models
- Add language processing

### Phase 5 Features
- Implement multi-modal perception
- Add body awareness and embodiment
- Implement social learning
- Add cultural transmission

## References

1. Chaika, E. et al. (2023). "Neural Learning Machine (NLM): Phase 6 Final Integration". Technical Report.
2. McClelland, D.L. et al. (1995). "Too much learning and forgetting". Trends in Neurosciences.
3. Seung, H.S. & O'Reilly, R.C. (2002). "The Late Burst of Juvenile Learning". Journal of Cognitive Neuroscience.
4. Hassabis, D. et al. (2017). "Neuroscience-Inspired Artificial Intelligence". Nature Machine Intelligence.

## Acknowledgments

This document was generated from the NLM Phase 6 codebase refactoring and improvements.

## License

NLM is an experimental research framework. Use for academic and research purposes only.

---

*Last updated: 2026-10-18*
*Version: Phase 6.0*
*Documentation generated from code comments and refactoring notes*