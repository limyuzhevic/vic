# NLM User Guide
# Neural Learning Machine Comprehensive Documentation

## Overview

The NLM (Neural Learning Machine) framework is a sophisticated artificial developmental brain simulation system designed to model brain-like neural computation. This comprehensive guide provides both beginners and advanced users with detailed instructions for using, configuring, and extending the NLM system.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Installation and Setup](#installation-and-setup)
3. [Configuration Management](#configuration-management)
4. [Basic Usage](#basic-usage)
5. [Advanced Features](#advanced-features)
6. [Python Interface](#python-interface)
7. [Command Line Interface](#command-line-interface)
8. [Performance Optimization](#performance-optimization)
9. [Troubleshooting](#troubleshooting)
10. [Best Practices](#best-practices)
11. [API Reference](#api-reference)
12. [Examples and Tutorials](#examples-and-tutorials)

## Quick Start

### What is NLM?

NLM is a Phase 6 integrated artificial brain simulation system that includes:

- **Neural computation**: Leaky Integrate-and-Fire neurons with spike-based propagation
- **Memory systems**: Working memory, episodic memory, semantic memory, procedural memory, and associative memory
- **Plasticity mechanisms**: STDP, Hebbian learning, and structural plasticity
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Cognitive systems**: Planning, concept formation, and attention
- **Sensory-motor loop**: Vision, touch, internal signals, and action selection

### Running Your First Simulation

#### 1. Simple C++ Program

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>

int main() {
    // Create default configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
        brain->logStatus();
    }
    
    return 0;
}
```

#### 2. Python Interface

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    print(f"Step {step} completed")

# Get statistics
stats = brain.getStatistics()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Active neurons: {stats['active_neurons']}")

brain.reset()
```

## Installation and Setup

### System Requirements

- **C++ Compiler**: C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake**: Version 3.16 or higher
- **Build Tools**: Make or Ninja
- **Python**: Python 3.8 or higher
- **Optional**: Python bindings with pybind11

### Building from Source

#### Step 1: Clone the Repository

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

#### Step 2: Configure Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20
```

#### Step 3: Build the Project

```bash
make -j4
```

#### Step 4: Install (Optional)

```bash
make install
```

### Building Python Bindings

```bash
# Install Python dependencies
pip install pytest numpy

# Build Python bindings
pip install .

# Test the installation
python -c "import pynlm; print(pynlm.version())"
```

### Docker Support

If you have Docker installed, you can build the project with Docker:

```bash
docker build -t nlm .
docker run -it nlm:latest
```

## Configuration Management

### Configuration File Formats

NLM supports multiple configuration file formats:

#### 1. JSON Configuration

```json
{
    "nlm": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "log_level": "INFO",
        "log_to_file": true,
        "max_simulation_steps": 10000
    }
}
```

#### 2. YAML Configuration

```yaml
nlm:
  random_seed: 42
  simulation_timestep: 0.001
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  stdp_ltp_weight: 0.01
  stdp_ltd_weight: 0.012
  log_level: INFO
  log_to_file: true
  max_simulation_steps: 10000
```

#### 3. Simple Key-Value Format

```
# configs/simple.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
log_to_file = false
```

### Configuration Loading Methods

#### 1. Load from File

```cpp
#include <nlm/Config.hpp>

auto config = std::make_shared<nlm::Config>();
config->loadFromFile("production_config.json");
```

```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("production_config.json")
```

#### 2. Load from Command Line

```bash
./nlm --neuron_count=5000 --region_count=4 --log_level=DEBUG
```

```python
import pynlm

config = pynlm.createDefaultConfig()
# Command line arguments are loaded when the C++ program is executed
```

#### 3. Load from Python Dictionary

```python
import pynlm

config_dict = {
    "neuron_count": 5000,
    "simulation_timestep": 0.0005,
    "region_count": 4,
    "log_level": "DEBUG",
    "visualization_enabled": False
}

config = pynlm.createDefaultConfig()
config.loadFromDict(config_dict)
```

### Configuration Validation

```cpp
auto config = std::make_shared<nlm::Config>();
config->loadFromFile("config.json");

auto validation = config->validate();
if (!validation.valid) {
    std::cout << "Configuration validation failed:" << std::endl;
    std::cout << validation.toString() << std::endl;
}
```

### Configuration Summary

```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
print(config.summary())
```

## Basic Usage

### Brain Initialization

```cpp
#include <nlm/Brain.hpp>

// Create configuration
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 2000);
config->set("region_count", 4);

// Create brain
auto brain = std::make_shared<nlm::Brain>(config);

// Initialize
if (!brain->initialize()) {
    std::cerr << "Brain initialization failed!" << std::endl;
    return 1;
}
```

### Running the Simulation

```cpp
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step);
    
    // Log status every 100 steps
    if (step % 100 == 0) {
        brain->logStatus();
    }
}
```

### Sensory Input

```cpp
#include <nlm/sensory/SensoryInput.hpp>

// Create sensory input
auto sensoryInput = std::make_shared<nlm::SensoryInput>();
sensoryInput->setData({0.1, 0.2, 0.3, 0.4, 0.5});

// Inject into brain
brain->receiveSensoryInput(*sensoryInput);
```

### Action Selection

```cpp
auto action = brain->produceAction();
std::cout << "Selected action: " << action->getName() << std::endl;
```

### Saving and Loading

```cpp
// Save brain state
brain->save("brain_state.nlm");

// Load brain state
brain->load("brain_state.nlm");
```

```python
# Python interface
brain.save("brain_state.nlm")
brain.load("brain_state.nlm")
```

## Advanced Features

### Memory Systems

NLM includes comprehensive memory systems:

#### Working Memory

```cpp
auto workingMemory = brain->getWorkingMemory();
workingMemory->setCapacity(1000);
workingMemory->storeToNeuron(neuronId, activation);
workingMemory->update(timestep);
```

#### Episodic Memory

```cpp
auto episodicMemory = brain->getEpisodicMemory();
episodicMemory->setMaxEpisodes(1000);
episodicMemory->storeEpisode(episode);
episodicMemory->replayEpisode(episode);
```

#### Semantic Memory

```cpp
auto semanticMemory = brain->getSemanticMemory();
semanticMemory->storeConcept(concept);
semanticMemory->retrieveConcept(conceptId);
```

### Plasticity Mechanisms

#### Spike-Timing Dependent Plasticity (STDP)

```cpp
auto stdp = brain->getSTDP();
stdp->configure(ltpWeight, ltdWeight, tau);
```

#### Hebbian Learning

```cpp
auto hebbian = brain->getHebbian();
```

#### Structural Plasticity

```cpp
auto structuralPlasticity = brain->getStructuralPlasticity();
structuralPlasticity->setSynaptogenesisRate(rate);
structuralPlasticity->setPruningRate(rate);
```

### Neuromodulation

```cpp
auto dopamine = brain->getDopamine();
auto curiosity = brain->getCuriosity();
auto novelty = brain->getNovelty();
auto predictionError = brain->getPredictionErrorSignal();
```

### Development

```cpp
brain->develop();
 DevelopmentalStage stage = brain->getDevelopmentalStage();
```

### Cognition

```cpp
auto planner = brain->getPlanner();
auto conceptFormation = brain->getConceptFormation();
auto attention = brain->getAttention();
```

## Python Interface

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.getStatistics()
print(stats)

brain.reset()
```

### Enhanced Configuration

```python
# Pythonic configuration
config = pynlm.createConfig()
config.set("neuron_count", 5000)
config.setInt("simulation_timestep", 0.0005)
config.setBool("visualization_enabled", False)

# Load from dictionary
config_dict = {"neuron_count": 2000, "region_count": 4}
config.loadFromDict(config_dict)

# Convert to dictionary
config_dict = config.toDict()
```

### Enhanced Brain Interface

```python
# Enhanced Python brain wrapper
brain = pynlm.createBrain(config)
brain.initialize()

# Flexible step function
brain.step(step)  # Simple step
brain.step(step, time)  # Step with timestamp

# Enhanced sensory input
brain.injectSensoryInput({
    "vision": [0.1, 0.2, 0.3, 0.4, 0.5],
    "touch": [0.0, 0.5, 1.0],
    "internal": [0.5, -0.2, 0.8],
    "proprioception": [1.0, 0.0, 0.5]
})

# Get enhanced statistics
stats = brain.getStatistics()
print(f"Spikes: {stats['total_spikes']}")
print(f"Active neurons: {stats['active_neurons']}")

# Enhanced configuration helper
config_helper = brain.getConfigHelper()
config_helper.set("neuron_count", 1000).set("log_level", "INFO")
```

### Agent Brain Interface

```python
# Create world
world = pynlm.createWorld()
world.configure(100, 100, 16, 16)
world.setAgentStart(50, 50)

# Create agent brain
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Process sensory input
percept_data = {
    "vision": [0.1, 0.5, 0.9] * 16 * 16,
    "touch": [0.0, 0.3, 0.7],
    "internal": [0.5, -0.2, 0.8],
    "proprioception": [1.0, 0.0, 0.5]
}
agent_brain.processSensoryInput(percept_data)

# Decode action
action = agent_brain.decodeMotorCommand()

# Get neuromodulation levels
levels = agent_brain.getNeuromodulationLevels()
print(f"Dopamine: {levels['dopamine']}")
print(f"Curiosity: {levels['curiosity']}")
```

## Command Line Interface

### Basic Usage

```bash
# Help and usage information
./nlm --help
./nlm -h

# Basic usage
./nlm [OPTIONS]

# Advanced usage with scripting
./nlm --config-file=production.json --log-level=DEBUG
```

### Command Line Options

#### Core Simulation Options

| Option | Short Form | Type | Description |
|--------|------------|------|-------------|
| `--neuron_count` | `-nc` | int | Number of neurons in the brain |
| `--region_count` | `-rc` | int | Number of brain regions |
| `--simulation_timestep` | `-st` | double | Simulation timestep in seconds |
| `--random_seed` | `-rs` | int | Random seed for reproducible experiments |
| `--connection_probability` | `-cp` | double | Connection probability for random connectivity |

#### Configuration File Options

| Option | Description |
|--------|-------------|
| `--config-file` | Load configuration from JSON/YAML file |
| `--config-dict` | Load configuration from Python-style dictionary string |

#### Logging Options

| Option | Short Form | Type | Description |
|--------|------------|------|-------------|
| `--log-level` | `-ll` | string | Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL) |
| `--log-to-file` | `-ltf` | bool | Whether to log to file in addition to console |
| `--log-filename` | `-lfn` | string | Filename for log output |

#### Simulation Control Options

| Option | Short Form | Type | Description |
|--------|------------|------|-------------|
| `--max-simulation-steps` | `-mss` | int | Maximum number of simulation steps |
| `--simulation-time-limit` | `-stl` | double | Simulation time limit in seconds |
| `--checkpoint-enabled` | `-ce` | bool | Whether to enable checkpoint saving |
| `--checkpoint-interval` | `-ci` | int | Checkpoint save interval in steps |
| `--checkpoint-dir` | `-cd` | string | Directory for checkpoint files |

### Command Line Examples

```bash
# Basic usage
./nlm --neuron_count=5000 --region_count=4 --log_level=DEBUG

# With configuration file
./nlm --config-file=production.json --log-level=INFO

# Override parameters from config file
./nlm --config-file=base_config.json --neuron_count=10000 --region_count=8

# Environment variable support
export NLM_NEURON_COUNT=15000
export NLM_LOG_LEVEL=WARNING
./nlm
```

## Performance Optimization

### Memory Optimization

#### Memory Pool Allocators

NLM uses custom memory pool allocators for improved performance:

```cpp
// Neuron memory pool
NeuronMemoryPool neuronPool(1000);
Neuron* neuron = neuronPool.allocate();

// Synapse memory pool
SynapseMemoryPool synapsePool(10000);
Synapse* synapse = synapsePool.allocate();
```

### SIMD Optimizations

#### Vectorized Operations

NLM uses SIMD (Single Instruction Multiple Data) optimizations:

```cpp
// SIMD spike detection
__m128 spikes = SIMDUtils::detectSpikesSIMD(neurons, count);

// SIMD weighted sum
float sum = SIMDUtils::simdWeightedSum(weights, inputs, count);
```

### Cache-Friendly Data Structures

#### Cache Optimized Regions

```cpp
// Cache-optimized region access
CacheOptimizedRegion region(neuralRegion);
Neuron* neuron = region.findNeuronById(neuronId);
```

### Lazy Evaluation

#### Deferred Computations

NLM supports lazy evaluation for expensive computations:

```cpp
// Add lazy computation
auto spikeComputation = std::make_unique<LazyComputationEngine::SpikeDetectionComputation>(
    neurons, step, time, firingNeurons, neuronsToSchedule);

pImpl->lazyEngine.addComputation(std::move(spikeComputation));
```

### Parallel Processing

#### Multi-threading Support

NLM provides parallel processing capabilities:

```cpp
// Parallel neuron updates
std::for_each(std::execution::par, neurons.begin(), neurons.end(),
             [](Neuron* neuron) {
                 neuron->stepLIF(currentTime, timestep);
             });
```

## Troubleshooting

### Common Issues

#### 1. Configuration File Not Found

```bash
# Error: Failed to open config file: config.json

Solution:
1. Check that the file exists
2. Verify the file path is correct
3. Ensure the file has read permissions
```

#### 2. Invalid JSON Syntax

```bash
# Error: JSON parsing failed

Solution:
1. Use a JSON validator to check syntax
2. Ensure proper quoting and escaping
3. Check for trailing commas
```

#### 3. Memory Issues

```bash
# Error: Memory allocation failed

Solution:
1. Reduce simulation size (neuron_count, region_count)
2. Use memory pool allocators
3. Check available system memory
```

#### 4. Performance Issues

```bash
# Error: Simulation too slow

Solution:
1. Reduce timestep (simulation_timestep)
2. Use SIMD optimizations
3. Enable checkpointing for periodic saves
```

### Getting Help

```bash
# Get comprehensive help
./nlm --help

# Get specific help sections
./nlm --help core
./nlm --help simulation
./nlm --help logging

# Get examples and tutorials
./nlm --help examples
./nlm --help tutorials
```

### Error Messages

NLM provides detailed error messages:

```
Error: Failed to open config file: production.json
Error: Invalid value '999999' for neuron_count (must be between 100 and 10000)
Error: JSON parsing failed at line 15: unexpected token
Error: Memory allocation failed (out of memory)
Error: Simulation step timeout
```

## Best Practices

### Configuration Best Practices

#### 1. Use Configuration Files

```bash
# Create a base configuration file
./nlm --config-file=production_config.json
```

#### 2. Validate Configuration

```cpp
auto config = std::make_shared<nlm::Config>();
config->loadFromFile("config.json");

auto validation = config->validate();
if (!validation.valid) {
    std::cerr << "Configuration validation failed:" << std::endl;
    std::cerr << validation.toString() << std::endl;
    return 1;
}
```

#### 3. Use Appropriate Timesteps

```cpp
// Good timestep for detailed simulations
config->setDouble("simulation_timestep", 0.0001);

// Good timestep for coarse simulations
config->setDouble("simulation_timestep", 0.01);
```

### Code Best Practices

#### 1. Error Handling

```cpp
try {
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

#### 2. Resource Management

```cpp
// Use smart pointers
auto config = std::make_shared<nlm::Config>();
auto brain = std::make_shared<nlm::Brain>(config);

// Ensure proper cleanup
brain.reset();
config.reset();
```

#### 3. Logging

```cpp
// Configure logging
Logger::getGlobal().setLevel(LogLevel::Info);
Logger::getGlobal().setOutputFile("simulation.log");

// Log important events
NLM_LOG_INFO("Starting simulation");
NLM_LOG_ERROR("Brain initialization failed");
```

### Performance Best Practices

#### 1. Memory Management

```cpp
// Use memory pools for better performance
NeuronMemoryPool pool(1000);

// Clear pools when not needed
pool.clear();
```

#### 2. Cache Optimization

```cpp
// Use cache-friendly data structures
CacheOptimizedRegion region(neuralRegion);
region.prefetchAllNeurons();
```

#### 3. Parallel Processing

```cpp
// Use parallel execution for independent operations
std::for_each(std::execution::par, neurons.begin(), neurons.end(),
             [](Neuron* neuron) {
                 neuron->stepLIF(time, timestep);
             });
```

### Testing Best Practices

#### 1. Unit Testing

```cpp
// Write unit tests for individual components
TEST(BrainTest, Initialization) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    auto brain = std::make_shared<Brain>(config);
    EXPECT_TRUE(brain->initialize());
}
```

#### 2. Integration Testing

```cpp
// Test integration of multiple components
TEST(IntegrationTest, SensorInput) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create and inject sensory input
    auto sensoryInput = std::make_shared<SensoryInput>();
    sensoryInput->setData({0.1, 0.2, 0.3});
    
    brain->receiveSensoryInput(*sensoryInput);
    
    // Verify input was processed
    // Add assertions here
}
```

#### 3. Performance Testing

```cpp
// Test performance
TEST(PerformanceTest, SimulationSpeed) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 10000);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 1000); // Should complete in under 1 second
}
```

## API Reference

### Core Classes

#### Config
```cpp
class Config {
public:
    Config();
    ~Config();
    
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    ValidationResult validate() const;
};
```

#### Brain
```cpp
class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    std::unique_ptr<Action> produceAction();
    void reset();
    
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Memory system accessors
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system accessor
    PredictionSystem* getPredictionSystem();
    
    // Cognition system accessors
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Getters
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    void logStatus() const;
};
```

#### AgentBrain
```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    void initialize(const SimpleWorld& world);
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    void reset();
    
    // Feature toggles
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    // Status checks
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
    std::shared_ptr<Brain> getBrain() const;
};
```

### Enhanced Python Classes

#### PythonConfigHelper
```python
class PythonConfigHelper:
    def get(self, key, default_value=None):
        """Get configuration value with default"""
    
    def has(self, key):
        """Check if key exists"""
    
    def keys(self):
        """Get all keys"""
    
    def set(self, key, value):
        """Set configuration value (fluent API)"""
    
    def loadFromDict(self, dict):
        """Load configuration from Python dictionary"""
    
    def toDict(self):
        """Convert configuration to Python dictionary"""
    
    def validate(self):
        """Validate configuration"""
