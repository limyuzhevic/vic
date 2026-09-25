# NLM API Reference Documentation

## Overview

The NLM (Neural Learning Machine) API provides comprehensive access to NLM functionality through both C++ and Python bindings. This documentation covers all available classes, functions, methods, and data structures.

## API Architecture

### C++ API
The primary API is implemented in C++ and provides direct access to all NLM functionality.

### Python API
The Python API is built using pybind11 and provides Python bindings for easy integration with Python-based workflows.

### Python Bindings
The Python API is accessible via the `pynlm` module and includes all core NLM functionality.

## Module Structure

### Core Modules
1. **Config** - Configuration management
2. **Brain** - Neural network simulation
3. **Agent** - Agent-brain interface
4. **World** - Environment simulation
5. **Memory** - Memory systems
6. **Neuromodulation** - Neuromodulator systems
7. **Plasticity** - Learning mechanisms
8. **Sensory** - Sensory processing
9. **Motor** - Motor control
10. **Development** - Developmental systems
11. **Prediction** - Prediction systems
12. **Cognition** - Cognitive mechanisms
13. **Environment** - Environment interface

### Testing Framework
1. **TestingFramework** - Enhanced testing infrastructure
2. **TestCase** - Individual test cases
3. **TestSuite** - Test suites
4. **Benchmarks** - Performance benchmarks

## C++ API Reference

### Namespace: nlm

All classes, functions, and constants are defined within the `nlm` namespace.

## Class Reference

### Config

#### Class: Config
The Config class provides comprehensive configuration management for NLM.

**Header:** `#include "core/Config/Config.hpp"`

**Methods:**

```cpp
// Construction and destruction
Config();                          // Default constructor
~Config();                         // Destructor
Config(Config&&) noexcept;         // Move constructor
Config& operator=(Config&&) noexcept; // Move assignment

// File I/O
bool loadFromFile(const std::string& filepath, bool validate = true);     // Load from file
bool loadFromJSON(const std::string& jsonStr, bool validate = true);       // Load from JSON
bool loadFromYAML(const std::string& yamlStr, bool validate = true);       // Load from YAML
bool saveToFile(const std::string& filepath, const std::string& format = "json") const; // Save to file

// Command line loading
bool loadFromArgs(int argc, char** argv); // Load from command line arguments

// Value access (templated)
template<typename T>
std::optional<T> get(const std::string& key) const;                       // Get value with type safety
template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;           // Get value with default

// Value setting
void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

// Configuration operations
bool has(const std::string& key) const;                                   // Check if key exists
void remove(const std::string& key);                                      // Remove key
std::vector<std::string> getKeys() const;                                 // Get all keys
void clear();                                                             // Clear all entries

// Configuration metadata
std::string summary() const;                                             // Get configuration summary
bool validateSchema(const std::vector<ConfigSchema>& schema) const;       // Validate against schema
bool validateType(const std::string& key, const std::string& expectedType) const; // Validate type

// Error handling
static ConfigError parseError(ConfigError::ErrorCode code, const std::string& message); // Create parse error

// Experiment support
bool saveExperimentConfig(const std::string& filepath, const std::string& experimentName) const;
bool loadExperimentConfig(const std::string& filepath, const std::string& experimentName);

// Configuration diffing
std::vector<ConfigDiffEntry> diff(const Config& other) const;              // Compare configurations
static std::string diffToString(const std::vector<ConfigDiffEntry>& diff); // Format diff as string

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t uniqueSources;
    std::map<ConfigSource, size_t> sourceCounts;
    std::map<std::string, size_t> typeCounts;
    double avgEntriesPerSource;
};
ConfigStats getStats() const;                                             // Get configuration statistics

// Internal helpers (private)
static std::string trim(const std::string& str);
static std::string toLower(const std::string& str);
static std::string toJSON(const ConfigValue& value);
static std::string toYAML(const ConfigValue& value);
static ConfigValue fromJSON(const nlohmann::json& j);
static ConfigValue fromYAML(const std::string& yamlStr);
static bool isJSON(const std::string& content);
static bool isYAML(const std::string& content);
static bool handleParseError(const std::string& error, ConfigError::ErrorCode code = ConfigError::ErrorCode::ParseError);
```

#### Enumerations

##### ConfigSource
```cpp
enum class ConfigSource {
    Default,      // Default configuration
    File,         // File-based configuration
    CommandLine,  // Command-line configuration
    Runtime       // Runtime configuration
};
```

