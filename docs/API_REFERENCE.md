# NLM API Reference: Complete Documentation of NLM's Public Interface

## Overview

This document provides comprehensive reference documentation for NLM's public API. It covers all classes, methods, enumerations, and types available in the NLM framework.

## Module Structure

NLM's public API is organized into several main modules:

### Core Brain Components
- `Brain` - The main neural network class
- `Neuron` - Individual neuron implementation
- `Synapse` - Neural connections between neurons
- `NeuralRegion` - Organization of neurons into functional regions
- `NeuralPopulation` - Groups of neurons with similar properties

### Memory Systems
- `WorkingMemory` - Active maintenance of current neural patterns
- `EpisodicMemory` - Storage of experienced events and episodes
- `AssociativeMemory` - Pattern completion and recall mechanisms
- `SemanticMemory` - Abstract knowledge representation
- `ProceduralMemory` - Skills and behavioral patterns

### Neuromodulation Systems
- `Dopamine` - Reward prediction error and reinforcement
- `Curiosity` - Novelty detection and exploration drive
- `Novelty` - Surprise detection and learning trigger
- `PredictionError` - Mismatch between expected and actual outcomes

### Cognitive Systems
- `NeuralPlanner` - Action sequence evaluation and planning
- `ConceptFormation` - Abstract concept learning from patterns
- `AttentionalSelection` - Competitive selection of neural activity
- `PredictionSystem` - Forward models and prediction

### Development System
- `DevelopmentSystem` - Brain maturation and structural changes
- `Synaptogenesis` - Formation of new synaptic connections
- `Pruning` - Elimination of inefficient connections
- `Maturation` - Refinement of neural circuits

### Sensory and Motor Systems
- `SensoryInput` - Input from environment (vision, audition, etc.)
- `InternalSignals` - Internal body state and feelings
- `Vision` - Visual processing system
- `Audio` - Auditory processing system
- `MotorSystem` - Output action generation

### Environment Interface
- `Environment` - Abstract environment interface
- `SimpleWorld` - Simple 2D grid world environment
- `Observation` - Information gathered from environment
- `Action` - Actions that can be taken in environment

### Agent Interface
- `AgentBrain` - Bridge between brain and environment
- `SensoryPercept` - Sensory information processed by agent
- `AgentBody` - Physical representation of agent

### Experiments and Testing
- `Experiment` - Base class for experimental protocols
- `ExperimentRunner` - Framework for running experiments
- `Metrics` - Collection and analysis of experimental data
- `Phase3Experiment`, `Phase4Experiment`, `Phase6IntegratedExperiment` - Specific experiment types

### Performance and Optimization
- `MemoryPool` - Memory management and optimization
- `EventQueue` - Efficient event scheduling
- `ParallelProcessing` - Multi-threading and parallel execution
- `SIMDVectorization` - Single Instruction Multiple Data optimizations
- `CheckpointSystem` - Save and load brain state

### Visualization
- `VisualizationInterface` - 3D visualization of neural activity

## Brain Class (Primary Interface)

The `Brain` class is the main interface for working with NLM brains. It provides access to all brain components and simulation functionality.

### Class Definition

```cpp
template<typename ConfigType = Config>
class Brain {
public:
    // Construction and destruction
    explicit Brain(std::shared_ptr<ConfigType> config);
    ~Brain();
    
    // Copy and move operations
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core simulation methods
    bool initialize();
    void reset();
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp timestamp);
    
    // Input/output methods
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    
    // State management
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Component access methods
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
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
    
    // Statistics methods
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Development methods
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    void develop();
    
    // Plasticity methods
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    
    // Logging and status
    void logStatus() const;
    
    // Region management
    RegionId addRegion(const std::string& name);
    std::vector<RegionId> getRegionIds() const;
    size_t getRegionCount() const;
    
    // Synaptic management
    void addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Connectivity methods
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
};
```

### Key Methods

#### Construction and Initialization

```cpp
// Create brain with default configuration
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);

// Initialize the brain
if (brain->initialize()) {
    std::cout << "Brain initialized successfully!" << std::endl;
} else {
    std::cerr << "Failed to initialize brain!" << std::endl;
    return 1;
}
```

#### Simulation Loop

```cpp
// Basic simulation
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step);
    
    // Log statistics periodically
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": " 
                  << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
    }
}
```

#### State Management

```cpp
// Save brain state
brain->save("my_brain_checkpoint.bin");

// Load brain state
auto newBrain = std::make_shared<Brain>(config);
newBrain->initialize();
newBrain->load("my_brain_checkpoint.bin");
```

#### Component Access

