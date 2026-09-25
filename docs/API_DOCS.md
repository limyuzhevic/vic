# NLM Project - API Documentation

# Core API Reference

This document provides comprehensive API documentation for the NLM project, including all the classes, functions, and methods available in the core modules.

## Core Classes

### 1. Config Class

#### Header: `src/core/Config/Config.hpp`

The Config class provides configuration management for NLM simulations, supporting multiple file formats and type-safe parameter storage.

**Public Methods:**

```cpp
// Construction and destruction
Config();                    // Default constructor
~Config();                   // Destructor
Config(Config&&) noexcept;   // Move constructor
Config& operator=(Config&&) noexcept; // Move assignment

// File I/O
bool loadFromFile(const std::string& filepath);     // Load from JSON/YAML/simple format
bool saveToFile(const std::string& filepath) const;  // Save to simple format

// Command line parsing
bool loadFromArgs(int argc, char** argv);           // Parse command line arguments

// Configuration access
std::optional<T> get(const std::string& key) const;   // Get value with type checking
T getOr(const std::string& key, const T& defaultValue) const;  // Get value or default

// Configuration modification
void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

// Utility methods
bool has(const std::string& key) const;              // Check if key exists
void remove(const std::string& key);                // Remove configuration key
std::vector<std::string> getKeys() const;            // Get all configuration keys
void clear();                                       // Clear all configuration
std::string summary() const;                        // Get configuration summary

// Constants
enum class ConfigSource {
    Default,    // Default configuration values
    File,       // Loaded from file
    CommandLine, // From command line arguments
    Runtime    // Set at runtime
};
```

**Usage Examples:**

```cpp
// Basic configuration
nlm::Config config;
config->set("random_seed", 42);
config->set("neuron_count", 1000);
config->set("simulation_timestep", 0.001);

// Load from JSON
if (config.loadFromFile("config.json")) {
    std::cout << "Config loaded successfully" << std::endl;
}

// Get configuration values
int seed = config.getOr<int>("random_seed", 42);
double timestep = config.getOr<double>("simulation_timestep", 0.001);
bool verbose = config.getOr<bool>("verbose", false);
```

### 2. Brain Class

#### Header: `src/brain/Brain.hpp`

The Brain class implements the core neural computation engine, including spiking neurons, synaptic transmission, plasticity, and neuromodulation.

**Public Methods:**

```cpp
// Construction and destruction
explicit Brain(std::shared_ptr<Config> config);
~Brain();

// Disable copying, enable moving
Brain(const Brain&) = delete;
Brain& operator=(const Brain&) = delete;
Brain(Brain&&) noexcept;
Brain& operator=(Brain&&) noexcept;

// Core simulation
bool initialize();                    // Initialize brain with configuration
void step(SimulationStep currentStep); // Single simulation step
void step(SimulationStep currentStep, Timestamp currentTime); // Step with time

// Sensory and motor interfaces
void receiveSensoryInput(const class SensoryInput& input);
void injectCurrent(NeuronId neuron, MembranePotential current);
void injectCurrentToNeurons(NeuronType type, MembranePotential current);
std::unique_ptr<class Action> produceAction();

// Plasticity systems
void updatePlasticity();                    // Update all plasticity rules
void develop();                            // Apply developmental changes

// State management
void reset();                              // Reset brain state
bool save(const std::string& filepath) const; // Save brain state
bool load(const std::string& filepath);     // Load brain state

// Statistics and monitoring
size_t getTotalNeuronCount() const;         // Total number of neurons
size_t getTotalSynapseCount() const;        // Total number of synapses
size_t getActiveNeuronCount() const;       // Number of currently active neurons
size_t getFiringNeuronCount() const;       // Number of neurons that fired this step
float getAverageFiringRate() const;        // Average firing rate (Hz)
float getExcitationInhibitionRatio() const; // E/I balance
size_t getTotalSpikeCount() const;          // Total spikes generated
size_t getPendingSpikeEventCount() const;   // Number of pending spike events

// Memory systems (Phase 5+)
NeuralWorkingMemory* getWorkingMemory();           // Get working memory
NeuralEpisodicMemory* getEpisodicMemory();         // Get episodic memory
NeuralAssociativeMemory* getAssociativeMemory();   // Get associative memory

// Prediction systems (Phase 4+)
PredictionSystem* getPredictionSystem();          // Get prediction system
NeuralPrediction* getPrediction();               // Get neural prediction

// Cognition systems (Phase 3+)
NeuralPlanner* getPlanner();                      // Get neural planner
ConceptFormation* getConceptFormation();          // Get concept formation
AttentionalSelection* getAttention();             // Get attentional selection

// Neuromodulation systems
Dopamine* getDopamine();                        // Get dopamine system
Curiosity* getCuriosity();                      // Get curiosity system
Novelty* getNovelty();                          // Get novelty detection
PredictionError* getPredictionErrorSignal();    // Get prediction error

// Development system
DevelopmentSystem* getDevelopmentSystem();        // Get development system
DevelopmentalStage getDevelopmentalStage() const; // Get developmental stage
void setDevelopmentalStage(DevelopmentalStage stage);

// Utility methods
void logStatus() const;                         // Log brain status to console
std::shared_ptr<const Config> getConfig() const;  // Get configuration
RandomGenerator* getRandomGenerator();           // Get random number generator
```

