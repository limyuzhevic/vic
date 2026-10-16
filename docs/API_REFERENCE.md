# NLM API Reference

## Overview

This document provides comprehensive API documentation for the NLM (熙然) neural learning machine project. The API is organized into logical modules and follows a consistent C++ design pattern. This documentation includes detailed parameter descriptions, return value documentation, and usage examples for all public classes and functions.

## Namespace

All classes and functions are contained within the `nlm` namespace:
```cpp
namespace nlm {
    // All API declarations
}
```

## Core Type Definitions

### Fundamental Types

#### `Types.hpp` - Basic Data Types

```cpp
// Neuron identification
using NeuronId = uint32_t;
using RegionId = uint32_t;
using SynapseId = uint32_t;
using SimulationStep = uint64_t;
using Timestamp = double;
using MembranePotential = float;
using Conductance = float;
using Delay = uint32_t; // in time steps

// Numerical types
using Weight = float;
using Rate = float; // firing rate in Hz
using TimeConstant = float; // in milliseconds

// Boolean flags
using PlasticityFlags = uint32_t;
using NeuromodulatorFlags = uint32_t;

// Array indices
using Index = int;
using Size = size_t;
```

## Core Classes

### Config Class (`src/core/Config/Config.hpp`)

#### Constructor/Destructor
```cpp
/**
 * @brief Configuration management class
 * @details Handles loading, saving, and querying configuration values from various sources
 * @note Supports JSON, XML, and YAML configuration formats
 * @author NLM Development Team
 * @date 2026
 */
Config();
~Config();
```

#### Configuration Management
```cpp
/**
 * @brief Load configuration from file
 * @param filepath Path to configuration file (JSON, XML, or YAML)
 * @return true if configuration loaded successfully, false otherwise
 * @throws std::runtime_error if file cannot be read or parsed
 * @note Automatically detects file format based on extension
 * @see loadFromArgs
 */
bool loadFromFile(const std::string& filepath);

/**
 * @brief Load configuration from command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @return true if configuration loaded successfully, false otherwise
 * @note Parses arguments like --key value or --key=value
 * @see loadFromFile
 */
bool loadFromArgs(int argc, char** argv);

/**
 * @brief Save configuration to file
 * @param filepath Path to output configuration file
 * @return true if configuration saved successfully, false otherwise
 * @throws std::runtime_error if file cannot be written
 * @note Preserves source information for reload
 * @param filepath The filepath to save to
 * @return Success status
 */
bool saveToFile(const std::string& filepath) const;

/**
 * @brief Get configuration value with type safety
 * @tparam T Type of configuration value (bool, int, double, std::string, etc.)
 * @param key Configuration key (dot-separated path)
 * @return Optional containing value if key exists, empty optional otherwise
 * @note Performs automatic type conversion with error checking
 * @param key The configuration key to retrieve
 * @return The configuration value, or empty if not found
 */
template<typename T>
std::optional<T> get(const std::string& key) const;

/**
 * @brief Get configuration value with default
 * @tparam T Type of configuration value
 * @param key Configuration key
 * @param defaultValue Default value if key doesn't exist
 * @return Value from configuration or default
 * @note Convenient alternative to get() with fallback
 * @param key The configuration key
 * @param defaultValue The default value
 * @return Configuration value or default
 */
template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;

/**
 * @brief Set configuration value
 * @tparam T Type of value to set
 * @param key Configuration key (dot-separated path)
 * @param value Value to set
 * @param source Source of configuration (Runtime, File, CommandLine)
 * @note Updates configuration metadata and tracks source
 * @param key The configuration key
 * @param value The value to set
 * @param source The source of the configuration
 */
void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

/**
 * @brief Check if configuration key exists
 * @param key Configuration key to check
 * @return true if key exists, false otherwise
 * @note Does not validate type or value, only key existence
 * @param key The configuration key
 * @return Key existence status
 */
bool has(const std::string& key) const;

/**
 * @brief Remove configuration key
 * @param key Configuration key to remove
 * @note Removes key from all configuration levels
 * @param key The configuration key to remove
 */
void remove(const std::string& key);

/**
 * @brief Get all configuration keys
 * @return Vector of all configuration keys (dot-separated paths)
 * @note Includes keys from all configuration sources
 * @return Vector of configuration keys
 */
std::vector<std::string> getKeys() const;

/**
 * @brief Clear all configuration
 * @note Removes all configuration values and resets to empty state
 */
void clear();

/**
 * @brief Get configuration summary
 * @return String representation of configuration contents
 * @note Useful for debugging and logging
 * @return Configuration summary string
 */
std::string summary() const;
```

