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

## Part 6: Advanced Configuration Options

### Advanced Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of neural regions |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Advanced Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.growth_rate` | float | 0.01 | Rate of structural plasticity growth |
| `plasticity.structural.pruning_rate` | float | 0.005 | Rate of structural pruning |

### Advanced Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.exploration_rate` | float | 0.5 | Rate of curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.3 | Novelty detection threshold |

### Advanced Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.time_step` | float | 1.0 | Simulation time step (ms) |
| `simulation.max_steps` | int | 1000 | Maximum simulation steps |
| `simulation.checkpoint_interval` | int | 100 | Steps between checkpoint saves |
| `simulation.logging_level` | int | 2 | Logging verbosity (0-3) |

---

## Part 7: Advanced Examples

### Complex Agent with Learning

```python
import pynlm

def create_advanced_agent(config_file=None):
    """Create an advanced agent with all learning subsystems."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    # 2. Configure for advanced learning
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    # 3. Create and initialize components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world with sensory capabilities
    world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
    world.setRandomSeed(42)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    agent.initialize(world)
    
    # 4. Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world

def run_advanced_training(brain, agent, world, num_episodes=100):
    """Run advanced training with learning monitoring."""
    
    for episode in range(num_episodes):
        # Reset for new episode
        world.reset()
        brain.reset()
        agent.reset()
        
        episode_reward = 0.0
        episode_steps = 0
        
        # Run episode
        for step in range(1000):
            # 1. Get sensory input
            percept = world.getSensoryPercept()
            
            # 2. Process and predict
            agent.processSensoryInput(percept)
            prediction = agent.getPredictionError()
            
            # 3. Make action
            action = agent.decodeMotorCommand()
            
            # 4. Apply action
            result = world.applyMotorCommand(action, world.getSimulationTime())
            episode_reward += result.reward
            episode_steps += 1
            
            # 5. Apply learning
            if result.is_good():
                agent.applyRewardModulation(result.reward, prediction)
            elif result.is_bad():
                agent.applyRewardModulation(-result.reward, prediction)
            
            # 6. Update development
            agent.updateDevelopment(0.1)
            
            # 7. Step brain
            brain.step(step, world.getSimulationTime())
            
            # 8. Update world
            world.update(0.1)
            
            # 9. Progress monitoring
            if step % 100 == 0:
                print(f"Episode {episode}: Step {step}, Reward: {result.reward:.3f}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")
        
        # Episode summary
        print(f"Episode {episode} complete: Total Reward={episode_reward:.2f}, "
              f"Steps={episode_steps}, Final Stage={brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Usage example
brain, agent, world = create_advanced_agent()
brain, agent, world = run_advanced_training(brain, agent, world, num_episodes=10)
```

### Batch Processing for Performance

```python
import pynlm
import time

def batch_simulation(neuron_counts, steps_per_brain):
    """Run multiple simulations with different brain sizes."""
    
    results = []
    
    for neuron_count in neuron_counts:
        # Create configuration for this size
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.1)
        
        # Time the brain creation and initialization
        start_time = time.time()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        creation_time = time.time() - start_time
        
        # Run simulation
        simulation_start = time.time()
        for step in range(steps_per_brain):
            brain.step(step)
        
        simulation_time = time.time() - simulation_start
        
        # Record metrics
        results.append({
            'neuron_count': neuron_count,
            'creation_time': creation_time,
            'simulation_time': simulation_time,
            'steps_per_second': steps_per_brain / simulation_time,
            'neurons_per_second': (neuron_count * steps_per_brain) / simulation_time
        })
        
        print(f"Brain with {neuron_count} neurons: "
              f"{steps_per_brain/simulation_time:.0f} steps/sec, "
              f"{neuron_count*steps_per_brain/simulation_time:.0f} neurons/sec")
    
    return results

# Benchmark different brain sizes
neuron_sizes = [100, 1000, 5000, 10000]
steps = 1000
benchmarks = batch_simulation(neuron_sizes, steps)
```

### Memory-Efficient Simulation

