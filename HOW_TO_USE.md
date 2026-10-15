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

## Part 2.5: Enhanced Python API (NEW!)

The enhanced NLM Python API provides significant improvements over the original bindings:

### Key Improvements

1. **Better Error Handling**: Custom `NLMError` exceptions for more precise error handling
2. **Convenience Methods**: Ready-to-use methods for common operations
3. **Enhanced Configuration Presets**: Predefined configurations for different use cases
4. **Statistical Analysis Tools**: Built-in methods for performance monitoring
5. **High-Level Agent Interfaces**: Simplified agent interaction with learning automation
6. **Backward Compatibility**: Original API still available alongside enhanced features

### Installation with Enhanced Bindings

```bash
# Install with enhanced bindings (recommended)
pip install --force-reinstall . --no-build-isolation

# For development with enhanced bindings
pip install -e . --no-build-isolation

# Build only (without installing)
pip install build
python -m build

# To verify enhanced bindings were built
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### Enhanced API Usage

#### Basic Usage with Enhanced API

```python
import pynlm

# Create enhanced configuration with preset
config = pynlm.ConfigWrapper()
config.setNeuronCount(1000)
config.enablePlasticity()

# Create enhanced brain
brain = pynlm.createEnhancedBrain(config)
brain.initialize()

# Run simulation with progress logging
brain.runSteps(500, logProgress=True)

# Get statistics
stats = brain.getStatistics()
print(f"Average firing rate: {stats['average_firing_rate']}")
```

#### Agent with Enhanced Features

```python
import pynlm

# Setup with preset configuration
brain = pynlm.createBrainWithPreset("advanced")
brain.initialize()