##### ConfigError::ErrorCode
```cpp
enum class ErrorCode {
    ParseError,      // Parse error
    ValidationError, // Validation error
    FileError,       // File error
    SchemaError,     // Schema error
    TypeError        // Type error
};
```

#### Structs and Types

##### ConfigSchema
```cpp
struct ConfigSchema {
    std::string key;
    std::string type;
    bool required;
    std::string description;
    std::map<std::string, std::string> constraints;
    
    ConfigSchema(const std::string& k, const std::string& t, bool req = false,
                 const std::string& desc = "",
                 const std::map<std::string, std::string>& cons = {});
};
```

##### ConfigDiffEntry
```cpp
struct ConfigDiffEntry {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::string description;
    double diffScore;
};
```

##### ConfigStats
```cpp
struct ConfigStats {
    size_t totalEntries;
    size_t uniqueSources;
    std::map<ConfigSource, size_t> sourceCounts;
    std::map<std::string, size_t> typeCounts;
    double avgEntriesPerSource;
};
```

### Brain

#### Class: Brain
The Brain class represents a neural network simulation.

**Header:** `#include "brain/Brain.hpp"`

**Methods:**

```cpp
// Construction and destruction
Brain(std::shared_ptr<Config> config); // Constructor with config
~Brain();                           // Destructor

// Initialization and cleanup
void initialize();                  // Initialize brain
void reset();                       // Reset brain state

// Simulation
void step(SimulationStep step);     // Run one simulation step
void step(SimulationStep step, double time); // Run step with timestamp
void step(SimulationStep step, double time, bool isReset = false); // Run step with reset option

// Input/output
void receiveSensoryInput(const SensoryInput& input); // Receive sensory input
action produceAction();             // Produce motor action

// Statistics
SimulationStatistics getStatistics() const; // Get simulation statistics

// Neuron and synapse access
NeuronState getNeuronState(NeuronIndex index) const; // Get neuron state
SynapseRecord getSynapseRecord(SynapseIndex index) const; // Get synapse record

// Configuration access
std::shared_ptr<Config> getConfig() const; // Get configuration

// Memory access
void setWorkingMemory(std::shared_ptr<WorkingMemory> memory);
void setEpisodicMemory(std::shared_ptr<EpisodicMemory> memory);
void setAssociativeMemory(std::shared_ptr<AssociativeMemory> memory);

// Plasticity access
void setPlasticityRules(const std::vector<std::shared_ptr<PlasticityRule>>& rules);

// Neuromodulation access
void setNeuromodulators(const std::map<NeuromodulatorType, std::shared_ptr<Neuromodulator>>& neuromodulators);

// Development access
void setDevelopment(std::shared_ptr<DevelopmentSystem> development);

// Checkpoint access
void save(const std::string& filepath) const; // Save brain state
void load(const std::string& filepath);       // Load brain state

// Statistics methods
int getTotalNeuronCount() const;       // Total neurons
int getTotalSynapseCount() const;      // Total synapses
int getFiringNeuronCount() const;      // Currently firing neurons
int getTotalSpikeCount() const;        // Total spikes
int getAverageFiringRate() const;      // Average firing rate
int getExcitationInhibitionRatio() const; // E/I balance
int getNeuronCountInRegion(int regionId) const; // Neurons in region
int getSynapseCountForNeuron(int neuronId) const; // Synapses for neuron

// Sensory access
void injectCurrentToNeurons(NeuronType type, double strength); // Inject current

// Development access
int getDevelopmentalStage() const;     // Current developmental stage
void updateDevelopment(double timeStep); // Update development
```

#### Class: Neuron
The Neuron class represents individual neurons in the brain.

**Header:** `#include "brain/Neuron.hpp"`

**Methods:**

```cpp
// Construction and destruction
Neuron();                           // Default constructor
~Neuron();                          // Destructor

// State management
void initialize(const NeuronConfig& config); // Initialize neuron
void update(double timeStep);        // Update neuron state

// State access
NeuronState getState() const;        // Get neuron state
NeuronConfig getConfig() const;      // Get neuron configuration

// Input/output
void receiveInput(double input);      // Receive input
void produceOutput();                // Produce output

// Plasticity
void applyPlasticity(double dt);      // Apply plasticity
void updateSynapticWeights();         // Update synaptic weights

// Statistics
int getSpikeCount() const;           // Get spike count
int getFiringCount() const;          // Get firing count
int getAverageFiringRate() const;    // Get average firing rate
```

#### Class: Synapse
The Synapse class represents connections between neurons.