```cpp
// Access specific brain components
auto* workingMemory = brain->getWorkingMemory();
auto* episodicMemory = brain->getEpisodicMemory();
auto* dopamine = brain->getDopamine();
auto* curiosity = brain->getCuriosity();
auto* planner = brain->getPlanner();
```

#### Statistics and Monitoring

```cpp
// Get comprehensive brain statistics
brain->logStatus();

// Get specific statistics
std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
```

#### Development Control

```cpp
// Set development stage
brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);

// Check current development stage
DevelopmentalStage stage = brain->getDevelopmentalStage();
std::cout << "Developmental stage: " << static_cast<int>(stage) << std::endl;

// Apply development effects
brain->develop();
```

## Neuron Class

The `Neuron` class represents individual neurons in the NLM system.

### Class Definition

```cpp
class Neuron {
public:
    // Construction and destruction
    explicit Neuron(NeuronId id);
    ~Neuron();
    
    // Copy and move operations
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // State setters
    void setType(NeuronType type);
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    void setThreshold(MembranePotential threshold);
    void setRestingPotential(MembranePotential potential);
    void setResetPotential(MembranePotential potential);
    void setFiringRate(FiringRate rate);
    void setLeakConductance(MembranePotential conductance);
    void setRefractoryPeriod(uint32_t steps);
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void setFiringRate(FiringRate rate);
    void setLeakConductance(MembranePotential conductance);
    void setRefractoryPeriod(uint32_t steps);
    
    // State getters
    NeuronId getId() const;
    NeuronType getType() const;
    NeuronState getState() const;
    NeuronState& getState();
    MembranePotential getMembranePotential() const;
    MembranePotential getThreshold() const;
    MembranePotential getRestingPotential() const;
    MembranePotential getResetPotential() const;
    FiringState getFiringState() const;
    float getLastSpikeTime() const;
    uint32_t getRefractoryPeriod() const;
    
    // Input methods
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    void injectCurrent(MembranePotential current);
    
    // Spike management
    void recordSpike(Timestamp timestamp);
    void clearSpikeHistory();
    float getLastSpikeTime() const;
    std::vector<float> getSpikeHistory() const;
    
    // Connectivity management
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    std::vector<SynapseHandle> getIncomingSynapses() const;
    std::vector<SynapseHandle> getOutgoingSynapses() const;
    
    // Plasticity management
    PlasticityFlags& getPlasticityFlags();
    const PlasticityFlags& getPlasticityFlags() const;
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Location management
    void setRegionId(RegionId region);
    void setPopulationId(PopulationId population);
    RegionId getRegionId() const;
    PopulationId getPopulationId() const;
    
    // Simulation methods
    bool checkThreshold() const;
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    void step(Timestamp currentTime);
    void reset();
    void initializeRandom(RandomGenerator& rng);
};
```

### Key Methods

#### State Management

```cpp
// Set and validate neuron properties
neuron->setType(NeuronType::Excitatory);
neuron->setMembranePotential(-65.0f);
neuron->setThreshold(-55.0f);
neuron->setRestingPotential(-70.0f);
neuron->setResetPotential(-75.0f);
neuron->setRefractoryPeriod(2);  // 2ms refractory period

// Get neuron state
std::cout << "Membrane potential: " << neuron->getMembranePotential() << " mV" << std::endl;
std::cout << "Threshold: " << neuron->getThreshold() << " mV" << std::endl;
std::cout << "Firing state: " << static_cast<int>(neuron->getFiringState()) << std::endl;
```

#### Input Processing

```cpp
// Process synaptic inputs
neuron->receiveExcitatoryInput(10.0f);   // Excitatory input
neuron->receiveInhibitoryInput(5.0f);    // Inhibitory input
neuron->injectCurrent(2.5f);             // Direct current injection

// Spike recording
neuron->recordSpike(currentTime);
```

#### Plasticity Control

```cpp
// Configure plasticity
neuron->enablePlasticity(true, true, false);  // Hebbian and STDP

// Check plasticity flags
auto flags = neuron->getPlasticityFlags();
std::cout << "Hebbian: " << flags.hebbian << std::endl;
std::cout << "STDP: " << flags.stdp << std::endl;
```

## Config Class

The `Config` class manages configuration settings for NLM brains.

### Class Definition

```cpp
class Config {
public:
    // Construction and destruction
    Config();
    ~Config();
    
    // Copy and move operations
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    // Key-value operations
    void set(const std::string& key, const ConfigValue& value, ConfigSource source);
    template<typename T>
    void set(const std::string& key, T value, ConfigSource source);
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
```

### Key Methods