```python
import pynlm
import gc
import psutil
import os

def monitor_memory():
    """Get current memory usage in MB."""
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / 1024 / 1024

def memory_efficient_simulation(neuron_count=1000, steps=5000, checkpoint_interval=1000):
    """Run simulation with memory monitoring and checkpointing."""
    
    print("Starting memory-efficient simulation...")
    initial_memory = monitor_memory()
    print(f"Initial memory: {initial_memory:.1f} MB")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.05)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run in chunks to monitor memory
    chunk_size = min(1000, steps)
    
    for chunk in range(0, steps, chunk_size):
        chunk_start = monitor_memory()
        
        for step in range(chunk, min(chunk + chunk_size, steps)):
            brain.step(step)
        
        chunk_end = monitor_memory()
        chunk_memory = chunk_end - chunk_start
        
        print(f"Chunk {chunk//1000 + 1}: Memory {chunk_end:.1f} MB "
              f"(+{chunk_memory:.1f} MB), Step {step}")
        
        # Save checkpoint if needed
        if step % checkpoint_interval == 0:
            checkpoint_file = f"checkpoint_{step}.bin"
            brain.save(checkpoint_file)
            print(f"  Saved checkpoint: {checkpoint_file}")
        
        # Force garbage collection if memory is getting high
        if chunk_end > initial_memory + 200:  # 200MB above initial
            gc.collect()
            print("  Forced garbage collection")
        
        # Brief pause to stabilize memory
        import time
        time.sleep(0.1)
    
    final_memory = monitor_memory()
    total_memory = final_memory - initial_memory
    
    print(f"\nSimulation complete!")
    print(f"Total memory used: {total_memory:.1f} MB")
    print(f"Peak memory: {max(m for m in [initial_memory, final_memory]):.1f} MB")
    print(f"Memory efficiency: {neuron_count}/{total_memory:.0f} neurons/MB")
    
    return brain

# Run memory-efficient simulation
brain = memory_efficient_simulation(neuron_count=5000, steps=2000)
```

---

## Part 8: API Reference

### Core Classes

#### Brain Class
**Description**: Central neural simulation brain class containing all neurons and synapses.

**Creation**:
```python
brain = pynlm.createBrain(config)
brain.initialize()
```

**Key Methods**:
- `brain.step(step)` - Perform a simulation step
- `brain.step(step, time)` - Perform step with timestamp
- `brain.step_many(steps, timestep=1.0)` - Perform multiple steps
- `brain.reset()` - Reset brain state
- `brain.receiveSensoryInput(input)` - Inject sensory data
- `brain.produceAction()` - Get motor action
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state
- `brain.getTotalNeuronCount()` - Get total neuron count
- `brain.getTotalSynapseCount()` - Get total synapse count
- `brain.getActiveNeuronCount()` - Get active neuron count
- `brain.getFiringNeuronCount()` - Get firing neuron count
- `brain.getAverageFiringRate()` - Get average firing rate
- `brain.getExcitationInhibitionRatio()` - Get E/I balance
- `brain.getTotalSpikeCount()` - Get total spike count
- `brain.getDevelopmentalStage()` - Get developmental stage

#### Config Class
**Description**: Configuration class for NLM system parameters.

**Creation**:
```python
config = pynlm.createDefaultConfig()
```

**Key Methods**:
- `config.loadFromFile(filepath)` - Load from JSON file
- `config.saveToFile(filepath)` - Save to JSON file
- `config.set(key, value)` - Set configuration value
- `config.has(key)` - Check if key exists
- `config.getKeys()` - Get all keys
- `config.get(key)` - Get configuration value

#### AgentBrain Class
**Description**: Agent brain interface connecting NLM brain to world.

**Creation**:
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

**Key Methods**:
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Get motor command
- `agent.applyRewardModulation(reward, predicted_reward)` - Apply reward learning
- `agent.updateDevelopment(timestep)` - Update development
- `agent.enableRewardModulation(enable)` - Enable/disable reward modulation
- `agent.enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `agent.enableDevelopment(enable)` - Enable/disable development
- `agent.enableCuriosity(enable)` - Enable/disable curiosity
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getPredictionError()` - Get prediction error
- `agent.getNeuromodulationLevel()` - Get neuromodulation level

#### SimpleWorld Class
**Description**: Simple 2D world for NLM simulation with agent interaction.

**Creation**:
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