### Brain Class (`src/brain/Brain.hpp`)

#### Constructor/Destructor
```cpp
/**
 * @brief Central coordinator of the neural system
 * @details Implements real spiking neural computation with event-driven dynamics
 * @note Integrates all brain systems (memory, prediction, cognition, neuromodulation)
 * @param config Configuration for brain initialization
 * @author NLM Development Team
 * @date 2026
 */
explicit Brain(std::shared_ptr<Config> config);

/**
 * @brief Destructor
 * @note Ensures proper cleanup of all brain resources and subsystems
 */
~Brain();

/**
 * @brief Disable copying, enable moving
 * @note Enforces move semantics for brain objects to prevent resource conflicts
 */
Brain(const Brain&) = delete;
Brain& operator=(const Brain&) = delete;
Brain(Brain&&) noexcept;
Brain& operator=(Brain&&) noexcept;
```

#### Initialization and Control
```cpp
/**
 * @brief Initialize brain with configuration
 * @return true if initialization successful, false otherwise
 * @note Allocates neurons, regions, and initializes all subsystems
 * @return Initialization success status
 */
bool initialize();

/**
 * @brief Main simulation step
 * @param currentStep Current simulation step number
 * @note Advances neural dynamics by one time step
 * @param currentStep The simulation step number
 */
void step(SimulationStep currentStep);

/**
 * @brief Simulation time step with explicit time
 * @param currentStep Current simulation step number
 * @param currentTime Current simulation time in milliseconds
 * @note Provides more accurate time-based simulation
 * @param currentStep The simulation step number
 * @param currentTime The current simulation time
 */
void step(SimulationStep currentStep, Timestamp currentTime);
```