```

#### PythonBrainWrapper
```python
class PythonBrainWrapper:
    def initialize(self, optional_params=None):
        """Initialize brain with optional parameters"""
    
    def step(self, current_step, current_time=None):
        """Step simulation with flexible input"""
    
    def injectSensoryInput(self, input_data):
        """Inject sensory input from dictionary"""
    
    def getStatistics(self):
        """Get simulation statistics"""
    
    def getConfigHelper(self):
        """Get configuration helper"""
    
    def getBrain(self):
        """Get underlying brain (advanced usage)"""
```

#### EnhancedAgentBrain
```python
class EnhancedAgentBrain:
    def addProcessingStep(self, name, processor, priority=1.0):
        """Add sensory processing step"""
    
    def getProcessingSteps(self):
        """Get processing steps"""
    
    def setActionStrategy(self, strategy_name):
        """Set action selection strategy"""
    
    def getAvailableStrategies(self):
        """Get available strategies"""
    
    def registerNeuromodulator(self, name, baseline, range, modifier):
        """Register neuromodulator"""
    
    def enableNeuromodulator(self, name, enabled):
        """Enable/disable neuromodulator"""
    
    def getNeuromodulatorNames(self):
        """Get neuromodulator names"""
    
    def getPerformanceReport(self):
        """Get performance report"""
    
    def getNeuromodulationStatus(self):
        """Get neuromodulation status"""
    
    def getPluginNames(self):
        """Get available plugins"""
    
    def setCustomMemory(self, key, value):
        """Set custom memory data"""
    
    def getCustomMemory(self, key):
        """Get custom memory data"""
    
    def enableCuriosity(self, enable):
        """Enable curiosity"""
    
    def getEpisodeStatistics(self):
        """Get episode statistics"""
