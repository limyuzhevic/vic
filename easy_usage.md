# NLM Python API Documentation

This document provides comprehensive documentation for the NLM Python API, covering all available classes, functions, and usage examples.

## Overview

The NLM (Neural Learning Machine) Python bindings provide access to the full C++ neural simulation framework through a Python interface. The API includes classes for creating and managing brains, configuring simulation parameters, processing sensory input, decoding motor commands, and interacting with virtual environments.

## Installation

```bash
# Install the NLM Python package
pip install pynlm

# For development (install in editable mode)
pip install -e .

# Install build dependencies (if building from source)
pip install pybind11 scikit-build-core pytest numpy
```

## Quick Start

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
```

### Complete Agent Simulation

```python
import pynlm

def run_complete_simulation(steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
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
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(steps):
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
        
        # Apply reward modulation based on world state
        reward = 0.0
        if world.getAgentBody().energy > 10.0:
            reward = 0.5
        elif world.getAgentBody().health < 50.0:
            reward = -0.3
            
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
    
    print("Simulation complete!")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation(1000)
```

## Core Classes

### Config Class

The Config class manages configuration parameters for the NLM system.

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    print("Neuron count configured")

# Get all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Modify configuration
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.enable", True)
config.set("brain.initial_weight_mean", 0.5)

# File I/O
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")

# Summary
print(config.summary())
```

**Config Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `get(key, default)` - Get configuration value with default
- `set(key, value)` - Set configuration value
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary

### Brain Class

The Brain class represents the central neural simulation engine.

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation control
for step in range(100):
    brain.step(step)  # Run one simulation step
    # or
    brain.step(step, step * 0.001)  # Run with timestamp

# Brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")

# Region management
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Development
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)

# Sensory input
from pynlm import Vision
sensory_input = Vision()
sensory_input.setData([0.0] * 256)
brain.receiveSensoryInput(sensory_input)

# Action production
action = brain.produceAction()
print(f"Action: {action.getType()}")

# Reset
brain.reset()
```

**Brain Methods:**
- `initialize()` - Initialize the brain
- `step(step_number)` - Run one simulation step
- `step(step_number, timestamp)` - Run step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron_id, current)` - Inject current into neuron
- `injectCurrentToNeurons(type, current)` - Inject to all neurons of type
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region by ID
- `getRegionCount()` - Get number of regions
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get active neuron count
- `getFiringNeuronCount()` - Get firing neuron count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I ratio
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `getConfig()` - Get configuration
- `logStatus()` - Log brain status

### AgentBrain Class

The AgentBrain class connects the NLM brain to the external world.

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Initialize with world
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Process sensory input
from pynlm import SensoryPercept
percept = SensoryPercept()
# ... configure percept ...
agent.processSensoryInput(percept)

# Decode motor command
action = agent.decodeMotorCommand()

# Apply reward modulation
agent.applyRewardModulation(reward=0.5, predicted_reward=0.3)

# Update development
agent.updateDevelopment(timestep=0.1)

# Get developmental stage
stage = agent.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Get statistics
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")

# Check if subsystems are enabled
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Development enabled: {agent.isDevelopmentEnabled()}")
```

**AgentBrain Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent
- `getBrain()` - Get underlying brain
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity

### SimpleWorld Class

The SimpleWorld class represents a virtual environment for agent interaction.

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Update world
world.update(timestep=0.1)

# Apply motor command
from pynlm import MotorCommand
action = MotorCommand(pynlm.ActionType.MoveForward)
world.applyMotorCommand(action, world.getSimulationTime())

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent body
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent velocity: ({body.velocityX}, {body.velocityY})")

# Manage world objects
from pynlm import WorldObject, WorldObjectType
resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0)
world.addObject(resource)

# Remove objects
world.removeObject(5.0, 5.0)

# Check valid positions
if world.isValidPosition(10.0, 10.0):
    print("Position is valid")

# Get world properties
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"Simulation time: {world.getSimulationTime()}")

# Configure energy
world.setMaxEnergy(100.0)
```

