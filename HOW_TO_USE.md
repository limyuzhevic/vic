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

#### Creating a Brain with Enhanced Configuration

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Configure using new convenience methods
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.12)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.structural.enable", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Use new convenience methods
print(f"Excitatory neurons: {brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)}")
print(f"Inhibitory neurons: {brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)}")

# Get comprehensive brain statistics
brain_stats = brain.getBrainStats()
print(f"Total spikes: {brain_stats['total_spikes']}")
print(f"Firing rate: {brain_stats['firing_rate']:.4f}")
print(f"Development stage: {brain_stats['developmental_stage']}")
```

#### Running a Simulation

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createDefaultConfig()

# Configure for interesting simulation
config.set("brain.neuron_count", 2000)
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        # Use new convenience methods for enhanced statistics
        neuron_stats = brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)
        print(f"  Excitatory neurons: {neuron_stats}")
        
        if step % 500 == 0:
            brain_stats = brain.getBrainStats()
            print(f"  Total spikes: {brain_stats['total_spikes']}")
            print(f"  Firing rate: {brain_stats['firing_rate']:.4f}")
```

#### Creating a Simple World with Enhanced Configuration

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

# Configure world settings using convenience methods
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.1)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)

# Get world statistics using convenience methods
world_info = world.getWorldInfo()
print(f"World setup complete: {world_info['width']}x{world_info['height']}")
print(f"Max energy: {world_info['max_energy']}")
print(f"Energy decay rate: {world_info['energy_decay_rate']}")
```

### Complete Agent Example (Enhanced)

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration with enhanced features
    config = pynlm.createDefaultConfig()
    
    # Configure using new convenience methods
    config.set("brain.neuron_count", 1500)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.structural.enable", True)
    
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
    
    # 6. Enable subsystems (using new convenience methods)
    agent.enableAllSystems()
    
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
        
        # Print progress with enhanced statistics
        if step % 100 == 0:
            print(f"Step {step}:")
            
            # Use new convenience methods for statistics
            brain_stats = brain.getBrainStats()
            agent_stats = agent.getAgentStats()
            
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain_stats['total_spikes']}")
            print(f"  Curiosity: {agent_stats['curiosity']:.3f}")
            print(f"  Novelty: {agent_stats['novelty']:.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            
            # Get system status using new convenience method
            system_status = agent.getSystemStatus()
            print(f"  Systems enabled: {sum(system_status.values())}/{len(system_status)}")

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

### New Convenience Methods

The NLM Python API now includes several convenience methods that simplify common operations:

#### Config Convenience Methods

| Method | Description | Example |
|--------|-------------|---------|
| `getInt(key, default)` | Safely get integer configuration value | `config.getInt("brain.neuron_count", 1000)` |
| `getFloat(key, default)` | Safely get float configuration value | `config.getFloat("brain.synapse_density", 0.1)` |
| `getBool(key, default)` | Safely get boolean configuration value | `config.getBool("neuromod.curiosity.enable", True)` |
| `getString(key, default)` | Safely get string configuration value | `config.getString("config.name", "default")` |

Example usage:
```python
# Set configuration
config.set("brain.neuron_count", 1500)

# Safely get values with error handling
neuron_count = config.getInt("brain.neuron_count", 1000)
density = config.getFloat("brain.synapse_density", 0.1)
autonomy = config.getBool("neuromod.curiosity.enable", False)
```

#### Brain Convenience Methods

| Method | Description | Example |
|--------|-------------|---------|
| `getNeuronCountByType(type)` | Count neurons by specific type | `brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)` |
| `getBrainStats()` | Get comprehensive brain statistics | `stats = brain.getBrainStats()` |

Example usage:
```python
# Get neuron counts by type
excitatory = brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)
inhibitory = brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)
sensory = brain.getNeuronCountByType(pynlm.NeuronType.Sensory)

# Get comprehensive brain statistics
brain_stats = brain.getBrainStats()
print(f"Total spikes: {brain_stats['total_spikes']}")
print(f"Firing rate: {brain_stats['firing_rate']:.4f}")
```

#### AgentBrain Convenience Methods

| Method | Description | Example |
|--------|-------------|---------|
| `getAgentStats()` | Get comprehensive agent statistics | `stats = agent.getAgentStats()` |
| `getActionRecommendation(context)` | Get action recommendation based on brain state | `rec = agent.getActionRecommendation()` |
| `enableAllSystems()` | Enable all agent systems at once | `agent.enableAllSystems()` |
| `disableAllSystems()` | Disable all agent systems at once | `agent.disableAllSystems()` |
| `getSystemStatus()` | Get current status of all agent systems | `status = agent.getSystemStatus()` |

Example usage:
```python
# Enable all systems at once
agent.enableAllSystems()

# Get comprehensive agent statistics
agent_stats = agent.getAgentStats()
print(f"Curiosity level: {agent_stats['curiosity']:.3f}")
print(f"Novelty level: {agent_stats['novelty']:.3f}")

# Get action recommendation
recommendation = agent.getActionRecommendation()
print(f"Recommended action: {recommendation['action']}")

