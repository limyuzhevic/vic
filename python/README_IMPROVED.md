# Improved Python Bindings for NLM

Enhanced Python interface for the Neural Learning Machine (NLM) framework with improved Pythonic APIs, convenience methods, and utilities.

## Overview

The `pynlm_improved` module provides enhanced Python bindings over the original C++ NLM framework. It offers:

- **Pythonic Wrappers**: Enhanced classes that follow Python conventions
- **Convenience Methods**: Shortcuts for common operations
- **Helper Functions**: Utilities for simulation and analysis
- **Better Error Handling**: Python-style exceptions
- **Context Managers**: Automatic resource management
- **Experimental Utilities**: Tools for running experiments

## Installation

```bash
# After building with scikit-build-core
pip install -e .
```

## Basic Usage

### Creating a Simple Simulation

```python
import pynlm_improved as nlm

# Create default configuration
config = nlm.create_default_config()

# Create brain
brain = nlm.PyBrain(config)

# Initialize brain
brain.initialize()

# Create world
world = nlm.PySimpleWorld()
world.configure(width=50.0, height=50.0)
world.reset()

# Create agent
agent = nlm.PyAgentBrain(brain)
agent.initialize(world)

# Set agent starting position
world.set_agent_start(10.0, 10.0)
```

### Running a Simulation

```python
# Run a simple simulation
results = nlm.run_simple_simulation(
    steps=1000,
    timestep=0.1,
    verbose=True
)

print(f"Final reward: {results['final_reward']}")
print(f"Final dopamine: {results['final_dopamine']}")
print(f"Final curiosity: {results['final_curiosity']}")

# Access simulation history
for step in results['history'][:10]:  # First 10 steps
    print(f"Step {step['step']}: {step['action']} "
          f"reward={step['reward']:.3f} "
          f"neurons={step['neurons']}")
```

### Advanced Simulation with State Management

```python
import pynlm_improved as nlm

# Create simulation components
brain = nlm.PyBrain()
world = nlm.PySimpleWorld()
agent = nlm.PyAgentBrain(brain)

# Configure and initialize
world.configure(width=100.0, height=100.0)
brain.initialize()
agent.initialize(world)
world.setAgentStart(20.0, 20.0)

# Use context manager for automatic cleanup
with brain.simulation():
    for step in range(500):
        # Get current state
        percept = world.getSensoryPerceptDict()
        agent.processSensoryInput(percept)
        
        # Get action
        action = agent.getMotorCommandString()
        
        # Apply action in world
        world.update(0.1)
        
        # Get reward (using agent's learned preference)
        reward = agent.getLearnedPreference()
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: {action} "
                  f"neurons={brain.getNeuronCount()} "
                  f"reward={reward:.3f}")
```

## Configuration Management

### Creating Configurations

```python
import pynlm_improved as nlm

# Method 1: Create simple configuration with defaults
config = nlm.create_simple_config(
    neuron_count=200,
    region_count=3,
    connection_prob=0.15
)

# Method 2: Create from Python dict
config_dict = {
    "neuron_count": 150,
    "region_count": 2,
    "connection_probability": 0.2,
    "learning_rate": 0.01,
    "plasticity_enabled": True,
    "development_enabled": True,
    "curiosity_enabled": True,
    "reward_modulation_enabled": True
}

config = nlm.create_config_from_dict(config_dict)

# Method 3: Load from JSON file
config.loadFromFile("my_config.json")

# Method 4: Load from arguments
config.loadFromArgs(["--neuron_count", "100", "--connection_probability", "0.1"])
```

### Configuration Operations

```python
# Get and set values
print(config.get("neuron_count"))  # Get as Python object
print(config.get_or("max_energy", 100.0))  # Get with default

config.set("learning_rate", 0.05)  # Type detection
config.set("plasticity_enabled", True)  # Bool
config.set("description", "My experiment")  # String

# Convenience methods
config.setNeuronCount(500)
config.setRegionCount(5)
config.setConnectionProbability(0.05)

# List all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Get configuration summary
print(config.summary())
```

## Brain Operations

### Basic Brain Control

```python
import pynlm_improved as nlm

brain = nlm.PyBrain()
brain.initialize()

# Single step simulation
brain.step(0)  # Step 0
brain.step(1)  # Step 1
brain.stepWithTime(2, 0.5)  # Step 2 at time 0.5

# Inject sensory current into all sensory neurons
brain.injectSensoryCurrent(5.0)

# Inject current into specific neuron type
brain.injectCurrent(nlm.NeuronType.Excitatory, 2.0)
```

