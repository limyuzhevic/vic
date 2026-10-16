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

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

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
- [API.md](docs/API.md) - Complete Python API reference

## Advanced Python Features

### Enhanced Error Handling and Validation

The enhanced Python bindings provide comprehensive error handling:

```python
import pynlm

# Create brain with validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Automatically validated

# Error handling
try:
    config.set("brain.neuron_count", -1)  # Invalid!
    brain = pynlm.createBrain(config)
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
```

### Convenience Functions and High-Level APIs

#### Advanced Agent Creation

```python
# Create complete agent with all systems enabled
brain, world, agent = pynlm.create_advanced_agent()
agent.enable_all_systems()

# Quick setup function
brain, world, agent = pynlm.create_advanced_agent({
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15,
    "brain.development.enabled": True,
    "brain.plasticity.enabled": True
})
```

#### Enhanced Simulation Management

```python
# Run complete training episode
brain, world, agent = pynlm.create_advanced_agent()

def train_episode(brain, world, agent, steps=100, with_reward=True):
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if with_reward and step % 10 == 0:
            agent.applyRewardModulation(0.5, 0.3)
    
    return brain.getAverageFiringRate()

# Run multiple episodes
for episode in range(10):
    firing_rate = train_episode(brain, world, agent)
    print(f"Episode {episode}: {firing_rate:.3f} Hz")
    
    # Reset for next episode
    brain.reset()
    world.reset()
    agent.reset()
```

### Performance Monitoring and Benchmarking

#### Basic Performance Tracking

```python
import time
import pynlm

# Create brain
brain = pynlm.createBrain()
brain.initialize()

# Benchmark performance
start_time = time.time()
for i in range(1000):
    brain.step(i)
end_time = time.time()

print(f"Performance: {1000/(end_time - start_time):.0f} steps/second")
print(f"Average step time: {(end_time - start_time)/1000:.6f}s")
```

#### Advanced Performance Benchmarking

```python
import pynlm
from benchmarks.performance_test import benchmark_brain_performance

# Comprehensive benchmark
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

results = benchmark_brain_performance(brain, num_trials=5, steps_per_trial=1000)
print(f"Average performance: {results['average_performance']:.0f} steps/sec")
```

### Memory and Resource Management

#### Memory Usage Monitoring

```python
import pynlm
from benchmarks.performance_test import benchmark_memory_usage

# Create brain
brain = pynlm.createBrain()
brain.initialize()

# Monitor memory usage
memory_results = benchmark_memory_usage(brain, steps=1000)
print(f"Memory increase: {memory_results['memory_increase']:.2f} MB")
```

### Advanced Configuration Management

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Set multiple parameters at once
advanced_config = {
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.15,
    "brain.region_count": 10,
    "brain.development.enabled": True,
    "brain.plasticity.enabled": True,
    "brain.memory.working.capacity": 200,
    "brain.memory.episodic.capacity": 100,
    "brain.prediction.enabled": True,
    "brain.cognition.enabled": True,
    "brain.neuromodulation.enabled": True
}

# Apply all configuration
for key, value in advanced_config.items():
    config.set(key, value)

print(f"Configuration summary: {config.summary()}")
```

### Enhanced Simulation Features

#### Development System Control

```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()

# Test all developmental stages
stages = pynlm.DevelopmentalStage
for stage_name in ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']:
    stage = getattr(stages, f'{stage_name}')
    brain.setDevelopmentalStage(stage)
    print(f"{stage_name}: {brain.getDevelopmentalStage()}")
```

#### Neuromodulation Systems

```python
import pynlm

brain, world, agent = pynlm.create_advanced_agent()

# Explore neuromodulation
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")

# Enable all neuromodulation systems
agent.enable_all_systems()
```

### Visualization and Analysis

```python
import pynlm
import matplotlib.pyplot as plt

brain = pynlm.createBrain()
brain.initialize()

# Run simulation first
for i in range(100):
    brain.step(i)

