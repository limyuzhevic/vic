# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

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

---

## Part 4: Using the Enhanced C++ Command-Line Interface

NLM now includes an enhanced command-line interface with multiple execution modes, comprehensive help system, and advanced configuration options. This makes it easier to use NLM for different purposes (research, education, performance testing, etc.) without needing to write custom code.

### New Modes Available

The enhanced CLI provides several execution modes:

- **demo** (default): Phase 6 integration test with connectivity, plasticity, and STDP verification
- **learning**: Plasticity learning experiment with detailed metrics
- **benchmark**: Performance testing with detailed timing measurements
- **explore**: Curiosity-driven exploration with adaptive input patterns
- **compare**: Parameter comparison and sensitivity analysis
- **test**: Basic functionality tests

### Getting Started with the Enhanced CLI

```bash
# Basic usage - runs demo mode
./nlm

# Specify mode explicitly
./nlm --mode learning

# Get comprehensive help
./nlm --help-modes          # Show mode descriptions
./nlm --help-options        # Show option details
./nlm --help-usage          # Show complete usage
./nlm --version             # Print version information
```

### Complete Usage Examples

#### 1. Running Different Modes

```bash
# Run demo mode (comprehensive integration test)
./nlm --mode demo

# Run learning experiment with custom settings
./nlm --mode learning --iterations 1000 --pattern random --verbose

# Run performance benchmark
./nlm --mode benchmark --iterations 20

# Run exploration mode with time limit
./nlm --mode explore --duration 120

# Compare different parameter sets
./nlm --mode compare
```

#### 2. Configuration and Output Options

```bash
# Load specific configuration file
./nlm --config myconfig.cfg

# Save results to file
./nlm --output results.json

# Validate configuration before running
./nlm --validate --config production.cfg

# List available configuration files
./nlm --list-configs
```

#### 3. Advanced Options

```bash
# Enable debug mode with maximum logging
./nlm --debug --verbose

# Quiet mode - only show errors and final results
./nlm --quiet

# Batch mode for automated testing
./nlm --batch --iterations 100

# Custom input patterns
./nlm --mode explore --pattern sinusoidal
./nlm --mode learning --pattern correlated
```

#### 4. Working with Patterns

The enhanced CLI supports different input patterns for experiments:

```bash
# Random pattern (default for learning)
./nlm --mode learning --pattern random

# Sinusoidal pattern (smooth input variations)
./nlm --mode explore --pattern sinusoidal

# Correlated pattern (pre/post firing)
./nlm --mode learning --pattern correlated

# Pulse pattern (sharp input bursts)
./nlm --mode explore --pattern pulse

# Custom pattern from file
./nlm --mode learning --pattern custom_input.txt
```

#### 5. Output Formats and Destinations

```bash
# Output to console (default)
./nlm --output stdout

# Output to text file
./nlm --output results.txt

# Output to JSON file for programmatic analysis
./nlm --output results.json

# Append to existing log file
./nlm --output experiment.log --mode benchmark
```

#### 6. Configuration Validation

The enhanced CLI includes robust configuration validation:

```bash
# Validate configuration file
./nlm --validate --config myconfig.cfg

# Check if all required parameters are present
./nlm --validate

# List all available configuration options
./nlm --config-options
```

### Mode Descriptions

#### Demo Mode (Phase 6 Integration Test)

```bash
./nlm --mode demo
```

**What it does:** Runs comprehensive integration tests verifying that all NLM systems work together:

- **Basic connectivity:** Tests that spikes propagate through the neural network
- **Plasticity learning:** Demonstrates measurable synaptic changes through experience
- **STDP verification:** Confirms spike-timing dependent plasticity works correctly

**Best for:** Initial setup verification, educational demonstrations, quality assurance

#### Learning Mode (Plasticity Experiment)

```bash
./nlm --mode learning --iterations 1000 --pattern random
```

**What it does:** Focuses on measurable synaptic changes through repeated experience with configurable input patterns.

**Key features:**
- Records initial and final brain states
- Tracks weight changes over time
- Supports different input patterns (random, sinusoidal, correlated, pulse)
- Provides detailed learning metrics

**Best for:** Research experiments, parameter tuning, studying learning dynamics

#### Benchmark Mode (Performance Testing)

```bash
./nlm --mode benchmark --iterations 10
```

**What it does:** Measures and reports performance characteristics for different network sizes and configurations.

**Key features:**
- Runs multiple benchmark configurations
- Measures execution time, spike rates, memory usage
- Compares against baseline performance
- Outputs formatted results tables

**Best for:** Performance optimization, capacity planning, system monitoring

#### Explore Mode (Curiosity-Driven Exploration)

```bash
./nlm --mode explore --duration 60 --pattern sinusoidal
```

**What it does:** Open-ended exploration with adaptive input based on brain state and curiosity levels.

**Key features:**
- Random stimulus application
- Tracks discovery patterns
- Adapts to network activity
- Records emergent behaviors

**Best for:** Behavioral research, curiosity studies, discovery experiments

#### Compare Mode (Parameter Sensitivity Analysis)

```bash
./nlm --mode compare
```

**What it does:** Runs the same experiment with different parameter sets to identify optimal configurations.

**Key features:**
- Tests multiple parameter combinations
- Compares results across configurations
- Identifies optimal parameter ranges
- Reports variance analysis

**Best for:** Parameter optimization, sensitivity analysis, experimental design

### Configuration Files

The enhanced CLI supports multiple configuration file formats:

#### Standard Format (configs/*.cfg)