**SimpleWorld Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid
- `getWidth()` - Get world width
- `getHeight()` - Get world height
- `getMaxEnergy()` - Get maximum energy
- `setMaxEnergy(e)` - Set maximum energy
- `getEnergyDecayRate()` - Get energy decay rate
- `setEnergyDecayRate(r)` - Set energy decay rate
- `getSimulationTime()` - Get simulation time
- `setRandomSeed(seed)` - Set random seed

## Enumerations

### NeuronType
- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

### SynapseType
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction

### DevelopmentalStage
- `Initial` - Initial development stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

### MotorCommand
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource
- `Hazard` - Hazard
- `Wall` - Wall
- `Marker` - Marker

## Configuration Options

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

## Advanced Usage Examples

### Batch Processing

```python
import pynlm
import time

def batch_simulate(configs, steps_per_config):
    """Run multiple simulations with different configurations."""
    results = []
    
    for i, config_template in enumerate(configs):
        print(f"Running simulation {i+1}/{len(configs)}")
        
        # Create config from template
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_template.get("neurons", 1000))
        config.set("brain.initial_weight_mean", config_template.get("weight_mean", 0.5))
        
        # Run simulation
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps_per_config):
            brain.step(step)
        
        end_time = time.time()
        
        # Store results
        results.append({
            'config': config,
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'firing': brain.getFiringNeuronCount(),
            'spikes': brain.getTotalSpikeCount(),
            'duration': end_time - start_time,
            'steps_per_second': steps_per_config / (end_time - start_time)
        })
    
    return results

# Example usage
configs = [
    {'neurons': 500, 'weight_mean': 0.3},
    {'neurons': 1000, 'weight_mean': 0.5},
    {'neurons': 2000, 'weight_mean': 0.7},
]

results = batch_simulate(configs, 500)
for i, result in enumerate(results):
    print(f"Simulation {i+1}: {result['neurons']} neurons, "
          f"{result['spikes']} spikes, {result['steps_per_second']:.1f} steps/sec")
```

### Learning Experiment

```python
import pynlm

def run_learning_experiment(steps=2000):
    """Run a learning experiment tracking synaptic changes over time."""
    
    # Initialize system
    config = pynlm.createDefaultConfig()
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Track learning metrics
    learning_data = []
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on world state
        reward = 0.0
        if world.getAgentBody().energy > 15.0:
            reward = 0.8
        elif world.getAgentBody().energy < 5.0:
            reward = -0.5
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record metrics
        if step % 100 == 0:
            learning_data.append({
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'reward': reward,
                'dev_stage': agent.getDevelopmentalStage(),
                'energy': world.getAgentBody().energy,
                'health': world.getAgentBody().health
            })
    
    return learning_data

# Run learning experiment
learning_data = run_learning_experiment(2000)

# Analyze results
import matplotlib.pyplot as plt

steps = [data['step'] for data in learning_data]
firing_rates = [data['firing_neurons'] / 500 for data in learning_data]  # Normalize
curiosity_levels = [data['curiosity'] for data in learning_data]

plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(steps, firing_rates)
plt.title('Normalized Firing Rate')
plt.xlabel('Step')
plt.ylabel('Firing Rate')

plt.subplot(2, 2, 2)
plt.plot(steps, curiosity_levels)
plt.title('Curiosity Level')
plt.xlabel('Step')
plt.ylabel('Curiosity')

plt.subplot(2, 2, 3)
total_spikes = [data['total_spikes'] for data in learning_data]
plt.plot(steps, total_spikes)
plt.title('Cumulative Spikes')
plt.xlabel('Step')
plt.ylabel('Total Spikes')

plt.subplot(2, 2, 4)
energy = [data['energy'] for data in learning_data]
plt.plot(steps, energy)
plt.title('Agent Energy')
plt.xlabel('Step')
plt.ylabel('Energy')

plt.tight_layout()
plt.show()

print("Learning experiment complete!")
print(f"Final developmental stage: {learning_data[-1]['dev_stage']}")
print(f"Final curiosity level: {learning_data[-1]['curiosity']:.3f}")
print(f"Total spikes generated: {learning_data[-1]['total_spikes']}")
```

