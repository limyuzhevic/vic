# NLM Python API Documentation

**Complete reference for the Python bindings of the NLM neural simulation framework**

## Overview

This document provides comprehensive API reference for the Python bindings of NLM (Neural Learning Machine). The Python API allows you to create, configure, and simulate artificial brain systems using Python interfaces to the underlying C++ neural computation engine.

## Core Architecture

### Python Binding System

NLM's Python API is built on pybind11, providing seamless integration between Python and the C++ neural simulation engine. The bindings expose the core neural computation capabilities while maintaining Python's ease of use and flexibility.

```python
# Core architecture concept
import pynlm

# Create brain system
brain = pynlm.createBrain(config)
brain.initialize()

# Neural computation
for step in range(num_steps):
    brain.step(step)
    # Neural dynamics happen here (C++ engine)
```

## Core Classes

### Brain Class

The `Brain` class represents the central neural processing unit. It manages neurons, synapses, plasticity, and cognitive systems.

#### Creation and Initialization

```python
import pynlm

# Method 1: Using default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Method 2: Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
brain = pynlm.createBrain(config)
brain.initialize()

# Method 3: Advanced configuration
config = pynlm.createAdvancedConfig()
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("development.system.enable", True)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Core Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `brain.initialize()` | Initialize brain state and systems | None | None |
| `brain.step(step_number)` | Process one simulation step | `step_number: int` | None |
| `brain.step(step_number, time)` | Process step with explicit timestamp | `step_number: int, time: float` | None |
| `brain.reset()` | Reset brain to initial state | None | None |

#### State Management

```python
# Save and load brain states
brain.save("checkpoint_001.bin")
brain.load("checkpoint_002.bin")

# Get brain statistics
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
firing = brain.getFiringNeuronCount()
rate = brain.getAverageFiringRate()
```

#### Region Management

```python
# Create and manage brain regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Get region properties
region.neuron_count = 500
region.excitation_inhibition_ratio = 0.8
```

### Configuration Class

The `Config` class manages NLM system configuration parameters. It provides access to and modification of all brain, plasticity, neuromodulation, and development settings.

#### Configuration Management

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Query configuration values
if config.has("brain.neuron_count"):
    count = config.get("brain.neuron_count")

# Get all configuration keys
keys = config.getKeys()

# Modify configuration values
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.learning_rate", 0.002)
```

#### Configuration Types

##### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity probability |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

##### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for potentiation (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for depression (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity (synaptogenesis/pruning) |

##### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | float | true | Enable novelty detection |
| `neuromod.prediction_error.scale` | float | 1.0 | Prediction error signal scale |

##### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.system.enable` | bool | true | Enable developmental processes |
| `development.plasticity_rate` | float | 1.0 | Rate of plasticity during development |
| `development.stage_duration` | float | 100.0 | Duration of each developmental stage (ms) |
| `development.critical_period` | float | 50.0 | Length of critical period (ms) |

#### Configuration File I/O

```python
import pynlm

# Save configuration to file
config.saveToFile("brain_config.json")
config.saveToFile("brain_config.xml")

# Load configuration from file
config.loadFromFile("brain_config.json")
config.loadFromFile("brain_config.xml")
```

### AgentBrain Class

The `AgentBrain` class bridges the NLM brain system with environmental interaction. It handles sensory processing, motor decoding, and neuromodulation.

#### Agent Brain Creation

```python
import pynlm

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world interface
agent.initialize(world)
```

#### Sensory Processing

```python
# Process sensory input from world
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get sensory information
vision = agent.getVisualInput()
touch = agent.getTactileInput()
internal = agent.getInternalState()
```

#### Motor Decoding

```python
# Decode motor commands from brain activity
motor_cmd = agent.decodeMotorCommand()

# Get motor command details
action_type = motor_cmd.getActionType()
magnitude = motor_cmd.getMagnitude()
parameters = motor_cmd.getParameters()
```

#### Neuromodulation

```python
# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Get neuromodulation levels
dopamine_level = agent.getCuriosityLevel()
novelty_level = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
```

#### Development Management

