# NLM API Documentation

This document provides comprehensive API documentation for the Neural Learning Machine (NLM) framework.

## Overview

NLM is an experimental computational brain project that implements a neural system capable of learning, memory, and cognitive functions. The framework includes:

- **Core Neural Components**: LIF neurons, synapses, neural populations, and regions
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Plasticity Systems**: STDP, Hebbian learning, and structural plasticity
- **Neuromodulation**: Dopamine, curiosity, novelty detection, and reward systems
- **Prediction Systems**: Neural prediction and error computation
- **Cognition Systems**: Neural planning, concept formation, and attention
- **Development Systems**: Developmental stages and structural changes
- **Agent System**: Sensory perception, motor output, and decision making
- **World System**: Environment interaction and simulation

## Core Classes

### Brain

**Description**: The central coordinator of the neural system, implementing real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

**Header**: `src/brain/Brain.hpp`

**Methods**:

#### Construction/Destruction
```cpp
Brain(std::shared_ptr<Config> config);      // Create brain with configuration
~Brain();                                   // Destructor
```

#### Core Functionality
```cpp
bool initialize();                          // Initialize brain with configuration
void step(SimulationStep currentStep);      // Main simulation step with timestamp
void step(SimulationStep currentStep, Timestamp currentTime); // Step with time
void reset();                                // Reset brain state
```

#### Input/Output
```cpp
void receiveSensoryInput(const SensoryInput& input);  // Receive sensory input
void injectCurrent(NeuronId neuron, MembranePotential current); // Inject current to neuron
void injectCurrentToNeurons(NeuronType type, MembranePotential current); // Inject to neuron type
std::unique_ptr<Action> produceAction();    // Get motor action
```

#### Subsystem Access
```cpp
SpikeSystem* getSpikeSystem();              // Get spike system
STDP* getSTDP();                            // Get STDP plasticity system
Hebbian* getHebbian();                      // Get Hebbian plasticity system
StructuralPlasticity* getStructuralPlasticity(); // Get structural plasticity

// Memory systems
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();

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

#### Statistics
```cpp
size_t getTotalNeuronCount() const;          // Total neurons
size_t getTotalSynapseCount() const;         // Total synapses
size_t getActiveNeuronCount() const;         // Currently active neurons
size_t getFiringNeuronCount() const;         // Currently firing neurons
float getAverageFiringRate() const;          // Average firing rate
size_t getTotalSpikeCount() const;           // Total spike count
float getExcitationInhibitionRatio() const;  // E/I balance
```

#### State Management
```cpp
bool save(const std::string& filepath) const; // Save brain state (checkpointing)
bool load(const std::string& filepath);       // Load brain state