```
# configs/myexperiment.cfg
neuron_count = 2000
region_count = 3
connection_probability = 0.05
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

#### Command Line Override

```bash
# Load config file and override specific values
./nlm --config config/base.cfg --neuron_count=5000 --connection_probability=0.1
```

### Script Automation

The enhanced CLI supports batch processing for automated workflows:

#### Simple Batch Script

```bash
#!/bin/bash

# Run multiple experiments in batch mode
for pattern in random sinusoidal correlated pulse; do
    echo "Running experiment with pattern: $pattern"
    ./nlm --mode learning --pattern $pattern --output $pattern"_results.txt"
done
```

#### Parameter Sweeps

```bash
#!/bin/bash

# Sweep through different parameter values
for neurons in 500 1000 2000; do
    for connections in 0.05 0.1 0.15; do
        ./nlm --mode learning \
            --neuron_count $neurons \
            --connection_probability $connections \
            --output "n${neurons}_c${connections}.txt"
    done
done
```

#### Parallel Execution

```bash
#!/bin/bash

# Run multiple experiments in parallel
for i in {1..5}; do
    ./nlm --mode learning --iterations 500 --output "run$i.txt" &
done

wait  # Wait for all background jobs to complete
```

### File Output Formats

The enhanced CLI supports multiple output formats:

#### Text Format (default for console)

```
=== DEMON STRATION MODE ===
Mode: demo
Duration: 2.34s
Success: true

Results:
- Basic connectivity: PASSED
- Plasticity learning: PASSED
- STDP verification: PASSED
```

#### JSON Format

```json
{
  "mode": "learning",
  "duration": 12.5,
  "success": true,
  "metrics": {
    "learning_progress": 0.342,
    "total_spikes": 15678,
    "synapse_changes": 234
  },
  "output": "Learning experiment completed successfully."
}
```

### Advanced Features

#### Log File Support

```bash
# Log to file in addition to console
./nlm --log-file nlm_debug.log --debug

# Separate log file for different modes
./nlm --mode benchmark --log-file benchmark.log
./nlm --mode learning --log-file learning.log
```

#### Custom Help and Documentation

```bash
# Show context-specific help
./nlm --mode learning --help
./nlm --mode benchmark --help

# Display version and build information
./nlm --version
```

### Troubleshooting

#### Common Issues and Solutions

**"Command not found: nlm"**
```bash
# First time building
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**"Unknown option: --myoption"**
```bash
# Check available options
./nlm --help-options

# Or get usage information
./nlm --help-usage
```

**"Configuration file not found"**
```bash
# Check available configuration files
./nlm --list-configs

# Or use default configuration
./nlm --mode learning
```

**"Permission denied"**
```bash
# Check file permissions
chmod +x nlm
chmod 644 configs/*.cfg
```

### Integration with Existing Workflows

The enhanced CLI is fully compatible with existing NLM workflows:

#### Using with Python Scripts

```bash
#!/usr/bin/env python3
import subprocess
import json
import sys

# Run NLM from Python
result = subprocess.run([
    './nlm', '--mode', 'learning', '--iterations', '1000'
], capture_output=True, text=True)

if result.returncode == 0:
    print("NLM completed successfully")
    print(result.stdout)
else:
    print("NLM failed:")
    print(result.stderr)
    sys.exit(1)
```

#### Post-Processing Results

```bash
#!/bin/bash

# Run experiment and parse results
./nlm --mode benchmark --output raw_results.txt

# Extract specific metrics
python3 parse_benchmark_results.py raw_results.txt > summary.txt

# Generate plots
python3 plot_performance.py raw_results.txt performance_plot.png
```

### Next Steps

After getting familiar with the enhanced CLI:

1. **Read `easy_usage.md`** - Quick start guide with examples
2. **Read `docs/ARCHITECTURE.md`** - System architecture overview
3. **Read `docs/PHASE6_FINAL_AUDIT.md`** - Phase 6 integration details
4. **Experiment with different modes** - Find the right setup for your needs
5. **Create custom configuration files** - Optimize for your specific use case
6. **Set up automated workflows** - Batch processing for research

---

## Part 5: Extending the CLI

The enhanced CLI provides hooks for extending functionality:

### Custom Modes

To add a custom mode, modify `src/main.cpp`:

```cpp
// Add to CommandLineArgs
std::string customMode = "";

// Add to parseArguments()
if (arg.find("--custom-mode=") == 0) {
    args.customMode = arg.substr(18);
}

// Add to main() processing
if (args.mode == "custom") {
    runCustomMode(brain, args);
}
```

### Plugin Architecture

The enhanced CLI can be extended with plugins:

```cpp
// Example plugin interface
class ModePlugin {
public:
    virtual ~ModePlugin() = default;
    virtual std::string getName() const = 0;
    virtual void run(std::shared_ptr<Brain> brain, const CommandLineArgs& args) = 0;
};
```

### Configuration Schema

Define configuration schema for validation:

```cpp
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "double", "bool", "string"
    std::string defaultValue;
    std::string description;
    bool required;
};
```

The enhanced CLI represents a significant improvement over the original basic interface, providing comprehensive functionality for diverse use cases while maintaining backward compatibility. It supports scientific research, educational purposes, performance optimization, and automated testing scenarios with robust configuration management and output flexibility.

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
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
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
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

# Run the simulation
run_agent_simulation(1000)
```

### Key Classes and Methods

#### Brain Class

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

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

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

#### AgentBrain Class

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

#### SimpleWorld Class

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

### Enumerations

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

---

## Part 4: Configuration Options

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

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Troubleshooting

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

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