#### Configuration Operations

```cpp
// Create configuration
auto config = std::make_shared<Config>();

// Set configuration values
config->set("brain.neuron_count", 1000);
config->set("brain.synapse_density", 0.1);
config->set("plasticity.stdp.enable", true);
config->set("neuromod.curiosity.enable", true);

// Get configuration values
if (config->has("brain.neuron_count")) {
    int64_t neuronCount = config->getOr<int64_t>("brain.neuron_count", 1000);
    std::cout << "Neuron count: " << neuronCount << std::endl;
}

// File operations
if (config->loadFromFile("config.cfg")) {
    std::cout << "Configuration loaded successfully!" << std::endl;
}

config->saveToFile("my_config.cfg");
```

#### Configuration Sources

```cpp
enum class ConfigSource {
    Default,      // Default configuration values
    File,         // Loaded from configuration file
    CommandLine,  // Override from command line
    Programmatic  // Set programmatically
};
```

## AgentBrain Class

The `AgentBrain` class bridges the gap between brain activity and environment interaction.

### Class Definition

```cpp
class AgentBrain {
public:
    // Construction and destruction
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Copy and move operations
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialization
    void initialize(const Environment& environment);
    
    // Sensory processing
    void processSensoryInput(const SensoryInput& input);
    
    // Motor command generation
    MotorCommand decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Development
    void updateDevelopment(TimestepDuration timestep);
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Feature enabling
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    void enablePrediction(bool enable);
    void enableAttention(bool enable);
    
    // Statistics
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    float getNeuromodulationLevel() const;
};
```

### Key Methods

```cpp
// Create and initialize agent
auto brain = std::make_shared<Brain>(config);
brain->initialize();

auto agent = std::make_shared<AgentBrain>(brain);
auto world = pynlm::createSimpleWorld();
world->configure(20, 20, 8, 8);
world->reset();

agent->initialize(*world);

// Enable learning subsystems
agent->enableRewardModulation(true);
agent->enableCuriosity(true);
agent->enableDevelopment(true);

// Main agent loop
for (step in range(1000)):
    world->update(0.1)
    agent->processSensoryInput(world->getSensoryPercept())
    brain->step(step)
    
    action = agent->decodeMotorCommand()
    world->applyMotorCommand(action, world->getSimulationTime())
    
    reward = world->getSensoryPercept().getInternal()[0]
    agent->applyRewardModulation(reward, 0.0)
    
    agent->updateDevelopment(0.1)
```

## Environment Classes

### SimpleWorld

```cpp
class SimpleWorld {
public:
    // Construction and destruction
    SimpleWorld();
    ~SimpleWorld();
    
    // Initialization
    void initialize(int width, int height);
    void reset();
    
    // Configuration
    void configure(int width, int height, int visionWidth, int visionHeight);
    void setAgentStart(float x, float y);
    void setRandomSeed(uint64_t seed);
    
    // World updates
    void update(TimestepDuration timestep);
    
    // Agent interaction
    void applyMotorCommand(const Action& action, Timestamp timestamp);
    std::unique_ptr<SensoryInput> getSensoryPercept() const;
    
    // State access
    const EnvironmentState& getState() const;
    float getLastReward() const;
    bool isDone() const;
    const char* getName() const;
    
    // World objects
    void addObject(const WorldObject& object);
    void removeObject(float x, float y);
    bool isValidPosition(float x, float y) const;
    
    // Properties
    int getWidth() const;
    int getHeight() const;
    size_t getActionSpaceSize() const;
    size_t getObservationSpaceSize() const;
    Timestamp getSimulationTime() const;
    float getMaxEnergy() const;
    void setMaxEnergy(float maxEnergy);
    
    // World simulation
    void applyMotorCommand(const Action& action, Timestamp timestamp);
    std::unique_ptr<SensoryInput> getSensoryPercept() const;
    float computeReward(const Brain* brain) const;
    const class AgentBody* getAgentBody() const;
};
```

## Enumerations

### NeuronType

```cpp
enum class NeuronType {
    Excitatory,    // Standard excitatory neuron
    Inhibitory,    // Standard inhibitory neuron
    Sensory,       // Sensory processing neuron
    Motor,         // Motor control neuron
    Modulatory,     // Neuromodulatory neuron
    Internal       // Internal processing neuron
};
```

### FiringState

```cpp
enum class FiringState {
    Resting,       // Not firing, below threshold
    Active,        // Above threshold but not yet fired
    Refractory,    // Just fired, in refractory period
    Inhibited,     // Actively inhibited
    Adapting       // Adapting firing rate
};
```