#### Sensory-Motor Interface
```cpp
/**
 * @brief Receive sensory input from environment
 * @param input Sensory input containing sensor readings and rewards
 * @note Injects current into sensory neurons based on input pattern
 * @param input The sensory input to process
 */
void receiveSensoryInput(const class SensoryInput& input);

/**
 * @brief Inject current directly into a specific neuron
 * @param neuron Neuron ID to inject current into
 * @param current Current value to inject (in nA)
 * @note Direct neural stimulation for testing and control
 * @param neuron The target neuron ID
 * @param current The current to inject
 */
void injectCurrent(NeuronId neuron, MembranePotential current);

/**
 * @brief Inject current into all neurons of a specific type
 * @param type Neuron type to target
 * @param current Current value to inject (in nA)
 * @note Efficient bulk neural stimulation
 * @param type The neuron type
 * @param current The current to inject
 */
void injectCurrentToNeurons(NeuronType type, MembranePotential current);

/**
 * @brief Get spike system pointer
 * @return Pointer to spike system (nullptr if not initialized)
 * @note Provides access to spike propagation and management
 * @return Spike system pointer
 */
SpikeSystem* getSpikeSystem();
const SpikeSystem* getSpikeSystem() const;

/**
 * @brief Get STDP plasticity system pointer
 * @return Pointer to STDP system (nullptr if not initialized)
 * @note Provides access to spike-timing-dependent plasticity
 * @return STDP system pointer
 */
STDP* getSTDP();

/**
 * @brief Get Hebbian plasticity system pointer
 * @return Pointer to Hebbian system (nullptr if not initialized)
 * @note Provides access to Hebbian synaptic strengthening
 * @return Hebbian system pointer
 */
Hebbian* getHebbian();

/**
 * @brief Get structural plasticity system pointer
 * @return Pointer to structural plasticity system (nullptr if not initialized)
 * @note Provides access to structural synaptic rewiring
 * @return Structural plasticity system pointer
 */
StructuralPlasticity* getStructuralPlasticity();

/**
 * @brief Get working memory system pointer
 * @return Pointer to working memory system (nullptr if not initialized)
 * @note Provides access to transient active information storage
 * @return Working memory system pointer
 */
NeuralWorkingMemory* getWorkingMemory();

/**
 * @brief Get episodic memory system pointer
 * @return Pointer to episodic memory system (nullptr if not initialized)
 * @note Provides access to experience encoding and replay
 * @return Episodic memory system pointer
 */
NeuralEpisodicMemory* getEpisodicMemory();

/**
 * @brief Get associative memory system pointer
 * @return Pointer to associative memory system (nullptr if not initialized)
 * @note Provides access to pattern association through Hebbian learning
 * @return Associative memory system pointer
 */
NeuralAssociativeMemory* getAssociativeMemory();

/**
 * @brief Get prediction system pointer
 * @return Pointer to prediction system (nullptr if not initialized)
 * @note Provides access to sensory prediction and error computation
 * @return Prediction system pointer
 */
PredictionSystem* getPredictionSystem();

/**
 * @brief Get neural planner pointer
 * @return Pointer to neural planner (nullptr if not initialized)
 * @note Provides access to action sequence evaluation and planning
 * @return Neural planner pointer
 */
NeuralPlanner* getPlanner();

/**
 * @brief Get concept formation pointer
 * @return Pointer to concept formation (nullptr if not initialized)
 * @note Provides access to pattern discovery and categorization
 * @return Concept formation pointer
 */
ConceptFormation* getConceptFormation();

/**
 * @brief Get attentional selection pointer
 * @return Pointer to attentional selection (nullptr if not initialized)
 * @note Provides access to competitive focus selection
 * @return Attentional selection pointer
 */
AttentionalSelection* getAttention();

/**
 * @brief Get development system pointer
 * @return Pointer to development system (nullptr if not initialized)
 * @note Provides access to age-dependent plasticity modulation
 * @return Development system pointer
 */
DevelopmentSystem* getDevelopmentSystem();

/**
 * @brief Get dopamine neuromodulator pointer
 * @return Pointer to dopamine system (nullptr if not initialized)
 * @note Provides access to reward prediction and reinforcement signaling
 * @return Dopamine pointer
 */
Dopamine* getDopamine();

/**
 * @brief Get curiosity neuromodulator pointer
 * @return Pointer to curiosity system (nullptr if not initialized)
 * @note Provides access to exploration motivation and novelty detection
 * @return Curiosity pointer
 */
Curiosity* getCuriosity();

/**
 * @brief Get novelty neuromodulator pointer
 * @return Pointer to novelty system (nullptr if not initialized)
 * @note Provides access to sensory novelty detection for exploration
 * @return Novelty pointer
 */
Novelty* getNovelty();

/**
 * @brief Get prediction error neuromodulator pointer
 * @return Pointer to prediction error system (nullptr if not initialized)
 * @note Provides access to prediction error signaling for learning
 * @return Prediction error pointer
 */
PredictionError* getPredictionErrorSignal();
```

#### Initialization and Control
```cpp
/**
 * @brief Initialize brain with configuration
 * @return true if initialization successful, false otherwise
 * @note Allocates neurons, regions, and initializes all subsystems
 * @return Initialization success status
 */
bool initialize();

/**
 * @brief Main simulation step
 * @param currentStep Current simulation step number
 * @note Advances neural dynamics by one time step
 * @param currentStep The simulation step number
 */
void step(SimulationStep currentStep);

/**
 * @brief Simulation time step with explicit time
 * @param currentStep Current simulation step number
 * @param currentTime Current simulation time in milliseconds
 * @note Provides more accurate time-based simulation
 * @param currentStep The simulation step number
 * @param currentTime The current simulation time
 */
void step(SimulationStep currentStep, Timestamp currentTime);
```