**Header:** `#include "brain/Synapse.hpp"`

**Methods:**

```cpp
// Construction and destruction
Synapse();                          // Default constructor
~Synapse();                         // Destructor

// State management
void initialize(const SynapseConfig& config); // Initialize synapse
void update(double timeStep);        // Update synapse state

// State access
SynapseState getState() const;       // Get synapse state
SynapseConfig getConfig() const;     // Get synapse configuration

// Input/output
void receivePreSynapticSignal();     // Receive pre-synaptic signal
void producePostSynapticSignal();    // Produce post-synaptic signal

// Plasticity
void applySTDP(double preTime, double postTime); // Apply STDP
void applyHebbianLearning();         // Apply Hebbian learning
void applyStructuralPlasticity();    // Apply structural plasticity

// Statistics
int getWeightCount() const;          // Get weight count
int getConnectionStrength() const;   // Get connection strength
int getDelay() const;                // Get synaptic delay
```

### Agent

#### Class: AgentBrain
The AgentBrain class provides the interface between the brain and the world.

**Header:** `#include "agent/AgentBrain.hpp"`

**Methods:**

```cpp
// Construction and destruction
AgentBrain(std::shared_ptr<Brain> brain); // Constructor
~AgentBrain();                          // Destructor

// Initialization and cleanup
void initialize(std::shared_ptr<World> world); // Initialize agent
void cleanup();                         // Cleanup agent

// Sensory processing
void processSensoryInput(const SensoryInput& input); // Process sensory input

// Motor decoding
Action decodeMotorCommand();           // Decode motor command

// Neuromodulation
void applyRewardModulation(double reward, double predictedReward); // Apply reward modulation
void applyPredictionError(double error); // Apply prediction error
void applyNovelty(double novelty);      // Apply novelty signal

// Development
void updateDevelopment(double timeStep); // Update development
DevelopmentalStage getDevelopmentalStage() const; // Get developmental stage

// Subsystem control
void enableRewardModulation(bool enabled); // Enable reward modulation
void enableStructuralPlasticity(bool enabled); // Enable structural plasticity
void enableDevelopment(bool enabled);   // Enable development
void enableCuriosity(bool enabled);    // Enable curiosity
void enableNoveltyDetection(bool enabled); // Enable novelty detection

// Statistics
int getCuriosityLevel() const;         // Get curiosity level
int getNoveltyLevel() const;           // Get novelty level
int getPredictionError() const;        // Get prediction error
int getNeuromodulationLevel() const;   // Get neuromodulation level

// Memory access
void recordEpisodicMemory(const Episode& episode); // Record episodic memory
Episode recallEpisodicMemory(const std::string& query); // Recall episodic memory

// Working memory access
void updateWorkingMemory(const std::string& key, const std::string& value); // Update working memory
std::string queryWorkingMemory(const std::string& key) const; // Query working memory

// Action planning
void planAction(const std::vector<Action>& options, const std::vector<double>& values); // Plan action
Action getPlannedAction() const;      // Get planned action

// Cognitive access
void formConcepts(const std::vector<std::string>& patterns); // Form concepts
std::string queryConcepts(const std::string& query) const; // Query concepts

// Attention access
void allocateAttention(const std::vector<std::string>& items); // Allocate attention
std::string selectAttentionTarget() const; // Select attention target
```

### World

#### Class: SimpleWorld
The SimpleWorld class provides basic environment simulation.

**Header:** `#include "environment/SimpleWorld.hpp"`

**Methods:**

```cpp
// Construction and destruction
SimpleWorld();                       // Default constructor
~SimpleWorld();                      // Destructor

// Configuration
void configure(int width, int height, int visionWidth, int visionHeight); // Configure world
void configure(const WorldConfig& config); // Configure from config

// State management
void reset();                         // Reset world to initial state
void update(double timeStep);         // Update world state

// Agent control
setAgentStart(double x, double y);    // Set agent start position
void applyMotorCommand(const Action& action, double timeStep); // Apply motor command

// Sensory input
SensoryInput getSensoryPercept() const; // Get sensory perception
WorldBody getAgentBody() const;       // Get agent body

// World objects
void addObject(const WorldObject& object); // Add world object
void removeObject(double x, double y); // Remove world object
WorldObject getObject(double x, double y) const; // Get world object
bool isValidPosition(double x, double y) const; // Check if position is valid

// World properties
int getWidth() const;                 // Get world width
int getHeight() const;                // Get world height
int getVisionWidth() const;           // Get vision width
int getVisionHeight() const;          // Get vision height
int getMaxEnergy() const;             // Get max energy
void setMaxEnergy(int energy);         // Set max energy
int getSimulationTime() const;        // Get simulation time

// Simulation control
void setSimulationTime(int time);     // Set simulation time
void advanceTime(double timeStep);    // Advance simulation time

// Randomization
void setRandomSeed(int seed);         // Set random seed

// Statistics
int getTotalObjects() const;          // Get total objects
int getAgentEnergy() const;           // Get agent energy
int getEnvironmentComplexity() const; // Get environment complexity
```