# Visualize neural network
try:
    import networkx as nx
    brain.visualizeNetwork("brain_network.png")
    print("Network visualization saved to brain_network.png")
except ImportError:
    print("networkx not available for visualization")

# Analyze statistics
stats = {
    'total_neurons': brain.getTotalNeuronCount(),
    'total_synapses': brain.getTotalSynapseCount(),
    'firing_rate': brain.getAverageFiringRate(),
    'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
    'development_stage': brain.getDevelopmentalStage()
}

print("Brain statistics:", stats)
```

### Error Handling and Validation

```python
import pynlm

# Validate configuration parameters
def validate_brain_config(config):
    errors = []
    
    if config.has("brain.neuron_count"):
        neurons = config.get("brain.neuron_count")
        if neurons < 1 or neurons > 1000000:
            errors.append("Neuron count must be between 1 and 1,000,000")
    
    if config.has("brain.synapse_density"):
        density = config.get("brain.synapse_density")
        if density < 0.0 or density > 1.0:
            errors.append("Synapse density must be between 0.0 and 1.0")
    
    return errors

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)

errors = validate_brain_config(config)
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid!")
```

### Advanced Example: Complete Training Pipeline

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

def complete_training_pipeline(episodes=20, neuron_count=2000):
    """Complete training pipeline with all advanced features."""
    
    print(f"Starting training pipeline: {episodes} episodes, {neuron_count} neurons")
    
    # 1. Create advanced agent with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.development.enabled", True)
    config.set("brain.plasticity.enabled", True)
    config.set("brain.memory.working.capacity", 500)
    config.set("brain.memory.episodic.capacity", 300)
    
    # 2. Create complete simulation setup
    brain, world, agent = pynlm.create_advanced_agent()
    
    # 3. Enable all learning systems
    agent.enable_all_systems()
    
    # 4. Training loop with comprehensive tracking
    training_stats = {
        'episodes': [],
        'firing_rates': [],
        'rewards': [],
        'curiosity_levels': [],
        'actions_taken': 0,
        'development_stages': [],
        'memory_usage': []
    }
    
    for episode in range(episodes):
        episode_stats = {
            'firing_rate': 0.0,
            'total_reward': 0.0,
            'curiosity_level': 0.0,
            'actions_taken': 0,
            'development_stage': agent.getDevelopmentalStage()
        }
        
        # Run episode
        for step in range(100):
            # Environment interaction
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reinforcement learning
            if step % 10 == 0:
                reward = 1.0 if action != pynlm.MotorCommand.Wait else -0.1
                agent.applyRewardModulation(reward, 0.5)
                episode_stats['total_reward'] += reward
            
            # Track curiosity and development
            episode_stats['curiosity_level'] = agent.getCuriosityLevel()
            episode_stats['development_stage'] = agent.getDevelopmentalStage()
            
            if action != pynlm.MotorCommand.Wait:
                episode_stats['actions_taken'] += 1
        
        # Record episode statistics
        training_stats['episodes'].append(episode)
        training_stats['firing_rates'].append(brain.getAverageFiringRate())
        training_stats['rewards'].append(episode_stats['total_reward'])
        training_stats['curiosity_levels'].append(episode_stats['curiosity_level'])
        training_stats['actions_taken'] += episode_stats['actions_taken']
        training_stats['development_stages'].append(episode_stats['development_stage'])
        
        # Print progress
        if episode % 5 == 0:
            print(f"  Episode {episode}: "
                  f"Reward={episode_stats['total_reward']:.2f}, "
                  f"Firing={brain.getAverageFiringRate():.3f}, "
                  f"Curiosity={episode_stats['curiosity_level']:.3f}")
        
        # Reset for next episode
        brain.reset()
        world.reset()
        agent.reset()
    
    print(f"\nTraining completed!")
    print(f"Average firing rate: {np.mean(training_stats['firing_rates']):.3f} Hz")
    print(f"Total actions: {training_stats['actions_taken']}")
    
    return brain, agent, training_stats

# Run complete training pipeline
brain, agent, stats = complete_training_pipeline(10, 3000)

# Visualize results
fig, axes = plt.subplots(2, 2, figsize=(12, 10))

# Plot 1: Training progress
axes[0, 0].plot(stats['episodes'], stats['firing_rates'], 'b-', linewidth=2, label='Firing Rate')
axes[0, 0].set_title('Training Progress: Firing Rate')
axes[0, 0].set_xlabel('Episode')
axes[0, 0].set_ylabel('Firing Rate (Hz)')
axes[0, 0].legend()
axes[0, 0].grid(True, alpha=0.3)

# Plot 2: Reward accumulation
axes[0, 1].plot(stats['episodes'], np.cumsum(stats['rewards']), 'g-', linewidth=2, label='Cumulative Reward')
axes[0, 1].set_title('Training Progress: Reward Accumulation')
axes[0, 1].set_xlabel('Episode')
axes[0, 1].set_ylabel('Cumulative Reward')
axes[0, 1].legend()
axes[0, 1].grid(True, alpha=0.3)

# Plot 3: Curiosity levels
axes[1, 0].plot(stats['episodes'], stats['curiosity_levels'], 'r-', linewidth=2, label='Curiosity')
axes[1, 0].set_title('Training Progress: Curiosity Levels')
axes[1, 0].set_xlabel('Episode')
axes[1, 0].set_ylabel('Curiosity Level')
axes[1, 0].legend()
axes[1, 0].grid(True, alpha=0.3)

# Plot 4: Actions per episode
axes[1, 1].bar(stats['episodes'], [1] * len(stats['episodes']), 
               color='skyblue', edgecolor='black', alpha=0.7)
axes[1, 1].set_title('Training Progress: Actions per Episode')
axes[1, 1].set_xlabel('Episode')
axes[1, 1].set_ylabel('Actions')
axes[1, 1].grid(True, alpha=0.3, axis='y')

plt.tight_layout()
plt.savefig('training_progress.png', dpi=150, bbox_inches='tight')
plt.show()

print("\nTraining visualization saved to training_progress.png")
```

