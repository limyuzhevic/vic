# NLM (Neural Learning Machine) - Comprehensive Python API Reference

## Overview

NLM is a brain-inspired spiking neural network simulator that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document provides a comprehensive guide to using the NLM Python API, with detailed explanations of all classes, methods, and configuration options.

---

## Table of Contents

1. [Python Installation and Setup](#part-1-python-installation-and-setup)
2. [Brain Class API Reference](#part-2-brain-class-api-reference)
3. [Config Class API Reference](#part-3-config-class-api-reference)
4. [AgentBrain Class API Reference](#part-4-agentbrain-class-api-reference)
5. [SimpleWorld Class API Reference](#part-5-simpleworld-class-api-reference)
6. [Enumerations](#part-6-enumerations)
7. [Configuration Options](#part-7-configuration-options)
8. [Example Scripts](#part-8-example-scripts)
9. [Troubleshooting](#part-9-troubleshooting)

---

## Part 1: Python Installation and Setup

### Prerequisites

- **Python**: 3.8+
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **pybind11**: Version 2.11.0+ (automatically included)

### Installation Methods

#### Method 1: Standard pip Install (Recommended)

```bash
# Clone the repository
cd /path/to/nlm

# Install Python bindings
pip install .

# Or install in development mode (for editing)
pip install -e .
```

#### Method 2: Build from Source

```bash
# Install build dependencies
pip install scikit-build-core pybind11 numpy

# Build and install
python -m build
pip install dist/*.whl

# Or directly with pip
pip install .
```

#### Method 3: Using pre-built packages

```bash
# Check available versions
pip search pynlm

# Install specific version
pip install pynlm==[version]
```

### Verifying Installation

```python
import pynlm

# Check library version
print(f"NLM Python bindings version: {pynlm.__version__}")

# Create test brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Test successful!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Troubleshooting Installation

**Common Issues:**

1. **ImportError: No module named 'pynlm'**
   ```bash
   pip uninstall pynlm -y
   pip install --force-reinstall .
   ```

2. **Build errors with CMake**
   ```bash
   # On Linux/macOS
   sudo apt-get install build-essential cmake
   sudo yum install gcc-c++ cmake
   
   # On Windows (with Windows Subsystem for Linux)
   sudo apt-get update
   sudo apt-get install build-essential cmake
   ```

3. **Compiler not finding Python**
   ```bash
   cmake .. -DPython_EXECUTABLE=$(which python3)
   ```

---

## Part 2: Brain Class API Reference

The `Brain` class represents the neural network that processes information. It implements spiking neurons with LIF dynamics and multiple plasticity mechanisms.

### Brain Creation and Initialization

```python
# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize brain (required before any operations)
brain.initialize()
```

#### Important Brain Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Connectivity probability |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |

### Core Brain Methods

#### Simulation Control

| Method | Signature | Description |
|--------|-----------|-------------|
| `initialize()` | `None` | Initialize brain state |
| `step(step_number)` | `int` | Run one simulation step |
| `step(step_number, time)` | `int, float` | Run step with timestamp |
| `reset()` | `None` | Reset brain to initial state |
| `save(filename)` | `str` | Save brain state to file |
| `load(filename)` | `str` | Load brain state from file |

**Example:**
```python
brain.initialize()
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### Input/Output Operations

| Method | Signature | Description |
|--------|-----------|-------------|
| `receiveSensoryInput(input)` | `SensoryInput` | Inject sensory data |
| `produceAction()` | `Action` | Get motor action from brain |

**Example:**
```python
# Create sensory input (example values)
sensory_input = pynlm.SensoryInput()
sensory_input.addNeuron(0, 1.0)  # Excite neuron 0

# Send to brain
brain.receiveSensoryInput(sensory_input)

# Get action from brain activity
action = brain.produceAction()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### Statistics and Monitoring

| Method | Signature | Returns |
|--------|-----------|---------|
| `getTotalNeuronCount()` | `int` | Total neurons |
| `getTotalSynapseCount()` | `int` | Total synapses |
| `getFiringNeuronCount()` | `int` | Currently firing neurons |
| `getAverageFiringRate()` | `float` | Average firing rate (Hz) |
| `getTotalSpikeCount()` | `int` | Total spikes fired |
| `getExcitationInhibitionRatio()` | `float` | E/I balance ratio |
| `getDevelopmentStage()` | `DevelopmentalStage` | Current developmental stage |

**Example:**
```python
print(f"Network size: {brain.getTotalNeuronCount()} neurons")
print(f"Activity level: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development: {brain.getDevelopmentalStage().name}")
```

#### Region Management

| Method | Signature | Description |
|--------|-----------|-------------|
| `addRegion(name)` | `str` | Add new neural region |
| `getRegion(region_id)` | `int` | Get region by ID |
| `getRegionName(region_id)` | `int` | Get region name |

**Example:**
```python
cortex_id = brain.addRegion("cortex")
cortex = brain.getRegion(cortex_id)
print(f"Cortex region: {brain.getRegionName(cortex_id)}")
```

### Advanced Brain Configuration

#### Accessing and Modifying Configuration

```python
# Create brain and access its config
brain = pynlm.createBrain(pynlm.createDefaultConfig())
config = brain.getConfig()

# Query configuration values
if config.has("brain.neuron_count"):
    neuron_count = config.getInt("brain.neuron_count")
    print(f"Neurons: {neuron_count}")

# Modify configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -45.0)

# Save/load configuration
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")
```

#### Plasticity Configuration

```python
# Enable specific plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
```

---

## Part 3: Config Class API Reference

The `Config` class manages all configuration parameters for NLM components. It supports hierarchical key-value storage with JSON serialization.

### Basic Configuration Operations

| Method | Signature | Description |
|--------|-----------|-------------|
| `createDefaultConfig()` | `Config` | Create default configuration |
| `has(key)` | `str, bool` | Check if key exists |
| `getKeys()` | `list` | Get all configuration keys |
| `set(key, value)` | `str, value` | Set configuration value |

### Configuration Value Types

```python
config = pynlm.createDefaultConfig()

# Set different types of values
config.set("brain.neuron_count", 1000)        # Integer
config.set("brain.v_thresh", -50.0)           # Float
config.set("plasticity.stdp.enable", True)    # Boolean
config.set("world.name", "my_world")          # String

# Get values (with type checking)
neuron_count = config.getInt("brain.neuron_count")
v_thresh = config.getFloat("brain.v_thresh")
enabled = config.getBool("plasticity.stdp.enable")
name = config.getString("world.name")
```

### Hierarchical Configuration Structure

NLM uses a hierarchical key system:

| Category | Subcategory | Examples |
|----------|-------------|----------|
| `brain.` | Core brain settings | `brain.neuron_count`, `brain.v_thresh` |
| `plasticity.` | Learning mechanisms | `plasticity.stdp.enable`, `plasticity.hebbian.rate` |
| `neuromod.` | Neuromodulation | `neuromod.dopamine.scale`, `neuromod.curiosity.enable` |
| `memory.` | Memory systems | `memory.working.capacity`, `memory.episodic.enable` |
| `world.` | World simulation | `world.width`, `world.height`, `world.vision` |

### Configuration File I/O

```python
# Save configuration to JSON
config.saveToFile("brain_config.json")

# Load configuration from JSON
config.loadFromFile("brain_config.json")

# Create from JSON string
json_string = '{"brain.neuron_count": 1500, "plasticity.stdp.enable": true}'
config = pynlm.createDefaultConfig()
config.loadFromJsonString(json_string)
```

---

## Part 4: AgentBrain Class API Reference

The `AgentBrain` class provides the interface between the neural network (brain) and the simulation environment (world). It handles sensory input processing and motor output decoding.

### AgentBrain Creation and Initialization

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

### Sensory-Motor Interface Methods

| Method | Signature | Description |
|--------|-----------|-------------|
| `processSensoryInput(percept)` | `Percept` | Send world observations to brain |
| `decodeMotorCommand()` | `Action` | Get brain's action decision |
| `applyRewardModulation(reward, predicted)` | `float, float` | Learn from rewards |
| `updateDevelopment(timestep)` | `float` | Update developmental state |

### Subsystem Control

| Method | Signature | Description |
|--------|-----------|-------------|
| `enableRewardModulation(enable)` | `bool` | Enable/disable reinforcement learning |
| `enableStructuralPlasticity(enable)` | `bool` | Enable/disable network growth |
| `enableDevelopment(enable)` | `bool` | Enable/disable developmental processes |
| `enableCuriosity(enable)` | `bool` | Enable/disable curiosity-driven exploration |

### Agent Statistics and Monitoring

| Method | Signature | Returns |
|--------|-----------|---------|
| `getCuriosityLevel()` | `float` | Current curiosity drive (0-1) |
| `getNoveltyLevel()` | `float` | Novelty detection (0-1) |
| `getPredictionError()` | `float` | Prediction error signal |
| `getNeuromodulationLevel()` | `float` | Overall neuromodulation strength |
| `getWorkingMemorySize()` | `int` | Working memory capacity |

### Complete Agent Usage Pattern

```python
# 1. Create all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# 2. Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 3. Run complete simulation loop
for step in range(1000):
    # Update environment
    world.update(0.1)
    
    # Perceive environment
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Neural processing
    brain.step(step)
    
    # Decision making
    action = agent.decodeMotorCommand()
    
    # Act in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Monitor progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
```

---

## Part 5: SimpleWorld Class API Reference

The `SimpleWorld` class represents the simulation environment where the agent interacts. It provides sensory input generation and motor command execution.

### World Creation and Configuration

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions and parameters
world.configure(
    width=20,              # Map width in cells
    height=20,             # Map height in cells
    visionWidth=8,         # Agent's visual range (width)
    visionHeight=8,        # Agent's visual range (height)
    maxEnergy=100.0,       # Maximum agent energy
    energyDecayRate=0.01,  # Energy loss per timestep
    timeStep=0.1,          # Simulation timestep
    randomSeed=42          # Reproducible randomness
)

world.reset()  # Initialize world state
```

### World Control Methods

| Method | Signature | Description |
|--------|-----------|-------------|
| `reset()` | `None` | Reset world to initial state |
| `update(timestep)` | `float` | Advance simulation by timestep |
| `setAgentStart(x, y)` | `float, float` | Set agent starting position |
| `applyMotorCommand(action, time)` | `Action, float` | Execute agent's action |
| `setRandomSeed(seed)` | `int` | Set random seed for reproducibility |

### State Access Methods

| Method | Signature | Returns |
|--------|-----------|---------|
| `getSensoryPercept()` | `Percept` | Current sensory input |
| `getAgentBody()` | `AgentBody` | Agent's physical state |
| `getSimulationTime()` | `float` | Current simulation time |
| `getWidth()` | `int` | World width |
| `getHeight()` | `int` | World height |
| `getMaxEnergy()` | `float` | Maximum agent energy |

### World Object Management

| Method | Signature | Description |
|--------|-----------|-------------|
| `addObject(obj)` | `WorldObject` | Add object to world |
| `removeObject(x, y)` | `int, int` | Remove object at position |
| `isValidPosition(x, y)` | `int, int` | Check if position is valid |

### World Object Types

```python
# Create different types of world objects
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
hazard = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, damage=5.0)
wall = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Wall)
marker = pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Marker)

# Add to world
world.addObject(resource)
world.addObject(hazard)
world.addObject(wall)
world.addObject(marker)
```

### Action Types

The world can generate various action types for the agent:

| Action Type | Description |
|-------------|-------------|
| `MoveForward` | Move forward one cell |
| `TurnLeft` | Rotate 90 degrees left |
| `TurnRight` | Rotate 90 degrees right |
| `Interact` | Interact with object at current position |
| `Eat` | Consume resource at current position |
| `Wait` | Stay in place for one timestep |

**Example:**
```python
action = world.createAction(pynlm.ActionType.MoveForward)
world.applyMotorCommand(action, world.getSimulationTime())
```

---

## Part 6: Enumerations

### Neuron Types

| Enumeration | Values |
|-------------|--------|
| `pynlm.NeuronType` | `Excitationary`, `Inhibitory`, `Sensory`, `Motor`, `Modulatory`, `Internal` |

### Developmental Stages

| Enumeration | Description |
|-------------|-------------|
| `pynlm.DevelopmentalStage` | `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging` |

### Action Types

| Enumeration | Values |
|-------------|--------|
| `pynlm.ActionType` | `MoveForward`, `TurnLeft`, `TurnRight`, `Interact`, `Eat`, `Wait`, `[more...]` |

### World Object Types

| Enumeration | Values |
|-------------|--------|
| `pynlm.WorldObjectType` | `Empty`, `Resource`, `Hazard`, `Wall`, `Marker`, `[more...]` |

---

## Part 7: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the network |
| `brain.synapse_density` | float | 0.1 | Probability of connection between neurons |
| `brain.connection_probability` | float | 0.05 | STDP connection probability |
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
| `plasticity.stdp.enable` | bool | true | Enable spike-timing dependent plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for potentiation |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for depression |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity (network growth) |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.capacity` | int | 100 | Working memory capacity |
| `memory.episodic.enable` | bool | true | Enable episodic memory |
| `memory.associative.enable` | bool | true | Enable associative memory |

### World Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `world.width` | int | 20 | World map width |
| `world.height` | int | 20 | World map height |
| `world.vision_width` | int | 8 | Agent's visual range width |
| `world.vision_height` | int | 8 | Agent's visual range height |
| `world.max_energy` | float | 100.0 | Maximum agent energy |
| `world.energy_decay` | float | 0.01 | Energy decay rate per timestep |

---

## Part 8: Example Scripts

### Complete Agent Tutorial

This script demonstrates building and running a complete agent with all subsystems:

```python
import pynlm
import time
import json

def create_advanced_agent(config_file=None):
    """Create an advanced agent with learning capabilities."""
    
    # 1. Load or create configuration
    if config_file:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
    else:
        config = pynlm.createDefaultConfig()
        # Configure for advanced learning
        config.set("brain.neuron_count", 2000)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("memory.episodic.enable", True)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, height=30,  # Larger world
        visionWidth=12, visionHeight=12,  # Wider vision
        maxEnergy=200.0,  # More energy
        energyDecayRate=0.005,  # Slower decay
        randomSeed=42  # Reproducible
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)  # Center start
    
    # 5. Initialize agent
    agent.initialize(world)
    
    # 6. Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world

def run_learning_experiment(num_episodes=10, steps_per_episode=500):
    """Run a complete learning experiment."""
    
    print("Starting NLM Learning Experiment")
    print("=" * 50)
    
    # Create advanced agent
    brain, agent, world = create_advanced_agent()
    
    # Run episodes
    episode_stats = []
    
    for episode in range(num_episodes):
        print(f"\nEpisode {episode + 1}/{num_episodes}")
        world.reset()
        
        episode_rewards = []
        episode_steps = []
        
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Perceive environment
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Neural processing
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Act in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            episode_rewards.append(reward)
            
            # Learn from reward
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log progress
            if step % 100 == 0:
                print(f"  Step {step}: Reward={reward:.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
        
        # Episode statistics
        avg_reward = sum(episode_rewards) / len(episode_rewards)
        max_reward = max(episode_rewards)
        
        episode_stats.append({
            'episode': episode + 1,
            'avg_reward': avg_reward,
            'max_reward': max_reward,
            'total_steps': steps_per_episode,
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_stage': brain.getDevelopmentalStage().name
        })
        
        print(f"  Episode complete: Avg Reward={avg_reward:.3f}")
    
    # Print summary
    print("\n" + "=" * 50)
    print("EXPERIMENT SUMMARY")
    print("=" * 50)
    
    for stats in episode_stats:
        print(f"Episode {stats['episode']}:")
        print(f"  Average Reward: {stats['avg_reward']:.3f}")
        print(f"  Maximum Reward: {stats['max_reward']:.3f}")
        print(f"  Final Curiosity: {stats['final_curiosity']:.3f}")
        print(f"  Final Novelty: {stats['final_novelty']:.3f}")
        print(f"  Final Development Stage: {stats['final_stage']}")
    
    # Save results
    with open('experiment_results.json', 'w') as f:
        json.dump(episode_stats, f, indent=2)
    
    print("\nResults saved to 'experiment_results.json'")
    return episode_stats

# Run the experiment
if __name__ == "__main__":
    start_time = time.time()
    stats = run_learning_experiment(num_episodes=5, steps_per_episode=300)
    end_time = time.time()
    
    print(f"\nExperiment completed in {end_time - start_time:.2f} seconds")
```

### Development Stage Tutorial

This script demonstrates how to use the developmental stages:

```python
import pynlm

def create_developing_brain():
    """Create a brain that develops over time."""
    
    # Configure for development
    config = pynlm.createDefaultConfig()
    config.set("brain.development.enable", True)
    config.set("brain.development.critical_period", 100)
    config.set("brain.development.maturation_rate", 0.8)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Track development
    development_log = []
    
    for step in range(500):
        brain.step(step)
        
        # Record development stage
        stage = brain.getDevelopmentalStage()
        stage_index = stage.value  # Use enum value
        
        development_log.append({
            'step': step,
            'stage': stage.name,
            'stage_index': stage_index,
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_rate': brain.getAverageFiringRate()
        })
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: {stage.name} stage - "
                  f"{brain.getFiringNeuronCount()} neurons firing")
    
    return brain, development_log

def analyze_development(log):
    """Analyze brain development over time."""
    
    stages = {}
    for entry in log:
        stage_name = entry['stage']
        if stage_name not in stages:
            stages[stage_name] = []
        stages[stage_name].append(entry)
    
    print("\nDevelopment Analysis:")
    print("-" * 40)
    
    for stage_name, entries in stages.items():
        duration = len(entries)
        final_firing = entries[-1]['firing_neurons']
        avg_rate = sum(e['average_rate'] for e in entries) / len(entries)
        
        print(f"{stage_name}: {duration} steps, "
              f"final firing: {final_firing}, "
              f"avg rate: {avg_rate:.2f} Hz")

# Run development analysis
brain, log = create_developing_brain()
analyze_development(log)
```

### Memory Systems Tutorial

This script demonstrates the memory systems:

```python
import pynlm

def create_memory_brain():
    """Create brain with all memory systems enabled."""
    
    config = pynlm.createDefaultConfig()
    config.set("memory.working.capacity", 200)
    config.set("memory.episodic.enable", True)
    config.set("memory.associative.enable", True)
    config.set("memory.episodic.capacity", 50)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    return brain, agent, world

def run_memory_test(brain, agent, world, steps=200):
    """Test all memory systems."""
    
    memory_stats = {
        'working_memory': [],
        'episodic_memory': [],
        'associative_memory': []
    }
    
    print("Testing Memory Systems")
    print("-" * 40)
    
    for step in range(steps):
        # Update environment
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural processing
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log memory statistics
        if step % 50 == 0:
            working_mem = brain.getWorkingMemoryCount()
            episodic_mem = brain.getEpisodicMemoryCount()
            associative_mem = brain.getAssociativeMemoryCount()
            
            memory_stats['working_memory'].append(working_mem)
            memory_stats['episodic_memory'].append(episodic_mem)
            memory_stats['associative_memory'].append(associative_mem)
            
            print(f"Step {step}:")
            print(f"  Working Memory: {working_mem} items")
            print(f"  Episodic Memory: {episodic_mem} episodes")
            print(f"  Associative Memory: {associative_mem} associations")
    
    return memory_stats

# Run memory test
brain, agent, world = create_memory_brain()
stats = run_memory_test(brain, agent, world)
```

---

## Part 9: Troubleshooting

### Common Issues and Solutions

**1. Brain Initialization Problems**

*Problem:* Brain doesn't initialize properly
*Solution:*
```python
# Always initialize brain before use
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # CRITICAL - this line!
```

**2. Agent Not Receiving Sensory Input**

*Problem:* Agent's percept is always empty
*Solution:*
```python
# Check world configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Verify agent initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Agent needs world reference
```

**3. High Memory Usage**

*Problem:* Memory consumption is too high
*Solutions:*
```python
# Reduce configuration parameters
config.set("brain.neuron_count", 500)  # Default: 1000
config.set("brain.synapse_density", 0.05)  # Default: 0.1
config.set("memory.working.capacity", 50)  # Default: 100
```

**4. Slow Simulation Performance**

*Problem:* Simulation runs too slow
*Solutions:*
```python
# Optimize configuration
config.set("brain.tau_mem", 10.0)  # Faster membrane time constant
config.set("brain.tau_ref", 1.0)  # Shorter refractory period
world.configure(width=10, height=10)  # Smaller world
```

**5. Import Error**

*Problem:* Cannot import pynlm module
*Solution:*
```bash
# Reinstall the module
pip uninstall pynlm -y
pip install -e .
```

**6. Configuration File Issues**

*Problem:* Configuration file not loading properly
*Solution:*
```python
# Check JSON syntax
import json
try:
    with open("config.json", "r") as f:
        json.load(f)  # Valid JSON
    config.loadFromFile("config.json")
except json.JSONDecodeError as e:
    print(f"Invalid JSON: {e}")
except FileNotFoundError:
    print("Configuration file not found")
```

### Performance Tips

1. **For large simulations:**
   ```python
   # Use smaller configurations
   config.set("brain.neuron_count", 500)
   config.set("world.width", 15)
   config.set("world.height", 15)
   ```

2. **For detailed monitoring:**
   ```python
   # Enable detailed statistics
   config.set("debug.enable", True)
   config.set("debug.logging_level", "INFO")
   ```

3. **For reproducibility:**
   ```python
   # Set random seed
   world.setRandomSeed(42)
   ```

4. **For long-running simulations:**
   ```python
   # Save checkpoints
   for step in range(10000):
       brain.step(step)
       if step % 1000 == 0:
           brain.save(f"checkpoint_step_{step}.bin")
   ```

### Error Messages Reference

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Brain not initialized" | Calling `brain.step()` without `brain.initialize()` | Always call `initialize()` first |
| "Invalid configuration key" | Setting unknown configuration key | Check configuration key format |
| "World not configured" | Using world without calling `configure()` | Always call `configure()` before `reset()` |
| "Agent not initialized" | Using agent without calling `initialize()` | Call `agent.initialize(world)` |
| "Memory capacity exceeded" | Memory system full | Increase memory capacity in config |

---

## Further Reading

### Documentation Resources

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results
- [ROADMAP.md](docs/ROADMAP.md) - Project roadmap and future plans

### Example Scripts

- `nlm_demo.py` - Comprehensive demonstration of all features
- `test_import.py` - Basic functionality tests
- `nlm_cli.py` - Command-line interface examples

### Additional Resources

- [NLM GitHub Repository](https://github.com/[repository]) - Source code and issues
- [NLM Documentation Wiki](https://[wiki]/wiki) - Community contributions
- [NLM Research Papers](https://[papers]/author/nlm) - Scientific publications

---

## Version Information

**Current Version:** NLM Phase 6 Final Integration
**Python Bindings:** Compatible with Python 3.8+
**API Stability:** Stable - no breaking changes planned in Phase 6
**License:** MIT

---

*This documentation is continuously updated. For the latest version, visit the NLM documentation site.*