```

### Enumerations

#### NeuronType
```cpp
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};
```

#### SynapseType
```cpp
enum class SynapseType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};
```

#### DevelopmentalStage
```cpp
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};
```

#### FiringState
```cpp
enum class FiringState {
    Resting,
    Active,
    Refractory,
    Inhibited
};
```

#### ActionType
```cpp
enum class ActionType {
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    TurnLeft,
    TurnRight,
    Look,
    LookUp,
    LookDown,
    Interact,
    Eat,
    Drink,
    Rest,
    Wait,
    Custom
};
```

#### MotorCommand
```cpp
enum class MotorCommand {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    LookLeft,
    LookRight,
    Interact,
    Wait
};
```

#### WorldObjectType
```cpp
enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker
};
```

#### DevelopmentalStage
```cpp
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};
```

## Examples and Tutorials

### Tutorial 1: Basic Simulation

#### Step 1: Create Configuration

```cpp
#include <nlm/Config.hpp>

auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);
config->set("simulation_timestep", 0.001);
```

#### Step 2: Create and Initialize Brain

```cpp
auto brain = std::make_shared<nlm::Brain>(config);
if (!brain->initialize()) {
    std::cerr << "Brain initialization failed!" << std::endl;
    return 1;
}
```

#### Step 3: Run Simulation

```cpp
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step);
    
    if (step % 100 == 0) {
        brain->logStatus();
    }
}
```

### Tutorial 2: Python Usage

#### Step 1: Import and Create Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
```