// Region management
RegionId addRegion(const std::string& name = "");
NeuralRegion* getRegion(RegionId id);
const NeuralRegion* getRegion(RegionId id) const;
size_t getRegionCount() const;
std::vector<RegionId> getRegionIds() const;
```

### Neuron

**Description**: Implements Leaky Integrate-and-Fire (LIF) neuron dynamics with membrane potential integration, spike detection, and refractory periods.

**Header**: `src/brain/Neuron.hpp`

**Methods**:

#### Construction/Destruction
```cpp
Neuron(NeuronId id);     // Create neuron with ID
~Neuron();              // Destructor
```

#### Identity and Type
```cpp
NeuronId getId() const;
NeuronType getType() const;
void setType(NeuronType type);
```

#### State Access
```cpp
const NeuronState& getState() const;         // Get current state
NeuronState& getState();                     // Get current state (non-const)
```

#### Membrane Potential
```cpp
MembranePotential getMembranePotential() const;
void setMembranePotential(MembranePotential potential);
void addToMembranePotential(MembranePotential delta);
```

#### Threshold and State
```cpp
MembranePotential getThreshold() const;
void setThreshold(MembranePotential threshold);
bool checkThreshold() const;                  // Check if neuron should fire
bool isFiring() const;                       // Check if neuron is firing
bool isRefractory() const;                    // Check if neuron is refractory
void setFiringState(FiringState state);
void setRefractoryPeriod(uint32_t steps);
void decrementRefractory();                  // Decrement refractory counter
```

#### Firing Rate
```cpp
FiringRate getFiringRate() const;
void setFiringRate(FiringRate rate);
```

#### LIF Neuron Parameters
```cpp
void setLeakConductance(MembranePotential conductance);
MembranePotential getLeakConductance() const;
uint32_t getRefractoryPeriod() const;
void setRestingPotential(MembranePotential potential);
MembranePotential getRestingPotential() const;
void setResetPotential(MembranePotential potential);
```

#### Spike Detection
```cpp
bool stepLIF(Timestamp currentTime, TimestepDuration dt); // LIF integration step
float getLastSpikeTime() const;              // Get last spike timestamp
void recordSpike(Timestamp timestamp);      // Record spike time
const std::vector<Timestamp>& getSpikeHistory() const; // Get spike history
```

#### Input Processing
```cpp
void receiveExcitatoryInput(MembranePotential amplitude);
void receiveInhibitoryInput(MembranePotential amplitude);
void receiveModulatoryInput(MembranePotential amplitude);
void injectCurrent(MembranePotential current); // External current injection
MembranePotential getTotalCurrent() const;   // Get total current
void clearTotalCurrent();                    // Clear current inputs
```

#### Synapse Management
```cpp
void addIncomingSynapse(SynapseHandle handle);
void addOutgoingSynapse(SynapseHandle handle);
const std::vector<SynapseHandle>& getIncomingSynapses() const;
const std::vector<SynapseHandle>& getOutgoingSynapses() const;
```

#### Plasticity
```cpp
const PlasticityFlags& getPlasticityFlags() const;
PlasticityFlags& getPlasticityFlags();
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
```

#### Membership
```cpp
void setRegionId(RegionId region);
RegionId getRegionId() const;
void setPopulationId(PopulationId population);
PopulationId getPopulationId() const;
```

#### Simulation and State
```cpp
void step(Timestamp currentTime);            // Main simulation step
void reset();                                // Reset to initial state
void initializeRandom(RandomGenerator& rng);  // Initialize with random parameters
```

### Synapse

**Description**: Represents a connection between neurons with delay, weight, and short-term plasticity dynamics.

**Header**: `src/brain/Synapse.hpp`

**Methods**:

#### Construction/Destruction
```cpp
Synapse(SynapseId id, NeuronId source, NeuronId destination);
~Synapse();
```

#### Identity
```cpp
SynapseId getId() const;
NeuronId getSourceNeuron() const;
NeuronId getDestinationNeuron() const;
```

#### Weight
```cpp
SynapticWeight getWeight() const;
void setWeight(SynapticWeight weight);
void addToWeight(SynapticWeight delta);
```

#### Delay
```cpp
Delay getDelay() const;
void setDelay(Delay delay);
```

#### Type
```cpp
SynapseType getType() const;
void setType(SynapseType type);
bool isExcitatory() const;
bool isInhibitory() const;
```

#### Activity History
```cpp
void recordPreSpike(Timestamp timestamp);
void recordPostSpike(Timestamp timestamp);
const std::vector<Timestamp>& getPreSpikeHistory() const;
const std::vector<Timestamp>& getPostSpikeHistory() const;
void clearHistory();
```

#### Plasticity
```cpp
const PlasticityFlags& getPlasticityFlags() const;
PlasticityFlags& getPlasticityFlags();
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
```

#### Learning Mechanisms
```cpp
float getEligibilityTrace() const;
void setEligibilityTrace(float trace);
void decayEligibilityTrace(float decayRate);