```python
# Update developmental stage
agent.updateDevelopment(0.1)

# Get current developmental stage
stage = agent.getDevelopmentalStage()
stage_name = agent.getDevelopmentalStageName()

# Check developmental milestones
is_critical_period = agent.isCriticalPeriod()
```

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
reward_modulation_enabled = agent.isRewardModulationEnabled()
structural_plasticity_enabled = agent.isStructuralPlasticityEnabled()
development_enabled = agent.isDevelopmentEnabled()
curiosity_enabled = agent.isCuriosityEnabled()
```

### SimpleWorld Class

The `SimpleWorld` class represents the simulated environment in which the NLM agent operates. It provides sensory input, motor output, and environmental dynamics.

#### World Creation

```python
import pynlm

# Create simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8,
    maxEnergy=100.0
)
```

#### World Configuration

```python
# Set world properties
world.setRandomSeed(42)  # For reproducible experiments
world.setAgentStart(10.0, 10.0)  # Agent starting position
world.setMaxEnergy(100.0)  # Maximum agent energy
world.setGravity(0.0)  # Gravity force
world.setFriction(0.1)  # Movement friction
```

#### World Control

```python
# World state management
world.reset()  # Reset to initial state
world.update(timestep)  # Update world by timestep
world.step()  # Update with default timestep

# Agent interaction
world.applyMotorCommand(motor_cmd, current_time)
percept = world.getSensoryPercept()
world.addObject(object)  # Add objects to world
world.removeObject(x, y)  # Remove objects
```

#### Sensory Access

```python
# Get sensory information
percept = world.getSensoryPercept()

# Extract sensory modalities
vision = percept.getVision()
touch = percept.getTouch()
proprioception = percept.getProprioception()
internal = percept.getInternal()

# Get agent's position and state
position = world.getAgentPosition()
orientation = world.getAgentOrientation()
energy = world.getAgentEnergy()
```

#### World Objects

```python
# Create world objects
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
hazard = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=-0.5)
marker = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Marker, value=0.0)

# Add and interact with objects
world.addObject(resource)
distance = world.getDistanceToObject(3.0, 3.0)
value = world.getObjectValueAt(5.0, 5.0)
```

#### World Properties

```python
# Query world properties
width = world.getWidth()
height = world.getHeight()
simulation_time = world.getSimulationTime()
timestep = world.getTimestep()
```

### Enumerations

#### Neuron Types

```python
# Neuron type classifications
pynlm.NeuronType.Excitatory    # Excitatory neurons
pynlm.NeuronType.Inhibitory    # Inhibitory neurons
pynlm.NeuronType.Sensory       # Sensory neurons
pynlm.NeuronType.Motor         # Motor neurons
pynlm.NeuronType.Modulatory    # Neuromodulatory neurons
pynlm.NeuronType.Internal      # Internal state neurons
```

#### Developmental Stages

```python
# Developmental stage classifications
pynlm.DevelopmentalStage.Initial        # Initial developmental state
pynlm.DevelopmentalStage.CriticalPeriod # Critical period for learning
pynlm.DevelopmentalStage.Maturation     # Maturity development
pynlm.DevelopmentalStage.Adult          # Adult development
pynlm.DevelopmentalStage.Aging          # Aging development
```

#### Action Types

```python
# Motor action types
pynlm.ActionType.MoveForward      # Move forward
pynlm.ActionType.TurnLeft          # Turn left
pynlm.ActionType.TurnRight         # Turn right
pynlm.ActionType.MoveBackward     # Move backward
pynlm.ActionType.Interact         # Interact with objects
pynlm.ActionType.Eat              # Consume resources
pynlm.ActionType.Sleep            # Rest and recover
```

#### World Object Types

```python
# World object classifications
pynlm.WorldObjectType.Empty       # Empty space
pynlm.WorldObjectType.Resource    # Consumable resources
pynlm.WorldObjectType.Hazard      # Harmful objects
pynlm.WorldObjectType.Wall        # Obstacles
pynlm.WorldObjectType.Marker      # Location markers
pynlm.WorldObjectType.Predator    # Threat objects
pynlm.WorldObjectType.Prey        # Prey objects
```

## Configuration Functions

### Factory Functions

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Create simple configuration for beginners
config = pynlm.createSimpleConfig()

# Create experimental configuration
config = pynlm.createExperimentConfig()
```

## Example Usage

### Basic Brain Simulation

```python
import pynlm
import time

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    # Log statistics
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

### Complete Agent Simulation

```python
import pynlm
import time