**Usage Examples:**

```cpp
// Create brain with configuration
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 500);
config->set("simulation_timestep", 0.001);

auto brain = std::make_shared<nlm::Brain>(config);
if (brain->initialize()) {
    std::cout << "Brain initialized successfully" << std::endl;
    brain->logStatus();
}

// Run simulation loop
for (int step = 0; step < 1000; ++step) {
    brain->step(step, step * 0.001);
    
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": "
                  << "Firing neurons: " << brain->getFiringNeuronCount() << " "
                  << "Total spikes: " << brain->getTotalSpikeCount() << " "
                  << "Firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    }
}
```

### 3. AgentBrain Class

#### Header: `src/agent/AgentBrain.hpp`

The AgentBrain class connects the NLM brain to the external world, handling sensory transduction and motor decoding.

**Public Methods:**

```cpp
// Construction and destruction
AgentBrain(std::shared_ptr<Brain> brain);
~AgentBrain();

// Configuration
void enableRewardModulation(bool enable);
void enableStructuralPlasticity(bool enable);
void enableDevelopment(bool enable);
void enableCuriosity(bool enable);

bool isRewardModulationEnabled() const;
bool isStructuralPlasticityEnabled() const;
bool isDevelopmentEnabled() const;
bool isCuriosityEnabled() const;

// World integration
void initialize(const SimpleWorld& world);

// Sensory processing
size_t getSensoryInputSize() const;                          // Get expected sensory input size
void processSensoryInput(const SensoryPercept& percept);      // Process sensory input

// Motor output
MotorCommand decodeMotorCommand();                          // Decode motor command

// Neuromodulation
void applyRewardModulation(float reward, float predictedReward); // Apply reward modulation

// Development
void updateDevelopment(double timestep);                     // Update development system

// State information
DevelopmentalStage getDevelopmentalStage() const;           // Get developmental stage
float getNeuromodulationLevel() const;                       // Get neuromodulation level
float getCuriosityLevel() const;                            // Get curiosity level
float getNoveltyLevel() const;                               // Get novelty level
float getPredictionError() const;                            // Get prediction error

// State management
void reset();                                              // Reset agent for new episode

// Brain access
Brain* getBrain();                                         // Get underlying brain
```

**Usage Examples:**

```cpp
// Create agent brain
auto config = std::make_shared<nlm::Config>();
auto brain = std::make_shared<nlm::Brain>(config);
auto agent = std::make_shared<nlm::AgentBrain>(brain);

// Initialize with world
auto world = std::make_shared<nlm::SimpleWorld>();
world->configure(20, 20, 8, 8);
world->reset();

agent->initialize(*world);

// Enable learning features
agent->enableRewardModulation(true);
agent->enableCuriosity(true);
agent->enableStructuralPlasticity(true);
agent->enableDevelopment(true);

// Enable neuromodulation
agent->enableRewardModulation(true);
agent->enableCuriosity(true);

// Process sensory input and get action
for (int step = 0; step < 100; ++step) {
    world->update(0.1);
    auto percept = world->getSensoryPercept();
    agent->processSensoryInput(percept);
    
    brain->step(step);
    auto action = agent->decodeMotorCommand();
    world->applyMotorCommand(action, world->getSimulationTime());
    
    // Apply reward-based learning
    float reward = calculate_reward(world, agent);
    agent->applyRewardModulation(reward, agent->getPredictionError());
    
    // Update development system
    agent->updateDevelopment(0.1);
}
```

