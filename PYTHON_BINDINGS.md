# Python Bindings Documentation

## Overview

This document provides comprehensive documentation for the NLM Python bindings (`pynlm`), which expose the powerful NLM (Neural Learning Machine) simulation framework to Python users. The Python bindings enable access to all NLM functionality while maintaining the performance and flexibility of the C++ core.

## Installation

### Prerequisites

- Python 3.8 or later
- pip
- Build tools (if installing from source)

### Installation Options

#### 1. Using pip (Recommended)

```bash
pip install pynlm
```

#### 2. Installing from Source

```bash
# Clone the repository
cd /path/to/nlm

# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

#### 3. Building with scikit-build-core

```bash
# Using pyproject.toml (modern Python packaging)
pip install build
python -m build
pip install dist/*.whl
```

## Basic Usage

### Creating a Simple Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getTotalSynapseCount()} synapses")
```

### Running a Simulation

```python
import pynlm
import time

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Working with Worlds and Agents

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent with world
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent simulation
for step in range(500):
    # Update world
    world.update(timestep=0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Get reward
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
```

## Core Classes

### Brain Class

#### Creation and Initialization

```python
# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Basic Operations

```python
# Simulation control
brain.step(step_number)                    # Run one simulation step
brain.step(step_number, time)             # Run step with timestamp
brain.reset()                              # Reset brain state
brain.save("checkpoint.ncl")               # Save brain state
brain.load("checkpoint.ncl")               # Load brain state

# Input/output
brain.receiveSensoryInput(sensory_input)   # Inject sensory data
action = brain.produceAction()            # Get motor action

# Statistics
brain.getTotalNeuronCount()                # Total neurons
brain.getTotalSynapseCount()               # Total synapses
brain.getFiringNeuronCount()               # Currently firing
brain.getAverageFiringRate()               # Average firing rate
brain.getExcitationInhibitionRatio()      # E/I balance
brain.getTotalSpikeCount()                 # Total spikes
brain.getFiringNeuronCount()               # Currently firing
brain.getDevelopmentalStage()             # Current developmental stage

# Regions
region_id = brain.addRegion("cortex")     # Add a new brain region
region = brain.getRegion(region_id)        # Get a specific region
```

#### Configuration Access

```python
# Query configuration
config = pynlm.createDefaultConfig()
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 1000)

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### Config Class

#### Creating Configurations

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create empty configuration
config = pynlm.Config()
```

#### Configuration Management

```python
# Set values with different types
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("simulation.timestep", 0.001)
config.set("plasticity.enabled", True)

# Get values
neuron_count = config.getOr("brain.neuron_count", 1000)
synapse_density = config.getOr("brain.synapse_density", 0.1)

# Check existence
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count').value}")

# Get all keys
keys = config.getKeys()

# Clear configuration
config.clear()
```

### AgentBrain Class

#### Creation and Initialization

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Sensory and Motor Processing

```python
# Process sensory input
agent.processSensoryInput(percept)

# Decode motor command
motor_cmd = agent.decodeMotorCommand()

# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(timestep)
```

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Get subsystem statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### SimpleWorld Class

#### World Creation and Configuration

```python
# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

#### World Control

```python
# Control methods
world.reset()                               # Reset to initial state
world.update(timestep)                      # Update world state
world.setRandomSeed(42)                     # Set random seed
world.setAgentStart(x, y)                   # Set agent starting position
world.applyMotorCommand(motor_cmd, time)    # Apply motor action

# State access
percept = world.getSensoryPercept()         # Get sensory input
body = world.getAgentBody()                 # Get agent body info

# World object management
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)
```

#### World Properties

```python
# Properties and settings
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.getAgentPosition()
world.getAgentOrientation()

# Configure properties
world.setMaxEnergy(100.0)
world.setBoundaryType(pynlm.BoundaryType.Reflect)
world.setResourceSpawnRate(0.1)
```

## Enumerations

### Neuron Types

```python
pynlm.NeuronType.Excitatory     # Excitatory neurons
pynlm.NeuronType.Inhibitory     # Inhibitory neurons  
pynlm.NeuronType.Sensory        # Sensory neurons
pynlm.NeuronType.Motor          # Motor neurons
pynlm.NeuronType.Modulatory     # Modulatory neurons
pynlm.NeuronType.Internal      # Internal neurons
```

### Developmental Stages

```python
pynlm.DevelopmentalStage.Initial       # Initial development
pynlm.DevelopmentalStage.CriticalPeriod  # Critical period
pynlm.DevelopmentalStage.Maturation     # Maturation phase
pynlm.DevelopmentalStage.Adult         # Adult stage
pynlm.DevelopmentalStage.Aging         # Aging stage
```

### Action Types

```python
pynlm.ActionType.MoveForward    # Move forward
pynlm.ActionType.TurnLeft       # Turn left
pynlm.ActionType.TurnRight      # Turn right
pynlm.ActionType.Interact       # Interact with environment
pynlm.ActionType.Eat            # Consume resource
pynlm.ActionType.Sleep          # Rest/consolidate memories
pynlm.ActionTypeExplore         # Explore new areas
```

### World Object Types

```python
pynlm.WorldObjectType.Empty      # Empty space
pynlm.WorldObjectType.Resource   # Resource for consumption
pynlm.WorldObjectType.Hazard     # Hazardous object
pynlm.WorldObjectType.Wall       # Wall/obstacle
pynlm.WorldObjectType.Marker     # Special marker location
```

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

## Advanced Usage

### Working with Checkpoints

```python
import pynlm
import os

# Save and load checkpoints
brain.save("brain_checkpoint.ncl")

# Load checkpoint
if os.path.exists("brain_checkpoint.ncl"):
    brain.load("brain_checkpoint.ncl")
    print("Brain state restored from checkpoint")
```

### Performance Monitoring

```python
import pynlm
import time

# Performance monitoring
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)

brain = pynlm.createBrain(config)
brain.initialize()

# Time the simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

elapsed = end_time - start_time
steps_per_second = 1000 / elapsed
spikes_per_second = brain.getTotalSpikeCount() / elapsed

print(f"Simulation speed: {steps_per_second:.0f} steps/sec")
print(f"Spike rate: {spikes_per_second:.0f} spikes/sec")
```

### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.Config()

# Add custom parameters
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.08)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.curiosity.threshold", 0.3)

# Save custom configuration
config.saveToFile("custom_config.json")

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Error Handling

```python
import pynlm
import logging

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def safe_simulation_run(config_file, output_file):
    """Run simulation with comprehensive error handling"""
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        if not config.loadFromFile(config_file):
            raise ValueError(f"Failed to load configuration from {config_file}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        if not brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
            
            # Check for errors
            if brain.getFiringNeuronCount() == 0 and step > 100:
                logger.warning(f"No neurons firing at step {step}")
        
        # Save results
        if not brain.save(output_file):
            raise RuntimeError(f"Failed to save brain to {output_file}")
            
        logger.info(f"Simulation completed successfully")
        return True
        
    except Exception as e:
        logger.error(f"Simulation failed: {e}")
        return False

# Usage
if __name__ == "__main__":
    success = safe_simulation_run("config.json", "brain_checkpoint.ncl")
    if success:
        print("Simulation completed successfully")
    else:
        print("Simulation failed")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: "ImportError: No module named 'pynlm'"

**Solution:**
```bash
pip install --force-reinstall .
pip install pynlm
```

#### Issue: "Segmentation fault"

**Solution:**
```python
# Ensure proper initialization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # Always call initialize()

# Use reset before re-running
simulation_steps = 100
for step in range(simulation_steps):
    brain.step(step)
    
# Clean reset and re-initialize
brain.reset()
brain.initialize()
```

#### Issue: "No neurons firing"

**Solution:**
```python
# Ensure brain has proper configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Ensure enough neurons
config.set("brain.synapse_density", 0.1)  # Ensure connectivity

brain = pynlm.createBrain(config)
brain.initialize()

# Add some initial input
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
```

#### Issue: "Checkpoint file not found"

**Solution:**
```python
import os

checkpoint_file = "brain_checkpoint.ncl"
if os.path.exists(checkpoint_file):
    brain.load(checkpoint_file)
else:
    print("Checkpoint file not found. Creating new brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
```

### Getting Help

- **Documentation:** See `docs/ARCHITECTURE.md` for detailed architecture information
- **API Reference:** Check the Python module documentation
- **Examples:** Look in the `examples/` directory for complete working examples
- **Community:** Join the NLM discussion forums for support

## Migration Guide

### From Version 1.x to 2.x

#### Configuration Changes

- **New JSON/YAML support:** Configuration files now support JSON and YAML formats
- **Enhanced validation:** Added configuration validation
- **New parameters:** Additional configuration options for advanced users

#### API Changes

- **Consistent naming:** Standardized method names across all classes
- **Enhanced error handling:** New exception types for better error reporting
- **Improved performance:** Optimized many methods for better performance

#### Breaking Changes

- **Method signatures:** Some method signatures have changed
- **Configuration keys:** Some configuration keys have been renamed
- **Exception types:** New exception hierarchy

### Migration Steps

1. Update configuration files to JSON/YAML format
2. Update import statements and API calls
3. Add error handling for new exception types
4. Test thoroughly with new configuration options

## Advanced Examples

### Example 1: Learning Agent

```python
import pynlm
import numpy as np

def create_learning_agent(config_file, world_size=20):
    """Create an agent that learns through interaction"""
    # Load configuration
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return brain, agent, world

def train_agent(agent, world, num_episodes=100):
    """Train agent through interaction"""
    for episode in range(num_episodes):
        # Reset environment
        world.reset()
        
        # Run episode
        for step in range(200):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process in agent
            agent.processSensoryInput(percept)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            
            # Learn from experience
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Print progress
        if episode % 20 == 0:
            print(f"Episode {episode}: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")

# Usage
brain, agent, world = create_learning_agent("learning_config.json")
train_agent(agent, world, num_episodes=100)
```

### Example 2: Brain Analysis

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

def analyze_brain(brain_file):
    """Analyze brain checkpoint and generate statistics"""
    # Load brain
    brain = pynlm.Brain()
    brain.load(brain_file)
    
    # Collect statistics
    stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'developmental_stage': brain.getDevelopmentalStage()
    }
    
    # Generate plots
    generate_activity_plot(brain)
    generate_connectivity_plot(brain)
    
    return stats

def generate_activity_plot(brain):
    """Generate neural activity visualization"""
    # Mock implementation
    plt.figure(figsize=(12, 8))
    
    # Simulate activity pattern
    time_steps = 1000
    activity = np.random.poisson(brain.getTotalSpikeCount() / time_steps, time_steps)
    
    plt.plot(range(time_steps), activity)
    plt.xlabel('Time Step')
    plt.ylabel('Spike Count')
    plt.title('Neural Activity Pattern')
    plt.grid(True, alpha=0.3)
    plt.savefig('activity_plot.png')
    plt.close()

def generate_connectivity_plot(brain):
    """Generate connectivity visualization"""
    # Mock implementation
    plt.figure(figsize=(10, 10))
    
    # Simulate connectivity matrix
    num_neurons = brain.getTotalNeuronCount()
    connectivity = np.random.rand(num_neurons, num_neurons) * 0.1
    
    # Display as heatmap
    plt.imshow(connectivity, cmap='hot', interpolation='nearest')
    plt.colorbar(label='Connection Strength')
    plt.title('Brain Connectivity Matrix')
    plt.savefig('connectivity_plot.png')
    plt.close()

# Usage
if __name__ == "__main__":
    stats = analyze_brain("brain_checkpoint.ncl")
    print("Brain Analysis Results:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
```

## Performance Tips

### 1. Optimize Configuration

```python
# Use appropriate neuron counts for your use case
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Smaller for faster testing
config.set("brain.synapse_density", 0.05)  # Lower for faster startup
```

### 2. Use Batched Operations

```python
# For multiple simulation steps, consider batch processing
for step in range(0, 1000, 100):
    # Process multiple steps
    for substep in range(100):
        brain.step(step + substep)
    
    # Log statistics
    print(f"Progress: {step}/1000 steps")
```

### 3. Memory Management

```python
# Monitor memory usage
import tracemalloc
tracemalloc.start()

# Run simulation
brain = pynlm.createBrain(config)
brain.initialize()

# Take memory snapshot
snapshot = tracemalloc.take_snapshot()
top_stats = snapshot.statistics('lineno')

# Print top memory usage
for stat in top_stats[:10]:
    print(stat)
```

### 4. Parallel Processing

```python
# For large simulations, consider parallel processing
import multiprocessing

def simulate_batch(steps):
    """Simulate a batch of steps"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return brain.getTotalSpikeCount()

# Run in parallel
with multiprocessing.Pool() as pool:
    results = pool.map(simulate_batch, [100, 100, 100, 100, 100])
    
print(f"Results from parallel simulation: {results}")
```

## Debugging Tips

### 1. Enable Debug Logging

```python
import logging

# Enable debug logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

logger = logging.getLogger(__name__)

# Add debug logging to your code
logger.debug("Creating brain")
logger.debug(f"Configuration: {config}")
```

### 2. Validate Inputs

```python
def validate_brain_parameters(config):
    """Validate brain configuration parameters"""
    errors = []
    
    # Check neuron count
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 10 or neuron_count > 100000:
        errors.append(f"Invalid neuron count: {neuron_count}")
    
    # Check synapse density
    synapse_density = config.getOr("brain.synapse_density", 0.1)
    if synapse_density < 0.0 or synapse_density > 1.0:
        errors.append(f"Invalid synapse density: {synapse_density}")
    
    if errors:
        raise ValueError(f"Configuration validation failed:\n" + "\n".join(errors))
    
    return True
```

### 3. Use Checkpoints for Debugging

```python
import pynlm

def debug_simulation_step_by_step(config_file, output_dir):
    """Run simulation step by step for debugging"""
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation and save checkpoints
    for step in range(100):
        brain.step(step)
        
        # Save checkpoint every 10 steps
        if step % 10 == 0:
            checkpoint_file = f"{output_dir}/checkpoint_step_{step}.ncl"
            brain.save(checkpoint_file)
            print(f"Saved checkpoint: {checkpoint_file}")
    
    return brain

# Usage
brain = debug_simulation_step_by_step("config.json", "debug_checkpoints")
```

## Frequently Asked Questions

### Q: What Python versions are supported?

A: NLM Python bindings support Python 3.8 and later. Python 3.10+ is recommended for best performance.

### Q: How do I handle large brain files?

A: For large brain files, consider:
- Using checkpoint files for incremental loading
- Implementing parallel processing
- Using memory-mapped files for very large simulations
- Processing data in chunks

### Q: What are the memory requirements?

A: Memory requirements depend on the brain size:
- Small brain (1000 neurons): ~50-100 MB
- Medium brain (10,000 neurons): ~500-1 GB
- Large brain (100,000 neurons): ~5-10 GB

### Q: Can I use NLM with external libraries?

A: Yes, NLM can be used with:
- NumPy for numerical operations
- Pandas for data analysis
- Matplotlib for visualization
- SciPy for scientific computing

### Q: How do I get help with NLM?

A: For help with NLM:
1. Check the documentation in the `docs/` directory
2. Join the NLM discussion forums
3. Report issues on GitHub
4. Check the examples in the `examples/` directory

## Conclusion

The NLM Python bindings provide a powerful interface to the NLM neural simulation framework. With comprehensive documentation, examples, and debugging tools, users can effectively develop and run neural simulations for research and applications.

The Python bindings make NLM accessible to a wide range of users, from researchers and developers to students and enthusiasts, while maintaining the performance and flexibility of the underlying C++ implementation.