def run_complete_simulation(num_steps=1000):
    """Run a complete agent simulation with all subsystems enabled."""
    
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
    
    # 6. Enable all subsystems
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
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation(1000)
```

### Advanced Configuration Example

```python
import pynlm
import json

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Customize specific parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.system.enable", True)
config.set("development.plasticity_rate", 0.8)
config.set("development.critical_period", 75.0)

# Save configuration to file
config.saveToFile("advanced_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("advanced_config.json")

# Create brain with advanced configuration
brain = pynlm.createBrain(loaded_config)
brain.initialize()
```

### World Simulation Example

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=10,
    visionHeight=10,
    maxEnergy=200.0
)
world.reset()
world.setAgentStart(15.0, 15.0)
world.setRandomSeed(12345)  # For reproducible experiments

# Add interesting world objects
world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=2.0))
world.addObject(pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Resource, value=2.0))
world.addObject(pynlm.WorldObject(15.0, 5.0, pynlm.WorldObjectType.Hazard, value=-1.0))
world.addObject(pynlm.WorldObject(5.0, 15.0, pynlm.WorldObjectType.Wall, value=0.0))

# Run agent simulation
def run_world_simulation(brain, agent, world, num_steps=500):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get statistics
        if step % 50 == 0:
            position = world.getAgentPosition()
            energy = world.getAgentEnergy()
            print(f"Step {step}: Position ({position.x:.1f}, {position.y:.1f}), Energy: {energy:.1f}")
    
    return brain, agent, world
```

## Advanced Topics

### Multi-threaded Simulation

```python
import pynlm
import threading
import queue

def threaded_simulation(brain, num_steps, result_queue):
    """Run simulation in separate thread for performance."""
    for step in range(num_steps):
        brain.step(step)
    
    # Put final results in queue
    result_queue.put({
        'total_neurons': brain.getTotalNeuronCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate()
    })

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run threaded simulation
result_queue = queue.Queue()
thread = threading.Thread(target=threaded_simulation, args=(brain, 1000, result_queue))
thread.start()
thread.join()

# Get results
results = result_queue.get()
print(f"Threaded simulation results: {results}")
```

### Checkpoint and Recovery

```python
import pynlm
import os

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation loop with checkpointing
checkpoint_interval = 100
simulation_steps = 1000
checkpoint_dir = "brain_checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)

for step in range(simulation_steps):
    brain.step(step)
    
    # Save checkpoint
    if step % checkpoint_interval == 0 and step > 0:
        checkpoint_path = os.path.join(checkpoint_dir, f"checkpoint_{step}.bin")
        brain.save(checkpoint_path)
        print(f"Checkpoint saved: {checkpoint_path}")

# Load checkpoint and resume
checkpoint_path = os.path.join(checkpoint_dir, "checkpoint_500.bin")
if os.path.exists(checkpoint_path):
    brain.load(checkpoint_path)
    print(f"Checkpoint loaded: {checkpoint_path}")
    
    # Continue simulation
    for step in range(501, 601):
        brain.step(step)
```

### Performance Benchmarking

```python
import pynlm
import time
import statistics

def benchmark_brain_performance(config, num_steps=1000, num_runs=5):
    """Benchmark brain performance across multiple runs."""
    
    run_times = []
    firing_rates = []
    spike_counts = []
    
    for run in range(num_runs):
        # Create fresh brain for each run
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        for step in range(num_steps):
            brain.step(step)
        end_time = time.time()
        
        run_time = end_time - start_time
        run_times.append(run_time)
        firing_rates.append(brain.getAverageFiringRate())
        spike_counts.append(brain.getTotalSpikeCount())
        
        print(f"Run {run + 1}: {run_time:.3f}s, "
              f"Firing rate: {brain.getAverageFiringRate():.2f}, "
              f"Spikes: {brain.getTotalSpikeCount()}")
    
    # Calculate statistics
    avg_time = statistics.mean(run_times)
    std_time = statistics.stdev(run_times) if len(run_times) > 1 else 0.0
    avg_firing_rate = statistics.mean(firing_rates)
    avg_spike_count = statistics.mean(spike_counts)
    
    print(f"\nBenchmark Results ({num_runs} runs):")
    print(f"Average time: {avg_time:.3f}s ± {std_time:.3f}s")
    print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"Average spike count: {avg_spike_count:.0f}")
    
    return {
        'avg_time': avg_time,
        'std_time': std_time,
        'avg_firing_rate': avg_firing_rate,
        'avg_spike_count': avg_spike_count,
        'run_times': run_times,
        'firing_rates': firing_rates,
        'spike_counts': spike_counts
    }

# Benchmark with different configurations
configs = {
    'small': pynlm.createSimpleConfig(),
    'medium': pynlm.createDefaultConfig(),
    'large': pynlm.createAdvancedConfig()
}

for name, config in configs.items():
    print(f"\nBenchmarking {name} brain configuration:")
    results = benchmark_brain_performance(config, num_steps=500, num_runs=3)
```

## Error Handling

### Common Error Scenarios

#### Brain Initialization Errors

```python
import pynlm
import traceback

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()  # This could fail if configuration is invalid
except Exception as e:
    print(f"Brain initialization failed: {e}")
    traceback.print_exc()
```

#### Simulation Step Errors

```python
import pynlm

# Simulation with error handling
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    try:
        brain.step(step)
    except RuntimeError as e:
        print(f"Simulation step {step} failed: {e}")
        # Handle error (save state, try different configuration, etc.)
        break
    except Exception as e:
        print(f"Unexpected error at step {step}: {e}")
        # Log error and continue
        continue
```

#### Configuration Errors

```python
import pynlm

# Safe configuration loading
config = pynlm.createDefaultConfig()

try:
    config.loadFromFile("valid_config.json")
except FileNotFoundError:
    print("Configuration file not found, using defaults")
except ValueError as e:
    print(f"Invalid configuration: {e}")
    # Use default configuration
    config = pynlm.createDefaultConfig()
except Exception as e:
    print(f"Unexpected error loading configuration: {e}")
    raise
```

## Troubleshooting

### Common Issues and Solutions

#### "Brain is not responding" (No activity)

**Cause**: Brain may need time to "warm up" or may have insufficient input.

**Solution**:
1. Check that `brain.initialize()` was called before `brain.step()`
2. Increase simulation steps (50-100 steps for warm-up)
3. Ensure sensory input is connected to brain
4. Check that neurons have appropriate connections

#### "Memory issues with large brains"

**Cause**: Large brain configurations may exceed available memory.

**Solution**:
1. Use smaller `brain.neuron_count` (100-1000 for testing)
2. Reduce `brain.synapse_density` for sparser connections
3. Use `pynlm.createSimpleConfig()` for resource-constrained environments
4. Monitor memory usage during simulation

#### "Non-deterministic behavior"

**Cause**: Lack of random seed for reproducible experiments.

**Solution**:
1. Set world random seed: `world.setRandomSeed(42)`
2. Use deterministic configurations
3. Enable reproducibility features in configuration

#### "Performance issues"

**Cause**: Inefficient simulation or overly complex configurations.

**Solution**:
1. Use `pynlm.createSimpleConfig()` for testing
2. Reduce `brain.neuron_count` and `brain.synapse_density`
3. Use smaller simulation steps
4. Consider `createExperimentConfig()` for optimized performance

## Migration Guide

### From v0.1.x to v0.2.0+

This guide covers migration from older NLM Python API versions.

#### Configuration Changes

**v0.1.x**:
```python
# Old way
config = {
    "brain.neuron_count": 1000,
    "plasticity.stdp.enable": True,
    "neuromod.dopamine.scale": 1.0
}
```

**v0.2.0+**:
```python
# New way
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
```

#### API Changes

**v0.1.x**:
```python
# Old brain usage
brain = pynlm.createBrain(config)
brain.initialize()
brain.runSimulation(100)
```

**v0.2.0+**:
```python
# New brain usage
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
```

#### Class Renaming

| Old Class | New Class | Description |
|-----------|-----------|-------------|
| `NLMBrain` | `Brain` | Main brain class |
| `NLMConfig` | `Config` | Configuration class |
| `NLMWorld` | `SimpleWorld` | World simulation class |
| `NLMAgent` | `AgentBrain` | Agent interface class |

## Version Information

### Current API Version
v0.2.0

### Supported Python Versions
- Python 3.8+
- Python 3.9+
- Python 3.10+
- Python 3.11+
- Python 3.12+

### Dependencies
- pybind11 >= 2.11.0 (C++ bindings)
- scikit-build-core >= 0.5.0 (build system)

### Installation
```bash
pip install pynlm
# Or install from source
pip install -e .
```

## References

### Related Documentation
- [NLM C++ API Documentation](docs/api/cpp-api.md)
- [System Architecture](docs/architecture/architecture.md)
- [Configuration System](docs/api/configuration.md)
- [Performance Optimization](docs/best-practices/performance.md)
- [Tutorial: Getting Started](docs/tutorial/getting-started.md)

### Research Papers
- [Biological Fidelity Documentation](docs/SCIENCE.md)
- [Phase 6 Final Results](docs/PHASE6_FINAL_RESULTS.md)
- [Research Methods](docs/RESEARCH_METHODS.md)

## Changelog

### v0.2.0 (Current)
- Major API refactoring for better usability
- Added configuration factory functions
- Enhanced error handling and validation
- Improved documentation and examples

### v0.1.0
- Initial Python API release
- Basic brain simulation capabilities
- Simple world interaction
- Core neuromodulation support

## Contact Information

### Issues and Feedback
- **GitHub Repository**: [nlm-project/nlm](https://github.com/nlm-project/nlm)
- **Documentation Issues**: Submit issues through repository
- **API Questions**: Check API reference documentation
- **Technical Support**: Consult system architecture documentation

### Support Channels
- **Documentation Hub**: Current documentation index
- **Quick Start Guide**: [easy_usage.md](easy_usage.md)
- **Technical Details**: [HOW_TO_USE.md](HOW_TO_USE.md)
- **Community Resources**: [SCIENCE.md](docs/SCIENCE.md)

---

*This documentation is continuously updated. Please report any issues or provide feedback to help improve the Python API documentation.*

**Last Updated**: v0.2.0 - Comprehensive API documentation enhancement**

---

## Python API Quick Reference

### Core Functions

```python
# Configuration creation
pynlm.createDefaultConfig()
pynlm.createAdvancedConfig()
pynlm.createSimpleConfig()
pynlm.createExperimentConfig()

# Brain creation
pynlm.createBrain(config)

# World creation
pynlm.createSimpleWorld()

# Agent creation
pynlm.createAgentBrain(brain)
```

### Neuron Types

```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

### Developmental Stages

```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### Action Types

```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.MoveBackward
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Sleep
```

### World Object Types

```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
pynlm.WorldObjectType.Predator
pynlm.WorldObjectType.Prey
```

### Class Methods Summary

#### Brain Class Methods
- `brain.initialize()` - Initialize brain
- `brain.step(step)` - Process simulation step
- `brain.reset()` - Reset brain state
- `brain.save(path)` - Save brain state
- `brain.load(path)` - Load brain state
- `brain.getTotalNeuronCount()` - Get total neurons
- `brain.getTotalSynapseCount()` - Get total synapses
- `brain.getFiringNeuronCount()` - Get firing neurons
- `brain.getAverageFiringRate()` - Get average firing rate

#### Config Class Methods
- `config.set(key, value)` - Set configuration value
- `config.get(key)` - Get configuration value
- `config.has(key)` - Check if key exists
- `config.getKeys()` - Get all configuration keys
- `config.loadFromFile(path)` - Load from file
- `config.saveToFile(path)` - Save to file

#### AgentBrain Class Methods
- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.applyRewardModulation(reward, predicted)` - Apply reward
- `agent.updateDevelopment(timestep)` - Update development
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level

#### SimpleWorld Class Methods
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.reset()` - Reset world
- `world.update(timestep)` - Update world
- `world.applyMotorCommand(action, time)` - Apply motor command
- `world.getSensoryPercept()` - Get sensory percept
- `world.getAgentPosition()` - Get agent position
- `world.getAgentEnergy()` - Get agent energy
- `world.addObject(object)` - Add object to world
- `world.removeObject(x, y)` - Remove object from world

### Usage Patterns

#### Pattern 1: Basic Simulation
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### Pattern 2: Agent in World
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()
agent.initialize(world)

# Simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Pattern 3: Configuration with File I/O
```python
import pynlm
import json

# Create and customize configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.002)

# Save configuration
config.saveToFile("config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("config.json")

# Use loaded configuration
brain = pynlm.createBrain(new_config)
brain.initialize()
```

This comprehensive Python API documentation provides complete reference for NLM's Python bindings, including core classes, configuration management, error handling, and usage examples. The API is designed for both beginners and advanced users, with extensive documentation and examples to support all use cases.