### Quick Reference

#### Core Imports

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
```

#### Configuration

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Get configuration values
neuron_count = config.get("brain.neuron_count")
```

#### Brain Operations

```python
# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Core operations
brain.step(0)                    # Take simulation step
brain.reset()                    # Reset brain state
brain.getTotalNeuronCount()     # Get neuron count
brain.getAverageFiringRate()    # Get firing rate
brain.getTotalSpikeCount()      # Get spike count
brain.getDevelopmentalStage()  # Get developmental stage
```

#### Agent Operations

```python
# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Core operations
agent.processSensoryInput(percept)     # Process sensory input
agent.decodeMotorCommand()             # Get motor command
agent.applyRewardModulation(reward, predicted)  # Apply reward
agent.updateDevelopment(timestep)       # Update development
agent.getCuriosityLevel()               # Get curiosity level
```

#### World Operations

```python
# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# World operations
world.update(0.1)                      # Update world
world.reset()                           # Reset world
world.getSensoryPercept()              # Get sensory percept
world.getAgentBody()                   # Get agent body
world.applyMotorCommand(action, time)  # Apply action
```

### Common Patterns

#### Pattern 1: Complete Simulation Loop

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # Update environment
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning if enabled
        if hasattr(agent, 'applyRewardModulation'):
            agent.applyRewardModulation(0.5, 0.3)
    
    return brain.getAverageFiringRate()
```

#### Pattern 2: Training with Evaluation

```python
def train_and_evaluate(brain, world, agent, train_episodes, test_episodes):
    # Training phase
    training_stats = []
    for episode in range(train_episodes):
        firing_rate = run_simulation(brain, world, agent, 100)
        training_stats.append(firing_rate)
        
        # Reset for next episode
        brain.reset()
        world.reset()
        agent.reset()
    
    # Evaluation phase
    evaluation_stats = []
    for episode in range(test_episodes):
        firing_rate = run_simulation(brain, world, agent, 100)
        evaluation_stats.append(firing_rate)
        
        # Reset for next episode
        brain.reset()
        world.reset()
        agent.reset()
    
    return training_stats, evaluation_stats
