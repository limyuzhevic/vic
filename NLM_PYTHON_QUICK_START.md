# NLM Python Quick Start Guide

Welcome to NLM (熙然) - an experimental artificial developmental brain project! This guide will help you get started with using NLM through Python.

## What is NLM?

NLM is a computational brain simulation framework that implements neural learning through:
- Real spiking neuron dynamics (LIF neurons)
- Memory systems (working memory, episodic memory)
- Plasticity rules (STDP, Hebbian learning)
- Neuromodulation (dopamine, curiosity, novelty detection)
- Prediction systems
- Cognitive development

## Quick Start: Creating Your First Brain

### Method 1: Using the Simple Convenience Functions

The easiest way to get started is to use our high-level convenience functions:

```python
import nlm

# 1. Create a default configuration with sensible parameters
config = nlm.create_default_config()
config.set("neuron_count", 1000)    # Number of neurons
config.set("region_count", 2)       # Number of brain regions
config.set("connection_probability", 0.1)  # Connect probability

# 2. Create and initialize a brain
brain = nlm.create_default_brain(config)
print("Brain created successfully!")

# 3. Run a learning experiment
world = nlm.create_simple_world()
world.configure(100, 100, 20, 20)  # 100x100 world, 20x20 vision
world.reset()
world.setAgentStart(50, 50)

# Run simulation for 500 steps
experiment_result = nlm.run_simple_learning_experiment(brain, world, 500)
print(experiment_result)

# 4. Visualize the brain state
visualization = nlm.visualize_brain_state(brain, max_regions=5)
print(visualization)

# 5. Save for later
brain.save("./my_first_brain.checkpoint")
print("Brain saved!")
```

### Method 2: Complete Example

Here's a complete working example that you can copy and run:

```python
import nlm

# Get a complete example showing all the steps
example_output = nlm.example_basic_usage()
print(example_output)
```

## Understanding the API

### Key Classes

#### 1. Config
Configuration object that controls brain parameters
- Use `nlm.create_default_config()` or `nlm.create_default_config_with_params()` to create
- Set parameters like neuron_count, region_count, connection_probability
- Load/save from files

#### 2. Brain  
The central neural simulation component
- Initialize with configuration
- Receive sensory input and process
- Produce motor actions
- Maintain internal state (memory, development, etc.)

#### 3. SimpleWorld
The environment where the brain interacts
- 2D grid world with agents and objects
- Provides sensory input (vision, touch, internal signals)
- Applies motor commands
- Computes rewards

#### 4. AgentBrain
Bridges the Brain and World
- Processes sensory input and injects into brain
- Decodes motor commands from brain activity
- Applies neuromodulation (reward, curiosity, etc.)
- Manages development

### Key Methods

#### Configuration Methods
```python
# Create config with custom parameters
config = nlm.create_default_config_with_params(
    neuron_count=500,
    region_count=2,
    connection_probability=0.15,
    simulation_timestep=0.001
)

# Set specific parameters
config.set("neuron_count", 1000)
config.set("random_seed", 42)
```

#### Brain Methods
```python
# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Run simulation
brain.step(step, timestamp)

# Process sensory input
percept = world.getSensoryPercept()
brain.receiveSensoryInput(percept)

# Get motor actions
action = brain.produceAction()
```

#### Visualization
```python
# Get simple text visualization
visualization = nlm.visualize_brain_state(brain, max_regions=10)
print(visualization)

# Includes: neurons, synapses, firing rates, regions, statistics
```

## Common Workflows

### 1. Learning and Adaptation
```python
# Run a learning experiment
world = nlm.create_simple_world()
brain = nlm.create_default_brain(config)
world.configure(80, 80, 15, 15)
world.reset()
world.setAgentStart(40, 40)

# Run for many steps to see learning
for step in range(1000):
    # Get observation
    percept = world.getSensoryPercept()
    
    # Process through agent brain
    agent_brain = nlm.createAgentBrain(brain)
    agent_brain.processSensoryInput(percept)
    
    # Step the brain
    brain.step(step, step * 0.01)
    
    # Get action and apply to world
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action.type, step)
    
    # Get reward
    reward = world.computeReward(brain.getRegions()[0].get())
    agent_brain.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent_brain.updateDevelopment(0.01)
```

### 2. Experiment with Different Configurations
```python
# Try different brain sizes and parameters
configs = [
    {"neuron_count": 500, "region_count": 1, "connection_probability": 0.05},
    {"neuron_count": 2000, "region_count": 3, "connection_probability": 0.15},
    {"neuron_count": 5000, "region_count": 5, "connection_probability": 0.1},
]

for cfg_params in configs:
    config = nlm.create_default_config_with_params(**cfg_params)
    brain = nlm.create_default_brain(config)
    
    # Run and compare results
    result = nlm.run_simple_learning_experiment(brain, world, 200)
    print(f"Config {cfg_params}: {result.split('Average firing rate: ')[1][:20]}")
```

### 3. Checkpoint and Resume
```python
# Save brain state
brain.save("experiment_checkpoint.checkpoint")

# Later, load the brain
loaded_brain = nlm.create_default_brain(config)
loaded_brain.load("experiment_checkpoint.checkpoint")
```

## Understanding the Output

The visualization output shows:

```
=== Brain State Visualization ===

Basic Statistics:
  Total neurons: 1000
  Total synapses: 150000
  Active neurons: 12
  Firing neurons: 3
  Total spikes: 45678
  Average firing rate: 0.0457
  Excitation/Inhibition ratio: 1.23
  Developmental stage: 3
  Pending spike events: 5

Region Information (showing 2 of 3 regions):
  Region 0 ("Default"):
    Neurons: 1000, Synapses: 150000
    Active: 12, Firing: 3
    Avg firing rate: 0.0457
    Avg synaptic weight: 0.012
    Synaptic density: 1.5

  Region 1 ("Cognitive"):
    Neurons: 0, Synapses: 0
    Active: 0, Firing: 0
    Avg firing rate: 0.0000
    Avg synaptic weight: 0.0000
    Synaptic density: 0.0

... and 1 more regions
```

## Troubleshooting

### Common Issues

1. **Memory errors with large brains**
```python
# For resource-constrained environments
config = nlm.create_default_config_with_params(
    neuron_count=500,      # Smaller
    region_count=1,        # Fewer regions  
    connection_probability=0.05
)
```

2. **No learning observed**
```python
# Ensure these are enabled
brain.setDevelopmentEnabled(True)
brain.setRewardModulationEnabled(True)
brain.setCuriosityEnabled(True)
brain.setStructuralPlasticityEnabled(True)
```

3. **Python import errors**
```bash
# First, ensure NLM is built
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Then install the Python bindings
pip install -e .
```

### Getting Help

1. **Complete examples**: Use `nlm.example_basic_usage()` to see a complete working example
2. **Documentation**: Check the NLM documentation for detailed API reference
3. **Source code**: The Python bindings in `python/bindings.cpp` contain all the implementation details

## Next Steps

1. **Run the Phase 6 Demo**: Use the C++ demo program to see all systems integrated
2. **Explore the API**: Use `dir(nlm)` in Python to see all available functions
3. **Write your own experiments**: Create custom scenarios and observe the brain learning
4. **Contribute**: Add new convenience functions or examples!

## Project Information

- **Phase**: 6 (Final Integration - All Systems Connected)
- **License**: MIT
- **Documentation**: Available in the NLM documentation
- **Support**: See the main NLM README for technical details

Enjoy experimenting with your artificial brain!
