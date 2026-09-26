# NLM (Neural Learning Machine) - Advanced Python Usage Guide

This guide covers advanced Python functionality for the NLM brain simulation framework, building on the easy usage guide with additional features and examples.

## Advanced Features Overview

The enhanced Python bindings provide:

### 1. **Comprehensive Brain Statistics**
Access detailed brain metrics through dictionaries for easy data analysis and visualization.

### 2. **Simulation Management Utilities**
Create, run, and manage complex simulation episodes programmatically.

### 3. **Configuration Management**
Advanced configuration creation and validation with parameter checking.

### 4. **Data Persistence**
Import and export brain states for saving experiments and resuming simulations.

### 5. **Batch Simulation Capabilities**
Run multiple simulation episodes and collect aggregated results.

### 6. **Pythonic Property Access**
Intuitive property-based access to brain and agent attributes.

## Quick Start Examples

### Basic Simulation with Statistics

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation and collect stats
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        stats = pynlm.getBrainStats(brain)
        print(f"Step {step}: {stats['firing_neurons']} firing neurons")
        print(f"Average firing rate: {stats['average_firing_rate']:.2f}")
```

### Complete Agent Simulation

```python
import pynlm

# Setup advanced configuration
config = pynlm.createDemoConfig()

# Create components
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation episode
num_steps = 500
print(f"Running {num_steps}-step simulation...")

pynlm.runSimulationEpisode(brain, agent, world, num_steps)

# Get final statistics
final_stats = pynlm.getBrainStats(brain)
print("\nFinal Statistics:")
for key, value in final_stats.items():
    print(f"  {key}: {value}")
```

### Configuration Validation

```python
import pynlm

# Create custom configuration
config = pynlm.create_custom_config(
    neuron_count=800,
    synapse_density=0.12,
    enable_stdp=True,
    enable_hebbian=True,
    enable_curiosity=True
)

# Validate configuration
is_valid, message = pynlm.validateConfig(config)
if is_valid:
    print("Configuration is valid!")
else:
    print(f"Configuration errors: {message}")
```

### Data Persistence

```python
import pynlm

# Create and run a simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(200):
    brain.step(i)

# Save brain state
print("Saving brain state to 'my_brain.bin'...")
pynlm.export_brain_data(brain, 'my_brain.bin')
print("Saved successfully!")

# Later, import the saved brain
print("Loading brain state from 'my_brain.bin'...")
loaded_brain = pynlm.import_brain_data('my_brain.bin')
if loaded_brain:
    print(f"Successfully loaded brain with {loaded_brain.total_neurons} neurons")
```

### Batch Simulation

```python
import pynlm

# Create configuration for batch simulation
config = pynlm.createDemoConfig()

# Run batch simulation
print("Running batch simulation...")
results = pynlm.run_batch_simulation(
    config=config,
    num_episodes=10,
    steps_per_episode=100
)

print("\nBatch Simulation Results:")
for i, episode_result in enumerate(results):
    print(f"Episode {i}:")
    for key, value in episode_result.items():
        print(f"  {key}: {value}")
    print()
```

## Advanced Usage Patterns

### Simulation with Custom Configuration

```python
import pynlm

# Create custom configuration using Python dict-style access
config = pynlm.create_custom_config(
    neuron_count=1200,
    synapse_density=0.15,
    enable_stdp=True,
    enable_hebbian=False,
    enable_curiosity=True
)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=12, visionHeight=12)

agent.initialize(world)

# Custom simulation loop with statistics collection
episode_data = []
for step in range(300):
    world.update(0.1)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect statistics periodically
    if step % 50 == 0:
        stats = pynlm.getBrainStats(brain)
        episode_data.append({
            'step': step,
            'firing_neurons': stats['firing_neurons'],
            'spike_count': stats['spike_count'],
            'avg_rate': stats['average_firing_rate'],
            'novelty': agent.novelty_level,
            'curiosity': agent.curiosity_level
        })

print(f"Simulation collected {len(episode_data)} data points")
```

### Machine Learning Experiment Setup

```python
import pynlm
import json

# Setup a machine learning experiment configuration
experiment_config = {
    "brain_parameters": {
        "neuron_count": 2000,
        "synapse_density": 0.2,
        "connection_probability": 0.08
    },
    "learning_parameters": {
        "enable_stdp": True,
        "enable_hebbian": True,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
        "enable_development": True
    },
    "simulation_parameters": {
        "num_steps": 1000,
        "world_size": [50, 50],
        "vision_size": [16, 16]
    }
}

