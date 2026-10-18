# NLM Enhanced Documentation
# Comprehensive Guide for All User Levels

This document provides comprehensive information about using the enhanced NLM (Neural Learning Machine) framework. It covers both beginner-friendly simple API and advanced features for expert users.

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start - Simple API](#quick-start---simple-api)
3. [Advanced API Usage](#advanced-api-usage)
4. [Configuration Management](#configuration-management)
5. [Error Handling](#error-handling)
6. [Performance Optimization](#performance-optimization)
7. [Advanced Features](#advanced-features)
8. [Troubleshooting](#troubleshooting)
9. [API Reference](#api-reference)
10. [Examples](#examples)

## Introduction

NLM (熙然) is an experimental artificial developmental brain simulation framework. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, multiple plasticity mechanisms (STDP, Hebbian, structural), and neuromodulation systems.

### Key Features

- **Real Neural Computation**: LIF neuron dynamics with spike propagation
- **Multiple Plasticity Mechanisms**: STDP, Hebbian, and structural plasticity
- **Neuromodulation Systems**: Dopamine, curiosity, novelty detection
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Predictive Processing**: Prediction error and confidence tracking
- **Developmental Stages**: Initial, critical period, maturation, adult, aging
- **Environmental Interaction**: Sensory processing and motor control

### Architecture Overview

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

## Quick Start - Simple API

The simple API is designed for beginners and provides an intuitive interface for creating and running neural simulations.

### Installation

```bash
# Install the package
pip install pynlm

# For development, install in editable mode
pip install -e .
```

### Basic Usage

#### Example 1: Silent Brain

```python
import pynlm

# Create and initialize a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 steps
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

#### Example 2: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
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

### Common Patterns

#### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

#### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

#### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

## Advanced API Usage

The advanced API provides more control and access to underlying functionality for expert users.

### Creating Custom Configurations

```python
import pynlm

# Create a custom configuration
custom_config = pynlm.createDefaultConfig()

# Set custom parameters
custom_config.set("brain.neuron_count", 2000, pynlm.ConfigSource.User)
custom_config.set("brain.synapse_density", 0.15, pynlm.ConfigSource.User)
custom_config.set("plasticity.stdp.learning_rate", 0.002, pynlm.ConfigSource.User)
custom_config.set("neuromod.dopamine.scale", 1.5, pynlm.ConfigSource.User)
custom_config.set("development.enabled", True, pynlm.ConfigSource.User)

# Create brain with custom config
brain = pynlm.createBrain(custom_config)
```

### Advanced Brain Control

```python
# Get comprehensive brain statistics
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
    "total_spikes": brain.getTotalSpikeCount(),
    "developmental_stage": brain.getDevelopmentalStage()
}

print("Brain Statistics:", stats)
```

### Advanced World Management

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.95)
world.setRandomSeed(42)

# Add objects to the world
resource = pynlm.WorldObject(x=15.0, y=15.0, type=pynlm.WorldObjectType.Resource, value=10.0)
hazard = pynlm.WorldObject(x=5.0, y=25.0, type=pynlm.WorldObjectType.Hazard, value=5.0)

world.addObject(resource)
world.addObject(hazard)

# Check if position is valid
if world.isValidPosition(10.0, 10.0):
    print("Position is valid for agent")
```

### Advanced Agent Brain Features

```python
# Create agent with custom settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)

# Get advanced agent statistics
agent_stats = {
    "curiosity_level": agent.getCuriosityLevel(),
    "novelty_level": agent.getNoveltyLevel(),
    "prediction_error": agent.getPredictionError(),
    "neuromodulation_level": agent.getNeuromodulationLevel(),
    "developmental_stage": agent.getDevelopmentalStage(),
    "reward_modulation_enabled": agent.isRewardModulationEnabled(),
    "structural_plasticity_enabled": agent.isStructuralPlasticityEnabled(),
    "development_enabled": agent.isDevelopmentEnabled(),
    "curiosity_enabled": agent.isCuriosityEnabled()
}

print("Agent Statistics:", agent_stats)
```

## Configuration Management

### Configuration Parameters

The NLM framework supports the following configuration parameters:

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

#### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enabled` | bool | true | Enable developmental processes |
| `development.initial_plasticity` | float | 1.0 | Initial plasticity factor |
| `development.critical_period_end` | float | 300.0 | End of critical period (simulated time) |

### Configuration File Loading

```python
import pynlm

# Load configuration from JSON file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Save configuration to JSON file
config.saveToFile("optimized_config.json")

# Check if configuration has a key
if config.has("brain.neuron_count"):
    neuron_count = config.getInt("brain.neuron_count")
    print(f"Neuron count: {neuron_count}")
```

## Error Handling

The enhanced NLM framework provides comprehensive error handling with specific exception types.

### Error Types

- **NLMError**: Base error class for all NLM errors
- **ConfigurationError**: Configuration-related errors
- **InitializationError**: Initialization-related errors
- **SimulationError**: Simulation-related errors
- **MemoryError**: Memory-related errors

### Error Handling Examples

```python
import pynlm

try:
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
        
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
    # Handle NLM-specific errors
    
except pynlm.ConfigurationError as e:
    print(f"Configuration Error: {e}")
    # Handle configuration errors
    
except pynlm.InitializationError as e:
    print(f"Initialization Error: {e}")
    # Handle initialization errors
    
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle unexpected errors
```

### Error Classification

```python
# Classify errors for better handling
error_type = pynlm.classify_error("ConfigurationError: Invalid neuron count")
print(f"Error type: {error_type}")  # Output: "configuration"

error_type = pynlm.classify_error("Memory allocation failed")
print(f"Error type: {error_type}")  # Output: "memory"
```

## Performance Optimization

### Memory Management

The enhanced NLM framework includes memory pool utilities for improved performance:

```python
# Memory pool for better performance (advanced users)
import pynlm

# Get memory usage estimates
memory_info = pynlm.get_memory_usage()
print(f"Estimated max memory: {memory_info['estimated_max'] / (1024*1024):.1f} MB")
print(f"Recommended minimum: {memory_info['recommended_min'] / (1024*1024):.1f} MB")
```

### Performance Monitoring

```python
# Get system performance information
performance_info = pynlm.get_platform_info()
print("Platform:", performance_info["platform"])
print(f"CPU cores: {performance_info['cpu_count']}")
print("Debug build:", performance_info["debug_build"])
```

### Configuration Presets

```python
# Get predefined configuration presets
presets = pynlm.get_config_presets()

# Use fast preset for quick testing
fast_brain = pynlm.createBrain(presets["fast"])
fast_brain.initialize()

# Use balanced preset for general use
balanced_brain = pynlm.createBrain(presets["balanced"])
balanced_brain.initialize()

# Use high-performance preset for complex simulations
high_perf_brain = pynlm.createBrain(presets["high_performance"])
high_perf_brain.initialize()
```

## Advanced Features

### Batch Processing

Process multiple sensory inputs simultaneously:

```python
import pynlm

# Create multiple sensory inputs
sensor1 = pynlm.createDefaultVision(8, 8)
sensor2 = pynlm.createDefaultVision(8, 8)
sensor3 = pynlm.createDefaultVision(8, 8)

# Batch process sensors
combined_data = pynlm.batch_process_sensory([sensor1, sensor2, sensor3])
print(f"Combined data size: {len(combined_data)}")
```

### Advanced Simulation Control

```python
import pynlm

# Run a complete simulation with error checking
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with advanced error handling
try:
    pynlm.run_simulation_loop(brain, world, agent, 1000, 0.1)
    print("Simulation completed successfully!")
except pynlm.SimulationError as e:
    print(f"Simulation failed: {e}")
```

### Development Environment Setup

```python
import pynlm

# Create a complete agent learning environment
environment = pynlm.create_agent_learning_environment()

brain = environment["brain"]
world = environment["world"]
agent = environment["agent"]

# The environment already has all features enabled:
# - 500 neurons
# - STDP, Hebbian, and structural plasticity
# - Curiosity-driven exploration
# - Developmental processes
```

### Test Environment

```python
import pynlm

# Create a minimal test environment for quick testing
environment = pynlm.create_test_environment()

brain = environment["brain"]
world = environment["world"]
agent = environment["agent"]

# This environment is optimized for fast testing:
# - 100 neurons
# - Low connection probability
# - Minimal configuration
```

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"

**Problem**: The brain appears inactive with no firing neurons.

**Solution**:
1. Ensure you call `brain.initialize()` before using the brain
2. Try increasing the number of simulation steps
3. Check if neurons are receiving input by adding `agent.processSensoryInput()`

#### "The agent isn't moving"

**Problem**: The agent's motor commands don't produce movement.

**Solution**:
1. Ensure you call `world.applyMotorCommand()` to apply actions
2. Check that `world.update()` is being called in the simulation loop
3. Verify the agent's motor neurons are active (check firing rates)

#### "Everything is 0"

**Problem**: All sensor values and brain outputs are zero.

**Solution**:
1. Brains need time to "warm up" - try more steps
2. Ensure sensory input is connected to the brain
3. Check if neurons are receiving sufficient current injection

#### "ImportError: No module named 'pynlm'"

**Problem**: The pynlm module is not installed.

**Solution**:
```bash
pip install --force-reinstall .
```

#### "Segmentation faults"

**Problem**: The program crashes with segmentation faults.

**Solution**:
1. Ensure `initialize()` is called before `step()`
2. Call `reset()` before re-running simulation
3. Check for null pointer usage in your code
4. Enable assertions: `pip install -e ".[debug]"`

### Debugging Tips

1. **Add print statements**: Insert print statements in your simulation loop to track progress
2. **Use debugger**: Set breakpoints in key functions to inspect state
3. **Check return values**: Always check return values of functions that indicate success/failure
4. **Monitor resources**: Use `pynlm.get_platform_info()` to check system resources
5. **Validate inputs**: Ensure all inputs are within expected ranges

## API Reference

### Core Classes

#### Brain Class

| Method | Description |
|--------|-------------|
| `createBrain(config)` | Create a new brain with configuration |
| `initialize()` | Initialize the brain with configuration |
| `step(step_number)` | Run one simulation step |
| `step(step_number, time)` | Run step with timestamp |
| `reset()` | Reset brain state |
| `receiveSensoryInput(input)` | Inject sensory input into the brain |
| `produceAction()` | Get motor action based on neural activity |
| `save(filepath)` | Save brain state to file |
| `load(filepath)` | Load brain state from file |
| `addRegion(name)` | Add a new neural region |
| `getRegion(id)` | Get a region by ID |
| `getTotalNeuronCount()` | Get total neuron count |
| `getTotalSynapseCount()` | Get total synapse count |
| `getFiringNeuronCount()` | Get count of currently firing neurons |
| `getAverageFiringRate()` | Get average firing rate |
| `getExcitationInhibitionRatio()` | Get E/I balance ratio |
| `getTotalSpikeCount()` | Get total spike count |

#### Config Class

| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `loadFromFile(filepath)` | Load configuration from JSON file |
| `saveToFile(filepath)` | Save configuration to JSON file |
| `has(key)` | Check if key exists |
| `get(key)` | Get value as string |
| `getInt(key)` | Get value as integer |
| `getFloat(key)` | Get value as float |
| `set(key, value)` | Set configuration value |
| `getKeys()` | Get all configuration keys |
| `clear()` | Clear all configuration entries |

#### AgentBrain Class

| Method | Description |
|--------|-------------|
| `createAgentBrain(brain)` | Create agent brain interface |
| `initialize(world)` | Initialize with world |
| `processSensoryInput(percept)` | Process sensory percept and inject into brain |
| `decodeMotorCommand()` | Decode brain motor activity into motor command |
| `applyRewardModulation(reward, predictedReward)` | Apply reward-based neuromodulation |
| `updateDevelopment(timestep)` | Update development system |
| `enableRewardModulation(enable)` | Enable/disable reward modulation |
| `enableCuriosity(enable)` | Enable/disable curiosity |
| `getCuriosityLevel()` | Get curiosity level |
| `getNoveltyLevel()` | Get novelty level |
| `getPredictionError()` | Get prediction error |

#### SimpleWorld Class

| Method | Description |
|--------|-------------|
| `createSimpleWorld()` | Create simple world |
| `configure(width, height, visionWidth, visionHeight)` | Configure world dimensions |
| `update(timestep)` | Update world state |
| `applyMotorCommand(cmd, time)` | Apply motor command |
| `getSensoryPercept()` | Get sensory percept from world |
| `getAgentBody()` | Get agent body state |
| `addObject(obj)` | Add world object |
| `removeObject(x, y)` | Remove world object |
| `isValidPosition(x, y)` | Check if position is valid |

### Enumerations

#### NeuronType
- `Excitatory`: Excitatory neuron
- `Inhibitory`: Inhibitory neuron
- `Modulatory`: Modulatory neuron
- `Sensory`: Sensory neuron
- `Motor`: Motor neuron
- `Internal`: Internal neuron

#### ActionType
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `MoveLeft`: Move left
- `MoveRight`: Move right
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `Look`: Look
- `Interact`: Interact with environment
- `Eat`: Eat food
- `Drink`: Drink water
- `Rest`: Rest/sleep
- `Wait`: Wait
- `Custom`: Custom action

#### DevelopmentalStage
- `Initial`: Initial developmental stage
- `CriticalPeriod`: Critical period for learning
- `Maturation`: Maturation stage
- `Adult`: Adult stage
- `Aging`: Aging stage

#### WorldObjectType
- `Empty`: Empty space
- `Resource`: Resource object
- `Hazard`: Hazardous object
- `Wall`: Wall barrier
- `Marker`: Marker point

### Utility Functions

#### Factory Functions
- `createDefaultConfig()`: Create default configuration
- `createMinimalConfig()`: Create minimal configuration for fast testing
- `createAdvancedConfig()`: Create configuration with all features enabled
- `createBrain(config)`: Create brain with configuration
- `createSimpleWorld()`: Create simple world
- `createAgentBrain(brain)`: Create agent brain interface

#### Advanced Factory Functions
- `createDefaultVision(width, height, channels)`: Create default vision object
- `createAction(type, parameters)`: Create action object

#### Environment Setup
- `create_agent_learning_environment()`: Create complete learning environment
- `create_test_environment()`: Create minimal test environment

#### Performance and Utility
- `run_simulation_loop(brain, world, agent, num_steps, timestep)`: Run complete simulation
- `batch_process_sensory(inputs)`: Batch process sensory inputs
- `get_version_info()`: Get version information
- `get_platform_info()`: Get platform information
- `classify_error(error_message)`: Classify error message
- `get_memory_usage()`: Get memory usage estimates
- `get_tutorials()`: Get list of tutorials
- `quick_start_example()`: Get quick start example

## Examples

### Example 1: Basic Neural Network

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation and collect statistics
for i in range(500):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}:")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")

print("Simulation completed!")
```

### Example 2: Agent in Interactive Environment

```python
import pynlm

def run_agent_episode(episode_num=0):
    # Create environment
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight: 6)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Run episode
    max_steps = 200
    for step in range(max_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Check if episode ended
        if world.getAgentBody().health <= 0:
            print(f"Episode {episode_num} ended at step {step}: Agent died")
            break
    
    # Print episode statistics
    print(f"Episode {episode_num} statistics:")
    print(f"  Total steps: {step + 1}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")

# Run multiple episodes
for episode in range(5):
    run_agent_episode(episode)
```

### Example 3: Learning Experiment

```python
import pynlm

def run_learning_experiment():
    # Create brain with balanced configuration
    config = pynlm.createBalancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with resources
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height: 20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Add resources to world
    resource1 = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=10.0)
    resource2 = pynlm.WorldObject(x=15.0, y=15.0, type=pynlm.WorldObjectType.Resource, value=20.0)
    world.addObject(resource1)
    world.addObject(resource2)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Learning loop
    target_performance = 1000
    achieved_performance = 0
    
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and learn
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Track performance
        if agent.getNoveltyLevel() > 0.1:
            achieved_performance += 1
        
        # Check if learning is complete
        if achieved_performance >= target_performance:
            print(f"Learning completed at step {step}!")
            break
        
        # Periodic progress report
        if step % 100 == 0:
            print(f"Step {step}: Performance {achieved_performance}/{target_performance}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}, Novelty {agent.getNoveltyLevel():.3f}")
    
    print(f"Learning experiment completed!")
    print(f"Final performance: {achieved_performance} novel experiences")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")

# Run learning experiment
run_learning_experiment()
```

### Example 4: Performance Benchmarking

```python
import pynlm
import time

def benchmark_configuration(config_name, config):
    """Benchmark a specific configuration."""
    print(f"Benchmarking {config_name} configuration...")
    
    # Create brain
    brain = pynlm.createBrain(config)
    
    # Measure initialization time
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    # Measure step execution time
    num_steps = 100
    start_time = time.time()
    for step in range(num_steps):
        brain.step(step)
    step_time = time.time() - start_time
    
    # Calculate statistics
    avg_step_time = step_time / num_steps
    steps_per_second = num_steps / step_time
    
    # Get brain statistics
    stats = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount()
    }
    
    # Print results
    print(f"  Initialization time: {init_time:.3f}s")
    print(f"  Average step time: {avg_step_time:.6f}s")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Brain statistics: {stats}")
    
    return {
        "init_time": init_time,
        "avg_step_time": avg_step_time,
        "steps_per_second": steps_per_second,
        "stats": stats
    }

# Get configuration presets
presets = pynlm.get_config_presets()

# Benchmark different configurations
print("Configuration Benchmark Results")
print("=" * 50)

fast_results = benchmark_configuration("Fast", presets["fast"])
balanced_results = benchmark_configuration("Balanced", presets["balanced"])  
high_perf_results = benchmark_configuration("High Performance", presets["high_performance"])

# Compare results
print("\nComparison:")
print(f"Fast:     {fast_results['steps_per_second']:.1f} steps/sec, {fast_results['avg_step_time']:.6f}s/step")
print(f"Balanced: {balanced_results['steps_per_second']:.1f} steps/sec, {balanced_results['avg_step_time']:.6f}s/step")
print(f"High Perf: {high_perf_results['steps_per_second']:.1f} steps/sec, {high_perf_results['avg_step_time']:.6f}s/step")
```

## Conclusion

The enhanced NLM framework provides a comprehensive and flexible platform for neural simulation research. With both simple API for beginners and advanced features for experts, NLM makes it easy to create and experiment with artificial developmental brains.

Key improvements include:

1. **Enhanced Error Handling**: Comprehensive error handling with specific exception types
2. **Configuration Management**: Advanced configuration system with validation
3. **Performance Monitoring**: Built-in performance tracking and optimization
4. **Advanced API**: Enhanced API with better type safety and functionality
5. **Comprehensive Documentation**: Complete documentation with examples
6. **Unit Tests**: Extensive unit test coverage
7. **Memory Management**: Memory pool utilities for performance
8. **Development Tools**: Tools for development and testing

Whether you're a beginner looking to understand neural computation or an expert researcher needing advanced features, NLM provides the tools you need for successful neural simulation projects.