#### Sensory-Motor Interface
```cpp
/**
 * @brief Receive sensory input from environment
 * @param input Sensory input containing sensor readings and rewards
 * @note Injects current into sensory neurons based on input pattern
 * @param input The sensory input to process
 */
void receiveSensoryInput(const class SensoryInput& input);

/**
 * @brief Inject current directly into a specific neuron
 * @param neuron Neuron ID to inject current into
 * @param current Current value to inject (in nA)
 * @note Direct neural stimulation for testing and control
 * @param neuron The target neuron ID
 * @param current The current to inject
 */
void injectCurrent(NeuronId neuron, MembranePotential current);

/**
 * @brief Inject current into all neurons of a specific type
 * @param type Neuron type to target
 * @param current Current value to inject (in nA)
 * @note Efficient bulk neural stimulation
 * @param type The neuron type
 * @param current The current to inject
 */
void injectCurrentToNeurons(NeuronType type, MembranePotential current);

/**
 * @brief Get spike system pointer
 * @return Pointer to spike system (nullptr if not initialized)
 * @note Provides access to spike propagation and management
 * @return Spike system pointer
 */
SpikeSystem* getSpikeSystem();
const SpikeSystem* getSpikeSystem() const;

/**
 * @brief Get STDP plasticity system pointer
 * @return Pointer to STDP system (nullptr if not initialized)
 * @note Provides access to spike-timing-dependent plasticity
 * @return STDP system pointer
 */
STDP* getSTDP();

/**
 * @brief Get Hebbian plasticity system pointer
 * @return Pointer to Hebbian system (nullptr if not initialized)
 * @note Provides access to Hebbian synaptic strengthening
 * @return Hebbian system pointer
 */
Hebbian* getHebbian();

/**
 * @brief Get structural plasticity system pointer
 * @return Pointer to structural plasticity system (nullptr if not initialized)
 * @note Provides access to structural synaptic rewiring
 * @return Structural plasticity system pointer
 */
StructuralPlasticity* getStructuralPlasticity();

/**
 * @brief Get working memory system pointer
 * @return Pointer to working memory system (nullptr if not initialized)
 * @note Provides access to transient active information storage
 * @return Working memory system pointer
 */
NeuralWorkingMemory* getWorkingMemory();

/**
 * @brief Get episodic memory system pointer
 * @return Pointer to episodic memory system (nullptr if not initialized)
 * @note Provides access to experience encoding and replay
 * @return Episodic memory system pointer
 */
NeuralEpisodicMemory* getEpisodicMemory();

/**
 * @brief Get associative memory system pointer
 * @return Pointer to associative memory system (nullptr if not initialized)
 * @note Provides access to pattern association through Hebbian learning
 * @return Associative memory system pointer
 */
NeuralAssociativeMemory* getAssociativeMemory();

/**
 * @brief Get prediction system pointer
 * @return Pointer to prediction system (nullptr if not initialized)
 * @note Provides access to sensory prediction and error computation
 * @return Prediction system pointer
 */
PredictionSystem* getPredictionSystem();

/**
 * @brief Get neural planner pointer
 * @return Pointer to neural planner (nullptr if not initialized)
 * @note Provides access to action sequence evaluation and planning
 * @return Neural planner pointer
 */
NeuralPlanner* getPlanner();

/**
 * @brief Get concept formation pointer
 * @return Pointer to concept formation (nullptr if not initialized)
 * @note Provides access to pattern discovery and categorization
 * @return Concept formation pointer
 */
ConceptFormation* getConceptFormation();

/**
 * @brief Get attentional selection pointer
 * @return Pointer to attentional selection (nullptr if not initialized)
 * @note Provides access to competitive focus selection
 * @return Attentional selection pointer
 */
AttentionalSelection* getAttention();

/**
 * @brief Get development system pointer
 * @return Pointer to development system (nullptr if not initialized)
 * @note Provides access to age-dependent plasticity modulation
 * @return Development system pointer
 */
DevelopmentSystem* getDevelopmentSystem();

/**
 * @brief Get dopamine neuromodulator pointer
 * @return Pointer to dopamine system (nullptr if not initialized)
 * @note Provides access to reward prediction and reinforcement signaling
 * @return Dopamine pointer
 */
Dopamine* getDopamine();

/**
 * @brief Get curiosity neuromodulator pointer
 * @return Pointer to curiosity system (nullptr if not initialized)
 * @note Provides access to exploration motivation and novelty detection
 * @return Curiosity pointer
 */
Curiosity* getCuriosity();

/**
 * @brief Get novelty neuromodulator pointer
 * @return Pointer to novelty system (nullptr if not initialized)
 * @note Provides access to sensory novelty detection for exploration
 * @return Novelty pointer
 */
Novelty* getNovelty();

/**
 * @brief Get prediction error neuromodulator pointer
 * @return Pointer to prediction error system (nullptr if not initialized)
 * @note Provides access to prediction error signaling for learning
 * @return Prediction error pointer
 */
PredictionError* getPredictionErrorSignal();
```

### Brain Class (`src/brain/Brain.hpp`)

#### Core Brain Functionality
```cpp
// Constructor
explicit Brain(std::shared_ptr<Config> config);

// Movement semantics
Brain(const Brain&) = delete;
Brain& operator=(const Brain&) = delete;
Brain(Brain&&) noexcept;
Brain& operator=(Brain&&) noexcept;
```