### Real-time Visualization

```python
import pynlm
import time

def real_time_visualization(duration=60):
    """Run real-time visualization of brain activity."""
    
    # Initialize system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    agent.enableCuriosity(True)
    
    print("Starting real-time visualization...")
    print("Press Ctrl+C to stop")
    
    start_time = time.time()
    frame_count = 0
    
    try:
        while time.time() - start_time < duration:
            frame_start = time.time()
            
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain
            brain.step(frame_count)
            
            # Decode action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
            
            # Print status every 10 frames
            if frame_count % 10 == 0:
                print(f"Frame {frame_count:4d} | "
                      f"Firing: {brain.getFiringNeuronCount():4d} | "
                      f"Spikes: {brain.getTotalSpikeCount():6d} | "
                      f"Curiosity: {agent.getCuriosityLevel():.3f} | "
                      f"Energy: {world.getAgentBody().energy:5.1f}")
            
            frame_count += 1
            
            # Control frame rate
            frame_time = time.time() - frame_start
            if frame_time < 0.1:  # Target 10 FPS
                time.sleep(0.1 - frame_time)
    
    except KeyboardInterrupt:
        print("\nVisualization stopped by user")
    
    print(f"\nVisualization complete!")
    print(f"Total frames: {frame_count}")
    print(f"Duration: {duration} seconds")
    print(f"Average FPS: {frame_count / duration:.1f}")

# Run real-time visualization
real_time_visualization(30)
```

### Configuration File Generator

```python
import pynlm
import json
import os

def generate_config_variations(base_config_path="config.json", output_dir="config_variations"):
    """Generate multiple configuration files for parameter exploration."""
    
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    # Load base configuration
    config = pynlm.createDefaultConfig()
    if os.path.exists(base_config_path):
        config.loadFromFile(base_config_path)
    
    # Get base configuration values
    base_neurons = config.getOr("brain.neuron_count", 1000)
    base_weights = config.getOr("brain.initial_weight_mean", 0.5)
    
    # Generate parameter variations
    variations = [
        {"neurons": 500, "weight_mean": 0.3, "stdp_enable": True},
        {"neurons": 1000, "weight_mean": 0.5, "stdp_enable": True},
        {"neurons": 1500, "weight_mean": 0.7, "stdp_enable": True},
        {"neurons": 2000, "weight_mean": 0.9, "stdp_enable": True},
        {"neurons": 1000, "weight_mean": 0.5, "stdp_enable": False},
        {"neurons": 1000, "weight_mean": 0.5, "hebbian_enable": True},
        {"neurons": 1000, "weight_mean": 0.5, "structural_enable": True},
    ]
    
    # Save each variation
    for i, variation in enumerate(variations):
        # Create config for this variation
        variation_config = pynlm.createDefaultConfig()
        variation_config.set("brain.neuron_count", variation["neurons"])
        variation_config.set("brain.initial_weight_mean", variation["weight_mean"])
        variation_config.set("plasticity.stdp.enable", variation.get("stdp_enable", True))
        variation_config.set("plasticity.hebbian.enable", variation.get("hebbian_enable", False))
        variation_config.set("plasticity.structural.enable", variation.get("structural_enable", False))
        
        # Generate filename based on parameters
        filename = f"{output_dir}/config_{variation['neurons']}_neurons_"
        filename += f"w{variation['weight_mean']:.1f}_stdp{'1' if variation.get('stdp_enable', True) else '0'}_"
        filename += f"hebb{'1' if variation.get('hebbian_enable', False) else '0'}_"
        filename += f"struct{'1' if variation.get('structural_enable', False) else '0'}.json"
        
        # Save configuration
        variation_config.saveToFile(filename)
        
        # Create corresponding simulation script
        script_content = f'''import pynlm

# Load configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("{filename}")

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {{step}}: {{brain.getFiringNeuronCount()}} neurons firing")

print("Simulation complete!")
'''
        
        # Save simulation script
        script_filename = f"{output_dir}/run_config_{i+1}.py"
        with open(script_filename, 'w') as f:
            f.write(script_content)
        
        print(f"Generated: {filename} and {script_filename}")
    
    print(f"\nGenerated {len(variations)} configuration variations in '{output_dir}'")

# Generate configuration variations
generate_config_variations()
```

