# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Quick Start (2-3 pages)
2. Basic Usage (comprehensive examples)
3. Advanced Features (new section)
4. Development & Debugging (new section)

---

## Part 1: Quick Start Guide

### Installation

**Step 1: Install Prerequisites**

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Ensure system dependencies
# - GCC 10+ or Clang 12+
# - CMake 3.16+
# - Python 3.8+
```

**Step 2: Build from Source**

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
```

**Step 3: Verify Installation**

```bash
# Run basic tests (if built)
ctest --output-on-failure

# Test the main executable
./nlm
```

### Basic Usage Example

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

---

## Part 2: Basic Usage

### 1. Creating a Brain

#### Create a default configuration

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

#### Create with custom configuration

```python
# Create configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Modify parameters
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -52.0)
config.set("plasticity.stdp.enable", True)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Running a Simulation

#### Basic simulation loop

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

#### Advanced simulation with time and input

```python
for step in range(2000):
    # Receive sensory input (if using environment)
    if step % 10 == 0:
        sensory_input = [0.5, 0.8, 0.2, 0.9]  # Example sensory data
        brain.receiveSensoryInput(sensory_input)
    
    # Run brain step with simulation time
    brain.step(step, step * 0.001)
    
    # Get action if using agent brain
    # action = agent_brain.decodeMotorCommand()
```

### 3. Creating a Simple World

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

### 4. Complete Agent Example

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

### 5. Key Classes and Methods

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

### 6. Enumerations

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

### 7. Configuration Options

#### Brain Configuration

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

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 3: Advanced Features

### 1. Advanced CLI Usage

```bash
# Basic usage
./nlm
./nlm --config my_config.cfg

# Advanced options
./nlm --neuron-count 2000 --timestep 0.0005 --plasticity true
./nlm --debug --log-level DEBUG --output-format json
./nlm --experiment phase6 --benchmark --profile

# Configuration file
./nlm --config custom.cfg --checkpoint /tmp/my_checkpoint.bin
```

### 2. Performance Optimization

```python
# Advanced configuration for performance
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)  # Lower density for faster simulation
config.set("simulation.timestep", 0.0001)  # Smaller timesteps for accuracy

# Parallel processing (if available)
brain.enableParallelProcessing(True)
brain.setThreadCount(4)

# Memory optimization
brain.enableMemoryPool(True)
brain.setMemoryPoolSize(1024)  # 1024 neuron slots
```

### 3. Debugging and Troubleshooting

```python
# Enable debugging
brain.enableDebugMode(True)
agent.enableDebugLogging(True)
world.enableDebugOutput(True)

# Set logging levels
Logger::getInstance().setLevel(Logger::Level::Debug)

# Common debugging patterns
brain.validateState()  # Check for inconsistencies
brain.checkMemoryUsage()  # Monitor memory consumption
brain.benchmarkPerformance()  # Performance profiling
```

### 4. Experiment Customization

```python
# Create custom experiment configuration
experiment_config = {
    'neuron_count': 1000,
    'max_steps': 5000,
    'enable_checkpointing': True,
    'enable_replay': True,
    'enable_development': True,
    'plasticity_params': {
        'stdp_learning_rate': 0.002,
        'hebbian_strength': 0.01
    }
}

# Run custom experiment
result = run_custom_experiment(experiment_config)

# Analyze results
analyze_learning_curve(result)
analyze_memory_consolidation(result)
analyze_behavioral_patterns(result)
```

### 5. Advanced Python API Features

```python
# Advanced brain manipulation
brain.enableAllSubsystems()  # Enable all agent subsystems
brain.setPlasticityParameters()  # Configure plasticity
brain.setNeuromodulation()  # Configure neuromodulation
brain.customLearningRule()  # Implement custom learning

# Advanced world management
world.setCustomPhysics()  # Custom physics simulation
world.addCustomBehavior()  # Add custom behaviors
world.configureAdvanced()  # Advanced configuration

# Agent capabilities
agent.enableCuriosity(True)  # Enable curiosity-driven learning
agent.setExplorationStrategy()  # Set exploration strategy
agent.customRewardFunction()  # Implement custom reward
agent.advancedDevelopment()  # Enable advanced development

# Performance and monitoring
brain.startProfiling()  # Start performance profiling
brain.getPerformanceStats()  # Get performance statistics
brain.exportNeuralState()  # Export neural state
brain.importNeuralState()  # Import neural state

# Experiment control
brain.runLearningExperiment()  # Run learning experiments
brain.compareNetworks()  # Compare different networks
brain.automatedTesting()  # Automated testing framework
```

### 6. Development Tools

#### Visualization Tools

```python
# Basic visualization
brain.visualizeNetwork()  # Visualize network structure
brain.plotActivity()     # Plot neural activity
brain.animateSimulation()  # Animate simulation

# Advanced visualization
brain.createCustomViz()   # Custom visualization
brain.exportForAnalysis() # Export data for analysis
```

#### Testing Framework

```python
# Unit testing
brain.testNeuronDynamics()      # Test neuron dynamics
brain.testPlasticity()          # Test plasticity mechanisms
brain.testMemorySystems()       # Test memory integration
brain.testIntegration()         # Test system integration

# Integration testing
brain.runIntegrationTest()     # Full integration test
brain.benchmarkPerformance()   # Performance benchmarking
brain.stressTest()            # Stress testing
```

---

## Part 4: Development & Debugging

### 1. Build Configuration

#### Debug Build

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Custom Compiler

```bash
# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
```

#### Install Options

```bash
# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### 2. Common Debugging Issues

#### Segmentation Faults

**Symptoms**: Program crashes with segfault

**Solutions**:

```bash
# Check that initialize() is called before step()
# Ensure reset() is called before re-running simulation
# Check for null pointer dereferences
# Verify memory allocation/deallocation
```

#### ImportError: No module named 'pynlm'

**Solutions**:

```bash
# Reinstall the module
pip install --force-reinstall .

# Check Python path
python -c "import sys; print(sys.path)"
```

#### CMake cannot find Python

**Solutions**:

```bash
# Install build dependencies
pip install scikit-build-core pybind11

# Specify Python executable
cmake .. -DPython_EXECUTABLE=$(which python)
```

### 3. Performance Tuning

#### Monitoring Memory Usage

```python
# Track memory consumption
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")
```

#### Profiling

```bash
# CPU profiling
./nlm --profile 60  # Profile for 60 seconds

# Memory profiling
valgrind ./nlm --debug
```

#### Optimization Techniques

```python
# Use memory pools
brain.enableMemoryPool(True)

# Optimize configuration
config.set("brain.synapse_density", 0.05)  # Lower = faster
config.set("simulation.timestep", 0.0001)  # Smaller = more accurate

# Parallel processing
brain.enableParallelProcessing(True)
```

### 4. Troubleshooting

#### Build Issues

**Problem**: CMake cannot find Python
**Solution**:
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Problem**: Compilation errors
**Solution**:
```bash
# Ensure C++20 compiler support
# Check CMake version >= 3.16
# Verify all source files are present
```

#### Runtime Issues

**Problem**: ImportError: No module named 'pynlm'
**Solution**:
```bash
pip install --force-reinstall .
```

**Problem**: Segmentation faults
**Solution**:
```bash
# Check that initialize() is called before step()
# Ensure reset() is called before re-running simulation
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit report

---

## License

MIT

---

## Authors

Research project — See docs for scientific background.