#### Initialization and Control
```cpp
// Initialize brain with configuration
bool initialize();

// Main simulation step
void step(SimulationStep currentStep);
void step(SimulationStep currentStep, Timestamp currentTime);
```

#### Sensory-Motor Interface
```cpp
// Sensory input processing
void receiveSensoryInput(const SensoryInput& input);
void injectCurrent(NeuronId neuron, MembranePotential current);
void injectCurrentToNeurons(NeuronType type, MembranePotential current);

// Motor output generation
std::unique_ptr<Action> produceAction();
```

#### System Access
```cpp
// Neural dynamics access
SpikeSystem* getSpikeSystem();
const SpikeSystem* getSpikeSystem() const;

// Plasticity systems access
STDP* getSTDP();
Hebbian* getHebbian();
StructuralPlasticity* getStructuralPlasticity();

// Memory systems access
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();
```

#### Statistics and Monitoring
```cpp
// Neural statistics
float getExcitationInhibitionRatio() const;
size_t getTotalSpikeCount() const;
size_t getPendingSpikeEventCount() const;

// System statistics
size_t getTotalNeuronCount() const;
size_t getTotalSynapseCount() const;
size_t getActiveNeuronCount() const;
size_t getFiringNeuronCount() const;
float getAverageFiringRate() const;
```

#### Development and Plasticity
```cpp
// Development control
void develop();
DevelopmentalStage getDevelopmentalStage() const;
void setDevelopmentalStage(DevelopmentalStage stage);

// Plasticity control
void updatePlasticity();

// State management
void reset();
bool save(const std::string& filepath) const;
bool load(const std::string& filepath);
```

#### Region Management
```cpp
// Region operations
RegionId addRegion(const std::string& name = "");
NeuralRegion* getRegion(RegionId id);
const NeuralRegion* getRegion(RegionId id) const;
size_t getRegionCount() const;
std::vector<RegionId> getRegionIds() const;
```

#### Inter-System Connections
```cpp
// Prediction system
PredictionSystem* getPredictionSystem();

// Cognition systems
NeuralPlanner* getPlanner();
ConceptFormation* getConceptFormation();
AttentionalSelection* getAttention();

// Development system
DevelopmentSystem* getDevelopmentSystem();

// Neuromodulation systems
Dopamine* getDopamine();
Curiosity* getCuriosity();
Novelty* getNovelty();
PredictionError* getPredictionErrorSignal();
```

### AgentBrain Class (`src/agent/AgentBrain.hpp`)

#### Agent-World Interface
```cpp
// Constructor
AgentBrain(std::shared_ptr<Brain> brain);

// Initialization
void initialize(const SimpleWorld& world);
```

#### Sensory Processing
```cpp
// Input processing
size_t getSensoryInputSize() const;  // Get expected input size
size_t getMotorOutputSize() const;   // Get expected output size
void processSensoryInput(const SensoryPercept& percept);
```

#### Motor Output
```cpp
// Output generation
MotorCommand decodeMotorCommand();
```

#### Neuromodulation
```cpp
// Reward-based modulation
void applyRewardModulation(float reward, float predictedReward);
```

#### State Monitoring
```cpp
// Neuromodulation levels
float getNeuromodulationLevel() const;
float getCuriosityLevel() const;
float getNoveltyLevel() const;
float getPredictionError() const;

// Development state
double getDevelopmentalAge() const;
DevelopmentalStage getDevelopmentalStage() const;
```

### Neuron Class (`src/brain/Neuron.hpp`)

#### Basic Neuron Operations
```cpp
// Neuron lifecycle
Neuron(NeuronId id, NeuronType type);
~Neuron();

// State management
void reset();
void injectCurrent(MembranePotential current);
void update(double timestep);
```

#### Neuron Properties
```cpp
// State query
bool isActive() const;
MembranePotential getMembranePotential() const;
float getFiringRate() const;
NeuronId getId() const;
NeuronType getType() const;
```

### Synapse Class (`src/brain/Synapse.hpp`)

#### Synapse Operations
```cpp
// Synapse lifecycle
Synapse(NeuronId preId, NeuronId postId, Weight weight);
~Synapse();

// Transmission
void transmit();  // Deliver spike to postsynaptic neuron
Weight getWeight() const;
void setWeight(Weight weight);
```