float getEfficacy() const;
void setEfficacy(float efficacy);
```

#### Simulation
```cpp
void step(Timestamp currentTime);            // Synaptic dynamics step
void reset();                                // Reset to initial state
void initializeRandom(RandomGenerator& rng);  // Initialize with random parameters
```

### Config

**Description**: Configuration management system for NLM with file-based configuration and runtime modification.

**Header**: `src/core/Config/Config.hpp`

**Methods**:

#### Construction/Destruction
```cpp
Config();                                    // Default constructor
~Config();                                   // Destructor
```

#### Configuration Operations
```cpp
bool loadFromFile(const std::string& filename);    // Load from file
bool loadFromJsonFile(const std::string& filename); // Load from JSON file
bool loadFromYamlFile(const std::string& filename); // Load from YAML file
void saveToFile(const std::string& filename) const; // Save to file
void saveToJsonFile(const std::string& filename) const; // Save to JSON file
void saveToYamlFile(const std::string& filename) const; // Save to YAML file
```

#### Key-Value Operations
```cpp
void set(const std::string& key, const Any& value, ConfigSource source = ConfigSource::Default);
Any getOr(const std::string& key, const Any& defaultValue = Any()) const;
const Any& get(const std::string& key) const;
bool has(const std::string& key) const;
std::vector<std::string> getKeys() const;    // Get all keys
```

#### Configuration Management
```cpp
void loadFromArgs(int argc, char** argv);    // Load from command line
std::shared_ptr<const Config> clone() const; // Create clone
bool equals(const Config& other) const;       // Compare configurations
```

### NeuralRegion

**Description**: Container for neurons and synapses, representing a brain region with local connectivity.

**Header**: `src/brain/NeuralRegion.hpp`

**Methods**:

#### Construction/Destruction
```cpp
NeuralRegion(RegionId id);                   // Create region with ID
~NeuralRegion();                            // Destructor
```

#### Neuron Management
```cpp
RegionId getId() const;
void setName(const std::string& name);
std::string getName() const;