# Create enhanced agent
agent = pynlm.createEnhancedAgent(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initializeWorld(world)

# Run episode with automatic learning
rewards = agent.runEpisode(world, maxSteps=1000, enableLearning=True)
print(f"Episode completed with {len(rewards)} steps")
```

### Presets Available

- **"beginner"**: 500 neurons, ideal for testing and learning
- **"advanced"**: 5000 neurons, good for realistic simulations
- **"research"**: 20000 neurons, for intensive research

### Error Handling

```python
import pynlm

try:
    config = pynlm.ConfigWrapper("my_config.json")
    brain = pynlm.createEnhancedBrain(config)
    brain.initialize()
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

---

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

## Part 3: Using the Enhanced NLM Python API

This section covers the enhanced Python API, which provides significant improvements over the original bindings.

### 3.1 Enhanced Configuration API

#### Enhanced Config Class

```python
import pynlm

# Create enhanced configuration
config = pynlm.ConfigWrapper()

# Convenience methods
config.setNeuronCount(1000)                    # Set neuron count
config.setSynapseDensity(0.1)                   # Set synapse density
config.setConnectionProbability(0.05)          # Set connection probability
config.enablePlasticity()                       # Enable all plasticity
config.enableNeuromodulation()                  # Enable neuromodulation

# Check configuration
if config.hasPlasticityEnabled():
    print("Plasticity is enabled")

# Get values
print(f"Neuron count: {config.getNeuronCount()}")
print(f"Synapse density: {config.getSynapseDensity()}")

# Save configuration
config.saveToFile("my_config.json")
```

#### Configuration Presets

```python
import pynlm

# Create configuration with preset
config = pynlm.createConfigWithPreset("advanced")
brain = pynlm.createBrainWithPreset("advanced")
brain.initialize()

print(f"Created brain with {config.getNeuronCount()} neurons")
```

### 3.2 Enhanced Brain API

#### Enhanced Brain Class

```python
import pynlm

# Create enhanced brain
config = pynlm.ConfigWrapper()
brain = pynlm.createEnhancedBrain(config)

# Initialize with all features enabled
brain.initialize(enableRewardModulation=True,
                 enableCuriosity=True,
                 enableDevelopment=True)

# Run simulation with progress logging
brain.runSteps(500, logProgress=True)

# Get comprehensive statistics
stats = brain.getStatistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Average firing rate: {stats['average_firing_rate']}")
print(f"Total spikes: {stats['total_spikes']}")
```

#### Development Features

```python
# Advance through developmental stages
brain.advanceDevelopmentalStage()
print(f"Current stage: {brain.getBrain().getDevelopmentalStage()}")

# Track development
for stage_name in pynlm.DevelopmentalStage.__members__:
    if stage_name == "Adult":
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
        print("Reached adult development stage")
        break
```

### 3.3 Enhanced Agent API

#### Enhanced Agent Class

```python
import pynlm

# Create enhanced agent
brain = pynlm.createBrainWithPreset("advanced")
brain.initialize()

agent = pynlm.createEnhancedAgent(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initializeWorld(world)

# Run episode with automatic learning
rewards = agent.runEpisode(world, maxSteps=500, enableLearning=True)

print(f"Episode completed with {len(rewards)} steps")
print(f"Total reward: {sum(rewards):.2f}")
print(f"Average reward: {sum(rewards) / len(rewards):.2f}")
```

#### Agent Statistical Analysis

```python
import pynlm

# Create and run agent
brain = pynlm.createBrainWithPreset("advanced")
brain.initialize()

agent = pynlm.createEnhancedAgent(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent.initializeWorld(world)

# Run multiple episodes
all_rewards = []
for episode in range(10):
    rewards = agent.runEpisode(world, maxSteps=200, enableLearning=True)
    all_rewards.append(rewards)
    print(f"Episode {episode + 1}: {sum(rewards):.2f} total reward")

# Calculate statistics
total_rewards = [sum(rewards) for rewards in all_rewards]
avg_reward = sum(total_rewards) / len(total_rewards)
best_reward = max(total_rewards)

print(f"\nTraining Statistics:")
print(f"Average reward per episode: {avg_reward:.2f}")
print(f"Best episode reward: {best_reward:.2f}")
print(f"Consistency (std dev): {sum([(r - avg_reward)**2 for r in total_rewards]) / len(total_rewards):.2f}")
```

### 3.4 Error Handling

```python
import pynlm

# Error handling examples

try:
    # Load configuration from file
    config = pynlm.ConfigWrapper("nonexistent_config.json")
except pynlm.NLMError as e:
    print(f"Configuration error: {e}")
    # Fallback to default
    config = pynlm.ConfigWrapper()

try:
    # Create and initialize brain
    brain = pynlm.createEnhancedBrain(config)
    if not brain.initialize():
        raise pynlm.NLMError("Brain initialization failed")
except pynlm.NLMError as e:
    print(f"Brain setup error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 3.5 Mixed Usage (Original + Enhanced API)

```python
import pynlm

# Use enhanced API for setup
config = pynlm.createConfigWithPreset("beginner")
brain = pynlm.createEnhancedBrain(config)
brain.initialize()

# Use original API for specific operations
original_brain = brain.getBrain()  # Get original C++ brain
print(f"Using original API: {original_brain.getTotalNeuronCount()} neurons")

# Use enhanced API for advanced features
stats = brain.getStatistics()
print(f"Enhanced API stats: {stats['total_spikes']} total spikes")

# Both APIs work together seamlessly
```

### 3.6 Statistical Analysis Tools

```python
import pynlm

# Create brain and get statistics
brain = pynlm.createBrainWithPreset("advanced")
brain.initialize()

# Basic statistics
stats = brain.getStatistics()
print("=== Basic Statistics ===")
for key, value in stats.items():
    if isinstance(value, float):
        print(f"{key}: {value:.3f}")
    else:
        print(f"{key}: {value}")

# Run simulation and track changes
print("\n=== Simulation Tracking ===")
previous_spikes = stats['total_spikes']
for step in range(100):
    brain.runSteps(10)
    stats = brain.getStatistics()
    current_spikes = stats['total_spikes']
    spike_increase = current_spikes - previous_spikes
    
    if step % 20 == 0:
        print(f"Step {step}: {spike_increase} new spikes, {stats['total_spikes']} total")
    
    previous_spikes = current_spikes
```

---

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