**Key Methods**:
- `world.reset()` - Reset world state
- `world.update(timestep)` - Update world
- `world.setAgentStart(x, y)` - Set agent starting position
- `world.applyMotorCommand(cmd, currentTime)` - Apply motor command
- `world.getSensoryPercept()` - Get sensory percept
- `world.getAgentBody()` - Get agent body state
- `world.addObject(obj)` - Add world object
- `world.removeObject(x, y)` - Remove object
- `world.isValidPosition(x, y)` - Check if position is valid
- `world.getWidth()`, `world.getHeight()` - Get world dimensions

### Enumerations

#### NeuronType
- `Excitatory`: Excitatory neuron
- `Inhibitory`: Inhibitory neuron
- `Modulatory`: Modulatory neuron
- `Sensory`: Sensory neuron
- `Motor`: Motor neuron
- `Internal`: Internal neuron

#### SynapseType
- `Excitatory`: Excitatory synapse
- `Inhibitory`: Inhibitory synapse
- `Modulatory`: Modulatory synapse
- `Electrical`: Electrical synapse
- `GapJunction`: Gap junction

#### DevelopmentalStage
- `Initial`: Brain is just born
- `CriticalPeriod`: High learning flexibility
- `Maturation`: Brain matures
- `Adult`: Fully mature
- `Aging`: Brain ages

#### ActionType
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `MoveLeft`: Move left
- `MoveRight`: Move right
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `Look`: Look in current direction
- `LookUp`: Look up
- `LookDown`: Look down
- `Interact`: Interact with object
- `Eat`: Eat object
- `Drink`: Drink object
- `Rest`: Rest
- `Wait`: Wait
- `Custom`: Custom action

#### MotorCommand
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `LookLeft`: Look left
- `LookRight`: Look right
- `Interact`: Interact
- `Wait`: Wait

#### WorldObjectType
- `Empty`: Empty space
- `Resource`: Resource object
- `Hazard`: Hazard object
- `Wall`: Wall object
- `Marker`: Marker object

---

## Part 9: Error Handling and Troubleshooting

### Common Errors

#### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

#### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

**Configuration errors:**
- Use `config.has(key)` to check keys before accessing
- Validate configuration parameters
- Use try-catch blocks for file operations

### Debugging Techniques

#### Basic Debugging
```python
import pynlm

# Create brain with logging
config = pynlm.createDefaultConfig()
config.set("simulation.logging_level", 3)  # Enable verbose logging

brain = pynlm.createBrain(config)
brain.initialize()

# Run with debug prints
for step in range(100):
    brain.step(step)
    
    # Check for issues
    if brain.getFiringNeuronCount() == 0 and step > 10:
        print("Warning: No neurons firing after initialization")
    
    # Print status periodically
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Performance Debugging
```python
import time
import pynlm

def profile_simulation(brain, steps=1000):
    """Profile simulation performance."""
    
    # Time the simulation
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"Simulation completed in {total_time:.2f} seconds")
    print(f"Performance: {steps/total_time:.0f} steps/second")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Spikes: {brain.getTotalSpikeCount()}")
    
    return {
        'total_time': total_time,
        'steps_per_second': steps / total_time,
        'neurons_per_second': (brain.getTotalNeuronCount() * steps) / total_time
    }
```

#### Memory Debugging
```python
import pynlm
import gc

def check_memory_usage(brain, label=""):
    """Check memory usage of brain."""
    import sys
    
    # Force garbage collection
    gc.collect()
    
    # Get memory usage
    neurons = brain.getTotalNeuronCount()
    synapses = brain.getTotalSynapseCount()
    
    print(f"{label}Brain: {neurons} neurons, {synapses} synapses")
    print(f"{label}Estimated memory: {(neurons * 64 + synapses * 32) / 1024 / 1024:.1f} MB")
    
    return (neurons * 64 + synapses * 32) / 1024 / 1024

# Monitor memory during simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

memory_before = check_memory_usage(brain, "Before: ")

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        memory_after = check_memory_usage(brain, f"After step {step}: ")
```

### Advanced Troubleshooting

#### Learning Issues

**Problem**: Brain is not learning from rewards

**Solution**: Check reward modulation setup:
```python
# Enable reward modulation
agent.enableRewardModulation(True)

# Ensure rewards are being calculated correctly
reward = calculate_reward(world, agent)
prediction = agent.getPredictionError()
agent.applyRewardModulation(reward, prediction)