```

#### Pattern 3: Development Tracking

```python
def track_development(brain, num_steps):
    development_stages = []
    firing_rates = []
    
    for step in range(num_steps):
        brain.step(step)
        development_stages.append(brain.getDevelopmentalStage())
        firing_rates.append(brain.getAverageFiringRate())
    
    return development_stages, firing_rates
```

### Installation and Setup

#### Development Installation

```bash
# Install with all development dependencies
pip install -e .[dev]

# Or install individual components
pip install pybind11 scikit-build-core pytest numpy
pip install -e .
```

#### Build from Source

```bash
# Build enhanced Python bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=ON -DNLM_ENABLE_ADVANCED_FEATURES=ON
make -j$(nproc)

# Install
make install
```

#### Quick Test

```bash
# Run quick test of Python bindings
python -c "import pynlm; print('NLM version:', pynlm.__version__); print('Import successful!')"

# Run comprehensive tests
pytest tests/ -v

# Run examples
python -m examples.advanced_usage
python -m examples.tutorial
```

### Troubleshooting

#### Common Issues

**Import Error:**
```bash
pip install --force-reinstall .
```

**Build Errors:**
- Ensure CMake >= 3.16
- Check compiler supports C++20
- Verify pybind11 installation

**Runtime Errors:**
- Ensure `brain.initialize()` is called before `brain.step()`
- Check configuration parameters are valid
- Verify agent is properly initialized with world

#### Performance Optimization

```python
# For large-scale simulations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # Adjust based on memory

# Profile performance
import cProfile, pstats
brain = pynlm.createBrain(config)
brain.initialize()

profiler = cProfile.Profile()
profiler.enable()
for i in range(100):
    brain.step(i)
profiler.disable()

stats = pstats.Stats(profiler).sort_stats('cumulative')
stats.print_stats(20)  # Top 20 time-consuming functions
```

### Resources

#### Documentation

- [API Reference](docs/API.md) - Complete Python API documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [ROADMAP.md](docs/ROADMAP.md) - Development roadmap

#### Examples and Tutorials

- [examples/advanced_usage.py](examples/advanced_usage.py) - Advanced usage patterns
- [examples/tutorial.py](examples/tutorial.py) - Step-by-step tutorial
- [benchmarks/performance_test.py](benchmarks/performance_test.py) - Performance benchmarks

#### Community and Support

- [GitHub Repository](https://github.com/nlm-project/nlm) - Source code and issues
- [Documentation Website](https://nlm.readthedocs.io) - Online documentation
- [Discussions](https://github.com/nlm-project/nlm/discussions) - Community discussions

#### Testing and Validation

Run the comprehensive test suite:

```bash
# Run Python binding tests
pytest tests/test_pynlm_bindings.py -v --cov=pynlm

# Run C++ tests
ctest --output-on-failure

# Run all tests
pytest tests/ -v --cov=pynlm
```

### Acknowledgements

This enhanced Python bindings implementation builds upon the original NLM framework and provides:

- Comprehensive error handling and validation
- Advanced convenience functions and APIs
- Performance monitoring and benchmarking tools
- Extensive documentation and examples
- Comprehensive test coverage
- Memory and resource management utilities
- Development system integration
- Neuromodulation support
- Memory system access
- Visualization and analysis tools

The enhanced bindings maintain full compatibility with the original NLM API while adding new features and improving usability for both beginners and advanced users.

---

**Last Updated:** Current version with enhanced Python bindings and advanced features
**Version:** 0.1.0 (Enhanced)
**License:** MIT

For more information, visit the [NLM project documentation](https://nlm.readthedocs.io) or [GitHub repository](https://github.com/nlm-project/nlm).
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