## System Component Classes

### Plasticity Systems

#### STDP (`src/plasticity/STDP.hpp`)
```cpp
class STDP {
public:
    STDP(double learningRate = 0.01, double timeWindow = 20.0);
    ~STDP();
    
    void applySTDP(NeuronId preId, NeuronId postId, Timestamp preSpikeTime, Timestamp postSpikeTime);
    void update(double timestep);
    void reset();
};
```

#### Hebbian (`src/plasticity/Hebbian.hpp`)
```cpp
c lass Hebbian {
public:
    Hebbian(double learningRate = 0.001);
    ~Hebbian();
    
    void strengthen(NeuronId preId, NeuronId postId, double correlation);
    void weaken(NeuronId preId, NeuronId postId, double antiCorrelation);
    void update(double timestep);
};
```

#### StructuralPlasticity (`src/plasticity/StructuralPlasticity.hpp`)
```cpp
class StructuralPlasticity {
public:
    StructuralPlasticity(double formationRate = 0.001, double eliminationRate = 0.0001);
    ~StructuralPlasticity();
    
    void formNewConnection(NeuronId preId, NeuronId postId);
    void eliminateConnection(SynapseId synapseId);
    void update(double timestep);
};
```

### Memory Systems

#### NeuralWorkingMemory (`src/memory/WorkingMemory.hpp`)
```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory(size_t capacity = 1000);
    ~NeuralWorkingMemory();
    
    void store(const std::vector<float>& pattern);
    std::vector<float> retrieve() const;
    void clear();
    bool isFull() const;
    size_t getActiveCount() const;
};
```

#### NeuralEpisodicMemory (`src/memory/EpisodicMemory.hpp`)
```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory(size_t maxEpisodes = 1000);
    ~NeuralEpisodicMemory();
    
    void encode(const std::vector<float>& experience, Timestamp time);
    std::vector<float> replay(Timestamp startTime, Timestamp endTime);
    void consolidate();
    void clearExpired(Timestamp cutoffTime);
};
```

#### NeuralAssociativeMemory (`src/memory/AssociativeMemory.hpp`)
```cpp
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory(double similarityThreshold = 0.8);
    ~NeuralAssociativeMemory();
    
    void associate(const std::vector<float>& pattern1, const std::vector<float>& pattern2);
    std::vector<float> retrieve(const std::vector<float>& pattern);
    void forget(const std::vector<float>& pattern);
    std::vector<std::pair<std::vector<float>, std::vector<float>>> getAllAssociations() const;
};
```

### Neuromodulation Systems

#### Dopamine (`src/neuromodulation/Dopamine.hpp`)
```cpp
class Dopamine {
public:
    Dopamine(double baselineLevel = 0.1, double gain = 10.0);
    ~Dopamine();
    
    void processReward(float reward, float predictionError);
    float getLevel() const;
    void setLevel(float level);
};
```

#### Curiosity (`src/neuromodulation/Curiosity.hpp`)
```cpp
class Curiosity {
public:
    Curiosity(double baselineLevel = 0.1, double noveltyGain = 5.0);
    ~Curiosity();
    
    void processNovelty(float novelty, float uncertainty);
    float getLevel() const;
    void setLevel(float level);
};
```

#### Novelty (`src/neuromodulation/Novelty.hpp`)
```cpp
class Novelty {
public:
    Novelty(double decayRate = 0.1);
    ~Novelty();
    
    void processSensoryInput(const std::vector<float>& input);
    float getLevel() const;
    void reset();
};
```

#### PredictionError (`src/neuromodulation/PredictionError.hpp`)
```cpp
class PredictionError {
public:
    PredictionError(double baselineError = 0.1);
    ~PredictionError();
    
    void update(float prediction, float actual);
    float getLevel() const;
};
```

### Development System

#### DevelopmentSystem (`src/development/DevelopmentSystem.hpp`)
```cpp
class DevelopmentSystem {
public:
    DevelopmentSystem(double plasticityDecayRate = 0.01);
    ~DevelopmentSystem();
    
    void update(double timestep);
    double getPlasticityMultiplier() const;
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
};
```

### Environment Interface