# Get system status
system_status = agent.getSystemStatus()
print(f"Systems enabled: {sum(system_status.values())}/{len(system_status)}")
```

### Error Handling Examples

#### Safe Configuration Access

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("neuromod.dopamine.scale", 1.5)

# Safe access with error handling
def safe_config_get(config, key, default_value=None):
    try:
        if default_value is None:
            return config.get(key)
        elif isinstance(default_value, int):
            return config.getInt(key, default_value)
        elif isinstance(default_value, float):
            return config.getFloat(key, default_value)
        elif isinstance(default_value, bool):
            return config.getBool(key, default_value)
        else:
            return config.getString(key, default_value)
    except Exception as e:
        print(f"Error getting {key}: {e}")
        return default_value

# Usage
neuron_count = safe_config_get(config, "brain.neuron_count", 1000)
dopamine_scale = safe_config_get(config, "neuromod.dopamine.scale", 1.0)
unknown_param = safe_config_get(config, "unknown.key", "default_value")
```

#### Safe Brain Statistics Access

```python
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()

# Safe brain statistics access
def safe_brain_stats(brain):
    try:
        stats = brain.getBrainStats()
        
        # Access statistics with defaults
        total_neurons = stats.get("total_neurons", 0)
        total_spikes = stats.get("total_spikes", 0)
        firing_rate = stats.get("firing_rate", 0.0)
        e_to_i_ratio = stats.get("e_to_i_ratio", 1.0)
        
        return {
            "total_neurons": total_neurons,
            "total_spikes": total_spikes,
            "firing_rate": firing_rate,
            "e_to_i_ratio": e_to_i_ratio
        }
    except Exception as e:
        print(f"Error getting brain stats: {e}")
        return {"total_neurons": 0, "total_spikes": 0, "firing_rate": 0.0, "e_to_i_ratio": 1.0}

# Usage
stats = safe_brain_stats(brain)
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
```

#### Safe Agent Statistics Access

```python
import pynlm

agent = pynlm.createAgentBrain(brain)

# Safe agent statistics access
def safe_agent_stats(agent):
    try:
        stats = agent.getAgentStats()
        
        # Extract key metrics
        curiosity = stats.get("curiosity", 0.0)
        novelty = stats.get("novelty", 0.0)
        prediction_error = stats.get("prediction_error", 0.0)
        neuromodulation = stats.get("neuromodulation", 0.0)
        
        return {
            "curiosity": curiosity,
            "novelty": novelty,
            "prediction_error": prediction_error,
            "neuromodulation": neuromodulation
        }
    except Exception as e:
        print(f"Error getting agent stats: {e}")
        return {"curiosity": 0.0, "novelty": 0.0, "prediction_error": 0.0, "neuromodulation": 0.0}

# Usage
agent_stats = safe_agent_stats(agent)
print(f"Curiosity level: {agent_stats['curiosity']:.3f}")
print(f"Novelty level: {agent_stats['novelty']:.3f}")
```

### Enhanced Example Scripts

See the `examples/` directory for working example scripts that demonstrate the new convenience methods and features:

1. `example1_basic_convenience.py` - Basic usage with new convenience methods
2. `example2_safe_config.py` - Configuration management with error handling
3. `example3_brain_stats.py` - Brain statistics and analysis
4. `example4_agent_features.py` - Agent behavior with enhanced features
5. `example5_complete_simulations.py` - Complete simulation workflows

### Key Classes and Methods (Updated)

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

# New convenience methods
brain.getNeuronCountByType(type)  # Get neuron count by type
brain.getBrainStats()             # Get comprehensive brain statistics
brain.to_dict()                   # Convert to Python dictionary

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

# New convenience methods
config.getInt(key, default)       # Get integer safely
config.getFloat(key, default)     # Get float safely
config.getBool(key, default)      # Get boolean safely
config.getString(key, default)    # Get string safely
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

# New convenience methods
agent.getAgentStats()              # Get comprehensive agent statistics
agent.getActionRecommendation(context)  # Get action recommendation
agent.enableAllSystems()           # Enable all systems
agent.disableAllSystems()          # Disable all systems
agent.getSystemStatus()            # Get system status
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

# New convenience methods
world.getPerceptAsDict()           # Get percept as dictionary
world.getBodyAsDict()              # Get body as dictionary
world.getWorldInfo()               # Get world information
world.setPosition(x, y)            # Set position conveniently
world.setEnergy(energy)            # Set energy conveniently
world.addResource(x, y, value)     # Add resource conveniently
world.addHazard(x, y, value)       # Add hazard conveniently
world.isAgentAlive()               # Check if agent is alive
world.getTimeToLive()              # Get time to live
world.getWorldStats()              # Get comprehensive world stats
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

### Enhanced Example Scripts

See the `examples/` directory for comprehensive working examples that demonstrate the new convenience methods and features:

1. `example1_basic_convenience.py` - Basic usage with new convenience methods
2. `example2_safe_config.py` - Configuration management with error handling
3. `example3_brain_stats.py` - Brain statistics and analysis
4. `example4_agent_features.py` - Agent behavior with enhanced features
5. `example5_complete_simulations.py` - Complete simulation workflows

These examples showcase:
- New Config convenience methods (getInt, getFloat, getBool, getString)
- New Brain convenience methods (getNeuronCountByType, getBrainStats)
- New AgentBrain convenience methods (getAgentStats, enableAllSystems)
- Comprehensive error handling
- Working code examples that can be copied and used
- Advanced simulation scenarios

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
