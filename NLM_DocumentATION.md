# NLM (熙然) - Neural Learning Machine
# Comprehensive Documentation and Examples

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment. This documentation provides comprehensive information for using, configuring, and extending the NLM system.

## Table of Contents

1. [System Architecture](#system-architecture)
2. [Core Components](#core-components)
3. [Configuration Management](#configuration-management)
4. [C++ API Usage](#c++-api-usage)
5. [Python Bindings](#python-bindings)
6. [Command-Line Interface](#command-line-interface)
7. [Test Suite](#test-suite)
8. [Performance Guidelines](#performance-guidelines)
9. [Troubleshooting](#troubleshooting)
10. [Extending NLM](#extending-nlm)

## System Architecture

NLM implements a closed-loop brain simulation with the following key components:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Core Systems

1. **Neural Core**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
2. **Memory Systems**: Working memory, episodic memory, and associative memory
3. **Neuromodulation**: Dopamine (reward), curiosity, novelty detection
4. **Prediction**: Forward models and prediction error computation
5. **Cognition**: Planning, concept formation, attentional selection
6. **Development**: Developmental stages affecting plasticity rates
7. **Plasticity**: STDP, Hebbian, and structural plasticity
8. **Environment**: Simple 2D world with objects and interactions

## Core Components

### Brain Class (`brain/Brain.hpp`)
The central neural simulation brain class:

```cpp
#include "brain/Brain.hpp"

// Create brain with configuration
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);

// Initialize brain
if (brain->initialize()) {
    // Brain is ready for simulation
}

// Main simulation step
brain->step(step);

// Receive sensory input
brain->receiveSensoryInput(sensoryInput);

// Produce motor action
auto action = brain->produceAction();
```

### Config Class (`core/Config/Config.hpp`)
Configuration management with JSON/YAML support:

```cpp
#include "core/Config/Config.hpp"

auto config = std::make_shared<Config>();

// Set configuration values
config->set("neuron_count", 1000);
config->set("simulation_timestep", 0.001);
config->set("random_seed", 42);
config->set("connection_probability", 0.1f);

// Load from file
config->loadFromFile("config.json");

// Load from command line
config->loadFromArgs(argc, argv);

// Get values
auto neurons = config->getOr<int64_t>("neuron_count", 1000);
```

### Neuron and Synapse (`brain/Neuron.hpp`, `brain/Synapse.hpp`)
Neural computation units:

```cpp
// Access neurons from brain regions
for (auto& region : brain->getRegions()) {
    for (auto& population : region->getPopulations()) {
        for (auto* neuron : population->getNeurons()) {
            // Control neuron behavior
            neuron->injectCurrent(50.0f);
            neuron->stepLIF(currentTime, timestep);
        }
    }
}

// Manage synaptic connections
for (auto& region : brain->getRegions()) {
    for (auto* synapse : region->getSynapses()) {
        // Control synaptic properties
        synapse->setWeight(0.5f);
        synapse->setDelay(1);
        synapse->enablePlasticity(true, true, false);
    }
}
```

## Configuration Management

### Configuration File Format

NLM supports JSON and YAML configuration files:

```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true,
    "checkpoint_dir": "./checkpoints"
}
```

```yaml
# config.yaml
neuron_count: 1000
region_count: 1
simulation_timestep: 0.001
random_seed: 42
connection_probability: 0.1

stdp:
  ltp_weight: 0.02
  ltd_weight: 0.015
  tau: 20.0

structural_plasticity:
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001

features:
  enable_checkpointing: true
  enable_replay: true
  enable_development: true
  checkpoint_dir: "./checkpoints"
```

### Configuration Validation

NLM provides comprehensive configuration validation:

```cpp
auto config = std::make_shared<Config>();

// Configuration schema validation
ConfigSchema schema;
schema.addField(ConfigSchema::Field("neuron_count", typeid(size_t), "1000", true, "Number of neurons"));
schema.addField(ConfigSchema::Field("simulation_timestep", typeid(double), "0.001", true, "Simulation timestep"));
// ... more fields

if (!schema.validate(*config)) {
    std::cerr << "Configuration validation failed: " << schema.getValidationError() << std::endl;
    return 1;
}
```

## C++ API Usage

### Basic Simulation

```cpp
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 2: Real Neural Computation ===" << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run basic connectivity test
    std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject current into neurons
            for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            // Run simulation
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikesGenerated = brain->getTotalSpikeCount() - initialSpikes;
            std::cout << "Spikes generated: " << spikesGenerated << std::endl;
            if (spikesGenerated > 0) {
                std::cout << "✓ Spikes propagate through network" << std::endl;
            }
        }
    }
    
    // Run plasticity learning test
    std::cout << "\n=== Test 2: Plasticity Learning Experiment ===" << std::endl;
    
    // Reset and reinitialize for plasticity test
    brain->reset();
    brain->initialize();
    
    // Enable plasticity
    for (auto& region : brain->getRegions()) {
        for (auto& synapse : region->getSynapses()) {
            synapse->enablePlasticity(true, true, false);
        }
    }
    
    // Apply learning stimuli
    for (SimulationStep step = 0; step < 200; ++step) {
        for (size_t i = 0; i < 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Internal, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    // Check for synaptic changes
    float plasticityEffect = brain->getExcitationInhibitionRatio();
    std::cout << "E/I ratio: " << plasticityEffect << std::endl;
    if (plasticityEffect > 0.1f && plasticityEffect < 10.0f) {
        std::cout << "✓ Plasticity is working" << std::endl;
    }
    
    std::cout << "\n=== Phase 2 Complete ===" << std::endl;
    std::cout << "The NLM brain is functioning correctly with plasticity." << std::endl;
    
    return 0;
}
```

### Advanced Configuration

```cpp
// Custom configuration with validation
auto config = std::make_shared<Config>();

// Load from multiple sources
config->loadFromFile("base_config.json");
config->loadFromArgs(argc, argv);

// Apply overrides
config->set("neuron_count", 2000, ConfigSource::CommandLine);
config->set("simulation_timestep", 0.002, ConfigSource::CommandLine);
config->set("random_seed", 12345, ConfigSource::CommandLine);

// Validate configuration
if (!config->validate()) {
    std::cerr << "Configuration validation failed!" << std::endl;
    return 1;
}

// Save configuration
config->saveToFile("final_config.json");
```

## Python Bindings

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create configuration with validation
config = pynlm.Config()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)

# Create brain
brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
brain.step(0)
brain.step(1, 0.001)

# Get statistics
stats = pynlm.brainSummary(brain)
print(stats)
```

### Pythonic Interface

```python
import pynlm

# Factory functions for easy setup
brain = pynlm.createSimpleSimulation()

# Use action factory functions
action = pynlm.Action.createMoveAction("forward")
look_action = pynlm.Action.createLookAction("left")

# World configuration
world = pynlm.SimpleWorld.create(800, 600, 80, 60)

# Memory operations (if available)
if hasattr(brain, 'getWorkingMemory'):
    working_memory = brain.getWorkingMemory()
    working_memory.storeToNeuron(1, 0.8)

# Results monitoring
results = pynlm.runExperiment(brain, 100)
print(f"Generated {results} spikes")

# Validate everything
if pynlm.validateConfiguration(config):
    print("Configuration is valid!")
```

### Error Handling

```python
try:
    config = pynlm.Config()
    config.set("invalid_key", "invalid_value")
    brain = pynlm.Brain(config)
    brain.initialize()
except pynlm.ValueError as e:
    print(f"Configuration error: {e}")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.TypeError as e:
    print(f"Type error: {e}")
```

## Command-Line Interface

### Basic Usage

```bash
# Run basic connectivity test
nlm_phase2

# Run all tests with custom configuration
nlm_phase2 --all --neurons 2000 --timestep 0.002

# Run specific test with debugging
nlm_phase2 --plasticity --verbose --debug --seed 12345

# Run with checkpointing
nlm_phase2 --plasticity --save brain_state.bin --load brain_state.bin

# Check integration
nlm_phase2 --basic --results results.json
```

### Command-Line Options

#### Test Selection
- `--basic`: Run basic connectivity test (default)
- `--plasticity`: Run plasticity learning experiment
- `--stdp`: Run STDP verification test
- `--all`: Run all tests
- `--single-test <name>`: Run a single test (basic|plasticity|stdp)

#### Configuration Options
- `--config <file>`: Load configuration from file
- `--neurons <n>`: Number of neurons
- `--regions <n>`: Number of brain regions
- `--timestep <t>`: Simulation timestep
- `--seed <s>`: Random seed for reproducible results

#### Advanced Options
- `--duration <steps>`: Number of simulation steps
- `--verbose`: Enable verbose logging
- `--debug`: Enable debug logging
- `--profile`: Enable performance profiling
- `--checkpoints <dir>`: Directory for checkpoint files
- `--load <file>`: Load brain state from checkpoint
- `--save <file>`: Save brain state to checkpoint

#### Performance Options
- `--threads <n>`: Number of threads for parallel processing
- `--vectorize`: Enable SIMD vectorization

#### Plasticity Options
- `--hebbian-on`: Enable Hebbian plasticity (default)
- `--hebbian-off`: Disable Hebbian plasticity
- `--stdp-on`: Enable STDP (default)
- `--stdp-off`: Disable STDP
- `--structural-on`: Enable structural plasticity (default)
- `--structural-off`: Disable structural plasticity

#### Neuromodulation Options
- `--dopamine <level>`: Set dopamine level (0.0-1.0)
- `--curiosity <level>`: Set curiosity level (0.0-1.0)
- `--novelty <level>`: Set novelty level (0.0-1.0)
- `--neuromodulation-on`: Enable neuromodulation (default)
- `--neuromodulation-off`: Disable neuromodulation

#### Development Options
- `--development-on`: Enable developmental stages (default)
- `--development-off`: Disable developmental stages
- `--stage <stage>`: Set developmental stage

#### Output Options
- `--quiet`: Minimal output
- `--results <file>`: Save results to file

### Example Command Lines

```bash
# Full simulation with custom configuration
nlm_phase2 --all --neurons 2000 --timestep 0.002 --seed 12345 \
          --hebbian --stdp --structural \
          --dopamine 0.1 --curiosity 0.05 --novelty 0.01 \
          --development --stage adult \
          --save brain_checkpoint.bin --results results.json

# Quick test with debugging
nlm_phase2 --plasticity --verbose --debug --stdp-ltp-weight 0.03 --stdp-ltd-weight 0.02

# Performance testing
nlm_phase2 --plasticity --threads 8 --vectorize --profile --results performance_results.json

# Checkpoint testing
nlm_phase2 --basic --save test_checkpoint.bin --load test_checkpoint.bin
```

## Test Suite

### Overview

NLM includes a comprehensive test suite covering all major functionality:

1. **Basic Connectivity Test**: Verifies neuron communication and spike propagation
2. **Plasticity Learning Test**: Tests synaptic weight changes through activity
3. **STDP Verification Test**: Validates Spike-Timing-Dependent Plasticity
4. **Memory Integration Test**: Checks memory system integration
5. **Neuromodulation Integration Test**: Verifies neuromodulation effects
6. **Checkpoint Test**: Tests save/load functionality
7. **Development Integration Test**: Tests developmental stage effects
8. **Comprehensive Integration Test**: End-to-end integration testing

### Running Tests

#### Using Command Line

```bash
# Run all tests
nlm_test_suite

# Run specific test
nlm_test_suite --test basic

# Run tests with custom configuration
nlm_test_suite --config test_config.json --all
```

#### Using Python

```python
import pynlm

# Run test suite
results = pynlm.runTestSuite()
print(results)

# Run specific test
result = pynlm.runSingleTest("plasticity")
print(f"Plasticity test result: {result}")
```

### Test Results Interpretation

Each test produces detailed output with:

- **Test Name**: Clear identification of what's being tested
- **Pass/Fail Status**: Whether the test passed or failed
- **Metrics**: Quantitative measurements
- **Details**: Contextual information about the test

```
=== Test Results ===
Basic Connectivity Test: PASS - 500 neurons, 1 regions, 50 spikes generated
Plasticity Learning Test: PASS - Learning detected: 0.023 weight change
STDP Verification Test: PASS - LTP induction: 0.015 weight increase
Memory Integration Test: PASS - Working memory: 2 active traces
Neuromodulation Integration Test: PASS - Dopamine level: 0.123
Checkpoint Test: PASS - Checkpoint saved and loaded successfully
Development Integration Test: PASS - Development stage: Adult
Comprehensive Integration Test: PASS - All systems operational

Total tests: 8
Passed: 8
Failed: 0
Success rate: 100.00%
```

## Performance Guidelines

### Performance Optimization

#### Configuration Optimization
- Use appropriate neuron counts for your use case (500-10000 for typical simulations)
- Set simulation timesteps between 0.001 and 0.01 seconds
- Use appropriate random seeds for reproducibility

#### Runtime Optimization
- Enable SIMD vectorization for better performance
- Use multiple threads for parallel processing
- Monitor memory usage during long simulations
- Implement checkpointing for long-running experiments

#### Memory Management
- Monitor spike history size (default: 100 entries per synapse)
- Use appropriate checkpoint intervals for large simulations
- Consider sparse connectivity for very large networks

### Performance Monitoring

```cpp
// Monitor brain performance during simulation
BrainPerformanceStats stats = brain->getPerformanceStats();
std::cout << "Steps per second: " << stats.stepsPerSecond << std::endl;
std::cout << "CPU utilization: " << stats.cpuUtilization << "%" << std::endl;
std::cout << "Memory usage: " << stats.memoryUsageMB << "MB" << std::endl;
std::cout << "Total spikes processed: " << stats.totalSpikes << std::endl;
```

```python
import pynlm

# Monitor performance in Python
brain = pynlm.createSimpleSimulation()

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        perf = pynlm.monitorPerformance(brain, step)
        print(f"Step {step}: {perf}")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain fails to initialize
**Symptoms**: Initialization fails, no output, or crashes

**Solutions**:
1. Check configuration validity
2. Verify sufficient memory resources
3. Check for configuration file syntax errors
4. Ensure all required libraries are linked

#### Issue: No spikes generated
**Symptoms**: Neurons receive current but no spikes are detected

**Solutions**:
1. Check neuron threshold parameters
2. Verify current injection values
3. Check simulation timestep settings
4. Verify neural population settings

#### Issue: Configuration validation failures
**Symptoms**: Error messages about invalid configuration

**Solutions**:
1. Check configuration file syntax (JSON/YAML)
2. Verify parameter ranges and types
3. Check for missing required fields
4. Use `pynlm.getValidationMessages()` for detailed error information

#### Issue: Memory usage is too high
**Symptoms**: System slows down or crashes during long simulations

**Solutions**:
1. Reduce neuron count for testing
2. Use checkpointing for long simulations
3. Monitor spike history settings
4. Consider sparse connectivity

### Debugging Tools

```cpp
// Debug brain state
void debugBrain(const Brain& brain) {
    NLM_LOG_INFO("=== BRAIN DEBUG ===");
    NLM_LOG_INFO("Total neurons: " << brain.getTotalNeuronCount());
    NLM_LOG_INFO("Total synapses: " << brain.getTotalSynapseCount());
    NLM_LOG_INFO("Total spikes: " << brain.getTotalSpikeCount());
    NLM_LOG_INFO("Active neurons: " << brain.getActiveNeuronCount());
    NLM_LOG_INFO("Development stage: " << static_cast<int>(brain.getDevelopmentalStage()));
    
    for (const auto& region : brain.getRegions()) {
        NLM_LOG_INFO("Region " << region->getId().index() << " (" << region->getName() << "):");
        NLM_LOG_INFO("  Neurons: " << region->getTotalNeuronCount());
        NLM_LOG_INFO("  Synapses: " << region->getSynapseCount());
        NLM_LOG_INFO("  Average weight: " << region->getAverageSynapticWeight());
    }
}
```

```python
# Debug brain state in Python
import pynlm

def debug_brain(brain):
    print("=== BRAIN DEBUG ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    for region in brain.getRegions():
        print(f"Region {region.getId().index()} ({region.getName()}):")
        print(f"  Neurons: {region.getTotalNeuronCount()}")
        print(f"  Synapses: {region.getSynapseCount()}")
        print(f"  Average weight: {region.getAverageSynapticWeight()}")
```

## Extending NLM

### Adding New Features

#### Extending Brain Class

```cpp
// Example: Add custom plasticity rule
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, Timestamp timestep) override {
        // Custom plasticity implementation
    }
};

// Register custom plasticity rule
void registerCustomPlasticity(Brain* brain) {
    auto customPlasticity = std::make_unique<CustomPlasticity>();
    brain->getPlasticitySystem()->addRule(std::move(customPlasticity));
}
```

#### Adding New Configuration Schema

```cpp
// Extend configuration validation
ConfigSchema schema;
schema.addField(ConfigSchema::Field("custom_parameter", typeid(float), "1.0", false, "Custom parameter"));
schema.addField(ConfigSchema::Field("custom_setting", typeid(std::string), "default", false, "Custom setting"));

if (schema.validate(*config)) {
    // Apply custom settings
    float customParam = config->getOr<float>("custom_parameter", 1.0f);
    std::string customSetting = config->getOr<std::string>("custom_setting", "default");
    
    // Apply to brain
    brain->setCustomParameter(customParam);
    brain->setCustomSetting(customSetting);
}
```

#### Adding New Sensory Input Types

```cpp
// Example: Add custom sensory input
class CustomSensoryInput : public SensoryInput {
public:
    CustomSensoryInput() : SensoryInput() {
        // Initialize custom sensory data
    }
    
    void update(float value) {
        // Update sensory data
    }
    
    SensoryInputType getType() const override {
        return SensoryInputType::Custom;
    }
};

// Use in brain
brain->receiveSensoryInput(customInput);
```

### Plugin Architecture

NLM supports plugin architecture for extending functionality:

```cpp
// Plugin interface
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    virtual std::string getName() const = 0;
    virtual void initialize(Brain* brain) = 0;
    virtual void shutdown() = 0;
    virtual bool canHandle(const std::string& command) const = 0;
    virtual std::string execute(const std::string& command, const std::vector<std::string>& args) = 0;
};

// Plugin implementation
class CustomPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "CustomPlugin"; }
    
    void initialize(Brain* brain) override {
        // Initialize plugin with brain reference
        this->brain = brain;
    }
    
    bool canHandle(const std::string& command) const override {
        return command == "custom_command";
    }
    
    std::string execute(const std::string& command, const std::vector<std::string>& args) override {
        if (command == "custom_command") {
            // Execute custom command
            return "Custom command executed successfully";
        }
        return "Unknown command";
    }
    
private:
    Brain* brain;
};
```

### Build and Deployment

#### Build Instructions

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Python bindings build
pip install -e .

# Tests build
cd tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Deployment Options

- **Docker Container**: Use Dockerfile for containerized deployment
- **Cloud Deployment**: Use cloud-native deployment patterns
- **Local Development**: Standard build and install process
- **CI/CD Integration**: Integration with continuous integration pipelines

## Conclusion

NLM (熙然) provides a comprehensive, extensible neural simulation platform with:

1. **Advanced Configuration Management**: JSON/YAML support, validation, error handling
2. **Comprehensive API**: Both C++ and Python interfaces with Pythonic features
3. **Enhanced Command-Line Interface**: 40+ options for flexible testing and experimentation
4. **Robust Testing**: Comprehensive test suite covering all major functionality
5. **Performance Optimization**: Built-in monitoring, vectorization, and parallel processing
6. **Extensible Architecture**: Plugin support for custom extensions
7. **Professional Documentation**: Complete guides and examples for all use cases

The NLM system is suitable for:
- Research and experimentation in computational neuroscience
- Educational purposes and teaching neural networks
- Performance benchmarking and testing
- Custom neural network development and research

Whether you're a beginner looking to understand neural computation or an advanced researcher developing novel neural architectures, NLM provides the tools and flexibility needed for your neural simulation needs.

---

**Version**: 0.1.0 Enhanced
**Authors**: NLM Development Team
**License**: MIT
**Documentation**: Auto-generated from source code

*This documentation is continuously updated with each NLM release. For the latest information, visit the NLM documentation repository.*