#### Environment (`src/environment/Environment.hpp`)
```cpp
class Environment {
public:
    virtual ~Environment() = default;
    
    virtual std::vector<float> getSensoryInput() const = 0;
    virtual void applyAction(const MotorCommand& action) = 0;
    virtual float getReward() const = 0;
    virtual bool isTerminal() const = 0;
    virtual void reset() = 0;
};
```

#### Observation (`src/environment/Observation.hpp`)
```cpp
class Observation {
public:
    Observation(const std::vector<float>& sensoryData, float reward);
    ~Observation();
    
    const std::vector<float>& getSensoryData() const;
    float getReward() const;
    Timestamp getTime() const;
};
```

### Experiment Framework

#### Experiment (`src/experiments/Experiment.hpp`)
```cpp
class Experiment {
public:
    Experiment(std::string name, std::shared_ptr<Brain> brain);
    ~Experiment();
    
    void run();
    void stop();
    bool isRunning() const;
    double getProgress() const;
    std::string getResults() const;
};
```

## Integration Points

### Brain Configuration
```cpp
// Create and configure brain
auto config = std::make_shared<Config>();
config->set("brain.maxNeurons", 1000);
config->set("brain.learningRate", 0.01);
config->set("brain.development.enabled", true);

auto brain = std::make_shared<Brain>(config);
```

### Complete Brain Loop
```cpp
// Initialize agent and brain
auto brain = std::make_shared<Brain>(config);
auto agent = std::make_shared<AgentBrain>(brain);

// Main simulation loop
while (!agent->isEpisodeComplete()) {
    // Get sensory input
    auto percept = agent->getSensoryInput(world);
    
    // Process in brain
    brain->receiveSensoryInput(percept);
    brain->step(currentStep);
    
    // Get motor output
    auto action = agent->decodeMotorCommand();
    
    // Execute in world
    world->applyAction(action);
    
    // Apply reward-based neuromodulation
    agent->applyRewardModulation(reward, predictedReward);
}
```

## Error Handling

### Error Types
- **ConfigurationError**: Invalid configuration values
- **MemoryError**: Memory allocation or access errors
- **PlasticityError**: Plasticity operation failures
- **NetworkError**: Network communication errors
- **SimulationError**: Simulation step failures

### Error Recovery
```cpp
// Use safe initialization
if (!brain->initialize()) {
    std::cerr << "Failed to initialize brain" << std::endl;
    return 1;
}

// Use checkpointing for long simulations
if (!brain->save("checkpoint.brain")) {
    std::cerr << "Failed to save checkpoint" << std::endl;
}
```

## Performance Considerations

### Memory Usage
- **Brain Memory**: ~10MB per 1000 neurons
- **Memory Systems**: ~1MB per active pattern
- **Plasticity Systems**: Minimal overhead

### Computational Complexity
- **Spike Propagation**: O(N_connections) per step
- **Plasticity Updates**: O(N_neurons) per step
- **Memory Operations**: O(N_patterns) for episodic memory

### Optimization Techniques
- Use SIMD for neuron simulations
- Implement memory pools for efficient allocation
- Use event-driven updates for spike propagation

## Thread Safety

### Thread-Safe Operations
- Configuration access (read-only)
- Read-only memory system access
- Statistics queries

### Thread-Unsafe Operations
- Configuration modification
- Brain simulation steps
- Memory system modifications

### Thread Safety Recommendations
```cpp
// For multi-threaded simulation
// Use separate brain instances for each thread
// Share configuration read-only
// Use thread-safe memory systems
```

## Future API Changes

### Versioning
- **Version 1.0**: Initial release (Phase 5 features)
- **Version 1.1**: Phase 6 integration features
- **Version 2.0**: Major architectural changes

### Backward Compatibility
The API maintains backward compatibility through:
- Stable class interfaces
- Virtual functions for extensibility
- Configuration-based feature control

## Further Reading

### Related Documentation
- **BEST_PRACTICES.md**: Usage best practices
- **PERFORMANCE_TUNING.md**: Performance optimization guide
- **TROUBLESHOOTING.md**: Common issues and solutions
- **TUTORIALS.md**: Step-by-step tutorials

### Research Papers
- "Computational Models of Developmental Learning"
- "Neuromodulation in Artificial Neural Systems"
- "Memory Integration in Brain-Like Architectures"

## Conclusion

The NLM API provides a comprehensive interface for implementing brain-like systems with developmental learning, integrated memory systems, and neuromodulation. The API design emphasizes computational efficiency, biological plausibility, and extensibility for research applications.