# Create configuration programmatically
config = pynlm.create_custom_config(
    neuron_count=experiment_config["brain_parameters"]["neuron_count"],
    synapse_density=experiment_config["brain_parameters"]["synapse_density"],
    enable_stdp=experiment_config["learning_parameters"]["enable_stdp"],
    enable_hebbian=experiment_config["learning_parameters"]["enable_hebbian"],
    enable_curiosity=experiment_config["learning_parameters"]["enable_curiosity"]
)

# Save experiment configuration
with open('experiment_config.json', 'w') as f:
    json.dump(experiment_config, f, indent=2)

print("Experiment configuration saved to 'experiment_config.json'")

# Create and run the experiment
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(
    width=experiment_config["simulation_parameters"]["world_size"][0],
    height=experiment_config["simulation_parameters"]["world_size"][1],
    visionWidth=experiment_config["simulation_parameters"]["vision_size"][0],
    visionHeight=experiment_config["simulation_parameters"]["vision_size"][1]
)

agent.initialize(world)

# Run experiment
for step in range(experiment_config["simulation_parameters"]["num_steps"]):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        stats = pynlm.getBrainStats(brain)
        print(f"Step {step}: {stats['firing_neurons']} firing neurons")

# Export results
final_stats = pynlm.getBrainStats(brain)
with open('experiment_results.json', 'w') as f:
    json.dump({
        'final_stats': final_stats,
        'experiment_config': experiment_config
    }, f, indent=2)

print("Experiment completed and results saved to 'experiment_results.json'")
```

## Property Access Examples

The enhanced bindings provide intuitive property access:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access statistics using property-like access (via __getitem__)
print(f"Neuron count: {brain['neuron_count']}")
print(f"Synapse count: {brain['synapse_count']}")
print(f"Active neurons: {brain['active_neurons']}")
print(f"Firing neurons: {brain['firing_neurons']}")

# For agents, use property access (some are implemented as properties)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Access agent attributes
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")
print(f"Development stage: {agent.getDevelopmentalStage()}")
```

## Example Code Collection

```python
def get_example_code():
    return """
    # Example 1: Simple Brain
    import pynlm
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    print(f"Simulation complete: {brain.total_neurons} neurons")
    
    # Example 2: Complete Agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    agent.initialize(world)
    
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    """

# Get example code
example = pynlm.get_example_code()
print(example)
```

## Error Handling and Validation

The enhanced bindings include comprehensive error handling:

```python
import pynlm

# Configuration validation
config = pynlm.create_custom_config(neuron_count=50)  # Too small
try:
    is_valid, message = pynlm.validateConfig(config)
    if not is_valid:
        print(f"Configuration validation failed: {message}")
except Exception as e:
    print(f"Error: {e}")

# File operations
try:
    pynlm.export_brain_data(brain, 'path/to/file.bin')
    print("Export successful")
except Exception as e:
    print(f"Export failed: {e}")

try:
    loaded_brain = pynlm.import_brain_data('nonexistent_file.bin')
    if loaded_brain:
        print("Import successful")
    else:
        print("Import failed: file not found or corrupted")
except Exception as e:
    print(f"Import error: {e}")
```

## Performance Monitoring

Track simulation performance with built-in monitoring:

```python
import time
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Time simulation
start_time = time.time()
for i in range(1000):
    brain.step(i)
end_time = time.time()

elapsed = end_time - start_time
print(f"Simulated 1000 steps in {elapsed:.2f} seconds")
print(f"Average step time: {elapsed/1000*1000:.2f} ms")

# Get performance statistics
stats = pynlm.getBrainStats(brain)
print(f"Performance: {stats['total_neurons']} neurons, "
      f"{stats['average_firing_rate']:.2f} avg firing rate")
```

## Advanced Features Summary

The enhanced NLM Python bindings provide:

1. **Comprehensive Statistics**: Dictionary-based access to brain metrics
2. **Batch Simulation**: Run multiple episodes and collect aggregated results
3. **Configuration Validation**: Automatic parameter checking and error reporting
4. **Data Persistence**: Import/export brain states for experiment reproducibility
5. **Custom Configuration**: Programmatic creation of custom simulation parameters
6. **Example Code**: Ready-to-use code examples for common patterns
7. **Error Handling**: Comprehensive error handling and validation
8. **Performance Monitoring**: Built-in performance tracking and reporting
9. **Property Access**: Intuitive property-style access to brain and agent attributes
10. **Machine Learning Integration**: Support for structured experiment configurations

These enhancements make NLM more accessible to Python users while maintaining full access to the powerful C++ neural simulation engine.