## Performance Optimization

### Batch Processing

```python
import pynlm
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor
import time

def simulate_single(params):
    """Run a single simulation with given parameters."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", params['neurons'])
    config.set("plasticity.stdp.enable", params['stdp'])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(params['steps']):
        brain.step(step)
    
    return {
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'spikes': brain.getTotalSpikeCount(),
        'firing': brain.getFiringNeuronCount()
    }

def parallel_simulation(params_list, use_processes=True):
    """Run multiple simulations in parallel."""
    
    if use_processes:
        # Use multiprocessing
        with ProcessPoolExecutor() as executor:
            results = list(executor.map(simulate_single, params_list))
    else:
        # Use threading
        with ThreadPoolExecutor() as executor:
            results = list(executor.map(simulate_single, params_list))
    
    return results

# Example usage
params_list = [
    {'neurons': 500, 'steps': 500, 'stdp': True},
    {'neurons': 1000, 'steps': 500, 'stdp': True},
    {'neurons': 1500, 'steps': 500, 'stdp': True},
    {'neurons': 2000, 'steps': 500, 'stdp': True},
]

start_time = time.time()
results = parallel_simulation(params_list, use_processes=True)
end_time = time.time()

print(f"Completed {len(results)} simulations in {end_time - start_time:.1f} seconds")
for i, result in enumerate(results):
    print(f"Simulation {i+1}: {result['neurons']} neurons, "
          f"{result['spikes']} spikes")
```

### Memory-Efficient Simulation

```python
import pynlm
import gc

def memory_efficient_simulation(steps=1000, chunks=10):
    """Run simulation in chunks to reduce memory usage."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    chunk_size = steps // chunks
    
    for chunk in range(chunks):
        print(f"Processing chunk {chunk + 1}/{chunks}")
        
        for step in range(chunk * chunk_size, (chunk + 1) * chunk_size):
            brain.step(step)
            
            # Print progress
            if step % 100 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        # Force garbage collection to free memory
        gc.collect()
    
    return brain

# Run memory-efficient simulation
brain = memory_efficient_simulation(steps=1000, chunks=10)
print("Memory-efficient simulation complete!")
```

## Troubleshooting

### Common Issues

**Issue: "My brain isn't doing anything"**
```python
# Solution: Check that you call initialize()
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # This is crucial!

# Then run simulation
for step in range(100):
    brain.step(step)
```

**Issue: "The agent isn't moving"**
```python
# Solution: Ensure world.update() is called
world.update(timestep=0.1)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

**Issue: "Everything is 0"**
```python
# Solution: Give the brain time to "warm up"
for step in range(1000):
    brain.step(step)  # More steps needed for activity
```

**Issue: "ImportError: No module named 'pynlm'"**
```bash
# Solution: Install the package
pip install --force-reinstall .
```

**Issue: "Segmentation faults"**
```python
# Solution: Call reset() between runs
brain1 = pynlm.createBrain(config)
brain1.initialize()
brain1.step(100)
brain1.reset()  # Important!

brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.step(100)  # This should not crash
```

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

## Further Reading

- [HOW_TO_USE.md](HOW_TO_USE.md) - Basic usage guide
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization
- [docs/DEBUGGING.md](docs/DEBUGGING.md) - Debugging techniques

## Acknowledgements

This documentation is based on the existing NLM Phase 2 implementation and expanded with comprehensive Python API coverage.

## Version

Python API Version: 0.1.0
C++ Core Version: 0.1.0
Documentation Date: 2026-01-01
```