### Statistics and Analysis

```python
# Get brain statistics
print(f"Neuron count: {brain.getNeuronCount()}")
print(f"Synapse count: {brain.getSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Get region information
regions = brain.getAllRegionInfo()
for i, region in enumerate(regions):
    print(f"Region {i}: id={region['id']}, "
          f"neurons={region['neuron_count']}, "
          f"synapses={region['synapse_count']}")

# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Set developmental stage
brain.setDevelopmentalStage("Adult")
```

### Action Production

```python
# Get action as Python dict (more useful than raw C++ Action object)
action_dict = brain.produceActionDict()
print(f"Action: {action_dict['name']}")
print(f"Parameters: {action_dict['parameters']}")
print(f"Type: {action_dict['type']}")
```

## World Operations

### World Configuration and Control

```python
import pynlm_improved as nlm

world = nlm.PySimpleWorld()

# Configure world
world.configure(width=100.0, height=100.0, visionWidth=32, visionHeight=32)

# Set random seed for reproducibility
world.setRandomSeed(42)

# Get world dimensions
dim = world.getDimensions()
print(f"World: {dim['width']}x{dim['height']} "
      f"vision: {dim['vision_width']}x{dim['vision_height']}")

# Reset world
world.reset()

# Add objects to world
world.addResource(10.0, 10.0, 20.0)  # Resource at (10, 10) with value 20
world.addHazard(50.0, 50.0, -15.0)   # Hazard at (50, 50) with value -15

# Set agent starting position
world.setAgentStart(5.0, 5.0)
```

### World State and Percepts

```python
# Get world state
world.update(0.1)  # Step world by 0.1 time units

# Get sensory percept as Python dict (easy to work with)
percept = world.getSensoryPerceptDict()

print(f"Vision: {len(percept['vision'])} rays")
print(f"Vision shape: {int(percept['vision_height'])}x{int(percept['vision_width'])}")

print(f"Touch sensors: {percept['touch']}")
print(f"Internal signals: {percept['internal']}")

# Get agent body state
agent_body = world.getAgentBodyDict()
print(f"Agent position: ({agent_body['x']:.1f}, {agent_body['y']:.1f})")
print(f"Agent energy: {agent_body['energy']:.1f}")
print(f"Agent health: {agent_body['health']:.1f}")
print(f"Agent age: {agent_body['age']:.1f}")
```

### World-Object Operations

```python
# Check if position is valid
if world.isValidPosition(10.0, 10.0):
    print("Position (10, 10) is valid")
else:
    print("Position (10, 10) is invalid")

# Calculate distance
distance = nlm.calculate_distance(0.0, 0.0, 10.0, 10.0)
print(f"Distance from (0,0) to (10,10): {distance:.2f}")
```

## AgentBrain Operations

### Agent Configuration and Control

```python
import pynlm_improved as nlm

brain = nlm.PyBrain()
world = nlm.PySimpleWorld()

agent = nlm.PyAgentBrain(brain)
agent.initialize(world)

# Enable/disable systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(False)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

### Sensory Processing and Actions

```python
# Process sensory input
percept = world.getSensoryPerceptDict()
agent.processSensoryInput(percept)

# Get motor command as string (human-readable)
action_str = agent.getMotorCommandString()
print(f"Agent action: {action_str}")

# Get motor command as enum value (for C++ compatibility)
action_code = agent.getMotorCommand()
print(f"Agent action code: {action_code}")
```

### Neuromodulation and Learning

```python
# Get neuromodulation levels
neuromod = agent.getNeuromodulationLevels()
print(f"Dopamine: {neuromod['dopamine']:.3f}")
print(f"Curiosity: {neuromod['curiosity']:.3f}")
print(f"Novelty: {neuromod['novelty']:.3f}")
print(f"Prediction error: {neuromod['prediction_error']:.3f}")

# Apply reward for learning
agent.applyReward(10.0, 8.0)  # reward, predicted reward
```

### Agent Diagnostics

```python
# Get diagnostic information
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
print(f"Thought trace: {agent.getThoughtTrace()}")
print(f"Learned preference: {agent.getLearnedPreference():.3f}")