# Verify neuromodulation level
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

**Problem**: Agent is not exploring enough

**Solution**: Increase curiosity and novelty:
```python
# Enable curiosity and novelty
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Check curiosity level
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
```

**Problem**: Brain development is stuck

**Solution**: Check development stage:
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor development
for step in range(1000):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: Development stage = {stage}")
    
    if stage == pynlm.DevelopmentalStage.Aging:
        print("Warning: Brain reached aging stage early")
```

#### Performance Issues

**Problem**: Simulation is too slow

**Solution**: Optimize configuration:
```python
# Optimize for performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Smaller brain
config.set("brain.synapse_density", 0.05)  # Fewer synapses
config.set("plasticity.stdp.enable", False)  # Disable learning for speed
config.set("plasticity.hebbian.enable", False)
config.set("plasticity.structural.enable", False)

# Use batch processing
brain.step_many(1000, timestep=1.0)
```

**Problem**: Memory usage is too high

**Solution**: Reduce memory footprint:
```python
# Monitor memory
initial_memory = get_memory_usage()

# Clear spike history periodically
for neuron in brain.getNeurons():
    neuron.clearSpikeHistory()

# Save checkpoints for long simulations
if step % 1000 == 0:
    brain.save(f"checkpoint_{step}.bin")

# Force garbage collection
gc.collect()
```

---

## Part 10: Advanced Resources

### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

### Related Projects

- **Neuromorphic Computing**: Research in brain-inspired computing
- **Spiking Neural Networks**: SNN implementations and research
- **Artificial Life**: ALife simulations and behaviors
- **Neural Engineering**: Brain-computer interfaces

### Research Papers

- "Event-Driven Spiking Neural Networks" - Core NLM architecture
- "Adaptive Plasticity in Neural Systems" - Learning mechanisms
- "Memory-Efficient Brain Simulation" - Performance optimizations
- "Curiosity-Driven Learning" - Exploration behaviors

### Future Enhancements

Phase 6 and beyond may include:
- GPU acceleration
- Distributed simulation
- Advanced learning algorithms
- Real-time visualization
- Hardware acceleration

---

## Quick Reference

### Command Summary

| Task | Command | Description |
|------|---------|-------------|
| Create brain | `pynlm.createBrain(config)` | Create neural brain |
| Initialize brain | `brain.initialize()` | Start brain simulation |
| Run simulation | `brain.step(step)` | Single simulation step |
| Batch simulation | `brain.step_many(steps)` | Multiple simulation steps |
| Create agent | `pynlm.createAgentBrain(brain)` | Create agent interface |
| Create world | `pynlm.createSimpleWorld()` | Create simulation world |
| Get percept | `world.getSensoryPercept()` | Get sensory input |
| Get action | `agent.decodeMotorCommand()` | Get motor command |
| Apply reward | `agent.applyRewardModulation(reward, prediction)` | Apply learning |
| Save state | `brain.save(filepath)` | Save brain state |
| Load state | `brain.load(filepath)` | Load brain state |

### Configuration Commands

| Config Key | Type | Description |
|------------|------|-------------|
| `brain.neuron_count` | int | Number of neurons |
| `brain.synapse_density` | float | Synapse density |
| `plasticity.stdp.enable` | bool | Enable STDP learning |
| `neuromod.curiosity.enable` | bool | Enable curiosity |
| `neuromod.novelty.enable` | bool | Enable novelty |

### Error Codes

| Error | Description | Solution |
|-------|-------------|----------|
| `NLMValidationError` | Invalid parameters | Check parameter values |
| `NLMFileError` | File operation failed | Check file permissions |
| `NLMRuntimeError` | Runtime error | Check initialization sequence |

---

## Quick Start Advanced

For advanced users, here's the quick start with all learning features:

```python
import pynlm

# Create advanced agent
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.setRandomSeed(42)
world.reset()
world.setAgentStart(15.0, 15.0)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

agent.initialize(world)

# Run advanced simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = 0.0  # Calculate based on environment
    prediction = agent.getPredictionError()
    agent.applyRewardModulation(reward, prediction)

print("Advanced simulation complete!")
print(f"Final stage: {brain.getDevelopmentalStage()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
```