#### Step 2: Create Brain

```python
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Step 3: Run Simulation

```python
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}")
```

### Tutorial 3: Using Memory Systems

#### Working Memory

```cpp
auto workingMemory = brain->getWorkingMemory();
workingMemory->setCapacity(1000);
workingMemory->storeToNeuron(neuronId, activation);
workingMemory->update(timestep);
```

#### Episodic Memory

```cpp
auto episodicMemory = brain->getEpisodicMemory();
episodicMemory->setMaxEpisodes(1000);
episodicMemory->storeEpisode(episode);
episodicMemory->replayEpisode(episode);
```

#### Semantic Memory

```cpp
auto semanticMemory = brain->getSemanticMemory();
semanticMemory->storeConcept(concept);
semanticMemory->retrieveConcept(conceptId);
```

### Tutorial 4: Advanced Configuration

```python
# Advanced configuration with Python
def create_production_config():
    config = pynlm.createConfig()
    
    # Core simulation parameters
    config.set("neuron_count", 10000)
    config.set("region_count", 8)
    config.set("simulation_timestep", 0.0005)
    
    # Plasticity settings
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.025)
    
    # Development settings
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    
    # Neuromodulation settings
    config.set("dopamine_baseline", 0.2)
    config.set("reward_discount_factor", 0.95)
    
    # Logging settings
    config.set("log_level", "INFO")
    config.set("log_to_file", True)
    
    # Simulation settings
    config.set("max_simulation_steps", 50000)
    
    return config