NeuronId addNeuron(NeuronType type);         // Add neuron to region
Neuron* getNeuron(NeuronId id);              // Get neuron by ID
const Neuron* getNeuron(NeuronId id) const;  // Get neuron by ID (const)
bool removeNeuron(NeuronId id);              // Remove neuron
size_t getNeuronCount() const;               // Get neuron count
std::vector<NeuronId> getNeuronIds() const;  // Get all neuron IDs
std::vector<Neuron*> getAllNeurons();        // Get all neurons
```

#### Synapse Management
```cpp
SynapseId addSynapse(NeuronId source, NeuronId destination, SynapseType type = SynapseType::Excitatory);
Synapse* getSynapse(SynapseId id);            // Get synapse by ID
const Synapse* getSynapse(SynapseId id) const; // Get synapse by ID (const)
bool removeSynapse(SynapseId id);            // Remove synapse
size_t getSynapseCount() const;              // Get synapse count
```

#### Statistics
```cpp
float getExcitatoryInhibitionRatio() const; // E/I balance
float getAverageWeight() const;              // Average synaptic weight
size_t getConnectedNeuronCount() const;      // Number of connected neurons
```

### SpikeSystem

**Description**: Event-driven spike propagation system with synaptic delays and spike queue management.

**Header**: `src/dynamics/SpikeSystem.hpp`

**Methods**:

#### Construction/Destruction
```cpp
SpikeSystem();                              // Constructor
~SpikeSystem();                             // Destructor
```

#### Spike Management
```cpp
void recordSpike(NeuronId neuronId, Timestamp spikeTime); // Record spike
void processSpikes(Timestamp currentTime);   // Process queued spikes
bool hasPendingSpikes() const;               // Check for pending spikes
size_t getPendingSpikeCount() const;         // Get pending spike count
```

#### Access
```cpp
const std::vector<std::pair<NeuronId, Timestamp>>& getPendingSpikes() const;
```

### RandomGenerator

**Description**: Thread-safe random number generator with various distribution functions.

**Header**: `src/core/Random/Random.hpp`

**Methods**:

#### Construction/Destruction
```cpp
RandomGenerator();                           // Default constructor
RandomGenerator(uint64_t seed);              // Constructor with seed
~RandomGenerator();                          // Destructor
```

#### Random Number Generation
```cpp
float uniformReal(float min = 0.0f, float max = 1.0f);    // Uniform [min, max]
int uniformInt(int min = 0, int max = 100);              // Uniform [min, max]
float normalReal(float mean = 0.0f, float stddev = 1.0f); // Normal distribution
float exponential(float lambda = 1.0f);                   // Exponential distribution
float gaussian(float mean = 0.0f, float stddev = 1.0f);  // Gaussian distribution
```

#### Seeding
```cpp
void setSeed(uint64_t seed);                 // Set seed
uint64_t getSeed() const;                    // Get current seed
void seedFromSystem();                       // Seed from system time
```

### Logger

**Description**: Thread-safe logging system with multiple output targets and configurable levels.

**Header**: `src/core/Logger/Logger.hpp`

**Methods**:

#### Construction/Destruction
```cpp
Logger();                                   // Default constructor
~Logger();                                  // Destructor
```

#### Logger Management
```cpp
void addLogger(std::shared_ptr<Logger> logger);  // Add logger
void removeLogger(std::shared_ptr<Logger> logger); // Remove logger
void clearLoggers();                         // Clear all loggers
static void setGlobal(std::shared_ptr<Logger> logger); // Set global logger
static std::shared_ptr<Logger> getGlobal();   // Get global logger
```

#### Logging Levels
```cpp
void setLevel(LogLevel level);               // Set global log level
LogLevel getLevel() const;                   // Get global log level
```

#### Log Methods
```cpp
void logDebug(const std::string& message);
void logInfo(const std::string& message);
void logWarning(const std::string& message);
void logError(const std::string& message);
void logCritical(const std::string& message);
```

### SimulationClock

**Description**: High-precision timing system for neural simulations.

**Header**: `src/core/SimulationClock/SimulationClock.hpp`

**Methods**:

#### Construction/Destruction
```cpp
SimulationClock(double timestep = 0.001);    // Constructor with timestep
~SimulationClock();                          // Destructor
```

#### Time Management
```cpp
double getTimestep() const;                  // Get timestep
void setTimestep(double timestep);           // Set timestep
double getTime() const;                      // Get current simulation time
void advanceTime();                           // Advance time by timestep
void setTime(double time);                    // Set current time
```

## Python API

The NLM framework provides Python bindings through pybind11, enabling easy integration with Python scripts and data analysis tools.

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Key Python Classes

#### Config Class
```python
# Create configuration
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    print(config.getOr("brain.neuron_count", 1000))

# Modify configuration
config.set("brain.neuron_count", 2000)

# Save/load configuration
config.saveToFile("config.json")
config.loadFromFile("config.json")
```

#### Brain Class
```python
# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Get statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
brain.getExcitationInhibitionRatio()

# Simulation control
brain.step(step_number)                    # Basic step
brain.step(step_number, time)              # Step with timestamp
brain.reset()                              # Reset state
brain.save("checkpoint.bin")                # Save state
brain.load("checkpoint.bin")                # Load state
```

#### AgentBrain Class
```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()

# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(timestep)
```

#### SimpleWorld Class
```python
# Create simple world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Control world
world.reset()
world.update(timestep)
world.setAgentStart(x, y)
world.applyMotorCommand(action, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

### Complete Example

```python
#!/usr/bin/env python3
import pynlm

def run_complete_example():
    """Run a complete NLM example with agent and world interaction."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(1000):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("✅ Complete example finished successfully!")
    return brain, world, agent

if __name__ == "__main__":
    run_complete_example()
```

## Configuration Options

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Data Types

### Enumeration Types
```cpp
// Neuron types
NeuronType::Excitatory    // Excitatory neuron
NeuronType::Inhibitory    // Inhibitory neuron
NeuronType::Sensory        // Sensory neuron
NeuronType::Motor          // Motor neuron
NeuronType::Modulatory      // Neuromodulatory neuron
NeuronType::Internal       // Internal neuron

// Developmental stages
DevelopmentalStage::Initial      // Initial developmental stage
DevelopmentalStage::CriticalPeriod // Critical period
DevelopmentalStage::Maturation    // Maturation phase
DevelopmentalStage::Adult        // Adult stage
DevelopmentalStage::Aging        // Aging stage

// Action types
ActionType::MoveForward
ActionType::TurnLeft
ActionType::TurnRight
ActionType::Interact
ActionType::Eat
ActionType::Sleep
ActionType::Explore
```

### Numeric Types
```cpp
using NeuronId = uint32_t;           // Neuron identifier
using SynapseId = uint32_t;          // Synapse identifier
using RegionId = uint32_t;           // Region identifier
using PopulationId = uint32_t;       // Population identifier

using MembranePotential = float;     // Membrane potential (mV)
using SynapticWeight = float;        // Synaptic weight
using FiringRate = float;            // Firing rate (Hz)
using Timestamp = double;            // Time (seconds)
using TimestepDuration = double;    // Time step duration (seconds)
using Delay = uint32_t;              // Delay in simulation steps
```

## File I/O

### Configuration Files
NLM supports multiple configuration file formats:

#### CFG Files (`.cfg`)
Simple key-value format:
```
neuron_count = 1000
region_count = 2
connection_probability = 0.1
```

#### JSON Files (`.json`)
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity": {
        "stdp": {
            "learning_rate": 0.01,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        }
    }
}
```

#### YAML Files (`.yaml`, `.yml`)
```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.1
plasticity:
  stdp:
    learning_rate: 0.01
    tau_plus: 20.0
    tau_minus: 20.0
```

## Building and Installation

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

### Building from Source
```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Building Python Library
```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python package
pip install .

# Or install in development mode
pip install -e .
```

### Build Options
```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Performance Optimization

### Memory Management
- **Memory Pools**: Lock-free memory pools for neurons and synapses
- **Structure of Arrays (SoA)**: Optimized for numerical computation
- **Event Queues**: Efficient spike propagation with delays

### Compilation Optimizations
- **SIMD Vectorization**: Use of SIMD instructions for neural computations
- **Cache-Friendly Layout**: Memory layout optimized for CPU cache
- **Thread Safety**: Lock-free algorithms for concurrent access

### Configuration for Performance
```cpp
// Enable performance optimizations in configuration
"performance.use_memory_pools": true
"performance.enable_simd": true
"performance.lock_free_structures": true
"performance.event_queue_size": 10000
```

## Testing and Validation

### Unit Tests
The framework includes comprehensive unit tests:

```bash
# Run tests from build directory
ctest --output-on-failure

# Run specific test
./nlm_test test_brain --verbose
```

### Performance Benchmarks
```bash
# Run performance benchmarks
./nlm --benchmark

# Or use Python benchmarking tools
python scripts/nlm_benchmark.py
```

### Validation Scripts
```bash
# Run validation suite
python scripts/nlm_validator.py --create
python scripts/nlm_validator.py --suite
```

## Examples and Tutorials

### Basic Examples
- `examples/getting_started.py` - Getting started with NLM
- `examples/basic_brain.py` - Basic brain configuration
- `examples/advanced_agent.py` - Advanced agent example

### Research Examples
- `examples/memory_systems.py` - Memory systems demonstration
- `examples/neuromodulation.py` - Neuromodulation example
- `examples/development.py` - Development system example
- `examples/plasticity.py` - Plasticity learning example

### Production Examples
- `examples/production_use.py` - Production-ready example
- `examples/visualization.py` - Visualization example
- `examples/benchmarking.py` - Performance benchmarking

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure the agent has a valid motor command

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check for memory corruption or buffer overflows

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

## Further Reading

### Documentation
- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/ROADMAP.md` - Development roadmap
- `docs/EXPERIMENTS.md` - Experiment descriptions

### Technical Reports
- `docs/PERFORMANCE_ANALYSIS.md` - Performance analysis
- `docs/MEMORY_MANAGEMENT.md` - Memory management optimization
- `docs/VALIDATION_REPORT.md` - Validation results

## License

MIT License. See `LICENSE` file for details.

## Authors

Research project - See `docs` for scientific background.