### Memory Systems

#### Class: WorkingMemory
The WorkingMemory class provides working memory functionality.

**Header:** `#include "memory/NeuralWorkingMemory.hpp"`

**Methods:**

```cpp
// Construction and destruction
WorkingMemory();                      // Default constructor
~WorkingMemory();                     // Destructor

// Memory management
void initialize(int capacity);        // Initialize working memory
void update(double timeStep);         // Update working memory

// Memory operations
void store(const std::string& key, const std::string& value); // Store in working memory
std::string retrieve(const std::string& key) const; // Retrieve from working memory
void erase(const std::string& key);    // Erase from working memory
bool contains(const std::string& key) const; // Check if key exists

// Memory competition
void activatePattern(const std::vector<std::string>& pattern); // Activate pattern
std::string competePattern(const std::vector<std::string>& pattern) const; // Compete with pattern

// Memory statistics
int getCapacity() const;               // Get memory capacity
int getUsage() const;                 // Get memory usage
int getActivationCount(const std::string& key) const; // Get activation count
```

#### Class: EpisodicMemory
The EpisodicMemory class provides episodic memory functionality.

**Header:** `#include "memory/NeuralEpisodicMemory.hpp"`

**Methods:**

```cpp
// Construction and destruction
EpisodicMemory();                     // Default constructor
~EpisodicMemory();                    // Destructor

// Memory management
void initialize(int capacity);        // Initialize episodic memory
void update(double timeStep);         // Update episodic memory

// Memory recording
void record(const Episode& episode);   // Record episode
Episode recall(double timeStep);       // Recall episode at time

// Memory replay
void replay();                         // Replay memories
void consolidate();                    // Consolidate memories

// Memory queries
std::vector<Episode> query(const std::string& pattern) const; // Query by pattern
std::vector<Episode> queryTimeRange(double startTime, double endTime) const; // Query by time range

// Memory statistics
int getCapacity() const;               // Get memory capacity
int getUsage() const;                 // Get memory usage
int getReplayCount() const;            // Get replay count
```

### Neuromodulation

#### Class: Neuromodulator
The Neuromodulator class provides base neuromodulator functionality.

**Header:** `#include "neuromodulation/Neuromodulator.hpp"`

**Methods:**

```cpp
// Construction and destruction
Neuromodulator();                     // Default constructor
~Neuromodulator();                    // Destructor

// State management
void initialize(double baseline);     // Initialize neuromodulator
void update(double timeStep, double input); // Update neuromodulator

// State access
double getLevel() const;               // Get neuromodulator level
double getBaseline() const;            // Get baseline level

// Control
void setLevel(double level);            // Set neuromodulator level
void setBaseline(double baseline);      // Set baseline level

// Statistics
int getUpdateCount() const;            // Get update count
int getActivationCount() const;        // Get activation count
```

#### Class: Reward
The Reward class provides reward neuromodulator functionality.

**Header:** `#include "neuromodulation/Reward.hpp"`

**Methods:**

```cpp
// Construction and destruction
Reward();                            // Default constructor
~Reward();                           // Destructor

// State management
void initialize(double baseline);     // Initialize reward
void update(double timeStep, double reward); // Update reward

// Reward processing
void applyReward(double reward);       // Apply reward
void applyPredictionError(double error); // Apply prediction error

// State access
double getLevel() const;               // Get reward level
double getExpectedReward() const;       // Get expected reward

// Control
void setBaseline(double baseline);      // Set baseline
void setPrediction(double prediction); // Set prediction

// Statistics
int getRewardCount() const;            // Get reward count
int getPredictionCount() const;       // Get prediction count
```

### Plasticity

#### Class: PlasticityRule
The PlasticityRule class provides base plasticity rule functionality.

**Header:** `#include "plasticity/PlasticityRule.hpp"`

**Methods:**