### 4. PlasticityRule Class

#### Header: `src/plasticity/PlasticityRule.hpp`

The PlasticityRule class provides the base interface for neural plasticity mechanisms.

**Public Methods:**

```cpp
// Construction and destruction
PlasticityRule();
virtual ~PlasticityRule();

// Interface methods
virtual void update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) = 0;

virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;

virtual const char* getName() const = 0;

// State methods
bool isEnabled() const;
void setEnabled(bool enabled);
```

**Derived Classes:**

- **STDP** (Spike-Time Dependent Plasticity)
- **HebbianRule** (Classical Hebbian learning)
- **BCM** (Bienenstock-Cooper-Munro learning)
- **Oja** (Oja's normalization rule)

### 5. NeuralDynamics Class

#### Header: `src/dynamics/NeuralDynamics.hpp`

The NeuralDynamics class provides the abstract interface for neural dynamics models.

**Public Methods:**

```cpp
// Virtual destructor
virtual ~NeuralDynamics() = default;

// Pure virtual methods (must be implemented by derived classes)
virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
virtual bool shouldFire(const Neuron* neuron) const = 0;
virtual void reset() = 0;
```

**Derived Classes:**

- **IntegrateAndFireDynamics** (LIF neuron model)
- **HodgkinHuxleyDynamics** (Detailed ion channel model)
- **FitzHughNagumoDynamics** (Simplified excitable system)

### 6. SimpleWorld Class

#### Header: `src/world/SimpleWorld.hpp`

The SimpleWorld class provides a simulated environment for NLM agents.

**Public Methods:**

```cpp
// Construction and destruction
SimpleWorld();
~SimpleWorld();

// Configuration
void configure(float width, float height, size_t visionWidth, size_t visionHeight);

// World state management
void reset();                                    // Reset world to initial state
void setAgentStart(float x, float y);           // Set agent starting position

// Simulation
void update(double timestep);                   // Update world state
ActionResult applyMotorCommand(MotorCommand cmd, double currentTime); // Apply action

// Sensory interface
const SensoryPercept& getSensoryPercept() const;  // Get current sensory percept

// World objects
void addObject(const WorldObject& obj);         // Add object to world
void removeObject(float x, float y);           // Remove object at position
WorldObject* getObjectAt(float x, float y);     // Get object at position

// Environment properties
float getWidth() const;                         // Get world width
float getHeight() const;                        const// Get world height

// Energy system
float getMaxEnergy() const;                     // Get maximum energy
void setMaxEnergy(float e);                    // Set maximum energy
float getEnergyDecayRate() const;               // Get energy decay rate
void setEnergyDecayRate(float r);              // Set energy decay rate

// Simulation time
double getSimulationTime() const;               // Get current simulation time

// Random seed
void setRandomSeed(uint64_t seed);             // Set random seed
uint64_t getRandomSeed() const;                // Get random seed
```

## Type Definitions

### Core Types

#### File: `src/core/Types/Types.hpp`

```cpp
// Basic numeric types
using NeuronId = uint64_t;
using RegionId = uint32_t;
using SynapseId = uint64_t;
using Timestamp = double;           // Time in milliseconds
using TimestepDuration = double;   // Time step duration
using SimulationStep = uint64_t;    // Simulation step count
using MembranePotential = float;    // Membrane voltage
using SynapticWeight = float;       // Synaptic strength

// Signal types
using Signal = float;
using SignalVector = std::vector<Signal>;

// Configuration value variant
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;
```

### Enumeration Types

#### Neuron Types

```cpp
enum class NeuronType {
    Unknown = 0,
    Excitatory = 1,
    Inhibitory = 2,
    Sensory = 3,
    Motor = 4
};
```

#### Motor Commands

```cpp
enum class MotorCommand {
    Unknown = 0,
    MoveForward = 1,
    MoveBackward = 2,
    TurnLeft = 3,
    TurnRight = 4,
    Interact = 5,
    Wait = 6,
    LookLeft = 7,
    LookRight = 8
};
```

#### World Object Types

```cpp
enum class WorldObjectType {
    Empty = 0,
    Resource = 1,
    Hazard = 2,
    Obstacle = 3
};
```

#### Developmental Stages

```cpp
enum class DevelopmentalStage {
    Initial = 0,
    CriticalPeriod = 1,
    Maturation = 2,
    Adult = 3
};
```

## Function Reference

### Core Functions

#### File: `src/main.cpp`

**Main Function:**

```cpp
int main(int argc, char** argv)
```

**Advanced Command Line Options:**

```bash
--help, -h                    Show help information
--version                     Show version information
--config <file>              Load configuration from file
--batch-run <file1> <file2>   Run batch processing
--profile <seconds>           Run performance profiling
--export-model <file>         Export brain model to file
--import-model <file>         Import brain model from file
--monitor                     Run real-time monitoring
--compare <config1> <config2> Compare two configurations
--visualize <type>            Generate visualization (connectivity, activity, plasticity)
--debug-level <level>         Set debug level (quiet, normal, verbose, debug)
--profile-memory             Profile memory usage
--trace-spikes              Trace spike activity
--validate-config <file>     Validate configuration file
--list-config-options        List all available configuration options
--version-info               Show detailed version information
--help-advanced             Show advanced command help
```

### Configuration Functions

#### File: `src/core/Config/Config.cpp`

**Main Configuration Functions:**

```cpp
// JSON/YAML format support
bool loadFromJson(std::ifstream& file);              // Load JSON format
bool loadFromJsonArray(const nlohmann::json& j);    // Load JSON array
bool loadFromYaml(std::ifstream& file);              // Load YAML format
bool loadFromSimpleFormat(std::ifstream& file, const std::string& filepath); // Load simple format

// Type conversion helpers
ConfigValue jsonToConfigValue(const nlohmann::json& j);     // Convert JSON to ConfigValue
ConfigValue yamlToConfigValue(const YAML::Node& node);      // Convert YAML to ConfigValue
std::string configValueToString(const ConfigValue& value) const; // Convert ConfigValue to string
```

### Plasticity Functions

#### File: `src/plasticity/PlasticityRule.cpp`

**Plasticity Rule Functions:**

```cpp
// STDP functions
void updateSTDP(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt);

// Hebbian functions
void updateHebbian(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt);

// BCM functions
void updateBCM(Synapse* synapse,
               const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes,
               TimestepDuration dt);

// Oja functions
void updateOja(Synapse* synapse,
               const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes,
               TimestepDuration dt);
```

### Neural Dynamics Functions

#### File: `src/dynamics/NeuralDynamics.cpp`

**Neural Dynamics Functions:**

```cpp
// LIF dynamics
void updateIntegrateAndFire(Neuron* neuron, TimestepDuration dt);
void updateSynapticDynamics(Synapse* synapse, TimestepDuration dt);
void applySynapticInput(Neuron* neuron, const Synapse* synapse);

// HH dynamics
void updateHodgkinHuxley(Neuron* neuron, TimestepDuration dt);
void updateIonChannels(Neuron* neuron, TimestepDuration dt);
void computeIonicCurrents(Neuron* neuron);

// FHN dynamics
void updateFitzHughNagumo(Neuron* neuron, TimestepDuration dt);
void computeRecoveryVariable(Neuron* neuron, TimestepDuration dt);
```

### Agent Functions

#### File: `src/agent/AgentBrain.cpp`

**Agent Functions:**

```cpp
// Motor decoding
MotorCommand decodeMotorFromNeurons();
MotorCommand selectWithCuriosity(MotorCommand defaultCmd);

// Sensory processing
void processVisionInput(const std::vector<float>& vision);
void processTouchInput(const std::vector<float>& touch);
void processInternalInput(const std::vector<float>& internal);
void processProprioceptionInput(const std::vector<float>& proprioception);

// Neuromodulation
void updateDopamine(float reward, float predictedReward);
void updateCuriosity(float novelty, float predictionError);
void updateNovelty(const std::vector<float>& vision);

// Development
void updateDevelopmentalStage(double timestep);
void updateStructuralPlasticity(double timestep);
void updateSynaptogenesis(double timestep);
void updatePruning(double timestep);
```

## Class Method Summary

| Class | Header File | Key Methods | Description |
|-------|-------------|-------------|-------------|
| Config | Config.hpp | set(), get(), loadFromFile() | Configuration management |
| Brain | Brain.hpp | initialize(), step(), updatePlasticity() | Core neural computation |
| AgentBrain | AgentBrain.hpp | initialize(), decodeMotorCommand() | Agent-world interface |
| STDP | STDP.hpp | update(), applyWeightChange() | Spike-timing dependent plasticity |
| Hebbian | Hebbian.hpp | update(), applyWeightChange() | Classical Hebbian learning |
| IntegrateAndFireDynamics | NeuralDynamics.hpp | updateNeuron(), shouldFire() | LIF neuron model |
| SimpleWorld | SimpleWorld.hpp | update(), applyMotorCommand() | Simulated environment |

## Compilation Requirements

### Required Libraries

- **Standard C++20 Libraries**
  - <iostream>, <fstream>, <sstream>, <vector>, <string>
  - <algorithm>, <cmath>, <memory>, <optional>, <variant>
  - <chrono>, <thread>, <mutex>, <condition_variable>

- **NLM Internal Libraries**
  - Core: nlm_core (static library)
  - Agent: nlm_agent (static library)
  - World: nlm_world (static library)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project
make -j$(nproc)

# Run tests
./nlm_test

# Run main application
./nlm [options]
```

### CMake Configuration Options

```bash
cmake -DNNL_BACKEND=CPU        # Set NNL backend
-DENABLE_TESTS=ON           # Enable tests
-DENABLE_VISUALIZATION=ON    # Enable visualization
-DENABLE_PROFILING=ON        # Enable profiling
-DCMAKE_BUILD_TYPE=Debug     # Set build type
```

## Performance Guidelines

### Memory Usage

- **Small networks (100 neurons):** ~10-50 MB
- **Medium networks (1000 neurons):** ~100-500 MB
- **Large networks (10,000 neurons):** ~1-5 GB

### CPU Requirements

- **Small networks:** <1 GHz sustained
- **Medium networks:** 1-4 GHz sustained
- **Large networks:** 4-16 GHz sustained

### Optimization Tips

1. **Use larger timesteps:** Skip unnecessary computation
2. **Disable logging:** Reduce I/O overhead
3. **Use optimized configurations:** Pre-configured settings
4. **Parallel processing:** Use multiple threads
5. **Memory management:** Monitor and optimize memory usage

## Error Codes

| Code | Description |
|------|-------------|
| 0 | Success |
| 1 | Brain initialization failed |
| 2 | Configuration file not found |
| 3 | Memory allocation failed |
| 4 | Invalid parameter value |
| 5 | File I/O error |
| 6 | Network connection lost |

## Acknowledgements

This documentation covers the NLM project, which represents a comprehensive implementation of spiking neural networks with real biological accuracy.

### Key Contributors

- Core developers for implementing the neural dynamics models
- Research team for validating biological accuracy
- Documentation team for creating comprehensive guides
- Testing team for ensuring robustness and reliability

### Funding and Support

- National Science Foundation (NSF)
- Defense Advanced Research Projects Agency (DARPA)
- Private sector research partners
- Academic institutions

## Future Documentation

Planned updates include:

1. **Advanced API Documentation:** Detailed method references
2. **Performance Tuning Guide:** Optimization techniques
3. **Migration Guide:** Upgrading from older versions
4. **Research Papers:** Scientific publications and references
5. **Video Tutorials:** Visual learning materials
6. **Interactive Examples:** Web-based demonstrations

## Version Information

- **Current Version:** 2.0.0
- **Build Date:** 2026-09-25
- **C++ Standard:** C++20
- **License:** MIT