### DevelopmentalStage

```cpp
enum class DevelopmentalStage {
    Initial,       // Initial developmental stage
    CriticalPeriod, // Sensitive learning period
    Maturation,     // Specialization and refinement
    Adult,          // Stable, efficient operation
    Aging          // Decline and adaptation
};
```

### ActionType

```cpp
enum class ActionType {
    MoveForward,   // Move forward
    MoveBackward,  // Move backward
    MoveLeft,      // Move left
    MoveRight,     // Move right
    TurnLeft,      // Rotate left
    TurnRight,     // Rotate right
    Interact,      // Interact with environment
    Eat,           // Consume resources
    Wait,          // Do nothing
    Sleep,         // Rest and recover
    Explore,       // Explore new areas
    Return,        // Return to starting position
};
```

### LogLevel

```cpp
enum class LogLevel {
    Debug,         // Detailed debugging information
    Info,          // General information
    Warning,       // Warnings and potential issues
    Error,         // Errors that affect functionality
    Critical       // Critical errors that prevent operation
};
```

### WorldObjectType

```cpp
enum class WorldObjectType {
    Empty,         // Empty space
    Resource,      // Consumable resource
    Hazard,        // Dangerous object
    Wall,          // Impassable barrier
    Marker,        // Important location
};
```

## Type Definitions

### Core Types

```cpp
using NeuronId = uint64_t;
using RegionId = uint64_t;
using PopulationId = uint64_t;
using SynapseHandle = uint64_t;
using MembranePotential = float;      // mV
using TimestepDuration = double;      // seconds
using Timestamp = double;             // seconds
using SimulationStep = uint64_t;
using FiringRate = float;             // Hz
using Delay = uint32_t;               // ms
using ConfigValue = std::variant<
    int64_t, double, bool, std::string
>;
```

### Mathematical Types

```cpp
using Vector2D = std::pair<float, float>;
using Matrix3x3 = std::array<std::array<float, 3>, 3>;
using Color = std::tuple<uint8_t, uint8_t, uint8_t>;
```

## Constants

### Default Configuration Values

```cpp
// Brain parameters
constexpr int64_t DEFAULT_NEURON_COUNT = 1000;
constexpr int64_t DEFAULT_REGION_COUNT = 1;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
constexpr float DEFAULT_INITIAL_WEIGHT_MEAN = 0.5f;
constexpr float DEFAULT_INITIAL_WEIGHT_STD = 0.1f;

// Neuron parameters
constexpr float DEFAULT_V_THRESH = -50.0f;   // mV
constexpr float DEFAULT_V_REST = -70.0f;     // mV
constexpr float DEFAULT_V_RESET = -75.0f;    // mV
constexpr float DEFAULT_TAU_MEM = 20.0f;      // ms
constexpr float DEFAULT_TAU_REF = 2.0f;       // ms

// Plasticity parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
constexpr float DEFAULT_STDP_TAU = 20.0f;
constexpr float DEFAULT_HEBBIAN_RATE = 0.001f;

// Development parameters
constexpr float DEFAULT_SYNAPSIS_RATE = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
constexpr float DEFAULT_DEVELOPMENT_RATE = 1.0f;

// Simulation parameters
constexpr TimestepDuration DEFAULT_TIMESTEP = 0.001;  // 1ms
constexpr SimulationStep DEFAULT_MAX_STEPS = 1000;
```

## Error Handling

### Error Codes

```cpp
enum class ErrorCode {
    Success,                    // Operation completed successfully
    InvalidArgument,            // Invalid argument provided
    FileNotFound,               // File not found
    PermissionDenied,            // Permission denied
    InvalidConfiguration,       // Invalid configuration
    InitializationFailed,       // Failed to initialize component
    SimulationError,            // Error during simulation
    MemoryError,                // Memory allocation failed
    UnsupportedFeature,          // Feature not supported
    Timeout,                    // Operation timed out
    Unknown                    // Unknown error
};
```

### Error Classes

```cpp
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorCode code = ErrorCode::Unknown);
    ErrorCode getCode() const;
    
private:
    ErrorCode code_;
};

class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, ErrorCode code = ErrorCode::InvalidConfiguration);
};

class BrainException : public NLMException {
public:
    BrainException(const std::string& message, ErrorCode code = ErrorCode::InitializationFailed);
};
```

## Performance Considerations

### Memory Usage

- **Brain Memory**: ~1000 neurons × 100 bytes = 100 KB
- **Synapses**: ~2400 synapses × 50 bytes = 120 KB
- **Memory Systems**: Working memory + episodic memory + associative memory
- **Total**: ~1-2 MB for standard configuration