```cpp
// Construction and destruction
PlasticityRule();                      // Default constructor
~PlasticityRule();                     // Destructor

// State management
void initialize(const PlasticityConfig& config); // Initialize rule
void update(double timeStep);          // Update rule

// Plasticity application
void apply(double preSpike, double postSpike); // Apply plasticity

// State access
double getWeight() const;               // Get synaptic weight
int getTrace() const;                   // Get synaptic trace

// Control
void setWeight(double weight);          // Set synaptic weight
void setTrace(double trace);            // Set synaptic trace
```

#### Class: STDP
The STDP class provides Spike-Timing-Dependent Plasticity functionality.

**Header:** `#include "plasticity/STDP.hpp"`

**Methods:**

```cpp
// Construction and destruction
STDP();                               // Default constructor
~STDP();                              // Destructor

// Configuration
void configure(double learningRate, double tauPlus, double tauMinus); // Configure STDP

// State management
void initialize();                     // Initialize STDP
void update(double timeStep);           // Update STDP

// Plasticity application
void applyLTP(double preTime, double postTime);   // Apply LTP
void applyLTD(double preTime, double postTime);   // Apply LTD

// State access
double getLTPWeight() const;            // Get LTP weight
double getLTDWeight() const;            // Get LTD weight
```

### Testing Framework

#### Namespace: nlm::test

The testing framework is available in the `nlm::test` namespace.

**Header:** `#include "TestingFramework.hpp"`

#### Class: TestCase
The TestCase class represents individual test cases.

**Methods:**

```cpp
// Construction and destruction
TestCase(const std::string& name, const std::string& category); // Constructor
virtual ~TestCase();                               // Destructor

// Test execution
virtual bool setupTestCase() = 0;                  // Setup test case
virtual void setup() = 0;                         // Setup test
virtual void run() = 0;                           // Run test
virtual void teardown() = 0;                      // Teardown test
virtual bool teardownTestCase() = 0;              // Teardown test case

// Test information
getName() const;                                  // Get test name
getCategory() const;                              // Get test category

// Test execution
void execute();                                   // Execute test
bool setupAndRun();                               // Setup and run test

// Test assertions (static methods)
static void assertEqual(const std::string& actual, const std::string& expected, const std::string& message);
static void assertNotEqual(const std::string& actual, const std::string& expected, const std::string& message);
```

#### Class: TestSuite
The TestSuite class represents test suites.

**Methods:**

```cpp
// Construction and destruction
TestSuite(const std::string& name, const std::string& category); // Constructor
~TestSuite();                                 // Destructor

// Test management
void registerTest(TestCase* testCase);       // Register test case
void unregisterTest(TestCase* testCase);     // Unregister test case

// Test execution
std::vector<TestResult> run();               // Run all tests

// Test information
getName() const;                              // Get suite name
getCategory() const;                          // Get suite category
```

### Python API

#### Module: pynlm

The Python API is accessible via the `pynlm` module.

**Usage:**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Python Classes

##### Brain Class
```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
```

##### Config Class
```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

##### AgentBrain Class
```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

##### SimpleWorld Class
```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

##### Enumerations
```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## API Documentation Examples

### Example 1: Basic Brain Simulation
```cpp
#include <nlm.h>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    // Print statistics
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}
```

### Example 2: Configuration Management
```cpp
#include <nlm.h>

int main() {
    // Create configuration
    nlm::Config config;
    
    // Load from file
    config.loadFromFile("config.json");
    
    // Modify configuration
    config.set("brain.neuron_count", 2000);
    config.set("plasticity.stdp.enable", true);
    
    // Save to file
    config.saveToFile("modified_config.json");
    
    // Validate configuration
    std::vector<nlm::ConfigSchema> schema = {
        nlm::ConfigSchema("brain.neuron_count", "int", true),
        nlm::ConfigSchema("brain.synapse_density", "float", false)
    };
    
    bool isValid = config.validateSchema(schema);
    
    return 0;
}
```

### Example 3: Agent Simulation
```cpp
#include <nlm.h>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(world);
    
    // Enable subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command
        auto motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command
        world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        
        // Update development
        agent->updateDevelopment(0.1);
    }
    
    // Print results
    std::cout << "Simulation completed successfully!" << std::endl;
    std::cout << "Final reward modulation level: " << agent->getCuriosityLevel() << std::endl;
    
    return 0;
}
```