# Use the configuration
production_config = create_production_config()
brain = pynlm.createBrain(production_config)
brain.initialize()
```

### Tutorial 5: Agent Brain Integration

```python
# Create world
world = pynlm.createWorld()
world.configure(100, 100, 16, 16)
world.setAgentStart(50, 50)

# Create agent brain
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Set up action strategy
agent_brain.setActionStrategy("Preference")

# Add sensory processing
agent_brain.addProcessingStep("vision_enhanced",
    lambda percept: print(f"Vision data: {len(percept.getVision())} pixels")
)

# Run simulation
for step in range(100):
    # Process sensory input
    percept_data = {
        "vision": [0.1, 0.5, 0.9] * 16 * 16,
        "touch": [0.0, 0.3, 0.7],
        "internal": [0.5, -0.2, 0.8],
        "proprioception": [1.0, 0.0, 0.5]
    }
    agent_brain.processSensoryInput(percept_data)
    
    # Decode action
    action = agent_brain.decodeMotorCommand()
    
    # Apply reward (in a real scenario)
    # agent_brain.applyRewardModulation(reward, predicted_reward)
    
    # Update world
    world.update(0.1)
    
    # Get status periodically
    if step % 20 == 0:
        status = agent_brain.getNeuromodulationStatus()
        print(f"Status: {status}")