### Computational Complexity

- **Per Step**: O(N × M) where N = neurons, M = average connections
- **Plasticity**: O(S × T) where S = synapses, T = time steps
- **Memory Access**: O(1) for working memory, O(log N) for episodic memory

### Optimization Tips

1. **Reduce Network Size**: Smaller networks are faster but less capable
2. **Disable Expensive Features**: Turn off unnecessary plasticity and neuromodulation
3. **Use Sparse Connectivity**: Reduce number of synaptic connections
4. **Optimize World Size**: Smaller worlds require less computation
5. **Use Parallel Processing**: Leverage multi-core processors
6. **Cache Frequently Used Data**: Minimize memory access

## Example Usage Patterns

### Pattern 1: Simple Brain Simulation

```cpp
// Create and run a simple brain
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 100);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

for (size_t step = 0; step < 1000; ++step) {
    brain->step(step);
    
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": " 
                  << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
    }
}
```

### Pattern 2: Complete Agent Simulation

```cpp
// Create complete agent with all features
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("plasticity.stdp.enable", true);
config->set("neuromod.curiosity.enable", true);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

auto agent = std::make_shared<AgentBrain>(brain);
auto world = pynlm::createSimpleWorld();
world->configure(20, 20, 8, 8);
world->reset();

agent->initialize(*world);
agent->enableRewardModulation(true);
agent->enableCuriosity(true);
agent->enableDevelopment(true);

for (step in range(1000)):
    world->update(0.1)
    agent->processSensoryInput(world->getSensoryPercept())
    brain->step(step)
    
    action = agent->decodeMotorCommand()
    world->applyMotorCommand(action, world->getSimulationTime())
    
    reward = world->getSensoryPercept().getInternal()[0]
    agent->applyRewardModulation(reward, 0.0)
    
    agent->updateDevelopment(0.1)
```

### Pattern 3: Experiment with Different Configurations

```cpp
// Run experiments with different configurations
struct ExperimentConfig {
    std::string name;
    std::shared_ptr<Config> config;
    size_t numSteps;
};

std::vector<ExperimentConfig> experiments = {
    {"Small Network", createSmallConfig(), 1000},
    {"Large Network", createLargeConfig(), 500},
    {"No Plasticity", createNoPlasticityConfig(), 2000},
    {"Full Learning", createFullLearningConfig(), 300}
};

for (const auto& exp : experiments) {
    std::cout << "Running experiment: " << exp.name << std::endl;
    
    auto brain = std::make_shared<Brain>(exp.config);
    brain->initialize();
    
    // Run simulation...
    
    // Collect results...
}
```

## Migration Guide

### From Version 1.0 to 2.0

1. **Configuration Changes**: Configuration keys have changed, update accordingly
2. **API Changes**: New classes and methods added, old ones deprecated
3. **Error Handling**: Better error handling with exception hierarchy
4. **Memory Management**: Smart pointers used instead of raw pointers
5. **Performance**: Optimization for larger networks

### Common Migration Issues

1. **Initialization**: Always call `initialize()` before using brain
2. **Memory Management**: Use smart pointers, don't manually delete objects
3. **Configuration**: Use `Config` class for settings, not raw maps
4. **Error Handling**: Catch `NLMException` for error handling
5. **Thread Safety**: Most classes are thread-safe for concurrent access

## Future Enhancements

### Planned Features

1. **GPU Acceleration**: Hardware acceleration for large networks
2. **Distributed Computing**: Multi-machine simulation support
3. **Advanced Visualization**: 3D neural activity visualization
4. **Scripting Interface**: Python/JavaScript scripting support
5. **Web API**: REST API for remote interaction
6. **Cloud Integration**: Integration with cloud computing platforms

### Research Directions

1. **Brain Development**: Study how brains develop and learn
2. **Neuromodulation**: Investigate biological neuromodulation mechanisms
3. **Memory Systems**: Research memory consolidation and retrieval
4. **Learning Algorithms**: Develop new learning and plasticity rules
5. **Emergent Behaviors**: Study how complex behaviors emerge from simple rules

## Conclusion

This API reference provides comprehensive documentation for NLM's public interface. The framework is designed to be:

- **Flexible**: Support for different configurations and use cases
- **Extensible**: Easy to add new brain components and features
- **Performant**: Optimized for both speed and accuracy
- **Research-Oriented**: Designed for studying neural computation
- **User-Friendly**: Intuitive API with comprehensive documentation

The NLM framework enables researchers and developers to create, study, and experiment with artificial developmental brains that learn, adapt, and develop through interaction with environments.