### Example 4: Testing Framework Usage
```cpp
#include <nlm/test/TestFramework.hpp>

class MemoryIntegrationTest : public nlm::test::TestCase {
public:
    MemoryIntegrationTest() : TestCase("MemoryIntegration", "Memory") {}
    
    bool setupTestCase() override {
        // Setup for all tests in this suite
        return true;
    }
    
    void setup() override {
        // Setup for each test
        brain = std::make_shared<nlm::Brain>(nlm::createDefaultConfig());
        brain->initialize();
    }
    
    void run() override {
        // Test logic
        testWorkingMemoryIntegration();
        testEpisodicMemoryIntegration();
        testMemoryConnectivity();
    }
    
    void teardown() override {
        // Cleanup after each test
        brain.reset();
    }
    
    bool teardownTestCase() override {
        // Cleanup after all tests
        return true;
    }
    
private:
    std::shared_ptr<nlm::Brain> brain;
    
    void testWorkingMemoryIntegration() {
        ASSERT_NOT_NULL(brain);
        // Test working memory integration
    }
    
    void testEpisodicMemoryIntegration() {
        // Test episodic memory integration
        ASSERT(brain->getTotalNeuronCount() > 0);
    }
    
    void testMemoryConnectivity() {
        // Test memory system connectivity
        ASSERT(true); // Placeholder for actual test
    }
};
```

## API Usage Notes

### Type Definitions
The API uses the following type definitions:

- `SimulationStep`: Simulation step number (int64_t)
- `NeuronIndex`: Neuron index (size_t)
- `SynapseIndex`: Synapse index (size_t)
- `WorldCoordinate`: World coordinate (double)
- `EnergyValue`: Energy value (double)
- `TimeValue`: Time value (double)

### Error Handling
The API throws exceptions for errors:

- `nlm::ConfigError`: Configuration errors
- `std::runtime_error`: Runtime errors
- `std::invalid_argument`: Invalid arguments
- `std::out_of_range`: Out of range errors

### Memory Management
The API uses smart pointers for memory management:

- `std::shared_ptr`: Shared ownership
- `std::unique_ptr`: Unique ownership
- Raw pointers are only used in internal implementation

### Thread Safety
The API is thread-safe for concurrent access:

- Configuration objects are thread-safe
- Brain objects are thread-safe
- World objects are thread-safe
- Agent objects are thread-safe

## Migration Guide

### From v0.x to v1.0
1. **New namespace**: All classes are now in the `nlm` namespace
2. **New configuration system**: Use `nlm::Config` for configuration management
3. **New testing framework**: Use `nlm::test` for testing
4. **New C++11 features**: Code uses C++11 features

### API Changes
```cpp
// Old API
Brain brain(config);
brain.initialize();
brain.step(step);

// New API
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();
brain->step(step);
```

### Python API Changes
```python
# Old API
import nlm
brain = nlm.Brain(config)
brain.initialize()
brain.step(step)

# New API
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
```

## Performance Considerations

### Memory Usage
- Configuration objects: Minimal memory overhead
- Brain objects: Scalable memory usage based on configuration
- World objects: Small memory overhead
- Agent objects: Minimal memory overhead

### CPU Usage
- Brain simulation: High CPU usage for large networks
- Configuration management: Low CPU usage
- Testing: Variable CPU usage based on test complexity

### I/O Usage
- Configuration files: Read/write operations
- Checkpoint files: Save/load operations
- Test output: Write operations

## Integration with Other Components

### Configuration Integration
The configuration system integrates with:

- Command line interface
- Testing framework
- Performance benchmarks
- Experiments

### Testing Integration
The testing framework integrates with:

- Legacy tests
- Enhanced tests
- Benchmarks
- Performance monitoring

### Performance Integration
Performance optimization integrates with:

- Configuration system
- Testing framework
- Experiments
- Visualization

## Conclusion

The NLM API provides comprehensive access to NLM functionality through both C++ and Python bindings. It includes:

1. **Core Modules**: Brain, Agent, World, Memory, Neuromodulation, Plasticity, Sensory, Motor, Development, Prediction, Cognition, Environment
2. **Testing Framework**: TestCase, TestSuite, Benchmarks
3. **Configuration System**: Config, ConfigSchema, ConfigDiffEntry
4. **Python Bindings**: Full Python API via pybind11
5. **Documentation**: Comprehensive API reference

By following the examples and best practices in this documentation, you can effectively use the NLM API to:

1. **Create neural networks** with custom configurations
2. **Run simulations** with various environments
3. **Manage agents** with different behaviors
4. **Test and validate** NLM functionality
5. **Integrate with existing systems**
6. **Extend functionality** with custom components

The API is designed to be flexible, powerful, and easy to use, supporting both simple one-line commands and complex multi-stage workflows.