# Get performance report
print(agent_brain.getPerformanceReport())
```

### Example: Batch Processing

```bash
#!/bin/bash

# Batch processing script for multiple configurations
CONFIGS=(
    "configs/small_config.json"
    "configs/medium_config.json"
    "configs/large_config.json"
)

RESULTS_DIR="results/batch"

mkdir -p "$RESULTS_DIR"

for config in "${CONFIGS[@]}"; do
    echo "Processing $config"
    
    # Extract experiment name
    exp_name=$(basename "$config" .json)
    
    # Create output directory
    mkdir -p "$RESULTS_DIR/$exp_name"
    
    # Run simulation
    ./nlm --config-file="$config" \
          --log-to-file=true \
          --log-filename="$RESULTS_DIR/$exp_name/simulation.log"
    
    if [ $? -eq 0 ]; then
        echo "Completed $exp_name"
        mv "final_brain_state.nlm" "$RESULTS_DIR/$exp_name/"
    else
        echo "Failed $exp_name"
        exit 1
    fi
done

echo "Batch processing completed"
```

### Example: Configuration Generator

```python
#!/usr/bin/env python3

import json
import sys

def generate_configuration(base_config, overrides):
    """Generate configuration with overrides"""
    config = base_config.copy()
    config.update(overrides)
    return config