# Check which systems are enabled
print(f"Reward modulation: {'enabled' if agent.isRewardModulationEnabled() else 'disabled'}")
print(f"Structural plasticity: {'enabled' if agent.isStructuralPlasticityEnabled() else 'disabled'}")
print(f"Development: {'enabled' if agent.isDevelopmentEnabled() else 'disabled'}")
print(f"Curiosity: {'enabled' if agent.isCuriosityEnabled() else 'disabled'}")
```

## Helper Functions

### Simulation Utilities

```python
import pynlm_improved as nlm

# Create a simple simulation with all components
brain, world, agent = nlm.create_simple_simulation()

# Run a complete simulation
results = nlm.run_simple_simulation(
    steps=1000,
    timestep=0.1,
    verbose=True
)

print(f"Simulation completed in {results['steps']} steps")
print(f"Final neurons: {results['final_neurons']}")
print(f"Final synapses: {results['final_synapses']}")
```

### Configuration Utilities

```python
# Create configuration from Python dict
config_dict = {"neuron_count": 100, "learning_rate": 0.01}
config = nlm.create_config_from_dict(config_dict)

# Create configuration with sensible defaults
config = nlm.create_simple_config(
    neuronCount=500,
    regionCount=3,
    connectionProb=0.1
)
```

### Data Processing Utilities

```python
# Convert vision to 2D array for visualization/analysis
vision_grid = nlm.vision_to_2d(percept['vision'], 32, 32)

# Calculate distances
agent_x = world.getAgentBodyDict()['x']
agent_y = world.getAgentBodyDict()['y']
target_x, target_y = 50.0, 50.0
distance = nlm.calculate_distance(agent_x, agent_y, target_x, target_y)

# Normalize values
normalized_value = nlm.normalize(5.0, 0.0, 10.0)  # 5.0 -> 0.5

# Clamp values
clamped_value = nlm.clamp(15.0, 0.0, 10.0)  # 15.0 -> 10.0
```

## Experiment Utilities

### Running and Managing Experiments

```python
import pynlm_improved as nlm

# Run an experiment
experiment_results = nlm.run_simple_simulation(
    steps=2000,
    timestep=0.05,
    verbose=False
)

# Save experiment state
brain = nlm.PyBrain()
world = nlm.PySimpleWorld()
agent = nlm.PyAgentBrain(brain)

# ... run experiment ...

nlm.save_simulation_state(
    "experiment_1",
    brain,
    world,
    agent
)

# Load experiment state
loaded_brain, loaded_world, loaded_agent = nlm.load_simulation_state("experiment_1")
```

## Visualization and Analysis

### Processing Vision Data

```python
# Get vision data
percept = world.getSensoryPerceptDict()
vision = percept['vision']
vision_height = percept['vision_height']
vision_width = percept['vision_width']

# Convert to 2D grid for analysis or visualization
vision_grid = nlm.vision_to_2d(vision, vision_width, vision_height)

# Analyze vision patterns
bright_spots = sum(1 for row in vision_grid for pixel in row if pixel > 0.5)
print(f"Number of bright spots in vision: {bright_spots}")
```

## Error Handling

The improved bindings provide Python-style error handling:

```python
import pynlm_improved as nlm

try:
    # This might raise RuntimeError
    config = nlm.create_simple_config(neuronCount=-1)
except nlm.RuntimeError as e:
    print(f"Error: {e}")
```

## Version Information

```python
import pynlm_improved as nlm
print(f"NLM Improved Bindings Version: {nlm.VERSION}")
```

## Migration Guide

### From Original to Improved Bindings

**Original (pynlm):**
- Direct C++ class exposure
- Less Pythonic API
- Limited helper functions
- No context managers

**Improved (pynlm_improved):**
- Pythonic wrapper classes
- Convenience methods
- Rich helper functions
- Context managers for resource management
- Better error handling
- Experimental utilities

### Key Changes

| Feature | Original | Improved |
|---------|----------|----------|
| Config | Raw C++ class | Pythonic PyConfig with type detection |
| Brain | Raw C++ class | PyBrain with convenience methods |
| World | Raw C++ class | PySimpleWorld with dict-based percepts |
| AgentBrain | Raw C++ class | PyAgentBrain with learning utilities |
| Error handling | C++ exceptions | Python-style exceptions |
| Context management | None | Automatic via context managers |
| Helpers | None | Rich utility functions |
