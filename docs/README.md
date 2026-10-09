# NLM User Guide: Clear Examples and Tutorials

## Table of Contents

1. [Quick Start](#quick-start)
2. [Basic Usage Examples](#basic-usage-examples)
3. [Python Examples](#python-examples)
4. [Configuration Examples](#configuration-examples)
5. [Command-Line Interface](#command-line-interface)
6. [Advanced Topics](#advanced-topics)
7. [Troubleshooting](#troubleshooting)

## Quick Start

This guide provides clear, step-by-step examples for getting started with the NLM (Neural Learning Machine) brain simulation system.

### What You'll Learn

- How to create and run basic neural simulations
- How to use the command-line interface
- How to work with Python bindings
- How to configure and customize simulations
- How to analyze and interpret results

### Prerequisites

Before you begin, make sure you have:

1. **NLM compiled and installed** (see documentation for build instructions)
2. **Basic understanding of neural networks** (optional but helpful)
3. **C++ and Python knowledge** (for advanced features)

## Basic Usage Examples

### Example 1: Simple Neural Simulation

This example demonstrates how to run a basic neural simulation using the command-line interface.

```bash
# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run basic connectivity test
./nlm --connectivity-test

# Run plasticity learning experiment
./nlm --plasticity-test

# Run STDP verification
./nlm --stdp-test

# Run all Phase 2 tests
./nlm --phase2
```

**What this does:**
- Creates a neural network with 500 neurons
- Tests basic neural connectivity by injecting current into neurons
- Demonstrates synaptic plasticity through learning experiments
- Verifies STDP (Spike-Timing Dependent Plasticity) mechanisms

### Example 2: Configuration Management

This example shows how to create and use custom configurations.

```bash
# Create a custom configuration file
cat > my_config.cfg << EOF
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.2,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.02,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002
}
EOF

# Run simulation with custom configuration
./nlm --config=my_config.cfg --steps=5000
```

**What this does:**
- Creates a larger neural network (1000 neurons)
- Uses two brain regions for more complex processing
- Configures synaptic plasticity parameters
- Runs a longer simulation (5000 steps)

### Example 3: Development and Aging

This example demonstrates how to simulate brain development and aging.

```bash
# Run development simulation
./nlm \
    --neuron-count=800 \
    --steps=10000 \
    --verbose \
    --config=development_config.cfg

# Development configuration example (development_config.cfg)
{
    "neuron_count": 800,
    "region_count": 1,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "developmental_stage": "adult",
    "plasticity_decay_rate": 0.01,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

**What this does:**
- Simulates a developing brain with 800 neurons
- Configures developmental parameters
- Runs for 10,000 steps to observe development changes
- Shows how brain properties change over time

## Python Examples

The NLM Python bindings provide a convenient interface for working with neural simulations.

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a simulation
for step in range(1000):
    brain.step(step)

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")

print("Simulation completed successfully!")
```

**What this does:**
- Creates a brain with default configuration (500 neurons)
- Initializes the brain
- Runs 1000 simulation steps
- Prints brain statistics
- Demonstrates basic Python API usage

### Example 2: Complex Simulation with World and Agent

```python
import pynlm
import numpy as np

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure the world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10, 10)
world.reset()

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(2000):
    # Update world
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"Reward: {world.computeReward(brain.getRegions()[0].get())}")

print("Simulation completed!")
print(f"Final firing rate: {brain.getAverageFiringRate()} Hz")
print(f"Total steps: {world.getSimulationTime():.1f}")
```

**What this does:**
- Creates a complete brain-world-agent system
- Configures a 20x20 world with vision
- Enables all learning features
- Runs a complex simulation with perception-action loop
- Shows how agent interacts with environment
- Demonstrates sensory-motor integration

### Example 3: Advanced Brain Inspection

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for i in range(500):
    brain.step(i)

# Inspect brain state using advanced utilities
print("=== Brain State Inspection ===")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
print(f"Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Get comprehensive brain statistics
stats = brain.getBrainStats()
print("\n=== Brain Statistics ===")
print(stats)

# Create a brain snapshot
snapshot = brain.createSnapshot()
print(f"\nBrain snapshot created with {snapshot.getTotalNeuronCount()} neurons")

# Export brain state
brain.exportBrainState("brain_state.bin")
print("Brain state exported to brain_state.bin")

print("\n=== Inspection Complete ===")
```

**What this does:**
- Creates a comprehensive brain state inspection
- Shows how to access brain statistics
- Demonstrates snapshot creation
- Shows brain state export
- Illustrates advanced inspection capabilities

## Configuration Examples

### Example 1: Learning Configuration

```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_development": true,
    "enable_structural_plasticity": true,
    "working_memory_size": 100,
    "episodic_memory_capacity": 50,
    "prediction_horizon": 10,
    "planning_horizon": 5
}
```

**What this does:**
- Configures a learning-focused brain
- Enables all neuromodulation systems
- Sets up memory systems with reasonable capacities
- Configures prediction and planning systems

### Example 2: Performance Configuration

```json
{
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.1,
    "simulation_timestep": 0.002,
    "random_seed": 123,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.008,
    "stdp_tau": 15.0,
    "synaptogenesis_rate": 0.00005,
    "pruning_rate": 0.000005,
    "enable_reward_modulation": false,
    "enable_curiosity": false,
    "enable_development": false,
    "enable_structural_plasticity": false,
    "working_memory_size": 50,
    "episodic_memory_capacity": 25,
    "prediction_horizon": 5,
    "planning_horizon": 3,
    "performance_mode": "high_speed"
}
```

**What this does:**
- Configures a performance-optimized brain
- Reduces memory system complexity
- Disables neuromodulation for faster simulation
- Enables high-speed performance mode

### Example 3: Research Configuration

```json
{
    "neuron_count": 2000,
    "region_count": 2,
    "connection_probability": 0.2,
    "simulation_timestep": 0.0005,
    "random_seed": 999,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.025,
    "stdp_tau": 25.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_development": true,
    "enable_structural_plasticity": true,
    "working_memory_size": 200,
    "episodic_memory_capacity": 100,
    "prediction_horizon": 20,
    "planning_horizon": 10,
    "novelty_detection": true,
    "social_learning": true,
    "meta_plasticity": true,
    "research_mode": "comprehensive"
}
```

**What this does:**
- Configures a research-grade brain with extensive features
- Uses two brain regions for more complex processing
- Enables all neuromodulation and learning systems
- Configures advanced research features
- Sets up comprehensive memory and prediction systems

## Command-Line Interface

### Basic Usage

```bash
nlm [OPTIONS]
```

### Available Options

#### Run Mode Options
- `--help, -h`: Show help message
- `--phase2, -2`: Run Phase 2 basic tests (default)
- `--phase6-demo, -6d`: Run Phase 6 integration demo
- `--phase6-integration, -6i`: Run full Phase 6 integration
- `--connectivity-test, -c`: Test basic neural connectivity
- `--plasticity-test, -p`: Test synaptic plasticity
- `--stdp-test, -s`: Test spike-timing dependent plasticity

#### Configuration Options
- `--config=<file>`: Load configuration from file
- `--neuron-count=<n>`: Set number of neurons (default: 500)
- `--steps=<n>`: Set number of simulation steps (default: 1000)
- `--verbose, -v`: Enable verbose logging
- `--quiet, -q`: Suppress logging output

#### Advanced Options
- `--version`: Show version information
- `--list-configs`: List available configuration templates
- `--benchmark`: Run performance benchmark
- `--validate-config=<file>`: Validate configuration file

### Example Command Line Usage

```bash
# Run help to see all options
nlm --help

# Run basic connectivity test with custom configuration
nlm --connectivity-test --config=my_config.cfg --verbose

# Run plasticity experiment with specific parameters
nlm --plasticity-test --neuron-count=800 --steps=3000

# Run Phase 6 demo with performance settings
nlm --phase6-demo --config=performance_config.cfg

# Run STDP test with quiet mode
nlm --stdp-test --quiet

# List available configuration templates
nlm --list-configs

# Validate a configuration file
nlm --validate-config=my_config.cfg
```

## Advanced Topics

### Working with Configurations

#### Loading and Saving Configurations

```python
import pynlm
import json

# Create and save a configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("connection_probability", 0.12)
config.saveToFile("my_config.json")

# Load a configuration
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")

# Get configuration values
print(f"Neuron count: {loaded_config.getOr('neuron_count', 500)}")
print(f"Connection probability: {loaded_config.getOr('connection_probability', 0.15)}")

# Export configuration as Python dict
config_dict = loaded_config.exportToPython()
print(f"Configuration dict: {config_dict}")

# Import configuration from dict
config_from_dict = pynlm.createDefaultConfig()
config_from_dict.importFromJson(json.dumps(config_dict))
```

#### Configuration Validation

```python
import pynlm

# Create a configuration
config = pynlm.createDefaultConfig()

# Validate configuration
is_valid = config.validate()
print(f"Configuration valid: {is_valid}")

# Get validation report
report = config.getValidationReport()
print(f"Validation report: {report}")

# Fix common issues
fixed = config.fixCommonIssues()
print(f"Fixed: {fixed}")
```

### Brain Inspection and Analysis

#### Advanced Brain Statistics

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

# Get detailed brain statistics
stats = brain.getBrainStats()
print(stats)

# Inspect brain components
print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 'N/A'}")
print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 'N/A'}")
print(f"Dopamine level: {brain.getDopamine() if brain.getDopamine() else 'N/A'}")
print(f"Curiosity level: {brain.getCuriosity() if brain.getCuriosity() else 'N/A'}")
print(f"Novelty level: {brain.getNovelty() if brain.getNovelty() else 'N/A'}")

# Get neurons by type
excitatory_neurons = brain.getNeuronsByType("Excitatory")
print(f"Excitatory neurons: {len(excitatory_neurons)}")

inhibitory_neurons = brain.getNeuronsByType("Inhibitory")
print(f"Inhibitory neurons: {len(inhibitory_neurons)}")

# Get plasticity rules
rules = brain.listAllPlasticityRules()
print(f"Available plasticity rules: {rules}")
```

#### Agent Control and Management

```python
import pynlm

# Create brain, world, and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.setAgentStart(7, 7)
world.reset()

# Initialize agent
agent.initialize(world)

# Manage features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Check feature status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"Development enabled: {agent.isDevelopmentEnabled()}")

# Get agent body state
body = agent.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent orientation: {body.orientation}")
print(f"Agent velocity: ({body.velocityX}, {body.velocityY})")
print(f"Agent energy: {body.energy}")
print(f"Agent health: {body.health}")
print(f"Agent age: {body.age}")

# Enable all features
agent.enableAllFeatures()

# Disable all features
agent.disableAllFeatures()
```

### Configuration Management

#### Preset Configuration Management

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create configurations for different use cases
# Note: These would need to be implemented in the Config class
print("Configuration presets would include:")
print("1. Development configuration for studying brain development")
print("2. Performance configuration for high-speed simulation")
print("3. Research configuration for comprehensive studies")
print("4. Minimal configuration for testing")
print("5. Large-scale configuration for extensive simulations")

# Example: Get recommended configuration for specific use case
recommended = pynlm.getRecommendedConfig("learning")
print(f"\nRecommended configuration for learning:\n{recommended}")
```

#### Configuration Comparison and Merging

```python
import pynlm

# Create two configurations
config1 = pynlm.createDefaultConfig()
config1.set("neuron_count", 500)
config1.set("connection_probability", 0.15)

config2 = pynlm.createDefaultConfig()
config2.set("neuron_count", 1000)
config2.set("connection_probability", 0.2)
config2.set("stdp_ltp_weight", 0.03)

# Merge configurations (config2 overrides config1)
merged = config1.mergeWith(config2)
print(f"Merged neuron count: {merged.getOr('neuron_count', 500)}")
print(f"Merged connection probability: {merged.getOr('connection_probability', 0.15)}")
print(f"Merged STDP LTP weight: {merged.getOr('stdp_ltp_weight', 0.02)}")

# Compare configurations
differences = config1.compareWith(config2)
print(f"\nDifferences between configurations:\n{differences}")

# Interpolate configurations
interpolated = config1.interpolateWith(config2, 0.5)
print(f"\nInterpolated neuron count: {interpolated.getOr('neuron_count', 500)}")
print(f"Interpolated connection probability: {interpolated.getOr('connection_probability', 0.15)}")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Brain Initialization Fails

**Problem:** Brain initialization fails with error "Failed to initialize brain!"

**Solutions:**
1. Check that configuration values are valid
2. Ensure required configuration parameters are set
3. Check memory availability
4. Try with smaller configuration

**Example:**
```python
# Create a minimal configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Reduce neuron count
config.set("region_count", 1)

brain = pynlm.createBrain(config)
if (!brain.initialize()) {
    std::cerr << "Failed to initialize brain with minimal config!";
    return 1;
}
```

#### Issue 2: Simulation Performance is Slow

**Problem:** Simulation runs too slowly

**Solutions:**
1. Reduce number of neurons
2. Disable learning features
3. Increase simulation timestep
4. Use performance configuration

**Example:**
```python
# Create performance configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("simulation_timestep", 0.005)
config.set("enable_reward_modulation", false)
config.set("enable_curiosity", false)
config.set("enable_development", false)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Issue 3: Python Bindings Not Working

**Problem:** Python bindings not importing correctly

**Solutions:**
1. Check that NLM is properly built with Python bindings
2. Verify Python path includes NLM module
3. Try importing with explicit module path
4. Check for compilation errors

**Example:**
```python
import sys
sys.path.append("/path/to/nlm/python")

import pynlm
print(f"NLM version: {pynlm.__version__}")
print(f"Available classes: {dir(pynlm)}")
```

#### Issue 4: Configuration File Not Found

**Problem:** Configuration file not found when loading

**Solutions:**
1. Use absolute path for configuration file
2. Ensure configuration file exists in working directory
3. Check file permissions
4. Use relative path correctly

**Example:**
```python
# Use absolute path
config.loadFromFile("/home/user/nlm_config.json")

# Use relative path
config.loadFromFile("config/my_config.json")

# Check if file exists
import os
if os.path.exists("config/my_config.json"):
    config.loadFromFile("config/my_config.json")
else:
    print("Configuration file not found!")
```

#### Issue 5: Memory Usage Too High

**Problem:** Memory usage is too high

**Solutions:**
1. Reduce neuron count
2. Disable memory systems
3. Use structural plasticity to limit synapse growth
4. Monitor memory usage regularly

**Example:**
```python
# Create memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("working_memory_size", 20)
config.set("episodic_memory_capacity", 10)
config.set("enable_structural_plasticity", true)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Error Messages and Their Meanings

#### "Invalid neuron count"
- **Meaning:** Neuron count is not a valid number or is out of range
- **Solution:** Set a valid neuron count (e.g., 100-10000)

#### "Configuration file not found"
- **Meaning:** Specified configuration file does not exist
- **Solution:** Check file path and ensure file exists

#### "Brain initialization failed"
- **Meaning:** Brain failed to initialize
- **Solution:** Check configuration and try with minimal settings

#### "Memory allocation failed"
- **Meaning:** System out of memory
- **Solution:** Reduce simulation size or increase system memory

### Debug Commands

#### Debug Python Environment
```python
import sys
print("Python version:", sys.version)
print("Python path:", sys.path)

# Check NLM module
import pynlm
print("NLM module location:", pynlm.__file__)
print("NLM version:", getattr(pynlm, '__version__', 'Unknown'))
```

#### Debug Configuration Loading
```python
import pynlm
import os

# Check configuration file
config_file = "config.json"
print(f"Configuration file exists: {os.path.exists(config_file)}")

if os.path.exists(config_file):
    config = pynlm.createDefaultConfig()
    try:
        config.loadFromFile(config_file)
        print("Configuration loaded successfully")
        print(f"Neuron count: {config.getOr('neuron_count', 500)}")
    except Exception as e:
        print(f"Failed to load configuration: {e}")
```

#### Debug Brain State
```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get brain information
print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")

# Run simulation
for i in range(100):
    brain.step(i)

# Debug brain state
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
```

## Conclusion

This guide provides comprehensive examples and tutorials for getting started with the NLM brain simulation system. The examples cover:

1. **Basic Usage**: Simple neural simulations
2. **Configuration Management**: Creating and using configurations
3. **Python Bindings**: Working with Python API
4. **Command-Line Interface**: Using NLM from the command line
5. **Advanced Topics**: Advanced features and debugging
6. **Troubleshooting**: Common issues and solutions

## Next Steps

After working through these examples, you can:

1. **Explore Advanced Features**: Try more complex simulations
2. **Customize Configurations**: Create custom configurations for your research
3. **Join the Community**: Participate in NLM discussions and forums
4. **Contribute**: Help improve NLM with bug reports and feature requests
5. **Publish Results**: Share your findings using NLM

The NLM system is designed to be accessible to beginners while providing powerful features for advanced users. Start with the basic examples and gradually explore more complex functionality as you become familiar with the system.