# Base configuration
base_config = {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "log_level": "INFO",
    "max_simulation_steps": 10000
}

# Experiment variations
experiments = {
    "small": {"neuron_count": 100, "max_simulation_steps": 100},
    "medium": {"neuron_count": 1000, "max_simulation_steps": 1000},
    "large": {"neuron_count": 10000, "max_simulation_steps": 10000},
    "detailed": {"simulation_timestep": 0.0001, "log_level": "DEBUG"}
}

# Generate configurations
for name, overrides in experiments.items():
    config = generate_configuration(base_config, overrides)
    
    # Save to file
    with open(f"configs/{name}_config.json", "w") as f:
        json.dump(config, f, indent=2)
    
    print(f"Generated {name} configuration")

print("Configuration generation completed")
```

## Conclusion

The NLM framework provides a comprehensive artificial brain simulation system with:

- **Comprehensive neural computation**: LIF neurons with spike-based propagation
- **Memory systems**: Working, episodic, semantic, procedural, and associative memory
- **Plasticity mechanisms**: STDP, Hebbian learning, and structural plasticity
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Cognitive systems**: Planning, concept formation, and attention
- **Sensory-motor loop**: Vision, touch, internal signals, and action selection
- **Advanced configuration**: JSON/YAML support with schema validation
- **Performance optimizations**: Memory pools, SIMD, lazy evaluation, and parallel processing
- **Modularity and extensibility**: Plugin system and strategy pattern
- **Python interface**: Enhanced Python bindings with fluent API
- **Command line interface**: Rich CLI with configuration management

The NLM framework is designed for both beginners and advanced users, providing a balance between simplicity and power. Whether you're running a simple simulation or building complex research experiments, NLM provides the tools you need.

For more information, visit the NLM documentation website or join the NLM community on GitHub.

---

*This documentation was generated with the NLM documentation system.*

Questions? Feedback? Comments?

Visit: https://nlm.readthedocs.io

Join the community: https://github.com/nlm-project/nlm/discussions

Report issues: https://github.com/nlm-project/nlm/issues
